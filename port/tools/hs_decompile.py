#!/usr/bin/env python3
"""Prints a map's compiled level scripts back as Lisp-like source.

Usage: hs_decompile.py MAP [SCRIPT_NAME...] [--node HEX...]

Reads the scenario tag's script blocks (offsets in the Xbox cache layout):
+0x1b0 string data (size, address), +0x1b8 scripts (0x28 bytes: name[32],
type, return type, root node), +0x1c0 globals, +0x238 syntax nodes (20 bytes:
salt, function index / constant type, type, flags, next node, source
offset, value). A function call's value is its first child, the function's
name symbol; its arguments follow through the next links. Every node's
source offset points at its token in the string data, which keeps the
original names. --node prints the expression at those node indices (as
the game's h2-script trace reports them).
"""
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from h2_map import Map  # noqa: E402


class Scripts:
    def __init__(self, path):
        self.map = m = Map(path)
        scenario = next(t for t in m.tags if t[0] == 'scnr')
        base = m.file_offset(scenario[2])
        u = lambda o: struct.unpack_from('<II', m.data, base + o)
        size, address = u(0x1b0)
        self.strings = m.data[m.file_offset(address):m.file_offset(address) + size]
        count, address = u(0x1b8)
        self.scripts = []
        for i in range(count):
            o = m.file_offset(address) + i * 0x28
            name = m.data[o:o + 32].split(b'\0')[0].decode('latin1')
            kind, rtype, root = struct.unpack_from('<hhI', m.data, o + 32)
            self.scripts.append((name, kind, root))
        count, address = u(0x238)
        self.nodes = []
        for i in range(count):
            self.nodes.append(struct.unpack_from('<hhhBBIII', m.data, m.file_offset(address) + i * 20))

    def text(self, offset):
        if offset >= len(self.strings):
            return '?'
        end = self.strings.find(b'\0', offset)
        return self.strings[offset:end].decode('latin1')

    def expression(self, index, depth=0):
        if index == 0xffffffff or depth > 60:
            return '?'
        salt, function, type_, flags, _, next_, source, value = self.nodes[index & 0xffff]
        if flags & 1:   # a primitive: a constant or a global
            token = self.text(source)
            return token if token and token != '?' else str(value)
        first = value & 0xffff
        parts = []
        child = first
        while child != 0xffff and len(parts) < 64:
            node = self.nodes[child]
            parts.append(self.expression(child, depth + 1) if parts else self.text(node[6]))
            child = node[5] & 0xffff if node[5] != 0xffffffff else 0xffff
        return '(' + ' '.join(parts) + ')'

    def pretty(self, text, width=100):
        """breaks a long expression over lines by nesting depth"""
        if len(text) <= width:
            return text
        out, depth, line = [], 0, ''
        for ch in text:
            if ch == '(' and line.strip():
                out.append('  ' * depth + line.strip())
                line = ''
            line += ch
            depth += ch == '('
            depth -= ch == ')'
        out.append('  ' * max(depth, 0) + line.strip())
        return '\n'.join(out)


def main():
    args = sys.argv[1:]
    if not args:
        print(__doc__)
        return 2
    s = Scripts(args[0])
    if '--node' in args:
        for hexa in args[args.index('--node') + 1:]:
            print(hexa, s.expression(int(hexa, 16)))
        return 0
    names = args[1:]
    for name, kind, root in s.scripts:
        if names and name not in names:
            continue
        print(f'; {name} (type {kind})')
        print(s.pretty(s.expression(root)))
        print()
    return 0


if __name__ == '__main__':
    sys.exit(main())
