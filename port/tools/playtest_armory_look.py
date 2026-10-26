#!/usr/bin/env python3
"""Plays the Armory's "look at the lights" tutorial with h2_driver and checks
that it reaches the "Press A to accept normal aiming" prompt.

Usage (repository root): /usr/bin/python3 port/tools/playtest_armory_look.py [work dir]

The steps: look at the top light, then the bottom one (yaw is locked; the
stick direction is learned, since the tutorial tries inverted look), then
either side light. Each look must hold for a moment, from the head marker
(as the game's own check, function_cc2b0, measures it within 5 degrees).
Prints a snapshot after each step (the light just looked at should glow
green: the lamps are lens flares) and at the end; exit status 1 if a step never passed.
"""
import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from h2_driver import Game, MENU_TO_CAMPAIGN  # noqa: E402

# the light markers' positions (fixed by the map)
TOP, BOTTOM = (28.907, -49.267, 3.794), (28.911, -49.270, 3.446)
SIDES = (28.754, -49.372, 3.618), (29.022, -49.136, 3.621)


def pitch(state):
    return math.degrees(math.asin(max(-1, min(1, state["look"][2]))))


def pitch_toward(state, point):
    head = state["head"]
    v = [point[i] - head[i] for i in range(3)]
    return math.degrees(math.atan2(v[2], math.hypot(v[0], v[1])))


def pitch_at(game, point, steps=100):
    """vertical-only aiming (the first two steps lock yaw)"""
    sign = 1
    for _ in range(steps):
        s = game.settle()
        d = pitch_toward(s, point) - pitch(s)
        if abs(d) < 0.5:
            return True
        before = pitch(s)
        game.press(("ry+" if d * sign > 0 else "ry-") + ("100" if abs(d) > 3 else "60"), 1)
        moved = pitch(game.settle()) - before
        if abs(moved) > 0.1 and (moved > 0) != (d > 0):
            sign = -sign
    return False


def main():
    work = sys.argv[1] if len(sys.argv) > 1 else None
    with Game(menu_script=MENU_TO_CAMPAIGN + ",5600:a:8", work=work) as game:
        game.wait_frame(6400)  # past the intro cinematic (A skips it), the tutorial running
        ok = True
        for name, point in (("top", TOP), ("bottom", BOTTOM)):
            ok &= pitch_at(game, point)
            game.wait(300)
            print(name, "frame", game.frame, "snapshot", game.snapshot()[0], flush=True)
        errors = []
        for side in SIDES:
            # the game checks either side light (within 5 degrees, from the
            # head); hold each a few seconds, as a player would
            errors.append(game.aim_at(side, tolerance_degrees=2.5, max_steps=60, hold=90))
            print("side light", side, "error %.2f degrees, frame %d" % (errors[-1], game.frame),
                  "snapshot", game.snapshot()[0], flush=True)
        ok &= min(errors) < 2.5
        game.wait(900)
        print("snapshot:", game.snapshot()[0])
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
