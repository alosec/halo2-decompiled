import hashlib



import pytest

from capstone import CS_ARCH_X86, CS_MODE_32, Cs



from inventory import COLUMNS, apply_owners, load_owners, check_retail, function_name, check_unique, fill_from_neighbours, is_eh_stub, library_hits, merge, owner, read_rows, write_rows
from inventory import DATA_RANGES, KNOWN_STARTS, NOT_STARTS, OWNERS_JSON, in_data
from xbe import FUNCTIONS_CSV





def test_inventory_rejects_wrong_xbe(tmp_path):

    fake = tmp_path / 'default.xbe'

    fake.write_bytes(b'XBEH' + bytes(100))

    with pytest.raises(SystemExit) as e:

        check_retail(str(fake))

    assert 'not the retail XBE this project matches' in str(e.value)

    assert hashlib.sha256(fake.read_bytes()).hexdigest() in str(e.value)





def test_rows_keep_source_and_status(tmp_path):

    old = {0x163ba0: dict(va='00163ba0', size='84', owner='game', style='speed', evidence='a16 pad',

                          name='x', object='', calls='', source='src/a.cpp', status='matched')}

    new = [dict(va='00163ba0', size='84', owner='game', style='speed', evidence='a16 pad',

                name='_strncmp', object='', calls='00163c00', source='', status='todo')]

    rows = merge(new, old)

    assert rows[0]['source'] == 'src/a.cpp' and rows[0]['status'] == 'matched'

    assert rows[0]['name'] == '_strncmp'

    path = tmp_path / 'f.csv'

    write_rows(str(path), rows)

    assert read_rows(str(path))[0x163ba0]['status'] == 'matched'

    assert path.read_text().splitlines()[0] == ','.join(COLUMNS)





def test_owner_rules():
    assert owner('D3D', None) == 'xdk:d3d8'
    assert owner('BINK', None) == 'third:bink'
    assert owner('.text', 'libcmt') == 'xdk:libcmt'
    assert owner('.text', None) is None

def _fill(*owners):

    rows = [dict(va=f'{0x1000 + i * 16:08x}', owner=o) for i, o in enumerate(owners)]

    fill_from_neighbours(rows)

    return [r['owner'] for r in rows]





def test_fill_from_neighbours():

    assert _fill('third:havok', None, 'third:havok') == ['third:havok'] * 3

    assert _fill('xdk:xvoice', None, None, 'xdk:xvoice') == ['xdk:xvoice'] * 4

    assert _fill('third:havok', None, 'game') == ['third:havok', 'game', 'game']

    assert _fill(None, 'third:havok') == ['game', 'third:havok']

    assert _fill('xdk:xvoice', None) == ['xdk:xvoice', 'game']

    assert _fill('game', None, 'game') == ['game'] * 3

    assert _fill('xdk:libcpmt', 'eh', None, 'xdk:libcpmt') == ['xdk:libcpmt', 'eh', 'xdk:libcpmt', 'xdk:libcpmt']

    assert _fill('xdk:libcpmt', 'eh', 'xdk:libcpmt') == ['xdk:libcpmt', 'eh', 'xdk:libcpmt']





def _eh(code, handlers=()):

    md = Cs(CS_ARCH_X86, CS_MODE_32)

    md.detail = True

    return is_eh_stub(list(md.disasm(code, 0x374000)), set(handlers))





def _jmp_to(va, at):

    """An e9 jmp placed at address `at` that lands on va."""

    return bytes([0xe9]) + (va - (at + 5)).to_bytes(4, 'little', signed=True)





def test_is_eh_stub():

    jmp = bytes.fromhex('e910200000')

    # __ehhandler thunk: only when the target is a known frame handler

    thunk = bytes.fromhex('b848f94500')

    assert _eh(thunk + _jmp_to(0x322093, 0x374005), {0x322093})

    assert not _eh(thunk + _jmp_to(0x322093, 0x374005), {0x400000})

    assert not _eh(thunk + jmp)

    # unwind funclets start with an ebp-relative read

    assert _eh(bytes.fromhex('8d4de8') + jmp)

    assert _eh(bytes.fromhex('8b4df083c110') + jmp)

    # this-adjustors, atexit destructors and register moves are game code

    assert not _eh(bytes.fromhex('83c130') + jmp)

    assert not _eh(bytes.fromhex('b900104000') + jmp)

    assert not _eh(bytes.fromhex('8bc1') + jmp)

    assert not _eh(bytes.fromhex('558bec33c05dc3'))

    assert not _eh(bytes.fromhex('33c0c3'))

    assert not _eh(bytes.fromhex('558bec') + jmp)

    assert not _eh(bytes.fromhex('33c0') + jmp)





def test_check_unique():

    check_unique([dict(va='00001000'), dict(va='00001010')])

    with pytest.raises(SystemExit) as e:

        check_unique([dict(va='00001000'), dict(va='00001000')])

    assert '00001000' in str(e.value)



def test_name_comes_from_the_library_signature():
    from libsig import Signature
    sig = Signature('libcmt', 'strncmp.obj', '_strncmp', b'', b'')
    assert function_name(sig) == '_strncmp'
    assert function_name(None) == ''

def _rows(*pairs):
    return [dict(va=f'{va:08x}', owner=who) for va, who in pairs]


TEXT = (0x10000, 0x400000)


def test_game_end_turns_game_rows_above_it_into_library_code():
    rows = _rows((0x12000, 'game'), (0x2cb8bf, 'game'), (0x2cb8c0, 'game'), (0x2cb900, 'xdk:libcmt'),
                 (0x2cc000, 'third:havok'), (0x2cd000, 'game'), (0x500000, 'game'))
    apply_owners(rows, {'game_end': '0x2cb8c0', 'ranges': []}, TEXT)
    assert [r['owner'] for r in rows] == ['game', 'game', 'other:library', 'xdk:libcmt', 'third:havok',
                                          'other:library', 'game']  # last: outside .text


def test_explicit_ranges_set_their_owner_last():
    rows = _rows((0x2cb900, 'game'), (0x2cba00, 'xdk:libcmt'), (0x2cbc00, 'game'), (0x2cbd00, 'game'),
                 (0x12000, 'game'))
    owners = {'game_end': '0x2cb8c0', 'ranges': [
        {'start': '0x2cb900', 'end': '0x2cbc00', 'owner': 'third:wma', 'note': 'x'},
        {'start': '0x12000', 'end': '0x12001', 'owner': 'xdk:xapi', 'note': 'y'}]}
    apply_owners(rows, owners, TEXT)
    assert [r['owner'] for r in rows] == ['third:wma', 'third:wma', 'other:library', 'other:library', 'xdk:xapi']


def test_no_game_end_changes_nothing(tmp_path):
    rows = _rows((0x2cd000, 'game'))
    apply_owners(rows, {}, TEXT)
    assert rows[0]['owner'] == 'game'
    assert load_owners(str(tmp_path / 'missing.json')) == {}
    (tmp_path / 'o.json').write_text('{"game_end": "0x10", "ranges": []}')
    assert load_owners(str(tmp_path / 'o.json'))['game_end'] == '0x10'


def test_library_signature_below_the_game_code_end_is_game_code():
    # retail 0x22ec84 has CTcpSocket::HasConnectedChild's bytes, but the libraries link after the game
    hits = {0x22ec84: 'HasConnectedChild', 0x2cb8c0: 'deflate', 0x300000: 'memcpy'}
    assert library_hits(hits, 0x2cb8c0) == {0x2cb8c0: 'deflate', 0x300000: 'memcpy'}
    assert library_hits(hits, 0) == hits  # no game_end in owners.json

def test_bungie_code_ends_where_zlibs_compress2_begins():
    # 0x2cb4e2 is the last game function; 0x2cb510, 0x2cb5b0, 0x2cb640 and 0x2cb850 are
    # zlib's compress2, uncompress, deflateInit2_ and deflateReset, which d3dx8.lib defines
    game_end = int(load_owners(OWNERS_JSON)['game_end'], 16)
    assert game_end == 0x2cb510
    rows = read_rows(FUNCTIONS_CSV)
    assert rows[0x2cb4e2]['owner'] == 'game'
    assert [rows[va]['owner'] for va in (0x2cb510, 0x2cb5b0, 0x2cb640, 0x2cb850)] == ['other:library'] * 4
    assert not [va for va, r in rows.items() if va >= game_end and r['owner'] == 'game']


def test_no_inventory_row_starts_at_a_not_start():
    assert not NOT_STARTS & read_rows(FUNCTIONS_CSV).keys()


def test_in_data_is_half_open():
    for start, end in DATA_RANGES:
        assert in_data(start) and in_data(end - 1)
        assert not in_data(start - 1) and not in_data(end)


def test_no_inventory_row_starts_in_data():
    assert not [va for va in read_rows(FUNCTIONS_CSV) if in_data(va)]


@pytest.mark.retail
def test_wmadec_code_reads_its_tables_as_data(retail_xbe):
    from xbe import Xbe
    image = Xbe(retail_xbe)
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    ins = next(md.disasm(image.read(0x38d2be, 16), 0x38d2be))
    assert (ins.mnemonic, ins.op_str) == ('mov', 'edx, dword ptr [edi*4 + 0x392830]')
    assert in_data(0x392830)


@pytest.mark.retail
@pytest.mark.parametrize('va, op_str, target', [
    (0x37a154, 'dword ptr [0x386b0c], 0', 0x386b0c),               # DSOUND
    (0x3d9907, 'esi, dword ptr [0x3e02e8]', 0x3e02e8),             # XNET
    (0x3fd840, 'eax, dword ptr [ecx*4 + 0x4039e8]', 0x4039e8),     # D3D
])
def test_library_code_reads_its_section_tails_as_data(retail_xbe, va, op_str, target):
    from xbe import Xbe
    image = Xbe(retail_xbe)
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    ins = next(md.disasm(image.read(va, 16), va))
    assert (ins.mnemonic, ins.op_str) == ('cmp', op_str)
    assert in_data(target)


def test_every_known_start_has_its_own_row():
    rows = read_rows(FUNCTIONS_CSV)
    starts = sorted(rows)
    for va in KNOWN_STARTS:
        assert va in rows
        before = starts[starts.index(va) - 1]
        assert before + int(rows[before]['size']) <= va
