#!/bin/sh
# Play with the session recorded: video (MP4), sound (WAV) and the per-frame
# player state log, all named by start time under ~/Movies/halo2-sessions.
# Usage: port/tools/play_recorded.sh [extra halo2-port arguments]
cd "$(dirname "$0")/../.." || exit 1
out="$HOME/Movies/halo2-sessions"
mkdir -p "$out"
stamp=$(date +%Y-%m-%d-%H%M%S)
echo "recording to $out/$stamp.{mp4,wav,log}"
# a control file (h2_driver's commands, e.g. "guest kill_enemies 30") so a
# stuck session can be helped along from outside the game
control="$out/control"
: > "$control"
H2_CONTROL="$control" H2_RECORD="$out/$stamp.mp4" H2_AUDIO_RECORD="$out/$stamp.wav" H2_TRACE=state \
  exec build/release/port/halo2-port --data-root orig/game \
  --guest-image build/release/port/whole/whole-image.elf \
  --guest-entry h2_whole_game_main "$@" > "$out/$stamp.log" 2>&1
