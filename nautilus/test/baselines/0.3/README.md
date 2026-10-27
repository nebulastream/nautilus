# 0.3 baselines

Recorded from `main` at the commit in `main-commit.txt` by `tools/baselines/record-0.3.sh` (the `baselines-0.3` workflow
runs the same script and uploads the result as an artifact); see M1.10 of `docs/plans/nautilus-0.4/16-implementation-plan.md`.

- `ir-functions.json`: for every function the execution tests compile, the attributes that 0.3's
  `FunctionAttributeInferencePass` derived (`readnone`, `readonly`, `willreturn`, `nounwind`) and the number of
  operations after 0.3's passes, keyed by name and registration site. M3.6 compares inferred bits with these (a bit more
  optimistic than 0.3's is a failure), and the `NirChain` checks of M3.2 and M3.3 compare operation counts.
- `corpus.jsonl`: the latency-corpus driver's output on `main` (three repetitions, the `dev` configuration, one cloud
  container). The absolute times belong to that machine and are a record, not a gate; the IR operation counts and
  allocation counts in it are what later runs are compared against. The reference machine's numbers
  (docs/plans/nautilus-0.4/10-observability.md §2) replace the times once it exists.

Refresh by running the script against a newer `main` and committing the result.
