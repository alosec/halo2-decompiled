"""Whole-image builds must not silently omit their retail vtable input."""

import importlib.util
from pathlib import Path
import subprocess
import sys

import pytest


SCRIPT = Path(__file__).resolve().parents[1] / "port/tools/build_whole_image.py"
spec = importlib.util.spec_from_file_location("whole_image_input", SCRIPT)
builder = importlib.util.module_from_spec(spec)
spec.loader.exec_module(builder)


def test_missing_xbe_explains_required_input(tmp_path):
    with pytest.raises(ValueError, match="H2_RETAIL_XBE"):
        builder.require_retail_xbe(tmp_path / "missing.xbe")


def test_directory_is_not_an_xbe(tmp_path):
    with pytest.raises(ValueError, match="retail XBE is required"):
        builder.require_retail_xbe(tmp_path)


@pytest.mark.parametrize("content", [b"", b"ELF!", b"not an executable"])
def test_wrong_magic_is_rejected(tmp_path, content):
    path = tmp_path / "default.xbe"
    path.write_bytes(content)
    with pytest.raises(ValueError, match="not an XBE"):
        builder.require_retail_xbe(path)


def test_synthetic_header_path_is_resolved(tmp_path):
    # Only input-header validation is tested; this is not a retail executable.
    path = tmp_path / "default.xbe"
    path.write_bytes(b"XBEH")
    assert builder.require_retail_xbe(path) == path.resolve()


def test_cli_fails_before_creating_outputs_or_running_compilers(tmp_path):
    work = tmp_path / "must-not-exist"
    command = [sys.executable, str(SCRIPT), "--root", str(tmp_path),
               "--llvm", str(tmp_path / "no-llvm"), "--lld", "no-linker",
               "--plugin", "no-plugin", "--work", str(work),
               "--start", "no-start", "--linker-script", "no-script"]
    result = subprocess.run(command, text=True, capture_output=True, timeout=10)
    assert result.returncode == 2
    assert "retail XBE is required" in result.stderr
    assert "--xbe" in result.stderr
    assert not work.exists()
