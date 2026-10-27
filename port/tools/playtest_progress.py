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
VOLUME_TEST = re.compile(r"\((volume_test_objects(?:_all)?|volume_test_object) (\S+) \((players|player0|player1)\)\)")


def trigger_volumes(scripts):
    """name -> (corner, forward, left, up, extents) of the scenario's trigger
    volumes (block at +0x108, 0x44-byte records: name id, forward +0xc, up
    +0x18, corner +0x24, extents +0x30)"""
    m = scripts.map
    scenario = next(t for t in m.tags if t[0] == 'scnr')
    base = m.file_offset(scenario[2])
    count, address = struct.unpack_from('<II', m.data, base + 0x108)
    volumes = {}
    for i in range(count):
        o = m.file_offset(address) + i * 0x44
        name = m.string_id(struct.unpack_from('<I', m.data, o)[0])
        fwd = struct.unpack_from('<3f', m.data, o + 0xc)
        up = struct.unpack_from('<3f', m.data, o + 0x18)
        corner = struct.unpack_from('<3f', m.data, o + 0x24)
        ext = struct.unpack_from('<3f', m.data, o + 0x30)
        left = (up[1] * fwd[2] - up[2] * fwd[1], up[2] * fwd[0] - up[0] * fwd[2], up[0] * fwd[1] - up[1] * fwd[0])
        volumes[name] = (corner, fwd, left, up, ext)
    return volumes


def volume_point(volume, a, b):
    """the point at fractions a, b along the volume's forward and left edges,
    a little above its floor"""
    corner, fwd, left, up, ext = volume
    return tuple(corner[k] + fwd[k] * ext[0] * a + left[k] * ext[1] * b + up[k] * min(ext[2] / 2, 0.6)
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
    volumes = trigger_volumes(scripts)
    names = {name for name, _, _ in scripts.scripts}
    skip = ",".join(f"{frame}:a:8" for frame in range(5000, 9000, 400))
    acted = {}
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
            served = False
            for index, thread in sorted(threads.items()):
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
                        # a structure bsp switch: the map's "bsp_FROM,TO" volume
                        wanted = (int(bsp.group(1)) - 0xffff0000) if int(bsp.group(1)) > 0xffff else int(bsp.group(1))
                        targets = [v for v in volumes if re.fullmatch(r"bsp_\d+,%d" % wanted, v)]
                    negated = "(not (volume_test" in text
                    if bsp and targets and not served and count < 4:
                        print("frame", g.frame, thread["script"], node, "-> cross", targets[0],
                              "done" if cross(g, volumes[targets[0]]) else "failed", flush=True)
                        acted[key] = count + 1
                        served = True
                    elif targets and not negated and count < len(SAMPLES) and not served:
                        volume = volumes[targets[0]]
                        if g.state and inside(volume, g.state["pos"]):
                            break   # already there: the script will see it
                        point = volume_point(volume, *SAMPLES[count])
                        g.guest("teleport %.3f %.3f %.3f" % point)
                        g.wait(20)
                        landed = inside(volume, g.state["pos"])
                        if not landed:
                            # the spot is taken (a doorway, a closed room): walk
                            # the rest of the way in, as a player would
                            g.go_to(point, tolerance=0.3, max_steps=25)
                            landed = inside(volume, g.state["pos"])
                        print("frame", g.frame, thread["script"], node, "-> teleport", targets[0],
                              [round(x, 2) for x in point], "inside" if landed else "outside",
                              flush=True)
                        acted[key] = count + 1
                        served = True   # one place at a time: hold it till the next poll
                    elif ("ai_living_count" in text or "ai_strength" in text or "ai_living_fraction" in text) \
                            and count < 6:
                        g.guest("kill_enemies 0")
                        print("frame", g.frame, thread["script"], node, "-> kill enemies", flush=True)
                        acted[key] = count + 1
                    elif count == 0:
                        print("frame", g.frame, thread["script"], node, "waits:", text[:160], flush=True)
                        acted[key] = 1
                    break
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
