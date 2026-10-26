#!/usr/bin/env python3
"""Walks the player around a level at random with h2_driver and reports any
fall through the floor: the unit more than 0.5 units below the ground a ray
finds under it, or a jump in position its velocity can't explain (the snaps
that pushed the player through floors). Falling into a pit, with no ground
under it, is the game's own and isn't reported.

Usage (repository root): /usr/bin/python3 port/tools/playtest_wander.py
           [--map 01b_spacestation] [--seconds 120] [--seed 1] [--work DIR]

Starts the map from the main menu (skipping its cinematic with A; with
--multiplayer, as a split-screen Slayer game from the lobby), then
alternates turns, walks, strafes and jumps (with --fire also shooting,
throwing grenades and reloading). Exit status 1 on a fall; the
snapshot taken at the fall is printed.
"""
import argparse
import random
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from h2_driver import Game, MENU_TO_CAMPAIGN, MENU_TO_SPLIT_SCREEN  # noqa: E402


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--map", default="01b_spacestation")
    parser.add_argument("--seconds", type=float, default=120)
    parser.add_argument("--seed", type=int, default=1)
    parser.add_argument("--work")
    parser.add_argument("--fire", action="store_true", help="also fire, throw grenades and reload")
    parser.add_argument("--multiplayer", action="store_true", help="--map is a multiplayer map: start a split-screen Slayer game")
    args = parser.parse_args()
    rng = random.Random(args.seed)
    if args.multiplayer:
        script, playing = MENU_TO_SPLIT_SCREEN, 5000
    else:
        script = MENU_TO_CAMPAIGN + "," + ",".join(f"{frame}:a:8" for frame in range(5000, 9000, 400))
        playing = 9500
    with Game(start_map=args.map, menu_script=script, work=args.work) as game:
        game.wait_frame(playing)
        start = game.frame
        checked = len(game.history)
        while game.frame - start < args.seconds * 60:
            actions = ["turn", "walk", "walk", "strafe", "jump"]
            if args.fire:
                actions += ["fire", "fire", "grenade", "reload"]
            action = rng.choice(actions)
            if action == "turn":
                game.press(rng.choice(["rx+", "rx-"]) + str(rng.randint(50, 100)), rng.randint(3, 20))
            elif action == "walk":
                game.press(rng.choice(["ly+", "ly+", "ly-"]), rng.randint(20, 120))
            elif action == "strafe":
                game.press(rng.choice(["lx+", "lx-"]), rng.randint(20, 80))
            elif action == "fire":
                game.press("rt", rng.randint(5, 60))
            elif action == "grenade":
                game.press("lt", 4)
            elif action == "reload":
                game.press("x", 4)
            else:
                game.press("a", 4)
                game.press("ly+", 40)
            game.wait(rng.randint(20, 90))
            for index in range(max(checked, 1), len(game.history)):
                state, previous = game.history[index], game.history[index - 1]
                ground = state["ground"]
                z = state["pos"][2]
                frames = max(1, state["frame"] - previous["frame"])
                moved = sum((a - b) ** 2 for a, b in zip(state["pos"], previous["pos"])) ** 0.5
                speed = sum(v * v for v in state["vel"]) ** 0.5
                if state.get("health", 1) <= 0 or previous.get("health", 1) <= 0:
                    continue   # a dead player's body is thrown about by the physics
                if ground is not None and z < ground - 0.5:
                    problem = "below the ground"
                elif state["unit"] == previous["unit"] and moved > 0.3 + 3 * speed * frames / 30.0 and moved < 1000:
                    problem = "snapped %.2f units at speed %.2f" % (moved, speed)
                else:
                    continue
                print(problem, "at frame", state["frame"], "pos", state["pos"], "ground", ground, "vel", state["vel"])
                print("snapshot:", game.snapshot()[0])
                return 1
            checked = len(game.history)
        s = game.state
        print("no fall in %d frames; ended at" % (game.frame - start), s["pos"], "ground", s["ground"])
        print("snapshot:", game.snapshot()[0])
    return 0


if __name__ == "__main__":
    sys.exit(main())
