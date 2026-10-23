"""The reduced image that tools/check.py --fast and tools/permute.py --fast
build instead of the whole program (tools/build.py's build(units=...)).

It holds a function's own source and the sources that define its callees in
retail, their callees in turn, and so on (reduced_units). From a full image of
the program it also takes the sources that pin calling conventions there
(pinning_units) and, through /INCLUDE, the library objects the full image
takes (library_symbols). It is linked with the ballast, and with unresolved
externals allowed: what the left-out sources define stays unresolved.

tools/check.py --fast also takes in the sources that define the functions a
checked function calls by name (named_units): retail's calls column leaves out
what retail inlined. FullBuild reads the last full build (build/halo2.map and
build/halo2.exe), whose code --fast compares a reduced image's with, to say
when the reduced image builds a function differently. find_body finds a
marked function's body in its source (tools/permute.py rewrites it).
"""
import bisect
import os
import re

import build
from linkmap import LinkMap, plain_name
from pe import Pe


def _skip_literal(text, i):
    """If a comment, string or character literal starts at i, the index after it, else i."""
    if text.startswith('//', i):
        j = text.find('\n', i)
        return len(text) if j < 0 else j
    if text.startswith('/*', i):
        j = text.find('*/', i + 2)
        return len(text) if j < 0 else j + 2
    if text[i] in '"\'':
        q, j = text[i], i + 1
        while j < len(text) and text[j] != q:
            j += 2 if text[j] == '\\' else 1
        return j + 1
    return i


def match_brace(text, open_index):
    """The index of the '}' matching the '{' at open_index."""
    depth, i = 0, open_index
    while i < len(text):
        j = _skip_literal(text, i)
        if j != i:
            i = j
            continue
        if text[i] == '{':
            depth += 1
        elif text[i] == '}':
            depth -= 1
            if depth == 0:
                return i
        i += 1
    raise ValueError('unbalanced braces')


def find_body(text, va):
    """(start, end) such that text[start:end] is the body of the function marked
    "// @retail <va>" (or "// @retail <va> standard"): the text between its braces."""
    for m in re.finditer(r'^[ \t]*//[ \t]*@retail[ \t]+(0x[0-9a-fA-F]+)(?:[ \t]+standard)?[ \t]*\r?$', text, re.M):
        if int(m.group(1), 16) != va:
            continue
        i = m.end()
        while i < len(text):
            j = _skip_literal(text, i)
            if j != i:
                i = j
            elif text[i] == '{':
                return i + 1, match_brace(text, i)
            else:
                i += 1
    raise ValueError(f'no function marked @retail {va:#x}')


def reduced_units(va, rows, markers, depth=None):
    """The sources of the reduced image that scores va (tools/build.py's
    build(units=...)): the one that marks va and those that mark its callees in
    retail (the calls column of config/functions.csv, with @retail or @stub
    markers), followed through callees that are themselves marked @retail, to
    depth levels (None: all the way). Link-time code generation fits a call to
    its callee's code, and that code to the callee's own callees, so leaving a
    callee out changes how the target calls it. A stub is a leaf: it is built
    without LTCG. A callee that no source marks comes from the libraries."""
    where = {m.retail: m.path for m in markers}
    decompiled = {m.retail for m in markers if not m.stub}
    seen, frontier, level = {va}, [va], 0
    while frontier and (depth is None or level < depth):
        level += 1
        found = [int(t, 16) for f in frontier if f in decompiled for t in rows.get(f, {}).get('calls', '').split()]
        frontier = [c for c in dict.fromkeys(found) if c not in seen]
        seen.update(frontier)
    return {where[f] for f in seen if f in where}


LTCG_LIBRARIES = {name[:-len('.lib')] for name in build.LIBRARIES if name.endswith('ltcg.lib')}  # built with /GL
LIBRARY_MEMBERS = {f'{stem}.obj' for stem in build.D3D_OBJECTS}  # library members the build links as objects


def library_object(name):
    """Whether a map's object (its Lib:Object column) is library code: taken
    from a library ('library:object.obj'), or one of the library members the
    build links as objects (build.D3D_OBJECTS), which the map names alone."""
    return ':' in name or name in LIBRARY_MEMBERS


def library_symbols(linkmap):
    """One public symbol of each object an image (given its LinkMap) took from
    a library built without LTCG. A reduced image that takes the same objects
    (/INCLUDE) keeps the standard conventions their calls force on the
    functions they call, such as the compiler's vector iterators."""
    found = {}
    for s in linkmap.symbols:
        library, colon, _ = s.object.partition(':')
        if colon and not s.static and library not in LTCG_LIBRARIES:
            found.setdefault(s.object, s.name)
    return sorted(found.values())


def rel_target(image, field):
    """Where the relative (call or jump) field at field leads."""
    return (field + 4 + int.from_bytes(image.read(field, 4), 'little', signed=True)) & 0xFFFFFFFF


def fields_within(fields, start, end):
    """The part of sorted fields in [start, end)."""
    return fields[bisect.bisect_left(fields, start):bisect.bisect_left(fields, end)]


def callees(linkmap, image, relative, symbol):
    """(the functions the code at symbol calls or jumps to, other than itself,
    and how many of its calls reach no function: unresolved, in a reduced
    image), in an image (its LinkMap and Pe) whose relative fields, sorted,
    are relative."""
    found, unresolved = {}, 0
    for field in fields_within(relative, *linkmap.extent(symbol)):
        target = linkmap.symbol_at(rel_target(image, field))
        if target is None:
            unresolved += 1
        elif target.va != symbol.va:
            found[target.va] = target
    return list(found.values()), unresolved


def pinning_units(linkmap, image, symbols, sources):
    """The sources that, in a full image (its LinkMap and Pe), pin the calling
    convention of the functions at symbols or of a function one of them calls
    directly: those that take such a function's address (link-time code
    generation keeps the standard convention of a function whose address
    escapes, such as a callback) and stubs, built without LTCG, that call one.
    sources maps the map's object names to their sources
    (build.source_objects)."""
    relative = sorted(linkmap.rel_fixups)
    pinned = {s.va for s in symbols}
    for symbol in symbols:
        pinned.update(c.va for c in callees(linkmap, image, relative, symbol)[0])
    objects = set()
    for field in image.fixups:
        if int.from_bytes(image.read(field, 4), 'little') in pinned and linkmap.symbol_at(field):
            objects.add(linkmap.symbol_at(field).object)
    for field in relative:
        caller = linkmap.symbol_at(field)
        if (caller is not None and sources.get(caller.object, '').startswith('src/stubs/')
                and rel_target(image, field) in pinned):
            objects.add(caller.object)
    return {sources[o] for o in objects if o in sources}


class Image:
    """A linked image: its LinkMap, its Pe, and its absolute (base
    relocations) and relative address fields, sorted."""

    def __init__(self, linkmap, image):
        self.linkmap, self.image = linkmap, image
        self.absolute, self.relative = sorted(image.fixups), sorted(linkmap.rel_fixups)

    @classmethod
    def read(cls, map_path, image_path):
        return cls(LinkMap.read(map_path), Pe(image_path))

    def callees(self, symbol):
        """See callees."""
        return callees(self.linkmap, self.image, self.relative, symbol)

    def unresolved_globals(self, symbol):
        """How many address fields in symbol's code hold no address in the
        image: a global left unresolved reads as the image base."""
        lowest = min(section.va for section in self.image.sections)
        return sum(int.from_bytes(self.image.read(field, 4), 'little') < lowest
                   for field in fields_within(self.absolute, *self.linkmap.extent(symbol)))

    def code(self, symbol):
        """(the code of symbol's extent without trailing fill, the offsets its
        address fields cover)."""
        start, end = self.linkmap.extent(symbol)
        masked = {field - start + i for fields in (self.absolute, self.relative)
                  for field in fields_within(fields, start - 3, end) for i in range(4)}
        return self.image.read(start, end - start).rstrip(b'\xcc'), masked


class FullBuild(Image):
    """The last full build of a tree (build/halo2.map and build/halo2.exe),
    linked at time built from the objects in obj_dir."""

    def __init__(self, linkmap, image, built, obj_dir):
        super().__init__(linkmap, image)
        self.built, self.obj_dir = built, obj_dir
        self.by_name = {(s.name, s.object): s for s in linkmap.symbols}

    @classmethod
    def load(cls, root):
        """The tree's last full build, or None when it has none."""
        out = os.path.join(root, 'build')
        map_path, image_path = os.path.join(out, build.MAP_NAME), os.path.join(out, build.EXE_NAME)
        if not (os.path.exists(map_path) and os.path.exists(image_path)):
            return None
        return cls(LinkMap.read(map_path), Pe(image_path), os.path.getmtime(image_path), os.path.join(out, 'obj'))

    def counterpart(self, symbol):
        """The full image's symbol with symbol's name and object, or None."""
        return self.by_name.get((symbol.name, symbol.object))

    def unchanged(self, symbol):
        """Whether the object of a symbol (of a reduced image built from
        obj_dir) went into this build unchanged: a library's always does, a
        source's when it is older than the build."""
        obj = os.path.join(self.obj_dir, symbol.object)
        return library_object(symbol.object) or (os.path.exists(obj) and os.path.getmtime(obj) < self.built)


CALLED = re.compile(r'([A-Za-z_]\w*)\s*\(')
NAME = re.compile(r'[A-Za-z_]\w*')
SPECIAL = ('??0', '??1', '??_G', '??_E')  # constructors and destructors, which a declaration calls unnamed
DATA = re.compile(r'^\?[^@?]+@(?:[^@]+@)*@[23]')  # a global variable or a static member


def bodies(marked, root):
    """The source text of each marked function's body (find_body). A marker
    with no body of its own ("deleting", "vcall") has none."""
    out = []
    for m in marked:
        text = build._read(os.path.join(root, *m.path.split('/')))
        try:
            start, end = find_body(text, m.retail)
        except ValueError:
            continue
        out.append(text[start:end])
    return out


def named_units(bodies, linkmap, sources):
    """The sources that define, in a full image (its LinkMap), a function or a
    global the given function bodies use by name. Link-time code generation
    can inline a function from another source, and retail's calls column, from
    which reduced_units works, leaves out what retail inlined: without that
    source the reduced image cannot inline it, and the call stays unresolved.
    It also builds code differently around a global it sees defined. A called
    name matches every public function with that last name component (a
    method's too); any name matches the constructors and destructors of a
    class with that name, and a global with that name. sources maps the map's
    objects to their sources (build.source_objects); stubs, which are not
    built with link-time code generation, never count, nor do the compiler's
    own constants and strings."""
    functions, others = {}, {}
    for s in linkmap.symbols:
        source = sources.get(s.object, '')
        if s.static or not source.startswith('src/') or source.startswith('src/stubs/'):
            continue
        scope, _, last = plain_name(s.name).rpartition('::')
        if s.function:
            functions.setdefault(last, set()).add(source)
            if s.name.startswith(SPECIAL):
                others.setdefault(scope.rpartition('::')[2], set()).add(source)
        elif DATA.match(s.name):
            others.setdefault(last, set()).add(source)
    found = set()
    for body in bodies:
        for name in CALLED.findall(body):
            found |= functions.get(name, set())
        for name in set(NAME.findall(body)):
            found |= others.get(name, set())
    return found
