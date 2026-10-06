import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import shutil
import struct
import subprocess
import sys
import tempfile


CASES = ("core-change", "provider-change", "core-missing", "provider-missing")


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def digest(path):
    value = hashlib.sha256()
    with path.open("rb") as source:
        for block in iter(lambda: source.read(1024 * 1024), b""):
            value.update(block)
    return value.hexdigest()


def build_id_note(path):
    notes = []
    with path.open("rb") as source:
        header = source.read(64)
        require(len(header) == 64 and header[:6] == b"\x7fELF\x02\x01", f"not Linux ELF64 little-endian: {path}")
        require(struct.unpack_from("<HH", header, 16) == (3, 62), f"not an actual x86-64 shared library: {path}")
        table = struct.unpack_from("<Q", header, 32)[0]
        entry_size, count = struct.unpack_from("<HH", header, 54)
        require(entry_size >= 56, f"invalid ELF program header size: {path}")
        for index in range(count):
            source.seek(table + index * entry_size)
            entry = source.read(56)
            require(len(entry) == 56, f"truncated ELF program header: {path}")
            kind, _, offset, _, _, size, _, _ = struct.unpack("<IIQQQQQQ", entry)
            if kind != 4:
                continue
            source.seek(offset)
            data = source.read(size)
            require(len(data) == size, f"truncated ELF note: {path}")
            cursor = 0
            while cursor + 12 <= len(data):
                name_size, value_size, note_type = struct.unpack_from("<III", data, cursor)
                name_start = cursor + 12
                value_start = name_start + ((name_size + 3) & ~3)
                end = value_start + ((value_size + 3) & ~3)
                require(end <= len(data), f"malformed ELF note: {path}")
                if note_type == 3 and data[name_start:name_start + name_size].rstrip(b"\0") == b"GNU" and value_size:
                    notes.append((offset + value_start, value_size, data[value_start:value_start + value_size].hex()))
                cursor = end
    require(len(notes) <= 1, f"ambiguous GNU build ID: {path}")
    return notes[0] if notes else None


def image_id(path):
    note = build_id_note(path)
    return note[2] if note else ""


def copy_images(directory, sources, sonames):
    directory.mkdir(mode=0o700)
    images = {}
    for role, source in sources.items():
        image = directory / source.name
        require(not image.exists(), "core and provider library names collide")
        shutil.copy2(source, image)
        soname = sonames[role]
        require(soname and Path(soname).name == soname, f"invalid {role} SONAME")
        if soname != image.name:
            (directory / soname).symlink_to(image.name)
        images[role] = image
    return images


def snapshot(directory):
    result = {}
    for path in directory.iterdir():
        require(path.is_file() and not path.is_symlink(), f"unexpected cache entry: {path}")
        result[path.name] = digest(path)
    return result


def assert_artifacts(directory, key):
    require(len(key) == 64 and all(byte in "0123456789abcdef" for byte in key),
            "cache did not produce a real digest key")
    for extension in (".o", ".mlirbc", ".manifest"):
        path = directory / (key + extension)
        require(path.is_file() and path.stat().st_size > 0, f"missing actual cache artifact: {path}")


def run_child(args, case, step, mode, images, cache, reports):
    report_path = reports / (step + ".report")
    command = [str(args.child), mode, str(cache), str(images["core"]), str(images["provider"]),
               str(reports), report_path.name]
    environment = os.environ.copy()
    environment["LD_LIBRARY_PATH"] = os.pathsep.join(
        [str(images["core"].parent), str(args.core.parent), str(args.provider.parent)]
        + ([environment["LD_LIBRARY_PATH"]] if environment.get("LD_LIBRARY_PATH") else [])
    )
    result = subprocess.run(command, env=environment, capture_output=True, text=True, timeout=120)
    (reports / (step + ".stdout")).write_text(result.stdout)
    (reports / (step + ".stderr")).write_text(result.stderr)
    invocation = {"case": case, "step": step, "command": command, "LD_LIBRARY_PATH": environment["LD_LIBRARY_PATH"],
                  "exit": result.returncode}
    (reports / (step + ".command.json")).write_text(json.dumps(invocation, indent=2) + "\n")
    require(result.returncode == 0, f"{case}/{step} failed ({result.returncode})\n{result.stdout}\n{result.stderr}")
    report = dict(line.split("=", 1) for line in report_path.read_text().splitlines())
    for role, path in images.items():
        require(Path(report[role + ".path"]).samefile(path), f"{role} was loaded from a different library")
        require(report[role + ".build_id"] == image_id(path),
                f"{role} identity did not describe the actual shared image")
    require(report["result"] == "42", "ordinary result was incorrect")
    require((int(report["wrapper_calls"]) == 0) == (mode == "hit"), "independent wrapper count contradicted lookup")
    invocation["report"] = report
    print(json.dumps(invocation, sort_keys=True), flush=True)
    return report


def run_case(args, case, sources, hashes, sonames):
    with tempfile.TemporaryDirectory(prefix="nautilus-cache-shared-image-") as temporary:
        root = Path(temporary)
        cache = root / "cache"
        cache.mkdir(mode=0o700)
        reports = args.evidence_dir / case if args.evidence_dir else root / "reports"
        reports.mkdir(parents=True, exist_ok=True)
        baseline = copy_images(root / "baseline", sources, sonames)
        original = run_child(args, case, "baseline-cold", "miss", baseline, cache, reports)
        assert_artifacts(cache, original["cache.key"])
        warm = run_child(args, case, "baseline-warm", "hit", baseline, cache, reports)
        require(warm["cache.key"] == original["cache.key"], "unchanged shared libraries changed the real cache key")
        before = snapshot(cache)
        altered = copy_images(root / "altered", sources, sonames)
        relocated = run_child(args, case, "relocated-unchanged-warm", "hit", altered, cache, reports)
        require(relocated["cache.key"] == original["cache.key"], "library paths changed the real cache key")
        require(snapshot(cache) == before, "unchanged relocated images modified the cache")
        role, operation = case.split("-", 1)
        target = altered[role]
        note = build_id_note(target)
        require(note is not None, f"actual {role} library was not built with a GNU build ID")
        if operation == "change":
            with target.open("r+b") as image:
                image.seek(note[0])
                byte = image.read(1)
                image.seek(note[0])
                image.write(bytes([byte[0] ^ 1]))
            require(image_id(target) != note[2], "GNU build-ID mutation did not change identity")
            mutation = {"role": role, "operation": "flip-build-id-byte", "file_offset": note[0]}
        else:
            command = [args.objcopy, "--remove-section", ".note.gnu.build-id", str(target)]
            result = subprocess.run(command, capture_output=True, text=True, timeout=120)
            require(result.returncode == 0, f"objcopy failed: {result.stdout}\n{result.stderr}")
            require(build_id_note(target) is None, "objcopy did not remove the actual GNU build-ID note")
            mutation = {"role": role, "operation": "remove-build-id-note", "command": command}
        unchanged = "provider" if role == "core" else "core"
        require(digest(altered[unchanged]) == hashes[unchanged], "the other actual implementation library changed")
        require(digest(target) != hashes[role], "the selected implementation library did not change")
        candidate = run_child(args, case, "altered-warm-lookup", "miss" if operation == "change" else case,
                              altered, cache, reports)
        for image in sources:
            require(candidate[image + ".load_offset"] == original[image + ".load_offset"],
                    "library symbol offset changed")
        require(candidate[unchanged + ".build_id"] == original[unchanged + ".build_id"], "other image identity changed")
        require(candidate[role + ".build_id"] != original[role + ".build_id"], "selected image identity was reused")
        if operation == "change":
            require(candidate["cache.key"] != original["cache.key"],
                    f"{role} build-ID change reused the real warm cache key")
            assert_artifacts(cache, candidate["cache.key"])
            rewritten = snapshot(cache)
            require(all(rewritten.get(name) == value for name, value in before.items()),
                    "original cache artifacts changed")
            expected = {candidate["cache.key"] + extension for extension in (".o", ".mlirbc", ".manifest", ".lock")}
            require(set(rewritten) - set(before) == expected,
                    "independent image change did not create exactly its own cache entry")
            repeated = run_child(args, case, "altered-new-key-warm", "hit", altered, cache, reports)
            require(repeated["cache.key"] == candidate["cache.key"], "altered-image cache entry was not reusable")
        else:
            require(candidate["cache.key"] == "", "missing image build ID produced a real cache key")
            require(snapshot(cache) == before, "missing image build ID readied or published cache files")
            repeated = run_child(args, case, "altered-repeated-decline", case, altered, cache, reports)
            require(repeated["cache.key"] == "" and snapshot(cache) == before,
                    "repeated missing identity used the cache")
        final = run_child(args, case, "baseline-still-warm", "hit", baseline, cache, reports)
        require(final["cache.key"] == original["cache.key"], "original library identities lost their warm entry")
        after = snapshot(cache)
        require(all(after.get(name) == value for name, value in before.items()),
                "original cache artifacts were overwritten")
        summary = {"case": case, "passed": True, "source_hashes": hashes, "mutation": mutation,
                   "baseline": original, "relocated": relocated, "altered": candidate,
                   "repeated": repeated, "final": final,
                   "altered_hashes": {image: digest(path) for image, path in altered.items()}}
        (reports / "summary.json").write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n")
        print(json.dumps(summary, sort_keys=True), flush=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--child", type=Path, required=True)
    parser.add_argument("--core", type=Path, required=True)
    parser.add_argument("--core-soname", required=True)
    parser.add_argument("--provider", type=Path, required=True)
    parser.add_argument("--provider-soname", required=True)
    parser.add_argument("--objcopy", required=True)
    parser.add_argument("--case", choices=(*CASES, "all"), default="all")
    parser.add_argument("--evidence-dir", type=Path)
    args = parser.parse_args()
    if sys.platform != "linux" or platform.machine().lower() not in ("x86_64", "amd64"):
        print("SKIP: actual shared-image compatibility requires Linux x86-64 ELF")
        return 77
    args.child = args.child.resolve(strict=True)
    args.core = args.core.resolve(strict=True)
    args.provider = args.provider.resolve(strict=True)
    sources = {"core": args.core, "provider": args.provider}
    require(not args.core.samefile(args.provider), "core and cache provider must be separate actual shared libraries")
    hashes = {role: digest(path) for role, path in sources.items()}
    for role, path in sources.items():
        require(build_id_note(path) is not None, f"baseline actual {role} library has no GNU build ID")
    sonames = {"core": args.core_soname, "provider": args.provider_soname}
    try:
        for case in CASES if args.case == "all" else (args.case,):
            run_case(args, case, sources, hashes, sonames)
    finally:
        require(all(digest(path) == hashes[role] for role, path in sources.items()),
                "an original shared build output was modified")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, RuntimeError, subprocess.SubprocessError) as error:
        print(f"SharedImageCompatibility: {error}", file=sys.stderr)
        sys.exit(1)
