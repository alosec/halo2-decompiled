"""Writes an objdiff progress report for ByteMatch from config/functions.csv.

ByteMatch (bytematch.dev, formerly decomp.dev) charts a project from an objdiff
report (report.proto version 2)
uploaded as a GitHub Actions artifact named ``<version>_report``. This repo's
checker does not emit that file: tools/check.py writes build/report.json as a
map of retail addresses, and it needs orig/default.xbe. The committed inventory
already has each function's address, size and status (matched / near / todo),
so this tool builds the objdiff report from the CSV alone.

Matched code is bytes whose CSV status is ``matched`` (the same rule as
check.py's summary). ``near`` is not a full match. check.py does not record how
many bytes of a near function matched, so those functions contribute their full
size to the total and nothing to matched code, with fuzzy match 0. That is a
stand-in until a live ``python tools/check.py`` pass can supply per-function
match bytes. Data is not compared at all; the data totals stay 0.

Functions are named ``function_<va>``. A unit is a source file; functions
without one are grouped by 64 KB address range (``asm/0x1e0000``). The csv's
``name`` and ``object`` columns are not read.

    python tools/decomp_dev_export.py [-o build/decomp.dev.json]
        [--scope game|in-scope|all] [--csv config/functions.csv]
        [--check-report build/report.json]

``--scope game`` is the README headline (owner ``game``). ``in-scope`` is
check.py's last line: everything except ``eh`` and the third-party libraries
other than Havok (``inventory.in_scope_owner``). ``--check-report``
overlays statuses from check.py's build/report.json when one is already on disk.
It does not read the XBE and does not invent fuzzy-match bytes.
"""
import argparse
import json
import os
import re
import sys
from collections import Counter, defaultdict

from inventory import in_scope_owner, read_rows
from xbe import FUNCTIONS_CSV

# objdiff objdiff_core::bindings::report::REPORT_VERSION
REPORT_VERSION = 2
SCOPES = ('game', 'in-scope', 'all')
ASM_RANGE = 0x10000  # functions without a source file, grouped by address
STATUSES = ('matched', 'near', 'todo')

CATEGORY_NAMES = {
    'with-source': 'Source file present',
    'no-source': 'No source yet',
    'game': 'Game code',
    'xdk': 'Xbox SDK',
    'third': 'Third-party',
    'eh': 'Exception-handling stubs',
    'other': 'Other libraries',
}


def in_scope(row, scope):
    """The rows check.py's summaries count, plus ``all`` for the whole CSV."""
    owner = row.get('owner') or ''
    if scope == 'game':
        return owner == 'game'
    if scope == 'in-scope':
        return in_scope_owner(owner)
    if scope == 'all':
        return True
    raise SystemExit(f'unknown scope {scope!r} (expected {"|".join(SCOPES)})')


def owner_bucket(owner):
    """A progress category id for an inventory owner."""
    owner = owner or 'unknown'
    if owner in ('game', 'eh'):
        return owner
    if ':' in owner:
        return owner.split(':', 1)[0]
    return owner


def unit_base(row):
    """Translation-unit name: the source file, else its 64 KB address range."""
    source = (row.get('source') or '').replace('\\', '/')
    if source:
        return source
    return f"asm/{int(row['va'], 16) & ~(ASM_RANGE - 1):#08x}"


def _status(row):
    status = row.get('status') or 'todo'
    if status not in STATUSES:
        raise SystemExit(f"{row.get('va', '?')}: unknown status {status!r}")
    return status


def _size(row):
    try:
        size = int(row['size'])
    except (KeyError, TypeError, ValueError):
        raise SystemExit(f"{row.get('va', '?')}: size {row.get('size')!r} is not an integer")
    if size < 0:
        raise SystemExit(f"{row['va']}: negative size")
    return size


def overlay_check_report(rows, check_report):
    """Copy rows, replacing status (and source, when present) from check.py's report.

    Returns (rows, overlaid, unknown). ``unknown`` is the number of report
    entries whose address is not in the CSV; they are skipped (no size to use).
    """
    out, overlaid, unknown = {}, 0, 0
    seen = set()
    for va, row in rows.items():
        row = dict(row)
        hit = check_report.get(va)
        if hit:
            row['status'] = _status(hit)
            if hit.get('source'):
                row['source'] = hit['source']
            overlaid += 1
            seen.add(va)
        out[va] = row
    unknown = sum(1 for va in check_report if va not in seen)
    return out, overlaid, unknown


def load_check_report(path):
    """check.py's build/report.json: ``{"<va hex>": {"status", "source", ...}}``."""
    with open(path, encoding='utf-8') as f:
        data = json.load(f)
    if not isinstance(data, dict) or 'measures' in data or 'units' in data:
        raise SystemExit(f'{path} is not a check.py report (expected a map of retail addresses)')
    out = {}
    for key, entry in data.items():
        if not isinstance(entry, dict) or not re.fullmatch(r'[0-9a-fA-F]{1,8}', str(key)):
            raise SystemExit(f'{path}: {key!r} is not a check.py report entry')
        _status(entry)
        out[int(key, 16)] = entry
    return out


def _pct(part, whole):
    """A percentage, with an empty total as 0%: objdiff's rule makes 0/0 100%,
    which would show an unchecked total (such as data) as fully matched."""
    return 0.0 if whole == 0 else 100.0 * part / whole


def _measures(functions, complete):
    """One objdiff Measures object. uint64 byte counts are JSON strings."""
    total_code = sum(fn['size'] for fn in functions)
    matched_code = sum(fn['size'] for fn in functions if fn['status'] == 'matched')
    total_functions = len(functions)
    matched_functions = sum(1 for fn in functions if fn['status'] == 'matched')
    # No per-function match length is stored for near/todo, so fuzzy == matched.
    complete_code = total_code if complete else 0
    return {
        'fuzzy_match_percent': _pct(matched_code, total_code),
        'total_code': str(total_code),
        'matched_code': str(matched_code),
        'matched_code_percent': _pct(matched_code, total_code),
        'total_data': '0',
        'matched_data': '0',
        'matched_data_percent': _pct(0, 0),
        'total_functions': total_functions,
        'matched_functions': matched_functions,
        'matched_functions_percent': _pct(matched_functions, total_functions),
        'complete_code': str(complete_code),
        'complete_code_percent': _pct(complete_code, total_code),
        'complete_data': '0',
        'complete_data_percent': _pct(0, 0),
        'total_units': 1,
        'complete_units': 1 if complete else 0,
    }


def _function_item(row):
    va = int(row['va'], 16)
    return {
        'name': f'function_{va:x}',
        'size': str(_size(row)),
        'fuzzy_match_percent': 100.0 if _status(row) == 'matched' else 0.0,
        'metadata': {'virtual_address': str(va)},
    }


def _add_measures(total, part):
    """Sum already-finished Measures. Fuzzy is re-derived from matched bytes."""
    def u(field):
        return int(total[field]) + int(part[field])

    total_code, matched_code = u('total_code'), u('matched_code')
    total_functions = total['total_functions'] + part['total_functions']
    matched_functions = total['matched_functions'] + part['matched_functions']
    complete_code = u('complete_code')
    total['fuzzy_match_percent'] = _pct(matched_code, total_code)
    total['total_code'] = str(total_code)
    total['matched_code'] = str(matched_code)
    total['matched_code_percent'] = _pct(matched_code, total_code)
    total['total_functions'] = total_functions
    total['matched_functions'] = matched_functions
    total['matched_functions_percent'] = _pct(matched_functions, total_functions)
    total['complete_code'] = str(complete_code)
    total['complete_code_percent'] = _pct(complete_code, total_code)
    total['total_units'] += part['total_units']
    total['complete_units'] += part['complete_units']


def _empty_measures():
    made = _measures([], complete=False)
    made['total_units'] = 0
    return made


def build_report(rows, scope='game', check_report=None):
    """An objdiff report dict, and a small info dict for the stderr note.

    ``rows`` is ``inventory.read_rows``'s ``{va: row}`` map (not mutated).
    """
    info = {'scope': scope, 'overlaid': 0, 'unknown_check_entries': 0,
            'matched': 0, 'near': 0, 'todo': 0}
    if check_report:
        rows, info['overlaid'], info['unknown_check_entries'] = overlay_check_report(rows, check_report)
    selected = []
    for row in rows.values():
        if not in_scope(row, scope):
            continue
        status = _status(row)
        selected.append(dict(row, status=status, size=_size(row)))
        info[status] += 1

    groups = defaultdict(list)
    for row in selected:
        bucket = owner_bucket(row.get('owner') or '')
        groups[(unit_base(row), bucket)].append(row)
    base_counts = Counter(base for base, _bucket in groups)

    units = []
    categories = {}
    total = _empty_measures()
    for (base, bucket), fns in sorted(groups.items(), key=lambda item: (item[0][0], item[0][1])):
        fns.sort(key=lambda r: int(r['va'], 16))
        name = base if base_counts[base] == 1 else f'{base} [{bucket}]'
        complete = bool(fns) and all(fn['status'] == 'matched' for fn in fns)
        measures = _measures(fns, complete)
        has_source = any(fn.get('source') for fn in fns)
        progress = ['with-source' if has_source else 'no-source']
        if scope != 'game':
            progress.append(bucket)
        metadata = {'complete': complete, 'progress_categories': progress}
        source = next((fn['source'].replace('\\', '/') for fn in fns if fn.get('source')), '')
        if source:
            metadata['source_path'] = source
        units.append({
            'name': name,
            'measures': measures,
            'functions': [_function_item(fn) for fn in fns],
            'metadata': metadata,
        })
        _add_measures(total, measures)
        for category_id in progress:
            slot = categories.get(category_id)
            if slot is None:
                slot = {
                    'id': category_id,
                    'name': CATEGORY_NAMES.get(category_id, category_id),
                    'measures': _empty_measures(),
                }
                categories[category_id] = slot
            _add_measures(slot['measures'], measures)

    report = {
        'measures': total,
        'units': units,
        'version': REPORT_VERSION,
        'categories': [categories[k] for k in sorted(categories)],
    }
    return report, info


def _note(info, check_path):
    lines = [
        f"scope {info['scope']}: matched {info['matched']}, near {info['near']}, "
        f"todo {info['todo']}",
        'needs a live check.py pass before this is worth uploading: statuses come from',
        'config/functions.csv (the last committed check), and near/todo functions have',
        'fuzzy match 0 because check.py does not record how many bytes matched.',
        'data is not compared: the data totals and percentages are 0.',
    ]
    if check_path:
        lines.append(
            f"overlaid {info['overlaid']} status(es) from {check_path}"
            + (f"; {info['unknown_check_entries']} report address(es) were not in the CSV"
               if info['unknown_check_entries'] else ''))
    return '\n'.join(lines)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('-o', '--output', default='-', help='report path, or - for stdout (default)')
    ap.add_argument('--scope', choices=SCOPES, default='game')
    ap.add_argument('--csv', default=FUNCTIONS_CSV, help='function inventory (default config/functions.csv)')
    ap.add_argument('--check-report', help="check.py's build/report.json, to overlay statuses")
    args = ap.parse_args(argv)
    if not os.path.exists(args.csv):
        raise SystemExit(f'function inventory not found at {args.csv}')
    rows = read_rows(args.csv)
    check_report = load_check_report(args.check_report) if args.check_report else None
    report, info = build_report(rows, args.scope, check_report)
    text = json.dumps(report, indent=2) + '\n'
    if args.output == '-':
        sys.stdout.write(text)
    else:
        os.makedirs(os.path.dirname(os.path.abspath(args.output)) or '.', exist_ok=True)
        with open(args.output, 'w', encoding='utf-8') as f:
            f.write(text)
    print(_note(info, args.check_report), file=sys.stderr)
    return 0


if __name__ == '__main__':
    sys.exit(main())
