"""Reads an MSVC linker map: its symbols (public and static), the extent of
each section, and the relative fields /MAPINFO:FIXUPS lists.

FIXUPS lines give one absolute RVA, then signed 32-bit deltas; each field
is the previous one plus the delta.
"""
import bisect
import re
from dataclasses import dataclass

SYMBOL = re.compile(r'^\s*([0-9a-f]{4}):([0-9a-f]{8})\s+(\S+)\s+([0-9a-f]{8})\b')
SECTION = re.compile(r'^\s*([0-9a-f]{4}):([0-9a-f]{8})\s+([0-9a-f]{8})H\s+\S+\s+\S+\s*$')
BASE = re.compile(r'Preferred load address is ([0-9a-f]{8})')
VCALL = re.compile(r'^\?\?_9.*?@\$B(\d|[A-P]+@)')  # a vcall thunk and its vtable offset
# after the name's '@@': the access code of a this-adjustor thunk (G/H private, O/P protected, W/X public)
# and the adjustment, then the this qualifier and the calling convention
ADJUSTOR = re.compile(r'^[GHOPWX](\d|[A-P]+@)[A-D][A-I]')


@dataclass(frozen=True)
class MapSymbol:
    name: str
    va: int
    section: int
    static: bool
    object: str = ''  # the map's Lib:Object column: 'object.obj', or 'library:object.obj'
    function: bool = False  # the map's 'f' flag: code, not data


def _number(encoded):
    """An MSVC-encoded number: a digit d is d + 1, else hex digits A-P ended by '@'."""
    if encoded.isdigit():
        return int(encoded) + 1
    return int(''.join('0123456789abcdef'[ord(c) - ord('A')] for c in encoded.rstrip('@')), 16)


def plain_name(decorated):
    """A decorated name without its decoration: 'name' or 'class::name'. The
    compiler's vcall thunk (??_9) is "`vcall'{<vtable offset>}"; its
    this-adjustor thunk for a member reached through a base at a nonzero
    offset is the member's plain name followed by "`adjustor{<offset>}'"."""
    if decorated.startswith('?') and not decorated.startswith('??_9'):
        rest = decorated.split('@@', 1)[1] if '@@' in decorated else ''
        if m := ADJUSTOR.match(rest):
            return plain_name(decorated[:-len(rest)] + 'U' + rest[m.end(1):]) + adjustor_suffix(_number(m.group(1)))
    if m := VCALL.match(decorated):
        return f"`vcall'{{{_number(m.group(1)):#x}}}"
    if decorated[:4] in ('??_G', '??_E'):  # scalar or vector deleting destructor
        parts = decorated[4:].split('@@', 1)[0].split('@')
        return '::'.join(reversed(parts)) + "::`deleting destructor'"
    if decorated[:3] in ('??0', '??1'):  # constructor, destructor
        parts = decorated[3:].split('@@', 1)[0].split('@')
        return '::'.join(reversed(parts)) + '::' + ('~' if decorated[2] == '1' else '') + parts[0]
    if decorated.startswith('?'):
        parts = decorated[1:].split('@@', 1)[0].split('@')
        return '::'.join(reversed(parts))
    name = decorated.lstrip('_@')
    return name.split('@', 1)[0]


def adjustor_suffix(offset):
    """What a this-adjustor thunk's plain name adds to its member's."""
    return f"`adjustor{{{offset:#x}}}'"


class LinkMap:
    def __init__(self, text):
        self.base = 0
        self.symbols = []
        self.rel_fixups = set()
        lengths = {}  # section -> end offset
        starts = {}  # section -> start va, from any symbol in it (va - offset)
        static = False
        for line in text.splitlines():
            m = BASE.search(line)
            if m:
                self.base = int(m.group(1), 16)
                continue
            if line.strip() == 'Static symbols':
                static = True
                continue
            if line.startswith('FIXUPS:'):
                rva = 0
                for delta in line.split()[1:]:
                    rva = (rva + int(delta, 16)) & 0xFFFFFFFF
                    self.rel_fixups.add(self.base + rva)
                continue
            m = SECTION.match(line)
            if m:
                section, start, length = int(m.group(1), 16), int(m.group(2), 16), int(m.group(3), 16)
                lengths[section] = max(lengths.get(section, 0), start + length)
                continue
            m = SYMBOL.match(line)
            if m and int(m.group(1), 16) != 0:
                section, offset, va = int(m.group(1), 16), int(m.group(2), 16), int(m.group(4), 16)
                words = line.split()
                self.symbols.append(MapSymbol(m.group(3), va, section, static, words[-1], 'f' in words[3:-1]))
                starts.setdefault(section, va - offset)
        self._ends = {s: starts[s] + lengths[s] for s in starts if s in lengths}
        self._by_plain = {}
        self._vas = {}  # section -> sorted symbol addresses
        self._by_address = sorted(self.symbols, key=lambda s: s.va)
        self._addresses = [s.va for s in self._by_address]
        for s in self.symbols:
            self._by_plain.setdefault(plain_name(s.name), []).append(s)
            self._vas.setdefault(s.section, []).append(s.va)
        for vas in self._vas.values():
            vas.sort()

    @classmethod
    def read(cls, path):
        with open(path, encoding='latin-1') as f:
            return cls(f.read())

    def section_end(self, section):
        return self._ends[section]

    def extent(self, symbol):
        """[start, end): up to the next symbol in its section, or the section's end."""
        vas = self._vas[symbol.section]
        i = bisect.bisect_right(vas, symbol.va)
        return symbol.va, vas[i] if i < len(vas) else self.section_end(symbol.section)

    def symbol_at(self, va):
        """The symbol whose extent contains va (normally va is its start), else None."""
        i = bisect.bisect_right(self._addresses, va)
        if i == 0:
            return None
        symbol = self._by_address[i - 1]
        return symbol if va < self.extent(symbol)[1] else None

    def symbols_at(self, va):
        """Every symbol starting where symbol_at(va) does: identical functions
        the linker folded into one body share its address."""
        symbol = self.symbol_at(va)
        if symbol is None:
            return []
        lo = bisect.bisect_left(self._addresses, symbol.va)
        return self._by_address[lo:bisect.bisect_right(self._addresses, symbol.va)]

    def around(self, symbol, count):
        """Up to count symbols before symbol's address, then up to count after it, each nearest first, in
        its section."""
        lo = bisect.bisect_left(self._addresses, symbol.va)
        hi = bisect.bisect_right(self._addresses, symbol.va)
        nearby = self._by_address[max(lo - count, 0):lo][::-1] + self._by_address[hi:hi + count]
        return [s for s in nearby if s.section == symbol.section]

    def find(self, plain):
        """The symbols with this plain name, one per address (the compiler
        aliases ??_G and ??_E, for instance)."""
        return list({s.va: s for s in reversed(self._by_plain.get(plain, ()))}.values())[::-1]
