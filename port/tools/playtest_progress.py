#!/usr/bin/env python3
"""Drives a campaign level's scripts to their end with test hooks, to check
that the level can be finished: a script-directed bot, not a skilled player.

Usage (repository root): /usr/bin/python3 port/tools/playtest_progress.py
           --map 01b_spacestation [--frames N] [--work DIR]

Every 150 frames the game logs each script thread's syntax nodes
(h2-script); for each thread the bot decompiles its innermost sleep_until
(port/tools/hs_decompile.py) and acts on what it waits for:
  - a player inside a trigger volume: teleport into the volume's centre;
  - a structure bsp switch: the player teleport refuses the level's switch
    volumes, so teleport beside the "bsp_FROM,TO" volume and walk through it;
  - living AI counts / strength: kill the enemies (all other-team AI);
  - anything else: wait (cinematics, timers, devices the script runs).
The player is kept deathless (the guest test hooks in
port/runtime/guest_test_state.cpp). Exit status 0 when the level ends (its
scripts are replaced by the next map's), 1 otherwise.
"""
import argparse
import math
import re
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from h2_driver import Game, MENU_TO_CAMPAIGN, ROOT  # noqa: E402
from hs_decompile import Scripts  # noqa: E402

BSP_WAIT = re.compile(r"\(= \(structure_bsp_index\) (\d+)\)")
SEAT_LABEL = re.compile(r"\(vehicle_test_seat \(.*?\) (\w+) \(unit")
OBJECT_NEAR_WAIT = re.compile(r"\(< \(objects_distance_to_object \(players\) (\w+)\)")
ACTOR_NEAR_WAIT = re.compile(r"\(< \(objects_distance_to_object \(players\) \(ai_get_object ai_current_actor\)\)")
UNIT_LINE = re.compile(r"unit (\w+) type \d+ team (-?\d+) body (\S+) at (\S+) (\S+) (\S+) parent (\w+)")
HEALTH_WAIT = re.compile(r"\(<= \(object_get_health (\w+)\) 0\)")
DEVICE_WAIT = re.compile(r"\(device_(?:get_position|group_get) (\w+)\)")
VOLUME_TEST = re.compile(r"\((volume_test_objects(?:_all)?|volume_test_object) (\S+) \((players|player0|player1)\)\)")


def trigger_volumes(scripts):
    """name -> (corner, forward, left, up, extents) of the scenario's trigger
    volumes (block at +0x108, 0x44-byte records: name id, object name +4,
    forward +0xc, up +0x18, corner +0x24, extents +0x30), and the names of
    those on an object (their box is in the object's node's space)"""
    m = scripts.map
    scenario = next(t for t in m.tags if t[0] == 'scnr')
    base = m.file_offset(scenario[2])
    count, address = struct.unpack_from('<II', m.data, base + 0x108)
    volumes, attached = {}, set()
    for i in range(count):
        o = m.file_offset(address) + i * 0x44
        name = m.string_id(struct.unpack_from('<I', m.data, o)[0])
        fwd = struct.unpack_from('<3f', m.data, o + 0xc)
        up = struct.unpack_from('<3f', m.data, o + 0x18)
        corner = struct.unpack_from('<3f', m.data, o + 0x24)
        ext = struct.unpack_from('<3f', m.data, o + 0x30)
        left = (up[1] * fwd[2] - up[2] * fwd[1], up[2] * fwd[0] - up[0] * fwd[2], up[0] * fwd[1] - up[1] * fwd[0])
        volumes[name] = (corner, fwd, left, up, ext)
        if struct.unpack_from('<h', m.data, o + 4)[0] != -1:
            attached.add(name)
    return volumes, attached


def live_volume(g, volumes, name):
    """a volume on an object where the object is now (the game's own matrix,
    the volume test hook), or None while the object is absent"""
    index = list(volumes).index(name)
    replies = len(g.test_replies)
    g.guest("volume %d" % index)
    g.wait(2)
    answer = next((r for r in g.test_replies[replies:] if r.startswith("volume %d " % index)), None)
    if not answer or answer.endswith("none"):
        return None
    v = [float(x) for x in answer.split()[2:]]
    scale = v[12]
    ext = tuple(e * scale for e in volumes[name][4])
    return tuple(v[0:3]), tuple(v[3:6]), tuple(v[6:9]), tuple(v[9:12]), ext


def string_id_values(m):
    """name -> string id ((length << 24) | index) of the map's string ids
    (count at header +0x164)"""
    ids = {}
    for index in range(m.u32(0x164)):
        name = m.string_id(index)
        ids.setdefault(name, (len(name) << 24) | index)
    return ids


def bsp_switches(scripts, volumes):
    """(volume name, from bsp, to bsp) of the scenario's structure bsp switch
    triggers (+0x130, 14-byte records: trigger volume, from, to)"""
    m = scripts.map
    scenario = next(t for t in m.tags if t[0] == 'scnr')
    count, address = struct.unpack_from('<II', m.data, m.file_offset(scenario[2]) + 0x130)
    names = list(volumes)
    return [(names[v], a, b) for v, a, b in
            (struct.unpack_from('<hhh', m.data, m.file_offset(address) + i * 0xe) for i in range(count))]


def encounter(script):
    """the encounter number of a script named eN_..., else 0"""
    m = re.match(r"e(\d+)_", script)
    return int(m.group(1)) if m else 0


def first_hop(switches, here, wanted):
    """the switch volume to cross first on the way from bsp here to wanted"""
    seen, frontier = {here}, [(here, None)]
    while frontier:
        bsp, hop = frontier.pop(0)
        for name, a, b in switches:
            if a == bsp and b not in seen:
                if b == wanted:
                    return hop or name
                seen.add(b)
                frontier.append((b, hop or name))
    return None


def volume_point(volume, a, b, c=None):
    """the point at fractions a, b along the volume's forward and left edges,
    a little above its floor (or at fraction c of its height)"""
    corner, fwd, left, up, ext = volume
    height = min(ext[2] / 2, 0.6) if c is None else ext[2] * c
    return tuple(corner[k] + fwd[k] * ext[0] * a + left[k] * ext[1] * b + up[k] * height
                 for k in range(3))


def inside(volume, point):
    corner, fwd, left, up, ext = volume
    d = [point[k] - corner[k] for k in range(3)]
    coords = [sum(d[k] * axis[k] for k in range(3)) for axis in (fwd, left, up)]
    return all(-0.05 <= c <= e + 0.05 for c, e in zip(coords, ext))


EDGE_SAMPLES = ((0.5, 0.02), (0.5, 0.98), (0.02, 0.5), (0.98, 0.5), (0.25, 0.02), (0.75, 0.02),
                (0.25, 0.98), (0.75, 0.98), (0.02, 0.25), (0.02, 0.75), (0.98, 0.25), (0.98, 0.75))
SAMPLES = ((0.5, 0.5), (0.25, 0.25), (0.75, 0.75), (0.25, 0.75), (0.75, 0.25), (0.5, 0.2), (0.5, 0.8),
           (0.2, 0.5), (0.8, 0.5))


def cross(g, volume):
    """walks through a volume the teleport refuses: from 1.5 units outside
    each side in turn (on a floor) through its centre to as far outside the
    opposite side; True once the structure bsp changes"""
    corner, fwd, left, up, ext = volume
    before = g.state
    centre = volume_point(volume, 0.5, 0.5)
    for axis, size in ((fwd, ext[0]), (left, ext[1])):
        for sign in (-1, 1):
            start = [centre[k] + axis[k] * sign * (size / 2 + 1.5) for k in range(3)]
            end = [2 * c - s0 for c, s0 in zip(centre, start)]
            g.guest("teleport %.3f %.3f %.3f" % tuple(start))
            g.wait(20)
            s = g.state
            if not s or math.dist(s["pos"][:2], start[:2]) > 1.0 or s["ground"] is None \
                    or s["pos"][2] - s["ground"] > 1.0:
                continue   # refused, blocked or over a drop: try another side
            g.go_to(centre, tolerance=0.3, max_steps=30)
            g.go_to(end, tolerance=0.4, max_steps=30)
            g.wait(30)
            if g.state and g.state.get("bsp") != s.get("bsp"):
                return True
    # a shaft or lift nobody walks across: placed inside, low in it, and
    # held there a while (05a's approach: its floor is the next bsp's, so
    # the player falls out of the volume before the switch is made)
    # place refuses points outside the loaded bsp, and a switch volume may
    # lie mostly in the next one: hold the player at the volume's points
    # this bsp has
    points = [volume_point(volume, a, b, c) for c in (0.4, 0.2, 0.7) for a in (0.5, 0.3, 0.7, 0.15, 0.85)
              for b in (0.5, 0.3, 0.7, 0.15, 0.85)]
    replies = len(g.test_replies)
    for point in points:
        g.guest("probe %.3f %.3f %.3f" % point)
    g.wait(6)
    answers = [r for r in g.test_replies[replies:] if r.startswith("probe ")]
    here = [p for p, a in zip(points, answers) if a == "probe inside"]
    for point in here[:6]:
        for _ in range(10):
            g.guest("place %.3f %.3f %.3f" % point)
            g.wait(3)
            if g.state and before and g.state.get("bsp") != before.get("bsp"):
                g.wait(30)
                return True
    if before:   # back to where it stood
        g.guest("teleport %.3f %.3f %.3f" % tuple(before["pos"]))
        g.wait(20)
    return False


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--map", required=True)
    parser.add_argument("--frames", type=int, default=120000)
    parser.add_argument("--work")
    parser.add_argument("--thorough", action="store_true",
                        help="serve the mission script's places only once no encounter script has one left "
                             "(07b's and 08a's missions wait only for the end volume, so the default jumps to it)")
    args = parser.parse_args()
    scripts = Scripts(str(ROOT / "orig/game/maps" / (args.map + ".map")))
    volumes, attached = trigger_volumes(scripts)
    switches = bsp_switches(scripts, volumes)
    string_ids = string_id_values(scripts.map)
    names = {name for name, _, _ in scripts.scripts}
    skip = ",".join(f"{frame}:a:8" for frame in range(5000, 9000, 400))
    acted = {}
    waited = {}      # (script, node) -> polls spent inside its volume, still waiting
    waves = {}       # (script, node) -> kill-enemies tries that found someone
    elsewhere = {}   # (script, node) -> (the bsp its place is not in, volume name)
    tried = {}       # (script, node) -> every bsp its place was found not to be in
    done = False
    with Game(start_map=args.map, menu_script=MENU_TO_CAMPAIGN + "," + skip, work=args.work) as g:
        g.wait_frame(9500)
        g.guest("deathless 1")
        last_report = 0
        idle = 0   # polls with nothing to do
        footing = None   # the last place the player stood
        while g.frame < args.frames:
            g.wait(150)
            # deathless, a fall into a pit never ends (07b's chasms): back to
            # the last place the player stood
            st = g.state
            if st and st.get("ground") is not None and abs(st["pos"][2] - st["ground"]) < 0.5:
                footing = st["pos"]
            elif st and footing and st["pos"][2] < footing[2] - 30:
                g.guest("place %.3f %.3f %.3f" % (footing[0], footing[1], footing[2] + 0.5))
                g.wait(20)
                print("frame", g.frame, "fell: back to", [round(x, 2) for x in footing], flush=True)
            threads = dict(g.scripts)
            ours = sum(t["script"] in names for t in threads.values())
            foreign = sum(t["script"] not in names for t in threads.values())
            # another map's scripts run now: most of them, or a few names this
            # map has none of (08a and 07b share their encounter names)
            if threads and (ours < len(threads) / 2 or foreign >= 3):
                done = True   # another map's scripts run now (some names, like
                break         # cinematic_skip_start, are in every map)
            places = []   # (times served, thread order, script, node, kind, volume name)
            away = []     # places in another structure bsp (the mission's first)
            # the latest encounter among the threads waiting for a bsp: only it
            # moves the player between bsps
            def waits_for_bsp(t):
                for n in t["nodes"]:
                    try:
                        if BSP_WAIT.search(scripts.expression(int(n, 16))):
                            return True
                    except (IndexError, ValueError):
                        pass
                return False
            waiting_texts = []
            latest = max((encounter(t["script"]) for t in threads.values() if waits_for_bsp(t)), default=0)
            for order, (index, thread) in enumerate(sorted(threads.items())):
                for node in thread["nodes"]:
                    try:
                        text = scripts.expression(int(node, 16))
                    except (IndexError, ValueError):
                        continue
                    if not text.startswith("(sleep_until"):
                        continue
                    waiting_texts.append(text)
                    key = (thread["script"], node)
                    count = acted.get(key, 0)
                    if count >= 6 and text.startswith("(sleep_until (>= (device_get_position"):
                        continue   # a machine nothing presses (08b's Scarab): the outer wait decides
                    targets = [v for _, v, _ in VOLUME_TEST.findall(text) if v in volumes]
                    bsp = BSP_WAIT.search(text)
                    if bsp and not targets:
                        # a structure bsp switch: cross the switch volumes on the way
                        wanted = (int(bsp.group(1)) - 0xffff0000) if int(bsp.group(1)) > 0xffff else int(bsp.group(1))
                        # once per script and bsp: ambient scripts (kill volumes,
                        # wind) wait for each bsp in turn, forever
                        key = (thread["script"], ("bsp", wanted))
                        count = acted.get(key, 0)
                        hop = first_hop(switches, g.state and g.state.get("bsp"), wanted)
                        # the mission's own bsp waits lead; an ambient script's
                        # (kill volumes, wind: they wait for each bsp in turn)
                        # only once nothing else is left, or it pulls the player
                        # back out of the bsp the mission has gone on to (05a)
                        # an earlier encounter's ambient wait gives way to a later
                        # one's (08b: e5's waits for bsp 1 pulled the player
                        # out of e7's bsp 0 again and again)
                        later = encounter(thread["script"]) < latest
                        if hop and count < 8 and not later and (thread["script"].startswith("mission") or idle >= 10
                                                               or args.thorough and encounter(thread["script"])):
                            places.append((count, order, key[0], key[1], "cross", hop))
                        if "(ai_actors" in text and count < 8:
                            g.guest("kill_enemies 0")   # and the enemies the wait counts (08b's e7 brutes)
                    elif targets and "(not (volume_test" not in text and count < len(SAMPLES):
                        volume = live_volume(g, volumes, targets[0]) if targets[0] in attached else volumes[targets[0]]
                        if volume and not (g.state and inside(volume, g.state["pos"])):
                            if elsewhere.get(key, (None,))[0] == (g.state and g.state.get("bsp")):
                                away.append((not thread["script"].startswith("mission"), count, order, key, targets[0]))
                            else:
                                places.append((count, order, thread["script"], node, "teleport", targets[0]))
                        elif volume:
                            # inside, and still waiting: the rest of the test is
                            # about the spot (04b's airlock wants its door shut
                            # behind the player), so after a while try another
                            waited[key] = waited.get(key, 0) + 1
                            if waited[key] % 20 == 4:   # who else is here
                                replies = len(g.test_replies)
                                g.guest("units 12")
                                g.wait(2)
                                print("frame", g.frame, thread["script"], node, "inside, nearby:",
                                      g.test_replies[replies:], flush=True)
                            if waited[key] % 4 == 2:
                                g.guest("kill_enemies 12")   # a fight at a door keeps it open
                            if waited[key] % 4 == 0:
                                places.append((count, order, thread["script"], node, "teleport", targets[0]))
                    elif "vehicle_test_seat" in text and count < 6:
                        # a seat in a vehicle (04a's Banshees): the nearest
                        # free seat with the label the script tests
                        label = SEAT_LABEL.search(text)
                        replies = len(g.test_replies)
                        g.guest("enter_vehicle %d" % (string_ids.get(label.group(1), 0) if label else 0))
                        g.wait(60)
                        reply = next((r for r in g.test_replies[replies:] if r.startswith("enter_vehicle")), "no reply")
                        print("frame", g.frame, thread["script"], node, "->", reply, flush=True)
                        if reply != "enter_vehicle no free seat":   # not spawned yet: no try spent
                            acted[key] = count + 1
                    elif ("regret_dead" in text or "ai_vitality_pinned" in text) and count < 40:
                        # a boss that cannot die (05b's Regret): board his throne
                        # and beat him down to his pinned vitality, round after
                        # round, as the player does
                        replies = len(g.test_replies)
                        if "regret" in text:   # Regret is fought aboard his throne
                            g.guest("enter_vehicle 0 boss")
                            g.wait(60)
                        g.guest("kill_enemies %d" % (10 if "regret" in text else 0))
                        g.wait(30)
                        print("frame", g.frame, thread["script"], node, "-> boss round",
                              [r for r in g.test_replies[replies:] if r.startswith(("enter_vehicle", "killed"))],
                              flush=True)
                        acted[key] = count + 1
                    elif ACTOR_NEAR_WAIT.search(text) and count < 8:
                        # near the command script's own actor (03b's Scarab
                        # pilots): visit the living units of other teams
                        # nearby, nearest first, one a try
                        replies = len(g.test_replies)
                        g.guest("units 150")
                        g.wait(2)
                        units = [UNIT_LINE.match(r) for r in g.test_replies[replies:]]
                        me = g.state and g.state.get("unit")
                        mine = next((m for m in units if m and me and int(m.group(1), 16) == int(me, 16)), None)
                        team = mine.group(2) if mine else "1"
                        here = g.state["pos"] if g.state else (0, 0, 0)
                        # units attached to something first (the pilots ride
                        # the Scarab), then the nearest
                        others = sorted((m for m in units if m and m.group(2) != team and float(m.group(3)) > 0),
                                        key=lambda m: (m.group(7).lower() == "ffffffff",
                                                       sum((float(m.group(4 + i)) - here[i]) ** 2 for i in range(3))))
                        reply = "no units"
                        if others:
                            target = others[count % len(others)]
                            replies = len(g.test_replies)
                            g.guest("goto_unit " + target.group(1))
                            g.wait(30)
                            reply = next((r for r in g.test_replies[replies:] if r.startswith("goto_unit")), "no reply")
                            reply += " " + target.group(1)
                        print("frame", g.frame, thread["script"], node, "->", reply, flush=True)
                        acted[key] = count + 1
                    elif OBJECT_NEAR_WAIT.search(text) and count < 6:
                        # near a named object (07a's mausoleum holograms)
                        replies = len(g.test_replies)
                        g.guest("goto_object " + OBJECT_NEAR_WAIT.search(text).group(1))
                        g.wait(30)
                        reply = next((r for r in g.test_replies[replies:] if r.startswith("goto_object")), "no reply")
                        print("frame", g.frame, thread["script"], node, "->", reply, flush=True)
                        acted[key] = count + 1
                    elif HEALTH_WAIT.search(text) and count < 6:
                        # an object to break (04b's cables), once a script
                        # lets it take damage
                        replies = len(g.test_replies)
                        g.guest("destroy " + HEALTH_WAIT.search(text).group(1))
                        g.wait(30)
                        reply = next((r for r in g.test_replies[replies:] if r.startswith("destroy")), "no reply")
                        if reply != "destroy invulnerable":   # not yet: no try spent
                            print("frame", g.frame, thread["script"], node, "->", reply, flush=True)
                            acted[key] = count + 1
                    elif DEVICE_WAIT.search(text) and count < 6:
                        # a machine moved by a control (04b's elevator up to
                        # the cable room): press it from beside it
                        replies = len(g.test_replies)
                        g.guest("operate " + DEVICE_WAIT.search(text).group(1))
                        g.wait(60)
                        reply = next((r for r in g.test_replies[replies:] if r.startswith("operate")), "no reply")
                        print("frame", g.frame, thread["script"], node, "->", reply, flush=True)
                        acted[key] = count + 1
                        if "pressed 0" in reply:
                            continue   # nothing to press: the outer wait may say what it is waiting on
                    elif ("ai_living_count" in text or "ai_strength" in text or "ai_living_fraction" in text
                           or "ai_nonswarm_count" in text) \
                            and count < 6:
                        replies = len(g.test_replies)
                        g.guest("kill_enemies 0")
                        g.wait(30)
                        reply = next((r for r in g.test_replies[replies:] if r.startswith("killed")), "no reply")
                        print("frame", g.frame, thread["script"], node, "-> kill enemies", reply, flush=True)
                        # a helper's wait serves wave after wave (07b's
                        # e12_safe_to_spawn_flood): only a try that found
                        # nobody counts, but at most 24 that killed some
                        # (03b's e23 pelicans bring reinforcements forever)
                        if reply in ("killed 0", "no reply"):
                            acted[key] = count + 1
                        else:
                            waves[key] = waves.get(key, 0) + 1
                            if waves[key] >= 24:
                                acted[key] = count + 1
                                waves[key] = 0
                    elif count == 0:
                        print("frame", g.frame, thread["script"], node, "waits:", text[:160], flush=True)
                        acted[key] = 1
                    else:
                        continue   # nothing left to try at this wait: the one around it (08b's boss loop)
                    break
            if args.thorough and any(not a[3][0].startswith("mission") for a in away):
                # an encounter's place waits in another bsp: the mission's own
                # places (07b's and 08a's end volumes) wait for it
                places = [p for p in places if not p[2].startswith("mission")]
            idle = 0 if places or away else idle + 1
            if idle and idle % 10 == 5 and any("game_safe_to_save" in scripts_text for scripts_text in waiting_texts):
                # a wait on game_safe_to_save: log which of its checks says no
                replies = len(g.test_replies)
                g.guest("safe_to_save")
                g.wait(3)
                print("frame", g.frame, "->", next((r for r in g.test_replies[replies:] if r.startswith("safe_to_save")), "no reply"), flush=True)
            if places:
                # one place a poll, the least served first: every thread's
                # wait gets its turn
                # crossing to another bsp last: the place there may be a
                # later part of the mission (04b's cable room above the
                # control room)
                # among bsp crossings, a later encounter's first (08b: e5's
                # ambient wait for bsp 1 pulled the player back from e7's 0)
                # thorough: the earliest encounter first, crossing or not
                # (08a's e2 waits for bsp 1 while e6-e10's volumes lie in
                # bsp 0 within reach: they came first and skipped e2-e5)
                count, _, script, node, kind, name = min(
                    places, key=lambda p: (encounter(p[2]) or 1000, p[2].startswith("mission"), p[4] == "cross", p)
                    if args.thorough else (p[4] == "cross", -encounter(p[2]), p))
                acted[(script, node)] = count + 1
                if kind == "cross":
                    crossed = cross(g, volumes[name])
                    print("frame", g.frame, script, node, "-> cross", name, "done" if crossed else "failed", flush=True)
                else:
                    volume = live_volume(g, volumes, name) if name in attached else volumes[name]
                    if volume is None:
                        continue   # its object is gone for now
                    # the points of the volume in the loaded structure bsp (its
                    # floor may be under the ground, or all of it elsewhere)
                    candidates = [volume_point(volume, a, b, c) for c in (None, 0.3, 0.6, 0.9) for a, b in SAMPLES]
                    # and along its edges: a long volume may reach out of the
                    # world but for the end the player walks in at (07b's
                    # tv_e12_end, 108 units deep)
                    candidates += [volume_point(volume, a, b, c) for c in (None, 0.4) for a, b in EDGE_SAMPLES]
                    replies = len(g.test_replies)
                    for p in candidates:
                        g.guest("probe %.3f %.3f %.3f" % p)
                    g.wait(4)
                    # only the probes' answers: a late reply to another hook
                    # (kill_enemies' "killed N") would shift them all
                    answers = [r for r in g.test_replies[replies:] if r.startswith("probe ")][:len(candidates)]
                    here = [p for p, a in zip(candidates, answers) if a == "probe inside"]
                    if here:
                        # from the spot this try's count picks, on through the
                        # others while a spot has no floor under it (05a's
                        # overlook hangs partly over a ravine); a volume with
                        # no floor anywhere is another bsp's room (04b)
                        for point in here[count % len(here):] + here[:count % len(here)]:
                            replies = len(g.test_replies)
                            g.guest("teleport %.3f %.3f %.3f" % point)
                            g.wait(20)
                            reply = next((r for r in g.test_replies[replies:] if r.startswith("teleport ")), "")
                            if reply == "teleport refused":
                                # in a bsp switch volume (05a's approach overlaps
                                # one): walked into, the switch happens too
                                replies = len(g.test_replies)
                                g.guest("place %.3f %.3f %.3f" % point)
                                g.wait(20)
                                reply = next((r for r in g.test_replies[replies:] if r.startswith("teleport ")), "")
                            if reply != "teleport no floor":
                                break
                        if reply == "teleport no floor":
                            # no floor anywhere in it: a place over a drop the
                            # player jumps into (07b's end, over the chasm to the
                            # ship), or another bsp's room; dropped in, the
                            # fall recovery brings a wrong guess back
                            point = here[count % len(here)]
                            replies = len(g.test_replies)
                            g.guest("place %.3f %.3f %.3f" % point)
                            g.wait(5)
                            reply = next((r for r in g.test_replies[replies:] if r.startswith("teleport ")), "")
                            if reply != "teleport done":
                                reply = "teleport outside the bsp"
                    else:
                        point, reply = candidates[0], "teleport outside the bsp"
                    landed = inside(volume, g.state["pos"])
                    if not landed and reply == "teleport done" and g.state.get("vehicle"):
                        # seated, the vehicle went (08b's banshee, flown off the
                        # boss platform): out of it, then the place on foot
                        g.guest("exit_vehicle")
                        g.wait(60)
                        g.guest("teleport %.3f %.3f %.3f" % point)
                        g.wait(20)
                        landed = inside(volume, g.state["pos"])
                    if not landed and reply == "teleport done":
                        # the spot is taken (a doorway, a closed room): walk
                        # the rest of the way in, as a player would
                        g.go_to(point, tolerance=0.3, max_steps=25)
                        landed = inside(volume, g.state["pos"])
                    if not landed and reply == "teleport done":
                        # still out: nothing to stand on there (08b's boss
                        # platform turns on a machine the floor ray does not
                        # see): held in it while the script polls
                        for _ in range(30):
                            g.guest("place %.3f %.3f %.3f" % point)
                            g.wait(3)
                        landed = inside(volume, g.state["pos"])
                    print("frame", g.frame, script, node, "-> teleport", name, [round(x, 2) for x in point],
                          "inside" if landed else "outside (%s)" % reply, flush=True)
                    if reply == "teleport outside the bsp":
                        # in a bsp not loaded: left till nothing is left here
                        elsewhere[(script, node)] = (g.state and g.state.get("bsp"), name)
                        tried.setdefault((script, node), set()).add(g.state and g.state.get("bsp"))
                        acted[(script, node)] = count
            elif away:
                # nothing left in this bsp: through the switch from it nearest
                # the place, the mission script's place first
                # the mission script's place first, or an encounter's when thorough
                _, _, _, key, name = min(away, key=lambda a: (not a[0], a[1:]) if args.thorough else a)
                nearby = [w for w in switches if w[1] == (g.state and g.state.get("bsp"))]
                if nearby:
                    centre = volume_point(volumes[name], 0.5, 0.5)
                    # toward the nearest bsp not yet tried for this place (06b's
                    # key ride lies two switches on, in bsp 3: the nearest
                    # switch led back and forth between bsps 0 and 1); with
                    # every bsp tried, through the switch nearest the place
                    here_bsp = g.state and g.state.get("bsp")
                    untried = sorted({b for _, _, b in switches} - tried.get(key, set()) - {here_bsp})
                    hops = [first_hop(switches, here_bsp, b) for b in untried]
                    hops = [h for h in hops if h]
                    way = next((w for w in nearby if hops and w[0] == hops[0]), None) or \
                        min(nearby, key=lambda w: math.dist(volume_point(volumes[w[0]], 0.5, 0.5), centre))
                    print("frame", g.frame, "-> cross", way[0], "toward", name, "for", key[0],
                          "done" if cross(g, volumes[way[0]]) else "failed", flush=True)
            if g.state is None or g.frame - g.state["frame"] > 1000:
                print("frame", g.frame, "player lost", flush=True)
                break
            if g.frame - last_report > 3000:
                last_report = g.frame
                s = g.state
                print("frame", g.frame, "pos", s["pos"] if s else None, "threads",
                      sorted({t["script"] for t in threads.values()}), flush=True)
        print("ended at frame", g.frame, "level finished" if done else "level not finished")
        if done:
            print("next map's scripts:", sorted({t["script"] for t in g.scripts.values()})[:12])
            g.wait(900)
            print("next map's player:", g.state and g.state["pos"])
        print("snapshot:", g.snapshot()[0])
    return 0 if done else 1


if __name__ == "__main__":
    sys.exit(main())
