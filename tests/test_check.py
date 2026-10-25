import json

import pytest

from build import Marked
from check import (Identity, Targets, check_function, check_unique_markers, compare, difference_score, extract,
                   function_targets, literal_size, masked_offsets, parse_addresses, relative_ok, resolve,
                   settle_statuses, settle_targets, StandinCalls, targets_summary, write_report)
from linkmap import LinkMap

MAP = """ Preferred load address is 00400000
 0001:00000000 00000100H .text                   CODE
 0001:00000000       ?f@@YIXH@Z                 00401000 f   a.obj
 0001:00000040       ?f@@YIXM@Z                 00401040 f   a.obj
 0001:00000080       ?g@@YIXXZ                  00401080 f   a.obj
"""


def marked(name):
    return Marked('src/a.cpp', 0x1000, name, 'void', [])


def test_resolve_symbol():
    assert resolve(LinkMap(MAP), marked('g')).va == 0x401080


def test_resolve_vcall_thunk_by_its_vtable_offset():
    linkmap = LinkMap(MAP + ' 0001:000000c0       ??_9@$BFM@AE               004010c0 f i a.obj\n')
    assert resolve(linkmap, Marked('src/a.cpp', 0x234c5f, "`vcall'{0x5c}", '', [], kind='vcall')).va == 0x4010c0


def test_resolve_symbol_ambiguous():
    with pytest.raises(SystemExit) as e:
        resolve(LinkMap(MAP), marked('f'))
    assert '?f@@YIXH@Z' in str(e.value) and '?f@@YIXM@Z' in str(e.value)


def test_resolve_symbol_missing_is_none():
    # folded into an identical function or dropped: reported as todo, not fatal
    assert resolve(LinkMap(MAP), marked('h')) is None


def test_extract_strips_fill_after_retail_length():
    assert extract(bytes.fromhex('c3cccccc'), bytes.fromhex('c3')) == bytes.fromhex('c3')


def test_extract_keeps_real_cc_bytes():
    assert extract(bytes.fromhex('ebcccccc'), bytes.fromhex('ebcc')) == bytes.fromhex('ebcc')


def test_compare_masks_only_listed_fields():
    ours = bytes.fromhex('a1' '00204000' 'c3')
    theirs = bytes.fromhex('a1' '88e75500' 'c3')
    assert compare(ours, theirs, masked_offsets(0x401000, 6, {0x401001})) is None
    assert compare(ours, theirs, set()) == 1


def test_compare_unequal_lengths():
    assert compare(bytes.fromhex('33c0c3'), bytes.fromhex('33c0'), set()) == 2


def test_relative_fields_must_leave_in_retail():
    call_out = bytes.fromhex('e8' '10000000' 'c3')     # call va+0x15, outside a 6-byte function
    jump_self = bytes.fromhex('e9' 'fbffffff' 'c3')    # jmp va, inside
    assert relative_ok(call_out, 0x401000, call_out, 0x1000, 1)
    assert not relative_ok(call_out, 0x401000, jump_self, 0x1000, 1)


LO, HI = 0x10000, 0x600000


def run(full, start, theirs, theirs_va=0x1000, absolute=(), relative=()):
    return check_function(full, start, theirs, theirs_va, set(absolute), set(relative), LO, HI)


def test_absolute_field_must_be_an_address_in_retail():
    ours = bytes.fromhex('b8' '00204000' 'c3')
    status, first, _, _ = run(ours, 0x401000, bytes.fromhex('b8' '2a000000' 'c3'), absolute={0x401001})
    assert first == 1 and status == 'near'
    assert run(ours, 0x401000, bytes.fromhex('b8' '00204000' 'c3'), absolute={0x401001})[:2] == ('matched', None)


def test_field_straddling_the_start_is_not_masked():
    status, first, _, _ = run(bytes.fromhex('c3'), 0x401000, bytes.fromhex('90'), relative={0x400ffd})
    assert first == 0 and status == 'near'


def test_disjoint_field_is_ignored():
    assert run(bytes.fromhex('c3'), 0x401000, bytes.fromhex('c3'), relative={0x400ff0})[1] is None


def test_failed_relative_field_checked_even_when_bytes_equal():
    jump_self = bytes.fromhex('e9' 'fbffffff' 'c3')
    jump_out = bytes.fromhex('e9' '10000000' 'c3')
    status, first, _, _ = run(jump_out, 0x401000, jump_self, 0x1000, relative={0x401001})
    assert first == 1 and status == 'near'


def test_difference_score_is_zero_when_matched():
    status, _, lines, length = run(bytes.fromhex('b8' '01000000' 'c3'), 0x401000, bytes.fromhex('b8' '01000000' 'c3'))
    assert status == 'matched' and difference_score(status, lines, length, 6) == 0


def test_difference_score_counts_differing_instructions_and_length():
    ours = bytes.fromhex('b8' '01000000' '90' 'c3')    # mov eax, 1; nop; ret
    theirs = bytes.fromhex('b8' '02000000' 'c3')       # mov eax, 2; ret
    status, _, lines, length = run(ours, 0x401000, theirs)
    differing = sum(1 for line in lines if line.startswith('  !!'))
    assert status == 'todo' and differing >= 1
    assert difference_score(status, lines, length, len(theirs)) == differing + 1
    # one fewer differing instruction scores lower
    same_length = bytes.fromhex('b8' '02000000' '90' 'c3')
    status2, _, lines2, length2 = run(same_length, 0x401000, theirs)
    assert difference_score(status2, lines2, length2, len(theirs)) < difference_score(status, lines, length, len(theirs))


def test_failed_field_counts_as_differing_instruction():
    # two nops differ from retail; with the failed jmp that is three instructions
    ours = bytes.fromhex('e9' '10000000' '90' '90' 'c3')
    theirs = bytes.fromhex('e9' 'fbffffff' '91' '91' 'c3')
    assert run(ours, 0x401000, theirs, relative={0x401001})[0] == 'todo'
    same_jmp = theirs[:5] + ours[5:]
    assert run(same_jmp, 0x401000, theirs)[0] == 'near'


def test_incomplete_disassembly_is_todo():
    # 0f is not decodable on its own
    assert run(bytes.fromhex('0f'), 0x401000, bytes.fromhex('c3'))[0] == 'todo'


def test_ours_longer_with_fill_compares_retail_length():
    assert run(bytes.fromhex('ebcc' 'cccc'), 0x401000, bytes.fromhex('ebcc'))[:2] == ('matched', None)


@pytest.mark.parametrize('opcode, fixups', [('b8', 'absolute'), ('e8', 'relative')])
def test_field_beyond_retail_length_is_not_masked(opcode, fixups):
    ours = bytes.fromhex(opcode + '00204000' + 'c3')
    status, first, _, _ = run(ours, 0x401000, bytes.fromhex(opcode + '90' + 'c3'), **{fixups: {0x401001}})
    assert first == 1 and status == 'todo'


def test_jump_table_entries_must_map_to_the_same_offsets():
    # jmp [eax*4+table]; three case bodies (nop, nop, ret); table of three absolute entries
    def func(base, order):
        body = bytes.fromhex('ff2485' + (base + 12).to_bytes(4, 'little').hex() + '90' '90' 'c3')
        # code is 10 bytes (0..9), table at 12? pad two bytes so the table sits at offset 12
        return body + bytes.fromhex('9090') + b''.join((base + o).to_bytes(4, 'little') for o in order)
    ours = func(0x401000, (7, 8, 9))
    fields = {0x401000 + 12, 0x401000 + 16, 0x401000 + 20, 0x401000 + 3}
    same = func(0x1000, (7, 8, 9))
    assert run(ours, 0x401000, same, absolute=fields)[:2] == ('matched', None)
    swapped = func(0x1000, (7, 9, 8))
    assert run(ours, 0x401000, swapped, absolute=fields)[1] == 16


def _jump_table_function(base, order, case=b'\x90'):
    """jmp [eax*4+table]; three case bodies (case, nop, ret); two bytes of fill; a table of three absolute
    entries at offset 12, pointing at the case bodies in order."""
    body = bytes.fromhex('ff2485') + (base + 12).to_bytes(4, 'little') + case + bytes.fromhex('90' 'c3')
    return body + bytes.fromhex('9090') + b''.join((base + o).to_bytes(4, 'little') for o in order)


JUMP_TABLE_FIELDS = {0x401000 + 3, 0x401000 + 12, 0x401000 + 16, 0x401000 + 20}


def _differing_lines(lines):
    """The differing lines of a listing, as (ours, retail) texts."""
    return [tuple(side.strip() for side in line[5:].split('|')) for line in lines if line.startswith('  !!')]


def test_jump_table_entries_that_differ_only_by_layout_do_not_count():
    # the entries hold different addresses (the functions sit at different places) but the same offsets; only
    # the changed case body differs, and the table is compared as dwords, not decoded as instructions
    ours = _jump_table_function(0x401000, (7, 8, 9))
    theirs = _jump_table_function(0x1000, (7, 8, 9), case=b'\x91')
    status, first, lines, _ = run(ours, 0x401000, theirs, absolute=JUMP_TABLE_FIELDS)
    assert first == 7 and status == 'near'
    assert _differing_lines(lines) == [('nop', 'xchg ecx, eax')]
    assert sum(line[5:].startswith('dd ') for line in lines) == 3


def test_jump_table_entries_that_differ_count_once_each():
    ours = _jump_table_function(0x401000, (7, 8, 9))
    theirs = _jump_table_function(0x1000, (7, 9, 8), case=b'\x91')
    status, first, lines, _ = run(ours, 0x401000, theirs, absolute=JUMP_TABLE_FIELDS)
    assert first == 7 and status == 'todo'
    assert _differing_lines(lines) == [('nop', 'xchg ecx, eax'), ('dd 0x401008', 'dd 0x1009'),
                                       ('dd 0x401009', 'dd 0x1008')]


def _shifted_jump_table_function(base):
    """_jump_table_function's code with two more bytes of fill, so its table sits at offset 14, where it does
    not line up with a table at offset 12."""
    body = bytes.fromhex('ff2485') + (base + 14).to_bytes(4, 'little') + bytes.fromhex('90' '90' 'c3' '90909090')
    return body + b''.join((base + o).to_bytes(4, 'little') for o in (7, 8, 9))


def test_a_jump_table_that_does_not_line_up_scores_the_same_wherever_the_function_sits():
    # retail's table sits elsewhere, so ours is decoded as instructions; its entries are rebased to retail's
    # address first, so the count does not depend on where the link placed our function
    theirs = _jump_table_function(0x1000, (7, 8, 9))
    counts = set()
    for base in (0x401000, 0x40a7c3):
        status, first, lines, _ = run(_shifted_jump_table_function(base), base, theirs,
                                      absolute={base + 3, base + 14, base + 18, base + 22})
        assert status == 'todo' and first == 3
        counts.add(sum(line.startswith('  !!') for line in lines))
    assert len(counts) == 1


def test_self_recursive_call_matches_when_retail_calls_itself():
    ours = bytes.fromhex('e8' 'fbffffff' 'c3')      # call own start
    assert run(ours, 0x401000, ours, relative={0x401001})[:2] == ('matched', None)


def test_call_elsewhere_inside_retail_is_a_difference():
    ours = bytes.fromhex('e8' 'fbffffff' 'c3')
    theirs = bytes.fromhex('e8' '00000000' 'c3')    # calls its own ret
    assert run(ours, 0x401000, theirs, relative={0x401001})[1] == 1


def test_parse_addresses_rejects_non_hex():
    assert parse_addresses(['163ba0', '0x259d0']) == {0x163ba0, 0x259d0}
    with pytest.raises(SystemExit) as e:
        parse_addresses(['function_163ba0'])
    assert 'not a hexadecimal' in str(e.value)


def test_duplicate_retail_markers_are_rejected():
    a, b = Marked('src/a.cpp', 0x1000, 'f', 'void', []), Marked('src/b.cpp', 0x1000, 'g', 'void', [])
    check_unique_markers([a, Marked('src/a.cpp', 0x2000, 'h', 'void', [])])
    with pytest.raises(SystemExit) as e:
        check_unique_markers([a, b])
    assert 'duplicate @retail 0x1000' in str(e.value)


def test_check_reads_standard_markers_like_any_other(tmp_path):
    """check.py takes its markers from build.marked_sources, so a "standard"
    marker is checked, and counts toward duplicates, like a plain one."""
    import build
    (tmp_path / 'src').mkdir()
    (tmp_path / 'src' / 'a.cpp').write_text('// @retail 0x1000 standard\nlong __stdcall f(long a)\n{\n}\n')
    (tmp_path / 'src' / 'b.cpp').write_text('// @retail 0x1000\nlong g(long a)\n{\n}\n')
    found = build.marked_sources(str(tmp_path))
    assert [(m.path, m.retail, m.name, m.standard) for m in found] == [
        ('src/a.cpp', 0x1000, 'f', True), ('src/b.cpp', 0x1000, 'g', False)]
    assert build.standin_names(found[:1]) == {('src/a.cpp', 0x1000): 'standin_a_0'}
    with pytest.raises(SystemExit, match='duplicate @retail 0x1000'):
        check_unique_markers(found)


def test_filtered_run_merges_into_the_report(tmp_path):
    path = str(tmp_path / 'report.json')
    write_report(path, {'a': 1, 'b': 1}, merge=False)
    write_report(path, {'b': 2}, merge=True)
    assert json.load(open(path)) == {'a': 1, 'b': 2}
    write_report(path, {'c': 3}, merge=False)
    assert json.load(open(path)) == {'c': 3}


IDENT_MAP = """ Preferred load address is 00400000
 0001:00000000 00000300H .text                   CODE
 0001:00000000       ?caller@@YAXXZ             00401000 f   a.obj
 0001:00000100       ?function_163ba0@@YIXPAK@Z 00401100 f   a.obj
 0001:00000200       _strncmp                   00401200 f   libcmt:strncmp.obj
"""
CALL_START = 0x401000
CALLEE = Marked('src/a.cpp', 0x2100, 'function_163ba0', 'void', ['unsigned long *'])


def call_to(base, target):
    """A function at base that calls target, then returns."""
    return bytes.fromhex('e8') + ((target - (base + 5)) & 0xFFFFFFFF).to_bytes(4, 'little') + bytes.fromhex('c3')


def run_call(retail_target, markers=(CALLEE,), rows=None, our_target=0x401100):
    identity = Identity(LinkMap(IDENT_MAP), rows or {}, list(markers))
    return check_function(call_to(CALL_START, our_target), CALL_START, call_to(0x1000, retail_target), 0x1000,
                          set(), {CALL_START + 1}, LO, HI, identity)


def test_call_to_a_marked_callee_matches_when_retail_targets_its_address():
    assert run_call(0x2100)[:2] == ('matched', None)


def test_call_to_a_marked_callee_with_the_wrong_address_differs_at_the_call_field():
    assert run_call(0x2200)[1] == 1


def test_call_to_a_marked_retail_function_must_reach_its_symbol():
    # retail calls 0x2100, which src/ claims; ours calls something else
    assert run_call(0x2100, our_target=0x401200)[1] == 1


def test_call_to_a_library_function_matches_by_plain_name():
    rows = {0x2200: {'name': '_strncmp'}}
    assert run_call(0x2200, markers=(), rows=rows, our_target=0x401200)[:2] == ('matched', None)


def test_call_to_a_library_function_with_another_name_differs():
    rows = {0x2200: {'name': '_strcmp'}}
    assert run_call(0x2200, markers=(), rows=rows, our_target=0x401200)[1] == 1


def test_call_to_an_aliased_library_function_matches():
    # libcmt exports the stack probe as both _chkstk and __alloca_probe
    alias_map = IDENT_MAP.replace('_strncmp                  ', '__chkstk                  ')
    identity = Identity(LinkMap(alias_map), {0x2200: {'name': '__alloca_probe'}}, [])
    result = check_function(call_to(CALL_START, 0x401200), CALL_START, call_to(0x1000, 0x2200), 0x1000,
                            set(), {CALL_START + 1}, LO, HI, identity)
    assert result[:2] == ('matched', None)


def test_call_to_memmove_matches_retail_memcpy():
    # libcmt's memcpy.obj and memmove.obj hold the same code; retail's copy is named _memcpy
    alias_map = IDENT_MAP.replace('_strncmp                  ', '_memmove                  ')
    identity = Identity(LinkMap(alias_map), {0x2200: {'name': '_memcpy'}}, [])
    result = check_function(call_to(CALL_START, 0x401200), CALL_START, call_to(0x1000, 0x2200), 0x1000,
                            set(), {CALL_START + 1}, LO, HI, identity)
    assert result[:2] == ('matched', None)


@pytest.mark.parametrize('twin_first', [False, True])
def test_call_to_a_folded_function_matches_through_any_of_its_names(twin_first):
    # the linker folded two identical functions into one body; the map lists both at its address
    twin = ' 0001:00000100       ?twin@@YIXPAK@Z            00401100 f   b.obj\n'
    lines = IDENT_MAP.splitlines(keepends=True)
    lines.insert(3 if twin_first else 4, twin)
    markers = [CALLEE, Marked('src/b.cpp', 0x2300, 'twin', 'void', ['unsigned long *'])]
    identity = Identity(LinkMap(''.join(lines)), {}, markers)

    def result(retail_target):
        return check_function(call_to(CALL_START, 0x401100), CALL_START, call_to(0x1000, retail_target), 0x1000,
                              set(), {CALL_START + 1}, LO, HI, identity)[:2]
    assert result(0x2100) == ('matched', None)
    assert result(0x2300) == ('matched', None)
    assert result(0x2200)[1] == 1


def test_call_between_unnamed_functions_is_accepted():
    assert run_call(0x2200, markers=(), rows={0x2200: {'name': ''}}, our_target=0x401200)[:2] == ('matched', None)



CTOR_MAP = """ Preferred load address is 00400000
 0001:00000000 00000300H .text                   CODE
 0001:00000000       ?caller@@YAXXZ             00401000 f   a.obj
 0001:00000100       ??0c_foo@@QAE@XZ           00401100 f   a.obj
 0001:00000200       ??0c_foo@@QAE@ABV0@@Z      00401200 f i b.obj
"""
FOO = Marked('src/a.cpp', 0x2100, 'c_foo::c_foo', '', [], cls='c_foo', kind='constructor')


def test_constructor_marker_ignores_the_implicit_copy_constructor():
    # another source's stand-in emits c_foo's implicit copy constructor, with the same plain name
    assert resolve(LinkMap(CTOR_MAP), FOO).va == 0x401100


def test_copy_constructor_marker_stays_ambiguous():
    copy = Marked('src/a.cpp', 0x2100, 'c_foo::c_foo', '', ['const c_foo &'], cls='c_foo', kind='constructor')
    with pytest.raises(SystemExit) as e:
        resolve(LinkMap(CTOR_MAP), copy)
    assert 'ambiguous' in str(e.value)


OVERLOAD_MAP = """ Preferred load address is 00400000
 0001:00000000 00000300H .text                   CODE
 0001:00000000       ??0c_foo@@QAE@G@Z          00401000 f   a.obj
 0001:00000100       ??0c_foo@@QAE@J@Z          00401100 f   a.obj
 0001:00000200       ??0c_foo@@QAE@PAX@Z        00401200 f   a.obj
"""


@pytest.mark.parametrize('param, va', [('word', 0x401000), ('unsigned short', 0x401000), ('long', 0x401100)])
def test_overloaded_constructors_are_told_apart_by_scalar_parameters(param, va):
    marked = Marked('src/a.cpp', 0x2100, 'c_foo::c_foo', '', [param], cls='c_foo', kind='constructor')
    assert resolve(LinkMap(OVERLOAD_MAP), marked).va == va


def test_overloaded_constructor_with_a_pointer_parameter_stays_ambiguous():
    marked = Marked('src/a.cpp', 0x2100, 'c_foo::c_foo', '', ['void *'], cls='c_foo', kind='constructor')
    with pytest.raises(SystemExit) as e:
        resolve(LinkMap(OVERLOAD_MAP), marked)
    assert 'ambiguous' in str(e.value)


def test_call_to_a_marked_constructor_ignores_its_implicit_copy_constructor():
    identity = Identity(LinkMap(CTOR_MAP), {}, [FOO])

    def result(our_target):
        return check_function(call_to(CALL_START, our_target), CALL_START, call_to(0x1000, 0x2100), 0x1000,
                              set(), {CALL_START + 1}, LO, HI, identity)[:2]
    assert result(0x401100) == ('matched', None)
    assert result(0x401200)[1] == 1  # the copy constructor is not the marked one


VECTOR_CONSTRUCTOR = '??_H@YGXPAXIHP6EPAX0@Z@Z'
HELPER_MAP = IDENT_MAP.replace('_strncmp                  ', VECTOR_CONSTRUCTOR)
ITERATOR = Marked('src/b.cpp', 0x2200, 'vector_constructor_iterator', 'void', [])


def call_helper(marker=ITERATOR):
    """Ours calls the compiler's ??_H; retail calls 0x2200, which src/ claims with marker."""
    identity = Identity(LinkMap(HELPER_MAP), {0x2200: {'name': ''}}, [marker])
    return check_function(call_to(CALL_START, 0x401200), CALL_START, call_to(0x1000, 0x2200), 0x1000,
                          set(), {CALL_START + 1}, LO, HI, identity)


def test_call_to_the_compilers_helper_matches_the_marker_with_its_plain_name():
    # src/ marks retail's ??_H as vector_constructor_iterator; the compiler still emits and calls its own ??_H
    assert call_helper()[:2] == ('matched', None)


def test_call_to_a_claimed_function_differs_unless_it_is_the_same_compiler_helper():
    rows = {0x2200: {'name': '_strncmp'}}
    assert run_call(0x2200, markers=(ITERATOR,), rows=rows, our_target=0x401200)[1] == 1
    other = Marked('src/b.cpp', 0x2200, 'vector_destructor_iterator', 'void', [])
    assert call_helper(other)[1] == 1  # a different helper

# Overloads with pointer parameters: find_marked's rules leave them ambiguous,
# and each marker's stand-in calls its own overload.
STANDIN_MAP = """ Preferred load address is 00400000
 0001:00000000 00000300H .text                   CODE
 0001:00000100       ??0c_text@@QAE@PBD@Z       00401100 f   ui.obj
 0001:00000140       ??0c_text@@QAE@PB_W@Z      00401140 f   ui.obj
 0001:00000200       ?standin_ui_0@@YIXXZ       00401200 f   ui.obj
 0001:00000220       ?standin_ui_1@@YIXXZ       00401220 f   ui.obj
 0001:00000240       ?standin_ui_2@@YIXXZ       00401240 f   ui.obj
"""
WIDE = Marked('src/ui.cpp', 0x2bac52, 'c_text::c_text', '', ['const wchar_t *'], cls='c_text', kind='constructor')
NARROW = Marked('src/ui.cpp', 0x2bac6a, 'c_text::c_text', '', ['const char *'], cls='c_text', kind='constructor')
CALLER = Marked('src/ui.cpp', 0x2baeb1, 'make_texts', 'void', [])


class FakeImage:
    """Our image: stand-in k is the k-th marker of src/ui.cpp, and calls that marker's overload."""

    def __init__(self, code):
        self.code = code

    def read(self, va, size):
        return self.code.get(va, b'').ljust(size, b'\xcc')[:size]


def standin_calls():
    image = FakeImage({0x401200: call_to(0x401200, 0x401140), 0x401220: call_to(0x401220, 0x401100)})
    return StandinCalls(LinkMap(STANDIN_MAP), image, [WIDE, NARROW, CALLER])


def test_overloads_are_told_apart_by_what_their_stand_ins_call():
    linkmap, calls = LinkMap(STANDIN_MAP), standin_calls()
    assert resolve(linkmap, WIDE, calls).name == '??0c_text@@QAE@PB_W@Z'
    assert resolve(linkmap, NARROW, calls).name == '??0c_text@@QAE@PBD@Z'


def test_pointer_overloads_without_stand_in_calls_stay_ambiguous():
    with pytest.raises(SystemExit) as e:
        resolve(LinkMap(STANDIN_MAP), WIDE)
    assert 'ambiguous' in str(e.value)


def test_calls_to_overloads_reach_the_marked_one():
    # retail's caller calls the wide constructor (0x2bac52); ours must call that overload, not the narrow one
    identity = Identity(LinkMap(STANDIN_MAP), {}, [WIDE, NARROW], standin_calls())

    def result(our_target):
        return check_function(call_to(CALL_START, our_target), CALL_START, call_to(0x1000, 0x2bac52), 0x1000,
                              set(), {CALL_START + 1}, LO, HI, identity)[:2]
    assert result(0x401140) == ('matched', None)
    assert result(0x401100)[1] == 1


# Where address fields lead (Targets): a marked function, a global named for
# its retail address, literal-pool constants, and symbols nothing ties to retail.
TARGET_MAP = """ Preferred load address is 00400000
 0001:00000000 00000300H .text                   CODE
 0002:00000000 00000100H .data                   DATA
 0001:00000000       ?caller@@YAXXZ             00401000 f   a.obj
 0001:00000100       ?function_163ba0@@YIXPAK@Z 00401100 f   a.obj
 0001:00000200       ?helper@@YIXXZ             00401200 f   a.obj
 0001:00000280       _strncmp                   00401280 f   libcmt:strncmp.obj
 0002:00000000       ?g_4e0300@@3PAJA           00402000     a.obj
 0002:00000010       ??_C@_03KLMEOHFB@abc?$AA@  00402010     a.obj
 0002:00000018       __real@3f800000            00402018     a.obj
 0002:00000020       ??_7c_foo@@6B@             00402020     a.obj
 0002:00000028       ?g_counter@@3JA            00402028     a.obj
 0002:00000030       __imp__KeTickCount         00402030     xboxkrnl:xboxkrnl.exe
 0002:00000034       ___lookuptable             00402034     libcmt:output.obj
"""
FLOAT_ONE = (0x3f800000).to_bytes(4, 'little')


class Memory:
    """Bytes at given addresses; anywhere else is in no section, as Xbe.read says."""

    def __init__(self, chunks):
        self.chunks = chunks

    def read(self, va, size):
        for start, data in self.chunks.items():
            if start <= va and va + size <= start + len(data):
                return data[va - start:va - start + size]
        raise ValueError(f'{va:#x} is in no section')


# "abc" is followed by 4 bytes of data the map does not name
OUR_DATA = Memory({0x402010: b'abc\0' + bytes(4) + FLOAT_ONE})
RETAIL_DATA = Memory({0x450000: b'abc\0' b'abd\0' + FLOAT_ONE})


def targets(text=TARGET_MAP, rows=None):
    return Targets(Identity(LinkMap(text), rows or {}, [CALLEE]), OUR_DATA, RETAIL_DATA)


@pytest.mark.parametrize('mine, value, result', [
    (0x401100, 0x2100, ('verified', None, 'function_163ba0', 0x2100)),
    (0x401104, 0x2104, ('verified', None, 'function_163ba0+0x4', 0x2104)),
    (0x401100, 0x2200, ('differs', None, 'function_163ba0', 0x2100)),
])
def test_a_marked_functions_address_must_be_its_markers(mine, value, result):
    # push offset function_163ba0, as a callback
    assert targets().absolute(mine, value)[:4] == result


@pytest.mark.parametrize('decorated', ['?g_4e0300@@3PAJA', '_g_4e0300', '?g_4e0300@?1??caller@@YAXXZ@4PAJA'])
def test_a_global_named_for_its_retail_address_must_be_at_it(decorated):
    # a C++ global, a C one, a function's static; the field reads 4 bytes into it
    found = targets(TARGET_MAP.replace('?g_4e0300@@3PAJA', decorated)).absolute
    assert found(0x402004, 0x4e0304)[:2] == ('verified', None)
    assert found(0x402004, 0x4e0308)[0] == 'differs' and found(0x402004, 0x4e0308)[3] == 0x4e0304
    # its size is unknown, so settle_targets decides the offset
    assert found(0x402004, 0x4e0308)[4] == (decorated, 4)


def test_a_global_named_otherwise_is_unverified():
    assert targets(TARGET_MAP.replace('?g_4e0300@@3PAJA', '?g_4e0300x@@3PAJA')).absolute(
        0x402004, 0x4e0304)[:2] == ('unverified', 'named global')


@pytest.mark.parametrize('mine, value, verdict', [
    (0x402010, 0x450000, 'verified'),   # "abc" where retail holds "abc"
    (0x402011, 0x450001, 'verified'),   # "abc" + 1
    (0x402010, 0x450004, 'differs'),    # retail holds "abd" there
    (0x402010, 0x900000, 'differs'),    # in no section of retail
    (0x402018, 0x450008, 'verified'),   # 1.0f
    (0x402018, 0x450004, 'differs'),
])
def test_a_literal_constant_must_hold_the_same_bytes_in_retail(mine, value, verdict):
    assert targets().absolute(mine, value)[0] == verdict


@pytest.mark.parametrize('decorated, size', [
    ('??_C@_08INJJKPHM@XBOX?51?40?$AA@', 9), ('??_C@_0ED@CEHICPLJ@?5?5?5Type?3@', 0x43),
    ('??_C@_1M@BHHLEAOO@?$AA?$CF?$AAs?$AA?$AA@', 12), ('__real@3f800000', 4), ('__real@3ff0000000000000', 8),
    ('?g_counter@@3JA', None), ('??_7c_foo@@6B@', None),
])
def test_literal_sizes_come_from_their_names(decorated, size):
    assert literal_size(decorated) == size


@pytest.mark.parametrize('text, rows, mine, same', [
    (TARGET_MAP, {0x2100: {'size': '16'}}, 0x401110, 0x2110),                             # past a marked function
    (TARGET_MAP.replace('?g_4e0300@@3PAJA', '?g_4e0300@@3JA'), None, 0x402004, 0x4e0304),  # past a long
    (TARGET_MAP.replace('?g_4e0300@@3PAJA', '?g_4e0300@?1??caller@@YAXXZ@4_NA'), None, 0x402001, 0x4e0301),  # a bool
    (TARGET_MAP, None, 0x402014, None),                                                    # past "abc"
])
def test_an_offset_past_a_symbols_known_size_is_unverified_unless_retail_agrees(text, rows, mine, same):
    # the map's extent runs on over data it does not name, which may lie anywhere in retail; a field the same
    # distance from the symbol's retail address on both sides is verified all the same
    found = targets(text, rows).absolute
    assert found(mine, 0x450010)[:2] == ('unverified', "past a symbol's end")
    if same is not None:
        assert found(mine, same)[0] == 'verified'


def test_a_field_past_a_symbol_before_or_ahead_of_one_after_is_verified_by_it():
    # a loop's end pointer: g_4e0300's end is where "abc" starts in our image, and retail's field holds its end too
    found = targets().absolute
    assert found(0x402010, 0x4e0310)[:4] == ('verified', None, 'g_4e0300+0x10', 0x4e0310)
    assert found(0x402019, 0x4e0319)[0] == 'verified'  # a member of the element past the end, beyond "abc"
    # an array's base less one element, ahead of the global after
    found = targets(TARGET_MAP.replace('??_C@_03KLMEOHFB@abc?$AA@', '?g_4e0340@@3PAJA')).absolute
    assert found(0x40200c, 0x4e033c)[:3] == ('verified', None, 'g_4e0340-0x4')
    # farther past g_4e0300's end than its own 16 bytes: our g_4e0300 would be smaller than the code needs
    found = targets(TARGET_MAP.replace('?g_counter@@3JA', '?g_4e0360@@3PAJA')).absolute
    assert found(0x402028, 0x4e0328)[0] == 'differs'
    # a symbol no rule ties to retail stays unverified
    assert found(0x402024, 0x4e0324)[:2] == ('unverified', 'vtable')


@pytest.mark.parametrize('mine, category', [
    (0x402020, 'vtable'), (0x402028, 'named global'), (0x402030, 'import'), (0x402034, 'library data'),
    (0x401280, 'library code'), (0x401200, 'unmarked function'), (0x400000, 'no symbol'),
])
def test_other_targets_are_unverified_by_category(mine, category):
    assert targets().absolute(mine, 0x450000)[:2] == ('unverified', category)


def caller(base, address, string, helper, callee):
    """mov eax, [address]; push offset string; call helper; call callee; ret."""
    def rel(at, target):
        return ((target - (base + at + 4)) & 0xFFFFFFFF).to_bytes(4, 'little')
    return (b'\xa1' + address.to_bytes(4, 'little') + b'\x68' + string.to_bytes(4, 'little')
            + b'\xe8' + rel(11, helper) + b'\xe8' + rel(16, callee) + b'\xc3')


def test_targets_count_each_field_that_leaves_the_function():
    # retail reads 4 bytes into g_4e0300 where ours reads its start; the call to the unmarked helper reaches
    # an unnamed retail function, which Identity lets pass unverified
    ours = caller(0x401000, 0x402000, 0x402010, 0x401200, 0x401100)
    theirs = caller(0x1000, 0x4e0304, 0x450000, 0x3000, 0x2100)
    absolute, relative = {0x401001, 0x401006}, {0x40100b, 0x401010}
    entry = dict(targets=function_targets(ours, 0x401000, theirs, 0x1000, absolute, relative, targets()))
    settle_targets([entry])
    assert entry['targets'] == dict(verified=2, differs=1, unverified=1, categories={'call': 1},
                                    differing=[dict(offset=1, ours='g_4e0300', expected='0x4e0300', retail='0x4e0304')])
    # its bytes match; settle_statuses decides whether the function does
    identity = Identity(LinkMap(TARGET_MAP), {}, [CALLEE])
    assert check_function(ours, 0x401000, theirs, 0x1000, absolute, relative, LO, HI, identity)[:2] == (
        'matched', None)


def checked_caller(address, retail_address):
    """caller's report entry as check_one makes it, before settling: ours
    reads address, retail's retail_address; the rest is the same on both sides."""
    ours = caller(0x401000, address, 0x402010, 0x401200, 0x401100)
    theirs = caller(0x1000, retail_address, 0x450000, 0x3000, 0x2100)
    absolute, relative = {0x401001, 0x401006}, {0x40100b, 0x401010}
    identity = Identity(LinkMap(TARGET_MAP), {}, [CALLEE])
    status, first, _, length = check_function(ours, 0x401000, theirs, 0x1000, absolute, relative, LO, HI, identity)
    return dict(name='caller', status=status, size=len(theirs), ours=length, first_difference=first,
                targets=function_targets(ours, 0x401000, theirs, 0x1000, absolute, relative, targets()))


def test_a_function_whose_bytes_match_but_whose_field_differs_does_not_match(capsys):
    # retail reads 4 bytes into g_4e0300 where ours reads its start: one differing field, so near
    report = {'00001000': checked_caller(0x402000, 0x4e0304)}
    assert report['00001000']['status'] == 'matched'
    assert settle_statuses(report) == [report['00001000']]
    assert report['00001000']['status'] == 'near' and report['00001000']['first_difference'] == 1
    assert capsys.readouterr().out.splitlines() == [
        'DIFF   00001000 caller at +0x1 (21 bytes, retail 21), score 1',
        "  !! address field at +0x1: ours leads to g_4e0300 (0x4e0300 in retail), retail's to 0x4e0304"]


@pytest.mark.parametrize('differing, status', [(1, 'near'), (2, 'near'), (3, 'todo')])
def test_each_differing_field_counts_as_a_differing_instruction(differing, status):
    fields = [(k, 0x4e0310, 'differs', None, 'g_4e0300', 0x4e0300, None) for k in (1, 6, 11)[:differing]]
    report = {'00001000': dict(name='f', status='matched', size=16, ours=16, first_difference=None,
                               targets=fields + [(16, 0x3000, 'unverified', 'call', 'helper', None, None)])}
    settle_statuses(report)
    assert report['00001000']['status'] == status and report['00001000']['first_difference'] == 1


def test_unverified_fields_do_not_count_against_a_match(capsys):
    # the call to the unmarked helper is unverified; 4 bytes into g_4e0300 where retail reads 16 in differs until
    # settled, then may lie past its end (an aggregate offset), so that function's MATCH waits for settling
    report = {'00001000': checked_caller(0x402000, 0x4e0300), '00002000': checked_caller(0x402004, 0x4e0310),
              '00003000': dict(name='missing', status='todo', size=4, ours=0, first_difference=0)}
    assert settle_statuses(report) == []
    assert [e['status'] for e in report.values()] == ['matched', 'matched', 'todo']
    assert report['00001000']['targets']['categories'] == {'call': 1}
    assert report['00002000']['targets']['categories'] == {'call': 1, 'aggregate offset': 1}
    assert capsys.readouterr().out == 'MATCH  00002000 caller\n'


def test_function_targets_leave_out_jump_table_entries():
    ours, theirs = _jump_table_function(0x401000, (7, 8, 9)), _jump_table_function(0x1000, (7, 8, 9))
    assert function_targets(ours, 0x401000, theirs, 0x1000, JUMP_TABLE_FIELDS, set(), None) == []


def test_an_offset_into_a_global_of_unknown_size_differs_only_where_a_verified_field_reaches():
    # one function verifies 8 bytes into g_4e0300, so 4 bytes in lies inside it; 12 bytes in may lie past its
    # end, in data the map does not name
    found = targets().absolute
    entries = [dict(targets=[(1, 0x4e0308, *found(0x402008, 0x4e0308))]),
               dict(targets=[(1, 0x4e0310, *found(0x402004, 0x4e0310)), (6, 0x4e0300, *found(0x40200c, 0x4e0300))]),
               dict(status='todo')]
    settle_targets(entries)
    assert entries[0]['targets']['verified'] == 1
    assert entries[1]['targets'] == dict(verified=0, differs=1, unverified=1, categories={'aggregate offset': 1},
                                         differing=[dict(offset=1, ours='g_4e0300+0x4', expected='0x4e0304',
                                                         retail='0x4e0310')])


@pytest.mark.parametrize('retail_target, rows, verdict', [
    (0x2100, {}, 'verified'), (0x2200, {}, 'differs'), (0x2200, {0x2200: {'name': ''}}, 'differs'),
])
def test_call_verdicts_to_a_marked_function(retail_target, rows, verdict):
    assert Identity(LinkMap(IDENT_MAP), rows, [CALLEE]).verdict(0x401100, retail_target) == verdict


def test_call_between_unnamed_functions_is_unverified():
    identity = Identity(LinkMap(IDENT_MAP), {0x2200: {'name': ''}}, [])
    assert identity.verdict(0x401200, 0x2200) == 'unverified' and identity.ok(0x401200, 0x2200)
    assert Identity(LinkMap(IDENT_MAP), {0x2200: {'name': '_strncmp'}}, []).verdict(0x401200, 0x2200) == 'verified'


def test_targets_summary_totals_the_functions_whose_bytes_match():
    def entry(status, verified, differs, categories):
        return dict(status=status, targets=dict(verified=verified, differs=differs,
                                                unverified=sum(categories.values()), categories=categories))
    demoted = entry('near', 2, 1, {'call': 1})
    entries = [demoted, entry('matched', 1, 0, {'vtable': 2, 'call': 1}), entry('near', 9, 9, {'call': 9}),
               dict(status='todo')]
    assert targets_summary(entries, [demoted]) == (
        'address targets in 2 functions whose bytes match: 3 verified, 1 differ, 4 unverified (2 vtable, 2 call); '
        '1 demoted from matched for a differing field')
    assert targets_summary(entries[1:], []).endswith('; 0 demoted from matched for a differing field')
