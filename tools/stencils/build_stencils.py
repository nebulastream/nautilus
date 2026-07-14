#!/usr/bin/env python3
"""Stencil extractor for the tbc copy-and-patch JIT.

Compiles nautilus/src/nautilus/compiler/backends/tbc/jit/TBCStencilSource.cpp
for a target triple with a pinned flag set, parses the object file's sections
and relocations (via llvm-readobj JSON), normalizes each relocation into a
patchable hole, and emits a generated stencil table
(jit/generated/stencils_<arch>_<fmt>.inc) consumed by TBCStencils.cpp.

The extractor is strict: any relocation kind, undefined symbol, or section it
does not recognize is a hard error at generation time, so surprises in
compiler output can never reach the runtime.

Usage:
  build_stencils.py [--clang clang++] [--readobj llvm-readobj] [--only TARGET]

Regenerates every target listed in TARGETS by default (cross-compilation
needs no sysroot: the stencil TU is freestanding). Requires clang >= 19
(preserve_none) and a matching llvm-readobj.
"""

import argparse
import hashlib
import json
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent.parent
SOURCE = REPO_ROOT / "nautilus/src/nautilus/compiler/backends/tbc/jit/TBCStencilSource.cpp"
OUT_DIR = REPO_ROOT / "nautilus/src/nautilus/compiler/backends/tbc/jit/generated"
SHIM_INCLUDE = REPO_ROOT / "tools/stencils/include"

# Pinned stencil compile flags. -mcmodel=large forces absolute 64-bit symbol
# materialization (x86-64: movabs/R_X86_64_64; aarch64: movz/movk chains), so
# every hole is patchable with the target anywhere in the address space.
CFLAGS = [
    "-c",
    "-O2",
    "-std=c++20",
    "-ffreestanding",
    "-nostdlibinc",
    "-fno-pic",
    "-mcmodel=large",
    "-fno-jump-tables",
    "-fno-exceptions",
    "-fno-rtti",
    "-fno-asynchronous-unwind-tables",
    "-fomit-frame-pointer",
    "-fno-stack-protector",
    "-ffunction-sections",
]

TARGETS = {
    "x86_64_elf": "x86_64-unknown-linux-gnu",
    "aarch64_elf": "aarch64-unknown-linux-gnu",
    "arm64_macho": "arm64-apple-macos11",
}

# Hole symbols -> HoleSym enumerator (TBCStencilRuntime.hpp).
HOLE_SYMS = {
    "_JIT_A": "A",
    "_JIT_B": "B",
    "_JIT_C": "C",
    "_JIT_D": "D",
    "_JIT_TARGET": "Target",
    "_JIT_CONTINUE": "Continue",
    "_JIT_CALL_TARGET": "CallTarget",
    "_JIT_SITE": "Site",
    "_JIT_FUNC": "Func",
    "_JIT_UNWIND": "Unwind",
    "tbcJitPushFrame": "HelperPushFrame",
    "tbcJitExtCall": "HelperExtCall",
    "tbcJitIndCall": "HelperIndCall",
}

# Mach-O arm64 relocation type -> HoleKind enumerator. Apple codegen routes
# every external symbol reference through the GOT; the stitcher emulates the
# GOT with per-instruction slots holding the patched values.
MACHO_ARM64_RELOC_KINDS = {
    5: "A64GotLoadPage21",     # ARM64_RELOC_GOT_LOAD_PAGE21
    6: "A64GotLoadPageOff12",  # ARM64_RELOC_GOT_LOAD_PAGEOFF12
}

# ELF relocation type -> HoleKind enumerator.
ELF_RELOC_KINDS = {
    "R_X86_64_64": "Abs64",
    "R_AARCH64_MOVW_UABS_G0_NC": "A64MovwG0",
    "R_AARCH64_MOVW_UABS_G1_NC": "A64MovwG1",
    "R_AARCH64_MOVW_UABS_G2_NC": "A64MovwG2",
    "R_AARCH64_MOVW_UABS_G3": "A64MovwG3",
    "R_AARCH64_JUMP26": "A64Jump26",
    "R_AARCH64_CALL26": "A64Call26",
    "R_AARCH64_ADR_PREL_PG_HI21": "A64AdrpPage21",
    "R_AARCH64_ADD_ABS_LO12_NC": "A64AddLo12",
    "R_AARCH64_LDST32_ABS_LO12_NC": "A64Ldst32Lo12",
    "R_AARCH64_LDST64_ABS_LO12_NC": "A64Ldst64Lo12",
    "R_AARCH64_LDST128_ABS_LO12_NC": "A64Ldst128Lo12",
}

# Sections we silently ignore (metadata; must carry no code or data we need).
IGNORED_SECTIONS = {
    "",
    ".text",
    ".strtab",
    ".symtab",
    ".comment",
    ".note.GNU-stack",
    ".llvm_addrsig",
    ".linker-options",
}

# Synthetic stencils not tied 1:1 to an opcode get "@"-prefixed table names.
SYNTHETIC = {"RET_VALUE", "RET_VOID", "EPILOGUE", "UNWIND"}


def fail(msg):
    print(f"build_stencils: error: {msg}", file=sys.stderr)
    sys.exit(1)


class Hole:
    def __init__(self, offset, kind, sym, addend):
        self.offset = offset
        self.kind = kind
        self.sym = sym
        self.addend = addend


class StencilInfo:
    def __init__(self, name, code, holes):
        self.name = name
        self.code = code
        self.holes = sorted(holes, key=lambda h: h.offset)
        self.continue_jmp = None  # byte offset of an elidable trailing jump


def run(cmd):
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        fail(f"command failed: {' '.join(map(str, cmd))}\n{result.stderr}")
    return result.stdout


def check_clang_version(clang):
    out = run([clang, "--version"])
    first = out.splitlines()[0]
    for token in first.replace("(", " ").replace(")", " ").split():
        if token[0:1].isdigit() and "." in token:
            major = int(token.split(".")[0])
            if major < 19:
                fail(f"clang >= 19 required for preserve_none (found: {first})")
            return
    fail(f"cannot parse clang version from: {first}")


def parse_elf(readobj, obj_path):
    """Returns (stencils, data_image) parsed from an ELF relocatable object."""
    doc = json.loads(run([readobj, "--elf-output-style=JSON", "--sections", "--section-data", "--relocations",
                          "--symbols", str(obj_path)]))[0]

    sections = {}
    for wrapper in doc["Sections"]:
        section = wrapper["Section"]
        sections[section["Index"]] = section

    def section_name(section):
        return section["Name"]["Name"]

    def section_bytes(section):
        return bytes(section["SectionData"]["Bytes"]) if "SectionData" in section else b""

    # Defined function symbols per section; local data symbols for reloc
    # targets that name a symbol inside a rodata section instead of the
    # section itself.
    func_syms = {}
    local_syms = {}
    for wrapper in doc["Symbols"]:
        symbol = wrapper["Symbol"]
        name = symbol["Name"]["Name"]
        sec = symbol["Section"]["Name"]
        if symbol["Type"]["Name"] == "Function" and name.startswith("stencil_"):
            func_syms.setdefault(sec, []).append(symbol)
        elif name and not name.startswith("stencil_"):
            local_syms[name] = symbol

    # Relocations are reported per .rela section; its Info links the target.
    relocs_by_target = {}
    for group in doc.get("Relocations", []):
        rela_section = sections[group["SectionIndex"]]
        target_index = rela_section["Info"]
        relocs_by_target.setdefault(target_index, []).extend(e["Relocation"] for e in group["Relocs"])

    # Data image: every rodata section referenced by any stencil relocation,
    # laid out with its required alignment; holes get image offsets.
    data_image = bytearray()
    data_base = {}

    def data_offset_for(name):
        if name in data_base:
            return data_base[name]
        for section in sections.values():
            if section_name(section) == name:
                align = max(1, section.get("AddressAlignment", 1))
                while len(data_image) % align:
                    data_image.append(0)
                data_base[name] = len(data_image)
                data_image.extend(section_bytes(section))
                return data_base[name]
        fail(f"relocation against unknown data section {name!r}")

    stencils = []
    seen_sections = set()
    for index, section in sections.items():
        name = section_name(section)
        if name.startswith(".rela") or name in IGNORED_SECTIONS:
            continue
        if name.startswith(".rodata"):
            continue  # pulled in on demand via data_offset_for
        if not (name.startswith(".text.stencil_") or name.startswith(".ltext.stencil_")):
            if section.get("Size", 0) != 0 and section["Type"]["Name"] not in ("SHT_NULL",):
                fail(f"unexpected non-empty section {name!r} ({section['Size']} bytes) — "
                     "stencil bodies must not create data/bss/unwind sections")
            continue
        seen_sections.add(name)

        symbols = func_syms.get(name, [])
        if len(symbols) != 1:
            fail(f"section {name!r} must contain exactly one stencil function, found {len(symbols)}")
        symbol = symbols[0]
        if symbol["Value"] != 0:
            fail(f"stencil {symbol['Name']['Name']} does not start its section")
        code = section_bytes(section)[: symbol["Size"]]

        holes = []
        for reloc in relocs_by_target.get(index, []):
            reloc_type = reloc["Type"]["Name"]
            kind = ELF_RELOC_KINDS.get(reloc_type)
            if kind is None:
                fail(f"{name}: unsupported relocation {reloc_type}")
            target = reloc["Symbol"]["Name"]
            addend = reloc.get("Addend", 0)
            if target in HOLE_SYMS:
                holes.append(Hole(reloc["Offset"], kind, HOLE_SYMS[target], addend))
            elif target.startswith(".rodata") or target.startswith(".lrodata"):
                holes.append(Hole(reloc["Offset"], kind, "Data", data_offset_for(target) + addend))
            elif target in local_syms and local_syms[target]["Section"]["Name"].startswith((".rodata", ".lrodata")):
                local = local_syms[target]
                base = data_offset_for(local["Section"]["Name"])
                holes.append(Hole(reloc["Offset"], kind, "Data", base + local["Value"] + addend))
            else:
                fail(f"{name}: relocation against unexpected symbol {target!r} — "
                     "stencils may only reference _JIT_* holes, tbcJit* helpers, and rodata")

        stencil_name = symbol["Name"]["Name"].removeprefix("stencil_")
        if stencil_name in SYNTHETIC:
            stencil_name = "@" + stencil_name
        stencils.append(StencilInfo(stencil_name, code, holes))

    if not stencils:
        fail("no stencil sections found — wrong object file?")
    return stencils, bytes(data_image)


def parse_macho_arm64(obj_path):
    """Returns (stencils, data_image) parsed from a Mach-O arm64 relocatable
    object. Minimal hand-rolled parser (llvm-readobj has no JSON output for
    Mach-O): one __TEXT,__text section sliced at symbol boundaries
    (.subsections_via_symbols), relocations classified per function.

    Apple codegen references every external symbol through the GOT
    (ARM64_RELOC_GOT_LOAD_PAGE21/PAGEOFF12 pairs); the stitcher emulates the
    GOT with per-instruction value slots, so both kinds map 1:1 to holes.
    """
    import struct

    blob = obj_path.read_bytes()
    magic, cputype, _cpusub, filetype, ncmds, _sizeofcmds, _flags, _res = struct.unpack_from("<IiiIIIII", blob, 0)
    if magic != 0xFEEDFACF:
        fail("not a 64-bit Mach-O object")
    if cputype != 0x0100000C:
        fail("not an arm64 Mach-O object")
    if filetype != 1:  # MH_OBJECT
        fail("not a relocatable Mach-O object")

    sections = []  # (sectname, segname, addr, size, fileoff, align, reloff, nreloc)
    symtab = None
    offset = 32
    for _ in range(ncmds):
        cmd, cmdsize = struct.unpack_from("<II", blob, offset)
        if cmd == 0x19:  # LC_SEGMENT_64
            nsects = struct.unpack_from("<I", blob, offset + 64)[0]
            for i in range(nsects):
                so = offset + 72 + i * 80
                sectname = blob[so : so + 16].rstrip(b"\0").decode()
                segname = blob[so + 16 : so + 32].rstrip(b"\0").decode()
                addr, size = struct.unpack_from("<QQ", blob, so + 32)
                fileoff, align, reloff, nreloc = struct.unpack_from("<IIII", blob, so + 48)
                sections.append((sectname, segname, addr, size, fileoff, align, reloff, nreloc))
        elif cmd == 0x2:  # LC_SYMTAB
            symtab = struct.unpack_from("<IIII", blob, offset + 8)  # symoff, nsyms, stroff, strsize
        offset += cmdsize

    if symtab is None:
        fail("Mach-O object has no symbol table")
    symoff, nsyms, stroff, _strsize = symtab

    symbols = []  # (name, n_type, n_sect, n_value)
    for i in range(nsyms):
        n_strx, n_type, n_sect, _n_desc, n_value = struct.unpack_from("<IBBHQ", blob, symoff + i * 16)
        end = blob.index(b"\0", stroff + n_strx)
        symbols.append((blob[stroff + n_strx : end].decode(), n_type, n_sect, n_value))

    text = next((s for s in sections if s[0] == "__text" and s[1] == "__TEXT"), None)
    if text is None:
        fail("Mach-O object has no __TEXT,__text section")
    _, _, text_addr, text_size, text_off, _, text_reloff, text_nreloc = text
    for sectname, segname, _, size, *_rest in sections:
        if size != 0 and (sectname, segname) != ("__text", "__TEXT") and segname != "__LD":
            fail(f"unexpected non-empty Mach-O section {segname},{sectname} — "
                 "stencil bodies must not create data/const sections on arm64-macho")

    # Function boundaries: defined symbols in __text, sorted by address.
    funcs = sorted(((v - text_addr, name) for name, n_type, n_sect, v in symbols
                    if n_sect == 1 and (n_type & 0x0E) == 0x0E and name.startswith("_stencil_")),
                   key=lambda t: t[0])
    if not funcs:
        fail("no stencil symbols found in Mach-O __text")
    bounds = {}
    for i, (start, name) in enumerate(funcs):
        end = funcs[i + 1][0] if i + 1 < len(funcs) else text_size
        bounds[name] = (start, end)

    # Relocations: r_address is relative to the section; extern relocs index
    # the symbol table.
    relocs_by_func = {name: [] for _, name in funcs}
    starts = [start for start, _ in funcs]
    names = [name for _, name in funcs]
    import bisect
    for i in range(text_nreloc):
        r_address, r_info = struct.unpack_from("<iI", blob, text_reloff + i * 8)
        r_symbolnum = r_info & 0xFFFFFF
        r_extern = (r_info >> 27) & 1
        r_type = (r_info >> 28) & 0xF
        kind = MACHO_ARM64_RELOC_KINDS.get(r_type)
        if kind is None:
            fail(f"unsupported Mach-O arm64 relocation type {r_type} at __text+{r_address:#x}")
        if not r_extern:
            fail(f"non-extern Mach-O relocation at __text+{r_address:#x} (literal pool?) — unsupported")
        target = symbols[r_symbolnum][0]
        func = names[bisect.bisect_right(starts, r_address) - 1]
        start, end = bounds[func]
        if not (start <= r_address < end):
            fail(f"relocation at __text+{r_address:#x} outside any stencil")
        # Mach-O prefixes C symbols with '_'.
        plain = target.removeprefix("_")
        if plain not in HOLE_SYMS:
            fail(f"{func}: relocation against unexpected symbol {target!r}")
        relocs_by_func[func].append(Hole(r_address - start, kind, HOLE_SYMS[plain], 0))

    stencils = []
    for start, name in funcs:
        end = bounds[name][1]
        code = blob[text_off + start : text_off + end]
        stencil_name = name.removeprefix("_stencil_")
        if stencil_name in SYNTHETIC:
            stencil_name = "@" + stencil_name
        stencils.append(StencilInfo(stencil_name, code, relocs_by_func[name]))
    return stencils, b""


def detect_continue_jmp_x86(stencil):
    """Trailing `movabs $_JIT_CONTINUE, %reg; jmp *%reg` (12 or 13 bytes)."""
    code = stencil.code
    for hole in stencil.holes:
        if hole.sym != "Continue" or hole.kind != "Abs64":
            continue
        end = hole.offset + 8
        # jmp *%reg is (FF E0+r) for rax..rdi, (41 FF E0+r) for r8..r15.
        if len(code) == end + 2 and code[end] == 0xFF and (code[end + 1] & 0xF8) == 0xE0:
            rex, opc = code[hole.offset - 2], code[hole.offset - 1]
            if rex in (0x48, 0x49) and (opc & 0xF8) == 0xB8:
                movabs_reg = ((rex & 1) << 3) | (opc & 7)
                if movabs_reg == (code[end + 1] & 7):
                    stencil.continue_jmp = hole.offset - 2
        elif (len(code) == end + 3 and code[end] == 0x41 and code[end + 1] == 0xFF
              and (code[end + 2] & 0xF8) == 0xE0):
            rex, opc = code[hole.offset - 2], code[hole.offset - 1]
            if rex == 0x49 and (opc & 0xF8) == 0xB8:
                if (8 | (opc & 7)) == (8 | (code[end + 2] & 7)):
                    stencil.continue_jmp = hole.offset - 2


def detect_continue_jmp_a64(stencil):
    """Trailing `b _JIT_CONTINUE` (one 4-byte instruction)."""
    for hole in stencil.holes:
        if hole.sym == "Continue" and hole.kind == "A64Jump26" and hole.offset + 4 == len(stencil.code):
            stencil.continue_jmp = hole.offset


def detect_continue_jmp_a64_got(stencil):
    """Trailing `adrp/ldr _JIT_CONTINUE@GOT; br` (three 4-byte instructions)."""
    size = len(stencil.code)
    if size < 12:
        return
    page = any(h.sym == "Continue" and h.kind == "A64GotLoadPage21" and h.offset == size - 12 for h in stencil.holes)
    off = any(h.sym == "Continue" and h.kind == "A64GotLoadPageOff12" and h.offset == size - 8 for h in stencil.holes)
    br = int.from_bytes(stencil.code[size - 4 : size], "little")
    if page and off and (br & 0xFFFFFC1F) == 0xD61F0000:
        stencil.continue_jmp = size - 12


def emit_inc(stencils, data_image, target_key, triple, clang_version, out_path):
    stencils = sorted(stencils, key=lambda s: s.name)
    src_sha = hashlib.sha256(SOURCE.read_bytes()).hexdigest()[:16]

    lines = []
    lines.append("// Generated by tools/stencils/build_stencils.py — DO NOT EDIT.")
    lines.append(f"// target: {triple}")
    lines.append(f"// compiler: {clang_version}")
    lines.append(f"// source: TBCStencilSource.cpp sha256:{src_sha}")
    lines.append(f"// stencils: {len(stencils)}, data image: {len(data_image)} bytes")
    lines.append("// clang-format off")
    lines.append("")

    def cname(name):
        return name.replace("@", "SYN_")

    for stencil in stencils:
        byte_rows = []
        for i in range(0, len(stencil.code), 20):
            byte_rows.append(", ".join(f"0x{b:02x}" for b in stencil.code[i : i + 20]))
        lines.append(f"static const unsigned char kCode_{cname(stencil.name)}[] = {{")
        for row in byte_rows:
            lines.append(f"    {row},")
        lines.append("};")
        if stencil.holes:
            lines.append(f"static const StencilHole kHoles_{cname(stencil.name)}[] = {{")
            for hole in stencil.holes:
                lines.append(f"    {{{hole.offset}, HoleKind::{hole.kind}, HoleSym::{hole.sym}, {hole.addend}}},")
            lines.append("};")
        lines.append("")

    lines.append("static const StencilEntry kEntries[] = {")
    for stencil in stencils:
        holes = f"kHoles_{cname(stencil.name)}" if stencil.holes else "nullptr"
        cont = stencil.continue_jmp if stencil.continue_jmp is not None else "kNoContinueJmp"
        lines.append(f'    {{"{stencil.name}", {{kCode_{cname(stencil.name)}, {len(stencil.code)}, '
                     f"{holes}, {len(stencil.holes)}, {cont}}}}},")
    lines.append("};")
    lines.append("")

    if data_image:
        lines.append("static const unsigned char kDataImage[] = {")
        for i in range(0, len(data_image), 20):
            lines.append("    " + ", ".join(f"0x{b:02x}" for b in data_image[i : i + 20]) + ",")
        lines.append("};")
    lines.append("")
    data_ref = "kDataImage, sizeof(kDataImage)" if data_image else "nullptr, 0"
    lines.append("static const StencilTable kTable = {")
    lines.append(f"    kEntries, {len(stencils)}, {data_ref}, StencilAbi::PreserveNone,")
    lines.append("};")
    lines.append("")

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text("\n".join(lines))
    print(f"wrote {out_path} ({len(stencils)} stencils, {sum(len(s.code) for s in stencils)} code bytes)")


def build_target(clang, readobj, target_key, triple, clang_version):
    with tempfile.TemporaryDirectory() as tmp:
        obj_path = Path(tmp) / "stencils.o"
        run([clang, f"--target={triple}", *CFLAGS,
             "-I", str(REPO_ROOT / "nautilus/src"),
             "-I", str(SHIM_INCLUDE),
             str(SOURCE), "-o", str(obj_path)])
        if target_key.endswith("_macho"):
            stencils, data_image = parse_macho_arm64(obj_path)
        else:
            stencils, data_image = parse_elf(readobj, obj_path)

    for stencil in stencils:
        if target_key.startswith("x86_64"):
            detect_continue_jmp_x86(stencil)
        elif target_key.endswith("_macho"):
            detect_continue_jmp_a64_got(stencil)
        else:
            detect_continue_jmp_a64(stencil)

    emit_inc(stencils, data_image, target_key, triple, clang_version, OUT_DIR / f"stencils_{target_key}.inc")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--clang", default=None, help="clang++ >= 19 (default: newest found on PATH)")
    parser.add_argument("--readobj", default=None, help="llvm-readobj (default: matches --clang)")
    parser.add_argument("--only", choices=sorted(TARGETS), help="regenerate a single target")
    args = parser.parse_args()

    clang = args.clang
    readobj = args.readobj
    if clang is None:
        for candidate in ("clang++-21", "clang++-20", "clang++-19", "clang++"):
            if shutil.which(candidate):
                clang = candidate
                break
        else:
            fail("no clang++ found on PATH; pass --clang")
    if readobj is None:
        suffix = clang.removeprefix("clang++")
        readobj = f"llvm-readobj{suffix}" if shutil.which(f"llvm-readobj{suffix}") else "llvm-readobj"

    check_clang_version(clang)
    clang_version = run([clang, "--version"]).splitlines()[0].strip()

    targets = {args.only: TARGETS[args.only]} if args.only else TARGETS
    for target_key, triple in targets.items():
        build_target(clang, readobj, target_key, triple, clang_version)


if __name__ == "__main__":
    main()
