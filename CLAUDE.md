# CLAUDE.md

The guide for agents working on this repository is AGENTS.md, imported here so
that Claude Code and other agents read the same instructions:

@AGENTS.md

## Claude Code specifics

- Cloud sessions run `.claude/hooks/session-start.sh` at startup, which installs
  the toolchain (AGENTS.md §1). `.claude/settings.json` sets `CC`/`CXX` to
  Clang 21 and pre-approves the usual build, test and git commands.
- Project skills are in `.claude/skills/` (AGENTS.md §8); invoke them by name.
