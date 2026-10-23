"""Writes config/functions.csv: every function in the retail XBE's code, with
who owns it (game, an Xbox SDK library, or third-party code), how it was
compiled (for speed or for size), and what it calls. Rerunning keeps each
row's source and status, and seeds function discovery with the existing rows,
so their start addresses stay.

    python tools/inventory.py [--xbe orig/default.xbe] [--xdk sdk/xbox] [--out config/functions.csv]

A library function's name comes from its signature in the contributor's own
SDK libraries (tools/libsig.py); other rows have no name. The object column is
kept empty.
"""
import argparse
import csv
import hashlib
import json
import os
import re
from collections import Counter

from capstone import CS_ARCH_X86, CS_MODE_32, Cs

import libsig
from functions import discover
from xbe import FUNCTIONS_CSV, ROOT, Xbe, retail_xbe_path, xdk_dir

RETAIL_SHA256 = '03215919bb7163259257d361f4c7bf802a7ab12aa85e2689436369b5c427935d'
OWNERS_JSON = os.path.join(ROOT, 'config', 'owners.json')
COLUMNS = ['va', 'size', 'owner', 'style', 'evidence', 'name', 'object', 'calls', 'source', 'status']
# code sections (XBE section flags mark data sections executable too, so go by name)
CODE_SECTIONS = {'.text', 'D3D', 'XPP', 'DSOUND', 'WMADEC', 'XONLINE', 'XNET'}
SECTION_OWNERS = {'D3D': 'xdk:d3d8', 'XPP': 'xdk:xapi', 'DSOUND': 'xdk:dsound', 'WMADEC': 'xdk:wmadec',
                  'XONLINE': 'xdk:xonline', 'XNET': 'xdk:xnet'}


def check_retail(path):
    with open(path, 'rb') as f:
        digest = hashlib.sha256(f.read()).hexdigest()
    if digest != RETAIL_SHA256:
        raise SystemExit(f'{path} is not the retail XBE this project matches '
                         f'(sha256 {digest}, expected {RETAIL_SHA256})')


def function_name(signature):
    """The function's name in the contributor's own SDK library, else ''."""
    return signature.name if signature else ''


def owner(section, lib_hit):
    """The owner the section or a library signature decides, else None."""
    if section.startswith('BINK'):
        return 'third:bink'
    if section != '.text':
        return SECTION_OWNERS.get(section, 'xdk:' + section.lower())
    if lib_hit:
        return 'xdk:' + lib_hit
    return None

# Addresses that look like function starts to discovery but are not, and that
# its rules cannot tell apart from real ones; a row seeds discovery, so such a
# start would otherwise stay. Each with its reason.
NOT_STARTS = {
    # inside 0x231ae9, right after its rep stosd; 16-aligned, and a few dwords in
    # the DOLBY section happen to equal it. No rule can drop it: a real function
    # can follow one that falls into it (retail 0x27b020 after 0x27ace0).
    0x231c00,
}

# Function starts that discovery cannot find: nothing calls these functions,
# and the function laid out just before each one reaches it only by a jmp, so
# tracing takes it for part of that function. A row seeds discovery, so each
# start stays once its row exists; the list keeps the retail evidence.
KNOWN_STARTS = {
    # 0x2382d6 jumps to it (at 0x23842b); it follows an unconditional jmp out
    # and opens by saving its own registers (push ebx ; push esi)
    0x238459: 'tail jump target of 0x2382d6',
    # 0x2b8da0 jumps to it (at 0x2b8dbf); it follows 0x2b8da0's jump table and
    # opens its own frame (sub esp, 0x6c) over its own stack arguments
    0x2b8e10: 'tail jump target of 0x2b8da0',
    # 0x2b557a ends `jmp 0x2b55ae ; ... ; ret`, and it opens a full frame
    # (push ebp ; lea ebp, [esp - 0x78] ; sub esp, 0x844)
    0x2b55ae: 'tail jump target of 0x2b557a',
}

# Data inside code sections: [start, end) ranges that hold no functions, each with
# its evidence. Discovery takes their bytes for code (they are reached by nothing,
# so the gaps pass fill them), so the rows it finds there are dropped.
DATA_RANGES = {
    # WMADEC's tables, from after 0x392447 (ends `ret 0x14` at 0x39250c) to the end
    # of the section. The decoder's code reads them as data, never as code: for
    # example `mov edx, dword ptr [edi*4 + 0x392830]` at 0x38d2be, `mov esi,
    # 0x39259c` at 0x388da4 (one of a run of 16-byte strided table rows) and
    # `push 0x3929b8` at 0x390027; no call or jump from code lands inside, and
    # decoded as code the bytes are full of retf, iretd, into, in and out.
    (0x392510, 0x3a061c): 'WMADEC tables',
    # DSOUND's constants, tables and variables, from after 0x385614 (ends `ret 4`
    # at 0x385743) to the end of the section. Its code reads and writes them, for
    # example `cmp dword ptr [0x386b0c], 0` at 0x37a154, `inc dword ptr
    # [0x386b34]` at 0x379cd8 and `fmul qword ptr [0x385760]` at 0x37a058, and
    # calls through a pointer table in them (`call dword ptr [0x3871cc]` at
    # 0x379dc0); no direct call or jump lands inside.
    (0x385748, 0x387384): 'DSOUND data',
    # XNET's tables, from after 0x3dffcc (ends `ret 0x10` at 0x3e0127, then int3
    # padding) to the end of the section: runs of 0xff, a zero-filled block and
    # byte tables, read by `cmp esi, dword ptr [0x3e02e8]` at 0x3d9907 and `push
    # 0x3e0190` at 0x3a4165; no call or jump lands inside.
    (0x3e0130, 0x3e0400): 'XNET tables',
    # D3D's tables, from after 0x403400 (ends `ret 8` at 0x40395e, then its jump
    # table to 0x4039cc and int3 padding) to the end of the section's raw data.
    # Read by `cmp eax, dword ptr [ecx*4 + 0x4039e8]` at 0x3fd840 and `fmul dword
    # ptr [eax*4 + 0x404238]` at 0x3fd756, and called through as pointers (`call
    # dword ptr [0x403af8]` at 0x3f937e); no direct call or jump lands inside.
    (0x4039d0, 0x404dc4): 'D3D tables',
    # XPP: the dword 0xff and zero padding in front of 0x408940 (push ebp ;
    # mov ebp, esp); nothing refers to it
    (0x408933, 0x408940): 'padding before 0x408940',
}


def in_data(va):
    """Whether va lies in one of DATA_RANGES."""
    return any(start <= va < end for start, end in DATA_RANGES)


EH_MAX_SIZE = 32
MIN_HANDLER_THUNKS = 8


def _jmp_target(instructions):
    """The immediate target of a trailing jmp in a stub of at most EH_MAX_SIZE bytes, else None."""
    if not 2 <= len(instructions) <= 6 or sum(i.size for i in instructions) > EH_MAX_SIZE:
        return None
    last = instructions[-1]
    if last.mnemonic != 'jmp' or not last.op_str.startswith('0x'):
        return None
    if any(i.mnemonic.startswith('ret') or (i.mnemonic == 'push' and i.op_str == 'ebp') for i in instructions):
        return None
    return int(last.op_str, 16)


def eh_thunk_target(instructions):
    """T for exactly `mov eax, imm32; jmp T` (a candidate __ehhandler thunk), else None."""
    target = _jmp_target(instructions)
    if (target is not None and len(instructions) == 2 and instructions[0].mnemonic == 'mov'
            and re.fullmatch(r'eax, 0x[0-9a-f]+', instructions[0].op_str)):
        return target
    return None


def is_eh_stub(instructions, handlers):
    """True for the compiler's exception-handling stubs, which belong to a parent
    function: an __ehhandler thunk (`mov eax, imm32; jmp T`, T in handlers) or an
    unwind funclet (starts with an ebp-relative lea/mov, then lea/mov/add on
    ecx/eax only, then jmp imm). `add ecx, N; jmp` (a this-adjustor) is not one."""
    if eh_thunk_target(instructions) in handlers:
        return True
    if _jmp_target(instructions) is None:
        return False
    body = instructions[:-1]
    first = body[0]
    if first.mnemonic not in ('lea', 'mov') or '[ebp' not in first.op_str:
        return False
    return all(i.mnemonic in ('lea', 'mov', 'add') and i.op_str.split(',')[0] in ('ecx', 'eax') for i in body)


def fill_from_neighbours(rows):
    """Settles rows whose owner is None (rows in address order): a gap between two
    decided rows with the same non-game owner takes it, anything else is game.
    'eh' rows are left alone and are transparent when looking for neighbours."""
    decided = [None if r['owner'] == 'eh' else r['owner'] for r in rows]
    following = [None] * len(rows)
    nxt = None
    for i in range(len(rows) - 1, -1, -1):
        following[i] = nxt
        nxt = decided[i] if decided[i] is not None else nxt
    prev = None
    for i, row in enumerate(rows):
        if decided[i] is not None:
            prev = decided[i]
        elif row['owner'] is None:
            row['owner'] = prev if prev and prev == following[i] and prev != 'game' else 'game'


def load_owners(path):
    """config/owners.json: where Bungie's code ends, and explicit owner ranges ({} if absent)."""
    if not os.path.exists(path):
        return {}
    with open(path, encoding='utf-8') as f:
        return json.load(f)


def library_hits(lib_hits, game_end):
    """The library signature hits in .text at or above game_end. The SDK's
    libraries link after the game's objects, so a hit below it is a small game
    function whose bytes happen to equal a library function's (retail 0x22ec84
    matches CTcpSocket::HasConnectedChild, 0x22cced std::locale::facet's
    deleting destructor); it says nothing about owner or name."""
    return {va: sig for va, sig in lib_hits.items() if va >= game_end}


def apply_owners(rows, owners, text_range):
    """Applies config/owners.json last: .text rows ('game' only) at or above
    game_end become 'other:library', then each explicit range sets its owner."""
    if 'game_end' in owners:
        end, (low, high) = int(owners['game_end'], 16), text_range
        for row in rows:
            if row['owner'] == 'game' and max(end, low) <= int(row['va'], 16) < high:
                row['owner'] = 'other:library'
    for r in owners.get('ranges', []):
        start, stop = int(r['start'], 16), int(r['end'], 16)
        for row in rows:
            if start <= int(row['va'], 16) < stop:
                row['owner'] = r['owner']


def style(before, start, first):
    """('speed' | 'size' | 'unknown', evidence). before: the bytes just before
    the function at start; first: its first two instructions."""
    a16 = start % 16 == 0
    pad = before[-1:] == b'\xcc'
    ebp = (len(first) >= 2 and first[0].mnemonic == 'push' and first[0].op_str == 'ebp'
           and first[1].mnemonic == 'mov' and first[1].op_str == 'ebp, esp')
    evidence = [name for name, on in (('a16', a16), ('pad', pad), ('ebp', ebp), ('packed', not a16 and not pad)) if on]
    kind = 'speed' if a16 and pad else 'size' if not a16 and not pad else 'unknown'
    return kind, ' '.join(evidence)


HAVOK = 'third:havok'


def in_scope_owner(owner):
    """Owners the decompilation covers: the game, the SDK libraries it was
    linked with, and Havok, whose physics must stay exactly as retail. The
    exception-handling stubs and the other third-party libraries (Bink, which
    a port replaces) are left out."""
    return owner != 'eh' and (not owner.startswith('third:') or owner == HAVOK)


def read_rows(path):
    if not os.path.exists(path):
        return {}
    with open(path, newline='', encoding='utf-8') as f:
        return {int(r['va'], 16): r for r in csv.DictReader(f)}


def write_rows(path, rows):
    os.makedirs(os.path.dirname(path) or '.', exist_ok=True)
    with open(path, 'w', newline='', encoding='utf-8') as f:
        w = csv.DictWriter(f, COLUMNS, lineterminator='\n')
        w.writeheader()
        w.writerows(rows)


def check_unique(rows):
    seen = set()
    for row in rows:
        if row['va'] in seen:
            raise SystemExit(f"duplicate inventory row for {row['va']}")
        seen.add(row['va'])


def merge(new_rows, old_rows):
    for row in new_rows:
        old = old_rows.get(int(row['va'], 16))
        if old:
            row['source'], row['status'] = old['source'], old['status']
    return new_rows


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('--xbe', default=retail_xbe_path())
    ap.add_argument('--xdk', default=xdk_dir())
    ap.add_argument('--out', default=FUNCTIONS_CSV)
    ap.add_argument('--owners', default=OWNERS_JSON)
    args = ap.parse_args()

    check_retail(args.xbe)
    image = Xbe(args.xbe)
    old_rows = read_rows(args.out)
    libraries = [os.path.join(args.xdk, 'lib', name + '.lib') for name in libsig.COFF_LIBRARIES]
    if not any(os.path.exists(p) for p in libraries):
        raise SystemExit(f'no Xbox SDK libraries found (looked for {libraries[0]} and others); '
                         'set XDK_DIR or pass --xdk')
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True

    owners = load_owners(args.owners)
    game_end = int(owners.get('game_end', '0'), 16)
    rows = []
    stubs = {}
    for section in image.sections:
        if section.name not in CODE_SECTIONS and not (section.name.startswith('BINK') and section.name != 'BINKDATA'):
            continue
        seeds = {va for va in old_rows.keys() | KNOWN_STARTS.keys() if not in_data(va)}
        found = discover(image, seeds=seeds, text=section, not_starts=NOT_STARTS)
        found = {va: fn for va, fn in found.items() if not in_data(va)}
        lib_hits = {}
        if section.name == '.text':
            lib_hits = library_hits(
                libsig.find_in_dir(os.path.join(args.xdk, 'lib'), image.section_bytes(section), section.va), game_end)
        for fn in found.values():
            before = image.read(fn.start - 1, 1) if fn.start > section.va else b''
            first = list(md.disasm(image.read(fn.start, 16), fn.start, 2))
            kind, evidence = style(before, fn.start, first)
            signature = lib_hits.get(fn.start)
            who = owner(section.name, signature and signature.library)
            if who is None and fn.end - fn.start <= EH_MAX_SIZE:
                stubs[fn.start] = list(md.disasm(image.read(fn.start, fn.end - fn.start), fn.start))
            rows.append(dict(
                va=f'{fn.start:08x}', size=str(fn.end - fn.start), owner=who,
                style=kind, evidence=evidence, name=function_name(signature), object='',
                calls=' '.join(f'{c:08x}' for c in sorted(fn.calls | fn.tail_jumps)),
                source='', status='todo'))
    thunk_targets = Counter(t for t in map(eh_thunk_target, stubs.values()) if t is not None)
    handlers = {t for t, n in thunk_targets.items() if n >= MIN_HANDLER_THUNKS}
    for row in rows:
        if row['owner'] is None and is_eh_stub(stubs.get(int(row['va'], 16), []), handlers):
            row['owner'] = 'eh'
    rows.sort(key=lambda r: r['va'])
    check_unique(rows)
    fill_from_neighbours(rows)
    text = image.section('.text')
    apply_owners(rows, owners, (text.va, text.va + text.vsize))
    write_rows(args.out, merge(rows, old_rows))
    counts = Counter(r['owner'] for r in rows)
    print('frame handlers:', ' '.join(f'{h:08x}' for h in sorted(handlers)))
    print(f'{len(rows)} functions:', ', '.join(f'{k} {v}' for k, v in sorted(counts.items())))


if __name__ == '__main__':
    main()
