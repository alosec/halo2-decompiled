"""Hardening tests for tools/xiso_extract.py."""
from __future__ import annotations

import hashlib
import struct
import sys
import time
from pathlib import Path

import pytest

import xiso_extract as xiso

SECTOR = xiso.SECTOR
MAGIC = xiso.MAGIC


def _entry(left: int, right: int, start: int, fsize: int, attr: int, name: str) -> bytes:
    name_b = name.encode('latin-1')
    return struct.pack('<HHIIBB', left, right, start, fsize, attr, len(name_b)) + name_b


def _pad_sector(data: bytes) -> bytes:
    if len(data) > SECTOR:
        return data
    return data + b'\x00' * (SECTOR - len(data))


def _image_with_root(root_sector: int, root_bytes: bytes, extra_sectors: dict[int, bytes]) -> bytes:
    last = max([root_sector, *extra_sectors.keys()], default=root_sector)
    size = max(0x10000 + 28, (last + 1) * SECTOR)
    img = bytearray(size)
    img[0x10000:0x10000 + len(MAGIC)] = MAGIC
    struct.pack_into('<II', img, 0x10000 + len(MAGIC), root_sector, len(root_bytes))
    root = _pad_sector(root_bytes) if len(root_bytes) <= SECTOR else root_bytes
    img[root_sector * SECTOR:root_sector * SECTOR + len(root)] = root
    for sector, data in extra_sectors.items():
        chunk = _pad_sector(data) if len(data) <= SECTOR else data
        img[sector * SECTOR:sector * SECTOR + len(chunk)] = chunk
    return bytes(img)


def test_rejects_dotdot_filename(tmp_path: Path) -> None:
    """A crafted \"..\" directory entry must not write outside out_dir."""
    root = _entry(0, 0, 2, 4, 0, '..')
    img = _image_with_root(1, root, {2: b'EVIL'})
    iso = tmp_path / 'evil.iso'
    iso.write_bytes(img)
    out = tmp_path / 'out'
    out.mkdir()
    with pytest.raises(ValueError, match=r'\.\.|unsafe|escape'):
        xiso.extract(str(iso), str(out))
    siblings = [p for p in tmp_path.iterdir() if p.name not in ('evil.iso', 'out')]
    assert siblings == []
    assert list(out.rglob('*')) == []


def test_truncated_image_does_not_hang(tmp_path: Path) -> None:
    """A file size larger than remaining image bytes must not loop forever."""
    root = _entry(0, 0, 2, 1_000_000, 0, 'big.bin')
    img = _image_with_root(1, root, {2: b'short!!!'})
    iso = tmp_path / 'trunc.iso'
    iso.write_bytes(img)
    out = tmp_path / 'out'
    out.mkdir()
    t0 = time.monotonic()
    with pytest.raises(EOFError):
        xiso.extract(str(iso), str(out))
    assert time.monotonic() - t0 < 2.0
    assert list(out.rglob('*')) == []


def test_directory_cycle_does_not_recurse_forever(tmp_path: Path) -> None:
    """A directory whose child points back at a parent must stop."""
    ent_a = _entry(0, 0, 2, SECTOR, 0x10, 'a')
    ent_b = _entry(0, 0, 1, SECTOR, 0x10, 'b')
    img = bytearray(max(0x10000 + 28, 3 * SECTOR))
    img[0x10000:0x10000 + len(MAGIC)] = MAGIC
    struct.pack_into('<II', img, 0x10000 + len(MAGIC), 1, SECTOR)
    img[1 * SECTOR:2 * SECTOR] = _pad_sector(ent_a)
    img[2 * SECTOR:3 * SECTOR] = _pad_sector(ent_b)
    iso = tmp_path / 'cycle.iso'
    iso.write_bytes(img)

    t0 = time.monotonic()
    entries = xiso.list_entries(str(iso))
    assert time.monotonic() - t0 < 2.0
    assert len(entries) < 100


@pytest.mark.parametrize('name', [
    'CON', 'nul', 'Aux.txt', 'com1', 'LPT9.log', 'COM¹', 'con .txt',  # devices
    'a::$DATA', 'a:b', 'what?', 'x<y', 'pipe|name', 'quote"d', 'star*',     # streams, reserved
    'tab\tname', 'trailing.', 'trailing ',                                   # control, altered
])
def test_rejects_names_windows_reads_as_something_else(tmp_path: Path, name: str) -> None:
    img = _image_with_root(1, _entry(0, 0, 2, 4, 0, name), {2: b'EVIL'})
    iso = tmp_path / 'win.iso'
    iso.write_bytes(img)
    out = tmp_path / 'out'
    out.mkdir()
    with pytest.raises(ValueError, match='unsafe'):
        xiso.extract(str(iso), str(out))
    assert list(out.rglob('*')) == []


@pytest.mark.parametrize('name', ['console.txt', 'CONFIG', 'nul_file', 'com10', 'default.xbe', 'a.b.c'])
def test_ordinary_names_still_extract(tmp_path: Path, name: str) -> None:
    img = _image_with_root(1, _entry(0, 0, 2, 4, 0, name), {2: b'DATA'})
    iso = tmp_path / 'ok.iso'
    iso.write_bytes(img)
    out = tmp_path / 'out'
    out.mkdir()
    xiso.extract(str(iso), str(out))
    assert (out / name).read_bytes() == b'DATA'


def test_redump_partition_lists_and_extracts(tmp_path: Path, capsys) -> None:
    """A full-disc partition uses relative sectors without filling the gap."""
    base = 0x18300000
    payload = b'Synthetic XISO payload\x00\xff\n'
    root = _entry(0, 0, 34, len(payload), 0, 'sample.bin')
    iso = tmp_path / 'redump.iso'
    with iso.open('wb') as f:
        f.seek(base)
        f.write(_image_with_root(33, root, {34: payload}))

    assert iso.stat().st_size > base
    # st_blocks counts allocated 512-byte blocks, not the sparse logical size.
    stat = iso.stat()
    if hasattr(stat, 'st_blocks'):
        assert stat.st_blocks * 512 < 1 << 20
    with iso.open('rb') as f:
        assert xiso.find_partition(f) == (base, 33, len(root))
    assert xiso.list_entries(str(iso)) == [('sample.bin', len(payload), 34)]

    out = tmp_path / 'out'
    digest = hashlib.sha256(payload).hexdigest()
    assert xiso.extract(str(iso), str(out)) == [('sample.bin', digest)]
    extracted = out / 'sample.bin'
    assert extracted.read_bytes() == payload
    assert hashlib.sha256(extracted.read_bytes()).hexdigest() == digest
    assert capsys.readouterr().out == f'sample.bin  sha256 {digest}\n'
    assert list(out.iterdir()) == [extracted]


@pytest.mark.parametrize('base', [0, 0x18300000])
@pytest.mark.parametrize('header', [b'', b'\x00' * len(MAGIC), MAGIC[:-1], b'X' + MAGIC[1:]])
@pytest.mark.parametrize('extract', [False, True], ids=['list', 'extract'])
def test_missing_or_invalid_header_exits_cleanly(
    tmp_path: Path, monkeypatch, base: int, header: bytes, extract: bool,
) -> None:
    iso = tmp_path / 'invalid.iso'
    with iso.open('wb') as f:
        if header:
            f.seek(base + 0x10000)
            f.write(header)
    out = tmp_path / 'out'
    argv = ['xiso_extract.py', str(iso)] + ([str(out)] if extract else [])
    monkeypatch.setattr(sys, 'argv', argv)
    with pytest.raises(SystemExit) as exc:
        xiso.main()
    assert exc.value.code == 'not an Xbox disc image (no XDVDFS header found)'
    assert not out.exists()


@pytest.mark.parametrize('base', [0, 0x18300000])
@pytest.mark.parametrize('metadata_size', [0, 4, 7])
@pytest.mark.parametrize('extract', [False, True], ids=['list', 'extract'])
def test_truncated_header_exits_cleanly(
    tmp_path: Path, monkeypatch, base: int, metadata_size: int, extract: bool,
) -> None:
    iso = tmp_path / 'truncated-header.iso'
    with iso.open('wb') as f:
        f.seek(base + 0x10000)
        f.write(MAGIC + struct.pack('<II', 33, SECTOR)[:metadata_size])
    out = tmp_path / 'out'
    argv = ['xiso_extract.py', str(iso)] + ([str(out)] if extract else [])
    monkeypatch.setattr(sys, 'argv', argv)
    with pytest.raises(SystemExit) as exc:
        xiso.main()
    assert exc.value.code == (
        f'error: truncated XISO header at offset {base + 0x10000}: '
        f'wanted 8 bytes of root directory metadata, got {metadata_size}'
    )
    assert not out.exists()
