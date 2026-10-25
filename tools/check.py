"""Builds the project (tools/build.py) and compares every function marked
"// @retail 0x..." with the retail XBE, byte for byte apart from address
fields. Our extent comes from the linker map, retail's from
config/functions.csv. The masked fields are exactly those our linker filled
in: base relocations (absolute) and the relative fields /MAPINFO:FIXUPS lists.
Each masked field must also be the same kind of field in retail: an absolute
one holds an address inside the retail image, a relative one leaves the
function.

Writes build/report.json, updates the status and source columns of
config/functions.csv, and prints a summary. Each report entry's targets also
say whether the address fields that leave the function lead to the same
function, global or constant as retail's (Targets), and a last line totals
them over the functions whose bytes match; in one whose bytes differ, the
fields need not line up with retail's. A function whose bytes match but one
of whose fields differs does not match (settle_statuses); an unverified field
does not count. Fields are settled across the whole run, so statuses are
final only then.

Every function that does not match also gets a difference score: its
differing instructions plus the difference in length, in bytes (0 =
matched). It is printed on the DIFF line and stored in the report entry, so
progress on a near miss shows before it matches; --scores lists the
unmatched functions of the last full check with their scores, as CSV.

--fast checks the listed functions on a reduced image instead (see
tools/reduced.py): one image holding each function's own source, the sources
of its callees in retail all the way down, those of the functions and globals
its body uses by name, and what the last full build (build/halo2.exe and
build/halo2.map) says pins their calling conventions and which library
objects it takes. Only those sources are compiled, and the link leaves out
the rest of the program, so a check takes seconds, not minutes.
The comparison and its output are the full check's, though fields are
settled across the functions it checks only. A CAVEAT line follows a
function when the reduced image may build it differently from the full one.
--fast writes build/report-fast.json and nothing else: the csv and
build/report.json only ever come from a full build, which stays the judge of
what is kept. The image is linked as build/fast/reduced.img, never as an .exe,
and is reused when nothing it was linked from has changed. --jobs compiles
that many sources at once (default 2).

    python tools/check.py [<retail va> ...] [--no-build]
    python tools/check.py --fast <retail va> ... [--jobs N]
"""
import argparse
import json
import os
import re
import sys
import time
from collections import Counter

from capstone import CS_ARCH_X86, CS_MODE_32, Cs

import build
import reduced
from inventory import HAVOK, check_retail, in_scope_owner, read_rows, write_rows
from linkmap import _number, plain_name
from xbe import FUNCTIONS_CSV, ROOT, Xbe, retail_xbe_path

NEAR = 2  # differing instructions, at most, for "near"


# A stand-in in another source that copy-constructs a derived class emits the
# base's implicit copy constructor, with the marked constructor's plain name.
COPY_CONSTRUCTOR = '@@QAE@ABV0@@Z'


# MSVC's codes for the scalar parameter types, by the names the sources use.
SCALAR_CODES = {
    'char': 'D', 'signed char': 'C', 'unsigned char': 'E', 'byte': 'E', 'bool': '_N',
    'short': 'F', 'unsigned short': 'G', 'word': 'G', 'int': 'H', 'unsigned int': 'I', 'unsigned': 'I',
    'long': 'J', 'unsigned long': 'K', 'dword': 'K', 'float': 'M', 'real': 'M', 'double': 'N',
}


def _constructor_parameters(decorated):
    """The parameter codes of a decorated constructor ('??0c@@QAE@JG@Z' -> 'JG'), or None."""
    m = re.match(r'^\?\?0.*?@@Q[A-D]E@(.*)@Z$', decorated)
    return m.group(1) if m else None


def find_marked(linkmap, marked):
    """The image symbols with a marked function's plain name. A marked
    constructor that is not itself a copy constructor never means the class's
    copy constructor, so that one is left out when it makes the name ambiguous.
    Overloaded constructors whose parameters are all scalars are told apart by
    their decorated parameter codes."""
    hits = linkmap.find(marked.name)
    own_class = marked.cls.rpartition('::')[2]
    is_copy = (len(marked.params) == 1 and marked.params[0].endswith('&')
               and re.search(rf'\b{re.escape(own_class)}\b', marked.params[0]))
    if len(hits) > 1 and marked.kind == 'constructor' and not is_copy:
        hits = [h for h in hits if h.name != f'??0{own_class}{COPY_CONSTRUCTOR}'] or hits
    if len(hits) > 1 and marked.kind == 'constructor' and marked.params:
        codes = [SCALAR_CODES.get(' '.join(p.split())) for p in marked.params]
        if all(codes):
            hits = [h for h in hits if _constructor_parameters(h.name) == ''.join(codes)] or hits
    return hits


def resolve(linkmap, marked, standin_calls=None):
    """The image symbol of a marked function, or None when the linker left it
    out (folded into an identical function, or unreferenced)."""
    hits = find_overload(linkmap, marked, standin_calls)
    if not hits:
        return None
    if len(hits) > 1:
        raise SystemExit(f'{marked.path}: {marked.name} is ambiguous: ' + ', '.join(h.name for h in hits))
    return hits[0]


class StandinCalls:
    """What each marked function's stand-in (tools/build.py) calls or jumps to
    directly, read from the built image. A stand-in calls exactly the overload
    its marker names, whatever its parameter types."""

    def __init__(self, linkmap, image, marked):
        self.linkmap, self.image = linkmap, image
        self.names = build.standin_names(marked)
        self.md = Cs(CS_ARCH_X86, CS_MODE_32)

    def __call__(self, marked):
        """The direct call and jump targets of marked's stand-in (empty without one)."""
        symbols = self.linkmap.find(self.names.get((marked.path, marked.retail), ''))
        if len(symbols) != 1:
            return set()
        start, end = self.linkmap.extent(symbols[0])
        code = self.image.read(start, end - start)
        return {int(op_str, 16) for _, _, mnemonic, op_str in self.md.disasm_lite(code, start)
                if mnemonic in ('call', 'jmp') and op_str.startswith('0x')}


def find_overload(linkmap, marked, standin_calls=None):
    """find_marked's symbols; when they are overloads that its rules leave
    ambiguous (a pointer or class parameter, a method or a plain function),
    the one the marker's stand-in calls, given standin_calls (a StandinCalls)."""
    hits = find_marked(linkmap, marked)
    if len(hits) > 1 and standin_calls is not None:
        called = standin_calls(marked)
        hits = [h for h in hits if h.va in called] or hits
    return hits


def extract(full, theirs):
    """The code to compare: exactly retail's length when the rest of our
    extent is fill, otherwise the whole extent minus trailing fill."""
    if len(full) >= len(theirs) and set(full[len(theirs):]) <= {0xCC}:
        return full[:len(theirs)]
    return full.rstrip(b'\xcc')


def field_starts(va, size, fixups):
    """Offsets of the 4-byte fields lying wholly inside [va, va+size)."""
    return {f - va for f in fixups if va <= f and f + 4 <= va + size}


def masked_offsets(va, size, fixups):
    return {k + i for k in field_starts(va, size, fixups) for i in range(4)}


def straddling_offsets(va, size, fixups):
    """Offsets covered by fields that cross an edge of the function."""
    offsets = set()
    for f in fixups:
        if f < va + size and f + 4 > va and not (va <= f and f + 4 <= va + size):
            offsets.update(k for k in range(f - va, f - va + 4) if 0 <= k < size)
    return offsets


def _dword(data, k, signed=False):
    return int.from_bytes(data[k:k + 4], 'little', signed=signed)


def relative_ok(ours, start, theirs, theirs_va, k):
    """Whether the relative field at offset k agrees with retail. A target
    inside our function must be inside retail's at the same offset; any other
    must leave retail's function."""
    mine = start + k + 4 + _dword(ours, k, signed=True)
    target = theirs_va + k + 4 + _dword(theirs, k, signed=True)
    if start <= mine < start + len(ours):
        return target - theirs_va == mine - start
    return not theirs_va <= (target & 0xFFFFFFFF) < theirs_va + len(theirs)


def relative_targets(ours, start, theirs, theirs_va, k):
    """(our target, retail's target) of the relative field at offset k."""
    return ((start + k + 4 + _dword(ours, k, signed=True)) & 0xFFFFFFFF,
            (theirs_va + k + 4 + _dword(theirs, k, signed=True)) & 0xFFFFFFFF)


def same_callee(ours, start, theirs, theirs_va, k, identity):
    """Whether the relative field at offset k, if it leaves the function, reaches the same function as retail."""
    mine, target = relative_targets(ours, start, theirs, theirs_va, k)
    if identity is None or start <= mine < start + len(ours):
        return True
    return identity.ok(mine, target)


def points_inside(ours, start, theirs, theirs_va, k, both=False):
    """Whether the absolute field at offset k points inside its own function
    on either side (on both, given both)."""
    mine, value = _dword(ours, k), _dword(theirs, k)
    inside = start <= mine < start + len(ours), theirs_va <= value < theirs_va + len(theirs)
    return all(inside) if both else any(inside)


def absolute_ok(ours, start, theirs, theirs_va, k, lo, hi):
    """Whether the absolute field at offset k agrees with retail. When either
    side points inside its own function (a jump table entry) both must point
    at the same offset; otherwise retail's must be an address inside [lo, hi)."""
    mine, value = _dword(ours, k), _dword(theirs, k)
    if points_inside(ours, start, theirs, theirs_va, k):
        return value - theirs_va == mine - start
    return lo <= value < hi


def compare(ours, theirs, masked):
    for k in range(min(len(ours), len(theirs))):
        if k not in masked and ours[k] != theirs[k]:
            return k
    return None if len(ours) == len(theirs) else min(len(ours), len(theirs))


def _text(ins):
    return f'{ins.mnemonic} {ins.op_str}' if ins else ''


def _decode(md, data, va, k):
    """The instruction at offset k of data, or None past the end or where nothing decodes."""
    return next(md.disasm(data[k:k + 16], va + k, count=1), None)


def differing_instructions(ours, ours_va, theirs, theirs_va, masked, forced=frozenset(), entries=frozenset(), decoded=None):
    """Returns (count, lines, complete). complete is False when the
    disassembly does not cover every byte of both sides. entries: the offsets
    of absolute fields that point inside the function on both sides. No
    instruction starts with its own address field, so where both sides reach
    one of these at the same offset the bytes there are a jump table entry:
    that dword is compared as one line, by the same rule as the instructions,
    and decoding resumes after it. Entries that differ only by layout are
    masked, so they do not count, and the instructions after a table line up
    as they do in the image. decoded: our bytes as the disassembler reads
    them, when they differ from ours (the comparisons always use ours)."""
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    lines, count = [], 0
    oa = ob = 0
    stuck_a = stuck_b = False

    def same_bytes(k, size, y_bytes):
        return all((k + i) not in forced and ((k + i) in masked or ours[k + i] == y_bytes[i]) for i in range(size))

    while True:
        if not (stuck_a or stuck_b) and oa == ob and oa in entries and oa + 4 <= min(len(ours), len(theirs)):
            same = same_bytes(oa, 4, theirs[ob:ob + 4])
            x_text, y_text = f'dd {_dword(ours, oa):#x}', f'dd {_dword(theirs, ob):#x}'
            oa += 4
            ob += 4
        else:
            x = None if stuck_a else _decode(md, decoded or ours, ours_va, oa)
            y = None if stuck_b else _decode(md, theirs, theirs_va, ob)
            stuck_a, stuck_b = x is None, y is None
            if x is None and y is None:
                break
            same = x is not None and y is not None and x.size == y.size and same_bytes(oa, x.size, y.bytes)
            x_text, y_text = _text(x), _text(y)
            oa += x.size if x else 0
            ob += y.size if y else 0
        count += not same
        lines.append(f'  {"  " if same else "!!"} '
                     f'{x_text:<44} | {y_text}')
    complete = oa == len(ours) and ob == len(theirs)
    return count, lines, complete


# Library routines exported under two names; the linker resolves both to one function
ALIASES = {'chkstk': 'alloca_probe', 'memmove': 'memcpy'}

# MSVC's special names for the helpers the compiler generates itself, and the
# plain name a source in src/ gives the helper when it recreates retail's copy.
COMPILER_HELPERS = {'??_H': 'vector_constructor_iterator', '??_I': 'vector_destructor_iterator',
                    '??_L': 'eh_vector_constructor_iterator', '??_M': 'eh_vector_destructor_iterator'}


def compiler_helper(decorated):
    """The plain name src/ uses for the compiler helper a symbol is ('??_H@...'), else None."""
    return COMPILER_HELPERS.get(decorated[:4]) if decorated.startswith('??_') else None


class Identity:
    """Whether a call or jump to another function reaches the same function in
    our image and in retail. Our target is a map symbol, retail's a row of
    config/functions.csv; markers (@retail and @stub) tie the two together.
    A retail row's name, where known, comes from a library signature."""

    def __init__(self, linkmap, rows, markers, standin_calls=None):
        self.linkmap, self.rows = linkmap, rows
        self.address_of = {}  # our symbol's name -> the retail address its marker gives
        self.claimed = {m.retail: m.name for m in markers}  # retail address -> its marker's plain name
        for m in markers:
            hits = find_overload(linkmap, m, standin_calls)
            if len(hits) == 1:
                self.address_of[hits[0].name] = m.retail

    def ok(self, ours, theirs):
        return self.verdict(ours, theirs) != 'differs'

    def verdict(self, ours, theirs):
        """'verified' when markers or names show that ours, in our image, and
        theirs, in retail, are the same function; 'differs' when they show
        otherwise; 'unverified' when nothing tells (ours is unmarked and
        retail's row has no name), which ok lets pass."""
        # several names when the linker folded identical functions into one
        symbols = self.linkmap.symbols_at(ours)
        unmarked = [s for s in symbols if s.name not in self.address_of]
        if any(self.address_of[s.name] == theirs for s in symbols if s.name in self.address_of):
            return 'verified'
        if symbols and not unmarked:
            return 'differs'
        if theirs in self.claimed:
            # the compiler's own copy of a helper (??_H, ...) that src/ marks under its plain name
            # matches; otherwise src/ says another function is retail's target
            return 'verified' if any(compiler_helper(s.name) == self.claimed[theirs] for s in unmarked) else 'differs'
        name = self.rows.get(theirs, {}).get('name')
        if unmarked and name:
            theirs_name = plain_name(name)
            theirs_name = ALIASES.get(theirs_name, theirs_name)
            same = any(ALIASES.get(n, n) == theirs_name for n in (plain_name(s.name) for s in unmarked))
            return 'verified' if same else 'differs'
        return 'unverified'


# a global named for its retail address, by its plain name's last component
ADDRESSED_GLOBAL = re.compile(r'g_([0-9a-f]{5,8})')
# the compiler's literal-pool constants: a string literal, narrow (_0) or wide
# (_1), then its size in bytes; a floating-point constant, then its bytes in hex
STRING_LITERAL = re.compile(r'\?\?_C@_[01](\d|[A-P]+@)')
FLOAT_LITERAL = re.compile(r'__real@([0-9a-f]+)')
# a variable of scalar type: its storage class, type code and qualifier end its
# decorated name. A pointer's code (P, Q) also stands for an array, so its size is unknown.
SCALAR = re.compile(r'@[0-4](_N|[CDEFGHIJKMN])[A-D]$')
SCALAR_SIZES = {'C': 1, 'D': 1, 'E': 1, '_N': 1, 'F': 2, 'G': 2, 'H': 4, 'I': 4, 'J': 4, 'K': 4, 'M': 4, 'N': 8}
NEARBY = 8  # symbols on each side whose retail addresses may account for a field outside its own symbol


def literal_size(decorated):
    """The size in bytes of the literal-pool constant with this decorated name, else None."""
    if m := STRING_LITERAL.match(decorated):
        return _number(m.group(1))
    if m := FLOAT_LITERAL.fullmatch(decorated):
        return len(m.group(1)) // 2
    return None


def _label(symbol, offset):
    """A symbol and an offset into it, as a report lists them: a literal-pool constant by its decorated name."""
    name = symbol.name if literal_size(symbol.name) is not None else plain_name(symbol.name)
    return name + (f'{offset:+#x}' if offset else '')


def target_category(symbol):
    """What an unverified absolute field points at in our image, by its map symbol."""
    if symbol.name.startswith('??_7'):
        return 'vtable'
    if symbol.name.startswith('__imp_'):
        return 'import'
    if reduced.library_object(symbol.object):
        return 'library code' if symbol.function else 'library data'
    return 'unmarked function' if symbol.function else 'named global'


class Targets:
    """Whether the address fields that leave a function point at the same
    thing in our image and in retail. An absolute field holds an address
    inside one of our map symbols, at some offset into it:
    - a marked function (Identity.address_of): retail's field must hold the
      marker's address plus that offset;
    - a global named for its retail address (g_<hex>): that address plus the
      offset;
    - a literal-pool constant (literal_size): retail must hold the constant's
      bytes where its field points, less the offset.
    Each rule makes the field verified or different; any other symbol leaves
    it unverified, by category (target_category). A field that a rule finds
    different but that lies just past one of the NEARBY symbols before (a
    loop's end pointer), or just ahead of one after, is verified by that
    symbol's retail address instead. The map gives no sizes, and link-time
    code generation leaves static data out of it, so a symbol's extent there
    can run on over unnamed data: an offset at or past the size that a
    marker's row, a constant's name or a scalar's type gives is unverified
    ('past a symbol's end') unless retail agrees, and settle_targets decides
    an offset into a global of unknown size. A call or jump is as
    Identity.verdict says, an unverified one in the category 'call'. A
    differing field keeps a function whose bytes match from matching
    (settle_statuses); an unverified one does not."""

    def __init__(self, identity, image, retail):
        self.identity, self.image, self.retail = identity, image, retail

    def retail_address(self, symbol):
        """Where its marker or its name (g_<hex>) puts a symbol in retail, else None."""
        if symbol.name in self.identity.address_of:
            return self.identity.address_of[symbol.name]
        m = ADDRESSED_GLOBAL.fullmatch(plain_name(symbol.name).rpartition('::')[2])
        return int(m.group(1), 16) if m else None

    def size(self, symbol):
        """A symbol's size where its marker's row, its name or its type gives it, else None."""
        if symbol.name in self.identity.address_of:
            row = self.identity.rows.get(self.identity.address_of[symbol.name])
            return int(row['size']) if row else None
        if (size := literal_size(symbol.name)) is not None:
            return size
        m = SCALAR.search(symbol.name)
        return SCALAR_SIZES[m.group(1)] if m else None

    def same_bytes(self, literal, offset, value):
        """Whether retail holds a literal-pool constant's bytes where value points, less offset."""
        size = literal_size(literal.name)
        try:
            return self.retail.read(value - offset, size) == self.image.read(literal.va, size)
        except ValueError:  # in no section of retail
            return False

    def absolute(self, mine, value):
        """(verdict, category of an unverified field, our symbol, the retail
        address expected or None, reach) for an absolute field holding mine in
        our image and value in retail. reach is (symbol, offset) for a nonzero
        offset into a global of unknown size, for settle_targets, else None.
        A field the same distance from its symbol's retail address on both
        sides is verified, also past the symbol's end; so is one that a rule
        would find different but that lies the same distance from a nearby
        symbol's, such as a loop's end pointer past the array before."""
        symbols = self.identity.linkmap.symbols_at(mine)
        if not symbols:
            return 'unverified', 'no symbol', f'{mine:#x}', None, None
        offset = mine - symbols[0].va
        bases = {s.name: self.retail_address(s) for s in symbols}
        # identical functions the linker folded into one share an address: any of their markers may be retail's
        s = next((s for s in symbols if bases[s.name] is not None and bases[s.name] + offset == value), None)
        if s is not None:
            reach = (s.name, offset) if offset and self.size(s) is None else None
            return 'verified', None, _label(s, offset), value, reach
        s = next((s for s in symbols if bases[s.name] is None), symbols[0])
        name, base, size = _label(s, offset), bases[s.name], self.size(s)
        literal = literal_size(s.name) is not None
        if base is None and not literal:
            return 'unverified', target_category(s), name, None, None
        if literal and offset < size and self.same_bytes(s, offset, value):
            return 'verified', None, name, None, None
        for other in self.identity.linkmap.around(s, NEARBY):
            other_base = self.retail_address(other)
            start, end = self.identity.linkmap.extent(other)
            # a member of the element just past the end, or just ahead of the start, lies within one element of
            # it, and an element is no larger than the whole extent
            near = 2 * start - end <= mine < 2 * end - start
            if other_base is not None and near and other_base + mine - other.va == value:
                return 'verified', None, _label(other, mine - other.va), value, None
        if size is not None and offset >= size:
            return 'unverified', "past a symbol's end", name, None, None
        if literal:
            return 'differs', None, name, None, None
        return 'differs', None, name, base + offset, (s.name, offset) if offset and size is None else None

    def call(self, mine, target):
        """absolute's tuple for a call or jump to mine in our image and to target in retail."""
        verdict = self.identity.verdict(mine, target)
        symbols = self.identity.linkmap.symbols_at(mine)
        expected = next((self.identity.address_of[s.name] for s in symbols if s.name in self.identity.address_of), None)
        name = plain_name(symbols[0].name) if symbols else f'{mine:#x}'
        return verdict, 'call' if verdict == 'unverified' else None, name, expected, None


def function_targets(full, start, theirs, theirs_va, absolute, relative, targets):
    """Where the address fields that leave a function lead (see Targets): a
    list of (offset, retail's value or target, verdict, category, our symbol,
    expected retail address, reach), which settle_targets totals. Jump table
    entries, which check_function compares, and jumps within the function are
    left out. Arguments as check_function's, with targets a Targets."""
    ours = extract(full, theirs)
    size = min(len(ours), len(theirs))
    found = []
    for k in sorted(field_starts(start, size, absolute)):
        if not points_inside(ours, start, theirs, theirs_va, k):
            found.append((k, _dword(theirs, k), *targets.absolute(_dword(ours, k), _dword(theirs, k))))
    for k in sorted(field_starts(start, size, relative)):
        mine, target = relative_targets(ours, start, theirs, theirs_va, k)
        if not start <= mine < start + len(ours):
            found.append((k, target, *targets.call(mine, target)))
    return sorted(found)


DIFFERING_LISTED = 8  # differing fields a report entry lists


def settle_targets(entries):
    """Replaces the field list (function_targets) of each report entry that
    has one with its targets: how many fields are verified, differ or are
    unverified, the unverified ones by category, and the first differing
    fields. A differing nonzero offset into a global of unknown size stays a
    difference only when a verified field among the entries reaches at least
    as far into that global; otherwise it may lie past the global's end, in
    data the map does not name, and is unverified ('aggregate offset')."""
    reached = {}
    for e in entries:
        for *_, verdict, _, _, _, reach in e.get('targets', ()):
            if verdict == 'verified' and reach:
                reached[reach[0]] = max(reached.get(reach[0], 0), reach[1])
    for e in entries:
        if 'targets' not in e:
            continue
        fields = []
        for k, value, verdict, category, name, expected, reach in e['targets']:
            if verdict == 'differs' and reach and reach[1] > reached.get(reach[0], 0):
                verdict, category = 'unverified', 'aggregate offset'
            fields.append((k, value, verdict, category, name, expected))
        counts = Counter(verdict for _, _, verdict, *_ in fields)
        differing = [dict(offset=k, ours=name, expected=None if expected is None else f'{expected:#x}',
                          retail=f'{value:#x}') for k, value, verdict, _, name, expected in fields if verdict == 'differs']
        e['targets'] = dict(verified=counts['verified'], differs=counts['differs'], unverified=counts['unverified'],
                            categories=dict(Counter(category for _, _, _, category, *_ in fields if category)),
                            differing=differing[:DIFFERING_LISTED])


def awaits_targets(entry):
    """Whether a report entry's status waits for settle_statuses: its bytes
    match, but a field (function_targets) differs before settling."""
    return entry['status'] == 'matched' and any(verdict == 'differs' for _, _, verdict, *_ in entry['targets'])


def settle_statuses(report):
    """Settles the targets of the report's entries (settle_targets), then
    their statuses: a function whose bytes match but one of whose fields
    differs does not match. Each differing field counts as one differing
    instruction, so the function is near when at most NEAR differ and todo
    otherwise, and its first difference is its first differing field.
    Prints MATCH, or DIFF with a line for each differing field listed, for
    the entries awaits_targets held back. Returns the entries it demoted."""
    waiting = [key for key, e in report.items() if awaits_targets(e)]
    settle_targets(report.values())
    demoted = []
    for key in waiting:
        e = report[key]
        t = e['targets']
        if not t['differs']:
            print(f'MATCH  {key} {e["name"]}')
            continue
        e['status'] = 'near' if t['differs'] <= NEAR else 'todo'
        e['first_difference'] = t['differing'][0]['offset']
        e['score'] = t['differs']  # each differing field counts as one differing instruction
        print(f'DIFF   {key} {e["name"]} at +{e["first_difference"]:#x} ({e["ours"]} bytes, retail {e["size"]}), '
              f'score {e["score"]}')
        for d in t['differing']:
            expected = '' if d['expected'] is None else f' ({d["expected"]} in retail)'
            print(f'  !! address field at +{d["offset"]:#x}: ours leads to {d["ours"]}{expected}, '
                  f'retail\'s to {d["retail"]}')
        demoted.append(e)
    return demoted


def targets_summary(entries, demoted):
    """One line totalling the targets of the functions among report entries
    whose bytes match: those that match, and those settle_statuses demoted."""
    found = [e['targets'] for e in entries if e['status'] == 'matched' and 'targets' in e] + [
        e['targets'] for e in demoted]
    total = {key: sum(t[key] for t in found) for key in ('verified', 'differs', 'unverified')}
    categories = sum((Counter(t['categories']) for t in found), Counter())
    return (f'address targets in {len(found)} function{"" if len(found) == 1 else "s"} whose bytes match: '
            f'{total["verified"]} verified, {total["differs"]} differ, '
            f'{total["unverified"]} unverified (' + ', '.join(f'{n} {c}' for c, n in categories.most_common())
            + f'); {len(demoted)} demoted from matched for a differing field')


def check_function(full, start, theirs, theirs_va, absolute, relative, lo, hi, identity=None):
    """Compares one function. Returns (status, first difference or None,
    disassembly lines, length of our code). identity, if given, also checks
    that each call out of the function reaches the same function as retail."""
    ours = extract(full, theirs)
    # a field counts only when it lies wholly inside both extents
    size = min(len(ours), len(theirs))
    fixups = [*absolute, *relative]
    failed = {k for k in field_starts(start, size, absolute)
              if not absolute_ok(ours, start, theirs, theirs_va, k, lo, hi)}
    failed |= {k for k in field_starts(start, size, relative)
               if not relative_ok(ours, start, theirs, theirs_va, k)
               or not same_callee(ours, start, theirs, theirs_va, k, identity)}
    forced = straddling_offsets(start, size, fixups)
    for k in failed:
        forced.update(range(k, k + 4))
    masked = masked_offsets(start, size, fixups) - forced
    first = compare(ours, theirs, masked)
    offsets = [k for k in forced if k < size]
    if first is not None:
        offsets.append(first)
    if not offsets:
        return 'matched', None, [], len(ours)
    # absolute fields that point inside the function on both sides: jump table entries, and a jmp's table address
    entries = {k for k in field_starts(start, size, absolute) if points_inside(ours, start, theirs, theirs_va, k, True)}
    # decode our side with each field that points inside our function rebased to retail's address, so a
    # jump table that doesn't line up with retail's decodes the same wherever the link places the function
    decoded = bytearray(ours)
    for k in field_starts(start, len(ours), absolute):
        if start <= _dword(ours, k) < start + len(ours):
            decoded[k:k + 4] = (_dword(ours, k) - start + theirs_va).to_bytes(4, 'little')
    count, lines, complete = differing_instructions(ours, start, theirs, theirs_va, masked, forced, entries,
                                                    bytes(decoded))
    near = complete and len(ours) == len(theirs) and count <= NEAR
    return ('near' if near else 'todo'), min(offsets), lines, len(ours)


def difference_score(status, lines, length, retail_length):
    """How far a function is from retail: its differing instructions (the
    listing's !! lines) plus the difference in length, in bytes. 0 when it
    matches. Only reported; no status depends on it."""
    if status == 'matched':
        return 0
    return sum(1 for line in lines if line.startswith('  !!')) + abs(length - retail_length)


_within = reduced.fields_within  # the part of sorted fixups in [low, high)


def parse_addresses(texts):
    try:
        return {int(a, 16) for a in texts}
    except ValueError:
        bad = next(a for a in texts if not re.fullmatch(r'(0[xX])?[0-9a-fA-F]+', a))
        raise SystemExit(f'{bad!r} is not a hexadecimal retail address')


def check_unique_markers(marked):
    seen = {}
    for m in marked:
        if m.retail in seen:
            raise SystemExit(f'duplicate @retail {m.retail:#x}: {seen[m.retail].path} {seen[m.retail].name} '
                             f'and {m.path} {m.name}')
        seen[m.retail] = m


def write_report(path, report, merge):
    """Writes the report; a filtered run (merge) updates the entries it checked
    and keeps the rest of an existing report."""
    if merge and os.path.exists(path):
        with open(path, encoding='utf-8') as f:
            report = {**json.load(f), **report}
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w', encoding='utf-8') as f:
        json.dump(report, f, indent=1, sort_keys=True)


def check_one(m, row, ours, retail, identity, standin_calls, targets):
    """Compares the marked function m (row, its config/functions.csv row) with
    retail in a built image (ours, a reduced.Image), and prints MATCH, DIFF
    with a listing, or MISSING. Returns (its report entry, its symbol in the
    image or None). The entry of a function in the image also lists where its
    address fields lead (function_targets, given targets, a Targets), for
    settle_statuses, which prints MATCH or DIFF instead for a function whose
    status waits for its fields (awaits_targets)."""
    symbol = resolve(ours.linkmap, m, standin_calls)
    if symbol is None:
        print(f'MISSING {m.retail:08x} {m.name}: not in the image '
              '(the linker folded it into an identical function, or nothing references it)')
        return dict(name=m.name, source=m.path, status='todo', size=int(row['size']), ours=0, first_difference=0,
                    score=None), None
    start, end = ours.linkmap.extent(symbol)
    theirs = retail.read(m.retail, int(row['size']))
    window = (start - 3, end)  # every fixup that touches the function
    code = ours.image.read(start, end - start)
    absolute, relative = _within(ours.absolute, *window), _within(ours.relative, *window)
    status, first, lines, length = check_function(
        code, start, theirs, m.retail, absolute, relative,
        retail.base, max(s.va + s.vsize for s in retail.sections), identity)
    score = difference_score(status, lines, length, len(theirs))
    entry = dict(name=m.name, source=m.path, status=status, size=len(theirs), ours=length, first_difference=first,
                 score=score, targets=function_targets(code, start, theirs, m.retail, absolute, relative, targets))
    if first is not None:
        print(f'DIFF   {m.retail:08x} {m.name} at +{first:#x} ({length} bytes, retail {len(theirs)}), score {score}')
        print('\n'.join(lines))
    elif not awaits_targets(entry):
        print(f'MATCH  {m.retail:08x} {m.name}')
    return entry, symbol


FAST_IMAGE = os.path.join('build', 'fast', 'reduced.img')
FAST_PARSED = os.path.join('build', 'fast', 'parsed.pickle')  # build.load_parse_cache
FAST_REPORT = os.path.join('build', 'report-fast.json')
FAST_JOBS = 2  # sources --fast compiles at once
NO_FULL_BUILD = ('no full build (build/halo2.exe and build/halo2.map), so the reduced image leaves out the library '
                 'objects and the address-taking sources one would add; run python tools/check.py once')


def same_code(a, a_symbol, b, b_symbol):
    """Whether two images (reduced.Image) hold the same code for the two
    symbols: the same length and the same bytes, apart from address fields."""
    x, x_masked = a.code(a_symbol)
    y, y_masked = b.code(b_symbol)
    return compare(x, y, x_masked | y_masked) is None


def fast_caveats(ours, symbol, full, identity):
    """What may make a reduced image (ours, a reduced.Image) build the function
    at symbol differently from a full build: calls and references to globals
    that the reduced image leaves out (unresolved: the full build may inline
    such a function, or build differently around such a global), and code that
    differs from the last full build (full, a reduced.FullBuild, or None) for
    the function itself or a function it calls, where their sources and those
    of their own callees went into that build unchanged."""
    direct, calls = ours.callees(symbol)
    unresolved = ours.unresolved_globals(symbol)
    notes = []
    if calls or unresolved:
        what = [f'{n} {kind}{"" if n == 1 else "s"}' for n, kind in ((calls, 'call'), (unresolved, 'global reference'))
                if n]
        notes.append(f'{" and ".join(what)} reach{"es" if calls + unresolved == 1 else ""} what the reduced image '
                     'leaves out (unresolved); the full build may inline such a function, or build differently '
                     'around such a global')
    if full is None:
        return notes
    for s in [symbol, *direct]:
        twin = full.counterpart(s)
        if (twin is None or reduced.library_object(s.object)
                or not all(map(full.unchanged, [s, *(direct if s is symbol else ours.callees(s)[0])]))
                or same_code(ours, s, full, twin)):
            continue
        if s is symbol:
            notes.append('it builds differently here than in the last full build of the same source; '
                         'trust the full check for it')
        else:
            va = identity.address_of.get(s.name)
            name = f'{va:#x}' if va is not None else plain_name(s.name)
            notes.append(f'its callee {name} builds differently here than in the last full build '
                         '(link-time code generation outside the reduced image); trust the full check for it')
    return notes


def fast_check(wanted, retail, jobs, root=ROOT, started=None):
    """--fast: checks the functions marked at the retail addresses in wanted on
    one reduced image (see the module docstring), prints the full check's lines
    for each, with CAVEAT lines, and writes build/report-fast.json. Returns the
    exit status: 1 when one differs. started: when the run started
    (time.perf_counter), for the time line."""
    started = time.perf_counter() if started is None else started
    build.load_parse_cache(os.path.join(root, FAST_PARSED))
    rows = read_rows(os.path.join(root, 'config', 'functions.csv'))
    marked, stubs = build.marked_sources(root), build.stub_sources(root)
    markers = marked + stubs
    check_unique_markers(markers)
    unmarked = wanted - {m.retail for m in marked}
    if unmarked:
        print('no @retail marker in src/ for: ' + ', '.join(f'{a:#x}' for a in sorted(unmarked)))
        return 1
    targets = [m for m in marked if m.retail in wanted]
    for m in targets:
        if m.retail not in rows:
            raise SystemExit(f'{m.path}: @retail {m.retail:#x} is not a function start in config/functions.csv')
    units = set().union(*(reduced.reduced_units(m.retail, rows, markers) for m in targets))
    caveats = {m.retail: [] for m in targets}
    full, include = reduced.FullBuild.load(root), ()
    if full is None:
        print(f'CAVEAT {NO_FULL_BUILD}')
    else:
        include = reduced.library_symbols(full.linkmap)
        full_calls = StandinCalls(full.linkmap, full.image, marked)
        found = []
        for m in targets:
            hits = find_overload(full.linkmap, m, full_calls)
            if len(hits) == 1:
                found.append(hits[0])
            else:
                caveats[m.retail].append('not in the last full build, so the sources that take its address '
                                         'there are unknown')
        sources = build.source_objects(root)
        units |= reduced.pinning_units(full.linkmap, full.image, found, sources)
        units |= reduced.named_units(reduced.bodies(targets, root), full.linkmap, sources)
    timings = {}
    image_path = os.path.join(root, FAST_IMAGE)
    map_path = build.build(root, units=units, image=image_path, include=include, jobs=jobs, reuse=True,
                           timings=timings)
    built = time.perf_counter()
    build.save_parse_cache(os.path.join(root, FAST_PARSED))

    ours = reduced.Image.read(map_path, image_path)
    standin_calls = StandinCalls(ours.linkmap, ours.image, marked)
    identity = Identity(ours.linkmap, rows, markers, standin_calls)
    addresses = Targets(identity, ours.image, retail)
    report = {}
    for m in targets:
        entry, symbol = check_one(m, rows[m.retail], ours, retail, identity, standin_calls, addresses)
        notes = caveats[m.retail] + (fast_caveats(ours, symbol, full, identity) if symbol else [])
        for note in notes:
            print(f'CAVEAT {m.retail:08x} {m.name}: {note}')
        report[f'{m.retail:08x}'] = dict(entry, caveats=([NO_FULL_BUILD] if full is None else []) + notes)
    demoted = settle_statuses(report)
    failed = sum(e['first_difference'] is not None for e in report.values())
    write_report(os.path.join(root, FAST_REPORT), report, merge=False)
    done = time.perf_counter()

    print(f'fast check: {len(targets) - failed} of {len(targets)} match on a reduced image of {len(units)} sources; '
          'only the full check (python tools/check.py) updates config/functions.csv')
    print(targets_summary(report.values(), demoted))
    link, compiled = timings['link'], timings['compiled']
    print(f'time {done - started:.1f}s: prepare {built - started - timings["compile"] - (link or 0):.1f}s, '
          f'compile {compiled} source{"" if compiled == 1 else "s"} {timings["compile"]:.1f}s, '
          f'link {"kept from the last run" if link is None else f"{link:.1f}s"}, compare {done - built:.1f}s')
    return 1 if failed else 0


def print_scores(path):
    """--scores: the unmatched functions of the last full check, as CSV (va,
    score, size, ours, status, source), in address order. A function the
    image lacks (MISSING) has an empty score."""
    if not os.path.exists(path):
        raise SystemExit(f'{path} not found: run the full check (python tools/check.py) first')
    with open(path, encoding='utf-8') as f:
        report = json.load(f)
    print('va,score,size,ours,status,source')
    for va, e in sorted(report.items()):
        if e.get('status') != 'matched':
            score = e.get('score')
            print(f"{va},{'' if score is None else score},{e.get('size')},{e.get('ours')},{e.get('status')},"
                  f"{e.get('source', '')}")
    return 0


def main():
    # `@file` reads arguments one per line, so long address lists stay off the
    # command line (antivirus heuristics flag long runs of hex arguments)
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0], fromfile_prefix_chars='@')
    ap.add_argument('addresses', nargs='*')
    ap.add_argument('--no-build', action='store_true')
    ap.add_argument('--fast', action='store_true',
                    help='check the listed functions on a reduced image; writes only build/report-fast.json')
    ap.add_argument('--jobs', type=int, default=None, help=f'with --fast: sources compiled at once (default {FAST_JOBS})')
    ap.add_argument('--scores', action='store_true',
                    help='print a CSV of the unmatched functions in build/report.json with their difference scores')
    args = ap.parse_args()
    if args.scores:
        sys.exit(print_scores(os.path.join(ROOT, 'build', 'report.json')))
    started = time.perf_counter()
    if args.fast and not args.addresses:
        ap.error('--fast checks the functions it is given: list their retail addresses')
    if args.fast and args.no_build:
        ap.error('--fast builds its own reduced image; --no-build is for the full check')
    if args.jobs is not None and (not args.fast or args.jobs < 1):
        ap.error('--jobs takes a positive count, and needs --fast')
    retail_path = retail_xbe_path()
    if not os.path.exists(retail_path):
        raise SystemExit(f'retail XBE not found at {retail_path}: put it at orig/default.xbe or set RETAIL_XBE')
    check_retail(retail_path)
    wanted = parse_addresses(args.addresses)
    if args.fast:
        sys.exit(fast_check(wanted, Xbe(retail_path), args.jobs or FAST_JOBS, started=started))
    map_path = os.path.join(ROOT, 'build', build.MAP_NAME)
    exe_path = os.path.join(ROOT, 'build', build.EXE_NAME)
    if args.no_build:
        if not os.path.exists(map_path) or not os.path.exists(exe_path):
            raise SystemExit('--no-build needs a previous build (build/halo2.exe and build/halo2.map); run without it first')
    else:
        map_path = build.build()

    ours = reduced.Image.read(map_path, exe_path)
    retail = Xbe(retail_path)
    rows = read_rows(FUNCTIONS_CSV)

    marked = build.marked_sources()
    stubs = build.stub_sources()
    check_unique_markers(marked + stubs)
    standin_calls = StandinCalls(ours.linkmap, ours.image, marked)
    identity = Identity(ours.linkmap, rows, marked + stubs, standin_calls)
    addresses = Targets(identity, ours.image, retail)
    unmarked = wanted - {m.retail for m in marked}
    if unmarked:
        print('no @retail marker in src/ for: ' + ', '.join(f'{a:#x}' for a in sorted(unmarked)))
        sys.exit(1)
    report, claimed = {}, set()
    for m in marked:
        if wanted and m.retail not in wanted:
            continue
        row = rows.get(m.retail)
        if row is None:
            raise SystemExit(f'{m.path}: @retail {m.retail:#x} is not a function start in config/functions.csv')
        entry, _ = check_one(m, row, ours, retail, identity, standin_calls, addresses)
        claimed.add(m.retail)
        row['source'] = m.path
        report[f'{m.retail:08x}'] = entry
    # fields are settled across the whole run, so only now are the statuses final
    demoted = settle_statuses(report)
    for va in claimed:
        rows[va]['status'] = report[f'{va:08x}']['status']
    failed = sum(e['first_difference'] is not None for e in report.values())
    if not wanted:
        for va, r in rows.items():
            if r['source'] and va not in claimed:
                r['source'], r['status'] = '', 'todo'
    write_rows(FUNCTIONS_CSV, list(rows.values()))
    write_report(os.path.join(ROOT, 'build', 'report.json'), report, merge=bool(wanted))

    def summary(label, subset):
        done = [r for r in subset if r['status'] == 'matched']
        total, matched = sum(int(r['size']) for r in subset), sum(int(r['size']) for r in done)
        print(f'matched {len(done)} of {len(subset)} {label} '
              f'({matched} of {total} bytes, {100 * matched / max(total, 1):.2f}%)')

    summary('game functions', [r for r in rows.values() if r['owner'] == 'game'])
    summary('Havok functions', [r for r in rows.values() if r['owner'] == HAVOK])
    summary('functions in scope', [r for r in rows.values() if in_scope_owner(r['owner'])])
    print(targets_summary(report.values(), demoted))
    sys.exit(1 if failed else 0)


if __name__ == '__main__':
    main()
