---
name: steward
description: Conventions for pull requests against the nautilus-0.4 branch, for an agent that opens or drives one. Use before acting on CI results or review comments of a 0.4 PR, and when preparing commits for it.
---

# Stewarding a nautilus-0.4 pull request

The plan is `docs/plans/nautilus-0.4/16-implementation-plan.md`; AGENTS.md §7 has the branch rules. In short:

- **Branch and base.** Work on a branch of `nautilus-0.4` and open the PR against `nautilus-0.4`, titled
  `[PR n] <capability>` (plan §4). A PR that needs another one stacks on that PR's branch until it merges.
- **Done means:** the branch builds, the component tests pass, the rules of plan §3 hold. Before the cut-over every
  existing test also passes; after it, `test/api-compat/` too (M1.10). <!-- doc-check: ignore -->
- **Commits.** One logical change per commit, imperative subject, the why in the body. Reference dumps and golden
  images (`nautilus/test/data`) are regenerated in a commit of their own, never with a behavior change.
- **Before every push:** `./format.sh` (clang-format 22), the tests of what you changed (`nautilus-nir-tests` for
  NIR, `ctest --preset dev -j8` before a PR leaves draft), and `python3 tools/doc-check/doc-check.py` if you touched
  a document, a path or an option. A new option needs a row in `docs/options.md` and an entry in
  `nautilus/src/nautilus/common/OptionRegistry.cpp`; the NIR tests check that they agree.
- **Spec first.** A change to NIR's format or rules changes `13-nir-spec.md` in the same PR, before the code.
- **CI.** `pr.yml` runs on `nautilus-0.4`. Red CI is the PR's to fix unless it is red on the base too; never skip or
  quarantine a test to get green. The benchmark bot's ratios vary by ±30% on shared runners; only the 200% alert is a signal.
- **Do not merge or approve** your own PR; say when it waits on reviewers.
