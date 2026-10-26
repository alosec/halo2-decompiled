#!/usr/bin/env python3
"""Plays The Armory (01a) to its end with h2_driver, reacting to what the
level script waits for, and reports how far it got.

Usage (repository root): /usr/bin/python3 port/tools/playtest_armory.py
           [--work DIR] [--frames N]

Each h2-script line (H2_TRACE=state, every 150 frames) gives mission_01a's
syntax nodes; the waits below are its sleep_until nodes in the retail
01a_tutorial map (port/tools/hs_decompile.py --node prints them). Positions
are the map's trigger volumes and flags. Exit status 0 when the level
ends (the tram ride's last wait passes and the map changes), 1 otherwise.
"""
import argparse
import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from h2_driver import Game, MENU_TO_CAMPAIGN  # noqa: E402
import playtest_armory_look as look  # noqa: E402

LIGHTS = (look.TOP, look.BOTTOM) + tuple(look.SIDES)
ZAPPER_WAYPOINTS = ((30.1, -48.06), (28.7, -46.6), (30.5, -44.6), (32.65, -45.48))  # around the console
TV_ZAPPER = (33.09, -46.19)          # the red square
TV_ELEVATOR = (28.08, -44.78)        # tv_elevator_tram_bot
TV_TRAM = (30.86, -47.32)            # tv_tram (the upper level)

# mission_01a's waits (node index -> what the player has to do)
WAITS = {
    "fed": "lights", "1096": "accept", "1328": "leave_start", "1363": "zapper",
    "1396": "wait", "13a2": "zapper", "13e9": "switch", "1511": "wait",
    "1568": "wait", "15bd": "wait", "16e7": "wait", "1704": "elevator",
    "172d": "wait", "1785": "wait", "17b2": "wait", "17c7": "tram", "17dd": "wait",
}


def mission(game):
    for thread in game.scripts.values():
        if thread["script"] == "mission_01a":
            return thread
    return None


def current_wait(game):
    m = mission(game)
    if not m:
        return None
    for node in m["nodes"]:
        if node in WAITS:
            return node
    return None


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--work")
    parser.add_argument("--frames", type=int, default=60000)
    args = parser.parse_args()
    seen = []
    with Game(menu_script=MENU_TO_CAMPAIGN + ",5600:a:8", work=args.work) as g:
        g.wait_frame(6400)
        light = 0
        while g.frame < args.frames:
            wait = current_wait(g)
            if wait and (not seen or seen[-1] != wait):
                seen.append(wait)
                print("frame", g.frame, "waiting at", wait, WAITS[wait], "pos", g.state["pos"], flush=True)
            action = WAITS.get(wait)
            if action is None:
                # the look tests before the diagnostic, or between waits: look
                # at the lights in turn as the technician asks
                if not mission(g) and seen:
                    break   # the level's scripts are gone: the next map loads
                g.aim_at(LIGHTS[light % 4], tolerance_degrees=2.5, max_steps=40, hold=60)
                light += 1
            elif action == "lights":
                g.aim_at(LIGHTS[light % 4], tolerance_degrees=2.5, max_steps=40, hold=60)
                light += 1
            elif action == "accept":
                g.wait(60)
                g.press("a", 6)
                g.wait(150)
            elif action == "leave_start":
                g.go_to(ZAPPER_WAYPOINTS[0], tolerance=0.6, max_steps=40)
            elif action == "zapper":
                for point in ZAPPER_WAYPOINTS[1:] + (TV_ZAPPER,):
                    g.go_to(point, tolerance=0.5, max_steps=40)
                g.wait(150)
            elif action == "switch":
                for point in ZAPPER_WAYPOINTS[2:] + (TV_ZAPPER,):
                    g.go_to(point, tolerance=0.3 if point != TV_ZAPPER else 0.12, max_steps=40)
                # the switch is in front of the square: try each way round,
                # then stand still (the cage closes around the square); the
                # script state refreshes every 150 frames
                for yaw in (180, 135, -135, 90, -90, 0):
                    g.face(yaw)
                    g.press("x", 10)
                    g.wait(200)
                    if current_wait(g) != "13e9":
                        break
                print("switch at frame", g.frame, "pos", g.state["pos"], flush=True)
                g.wait(900)
            elif action == "elevator":
                g.go_to(TV_ELEVATOR, tolerance=0.4, max_steps=60)
                g.wait(150)
            elif action == "tram":
                g.go_to(TV_TRAM, tolerance=0.4, max_steps=60)
                g.wait(150)
            else:
                g.wait(150)
        s = g.state
        print("ended at frame", g.frame, "waits seen", " ".join(seen), "pos", s["pos"] if s else None)
        print("snapshot:", g.snapshot()[0])
        done = "17c7" in seen and not mission(g)
    return 0 if done else 1


if __name__ == "__main__":
    sys.exit(main())
