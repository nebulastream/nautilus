# CI image of the 0.4 measurement jobs

`ci-0.4/Dockerfile` is the image the instruction-count gate (`callgrind-gate` in `.github/workflows/pr.yml`,
`tools/latency/`, plan `10-observability.md` §2.1) runs in, and the one `baselines-0.3` and `latency-corpus` are meant to
run in (decision D9). Callgrind counts are reproducible only when the toolchain, libc and valgrind are fixed; the image
fixes them:

| Pinned | By |
|---|---|
| base image | digest of `ubuntu:24.04` (`FROM ...@sha256:`) |
| Ubuntu packages (libc, libstdc++, valgrind, CMake, GCC 14, ...) | the dated snapshot `UBUNTU_SNAPSHOT` of snapshot.ubuntu.com |
| Clang 21 | exact version `LLVM_VERSION` from apt.llvm.org (the 21 branch is closed), the repository key by `LLVM_KEY_SHA256` |

What a run adds (`tools/latency/callgrind-corpus.sh`): an empty environment, `GLIBC_TUNABLES` below the x86-64 baseline,
`LD_BIND_NOW=1`, `NAUTILUS_DETERMINISTIC=1`, one tier. The gate records a fingerprint of valgrind, libc, libstdc++, the
compiler and the tunables in `nautilus/test/baselines/callgrind-gates.json`; counts and thresholds are compared only
within one fingerprint.

## How the job gets the image

The job builds the Dockerfile itself (`docker/build-push-action` with the GitHub Actions layer cache), runs
`tools/latency/run-callgrind-local.sh --ci` in it and prints the image id in the job summary. Nothing is pulled from a
registry, because the `ci-image` workflow runs only on `nautilus-0.4` and a PR cannot publish packages.

## Changing the image

Changing the Dockerfile (or a pin) changes the toolchain, so the recorded thresholds no longer apply: the job fails
with a different fingerprint. Download the `callgrind-gate-<sha>` artifact of that run and re-record from its counts:

```bash
python3 tools/latency/callgrind-gate.py update --ref 0.3.tsv --new nir.tsv --meta nir.tsv.meta
```

(`update` only lowers thresholds; the fingerprint is replaced, and a ratio that rose needs `--allow-raise`.) Commit the
Dockerfile and `callgrind-gates.json` together.

## What the repository owners still have to do

1. Merge the `ci-image` workflow to `nautilus-0.4` (it publishes `ghcr.io/<repo>/ci-0.4` only from there) and run it once;
   the job summary prints the image digest.
2. Name that digest as the `container:` image of the `callgrind-gate`, `latency-corpus` and `baselines-0.3` jobs
   (`image: ghcr.io/nebulastream/nautilus/ci-0.4@sha256:...`) in place of the build step, and delete the build step.
   Then bump `UBUNTU_SNAPSHOT` only together with a re-record (above).
3. Name the owner and procure the reference machine for the wall-clock targets (plan `16`, risks table; `10-observability.md` §2).
4. After a few stable runs of `callgrind-gate` on the branch, remove its `continue-on-error` and add it to the required
   checks of `nautilus-0.4`.
