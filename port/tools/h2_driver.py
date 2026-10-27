#!/usr/bin/env python3
"""Drive a running game from a script and watch it: a playtesting harness.

    from h2_driver import Game
    with Game(start_map="01b_spacestation") as game:   # hidden, silent, fresh profile
        game.wait_frame(5000)
        game.press("a", 8)                 # any input-script button: a, rt, ly+, rx-25, ...
        game.snapshot("before_walk")       # saves the game frame and the window as BMP
        print(game.state)                  # the player's unit: pos, vel, look (live)

The game runs with H2_CONTROL (commands appended to a file, read each frame)
and H2_TRACE=state (one line per frame with the player's position, velocity
and look direction, in thousandths); this module tails its log. Run from the
repository root. One game at a time.
"""
import math
import os
import re
import subprocess
import tempfile
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
STATE = re.compile(r"h2-state frame (\d+) unit (\w+) pos (-?\d+) (-?\d+) (-?\d+) vel (-?\d+) (-?\d+) (-?\d+) "
                   r"look (-?\d+) (-?\d+) (-?\d+)(?: head (-?\d+) (-?\d+) (-?\d+))?(?: fwd (-?\d+) (-?\d+) (-?\d+))?(?: ground (-?\d+))?(?: leaf (-?\d+) cluster (-?\d+))?")
FRAME = re.compile(r"h2-state frame (\d+) none")
EXTRA = re.compile(r" (lights|flashlight|health|shield|bsp) (-?\d+)")
SCRIPT = re.compile(r"h2-script frame (\d+) thread (\d+) script (\S+) sleep (-?\d+) frames(.*)")
SNAP = re.compile(r"h2-port: snapshot \"?([^\"\n]+)\"?")
MENU_TO_CAMPAIGN = "1300:start:8,1700:down:8,1850:a:8,2300:a:8,2900:a:8,3300:a:8,3700:a:8,4100:a:8"
# main menu -> Split Screen -> pregame lobby -> Start Game (Slayer; H2_START_MAP
# picks the multiplayer map); the game is running by frame ~4100
MENU_TO_SPLIT_SCREEN = "1300:start:8,1700:down:8,1850:a:8,2300:a:8,2800:down:8,2870:a:8,3070:a:8,3270:a:8"


class Game:
    def __init__(self, start_map=None, visible=False, audio=False, work=None, menu_script=MENU_TO_CAMPAIGN,
                 extra_env=None):
        self.work = Path(work or tempfile.mkdtemp(prefix="h2-play-"))
        self.work.mkdir(parents=True, exist_ok=True)
        self.control = self.work / "control"
        self.control.write_text("")
        self.log_path = self.work / "game.log"
        self.snap_dir = self.work / "snapshots"
        env = dict(os.environ, H2_CONTROL=str(self.control), H2_TRACE="state", H2_SNAPSHOT_DIR=str(self.snap_dir),
                   H2_INPUT_SCRIPT=menu_script)
        if start_map:
            env["H2_START_MAP"] = start_map
        if not visible:
            env["H2_WINDOW_HIDDEN"] = "1"
        if not audio:
            env["H2_AUDIO"] = "off"
        env.update(extra_env or {})
        self.log = open(self.log_path, "w")
        self.process = subprocess.Popen(
            [str(ROOT / "build/release/port/halo2-port"), "--data-root", str(ROOT / "orig/game"),
             "--profile-root", str(self.work / "profiles"),
             # H2_DRIVER_IMAGE: another guest image (e.g. a stack-protected build)
             "--guest-image", os.environ.get("H2_DRIVER_IMAGE", str(ROOT / "build/release/port/whole/whole-image.elf")),
             "--guest-entry", "h2_whole_game_main"],
            cwd=ROOT, env=env, stdout=self.log, stderr=subprocess.STDOUT)
        self._offset = 0
        self.frame = 0
        self.state = None
        self.history = []
        self.snapshots = []
        self._script_frame = None
        self.scripts = {}   # thread index -> latest h2-script state (H2_TRACE=state, every 150 frames)

    # -- plumbing ---------------------------------------------------------
    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()

    def close(self):
        if self.process.poll() is None:
            self._command("quit")
            try:
                self.process.wait(timeout=10)
            except subprocess.TimeoutExpired:
                self.process.kill()
        self.log.close()

    def _command(self, line):
        with open(self.control, "a") as f:
            f.write(line + "\n")

    def poll(self):
        """read new log lines; returns False when the game has exited"""
        with open(self.log_path, errors="replace") as f:
            f.seek(self._offset)
            data = f.read()
        end = data.rfind("\n")
        if end >= 0:
            self._offset += len(data[:end + 1].encode(errors="replace"))
            for line in data[:end].splitlines():
                m = STATE.search(line)
                if m:
                    v = [int(x) for x in m.groups()[2:] if x is not None]
                    self.frame = int(m.group(1))
                    self.state = {"frame": self.frame, "unit": m.group(2),
                                  "pos": [x / 1000 for x in v[0:3]], "vel": [x / 1000 for x in v[3:6]],
                                  "look": [x / 1000 for x in v[6:9]],
                                  "head": [x / 1000 for x in v[9:12]] if len(v) >= 12 else None,
                                  "forward": [x / 1000 for x in v[12:15]] if len(v) >= 15 else None,
                                  # the ground's height under the unit (None: no ground within 50 units)
                                  "ground": (v[15] / 1000 if v[15] != -999999 else None) if len(v) >= 16 else None,
                                  # the structure bsp leaf and cluster (-1: outside the world)
                                  "leaf": v[16] if len(v) >= 18 else None, "cluster": v[17] if len(v) >= 18 else None}
                    # the light count, flashlight power and body vitality (0: dead)
                    for name, value in EXTRA.findall(line):
                        self.state[name] = int(value) if name in ("lights", "bsp") else int(value) / 1000
                    self.history.append(self.state)
                t = SCRIPT.search(line)
                if t:
                    # each script thread's latest state: its script and the
                    # syntax nodes it is in, innermost first (hs_decompile.py --node);
                    # each report lists every thread, so a new one replaces the last
                    if int(t.group(1)) != self._script_frame:
                        self._script_frame = int(t.group(1))
                        self.scripts = {}
                    self.scripts[int(t.group(2))] = {"script": t.group(3), "sleep": int(t.group(4)),
                                                     "nodes": [x.split("/")[1] for x in t.group(5).split()]}
                f = FRAME.search(line)
                if f:
                    self.frame = int(f.group(1))
                s = SNAP.search(line)
                if s:
                    self.snapshots.append(s.group(1))
                if "fault signal" in line:
                    self.fault = line
        return self.process.poll() is None

    def wait_frame(self, frame, timeout=900):
        """waits until the game has presented `frame` frames (the state line's
        count; before the player exists, the host's own count from the log)"""
        start = time.time()
        while time.time() - start < timeout:
            if not self.poll():
                raise RuntimeError(f"game exited at frame {self.frame}: see {self.log_path}")
            if self.frame >= frame:
                return self.state
            time.sleep(0.05)
        raise TimeoutError(f"frame {frame} not reached (at {self.frame})")

    def wait(self, frames):
        return self.wait_frame(self.frame + frames)

    # -- actions ----------------------------------------------------------
    def press(self, button, frames=1):
        self._command(f"press {button} {frames}")

    def guest(self, command):
        """a test hook in the guest (port/runtime/guest_test_state.cpp):
        "teleport X Y Z", "deathless 1", "kill_enemies RADIUS" """
        with open(self.control, "a") as f:
            f.write("guest " + command + "\n")

    def snapshot(self, wait=True):
        before = len(self.snapshots)
        self._command("snapshot")
        if wait:
            start = time.time()
            while len(self.snapshots) < before + 2 and time.time() - start < 30:
                self.poll()
                time.sleep(0.05)
        return self.snapshots[before:]

    def settle(self, frames=4, timeout=60):
        """waits until the player's head and look have held still for `frames`
        frames (turning plays a turn-in-place animation that swings the head
        by over 10 cm for a while); returns the state"""
        start = self.frame
        s = self.wait(1)
        while self.frame - start < timeout:
            recent = self.history[-frames:]
            if len(recent) == frames and all(h["head"] == recent[0]["head"] and h["look"] == recent[0]["look"]
                                             for h in recent):
                return recent[-1]
            s = self.wait(1)
        return s

    # -- closed-loop aiming -------------------------------------------------
    def aim_at(self, point, tolerance_degrees=1.0, max_steps=80, hold=0):
        """turns the player's look toward a world point, seen from the head
        marker (as the game's own "looking at" checks are), with short
        right-stick taps sized to the error, measuring after the head has
        settled each time. Right stick x+ turns clockwise; y+ looks up unless
        the look is inverted, which is learned from the first pitch taps (the
        Armory tutorial tries inverted look). With `hold`, keeps correcting
        until `hold` frames have passed within tolerance. Returns the final
        angle error in degrees."""
        pitch_sign = 1
        error = 180.0
        held_since = None

        def tap(degrees):
            magnitude = abs(degrees)
            if magnitude > 12:
                return 100, 3
            if magnitude > 5:
                return 100, 1
            return (70 if magnitude > 2.5 else 55), 1

        for _ in range(max_steps + hold):
            s = self.settle()
            eye = s["head"] or [s["pos"][0], s["pos"][1], s["pos"][2] + 0.61]
            to = [point[i] - eye[i] for i in range(3)]
            length = math.sqrt(sum(c * c for c in to)) or 1.0
            to = [c / length for c in to]
            look = s["look"]
            yaw_error = math.degrees((math.atan2(to[1], to[0]) - math.atan2(look[1], look[0]) + math.pi)
                                     % (2 * math.pi) - math.pi)
            pitch_error = math.degrees(math.asin(max(-1, min(1, to[2]))) - math.asin(max(-1, min(1, look[2]))))
            error = math.degrees(math.acos(max(-1, min(1, sum(a * b for a, b in zip(to, look))))))
            if os.environ.get("H2_AIM_DEBUG"):
                print("aim", s["frame"], round(error, 2), round(yaw_error, 2), round(pitch_error, 2), pitch_sign,
                      flush=True)
            if error < tolerance_degrees:
                held_since = held_since or s["frame"]
                if s["frame"] - held_since >= hold:
                    return error
                continue
            held_since = None
            if abs(yaw_error) >= abs(pitch_error):
                strength, frames = tap(yaw_error)
                self.press(("rx-" if yaw_error > 0 else "rx+") + str(strength), frames)
            else:
                before = s["look"][2]
                strength, frames = tap(pitch_error)
                self.press(("ry+" if pitch_error * pitch_sign > 0 else "ry-") + str(strength), frames)
                after = self.settle()["look"][2]
                if abs(after - before) > 0.002 and (after > before) != (pitch_error > 0):
                    pitch_sign = -pitch_sign
                elif abs(after - before) <= 0.002 and abs(before) > 0.98:
                    # pinned at the pitch limit: the tap pushed into it (the
                    # tutorial can switch the look inversion mid-way)
                    pitch_sign = -pitch_sign
        return error

    # -- walking --------------------------------------------------------------
    def face(self, yaw_degrees, tolerance_degrees=2.0, max_steps=60):
        """turns (level) to a compass yaw: 0 along +x, 90 along +y. Right
        stick x+ turns clockwise (yaw down). Returns the final error."""
        error = 180.0
        for _ in range(max_steps):
            s = self.wait(2)
            yaw = math.degrees(math.atan2(s["look"][1], s["look"][0]))
            error = (yaw_degrees - yaw + 180) % 360 - 180
            if abs(error) < tolerance_degrees:
                break
            frames = 1 if abs(error) < 20 else 3
            strength = 100 if abs(error) > 10 else 70 if abs(error) > 4 else 55
            self.press(("rx-" if error > 0 else "rx+") + str(strength), frames)
            self.wait(frames + 1)
        return error

    def go_to(self, point, tolerance=0.25, max_steps=200):
        """walks to a point on the floor: faces it, then walks in short bursts,
        re-aiming as it goes. Returns the remaining horizontal distance."""
        distance = None
        for _ in range(max_steps):
            s = self.wait(1)
            dx, dy = point[0] - s["pos"][0], point[1] - s["pos"][1]
            distance = math.hypot(dx, dy)
            if distance < tolerance:
                break
            self.face(math.degrees(math.atan2(dy, dx)), tolerance_degrees=5.0)
            self.press("ly+", max(2, min(20, int(distance / 0.06))))  # about 0.06 units a frame
            self.wait(max(2, min(20, int(distance / 0.06))) + 2)
        return distance

