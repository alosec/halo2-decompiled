#!/usr/bin/env python3
"""Reads a retail Halo 2 Xbox map's tag index offline.

Usage: h2_map.py MAP [NAME_SUBSTRING...]
  lists tags (class, id, address, size, name) whose names contain any of the
  substrings, or all tags; for shaders it also names the template.

Layout (Xbox cache file, version 8):
  header +0x10 index offset, +0x14 index size; the index starts with
  (magic, class count, instances address, scenario, globals, ?, tag count, 'tags'),
  then class entries (12 bytes) and tag instances (class, id, address, size).
  Tag data follows the index in the file, so a tag address maps to
  address - (lowest tag address - (index offset + index size)).
  Names: count +0x2c0, strings +0x2c4, offset table +0x2cc.
  String ids: the offset table's address +0x16c, strings +0x170; an id is
  (length << 24) | index.
"""
import struct
import sys


class Map:
    def __init__(self, path):
        with open(path, 'rb') as f:
            self.data = f.read()
        d = self.data
        u = self.u32
        index = u(0x10)
        _, class_count, _, _, _, _, tag_count, _ = struct.unpack_from('<IIIIIII4s', d, index)
        instances = index + 32 + class_count * 12
        names_data, names_index, name_count = u(0x2c4), u(0x2cc), u(0x2c0)
        names = []
        for i in range(name_count):
            start = names_data + u(names_index + 4 * i)
            names.append(d[start:d.index(b'\0', start)].decode('latin1'))
        self.tags = []
        for i in range(tag_count):
            cls, tag_id, address, size = struct.unpack_from('<4sIII', d, instances + 16 * i)
            self.tags.append((cls[::-1].decode('latin1'), tag_id, address, size, names[i] if i < len(names) else '?'))
        lowest = min(t[2] for t in self.tags if t[2])
        self.base = lowest - (index + u(0x14))
        self.by_id = {t[1]: t for t in self.tags}

    def u32(self, offset):
        return struct.unpack_from('<I', self.data, offset)[0]

    def file_offset(self, address):
        return address - self.base

    def string_id(self, value):
        index = value & 0xffffff
        start = self.u32(0x170) + self.u32(self.u32(0x16c) + 4 * index)
        return self.data[start:self.data.index(b'\0', start)].decode('latin1')


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return 2
    m = Map(sys.argv[1])
    for cls, tag_id, address, size, name in m.tags:
        if len(sys.argv) > 2 and not any(k in name for k in sys.argv[2:]):
            continue
        extra = ''
        if cls == 'shad' and address:
            _, template = struct.unpack_from('<4sI', m.data, m.file_offset(address))
            if template in m.by_id:
                extra = '  template ' + m.by_id[template][4]
        print(f'{cls} {tag_id:08x} {address:08x} {size:6x} {name}{extra}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
