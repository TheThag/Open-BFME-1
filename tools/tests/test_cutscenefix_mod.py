#!/usr/bin/env python3
"""Structural checks for the 041-cutscenefix retail detour."""
import struct
import subprocess
import sys
import shutil
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))

EXE = ROOT / "baselines/bfme1/workshop-vanilla-1.03/files/lotrbfme.exe"
VC71 = (ROOT / "build/toolchains/vs2003/Program Files/Microsoft Visual Studio .NET 2003"
        / "Vc7/bin/cl.exe")
pytestmark = [
    pytest.mark.skipif(not EXE.exists(), reason="retail baseline not present"),
    pytest.mark.skipif(not VC71.exists(), reason="MSVC 7.1 toolchain not present"),
    pytest.mark.skipif(shutil.which("wine") is None, reason="wine not installed"),
]
capstone = pytest.importorskip("capstone")

from cave import PE  # noqa: E402
from modbuild import TARGET_CUTSCENEFIX_MOVIE  # noqa: E402

FEATURE = "041-cutscenefix"
MOVIE_HELPER_ENTRY = 0x006EE0C0
DISPLACED = bytes.fromhex("8b9604010000")


@pytest.fixture(scope="module")
def built():
    out = Path(tempfile.mkdtemp()) / "cutscenefix.exe"
    result = subprocess.run(
        [sys.executable, str(ROOT / "tools/modbuild.py"), "--only", FEATURE,
         "-o", str(out)], capture_output=True, text=True, cwd=ROOT)
    assert result.returncode == 0, result.stderr or result.stdout
    return PE(out), result.stdout


def _disasm(pe, rva, n=160):
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    return list(md.disasm(pe.read(rva, n), pe.image_base + rva))


def _detour_start(pe):
    assert pe.read(TARGET_CUTSCENEFIX_MOVIE, 1) == b"\xE9"
    displacement = struct.unpack_from("<i", pe.read(TARGET_CUTSCENEFIX_MOVIE + 1, 4))[0]
    return TARGET_CUTSCENEFIX_MOVIE + 5 + displacement


def test_the_retail_site_is_the_final_rectangle_load(built):
    pe, stdout = built
    retail = PE(EXE)
    assert TARGET_CUTSCENEFIX_MOVIE == 0x006EE185
    assert retail.read(TARGET_CUTSCENEFIX_MOVIE, len(DISPLACED)) == DISPLACED
    assert "0x006EE185" in stdout
    assert pe.read(MOVIE_HELPER_ENTRY, 16) == retail.read(MOVIE_HELPER_ENTRY, 16)


def test_the_shim_uses_live_display_and_current_mode(built):
    pe, _ = built
    ins = _disasm(pe, _detour_start(pe))
    assert [i.mnemonic for i in ins[:4]] == ["pushal", "pushfd", "cld", "push"]
    assert bytes(ins[3].bytes) == bytes.fromhex("ff742450")
    assert ins[4].mnemonic == "push" and ins[4].op_str == "esi"
    call = next(i for i in ins if i.mnemonic == "call")
    assert ins[ins.index(call) + 1].mnemonic == "add"
    assert ins[ins.index(call) + 1].op_str.replace(" ", "") == "esp,8"


def test_the_stolen_final_load_is_replayed_before_the_original_draw(built):
    pe, _ = built
    ins = _disasm(pe, _detour_start(pe))
    popal = next(n for n, i in enumerate(ins) if i.mnemonic == "popal")
    assert bytes(ins[popal + 1].bytes) == DISPLACED
    assert ins[popal + 2].mnemonic == "jmp"
    assert int(ins[popal + 2].op_str, 16) - pe.image_base == \
        TARGET_CUTSCENEFIX_MOVIE + len(DISPLACED)


def test_only_the_new_site_and_cave_are_changed(built):
    pe, _ = built
    retail = PE(EXE)
    changed = {i for i in range(len(retail.data)) if pe.data[i] != retail.data[i]}
    header_end = pe.sect_table + pe.n_sections * 40
    site = set(range(pe.rva_to_off(TARGET_CUTSCENEFIX_MOVIE),
                     pe.rva_to_off(TARGET_CUTSCENEFIX_MOVIE) + len(DISPLACED)))
    outside = {i for i in changed if i >= header_end and i not in site}
    cave = next(s for s in pe.sections() if s["name"] == ".bfmemod")
    cave_offsets = set(range(cave["raddr"], cave["raddr"] + cave["rsize"]))
    assert outside <= cave_offsets
    assert retail.read(MOVIE_HELPER_ENTRY, 5) == pe.read(MOVIE_HELPER_ENTRY, 5)


if __name__ == "__main__":
    raise SystemExit(pytest.main([__file__]))
