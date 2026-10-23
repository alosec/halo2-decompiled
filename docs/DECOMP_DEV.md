# ByteMatch progress report

[ByteMatch](https://bytematch.dev) charts a decompilation from an
[objdiff](https://github.com/encounter/objdiff) progress report. This
project's page is <https://bytematch.dev/kirklandsig/halo2-decompiled>.
ByteMatch replaced decomp.dev, the earlier site. The workflow, this file, the
exporter and its tests keep their decomp.dev names, so the CI job and its
artifact are unchanged.

The decomp.dev maintainer's note on [issue #1](https://github.com/kirklandsig/halo2-decompiled/issues/1#issuecomment-5960390856)
is that a listing would be welcome, and that we would have to export the
checker's results in that report format ourselves: the toolchain is MSVC with
link-time code generation building an Xbox XBE, so `objdiff-cli report
generate` does not apply.

The format is
[`objdiff-core/protos/report.proto`](https://github.com/encounter/objdiff/blob/main/objdiff-core/protos/report.proto)
(report version 2; see the
[decomp.wiki guide](https://decomp.wiki/tools/decomp-dev)). ByteMatch reads a
JSON report uploaded as a GitHub Actions artifact named `<version>_report`
(for example `retail_report`). Byte counts (`total_code`, `matched_code`, …)
are JSON strings; function counts are JSON numbers; percents are JSON numbers.
Projects that cannot run objdiff synthesise the same `measures` object. Fire
Emblem 8 (JP) does that in
[`scripts/gen-report.py`](https://github.com/laqieer/fireemblem8j/blob/main/scripts/gen-report.py).

## What this repo exports

`tools/check.py` writes `build/report.json`, but that file is a map of retail
addresses (`status`, `size`, `first_difference`), not an objdiff report, and
producing it needs `orig/default.xbe`. The committed inventory already records
every function's address, size and status.

```
python tools/decomp_dev_export.py -o build/decomp.dev.json
```

That reads `config/functions.csv` only. Default `--scope game` is the README
headline (owner `game`). `--scope in-scope` is the checker's second line:
everything except `eh` and `third:`. `--scope all` is the whole CSV.

| CSV status | In the report |
| --- | --- |
| `matched` | Full size counts as matched code. Fuzzy match 100. |
| `near` | Counted in the total only. Fuzzy match 0. |
| `todo` | Counted in the total only. Fuzzy match 0. |

`near` means the checker saw at most two differing instructions. It does not
record how many bytes matched, so this export cannot fill objdiff's
`fuzzy_match_percent` honestly for a near function. Those functions stay at 0
until a live check can supply per-function match bytes.

Data is not compared. `total_data` and `matched_data` are `"0"`, and the data
percentages are 0. objdiff's own rule would show an empty total as 100%, which
ByteMatch would display as a data match, so any empty total is reported as 0%.

Units are source files (`src/*.cpp` when the CSV has one), otherwise the
function's 64 KB address range (`asm/0x1e0000`). Functions are named
`function_<va>`; the export does not read the CSV's `name` or `object`
columns. Categories are `with-source` and `no-source`. Scopes other than `game` also split by owner bucket (`game`,
`xdk`, `third`, `eh`, `other`).

If a `build/report.json` from `tools/check.py` is already on disk, overlay its
statuses (the CSV size is kept; the XBE is not opened):

```
python tools/decomp_dev_export.py -o build/decomp.dev.json \
    --check-report build/report.json
```

The tool always prints a stderr note that a live `python tools/check.py` pass
is still required before uploading, so the statuses are the ones just checked.

`build/` is gitignored. Do not commit the report.

## Uploading

ByteMatch only sees a report that the default branch publishes as an artifact.
`.github/workflows/decomp-dev.yml` does that: on every push to `main` it runs
`python tools/decomp_dev_export.py -o build/report.json` (the exporter does not
need the XBE or the XDK, and the job holds no secrets) and uploads the file as
the artifact `retail_report`. Suggested version slug: `retail` (this repo's
disc build; `config/files.json` calls it `default`). The file inside the
artifact is named `report.json`; if ByteMatch expects another name, change
`-o` and `path` in the workflow.

The project is registered on ByteMatch. Without ByteMatch's GitHub app, the
site polls for new workflow runs every few minutes. A repo admin can force a
poll from the project's manage page. The workflow can also be started by hand
(`workflow_dispatch`).

The statuses in the report are the ones last committed to the CSV. If the
retail XBE ever becomes available to CI, run `python tools/check.py` in the
workflow first and pass `--check-report build/report.json`, so the listing is
not stuck on whatever statuses were last committed. Until `check.py` records a
matched-byte count for `near` functions, the fuzzy percent will keep equalling
the full-match percent.
