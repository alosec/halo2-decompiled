#!/usr/bin/env python3
"""Walks the player around a level at random with h2_driver and reports any
fall through the floor: the unit more than 0.5 units below the ground a ray
finds under it, or falling with no ground within 50 units.

Usage (repository root): /usr/bin/python3 port/tools/playtest_wander.py
           [--map 01b_spacestation] [--seconds 120] [--seed 1] [--work DIR]

Starts the map from the main menu (skipping its cinematic with A), then
alternates turns, walks, strafes and jumps. Exit status 1 on a fall; the
snapshot taken at the fall is printed.
"""
import argparse
import random
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from h2_driver import Game, MENU_TO_CAMPAIGN  # noqa: E402


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--map", default="01b_spacestation")
    parser.add_argument("--seconds", type=float, default=120)
    parser.add_argument("--seed", type=int, default=1)
    parser.add_argument("--work")
    args = parser.parse_args()
    rng = random.Random(args.seed)
    skip = ",".join(f"{frame}:a:8" for frame in range(5000, 9000, 400))
    with Game(start_map=args.map, menu_script=MENU_TO_CAMPAIGN + "," + skip, work=args.work) as game:
        game.wait_frame(9500)
        start = game.frame
        checked = len(game.history)
        while game.frame - start < args.seconds * 60:
            action = rng.choice(["turn", "walk", "walk", "strafe", "jump"])
            if action == "turn":
                game.press(rng.choice(["rx+", "rx-"]) + str(rng.randint(50, 100)), rng.randint(3, 20))
            elif action == "walk":
                game.press(rng.choice(["ly+", "ly+", "ly-"]), rng.randint(20, 120))
            elif action == "strafe":
                game.press(rng.choice(["lx+", "lx-"]), rng.randint(20, 80))
            else:
                game.press("a", 4)
                game.press("ly+", 40)
            game.wait(rng.randint(20, 90))
            for state in game.history[checked:]:
                ground = state["ground"]
                z = state["pos"][2]
                if (ground is not None and z < ground - 0.5) or (ground is None and state["vel"][2] < -5):
                    print("fall at frame", state["frame"], "pos", state["pos"], "ground", ground, "vel", state["vel"])
                    print("snapshot:", game.snapshot()[0])
                    return 1
            checked = len(game.history)
        s = game.state
        print("no fall in %d frames; ended at" % (game.frame - start), s["pos"], "ground", s["ground"])
        print("snapshot:", game.snapshot()[0])
    return 0


if __name__ == "__main__":
    sys.exit(main())
