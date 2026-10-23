"""check.py --fast: argument handling, what it writes, the reduced image it
asks tools/build.py for, and its CAVEAT lines (tools/reduced.py)."""
import json
import os

import pytest

import build
import check
import reduced
from linkmap import LinkMap
from xbe import Section


def run_main(monkeypatch, *argv):
    monkeypatch.setattr(check.sys, 'argv', ['check.py', *argv])
    check.main()


@pytest.mark.parametrize('argv, message', [
    (['--fast'], 'list their retail addresses'),
    (['--fast', '--no-build', '0x1000'], '--no-build is for the full check'),
    (['--jobs', '2', '0x1000'], '--jobs takes a positive count, and needs --fast'),
    (['--fast', '--jobs', '0', '0x1000'], '--jobs takes a positive count, and needs --fast'),
])
def test_fast_arguments_are_checked(monkeypatch, capsys, argv, message):
    with pytest.raises(SystemExit):
        run_main(monkeypatch, *argv)
    assert message in capsys.readouterr().err


def test_fast_reads_addresses_from_a_file_and_skips_the_full_build(monkeypatch, tmp_path):
    """@file lists work as for the full check, and --fast never starts a full build."""
    (tmp_path / 'list.txt').write_text('0x1000\n0x2000\n')
    seen = {}
    monkeypatch.setattr(check, 'retail_xbe_path', lambda: __file__)
    monkeypatch.setattr(check, 'check_retail', lambda path: None)
    monkeypatch.setattr(check, 'Xbe', lambda path: 'retail')
    monkeypatch.setattr(check, 'fast_check', lambda wanted, retail, jobs, started=None: seen.update(
        wanted=wanted, retail=retail, jobs=jobs) or 0)
    monkeypatch.setattr(check.build, 'build', lambda *a, **k: pytest.fail('--fast ran the full build'))
    with pytest.raises(SystemExit) as e:
        run_main(monkeypatch, '--fast', f'@{tmp_path / "list.txt"}')
    assert e.value.code == 0
    assert seen == dict(wanted={0x1000, 0x2000}, retail='retail', jobs=2)


# ------------------------------------------------------------ a fake tree

# 0x1000 (src/a.cpp) calls 0x2000 (src/b.cpp), which calls 0x3000 (src/c.cpp);
# 0x4000 (src/d.cpp) calls 0x5000 (src/stubs/s.cpp); src/e.cpp calls 0x1000
CALLS = {0x1000: '00002000', 0x2000: '00003000', 0x3000: '', 0x4000: '00005000', 0x5000: '', 0x6000: '00001000'}
ROWS = {va: {'va': f'{va:08x}', 'size': '1', 'calls': calls, 'status': 'todo', 'source': ''}
        for va, calls in CALLS.items()}
MARKED = [build.Marked('src/a.cpp', 0x1000, 'f1000', 'void', []), build.Marked('src/b.cpp', 0x2000, 'f2000', 'void', []),
          build.Marked('src/c.cpp', 0x3000, 'f3000', 'void', []), build.Marked('src/d.cpp', 0x4000, 'f4000', 'void', []),
          build.Marked('src/e.cpp', 0x6000, 'f6000', 'void', [])]
STUBS = [build.Marked('src/stubs/s.cpp', 0x5000, 's5000', 'void', [], stub=True)]

REDUCED_MAP = '''  Preferred load address is 00400000

 0001:00000000 00000100H .text                   CODE

  Address         Publics by Value              Rva+Base     Lib:Object

 0001:00000000       ?f1000@@YIXXZ              00401000 f   a.obj
 0001:00000010       ?f4000@@YIXXZ              00401010 f   d.obj
 0001:00000020       ?f2000@@YIXXZ              00401020 f   b.obj
'''


class FakeImage:
    """A Pe stand-in: code at given addresses, absolute fixups, one section."""

    def __init__(self, code, fixups=()):
        self.code, self.fixups = code, set(fixups)
        self.sections = [Section('.text', 0x401000, 0x100, 0, 0x100, 0)]

    def read(self, va, size):
        out = bytearray(b'\xcc' * size)
        for start, data in self.code.items():
            for i, b in enumerate(data):
                if 0 <= start + i - va < size:
                    out[start + i - va] = b
        return bytes(out)


def image(text, code, fixups=()):
    """A reduced.Image over a map text and FakeImage code."""
    return reduced.Image(LinkMap(text), FakeImage(code, fixups))


class Retail:
    base = 0x10000
    sections = [Section('.text', 0x1000, 0x10000, 0, 0x10000, 0)]

    def read(self, va, size):
        return b'\xc3'


@pytest.fixture
def fake_tree(monkeypatch, tmp_path):
    """A tree with no full build whose reduced image holds f1000 and f4000
    as a lone ret, as retail has them; records what build.build was asked."""
    (tmp_path / 'config').mkdir()
    (tmp_path / 'build').mkdir()
    for name in ('functions.csv', 'report.json'):
        (tmp_path / ('config' if name.endswith('.csv') else 'build') / name).write_text('untouched')
    asked = {}

    def fake_build(root, **kwargs):
        asked.update(kwargs, root=root)
        kwargs['timings'].update(compile=0.0, compiled=0, link=1.0)
        return 'reduced.map'

    monkeypatch.setattr(check, 'read_rows', lambda path: {va: dict(row) for va, row in ROWS.items()})
    monkeypatch.setattr(check.build, 'marked_sources', lambda root: MARKED)
    monkeypatch.setattr(check.build, 'stub_sources', lambda root: STUBS)
    monkeypatch.setattr(check.build, 'build', fake_build)
    monkeypatch.setattr(check.reduced.Image, 'read', staticmethod(lambda map_path, image_path: image(
        REDUCED_MAP, {0x401000: b'\xc3', 0x401010: b'\xc3', 0x401020: b'\xc3'})))
    monkeypatch.setattr(check, 'write_rows', lambda *a: pytest.fail('--fast wrote config/functions.csv'))
    return tmp_path, asked


def test_fast_writes_only_its_own_report(fake_tree, capsys):
    root, asked = fake_tree
    assert check.fast_check({0x1000, 0x4000}, Retail(), 2, root=str(root)) == 0
    out = capsys.readouterr().out
    assert 'MATCH  00001000 f1000' in out and 'MATCH  00004000 f4000' in out
    assert 'fast check: 2 of 2 match on a reduced image of 5 sources' in out
    assert (root / 'config' / 'functions.csv').read_text() == 'untouched'
    assert (root / 'build' / 'report.json').read_text() == 'untouched'
    report = json.loads((root / 'build' / 'report-fast.json').read_text())
    assert report['00001000']['status'] == 'matched' and report['00004000']['first_difference'] is None


def test_fast_builds_one_image_of_the_union_of_reduced_units(fake_tree):
    """One reduced image for every function asked for: each one's source and
    its callees' all the way down (a stub is a leaf); no callers. It is linked
    as an .img, never an .exe, and reused when nothing changed."""
    root, asked = fake_tree
    check.fast_check({0x1000, 0x4000}, Retail(), jobs=3, root=str(root))
    assert asked['units'] == {'src/a.cpp', 'src/b.cpp', 'src/c.cpp', 'src/d.cpp', 'src/stubs/s.cpp'}
    assert asked['image'].endswith(os.path.join('build', 'fast', 'reduced.img'))
    assert asked['reuse'] is True and asked['jobs'] == 3 and asked['include'] == ()


def test_fast_without_a_full_build_says_so(fake_tree, capsys):
    root, _ = fake_tree
    check.fast_check({0x1000}, Retail(), 2, root=str(root))
    out = capsys.readouterr().out
    caveats = [l for l in out.splitlines() if l.startswith('CAVEAT')]
    assert caveats == [f'CAVEAT {check.NO_FULL_BUILD}']
    report = json.loads((root / 'build' / 'report-fast.json').read_text())
    assert report['00001000']['caveats'] == [check.NO_FULL_BUILD]


FIELD_MAP = REDUCED_MAP.replace('CODE\n', 'CODE\n 0002:00000000 00000100H .data                   DATA\n') + (
    ' 0002:00000000       ?g_4e0300@@3JA             00402000     a.obj\n')


class FieldRetail(Retail):
    """f1000 reads 4 bytes past retail's g_4e0300."""
    sections = [Section('.text', 0x1000, 0x500000, 0, 0x500000, 0)]

    def read(self, va, size):
        return bytes.fromhex('a1' '04034e00' 'c3') if va == 0x1000 else b'\xc3'


def test_fast_applies_the_address_field_rule(fake_tree, monkeypatch, capsys):
    """mov eax, [g_4e0300]; ret matches retail's bytes, but retail reads
    another address: the function is near, as in the full check."""
    root, _ = fake_tree
    monkeypatch.setattr(check, 'read_rows', lambda path: {va: dict(row, size='6' if va == 0x1000 else '1')
                                                          for va, row in ROWS.items()})
    monkeypatch.setattr(check.reduced.Image, 'read', staticmethod(lambda map_path, image_path: image(
        FIELD_MAP, {0x401000: bytes.fromhex('a1' '00204000' 'c3')}, fixups={0x401001})))
    assert check.fast_check({0x1000}, FieldRetail(), 2, root=str(root)) == 1
    out = capsys.readouterr().out
    assert 'MATCH' not in out and 'fast check: 0 of 1 match' in out
    assert ('DIFF   00001000 f1000 at +0x1 (6 bytes, retail 6), score 1\n'
            "  !! address field at +0x1: ours leads to g_4e0300 (0x4e0300 in retail), retail's to 0x4e0304\n") in out
    assert out.rstrip().splitlines()[-2].endswith('; 1 demoted from matched for a differing field')
    report = json.loads((root / 'build' / 'report-fast.json').read_text())
    assert report['00001000']['status'] == 'near' and report['00001000']['first_difference'] == 1


def test_fast_reports_an_unmarked_address(fake_tree, capsys):
    root, _ = fake_tree
    assert check.fast_check({0x1000, 0x9000}, Retail(), 2, root=str(root)) == 1
    assert 'no @retail marker in src/ for: 0x9000' in capsys.readouterr().out


# ------------------------------------------------------------ caveats

CAVEAT_MAP = '''  Preferred load address is 00400000

 0001:00000000 00000100H .text                   CODE

  Address         Publics by Value              Rva+Base     Lib:Object

 0001:00000000       ?target@@YIXXZ             00401000 f   a.obj
 0001:00000020       ?callee@@YIXXZ             00401020 f   b.obj

FIXUPS: 1001 5
'''


def call(source, target):
    return bytes.fromhex('e8') + ((target - (source + 5)) & 0xFFFFFFFF).to_bytes(4, 'little')


def caveat_images(callee_in_full=b'\x33\xc0\xc3'):
    """target calls callee, then calls something left out (the image base),
    then reads a global left out; the full build's callee is callee_in_full."""
    ours = image(CAVEAT_MAP, {0x401000: call(0x401000, 0x401020) + call(0x401005, 0x400000)
                              + bytes.fromhex('a1') + (0x400000).to_bytes(4, 'little') + b'\xc3',
                              0x401020: b'\x31\xc0\xc3'}, fixups={0x40100b})
    full = image(CAVEAT_MAP.replace('FIXUPS: 1001 5', 'FIXUPS: 1001'),
                 {0x401000: call(0x401000, 0x401020) + b'\xc3', 0x401020: callee_in_full})
    return ours, full


class Identity:
    address_of = {'?callee@@YIXXZ': 0x2000}


def full_build(full, built, obj_dir):
    return reduced.FullBuild(full.linkmap, full.image, built, str(obj_dir))


def test_caveats_name_what_the_reduced_image_leaves_out(tmp_path):
    ours, _ = caveat_images()
    target = ours.linkmap.find('target')[0]
    assert check.fast_caveats(ours, target, None, Identity()) == [
        '1 call and 1 global reference reach what the reduced image leaves out (unresolved); the full build may '
        'inline such a function, or build differently around such a global']


def test_caveats_compare_unchanged_code_with_the_last_full_build(tmp_path):
    """The callee's object went into the full build unchanged, yet its code
    there differs (xor eax, eax there, xor eax, eax spelled 31 c0 here); the
    target's own code is compared too, and differs in length."""
    for name in ('a.obj', 'b.obj'):
        (tmp_path / name).write_bytes(b'')
        os.utime(tmp_path / name, (1000, 1000))
    ours, full = caveat_images()
    target = ours.linkmap.find('target')[0]
    notes = check.fast_caveats(ours, target, full_build(full, 2000, tmp_path), Identity())
    assert notes[1:] == [
        'it builds differently here than in the last full build of the same source; trust the full check for it',
        'its callee 0x2000 builds differently here than in the last full build (link-time code generation '
        'outside the reduced image); trust the full check for it']
    # sources compiled after the full build (an edit) are not compared
    assert check.fast_caveats(ours, target, full_build(full, 500, tmp_path), Identity()) == notes[:1]
    # nor is code that is the same apart from address fields
    ours, full = caveat_images(callee_in_full=b'\x31\xc0\xc3')
    assert len(check.fast_caveats(ours, target, full_build(full, 2000, tmp_path), Identity())) == 2


def test_d3d_members_linked_as_objects_count_as_library_code():
    """The map names a library object 'library:object.obj', but a D3D member the
    build links as an object by its file name alone."""
    assert reduced.library_object('libcmt:memcpy.obj') and reduced.library_object('pusher.obj')
    assert not reduced.library_object('b.obj')


NAMED_MAP = '''  Address         Publics by Value              Rva+Base     Lib:Object

 0001:00000000       ?helper@@YI_NPAUs_x@@@Z    00401000 f   b.obj
 0001:00000010       ??0s_item@@QAE@XZ          00401010 f   c.obj
 0001:00000020       ?update@c_thing@@QAEXXZ    00401020 f   d.obj
 0001:00000030       ?stub_call@@YGXXZ          00401030 f   stubs_s.obj
 0001:00000040       _memcpy                    00401040 f   libcmt:memcpy.obj
 0002:00000000       ?g_counter@@3JA            00402000     e.obj
 0002:00000010       ?g_unused@@3JA             00402010     f.obj
 0002:00000020       __real@40800000            00402020     h.obj

 Static symbols

 0001:00000050       ?local_helper@@YIXXZ       00401050 f   g.obj
'''
SOURCES = {'b.obj': 'src/b.cpp', 'c.obj': 'src/c.cpp', 'd.obj': 'src/d.cpp', 'stubs_s.obj': 'src/stubs/s.cpp',
           'e.obj': 'src/e.cpp', 'f.obj': 'src/f.cpp', 'g.obj': 'src/g.cpp', 'h.obj': 'src/h.cpp'}


def test_named_units_take_the_sources_of_what_a_body_uses_by_name():
    """Called functions (methods too), the constructor a declaration calls, and
    globals; not stubs, libraries, static functions, the compiler's constants
    (a float literal's __real@, whose plain name is real) or unused names."""
    body = '''
    s_item item;
    real (&corners)[4] = g_corners;
    if (helper(&x) && local_helper())
        thing->update();
    stub_call();
    memcpy(a, b, 4);
    g_counter++;
'''
    assert reduced.named_units([body], LinkMap(NAMED_MAP), SOURCES) == {
        'src/b.cpp', 'src/c.cpp', 'src/d.cpp', 'src/e.cpp'}


def test_fields_and_variables_named_like_functions_do_not_count():
    body = 'long helper = thing->update + item.s_other;'
    assert reduced.named_units([body], LinkMap(NAMED_MAP), SOURCES) == set()


def test_parse_cache_round_trip(tmp_path, monkeypatch):
    """What one process derived from a file is taken by the next while the
    file is unchanged, and dropped when it changes."""
    source = tmp_path / 'a.cpp'
    source.write_text('// @retail 0x1000\nvoid f(void)\n{\n}\n')
    calls = []

    def derive(text):
        calls.append(text)
        return len(text)

    monkeypatch.setattr(build, '_FILES', {})
    assert build._read(str(source), 'size', derive) == len(source.read_text())
    build.save_parse_cache(str(tmp_path / 'cache' / 'parsed.pickle'))
    monkeypatch.setattr(build, '_FILES', {})
    build.load_parse_cache(str(tmp_path / 'cache' / 'parsed.pickle'))
    assert build._read(str(source), 'size', derive) == len(source.read_text()) and len(calls) == 1
    source.write_text('// changed\n')
    os.utime(source, (5000, 5000))
    assert build._read(str(source), 'size', derive) == len('// changed\n') and len(calls) == 2
