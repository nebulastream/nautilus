#!/usr/bin/env python3
"""doc-check: the paths, targets, options and skills that the agent documents name must exist.

Checks every `AGENTS.md`, `CLAUDE.md` and `.claude/skills/*/SKILL.md`:
  * a backticked token that looks like a repository path (it has a `/`, no space, glob or placeholder) exists,
    relative to the repository root or to the document's directory;
  * a backticked `nautilus-...` token that looks like a CMake target or binary is defined by some CMakeLists.txt
    (as a target, a test binary name or an `add_test` name);
  * a backticked option (`engine.x`, `dump.x`, `ir.x`, `asmjit.x`, `tbc.x`, ...) is a row of docs/options.md;
  * a skill named in a table of AGENTS.md exists in .claude/skills.

Usage: doc-check.py [--root DIR] [file ...]       exit status 1 when something is stale
       doc-check.py --self-test                   plants a stale path and requires the check to fail
"""
import argparse
import glob
import os
import re
import subprocess
import sys
import tempfile

TOKEN = re.compile(r"`([^`\n]+)`")
OPTION_PREFIXES = ("engine.", "dump.", "ir.", "mlir.", "bc.", "tbc.", "asmjit.", "perf.", "gpu.")
IGNORE_MARK = "doc-check: ignore"
# paths that are produced by a build or a run, or are examples inside the text
IGNORED_PREFIXES = ("build/", "/", "~", "http", "docs/plans/", "autoresearch/")


def repo_root(start):
    return subprocess.check_output(["git", "rev-parse", "--show-toplevel"], cwd=start, text=True).strip()


def documents(root):
    docs = [p for p in glob.glob(os.path.join(root, "**", "AGENTS.md"), recursive=True)]
    docs += glob.glob(os.path.join(root, "CLAUDE.md"))
    docs += glob.glob(os.path.join(root, ".claude", "skills", "*", "SKILL.md"))
    return sorted(d for d in docs if "/third_party/" not in d and "/build/" not in d)


def cmake_text(root):
    text = []
    for dirpath, dirs, files in os.walk(root):
        dirs[:] = [d for d in dirs if d not in (".git", "build", "third_party", "node_modules")]
        for f in files:
            if f == "CMakeLists.txt" or f.endswith(".cmake") or f == "CMakePresets.json":
                with open(os.path.join(dirpath, f), errors="replace") as fh:
                    text.append(fh.read())
    return "\n".join(text)


def option_names(root):
    names = set()
    path = os.path.join(root, "docs", "options.md")
    if not os.path.exists(path):
        return names
    for line in open(path):
        if line.startswith("|"):
            first = line.split("|")[1].strip()
            for part in first.split(" / "):
                names.add(re.split(r"[=\s]", part.strip())[0])
    return names


def tracked(root):
    out = subprocess.check_output(["git", "ls-files", "-z"], cwd=root, text=True)
    files = [f for f in out.split("\0") if f]
    dirs = set()
    for f in files:
        parts = f.split("/")
        for i in range(1, len(parts)):
            dirs.add("/".join(parts[:i]))
    return set(files), dirs


def exists_anywhere(tok, files, dirs):
    """Whether the token names a tracked file or directory, from the root or as a trailing part of one."""
    t = tok.rstrip("/")
    if t in files or t in dirs:
        return True
    suffix = "/" + t
    return any(f.endswith(suffix) for f in files) or any(d.endswith(suffix) for d in dirs)


def source_options(root):
    """Option names that appear as string literals in the sources (some are not in docs/options.md yet)."""
    found = set()
    pattern = re.compile(r'"((?:engine|dump|ir|mlir|bc|tbc|asmjit|perf|gpu)\.[A-Za-z0-9_.]+)"')
    for dirpath, dirs, fs in os.walk(root):
        dirs[:] = [d for d in dirs if d not in (".git", "build", "third_party", "node_modules")]
        for f in fs:
            if f.endswith((".cpp", ".hpp")):
                with open(os.path.join(dirpath, f), errors="replace") as fh:
                    found.update(pattern.findall(fh.read()))
    return found


def looks_like_path(tok):
    if " " in tok or any(c in tok for c in "*<>{}$()|,;="):
        return False
    if "/" not in tok or tok.startswith(IGNORED_PREFIXES) or tok.startswith("-"):
        return False
    # a path has a plausible last component: a file with an extension or a directory ending in /
    last = tok.rstrip("/").split("/")[-1]
    if last.startswith(".") or not re.fullmatch(r"[A-Za-z0-9_.+-]+", last):
        return False
    return tok.endswith("/") or "." in last


def check(root, files):
    problems = []
    cmake = cmake_text(root)
    options = option_names(root) | source_options(root)
    files_set, dirs_set = tracked(root)
    targets_defined = cmake
    skills = {os.path.basename(os.path.dirname(p)) for p in glob.glob(os.path.join(root, ".claude", "skills", "*", "SKILL.md"))}
    for doc in files:
        rel = os.path.relpath(doc, root)
        base = os.path.dirname(doc)
        in_fence = False
        for number, line in enumerate(open(doc, errors="replace"), 1):
            if line.lstrip().startswith("```"):
                in_fence = not in_fence
                continue
            if in_fence:
                continue
            if IGNORE_MARK in line:
                continue
            for tok in TOKEN.findall(line):
                tok = tok.strip()
                if looks_like_path(tok):
                    candidates = [os.path.join(root, tok), os.path.join(base, tok)]
                    last = tok.rstrip("/").split("/")[-1]
                    is_binary = re.fullmatch(r"nautilus-[a-z0-9-]+", last) and last in cmake
                    if not any(os.path.exists(c) for c in candidates) and not exists_anywhere(tok, files_set, dirs_set) and not is_binary:
                        problems.append(f"{rel}:{number}: path `{tok}` does not exist")
                elif re.fullmatch(r"nautilus-[a-z][a-z0-9-]*", tok):
                    if tok not in cmake and tok not in skills:
                        problems.append(f"{rel}:{number}: target `{tok}` is not defined by any CMakeLists.txt")
                elif tok.startswith(OPTION_PREFIXES) and re.fullmatch(r"[A-Za-z0-9_.]+(=\S+)?", tok):
                    name = tok.split("=")[0]
                    if name not in options and not any(o.startswith(name + ".") for o in options):
                        problems.append(f"{rel}:{number}: option `{name}` is not in docs/options.md")
            # a skill named in a table row of an AGENTS.md: | `skill-name` | ...
            m = re.match(r"\|\s*`([a-z][a-z0-9-]+)`\s*\|", line)
            if m and os.path.basename(doc) == "AGENTS.md" and "skills" in open(doc).read().lower():
                name = m.group(1)
                if name not in skills and name in KNOWN_SKILL_COLUMN_HINTS(doc):
                    problems.append(f"{rel}:{number}: skill `{name}` does not exist in .claude/skills")
    return problems


def KNOWN_SKILL_COLUMN_HINTS(doc):
    # only the table that follows "Project skills" lists skills; other tables hold targets, options and so on
    text = open(doc, errors="replace").read()
    start = text.find("Project skills")
    if start < 0:
        return set()
    section = text[start:text.find("\n\n", text.find("|", start) + 1) + 1 if False else len(text)]
    table = []
    for line in section.split("\n")[1:]:
        if line.startswith("|"):
            table.append(line)
        elif table:
            break
    return {m.group(1) for l in table if (m := re.match(r"\|\s*`([a-z][a-z0-9-]+)`\s*\|", l))}


def self_test():
    """The check passes on a clean tree and fails when a stale path is planted."""
    with tempfile.TemporaryDirectory() as tmp:
        def run(*cmd):
            subprocess.run(cmd, cwd=tmp, check=True, capture_output=True)
        run("git", "init", "-q")
        os.makedirs(os.path.join(tmp, "src"))
        open(os.path.join(tmp, "src", "real.cpp"), "w").write("\n")
        open(os.path.join(tmp, "AGENTS.md"), "w").write("See `src/real.cpp`.\n")
        clean = check(tmp, documents(tmp))
        if clean:
            print("self-test: a clean tree reported problems:", clean)
            return 1
        open(os.path.join(tmp, "AGENTS.md"), "w").write("See `src/real.cpp` and `src/gone.cpp`.\n")
        stale = check(tmp, documents(tmp))
        if not any("src/gone.cpp" in p for p in stale):
            print("self-test: a planted stale path was not found:", stale)
            return 1
    print("self-test ok")
    return 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--root")
    ap.add_argument("--self-test", action="store_true")
    ap.add_argument("files", nargs="*")
    args = ap.parse_args()
    if args.self_test:
        return self_test()
    root = args.root or repo_root(os.getcwd())
    files = [os.path.abspath(f) for f in args.files] or documents(root)
    problems = check(root, files)
    for p in problems:
        print(p)
    print(f"doc-check: {len(files)} documents, {len(problems)} problems")
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
