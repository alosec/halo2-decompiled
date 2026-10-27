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
    if before:   # back to where it stood
        g.guest("teleport %.3f %.3f %.3f" % tuple(before["pos"]))
        g.wait(20)
    return False


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--map", required=True)
    parser.add_argument("--frames", type=int, default=120000)
    parser.add_argument("--work")
    args = parser.parse_args()
    scripts = Scripts(str(ROOT / "orig/game/maps" / (args.map + ".map")))
    volumes, attached = trigger_volumes(scripts)
    switches = bsp_switches(scripts, volumes)
    string_ids = string_id_values(scripts.map)
    names = {name for name, _, _ in scripts.scripts}
    skip = ",".join(f"{frame}:a:8" for frame in range(5000, 9000, 400))
    acted = {}
    elsewhere = {}   # (script, node) -> (the bsp its place is not in, volume name)
    done = False
    with Game(start_map=args.map, menu_script=MENU_TO_CAMPAIGN + "," + skip, work=args.work) as g:
        g.wait_frame(9500)
        g.guest("deathless 1")
        last_report = 0
        while g.frame < args.frames:
            g.wait(150)
            threads = dict(g.scripts)
            ours = sum(t["script"] in names for t in threads.values())
            if threads and ours < len(threads) / 2:
                done = True   # another map's scripts run now (some names, like
                break         # cinematic_skip_start, are in every map)
            places = []   # (times served, thread order, script, node, kind, volume name)
            away = []     # places in another structure bsp (the mission's first)
            for order, (index, thread) in enumerate(sorted(threads.items())):
                for node in thread["nodes"]:
                    try:
                        text = scripts.expression(int(node, 16))
                    except (IndexError, ValueError):
                        continue
                    if not text.startswith("(sleep_until"):
                        continue
                    key = (thread["script"], node)
                    count = acted.get(key, 0)
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
                        if hop and count < 8:
                            places.append((count, order, key[0], key[1], "cross", hop))
                    elif targets and "(not (volume_test" not in text and count < len(SAMPLES):
                        volume = live_volume(g, volumes, targets[0]) if targets[0] in attached else volumes[targets[0]]
                        if volume and not (g.state and inside(volume, g.state["pos"])):
                            if elsewhere.get(key, (None,))[0] == (g.state and g.state.get("bsp")):
                                away.append((not thread["script"].startswith("mission"), count, order, key, targets[0]))
                            else:
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
                    elif ("ai_living_count" in text or "ai_strength" in text or "ai_living_fraction" in text) \
                            and count < 6:
                        g.guest("kill_enemies 0")
                        print("frame", g.frame, thread["script"], node, "-> kill enemies", flush=True)
                        acted[key] = count + 1
                    elif count == 0:
                        print("frame", g.frame, thread["script"], node, "waits:", text[:160], flush=True)
                        acted[key] = 1
                    break
            if places:
                # one place a poll, the least served first: every thread's
                # wait gets its turn
                count, _, script, node, kind, name = min(places)
                acted[(script, node)] = count + 1
                if kind == "cross":
                    crossed = cross(g, volumes[name])
                    print("frame", g.frame, script, node, "-> cross", name, "done" if crossed else "failed", flush=True)
                    if crossed and g.state and g.state.get("bsp") == node[1]:
                        acted[(script, node)] = 99   # there: served for good
                else:
                    volume = live_volume(g, volumes, name) if name in attached else volumes[name]
                    if volume is None:
                        continue   # its object is gone for now
                    # the points of the volume in the loaded structure bsp (its
                    # floor may be under the ground, or all of it elsewhere)
                    candidates = [volume_point(volume, a, b, c) for a, b in SAMPLES for c in (None, 0.3, 0.6, 0.9)]
                    replies = len(g.test_replies)
                    for p in candidates:
                        g.guest("probe %.3f %.3f %.3f" % p)
                    g.wait(4)
                    answers = g.test_replies[replies:replies + len(candidates)]
                    here = [p for p, a in zip(candidates, answers) if a == "probe inside"]
                    if here:
                        point = here[count % len(here)]
                        replies = len(g.test_replies)
                        g.guest("teleport %.3f %.3f %.3f" % point)
                        g.wait(20)
                        reply = g.test_replies[replies] if len(g.test_replies) > replies else ""
                    else:
                        point, reply = candidates[0], "teleport outside the bsp"
                    landed = inside(volume, g.state["pos"])
                    if not landed and reply == "teleport done":
                        # the spot is taken (a doorway, a closed room): walk
                        # the rest of the way in, as a player would
                        g.go_to(point, tolerance=0.3, max_steps=25)
                        landed = inside(volume, g.state["pos"])
                    print("frame", g.frame, script, node, "-> teleport", name, [round(x, 2) for x in point],
                          "inside" if landed else "outside (%s)" % reply, flush=True)
                    if reply == "teleport outside the bsp":
                        # in a bsp not loaded: left till nothing is left here
                        elsewhere[(script, node)] = (g.state and g.state.get("bsp"), name)
                        acted[(script, node)] = count
            elif away:
                # nothing left in this bsp: through the switch from it nearest
                # the place, the mission script's place first
                _, _, _, key, name = min(away)
                nearby = [w for w in switches if w[1] == (g.state and g.state.get("bsp"))]
                if nearby:
                    centre = volume_point(volumes[name], 0.5, 0.5)
                    way = min(nearby, key=lambda w: math.dist(volume_point(volumes[w[0]], 0.5, 0.5), centre))
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
