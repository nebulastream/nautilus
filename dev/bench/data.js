window.BENCHMARK_DATA = {
  "lastUpdate": 1777232204039,
  "repoUrl": "https://github.com/nebulastream/nautilus",
  "entries": {
    "Tracing Benchmark": [
      {
        "commit": {
          "author": {
            "email": "noreply@anthropic.com",
            "name": "Claude",
            "username": "claude"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "90387c94d245cb4dd90e6d8769cb2028de418b37",
          "message": "Add ArenaPool and route IR arenas through it\n\nFollow-up to the IR-arena commit.  Introduces a mutex-synchronized\n`common::ArenaPool` in Arena.hpp that hands out RAII `Handle` objects\n(`unique_ptr<Arena, ArenaPool::Returner>`) and recycles released\nArenas into its spare list instead of freeing them.  A cap\n(DEFAULT_MAX_SPARES = 8) prevents unbounded growth.\n\n`Handle` is the universal Arena-owning smart pointer now:\n- pool-backed: destruction softReset()s the Arena and pushes it back\n  into the pool (falls back to `delete` if the pool is over cap).\n- standalone (pool == nullptr): destruction just `delete`s the Arena,\n  matching the previous `unique_ptr<Arena>` behaviour.\n\n`IRGraph` stores its Arena via `ArenaPool::Handle`.  Two constructors:\none that pulls a freshly heap-allocated standalone Arena (used by\ntests and benchmarks) and one that consumes a pool-backed Handle\n(used on the engine path).\n\nPlumbing through the compilation pipeline:\n- `NautilusEngine` owns a `unique_ptr<ArenaPool>` alongside the\n  existing trace `Arena`.\n- `LegacyCompiler` / `TieredJITCompiler` take an `ArenaPool&` next to\n  their existing `Arena&` and forward it to `TraceToIRConversionPhase::\n  apply`, which now has pool-backed overloads for both the\n  TraceModule and single-ExecutionTrace entry points.\n\nLifecycle story: IRGraph (and thus its Handle) outlives a single\ncompile cycle, including the case where `TieredJITCompiler` caches\nthe IR in `lastCachedIR_` for asynchronous tier-1 promotion.  When\nthe cached IR is finally dropped — on the main thread or inside the\npromotion thread once `waitForPendingPromotions` joins it — the\nHandle hits the pool's mutex and the Arena is recycled.  No\nsoftReset-at-compile-start dependency, so the pool is safe to share\nacross the main and promotion threads.\n\nBenchmark impact (Release, clang-18, ran back-to-back to avoid\nmachine-noise drift):\n- \"IR Creation Benchmark (pooled arena)\" (new) vs the standalone\n  \"IR Creation Benchmark\": pooled is 5-47% faster on most cases,\n  biggest wins on small-to-medium IRs (ir_add -21%, ir_gcd -47%,\n  ir_ifThenElse -20%, ir_loop -23%, ir_nestedIf100 -18%).\n- Backend Compilation Benchmark: essentially flat (BC -2.1%, CPP\n  +0.1%, MLIR +3.1%) as expected — the backend walk itself is\n  unchanged by the pool.\n\nAll 150 tests pass.",
          "timestamp": "2026-04-15T15:43:38+02:00",
          "tree_id": "4552552e437c8719a2100acd98d8e2d7c66b1c19",
          "url": "https://github.com/nebulastream/nautilus/commit/90387c94d245cb4dd90e6d8769cb2028de418b37"
        },
        "date": 1776261068374,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 1084.5800000000002,
            "range": "± 101601",
            "unit": "ns",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.75831,
            "range": "± 195.627",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.32338,
            "range": "± 336.818",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.9224,
            "range": "± 282.181",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.31263,
            "range": "± 647.42",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.01825,
            "range": "± 245.074",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.79482,
            "range": "± 348.9",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.34742,
            "range": "± 150.083",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.0563,
            "range": "± 240.528",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.60731,
            "range": "± 244.059",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.2594,
            "range": "± 1.09807",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 139.42,
            "range": "± 10.2277",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 25.1263,
            "range": "± 2.72068",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 254.521,
            "range": "± 9.54626",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 186.924,
            "range": "± 10.2777",
            "unit": "ns",
            "extra": "100 samples\n158 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 457.805,
            "range": "± 28.3194",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.13692,
            "range": "± 111.204",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 496.746,
            "range": "± 29.555",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 884.904,
            "range": "± 49.9173",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 497.335,
            "range": "± 27.083",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 590.462,
            "range": "± 36.2386",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 436.974,
            "range": "± 20.6036",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 515.733,
            "range": "± 43.5152",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 451.565,
            "range": "± 28.1708",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ir_pooled_add",
            "value": 812.489,
            "range": "± 48.5223",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ir_pooled_ifThenElse",
            "value": 2.02109,
            "range": "± 169.526",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_pooled_deeplyNestedIfElse",
            "value": 5.05552,
            "range": "± 392.698",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_pooled_loop",
            "value": 2.20763,
            "range": "± 150.893",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_pooled_ifInsideLoop",
            "value": 4.13115,
            "range": "± 320.792",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_pooled_loopDirectCall",
            "value": 2.36923,
            "range": "± 171.558",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_pooled_pointerLoop",
            "value": 2.86536,
            "range": "± 188.36",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_pooled_staticLoop",
            "value": 1.79838,
            "range": "± 205.579",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pooled_fibonacci",
            "value": 2.36223,
            "range": "± 228.743",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_pooled_gcd",
            "value": 1.95854,
            "range": "± 113.702",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_pooled_nestedIf10",
            "value": 12.6976,
            "range": "± 955.949",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_pooled_nestedIf100",
            "value": 126.004,
            "range": "± 6.38694",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_pooled_chainedIf10",
            "value": 22.6826,
            "range": "± 1.98616",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_pooled_chainedIf100",
            "value": 235.906,
            "range": "± 16.3151",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 34.7859,
            "range": "± 3.69026",
            "unit": "ns",
            "extra": "100 samples\n862 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 283.706,
            "range": "± 17.3791",
            "unit": "ns",
            "extra": "100 samples\n8310 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.02445,
            "range": "± 0.734763",
            "unit": "ns",
            "extra": "100 samples\n7759 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.2366,
            "range": "± 1.89024",
            "unit": "ns",
            "extra": "100 samples\n740 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 43.3497,
            "range": "± 5.39753",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.22969,
            "range": "± 50.0128",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 24.5568,
            "range": "± 639.038",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 45.4018,
            "range": "± 11.6754",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 64.0067,
            "range": "± 12.3319",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.3575,
            "range": "± 160.678",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.4425,
            "range": "± 419.054",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 64.3394,
            "range": "± 10.9811",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.20026,
            "range": "± 39.2954",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.75528,
            "range": "± 258.461",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.65956,
            "range": "± 39.8325",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.85337,
            "range": "± 914.69",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.9361,
            "range": "± 127.578",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.4324,
            "range": "± 210.809",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.0478,
            "range": "± 97.8766",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.61926,
            "range": "± 38.5706",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.0425,
            "range": "± 122.724",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.037,
            "range": "± 52.4397",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.1034,
            "range": "± 130.529",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.2735,
            "range": "± 188.788",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.1342,
            "range": "± 43.9335",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.6251,
            "range": "± 238.21",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.25,
            "range": "± 244.004",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 24.8431,
            "range": "± 252.801",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 25.966,
            "range": "± 356.128",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 24.9592,
            "range": "± 412.459",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 25.8977,
            "range": "± 248.915",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.28,
            "range": "± 269.68",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.5174,
            "range": "± 402.238",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.663,
            "range": "± 212.678",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.0143,
            "range": "± 243.816",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 24.8266,
            "range": "± 278.456",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 27.8527,
            "range": "± 261.202",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 60.9336,
            "range": "± 565.31",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.1422,
            "range": "± 239.374",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 90.4828,
            "range": "± 447.379",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.0474,
            "range": "± 1.68074",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.3954,
            "range": "± 1.56433",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.6756,
            "range": "± 2.31487",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 17.8752,
            "range": "± 2.52543",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.2906,
            "range": "± 2.71779",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.5852,
            "range": "± 3.31956",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.3349,
            "range": "± 3.07779",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.3348,
            "range": "± 2.5848",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 16.6641,
            "range": "± 1.50384",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 16.5126,
            "range": "± 2.08332",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 29.1303,
            "range": "± 2.94041",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 145.671,
            "range": "± 7.1221",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 41.6457,
            "range": "± 7.0057",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 220.297,
            "range": "± 12.4825",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.0098,
            "range": "± 3.08982",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.573,
            "range": "± 3.61918",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 54.5448,
            "range": "± 6.57272",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 33.963,
            "range": "± 3.4591",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 54.2304,
            "range": "± 5.49732",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 43.7414,
            "range": "± 4.82039",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 45.4236,
            "range": "± 5.28499",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.1266,
            "range": "± 3.84473",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 41.3166,
            "range": "± 4.90837",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 34.5046,
            "range": "± 4.39597",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 98.263,
            "range": "± 7.70696",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.06111,
            "range": "± 12.1277",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 147.948,
            "range": "± 6.11301",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.15931,
            "range": "± 23.8307",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.23465,
            "range": "± 116.489",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.23514,
            "range": "± 134.828",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.49415,
            "range": "± 574.899",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.53028,
            "range": "± 267.87",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.5625,
            "range": "± 1.75983",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.5022,
            "range": "± 1.20767",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.36554,
            "range": "± 647.628",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.58684,
            "range": "± 453.846",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.6274,
            "range": "± 1.8672",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.40456,
            "range": "± 589.385",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.61559,
            "range": "± 632.798",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.67837,
            "range": "± 333.225",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.972,
            "range": "± 1.74621",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.0012,
            "range": "± 1.41726",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.17814,
            "range": "± 640.237",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.25349,
            "range": "± 544.425",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.87766,
            "range": "± 1.51937",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.99987,
            "range": "± 644.124",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.85605,
            "range": "± 1.38311",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.82887,
            "range": "± 364.729",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 37.3899,
            "range": "± 5.57429",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 37.6438,
            "range": "± 3.53812",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.34552,
            "range": "± 30.0272",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.34859,
            "range": "± 15.0579",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 96.6807,
            "range": "± 5.12403",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 47.8409,
            "range": "± 3.65634",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.42707,
            "range": "± 74.5615",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.27005,
            "range": "± 42.4258",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 44.1427,
            "range": "± 5.45675",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.08762,
            "range": "± 45.7122",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.451,
            "range": "± 0.760373",
            "unit": "ns",
            "extra": "100 samples\n8466 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.6396,
            "range": "± 1.3649",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 529.325,
            "range": "± 14.5418",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.67241,
            "range": "± 0.797046",
            "unit": "ns",
            "extra": "100 samples\n6585 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.3407,
            "range": "± 8.11355",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0301,
            "range": "± 575.059",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.5659,
            "range": "± 4.05336",
            "unit": "ns",
            "extra": "100 samples\n684 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 826.119,
            "range": "± 10.3436",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 176.849,
            "range": "± 4.84722",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.52046,
            "range": "± 0.466975",
            "unit": "ns",
            "extra": "100 samples\n8490 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.2278,
            "range": "± 1.4628",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.61219,
            "range": "± 18.4995",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "noreply@anthropic.com",
            "name": "Claude",
            "username": "claude"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "265d035ad25cb46d2cb3aa83d03bb1161a10adad",
          "message": "Always pass Arena as first create<T> arg in addOperation\n\nBasicBlock::addOperation<T> used to dispatch through an\n`if constexpr (is_constructible_v<T, Arena&, ...>)` to conditionally\ninject the owning arena into the operation's constructor. The extra\nbranch was load-bearing for only seven ops that had no SSA inputs to\nallocate — Alloca, Const{Int,Bool,Float,Ptr}, FunctionAddressOf, and\nthe void ReturnOperation — which each built their own constructor\nsignature that omitted Arena&.\n\nDrop the SFINAE and make the factory unconditional:\n\n    arena_->create<T>(*arena_, std::forward<Args>(args)...);\n\nThe seven previously-arenaless constructors now accept (and ignore) a\nleading `common::Arena&` so the call site is uniform. This keeps the\nfactory dead simple, makes the invariant obvious at every construction\nsite (\"arena always comes first\"), and leaves zero wiggle room for a\nnew op to accidentally pick up the no-Arena overload and allocate its\ninputs off the heap.\n\nBranchOperation stays unchanged because it is only constructed\ndirectly through `arena_->create<BranchOperation>()` in\nBasicBlock::addNextBlock, not through the addOperation template.",
          "timestamp": "2026-04-15T23:32:27+02:00",
          "tree_id": "26c5fe4b4d8dfcb85f002dcecda109b7c35e06a6",
          "url": "https://github.com/nebulastream/nautilus/commit/265d035ad25cb46d2cb3aa83d03bb1161a10adad"
        },
        "date": 1776289123602,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 196.917,
            "range": "± 16.3506",
            "unit": "ns",
            "extra": "100 samples\n157 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 499.164,
            "range": "± 78.7904",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.23439,
            "range": "± 332.086",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 506.68,
            "range": "± 69.116",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 940.39,
            "range": "± 122.244",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 517.33,
            "range": "± 71.6739",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 627.411,
            "range": "± 86.8859",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 463.523,
            "range": "± 55.0054",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 538.082,
            "range": "± 61.0147",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 479.921,
            "range": "± 53.1248",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 36.0367,
            "range": "± 4.58947",
            "unit": "ns",
            "extra": "100 samples\n860 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 315.964,
            "range": "± 10.8308",
            "unit": "ns",
            "extra": "100 samples\n8293 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.1493,
            "range": "± 0.847854",
            "unit": "ns",
            "extra": "100 samples\n7770 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.2766,
            "range": "± 1.98398",
            "unit": "ns",
            "extra": "100 samples\n742 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 9.48003,
            "range": "± 272.76",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.86736,
            "range": "± 265.446",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.71618,
            "range": "± 174.636",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 11.1469,
            "range": "± 564.091",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 33.6831,
            "range": "± 627.356",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 16.2643,
            "range": "± 282.354",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 33.0415,
            "range": "± 588.558",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.42852,
            "range": "± 194.064",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.6069,
            "range": "± 169.267",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.9732,
            "range": "± 222.459",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 14.0073,
            "range": "± 211.136",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 29.2202,
            "range": "± 190.802",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.4361,
            "range": "± 219.214",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 24.6322,
            "range": "± 266.381",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.0982,
            "range": "± 446.852",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.417,
            "range": "± 896.404",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.6384,
            "range": "± 600.668",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.2138,
            "range": "± 432.811",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.5922,
            "range": "± 505.213",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.9479,
            "range": "± 466.51",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.2957,
            "range": "± 778.798",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.0927,
            "range": "± 459.521",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.5884,
            "range": "± 465.52",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.0346,
            "range": "± 637.381",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.6459,
            "range": "± 439.505",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.8484,
            "range": "± 2.00252",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.7467,
            "range": "± 723.153",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.3215,
            "range": "± 697.112",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.4046,
            "range": "± 2.68956",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.3473,
            "range": "± 2.41846",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.0472,
            "range": "± 3.30563",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.2737,
            "range": "± 3.1157",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 19.091,
            "range": "± 3.23542",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 16.5599,
            "range": "± 2.30698",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 18.4955,
            "range": "± 3.61692",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 15.7198,
            "range": "± 3.18069",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 16.2977,
            "range": "± 2.55181",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 16.4493,
            "range": "± 2.97242",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 28.5936,
            "range": "± 6.19608",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 138.509,
            "range": "± 10.3287",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 34.8279,
            "range": "± 4.15074",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 212.52,
            "range": "± 17.9544",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 20.7846,
            "range": "± 5.33522",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.525,
            "range": "± 5.58011",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 54.65,
            "range": "± 11.1033",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 34.9476,
            "range": "± 5.84759",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 55.2734,
            "range": "± 10.1808",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 42.874,
            "range": "± 7.8909",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 44.7679,
            "range": "± 7.77137",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 27.6785,
            "range": "± 4.93241",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 41.46,
            "range": "± 8.33293",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 34.4832,
            "range": "± 5.9212",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 97.1155,
            "range": "± 12.9297",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.0714,
            "range": "± 20.1808",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 152.804,
            "range": "± 19.5019",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.29002,
            "range": "± 78.0787",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 43.299,
            "range": "± 13.3715",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.61297,
            "range": "± 277.434",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_pooled_add",
            "value": 650.617,
            "range": "± 76.8701",
            "unit": "ns",
            "extra": "100 samples\n50 iterations"
          },
          {
            "name": "ir_pooled_ifThenElse",
            "value": 1.49455,
            "range": "± 165.156",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_pooled_deeplyNestedIfElse",
            "value": 3.61292,
            "range": "± 595.791",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pooled_loop",
            "value": 1.61111,
            "range": "± 191.055",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_pooled_ifInsideLoop",
            "value": 3.00108,
            "range": "± 397.999",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_pooled_loopDirectCall",
            "value": 1.75606,
            "range": "± 196.993",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_pooled_pointerLoop",
            "value": 2.10423,
            "range": "± 397.343",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pooled_staticLoop",
            "value": 1.32949,
            "range": "± 144.701",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_pooled_fibonacci",
            "value": 1.75069,
            "range": "± 232.493",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_pooled_gcd",
            "value": 1.4139,
            "range": "± 161.469",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_pooled_nestedIf10",
            "value": 8.66757,
            "range": "± 849.242",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_pooled_nestedIf100",
            "value": 109.385,
            "range": "± 5.89268",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_pooled_chainedIf10",
            "value": 15.0216,
            "range": "± 1.5889",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_pooled_chainedIf100",
            "value": 201.497,
            "range": "± 12.3913",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 43.9096,
            "range": "± 11.0875",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 7.01032,
            "range": "± 214.714",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 28.0657,
            "range": "± 791.257",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 42.9404,
            "range": "± 9.81064",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 60.7734,
            "range": "± 13.5594",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 9.34531,
            "range": "± 362.339",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 28.6291,
            "range": "± 747.797",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 60.3933,
            "range": "± 13.1997",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.4133,
            "range": "± 1.27449",
            "unit": "ns",
            "extra": "100 samples\n8475 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 18.1261,
            "range": "± 1.93621",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 612.219,
            "range": "± 30.1952",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.68055,
            "range": "± 0.495148",
            "unit": "ns",
            "extra": "100 samples\n6549 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.6446,
            "range": "± 12.1865",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0849,
            "range": "± 415.764",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 51.4019,
            "range": "± 12.1713",
            "unit": "ns",
            "extra": "100 samples\n620 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 902.937,
            "range": "± 12.8746",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 190.761,
            "range": "± 263.241",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.46647,
            "range": "± 0.285062",
            "unit": "ns",
            "extra": "100 samples\n8479 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.3175,
            "range": "± 1.54752",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.62094,
            "range": "± 24.0413",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.27954,
            "range": "± 234.231",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.29159,
            "range": "± 209.001",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.74021,
            "range": "± 1.29935",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.65401,
            "range": "± 536.438",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.9039,
            "range": "± 3.12117",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.8296,
            "range": "± 1.91869",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.64655,
            "range": "± 1.21152",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.90058,
            "range": "± 874.956",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.8517,
            "range": "± 2.69233",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.45491,
            "range": "± 1.03911",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.72343,
            "range": "± 1.28844",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.81391,
            "range": "± 534.65",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.3676,
            "range": "± 2.57263",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.3048,
            "range": "± 1.7915",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.33791,
            "range": "± 830.738",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.6624,
            "range": "± 889.451",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.2135,
            "range": "± 1.65844",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.06276,
            "range": "± 784.586",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.81886,
            "range": "± 1.07183",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.11803,
            "range": "± 558.916",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 39.0547,
            "range": "± 7.71688",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 38.889,
            "range": "± 7.69189",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.34217,
            "range": "± 50.6505",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.35835,
            "range": "± 57.4087",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 102.858,
            "range": "± 21.2941",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 48.57,
            "range": "± 7.41635",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.42508,
            "range": "± 45.1157",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.26551,
            "range": "± 48.0409",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 767.751,
            "range": "± 99.4112",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.69057,
            "range": "± 185.009",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.83131,
            "range": "± 302.162",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.79853,
            "range": "± 221.886",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 3.20298,
            "range": "± 366.332",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.89926,
            "range": "± 224.441",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.2581,
            "range": "± 242.55",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.57155,
            "range": "± 190.86",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.90635,
            "range": "± 157.63",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.64413,
            "range": "± 196.321",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 10.3737,
            "range": "± 1.57612",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 113.447,
            "range": "± 5.93201",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 16.8451,
            "range": "± 2.29688",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 219.353,
            "range": "± 13.1134",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "noreply@anthropic.com",
            "name": "Claude",
            "username": "claude"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "bc3ce09db1a4d4ed7fe62a7017c15beccb6c782a",
          "message": "Use ArenaPool in all tracing benchmarks\n\nRoute every benchmark's trace arena (and every IRGraph's arena, where\napplicable) through a per-test ArenaPool so chunk memory and inline\nbuffers are recycled across samples and iterations.  This matches the\namortisation pattern the engine relies on in production.\n\nRemoves the dedicated \"IR Creation Benchmark (pooled arena)\" case,\nwhich is now redundant because the main IR Creation Benchmark routes\nIR arenas through a pool as well.",
          "timestamp": "2026-04-16T00:12:12+02:00",
          "tree_id": "82f83695be6f5ef4aaac4e4b602e2bec30b2a67d",
          "url": "https://github.com/nebulastream/nautilus/commit/bc3ce09db1a4d4ed7fe62a7017c15beccb6c782a"
        },
        "date": 1776291417569,
        "tool": "catch2",
        "benches": [
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 38.5289,
            "range": "± 13.1385",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.99092,
            "range": "± 172.172",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 9.11584,
            "range": "± 303.343",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.71465,
            "range": "± 212.569",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.90667,
            "range": "± 491.624",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.9042,
            "range": "± 507.195",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 33.6232,
            "range": "± 781.448",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.7319,
            "range": "± 329.025",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 32.4084,
            "range": "± 925.036",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 9.65385,
            "range": "± 3.58598",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.5555,
            "range": "± 319.14",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 13.1268,
            "range": "± 347.393",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 14.1022,
            "range": "± 276.128",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.9065,
            "range": "± 1.29599",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.9503,
            "range": "± 678.821",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 24.9615,
            "range": "± 770.792",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.36605,
            "range": "± 206.737",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.3622,
            "range": "± 203.348",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.37645,
            "range": "± 1.54181",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.61286,
            "range": "± 588.954",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.2198,
            "range": "± 3.73114",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.5444,
            "range": "± 2.08831",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.19829,
            "range": "± 1.60949",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.68641,
            "range": "± 697.311",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 15.9201,
            "range": "± 2.71233",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.46334,
            "range": "± 1.38964",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.29852,
            "range": "± 1.60655",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.78092,
            "range": "± 667.725",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.2993,
            "range": "± 3.24653",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.1432,
            "range": "± 1.381",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.45062,
            "range": "± 805.978",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.49967,
            "range": "± 824.341",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.49033,
            "range": "± 1.57567",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.05992,
            "range": "± 685.762",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.54655,
            "range": "± 1.17304",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.89617,
            "range": "± 464.396",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 39.2913,
            "range": "± 9.27663",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.1743,
            "range": "± 7.89401",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.46213,
            "range": "± 36.1938",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.55809,
            "range": "± 275.82",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 97.9767,
            "range": "± 12.2718",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 49.7077,
            "range": "± 9.3719",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.45886,
            "range": "± 271.89",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.24515,
            "range": "± 42.1545",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 9.50961,
            "range": "± 0.634683",
            "unit": "ns",
            "extra": "100 samples\n9767 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 16.196,
            "range": "± 1.35205",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 575.016,
            "range": "± 64.6161",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.55031,
            "range": "± 0.555062",
            "unit": "ns",
            "extra": "100 samples\n7215 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 109.278,
            "range": "± 8.43512",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.5752,
            "range": "± 57.5588",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 42.9673,
            "range": "± 2.82028",
            "unit": "ns",
            "extra": "100 samples\n668 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 667.277,
            "range": "± 22.8596",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 152.741,
            "range": "± 13.18",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.55304,
            "range": "± 0.217828",
            "unit": "ns",
            "extra": "100 samples\n8807 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.3163,
            "range": "± 1.88468",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.13294,
            "range": "± 30.0062",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 37.6762,
            "range": "± 13.6229",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.92101,
            "range": "± 224.702",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 28.5126,
            "range": "± 902.347",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 84.7525,
            "range": "± 50.1147",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 50.9835,
            "range": "± 16.5228",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 9.60613,
            "range": "± 1.29522",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 28.9684,
            "range": "± 929.214",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 51.0166,
            "range": "± 16.4943",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 180.594,
            "range": "± 9.18788",
            "unit": "ns",
            "extra": "100 samples\n173 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 436.43,
            "range": "± 33.8256",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.11312,
            "range": "± 65.6895",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 451.425,
            "range": "± 21.7922",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 870.621,
            "range": "± 75.4506",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 470.672,
            "range": "± 30.9386",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 560.286,
            "range": "± 35.4656",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 437.643,
            "range": "± 31.9128",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 493.484,
            "range": "± 66.4201",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 423.702,
            "range": "± 59.1071",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ir_add",
            "value": 612.558,
            "range": "± 34.625",
            "unit": "ns",
            "extra": "100 samples\n51 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.46265,
            "range": "± 72.2078",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.31545,
            "range": "± 196.844",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.52774,
            "range": "± 99.4745",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.76332,
            "range": "± 289.919",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.66976,
            "range": "± 87.6731",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.85704,
            "range": "± 138.025",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.32955,
            "range": "± 121.085",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.6379,
            "range": "± 90.9151",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.35432,
            "range": "± 82.1664",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.45952,
            "range": "± 370.592",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 88.6018,
            "range": "± 4.92842",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.998,
            "range": "± 905.5",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 171.236,
            "range": "± 11.6037",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 36.3346,
            "range": "± 3.25743",
            "unit": "ns",
            "extra": "100 samples\n881 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 276.467,
            "range": "± 12.5007",
            "unit": "ns",
            "extra": "100 samples\n9552 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.98546,
            "range": "± 0.842416",
            "unit": "ns",
            "extra": "100 samples\n8688 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.6727,
            "range": "± 2.58954",
            "unit": "ns",
            "extra": "100 samples\n809 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "763a723d6881502afe1dc8e16cad61c6a70feb3f",
          "message": "Add IR pass manager infrastructure and empty-block elimination (#255)",
          "timestamp": "2026-04-16T08:49:56+02:00",
          "tree_id": "e851e721a8522460a72a97c62aa59da71ba430dc",
          "url": "https://github.com/nebulastream/nautilus/commit/763a723d6881502afe1dc8e16cad61c6a70feb3f"
        },
        "date": 1776322544571,
        "tool": "catch2",
        "benches": [
          {
            "name": "tiered_compile_addOne",
            "value": 43.3147,
            "range": "± 9.89391",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.12484,
            "range": "± 161.013",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 24.6134,
            "range": "± 566.604",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 44.3935,
            "range": "± 9.9115",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 62.0047,
            "range": "± 13.3074",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 9.03237,
            "range": "± 323.784",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 26.3172,
            "range": "± 903.662",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 61.3854,
            "range": "± 10.6178",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 42.7964,
            "range": "± 6.67415",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.15732,
            "range": "± 205.127",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.4168,
            "range": "± 2.01516",
            "unit": "ns",
            "extra": "100 samples\n8339 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.2191,
            "range": "± 1.42035",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 540.572,
            "range": "± 21.1513",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.78582,
            "range": "± 0.616622",
            "unit": "ns",
            "extra": "100 samples\n6548 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.0315,
            "range": "± 8.52595",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9761,
            "range": "± 194.218",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 46.1587,
            "range": "± 10.4978",
            "unit": "ns",
            "extra": "100 samples\n705 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 803.783,
            "range": "± 49.3387",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 168.489,
            "range": "± 817.758",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.71552,
            "range": "± 0.6933",
            "unit": "ns",
            "extra": "100 samples\n8508 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.1283,
            "range": "± 3.57245",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.70274,
            "range": "± 19.7906",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 205.985,
            "range": "± 27.8473",
            "unit": "ns",
            "extra": "100 samples\n159 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 519.276,
            "range": "± 90.6731",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.24415,
            "range": "± 162.005",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 530.057,
            "range": "± 80.2222",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 961.545,
            "range": "± 121.03",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 546.808,
            "range": "± 95.4324",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 630.558,
            "range": "± 83.2931",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 493.008,
            "range": "± 93.5544",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 535.272,
            "range": "± 73.5706",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 483.175,
            "range": "± 67.2249",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ir_add",
            "value": 678.849,
            "range": "± 90.53",
            "unit": "ns",
            "extra": "100 samples\n48 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.66762,
            "range": "± 231.835",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.63285,
            "range": "± 567.166",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.97287,
            "range": "± 374.595",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.97638,
            "range": "± 462.189",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.93718,
            "range": "± 159.454",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.07988,
            "range": "± 312.624",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.51684,
            "range": "± 282.521",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.76936,
            "range": "± 201.541",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.70408,
            "range": "± 235.963",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.73085,
            "range": "± 972.766",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 87.3968,
            "range": "± 6.22892",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.4329,
            "range": "± 1.82595",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 172.409,
            "range": "± 7.48893",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.03052,
            "range": "± 159.57",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.61422,
            "range": "± 182.303",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.85485,
            "range": "± 339.191",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.92641,
            "range": "± 317.902",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.9328,
            "range": "± 498.744",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.5224,
            "range": "± 343.902",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.751,
            "range": "± 797.4",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.69974,
            "range": "± 229.757",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.3606,
            "range": "± 407.578",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.9105,
            "range": "± 282.166",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.0071,
            "range": "± 291.172",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.8641,
            "range": "± 669.068",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.9929,
            "range": "± 777.259",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.8378,
            "range": "± 489.073",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.4784,
            "range": "± 516.393",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.142,
            "range": "± 508.197",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.4055,
            "range": "± 461.761",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.0555,
            "range": "± 885.664",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.2153,
            "range": "± 813.285",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.201,
            "range": "± 618.087",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.8509,
            "range": "± 751.096",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.4717,
            "range": "± 597.816",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.1,
            "range": "± 446.39",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.2302,
            "range": "± 560.427",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.7004,
            "range": "± 713.17",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.5179,
            "range": "± 862.408",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.2728,
            "range": "± 782.72",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.3002,
            "range": "± 1.53139",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.3603,
            "range": "± 3.17149",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.2053,
            "range": "± 2.61894",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 19.7193,
            "range": "± 3.53065",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.0409,
            "range": "± 2.875",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 19.4437,
            "range": "± 4.46589",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 17.8205,
            "range": "± 4.74345",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 18.4514,
            "range": "± 4.12273",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 15.8287,
            "range": "± 2.50201",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 16.2368,
            "range": "± 2.68134",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 15.9555,
            "range": "± 2.48815",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 30.1856,
            "range": "± 4.44578",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 150.194,
            "range": "± 14.5689",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 35.9958,
            "range": "± 4.96047",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 210.931,
            "range": "± 22.6688",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.2247,
            "range": "± 5.35236",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.0608,
            "range": "± 5.92889",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 54.1521,
            "range": "± 10.5453",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 33.6998,
            "range": "± 4.85809",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 55.4785,
            "range": "± 10.7059",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 43.2504,
            "range": "± 8.96756",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 47.0175,
            "range": "± 10.7912",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.3966,
            "range": "± 5.29042",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 41.5319,
            "range": "± 7.5499",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 33.9743,
            "range": "± 5.96297",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 98.0077,
            "range": "± 13.9212",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.02518,
            "range": "± 20.3809",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 151.171,
            "range": "± 16.7437",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.12497,
            "range": "± 32.3569",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.4906,
            "range": "± 454.826",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.57293,
            "range": "± 480.507",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.78016,
            "range": "± 1.33981",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.89207,
            "range": "± 766.857",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.72,
            "range": "± 3.68451",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.0781,
            "range": "± 1.77125",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.79997,
            "range": "± 1.37605",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.03598,
            "range": "± 781.473",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.7772,
            "range": "± 3.12215",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.09296,
            "range": "± 1.42385",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.98149,
            "range": "± 1.21218",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.19668,
            "range": "± 822.959",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.5575,
            "range": "± 2.4488",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.4947,
            "range": "± 1.60677",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.65264,
            "range": "± 874.662",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.1724,
            "range": "± 1.24987",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.006,
            "range": "± 2.43187",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.59116,
            "range": "± 1.03933",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.20763,
            "range": "± 1.25186",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.37954,
            "range": "± 797.086",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 40.041,
            "range": "± 9.26148",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 40.3907,
            "range": "± 7.15513",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.34799,
            "range": "± 80.4701",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.34034,
            "range": "± 70.8283",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 100.044,
            "range": "± 10.8488",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.1872,
            "range": "± 9.8375",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.42347,
            "range": "± 72.2342",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.26057,
            "range": "± 57.4025",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 37.8915,
            "range": "± 8.55568",
            "unit": "ns",
            "extra": "100 samples\n888 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 285.064,
            "range": "± 11.2032",
            "unit": "ns",
            "extra": "100 samples\n8326 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.9723,
            "range": "± 0.362044",
            "unit": "ns",
            "extra": "100 samples\n7778 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 43.1843,
            "range": "± 4.36281",
            "unit": "ns",
            "extra": "100 samples\n732 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "5edb03f93ae5cd2815ce5e1e2d1d68c85a999c7b",
          "message": "Add CompilationStatistics and collect metrics across pipeline (#258)",
          "timestamp": "2026-04-16T17:01:21+02:00",
          "tree_id": "d0346f69bc45537e46bf3d62b05622042a3b9fef",
          "url": "https://github.com/nebulastream/nautilus/commit/5edb03f93ae5cd2815ce5e1e2d1d68c85a999c7b"
        },
        "date": 1776352067898,
        "tool": "catch2",
        "benches": [
          {
            "name": "tiered_compile_addOne",
            "value": 47.0175,
            "range": "± 13.9771",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.9882,
            "range": "± 133.229",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.5417,
            "range": "± 285.674",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 42.4727,
            "range": "± 14.7718",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 58.0472,
            "range": "± 17.2933",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.99303,
            "range": "± 185.317",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.8132,
            "range": "± 4.31668",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 61.1397,
            "range": "± 16.9569",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 35.7581,
            "range": "± 3.79304",
            "unit": "ns",
            "extra": "100 samples\n891 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 272.363,
            "range": "± 6.18073",
            "unit": "ns",
            "extra": "100 samples\n9585 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.68523,
            "range": "± 0.283693",
            "unit": "ns",
            "extra": "100 samples\n8702 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.4862,
            "range": "± 2.12428",
            "unit": "ns",
            "extra": "100 samples\n810 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 42.7721,
            "range": "± 14.7677",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.95818,
            "range": "± 152.467",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 591.964,
            "range": "± 32.9629",
            "unit": "ns",
            "extra": "100 samples\n53 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.52411,
            "range": "± 81.238",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.37083,
            "range": "± 261.278",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.51249,
            "range": "± 63.839",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.74749,
            "range": "± 183.411",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.61859,
            "range": "± 87.2015",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.89311,
            "range": "± 116.021",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.25247,
            "range": "± 76.8006",
            "unit": "us",
            "extra": "100 samples\n26 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.63495,
            "range": "± 116.529",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.33639,
            "range": "± 122.172",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.67998,
            "range": "± 339.485",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 92.7243,
            "range": "± 3.25701",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.5559,
            "range": "± 914.664",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 180.164,
            "range": "± 6.26125",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.28481,
            "range": "± 203.564",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.30928,
            "range": "± 214.438",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.62567,
            "range": "± 1.6893",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.66375,
            "range": "± 584.611",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 24.7825,
            "range": "± 3.53471",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.95,
            "range": "± 1.90745",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.62436,
            "range": "± 1.68931",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.77554,
            "range": "± 592.334",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.6801,
            "range": "± 4.29889",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.62214,
            "range": "± 1.23471",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.505,
            "range": "± 1.55037",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.72257,
            "range": "± 613.27",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.9049,
            "range": "± 2.44025",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.2015,
            "range": "± 1.33769",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.58072,
            "range": "± 862.793",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.65187,
            "range": "± 763.804",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.85092,
            "range": "± 1.56601",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.10268,
            "range": "± 788.731",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.58921,
            "range": "± 1.12396",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.84906,
            "range": "± 403",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 40.4577,
            "range": "± 8.04168",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 40.3029,
            "range": "± 8.04047",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.47893,
            "range": "± 32.53",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.50037,
            "range": "± 33.9023",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 95.9567,
            "range": "± 11.2483",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.4501,
            "range": "± 8.90639",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.33917,
            "range": "± 59.2269",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.31156,
            "range": "± 39.2536",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.0508,
            "range": "± 0.994392",
            "unit": "ns",
            "extra": "100 samples\n9803 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 17.0815,
            "range": "± 1.73926",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 549.693,
            "range": "± 61.0256",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.51945,
            "range": "± 0.568832",
            "unit": "ns",
            "extra": "100 samples\n7242 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 109.135,
            "range": "± 8.7756",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.5639,
            "range": "± 67.9323",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.886,
            "range": "± 5.26376",
            "unit": "ns",
            "extra": "100 samples\n716 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 633.955,
            "range": "± 24.2721",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 148.767,
            "range": "± 13.4705",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.55531,
            "range": "± 0.288199",
            "unit": "ns",
            "extra": "100 samples\n8844 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.3051,
            "range": "± 1.79641",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.30574,
            "range": "± 221.605",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 9.01965,
            "range": "± 168.967",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.62362,
            "range": "± 172.091",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.48976,
            "range": "± 151.39",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.6769,
            "range": "± 178.663",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.3398,
            "range": "± 243.797",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.3976,
            "range": "± 194.141",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.3695,
            "range": "± 233.484",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.38603,
            "range": "± 159.285",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.0433,
            "range": "± 194.412",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.8134,
            "range": "± 176.2",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.8315,
            "range": "± 165.673",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.004,
            "range": "± 610.131",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.2634,
            "range": "± 503.406",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.9681,
            "range": "± 198.064",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.7655,
            "range": "± 424.707",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.3663,
            "range": "± 346.839",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.1925,
            "range": "± 269.583",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.4243,
            "range": "± 246.26",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.4681,
            "range": "± 504.508",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.7436,
            "range": "± 236.402",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.8806,
            "range": "± 249.175",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.1074,
            "range": "± 220.598",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.5652,
            "range": "± 228.01",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.3351,
            "range": "± 255.446",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.4633,
            "range": "± 232.62",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.6154,
            "range": "± 312.523",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.8111,
            "range": "± 227.861",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.6414,
            "range": "± 428.905",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.1383,
            "range": "± 2.57622",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 15.6066,
            "range": "± 2.6789",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 17.9751,
            "range": "± 3.42482",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 15.7051,
            "range": "± 3.14903",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 17.4377,
            "range": "± 4.06636",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 15.7656,
            "range": "± 2.76096",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 15.9276,
            "range": "± 2.53715",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 15.526,
            "range": "± 2.66864",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 15.804,
            "range": "± 3.1205",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 15.2329,
            "range": "± 2.52074",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 27.9573,
            "range": "± 4.33039",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 146.341,
            "range": "± 14.3293",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 32.2334,
            "range": "± 4.61301",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 212.065,
            "range": "± 11.4733",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 17.0925,
            "range": "± 4.73054",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 26.5572,
            "range": "± 5.65916",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 47.5948,
            "range": "± 10.0431",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 27.7475,
            "range": "± 5.29272",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 47.2105,
            "range": "± 11.4821",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 31.7852,
            "range": "± 6.03308",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 33.6431,
            "range": "± 5.72051",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 23.5431,
            "range": "± 4.83946",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 30.3563,
            "range": "± 6.05367",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 28.3111,
            "range": "± 7.74562",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 93.4522,
            "range": "± 12.8504",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.04108,
            "range": "± 61.0518",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 147.099,
            "range": "± 15.1467",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.18003,
            "range": "± 18.134",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 178.175,
            "range": "± 8.29234",
            "unit": "ns",
            "extra": "100 samples\n174 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 438.001,
            "range": "± 19.5146",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.10827,
            "range": "± 61.9062",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 457.095,
            "range": "± 26.026",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 860.46,
            "range": "± 54.8467",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 460.863,
            "range": "± 26.8741",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 551.235,
            "range": "± 36.2715",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 431.054,
            "range": "± 21.0499",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 470.461,
            "range": "± 14.5928",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 418.266,
            "range": "± 17.9009",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "5eb143f265a4a792f413f9a59462cc2aab5fb83a",
          "message": "Add constant folding and copy propagation IR pass (#259)",
          "timestamp": "2026-04-17T12:11:36+02:00",
          "tree_id": "a3e435c3ca38c45505b9bb355dbc9f9bfad4086d",
          "url": "https://github.com/nebulastream/nautilus/commit/5eb143f265a4a792f413f9a59462cc2aab5fb83a"
        },
        "date": 1776421294160,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 581.365,
            "range": "± 35.9946",
            "unit": "ns",
            "extra": "100 samples\n51 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.397,
            "range": "± 91.8731",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.27156,
            "range": "± 298.727",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.50235,
            "range": "± 110.082",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.63817,
            "range": "± 187.603",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.61893,
            "range": "± 107.92",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.92324,
            "range": "± 213.389",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.2469,
            "range": "± 76.1491",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.68918,
            "range": "± 293.044",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.33856,
            "range": "± 92.8636",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.01543,
            "range": "± 423.802",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 89.9401,
            "range": "± 4.95277",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.8395,
            "range": "± 1.40194",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 174.383,
            "range": "± 9.08259",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.88872,
            "range": "± 286.975",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.41532,
            "range": "± 313.578",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.27409,
            "range": "± 305.103",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.9478,
            "range": "± 431.993",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 33.497,
            "range": "± 544.36",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.8406,
            "range": "± 401.262",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 32.7264,
            "range": "± 465.632",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.21151,
            "range": "± 399.85",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.3706,
            "range": "± 440.813",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 13.4737,
            "range": "± 523.709",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 14.2342,
            "range": "± 454.43",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 29.1042,
            "range": "± 443.958",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.3597,
            "range": "± 533.596",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 24.709,
            "range": "± 368.543",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.7385,
            "range": "± 399.914",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.3127,
            "range": "± 474.816",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.4488,
            "range": "± 712.75",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.2702,
            "range": "± 390.316",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.8029,
            "range": "± 753.131",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.933,
            "range": "± 601.524",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.1436,
            "range": "± 522.619",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.0351,
            "range": "± 455.029",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.6312,
            "range": "± 358.843",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.2935,
            "range": "± 469.87",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.6526,
            "range": "± 710.458",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.3953,
            "range": "± 614.261",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.0546,
            "range": "± 600.929",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.6754,
            "range": "± 1.12876",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.5421,
            "range": "± 2.54143",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 17.4782,
            "range": "± 3.63239",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 21.3533,
            "range": "± 4.26581",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 17.1659,
            "range": "± 3.0229",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 19.6355,
            "range": "± 3.70396",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 17.9581,
            "range": "± 3.31772",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 18.8125,
            "range": "± 3.30093",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.592,
            "range": "± 3.72846",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 17.9761,
            "range": "± 3.73704",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 17.6943,
            "range": "± 4.07339",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 32.4821,
            "range": "± 6.13241",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 149.775,
            "range": "± 10.0799",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 37.192,
            "range": "± 4.78747",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 221.472,
            "range": "± 15.7915",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 23.1178,
            "range": "± 6.97927",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.1706,
            "range": "± 5.34313",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 55.327,
            "range": "± 9.79015",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 34.12,
            "range": "± 5.23622",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 54.7514,
            "range": "± 10.4123",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 44.0738,
            "range": "± 9.23388",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 46.8872,
            "range": "± 9.63775",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.3254,
            "range": "± 4.65954",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 41.0695,
            "range": "± 8.02518",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 34.2124,
            "range": "± 5.2867",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 103.408,
            "range": "± 14.218",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.06424,
            "range": "± 34.322",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 151.179,
            "range": "± 13.6105",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.19472,
            "range": "± 73.3481",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.4337,
            "range": "± 0.960617",
            "unit": "ns",
            "extra": "100 samples\n8463 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.5444,
            "range": "± 1.30706",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 582.14,
            "range": "± 27.1533",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.66378,
            "range": "± 0.388479",
            "unit": "ns",
            "extra": "100 samples\n6520 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.697,
            "range": "± 8.63311",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.967,
            "range": "± 86.5471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 45.6313,
            "range": "± 10.2735",
            "unit": "ns",
            "extra": "100 samples\n699 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 790.387,
            "range": "± 14.0436",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 168.929,
            "range": "± 480.867",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.49179,
            "range": "± 0.383996",
            "unit": "ns",
            "extra": "100 samples\n8481 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.4781,
            "range": "± 2.26889",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.71963,
            "range": "± 22.1914",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 193.884,
            "range": "± 13.9281",
            "unit": "ns",
            "extra": "100 samples\n158 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 464.803,
            "range": "± 42.3798",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.19805,
            "range": "± 167.139",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 490.523,
            "range": "± 26.6667",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 926.686,
            "range": "± 75.4471",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 501.85,
            "range": "± 34.8381",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 603.105,
            "range": "± 48.7464",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 454.552,
            "range": "± 29.4397",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 535.349,
            "range": "± 53.0147",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 475.737,
            "range": "± 60.0051",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.35756,
            "range": "± 231.639",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.42792,
            "range": "± 292.295",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.02155,
            "range": "± 1.21738",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.88755,
            "range": "± 709.647",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.0448,
            "range": "± 3.4662",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 14.36,
            "range": "± 3.53393",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.81271,
            "range": "± 1.25288",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.97354,
            "range": "± 766.42",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.2501,
            "range": "± 3.4359",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.98214,
            "range": "± 1.47234",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.79594,
            "range": "± 1.40095",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.88259,
            "range": "± 602.633",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.4402,
            "range": "± 2.45729",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.8075,
            "range": "± 1.81916",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.70461,
            "range": "± 1.05647",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.59891,
            "range": "± 851.89",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.6558,
            "range": "± 1.91388",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.4268,
            "range": "± 1.00418",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.14733,
            "range": "± 1.16064",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.19959,
            "range": "± 690.695",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 40.2568,
            "range": "± 7.99443",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.9495,
            "range": "± 6.97921",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.4517,
            "range": "± 32.2924",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.36899,
            "range": "± 64.917",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 98.5302,
            "range": "± 10.5559",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.3803,
            "range": "± 8.75972",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.43866,
            "range": "± 66.6949",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.27754,
            "range": "± 64.9976",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 53.4951,
            "range": "± 12.5811",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.60703,
            "range": "± 417.461",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 52.1934,
            "range": "± 12.3146",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.51997,
            "range": "± 320.049",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.6782,
            "range": "± 465.487",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 53.9083,
            "range": "± 14.9964",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 72.8687,
            "range": "± 17.0653",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.99567,
            "range": "± 441.984",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.5712,
            "range": "± 561.132",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 71.6298,
            "range": "± 12.7744",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 34.8775,
            "range": "± 6.6185",
            "unit": "ns",
            "extra": "100 samples\n891 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 303.33,
            "range": "± 14.103",
            "unit": "ns",
            "extra": "100 samples\n8302 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.06617,
            "range": "± 0.649967",
            "unit": "ns",
            "extra": "100 samples\n7764 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.7299,
            "range": "± 2.36035",
            "unit": "ns",
            "extra": "100 samples\n754 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "ac2c4ea2bdd62b54ce85d073fd507c5e529b0dc5",
          "message": "Add simple linear register allocator to BC backend (#257)\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-04-19T09:33:39+02:00",
          "tree_id": "fe9c6f8aea1ee539d4b4bfb3191b5afffb02bf3c",
          "url": "https://github.com/nebulastream/nautilus/commit/ac2c4ea2bdd62b54ce85d073fd507c5e529b0dc5"
        },
        "date": 1776584325619,
        "tool": "catch2",
        "benches": [
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 47.7032,
            "range": "± 8.68568",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.23818,
            "range": "± 491.471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 618.115,
            "range": "± 53.3797",
            "unit": "ns",
            "extra": "100 samples\n48 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.44905,
            "range": "± 86.8949",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.37592,
            "range": "± 239.133",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.53605,
            "range": "± 105.581",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.77156,
            "range": "± 196.154",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.66853,
            "range": "± 129.354",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.92704,
            "range": "± 132.013",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.2645,
            "range": "± 80.8019",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.65853,
            "range": "± 128.906",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.37896,
            "range": "± 94.5931",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.6512,
            "range": "± 799.841",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 93.3698,
            "range": "± 6.73228",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.4575,
            "range": "± 863.742",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 183.581,
            "range": "± 16.7128",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.27443,
            "range": "± 166.248",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.30772,
            "range": "± 203.032",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.79821,
            "range": "± 706.962",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.5873,
            "range": "± 359.51",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.9509,
            "range": "± 1.83273",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.719,
            "range": "± 1.13015",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.60986,
            "range": "± 668.278",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.69067,
            "range": "± 293.036",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.1019,
            "range": "± 1.72681",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.49425,
            "range": "± 815.985",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.87258,
            "range": "± 695.472",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.8027,
            "range": "± 315.653",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.3937,
            "range": "± 1.20265",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.4807,
            "range": "± 1.59059",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.39065,
            "range": "± 849.79",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.4703,
            "range": "± 715.621",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.163,
            "range": "± 1.71855",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.05346,
            "range": "± 710.97",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.99856,
            "range": "± 1.09167",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.98571,
            "range": "± 405.803",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.6445,
            "range": "± 6.35823",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.1952,
            "range": "± 6.89562",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.36513,
            "range": "± 19.5193",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.38142,
            "range": "± 29.3291",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 100.633,
            "range": "± 10.9977",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 49.6066,
            "range": "± 7.49433",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.45201,
            "range": "± 36.6404",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.27281,
            "range": "± 21.1455",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.822,
            "range": "± 0.936004",
            "unit": "ns",
            "extra": "100 samples\n8445 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.7916,
            "range": "± 1.74485",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 515.967,
            "range": "± 16.084",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.6159,
            "range": "± 0.528671",
            "unit": "ns",
            "extra": "100 samples\n6535 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.6672,
            "range": "± 10.8249",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9244,
            "range": "± 434.181",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.8834,
            "range": "± 7.90089",
            "unit": "ns",
            "extra": "100 samples\n538 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 842.734,
            "range": "± 20.2036",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 178.8,
            "range": "± 3.10469",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.49445,
            "range": "± 0.375379",
            "unit": "ns",
            "extra": "100 samples\n8453 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.6426,
            "range": "± 2.74605",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.71937,
            "range": "± 21.7272",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 43.9791,
            "range": "± 4.89944",
            "unit": "ns",
            "extra": "100 samples\n678 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 43.5639,
            "range": "± 2.86818",
            "unit": "ns",
            "extra": "100 samples\n483 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 861.571,
            "range": "± 98.4839",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 839.407,
            "range": "± 13.4705",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 179.223,
            "range": "± 2.02788",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 178.601,
            "range": "± 3.45719",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 185.816,
            "range": "± 10.3996",
            "unit": "ns",
            "extra": "100 samples\n158 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 455.998,
            "range": "± 8.73188",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.1929,
            "range": "± 84.0502",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 495.525,
            "range": "± 42.2521",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 923.55,
            "range": "± 58.5963",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 497.983,
            "range": "± 20.4573",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 600.749,
            "range": "± 36.2279",
            "unit": "ns",
            "extra": "100 samples\n53 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 497.541,
            "range": "± 60.4145",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 520.747,
            "range": "± 31.8873",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 451.525,
            "range": "± 29.293",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.03653,
            "range": "± 44.1051",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.61342,
            "range": "± 54.5514",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.5802,
            "range": "± 40.4471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.48877,
            "range": "± 103.05",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.5007,
            "range": "± 230.085",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.0043,
            "range": "± 46.9025",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.5874,
            "range": "± 103.344",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.47096,
            "range": "± 38.3264",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.6628,
            "range": "± 38.156",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.6741,
            "range": "± 63.0358",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.8187,
            "range": "± 55.0301",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.225,
            "range": "± 87.2712",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.8168,
            "range": "± 116.022",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.1996,
            "range": "± 240.353",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.5321,
            "range": "± 480.572",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.2727,
            "range": "± 558.226",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.2665,
            "range": "± 459.988",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.2988,
            "range": "± 344.351",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.4559,
            "range": "± 789.37",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.0943,
            "range": "± 221.508",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.222,
            "range": "± 258.79",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.5928,
            "range": "± 309.914",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 24.9754,
            "range": "± 293.062",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.2159,
            "range": "± 502.157",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.5753,
            "range": "± 693.59",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.7031,
            "range": "± 527.471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.694,
            "range": "± 425.766",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.4516,
            "range": "± 673.024",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.3597,
            "range": "± 2.48149",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.1495,
            "range": "± 2.61377",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 21.8467,
            "range": "± 3.49218",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.5722,
            "range": "± 4.77224",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.1077,
            "range": "± 3.93291",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.8034,
            "range": "± 3.15769",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.003,
            "range": "± 3.56989",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.0675,
            "range": "± 3.19641",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.8155,
            "range": "± 3.99054",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.1727,
            "range": "± 3.2952",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.1955,
            "range": "± 4.58262",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 197.212,
            "range": "± 11.5333",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 50.988,
            "range": "± 9.82885",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 306.036,
            "range": "± 11.3522",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 20.6396,
            "range": "± 3.7317",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.008,
            "range": "± 5.75999",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 54.3531,
            "range": "± 8.14402",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 33.7759,
            "range": "± 4.23803",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 53.9387,
            "range": "± 5.94154",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 44.0242,
            "range": "± 8.85063",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 46.0983,
            "range": "± 8.87218",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 27.1825,
            "range": "± 4.14008",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 41.5607,
            "range": "± 7.09007",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 34.0827,
            "range": "± 5.38372",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 100.924,
            "range": "± 9.12832",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.02018,
            "range": "± 11.7866",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 152.283,
            "range": "± 13.5281",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.13238,
            "range": "± 35.6257",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 36.2618,
            "range": "± 5.56148",
            "unit": "ns",
            "extra": "100 samples\n836 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 276.222,
            "range": "± 5.76067",
            "unit": "ns",
            "extra": "100 samples\n8285 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.88144,
            "range": "± 0.331571",
            "unit": "ns",
            "extra": "100 samples\n7738 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.0484,
            "range": "± 1.8257",
            "unit": "ns",
            "extra": "100 samples\n788 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 51.9377,
            "range": "± 13.0686",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.10715,
            "range": "± 63.2407",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 24.4534,
            "range": "± 271.946",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 51.9263,
            "range": "± 8.48295",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 73.8566,
            "range": "± 13.3747",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.15025,
            "range": "± 122.033",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.6265,
            "range": "± 346.062",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 73.7859,
            "range": "± 9.66704",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "735a59de3fd11e94fdefbab9bef9cb375185faf1",
          "message": "Add post-RA peephole pass to AsmJit backend (#256)\n\nCo-authored-by: Claude Sonnet 4.6 <noreply@anthropic.com>",
          "timestamp": "2026-04-19T20:02:52+02:00",
          "tree_id": "7104e26c84d73f2f4d141a28faf6b7a03938f98e",
          "url": "https://github.com/nebulastream/nautilus/commit/735a59de3fd11e94fdefbab9bef9cb375185faf1"
        },
        "date": 1776622076002,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 557.603,
            "range": "± 33.4202",
            "unit": "ns",
            "extra": "100 samples\n57 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.35697,
            "range": "± 125.581",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.25431,
            "range": "± 120.92",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.48915,
            "range": "± 149.204",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.71554,
            "range": "± 269.14",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.53102,
            "range": "± 46.9752",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.97107,
            "range": "± 430.214",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.21683,
            "range": "± 80.1593",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.57054,
            "range": "± 100.473",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.30244,
            "range": "± 80.1681",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.07715,
            "range": "± 494.947",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 94.1299,
            "range": "± 6.63792",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.6931,
            "range": "± 684.358",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 178.212,
            "range": "± 10.231",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.06394,
            "range": "± 181.932",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.09441,
            "range": "± 173.694",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 7.82154,
            "range": "± 1.27749",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.15494,
            "range": "± 494.679",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 23.5052,
            "range": "± 3.7184",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 11.5912,
            "range": "± 1.68569",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 7.70984,
            "range": "± 1.09486",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.27197,
            "range": "± 452.073",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 15.3213,
            "range": "± 2.10195",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 7.73302,
            "range": "± 938.568",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 7.80253,
            "range": "± 1.15647",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.2795,
            "range": "± 509.818",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 12.6363,
            "range": "± 2.33328",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 8.99261,
            "range": "± 1.44436",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.04359,
            "range": "± 755.434",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.09734,
            "range": "± 735.226",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.02193,
            "range": "± 1.46526",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.49339,
            "range": "± 665.791",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.90115,
            "range": "± 2.65556",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.57162,
            "range": "± 411.759",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 36.5071,
            "range": "± 7.32195",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 36.8142,
            "range": "± 7.18954",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.56252,
            "range": "± 247.341",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.4543,
            "range": "± 35.5561",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 92.2442,
            "range": "± 11.1303",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 46.815,
            "range": "± 8.88196",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.4846,
            "range": "± 225.461",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.19662,
            "range": "± 55.2405",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.1087,
            "range": "± 0.853075",
            "unit": "ns",
            "extra": "100 samples\n9880 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 18.7581,
            "range": "± 2.52184",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 565.098,
            "range": "± 56.3939",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.67595,
            "range": "± 1.06637",
            "unit": "ns",
            "extra": "100 samples\n7285 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 109.488,
            "range": "± 8.76714",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.5994,
            "range": "± 113.601",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.5227,
            "range": "± 6.90048",
            "unit": "ns",
            "extra": "100 samples\n733 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 626.695,
            "range": "± 14.1954",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 143.596,
            "range": "± 1.5291",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.16976,
            "range": "± 0.210314",
            "unit": "ns",
            "extra": "100 samples\n9905 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.4194,
            "range": "± 2.2047",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.28048,
            "range": "± 29.4129",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 43.4498,
            "range": "± 2.08094",
            "unit": "ns",
            "extra": "100 samples\n723 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 43.0869,
            "range": "± 1.60579",
            "unit": "ns",
            "extra": "100 samples\n723 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 631.805,
            "range": "± 12.0476",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 626.084,
            "range": "± 11.7745",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 144.361,
            "range": "± 7.34656",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 143.149,
            "range": "± 1.08876",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 43.1655,
            "range": "± 15.6613",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 7.30884,
            "range": "± 250.756",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 29.3191,
            "range": "± 604.247",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 47.631,
            "range": "± 23.1615",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 60.9415,
            "range": "± 18.3227",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 9.44764,
            "range": "± 302.585",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 30.3033,
            "range": "± 749.906",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 62.316,
            "range": "± 18.6408",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 35.8115,
            "range": "± 3.14464",
            "unit": "ns",
            "extra": "100 samples\n879 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 318.969,
            "range": "± 13.122",
            "unit": "ns",
            "extra": "100 samples\n8703 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.15126,
            "range": "± 0.438887",
            "unit": "ns",
            "extra": "100 samples\n8003 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.4164,
            "range": "± 1.75957",
            "unit": "ns",
            "extra": "100 samples\n816 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 9.42323,
            "range": "± 413.446",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 10.0186,
            "range": "± 356.845",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 9.30722,
            "range": "± 253.08",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 11.6633,
            "range": "± 259.204",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 33.8733,
            "range": "± 651.356",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 16.3832,
            "range": "± 447.685",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 32.7066,
            "range": "± 451.19",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.53315,
            "range": "± 175.692",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.6574,
            "range": "± 312.126",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 13.9072,
            "range": "± 1.1126",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 14.7447,
            "range": "± 361.456",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 29.0962,
            "range": "± 380.159",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 14.0527,
            "range": "± 1.03727",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 24.8764,
            "range": "± 347.559",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 28.8601,
            "range": "± 464.566",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 29.4161,
            "range": "± 503.621",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 30.3349,
            "range": "± 470.032",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 29.5386,
            "range": "± 452.283",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 30.6408,
            "range": "± 1.1838",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 29.968,
            "range": "± 534.834",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 30.0626,
            "range": "± 649.42",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 29.3365,
            "range": "± 625",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 30.3049,
            "range": "± 1.0683",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 30.1386,
            "range": "± 655.731",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 33.1127,
            "range": "± 1.59488",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 66.1697,
            "range": "± 1.99571",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 35.0051,
            "range": "± 569.074",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 95.6542,
            "range": "± 1.27066",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.4189,
            "range": "± 2.37199",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.7978,
            "range": "± 3.7847",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.1859,
            "range": "± 4.35147",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.8307,
            "range": "± 3.90376",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 18.9231,
            "range": "± 3.849",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 17.0217,
            "range": "± 3.51388",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 18.0283,
            "range": "± 4.01563",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.5494,
            "range": "± 3.68146",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 17.0462,
            "range": "± 3.76914",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 15.9395,
            "range": "± 2.47334",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 31.0134,
            "range": "± 4.42009",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 191.712,
            "range": "± 9.52673",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 43.141,
            "range": "± 8.89736",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 309.419,
            "range": "± 12.6364",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 17.5231,
            "range": "± 5.7044",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 26.185,
            "range": "± 5.62526",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 47.5287,
            "range": "± 10.7073",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 28.2969,
            "range": "± 6.06829",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 47.413,
            "range": "± 12.3118",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 30.9936,
            "range": "± 6.48682",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 33.2829,
            "range": "± 5.89897",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 23.4582,
            "range": "± 4.88987",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 30.2905,
            "range": "± 5.68839",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 28.1555,
            "range": "± 6.13342",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 96.044,
            "range": "± 15.4869",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.04677,
            "range": "± 51.1612",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 148.825,
            "range": "± 16.3448",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.19743,
            "range": "± 43.3502",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 178.589,
            "range": "± 8.61098",
            "unit": "ns",
            "extra": "100 samples\n175 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 432.511,
            "range": "± 24.8694",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.12092,
            "range": "± 62.1812",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 454.465,
            "range": "± 22.6714",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 914.873,
            "range": "± 156.953",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 463.089,
            "range": "± 27.2138",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 554.161,
            "range": "± 36.2962",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 430.496,
            "range": "± 17.6849",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 475.635,
            "range": "± 26.8476",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 422.707,
            "range": "± 24.5879",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 44.7721,
            "range": "± 16.4703",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 9.16088,
            "range": "± 188.959",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "2ffaf1e3d551a54606e830e8c4cb6c9c1a943d8f",
          "message": "Add GPU support: val primitives, CUDA and Metal backends (#212)\n\nCo-authored-by: Claude Sonnet 4.6 <noreply@anthropic.com>",
          "timestamp": "2026-04-21T22:57:35+02:00",
          "tree_id": "1deb1b127bc8f7d2353d8cd1f3369635cbe04179",
          "url": "https://github.com/nebulastream/nautilus/commit/2ffaf1e3d551a54606e830e8c4cb6c9c1a943d8f"
        },
        "date": 1776805551731,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 8.76627,
            "range": "± 332.405",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.95835,
            "range": "± 273.048",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.37599,
            "range": "± 311.638",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 11.2263,
            "range": "± 405.766",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 33.7197,
            "range": "± 616.204",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 16.0557,
            "range": "± 301.098",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 32.7766,
            "range": "± 806.438",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.55551,
            "range": "± 304.977",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.843,
            "range": "± 225.661",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 13.5026,
            "range": "± 441.728",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 14.3348,
            "range": "± 358.605",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 29.5408,
            "range": "± 500.116",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 14.2258,
            "range": "± 909.105",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 25.1272,
            "range": "± 1.54082",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.1948,
            "range": "± 555.352",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.1286,
            "range": "± 610.986",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.5929,
            "range": "± 547.02",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.3429,
            "range": "± 441.857",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.902,
            "range": "± 480.129",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 29.2206,
            "range": "± 662.053",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.8687,
            "range": "± 296.807",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.4622,
            "range": "± 528.164",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.5222,
            "range": "± 633.469",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.0743,
            "range": "± 1.44027",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.8068,
            "range": "± 836.067",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.4829,
            "range": "± 1.10422",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.5639,
            "range": "± 309.967",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.2252,
            "range": "± 776.673",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 15.2691,
            "range": "± 3.40585",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 19.6165,
            "range": "± 5.06579",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.9172,
            "range": "± 3.59346",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.8338,
            "range": "± 3.24741",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.9345,
            "range": "± 4.03811",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.7819,
            "range": "± 4.37981",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 21.142,
            "range": "± 6.15798",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.6114,
            "range": "± 4.31367",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.7324,
            "range": "± 4.59893",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.4547,
            "range": "± 3.15436",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 36.2171,
            "range": "± 5.29525",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 202.596,
            "range": "± 10.1372",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 52.1024,
            "range": "± 7.90861",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 324.112,
            "range": "± 16.9692",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 20.9202,
            "range": "± 4.38316",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.0044,
            "range": "± 6.12951",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 56.3703,
            "range": "± 9.75886",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 34.7087,
            "range": "± 5.49668",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 58.7126,
            "range": "± 10.0651",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 45.9739,
            "range": "± 8.80549",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 50.9986,
            "range": "± 11.1165",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 27.8058,
            "range": "± 5.39393",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 43.33,
            "range": "± 8.22075",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.021,
            "range": "± 5.95339",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 102.01,
            "range": "± 13.3976",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.05967,
            "range": "± 20.5217",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 156.759,
            "range": "± 18.6395",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.20871,
            "range": "± 48.7181",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 54.3277,
            "range": "± 13.1825",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.73377,
            "range": "± 281.407",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.4903,
            "range": "± 420.01",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 54.8942,
            "range": "± 14.8212",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 75.3287,
            "range": "± 16.237",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 9.07401,
            "range": "± 281.979",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.7854,
            "range": "± 557.305",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 80.3408,
            "range": "± 17.7822",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.2091,
            "range": "± 1.42934",
            "unit": "ns",
            "extra": "100 samples\n8449 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 14.2824,
            "range": "± 1.92806",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 597.951,
            "range": "± 23.047",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.80544,
            "range": "± 0.814677",
            "unit": "ns",
            "extra": "100 samples\n6541 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.0914,
            "range": "± 8.16828",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9784,
            "range": "± 138.843",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.4782,
            "range": "± 4.70274",
            "unit": "ns",
            "extra": "100 samples\n676 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 857.194,
            "range": "± 17.4174",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 184.064,
            "range": "± 8.16741",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.60927,
            "range": "± 0.607746",
            "unit": "ns",
            "extra": "100 samples\n8457 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 20.5429,
            "range": "± 1.9346",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.85868,
            "range": "± 49.2357",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 45.0151,
            "range": "± 5.14882",
            "unit": "ns",
            "extra": "100 samples\n682 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 45.238,
            "range": "± 6.37318",
            "unit": "ns",
            "extra": "100 samples\n676 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 855.189,
            "range": "± 9.66849",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 854.978,
            "range": "± 11.2227",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 185.09,
            "range": "± 6.42496",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 185.4,
            "range": "± 6.91377",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 193.444,
            "range": "± 14.4574",
            "unit": "ns",
            "extra": "100 samples\n150 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 481.025,
            "range": "± 52.4164",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.22984,
            "range": "± 122.086",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 504.249,
            "range": "± 37.4013",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 933.215,
            "range": "± 66.4233",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 518.198,
            "range": "± 56.5645",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 620.896,
            "range": "± 51.2193",
            "unit": "ns",
            "extra": "100 samples\n53 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 510.538,
            "range": "± 45.8424",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 534.56,
            "range": "± 58.5422",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 474.654,
            "range": "± 40.2272",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ir_add",
            "value": 668.487,
            "range": "± 68.8937",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.55566,
            "range": "± 154.265",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.51809,
            "range": "± 317.213",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.6329,
            "range": "± 190.438",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.9639,
            "range": "± 371.031",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.7762,
            "range": "± 185.217",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.19343,
            "range": "± 373.102",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.36351,
            "range": "± 127.388",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.75353,
            "range": "± 145.959",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.44216,
            "range": "± 117.369",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.54684,
            "range": "± 636.769",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 87.9756,
            "range": "± 5.29577",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.6134,
            "range": "± 1.04809",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 174.304,
            "range": "± 9.65732",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.36936,
            "range": "± 235.077",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.4785,
            "range": "± 300.96",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.06578,
            "range": "± 1.31373",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.96463,
            "range": "± 680.073",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.4539,
            "range": "± 3.12633",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.4822,
            "range": "± 1.9273",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 9.26316,
            "range": "± 1.58615",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.14295,
            "range": "± 829.938",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.3075,
            "range": "± 2.76239",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.33254,
            "range": "± 1.66561",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.20987,
            "range": "± 1.29384",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.22485,
            "range": "± 875.968",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.3704,
            "range": "± 2.7101",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.739,
            "range": "± 1.78075",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.76128,
            "range": "± 1.00223",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.6479,
            "range": "± 843.779",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.2995,
            "range": "± 1.73107",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.27241,
            "range": "± 773.99",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.13126,
            "range": "± 1.14014",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.16913,
            "range": "± 516.124",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 39.803,
            "range": "± 7.7761",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.5214,
            "range": "± 6.88634",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.39386,
            "range": "± 54.148",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.38372,
            "range": "± 55.8981",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 100.552,
            "range": "± 9.62073",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.157,
            "range": "± 8.84456",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.43697,
            "range": "± 41.7797",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.25375,
            "range": "± 45.4821",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 36.2979,
            "range": "± 6.35257",
            "unit": "ns",
            "extra": "100 samples\n859 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 297.733,
            "range": "± 10.9758",
            "unit": "ns",
            "extra": "100 samples\n8268 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.08932,
            "range": "± 0.746367",
            "unit": "ns",
            "extra": "100 samples\n7710 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.6547,
            "range": "± 2.22188",
            "unit": "ns",
            "extra": "100 samples\n733 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 55.0947,
            "range": "± 14.543",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.47637,
            "range": "± 262.69",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "46024165b9347d0f7f5e3f8b4e9d21dc703d16e2",
          "message": "Harden the inlining plugin: registry thread-safety, JIT hook safety, tests (#246)\n\nCo-authored-by: Claude Opus 4.7 (1M context) <noreply@anthropic.com>",
          "timestamp": "2026-04-21T23:19:18+02:00",
          "tree_id": "991b5d3720cac210d5d7bb9bff6f09499d6b5967",
          "url": "https://github.com/nebulastream/nautilus/commit/46024165b9347d0f7f5e3f8b4e9d21dc703d16e2"
        },
        "date": 1776806673714,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_bc_addOne",
            "value": 35.3865,
            "range": "± 4.28447",
            "unit": "ns",
            "extra": "100 samples\n861 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 279.328,
            "range": "± 3.7127",
            "unit": "ns",
            "extra": "100 samples\n8285 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.00184,
            "range": "± 0.802946",
            "unit": "ns",
            "extra": "100 samples\n7748 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.5706,
            "range": "± 1.964",
            "unit": "ns",
            "extra": "100 samples\n732 iterations"
          },
          {
            "name": "ssa_add",
            "value": 183.954,
            "range": "± 9.73075",
            "unit": "ns",
            "extra": "100 samples\n158 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 458.789,
            "range": "± 11.2779",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.18968,
            "range": "± 74.4717",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 496.71,
            "range": "± 27.9232",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 925.652,
            "range": "± 61.8054",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 496.849,
            "range": "± 36.6132",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 605.177,
            "range": "± 39.2943",
            "unit": "ns",
            "extra": "100 samples\n53 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 484.837,
            "range": "± 31.741",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 521.649,
            "range": "± 35.3707",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 452.32,
            "range": "± 25.6002",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.28506,
            "range": "± 132.953",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.30878,
            "range": "± 132.103",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.75318,
            "range": "± 1.01512",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.52338,
            "range": "± 312.956",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.9442,
            "range": "± 1.69359",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.7602,
            "range": "± 952.895",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.68083,
            "range": "± 1.04071",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.66315,
            "range": "± 386.388",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.7423,
            "range": "± 1.38269",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.70168,
            "range": "± 1.04226",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.9146,
            "range": "± 5.65649",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.79562,
            "range": "± 412.453",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.0235,
            "range": "± 3.17108",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.4921,
            "range": "± 1.22365",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.26646,
            "range": "± 790.488",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.48803,
            "range": "± 921.949",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.1139,
            "range": "± 1.41133",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.11648,
            "range": "± 687.577",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.92349,
            "range": "± 1.3417",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.98782,
            "range": "± 588.581",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.8742,
            "range": "± 7.11188",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.0798,
            "range": "± 6.11528",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.3915,
            "range": "± 25.138",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.46556,
            "range": "± 30.9473",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 99.9447,
            "range": "± 9.91357",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.8984,
            "range": "± 8.06197",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.46404,
            "range": "± 71.8989",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.32265,
            "range": "± 50.7391",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 53.0753,
            "range": "± 8.9913",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.18759,
            "range": "± 170.911",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 54.0091,
            "range": "± 7.62784",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.23982,
            "range": "± 93.2092",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 24.3315,
            "range": "± 728.801",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 54.1375,
            "range": "± 14.0183",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 73.9768,
            "range": "± 10.5077",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.22793,
            "range": "± 45.9556",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 24.9887,
            "range": "± 497.241",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 75.5952,
            "range": "± 15.4558",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.18217,
            "range": "± 124.492",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.83464,
            "range": "± 188.158",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.78707,
            "range": "± 110.723",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.75533,
            "range": "± 57.1609",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.1228,
            "range": "± 372.15",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.4405,
            "range": "± 488.466",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.6527,
            "range": "± 717.587",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.59031,
            "range": "± 58.9107",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.996,
            "range": "± 80.4379",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.9603,
            "range": "± 110.88",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.0351,
            "range": "± 128.072",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.0546,
            "range": "± 252.895",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.0206,
            "range": "± 62.901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.3266,
            "range": "± 110.023",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.172,
            "range": "± 273.243",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 24.8408,
            "range": "± 223.603",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 25.8796,
            "range": "± 219.824",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 24.9526,
            "range": "± 312.241",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 25.82,
            "range": "± 241.339",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.3217,
            "range": "± 325.548",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.4441,
            "range": "± 243.053",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.8067,
            "range": "± 400.121",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.1638,
            "range": "± 269.58",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 24.8562,
            "range": "± 281.3",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 27.8813,
            "range": "± 240.944",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 60.8269,
            "range": "± 460.895",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.1395,
            "range": "± 269.48",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 90.7091,
            "range": "± 984.812",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.441,
            "range": "± 2.9241",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.1243,
            "range": "± 3.28918",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.2027,
            "range": "± 3.60003",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.1823,
            "range": "± 3.36859",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.8275,
            "range": "± 2.98808",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.7606,
            "range": "± 3.09223",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.8801,
            "range": "± 3.25984",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.8993,
            "range": "± 3.84761",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.5432,
            "range": "± 3.46523",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.0011,
            "range": "± 3.96605",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 34.4416,
            "range": "± 4.14209",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 195.257,
            "range": "± 11.2114",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 49.5842,
            "range": "± 7.49923",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 301.348,
            "range": "± 12.1043",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 20.7889,
            "range": "± 3.86536",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.6031,
            "range": "± 5.23951",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 57.3092,
            "range": "± 11.3552",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 33.9114,
            "range": "± 4.15675",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 55.8297,
            "range": "± 9.98323",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 45.1046,
            "range": "± 9.45127",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 47.4143,
            "range": "± 8.76097",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 27.5159,
            "range": "± 4.46123",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 42.3652,
            "range": "± 7.75237",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 36.0563,
            "range": "± 7.8456",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 102.035,
            "range": "± 11.5914",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.04556,
            "range": "± 18.2254",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 149.222,
            "range": "± 10.1037",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.14167,
            "range": "± 30.6905",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 656.589,
            "range": "± 35.4979",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.49466,
            "range": "± 100.096",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.46657,
            "range": "± 218.509",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.624,
            "range": "± 118.533",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.88257,
            "range": "± 208.104",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.72484,
            "range": "± 73.824",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.97159,
            "range": "± 163.276",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.33954,
            "range": "± 78.7507",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.72177,
            "range": "± 104.377",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.46473,
            "range": "± 84.9551",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.59077,
            "range": "± 592.179",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 86.4191,
            "range": "± 5.74789",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.2545,
            "range": "± 751.315",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 173.748,
            "range": "± 10.1368",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.4405,
            "range": "± 1.01308",
            "unit": "ns",
            "extra": "100 samples\n8449 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.831,
            "range": "± 1.5398",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 608.54,
            "range": "± 20.5981",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.72473,
            "range": "± 0.548913",
            "unit": "ns",
            "extra": "100 samples\n6548 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 98.8966,
            "range": "± 12.0742",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9949,
            "range": "± 209.528",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.4592,
            "range": "± 2.61241",
            "unit": "ns",
            "extra": "100 samples\n680 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 860.076,
            "range": "± 14.6739",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 185.963,
            "range": "± 1.07498",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.47005,
            "range": "± 0.194007",
            "unit": "ns",
            "extra": "100 samples\n8455 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.1475,
            "range": "± 3.99034",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.85045,
            "range": "± 22.5172",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 45.7868,
            "range": "± 8.63127",
            "unit": "ns",
            "extra": "100 samples\n682 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 44.0523,
            "range": "± 4.26555",
            "unit": "ns",
            "extra": "100 samples\n675 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 856.606,
            "range": "± 10.551",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 857.01,
            "range": "± 11.4079",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 186.254,
            "range": "± 1.43595",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 186.122,
            "range": "± 415.862",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "f1b569ae33efcea842e7fc5ee44464cedf00e60c",
          "message": "Introduce MLIRJit wrapper around mlir::ExecutionEngine (#264)",
          "timestamp": "2026-04-22T20:14:43+02:00",
          "tree_id": "daf5a9e171f81c8bb4bcaff9bcdd7952c46a96fa",
          "url": "https://github.com/nebulastream/nautilus/commit/f1b569ae33efcea842e7fc5ee44464cedf00e60c"
        },
        "date": 1776882214035,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 6.15625,
            "range": "± 303.542",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.77636,
            "range": "± 348.984",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.71346,
            "range": "± 399.718",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.05246,
            "range": "± 484.574",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.0459,
            "range": "± 473.27",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.7572,
            "range": "± 674.472",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.2404,
            "range": "± 731.087",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.52754,
            "range": "± 349.595",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.7068,
            "range": "± 500.289",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.0179,
            "range": "± 606.665",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.79,
            "range": "± 474.546",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.577,
            "range": "± 376.382",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.9161,
            "range": "± 706.058",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.8504,
            "range": "± 306.241",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 213.075,
            "range": "± 40.8268",
            "unit": "ns",
            "extra": "100 samples\n161 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 509.048,
            "range": "± 110.156",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.26731,
            "range": "± 226.545",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 543.408,
            "range": "± 104.802",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 985.548,
            "range": "± 174.281",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 540.139,
            "range": "± 76.8559",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 635.238,
            "range": "± 95.5448",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 523.208,
            "range": "± 114.916",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 540.04,
            "range": "± 72.5173",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 483.802,
            "range": "± 76.8216",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 38.8669,
            "range": "± 8.48797",
            "unit": "ns",
            "extra": "100 samples\n824 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 316.48,
            "range": "± 18.5648",
            "unit": "ns",
            "extra": "100 samples\n8233 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.51894,
            "range": "± 0.976781",
            "unit": "ns",
            "extra": "100 samples\n7740 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 42.6555,
            "range": "± 6.67114",
            "unit": "ns",
            "extra": "100 samples\n757 iterations"
          },
          {
            "name": "ir_add",
            "value": 721.261,
            "range": "± 126.19",
            "unit": "ns",
            "extra": "100 samples\n47 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.70107,
            "range": "± 374.782",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.74096,
            "range": "± 754.735",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.78289,
            "range": "± 386.765",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.9735,
            "range": "± 478.32",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.89926,
            "range": "± 287.618",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.09779,
            "range": "± 316.588",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.42025,
            "range": "± 229.726",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.85828,
            "range": "± 348.13",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.5604,
            "range": "± 276.914",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.76773,
            "range": "± 1.20542",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 85.9913,
            "range": "± 4.6035",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.298,
            "range": "± 1.31238",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 174.876,
            "range": "± 9.46844",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 54.7828,
            "range": "± 16.0603",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.30052,
            "range": "± 127.913",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 28.0157,
            "range": "± 1.42144",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 55.7654,
            "range": "± 13.6604",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 78.3767,
            "range": "± 17.1943",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.77246,
            "range": "± 374.67",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 30.2012,
            "range": "± 2.62731",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 79.3881,
            "range": "± 25.8478",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.52328,
            "range": "± 414.793",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.78145,
            "range": "± 606.676",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.15499,
            "range": "± 1.89719",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.22972,
            "range": "± 1.0227",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 27.4265,
            "range": "± 4.37764",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 14.4735,
            "range": "± 2.80203",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 9.07422,
            "range": "± 1.47101",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.216,
            "range": "± 1.00623",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.9715,
            "range": "± 3.36536",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.39063,
            "range": "± 2.07364",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.60999,
            "range": "± 1.78952",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.05364,
            "range": "± 830.737",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.4378,
            "range": "± 3.50047",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.9546,
            "range": "± 2.20134",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.49257,
            "range": "± 1.10441",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.06281,
            "range": "± 1.38798",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.7348,
            "range": "± 2.00721",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.66713,
            "range": "± 1.27929",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.38009,
            "range": "± 1.36754",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.32385,
            "range": "± 828.309",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 42.5086,
            "range": "± 12.2621",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 40.5316,
            "range": "± 8.86573",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.35276,
            "range": "± 85.3128",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.3573,
            "range": "± 79.1349",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 100.804,
            "range": "± 13.549",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 52.3929,
            "range": "± 11.7074",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.43811,
            "range": "± 58.6492",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.26657,
            "range": "± 67.0771",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 56.1278,
            "range": "± 14.5292",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.04734,
            "range": "± 427.763",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.8586,
            "range": "± 1.30662",
            "unit": "ns",
            "extra": "100 samples\n8435 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 17.3837,
            "range": "± 3.81913",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 610.856,
            "range": "± 32.8518",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.81187,
            "range": "± 0.70764",
            "unit": "ns",
            "extra": "100 samples\n6458 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 98.2451,
            "range": "± 10.7937",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9858,
            "range": "± 112.097",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 46.6278,
            "range": "± 7.78666",
            "unit": "ns",
            "extra": "100 samples\n682 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 848.49,
            "range": "± 26.9267",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 179.11,
            "range": "± 345.595",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.54263,
            "range": "± 0.339404",
            "unit": "ns",
            "extra": "100 samples\n8444 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.0789,
            "range": "± 3.0195",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.84693,
            "range": "± 33.8549",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 45.6569,
            "range": "± 6.09227",
            "unit": "ns",
            "extra": "100 samples\n681 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 47.7603,
            "range": "± 11.316",
            "unit": "ns",
            "extra": "100 samples\n682 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 843.21,
            "range": "± 18.4569",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 845.561,
            "range": "± 21.0756",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 178.97,
            "range": "± 338.631",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 179.001,
            "range": "± 338.902",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "9fb5464a8e80715c508b3c13d2389e5f989ea0c2",
          "message": "Replace stale NESIR codename with .nautilus in IR dumps (#268)\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-04-25T17:04:44+02:00",
          "tree_id": "ecaa01fbe2392ea367f50d6bdf6ddbca3d314b5e",
          "url": "https://github.com/nebulastream/nautilus/commit/9fb5464a8e80715c508b3c13d2389e5f989ea0c2"
        },
        "date": 1777129795556,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 138.373,
            "range": "± 7.32871",
            "unit": "ns",
            "extra": "100 samples\n172 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 325.389,
            "range": "± 18.0966",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.8435180000000001,
            "range": "± 0.180735",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 349.106,
            "range": "± 30.8471",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 624.318,
            "range": "± 44.1548",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 350.94,
            "range": "± 32.6573",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 418.433,
            "range": "± 23.6135",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 329.866,
            "range": "± 15.4487",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 360.966,
            "range": "± 20.5787",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 322.127,
            "range": "± 16.0493",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 36.2276,
            "range": "± 13.4346",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.15552,
            "range": "± 339.467",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 28.3379,
            "range": "± 1.96656",
            "unit": "ns",
            "extra": "100 samples\n840 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 256.569,
            "range": "± 10.6004",
            "unit": "ns",
            "extra": "100 samples\n8671 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.34196,
            "range": "± 0.616277",
            "unit": "ns",
            "extra": "100 samples\n7909 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 31.9886,
            "range": "± 1.67641",
            "unit": "ns",
            "extra": "100 samples\n753 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 8.59958,
            "range": "± 1.19358",
            "unit": "ns",
            "extra": "100 samples\n8792 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.513,
            "range": "± 1.34449",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 525.225,
            "range": "± 53.3368",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 3.67922,
            "range": "± 0.811903",
            "unit": "ns",
            "extra": "100 samples\n7227 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 83.9984,
            "range": "± 4.82933",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 18.3061,
            "range": "± 161.764",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 34.0835,
            "range": "± 1.72115",
            "unit": "ns",
            "extra": "100 samples\n709 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 525.483,
            "range": "± 8.75544",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 121.795,
            "range": "± 980.843",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 2.77541,
            "range": "± 0.238009",
            "unit": "ns",
            "extra": "100 samples\n8809 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 17.177,
            "range": "± 922.065",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.09164,
            "range": "± 13.2278",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 33.7634,
            "range": "± 2.23986",
            "unit": "ns",
            "extra": "100 samples\n709 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 34.1226,
            "range": "± 2.07605",
            "unit": "ns",
            "extra": "100 samples\n719 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 525.659,
            "range": "± 14.7636",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 522.697,
            "range": "± 6.12793",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 121.631,
            "range": "± 377.059",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 123.881,
            "range": "± 11.7256",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 472.519,
            "range": "± 22.4289",
            "unit": "ns",
            "extra": "100 samples\n51 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.10245,
            "range": "± 56.8718",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 2.60794,
            "range": "± 218.537",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.2177,
            "range": "± 68.2452",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.15584,
            "range": "± 138.005",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.31324,
            "range": "± 132.945",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.48275,
            "range": "± 88.4029",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 0.9845900000000001,
            "range": "± 0.061127600000000004",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.30005,
            "range": "± 67.7673",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.07357,
            "range": "± 67.1601",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 5.8564,
            "range": "± 357.945",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 67.8284,
            "range": "± 2.62984",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 9.60605,
            "range": "± 593.912",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 131.857,
            "range": "± 3.68609",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 4.99102,
            "range": "± 184.922",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 5.51107,
            "range": "± 194.213",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 4.63724,
            "range": "± 262.943",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 6.42914,
            "range": "± 261.433",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 24.9197,
            "range": "± 404.832",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 9.95457,
            "range": "± 371.462",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 23.5659,
            "range": "± 347.826",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.52332,
            "range": "± 146.446",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 8.86845,
            "range": "± 187.518",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 8.01767,
            "range": "± 235.656",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 8.79452,
            "range": "± 177.471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 20.0363,
            "range": "± 937.456",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.83094,
            "range": "± 203.638",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 47.8921,
            "range": "± 546.736",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 42.7949,
            "range": "± 59.7657",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 40.3514,
            "range": "± 51.4142",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 53.97,
            "range": "± 98.7519",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 34.1145,
            "range": "± 30.3093",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 35.4887,
            "range": "± 30.2107",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 40.7899,
            "range": "± 46.446",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 51.8653,
            "range": "± 61.5547",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.8164,
            "range": "± 20.2931",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 44.4791,
            "range": "± 59.8791",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 30.3493,
            "range": "± 23.4542",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 39.8744,
            "range": "± 39.6376",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 80.9904,
            "range": "± 91.9365",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 40.0757,
            "range": "± 45.1489",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 90.1654,
            "range": "± 50.7773",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 11.2413,
            "range": "± 2.23587",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 12.5936,
            "range": "± 2.25207",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 15.4525,
            "range": "± 4.15042",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 12.6179,
            "range": "± 2.79536",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 14.7527,
            "range": "± 3.37745",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 13.1268,
            "range": "± 2.96684",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 13.9536,
            "range": "± 3.45096",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 12.699,
            "range": "± 3.28113",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 13.1513,
            "range": "± 3.33624",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 12.4383,
            "range": "± 2.30001",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 23.8622,
            "range": "± 4.27116",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 144.856,
            "range": "± 8.86019",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 32.6553,
            "range": "± 7.31153",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 234.82,
            "range": "± 12.8432",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 13.3676,
            "range": "± 4.33233",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 20.2674,
            "range": "± 5.0832",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 36.6596,
            "range": "± 9.1622",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 21.8916,
            "range": "± 4.988",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 37.2603,
            "range": "± 9.43391",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 24.828,
            "range": "± 5.16459",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 26.3979,
            "range": "± 5.54747",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 18.3299,
            "range": "± 4.62805",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 24.098,
            "range": "± 5.40726",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 21.8813,
            "range": "± 5.16793",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 73.5622,
            "range": "± 11.3226",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 809.944,
            "range": "± 22.6819",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 114.346,
            "range": "± 12.8187",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 1.70617,
            "range": "± 53.3082",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 34.5292,
            "range": "± 14.2278",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 2.8972,
            "range": "± 113.195",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 40.246,
            "range": "± 71.2071",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 34.9898,
            "range": "± 14.1045",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 47.7755,
            "range": "± 14.8728",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 4.51592,
            "range": "± 185.666",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 32.3056,
            "range": "± 30.5169",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 49.6459,
            "range": "± 16.3775",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.79071,
            "range": "± 181.7",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 1.83846,
            "range": "± 153.197",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 6.42968,
            "range": "± 1.24721",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 3.56841,
            "range": "± 492.088",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 19.0595,
            "range": "± 3.28438",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 9.80151,
            "range": "± 1.38996",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 6.4129,
            "range": "± 1.20831",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 3.6843,
            "range": "± 498.318",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 12.5007,
            "range": "± 2.96309",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 6.51683,
            "range": "± 1.08462",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 6.49754,
            "range": "± 1.51684",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 3.72241,
            "range": "± 483.529",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 10.5193,
            "range": "± 1.94018",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 7.80922,
            "range": "± 1.23348",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 5.8635,
            "range": "± 604.254",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 5.93927,
            "range": "± 618.967",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 7.44806,
            "range": "± 1.35005",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 4.65529,
            "range": "± 604.034",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 5.82484,
            "range": "± 1.02135",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.02964,
            "range": "± 417.274",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 30.4337,
            "range": "± 6.81674",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 30.1798,
            "range": "± 6.87712",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.13193,
            "range": "± 38.2485",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.13391,
            "range": "± 28.1813",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 73.6661,
            "range": "± 11.1804",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 38.2564,
            "range": "± 8.16948",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 3.34687,
            "range": "± 54.2151",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 1.79013,
            "range": "± 30.0181",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "372107d97e6597ac0c547ad42f332e45a66bf11d",
          "message": "Print function args and return type in IR dumps (#269)",
          "timestamp": "2026-04-25T18:00:47+02:00",
          "tree_id": "f575d5553f0b91b198f91a988b4de2e328b4aa15",
          "url": "https://github.com/nebulastream/nautilus/commit/372107d97e6597ac0c547ad42f332e45a66bf11d"
        },
        "date": 1777133152511,
        "tool": "catch2",
        "benches": [
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 54.4104,
            "range": "± 8.95961",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.56332,
            "range": "± 137.484",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 619.537,
            "range": "± 36.4819",
            "unit": "ns",
            "extra": "100 samples\n48 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.50086,
            "range": "± 81.3082",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.40164,
            "range": "± 221.664",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.6028,
            "range": "± 81.8544",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.81556,
            "range": "± 178.402",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.74588,
            "range": "± 150.572",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.98007,
            "range": "± 159.867",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.27379,
            "range": "± 75.6583",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.74462,
            "range": "± 138.229",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.43131,
            "range": "± 85.7994",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.59059,
            "range": "± 542.627",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 90.8972,
            "range": "± 5.45101",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.521,
            "range": "± 1.43098",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 182.378,
            "range": "± 11.455",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 182.58,
            "range": "± 9.12009",
            "unit": "ns",
            "extra": "100 samples\n159 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 456.382,
            "range": "± 29.1011",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.16708,
            "range": "± 76.3917",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 483.381,
            "range": "± 27.8521",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 917.335,
            "range": "± 64.8841",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 497.03,
            "range": "± 29.4655",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 592.786,
            "range": "± 37.3542",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 434.579,
            "range": "± 23.1642",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 507.457,
            "range": "± 31.369",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 451.343,
            "range": "± 43.8671",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.2584,
            "range": "± 121.6",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.26978,
            "range": "± 171.719",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.79006,
            "range": "± 935.383",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.67657,
            "range": "± 442.12",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.0941,
            "range": "± 2.84492",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.8557,
            "range": "± 1.23903",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.73548,
            "range": "± 1.04218",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.85768,
            "range": "± 754.464",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.0237,
            "range": "± 2.29432",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.65966,
            "range": "± 1.23288",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.78359,
            "range": "± 851.083",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.88873,
            "range": "± 470.685",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.2892,
            "range": "± 1.33491",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.3502,
            "range": "± 931.396",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.25092,
            "range": "± 495.313",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.31644,
            "range": "± 711.198",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.1527,
            "range": "± 1.06301",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.18477,
            "range": "± 757.785",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.93711,
            "range": "± 907.056",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.9689,
            "range": "± 474.632",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.7926,
            "range": "± 4.39903",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 38.6114,
            "range": "± 3.52553",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.39609,
            "range": "± 55.2737",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.39202,
            "range": "± 16.5559",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 99.5334,
            "range": "± 6.60462",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 49.8175,
            "range": "± 6.29341",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.48381,
            "range": "± 36.9859",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.3486,
            "range": "± 29.2588",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 54.7859,
            "range": "± 8.01448",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.29504,
            "range": "± 91.0984",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 40.6336,
            "range": "± 2.31122",
            "unit": "ns",
            "extra": "100 samples\n716 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 287.544,
            "range": "± 6.02503",
            "unit": "ns",
            "extra": "100 samples\n8277 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.01811,
            "range": "± 0.721024",
            "unit": "ns",
            "extra": "100 samples\n7729 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.3544,
            "range": "± 1.96528",
            "unit": "ns",
            "extra": "100 samples\n757 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.506,
            "range": "± 1.72291",
            "unit": "ns",
            "extra": "100 samples\n8435 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.5047,
            "range": "± 1.82455",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 527.829,
            "range": "± 20.6702",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.67513,
            "range": "± 0.602962",
            "unit": "ns",
            "extra": "100 samples\n6425 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.2921,
            "range": "± 6.18815",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9457,
            "range": "± 172.806",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 50.0819,
            "range": "± 8.1899",
            "unit": "ns",
            "extra": "100 samples\n599 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 844.825,
            "range": "± 17.5308",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 175.618,
            "range": "± 13.0318",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.59202,
            "range": "± 0.764682",
            "unit": "ns",
            "extra": "100 samples\n8460 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 20.8781,
            "range": "± 3.36653",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.85523,
            "range": "± 44.0952",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 49.3641,
            "range": "± 7.08352",
            "unit": "ns",
            "extra": "100 samples\n601 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 50.2469,
            "range": "± 9.12771",
            "unit": "ns",
            "extra": "100 samples\n602 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 843.303,
            "range": "± 14.8413",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 845.205,
            "range": "± 19.0519",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 179.055,
            "range": "± 9.21709",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 177.824,
            "range": "± 15.5412",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.59397,
            "range": "± 148.25",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.1654,
            "range": "± 121.901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.09349,
            "range": "± 122.937",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.3224,
            "range": "± 183.702",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.3991,
            "range": "± 344.056",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.1469,
            "range": "± 168.834",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.4173,
            "range": "± 279.421",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.06699,
            "range": "± 117.709",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.6901,
            "range": "± 296.197",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.88788,
            "range": "± 255.672",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.712,
            "range": "± 138.942",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.184,
            "range": "± 1.23411",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.90083,
            "range": "± 244.213",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.1712,
            "range": "± 403.241",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.7615,
            "range": "± 749.522",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.5843,
            "range": "± 410.074",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.8901,
            "range": "± 523.286",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.4157,
            "range": "± 272.471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.3116,
            "range": "± 254.167",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.7045,
            "range": "± 514.043",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.7815,
            "range": "± 238.322",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.0646,
            "range": "± 346.572",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.4566,
            "range": "± 1.17525",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.176,
            "range": "± 289.832",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.4871,
            "range": "± 337.05",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.9278,
            "range": "± 700.796",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.8591,
            "range": "± 290.179",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.9446,
            "range": "± 687.047",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.7638,
            "range": "± 2.57308",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 20.159,
            "range": "± 5.01441",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.0415,
            "range": "± 3.92402",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 19.102,
            "range": "± 4.18315",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.9605,
            "range": "± 4.15894",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.4368,
            "range": "± 3.62401",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.3381,
            "range": "± 4.54902",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.7875,
            "range": "± 3.621",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.038,
            "range": "± 4.30861",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.8859,
            "range": "± 4.72402",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.5908,
            "range": "± 5.01825",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 200.773,
            "range": "± 15.0961",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 51.4084,
            "range": "± 7.75536",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 313.218,
            "range": "± 17.4299",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 22.1107,
            "range": "± 8.49649",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.6074,
            "range": "± 5.79668",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 55.9146,
            "range": "± 8.28231",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 41.2731,
            "range": "± 8.38285",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 58.245,
            "range": "± 10.3377",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 46.468,
            "range": "± 10.8509",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 48.2615,
            "range": "± 9.19757",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.3749,
            "range": "± 5.60941",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 43.4564,
            "range": "± 8.27571",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 40.3962,
            "range": "± 8.29466",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 104.343,
            "range": "± 14.089",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1051.75,
            "range": "± 22730.6",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 155.171,
            "range": "± 12.7413",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.19101,
            "range": "± 44.4322",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "0805493c84f7e3ce005b9e03b11ed3db2fab860f",
          "message": "Add VS Code extension for Nautilus IR (#267)",
          "timestamp": "2026-04-25T18:28:38+02:00",
          "tree_id": "f75a2d19f6eb2b19f5ae1230fb8ece28b9d232b7",
          "url": "https://github.com/nebulastream/nautilus/commit/0805493c84f7e3ce005b9e03b11ed3db2fab860f"
        },
        "date": 1777134826666,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 176.367,
            "range": "± 12.4925",
            "unit": "ns",
            "extra": "100 samples\n175 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 429.157,
            "range": "± 22.6664",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.0689,
            "range": "± 61.8558",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 467.293,
            "range": "± 62.0738",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 815.578,
            "range": "± 44.8433",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 457.363,
            "range": "± 30.4163",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 545.421,
            "range": "± 38.7409",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 429.744,
            "range": "± 30.9281",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 471.73,
            "range": "± 24.99",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 421.886,
            "range": "± 21.4701",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ir_add",
            "value": 574.097,
            "range": "± 39.6549",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.41565,
            "range": "± 83.0177",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.15374,
            "range": "± 172.282",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.52537,
            "range": "± 219.524",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.60759,
            "range": "± 179.01",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.57927,
            "range": "± 101.415",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.79466,
            "range": "± 106.256",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.20037,
            "range": "± 73.4685",
            "unit": "us",
            "extra": "100 samples\n26 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.56561,
            "range": "± 102.173",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.29546,
            "range": "± 80.3113",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.18524,
            "range": "± 507.475",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 94.1927,
            "range": "± 5.78112",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.5913,
            "range": "± 799.917",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 173.39,
            "range": "± 7.87145",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.07515,
            "range": "± 180.296",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.08854,
            "range": "± 169.144",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.00134,
            "range": "± 1.1291",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.20173,
            "range": "± 479.65",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 23.4615,
            "range": "± 3.68329",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 11.824,
            "range": "± 1.71419",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 7.7731,
            "range": "± 1.22503",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.29191,
            "range": "± 579.756",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 15.074,
            "range": "± 2.01083",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 7.99542,
            "range": "± 1.29304",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 7.83059,
            "range": "± 1.40378",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.34534,
            "range": "± 486.03",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 12.6511,
            "range": "± 2.25646",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 9.09764,
            "range": "± 1.3512",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.02112,
            "range": "± 704.084",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.05486,
            "range": "± 712.874",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.14423,
            "range": "± 1.3965",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.58248,
            "range": "± 732.254",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.15737,
            "range": "± 1.02195",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.61537,
            "range": "± 385.615",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 36.5526,
            "range": "± 6.80817",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 36.6558,
            "range": "± 6.73606",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.44265,
            "range": "± 37.7869",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.47275,
            "range": "± 147.295",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 92.2129,
            "range": "± 10.291",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 46.5739,
            "range": "± 7.94782",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.60619,
            "range": "± 166.103",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.18757,
            "range": "± 28.9027",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 44.7466,
            "range": "± 14.5872",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.58367,
            "range": "± 128.779",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.6145,
            "range": "± 299.843",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 45.7908,
            "range": "± 15.5018",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 62.3963,
            "range": "± 17.9242",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.62424,
            "range": "± 131.917",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.5536,
            "range": "± 233.101",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 63.2456,
            "range": "± 17.6061",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.6576,
            "range": "± 0.991841",
            "unit": "ns",
            "extra": "100 samples\n8808 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.8539,
            "range": "± 1.74734",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 552.824,
            "range": "± 65.3336",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.49869,
            "range": "± 0.483184",
            "unit": "ns",
            "extra": "100 samples\n7212 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 107.013,
            "range": "± 4.74178",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.5527,
            "range": "± 135.091",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 42.8723,
            "range": "± 3.83581",
            "unit": "ns",
            "extra": "100 samples\n731 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 685.516,
            "range": "± 13.395",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 250.23,
            "range": "± 92.4725",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.56369,
            "range": "± 0.365164",
            "unit": "ns",
            "extra": "100 samples\n8813 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.8027,
            "range": "± 3.41867",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.29416,
            "range": "± 28.6855",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 43.6543,
            "range": "± 4.95053",
            "unit": "ns",
            "extra": "100 samples\n719 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 42.4954,
            "range": "± 1.9199",
            "unit": "ns",
            "extra": "100 samples\n732 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 683.681,
            "range": "± 14.7038",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 676.079,
            "range": "± 8.52969",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 158.437,
            "range": "± 5.72461",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 157.399,
            "range": "± 1.88941",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 36.4557,
            "range": "± 3.09927",
            "unit": "ns",
            "extra": "100 samples\n861 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 302.347,
            "range": "± 7.6278",
            "unit": "ns",
            "extra": "100 samples\n8599 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.09671,
            "range": "± 0.465233",
            "unit": "ns",
            "extra": "100 samples\n7913 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 41.4487,
            "range": "± 2.4407",
            "unit": "ns",
            "extra": "100 samples\n753 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 46.3851,
            "range": "± 17.0725",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.17558,
            "range": "± 174.186",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.16591,
            "range": "± 178.123",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.76589,
            "range": "± 168.692",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.62683,
            "range": "± 151.025",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.80228,
            "range": "± 205.89",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.3791,
            "range": "± 182.624",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.551,
            "range": "± 203.465",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.3807,
            "range": "± 212.179",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.57849,
            "range": "± 151.475",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.2158,
            "range": "± 282.051",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.0003,
            "range": "± 185.187",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.0299,
            "range": "± 187.95",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.2171,
            "range": "± 215.812",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.3255,
            "range": "± 180.649",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.1679,
            "range": "± 204.988",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.8411,
            "range": "± 297.96",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.5363,
            "range": "± 264.202",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.5642,
            "range": "± 281.955",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.6031,
            "range": "± 267.417",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.4281,
            "range": "± 261.182",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.9387,
            "range": "± 413.373",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.054,
            "range": "± 345.845",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.2919,
            "range": "± 246.503",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.6268,
            "range": "± 287.712",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.5573,
            "range": "± 270.275",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.6656,
            "range": "± 315.833",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.1685,
            "range": "± 396.885",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.1103,
            "range": "± 399.151",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.9826,
            "range": "± 454.842",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.8795,
            "range": "± 3.06571",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.275,
            "range": "± 3.49937",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 19.8876,
            "range": "± 3.78642",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.3598,
            "range": "± 3.01378",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 19.1425,
            "range": "± 3.85308",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 17.3628,
            "range": "± 4.10692",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 18.2636,
            "range": "± 4.68784",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.3902,
            "range": "± 3.26792",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.6219,
            "range": "± 3.72122",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 15.8066,
            "range": "± 1.80086",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 31.441,
            "range": "± 4.56336",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 192.69,
            "range": "± 13.9275",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 43.3099,
            "range": "± 8.11926",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 308.235,
            "range": "± 20.6476",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 17.3503,
            "range": "± 4.20456",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 26.6174,
            "range": "± 5.95023",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 47.1767,
            "range": "± 8.86768",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 28.5431,
            "range": "± 6.64092",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 47.6898,
            "range": "± 11.6867",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 32.398,
            "range": "± 6.65969",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 33.9816,
            "range": "± 6.23328",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 24.1937,
            "range": "± 5.75872",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 30.6312,
            "range": "± 5.78251",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 28.6682,
            "range": "± 7.13123",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 95.8113,
            "range": "± 12.7893",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1051.25,
            "range": "± 16441.9",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 147.162,
            "range": "± 14.955",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.19622,
            "range": "± 72.4231",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "13d30890af186e2ed0376c40776752ab904c74d6",
          "message": "Fix CUDA backend: identify host/device functions via IR attributes (#263)",
          "timestamp": "2026-04-25T23:16:16+02:00",
          "tree_id": "11675af93b149bc35ef18cb83819133107763602",
          "url": "https://github.com/nebulastream/nautilus/commit/13d30890af186e2ed0376c40776752ab904c74d6"
        },
        "date": 1777152075070,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_bc_addOne",
            "value": 35.9021,
            "range": "± 1.1682",
            "unit": "ns",
            "extra": "100 samples\n856 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 305.081,
            "range": "± 11.1843",
            "unit": "ns",
            "extra": "100 samples\n8613 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.20918,
            "range": "± 0.627832",
            "unit": "ns",
            "extra": "100 samples\n7919 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 42.0296,
            "range": "± 2.48304",
            "unit": "ns",
            "extra": "100 samples\n744 iterations"
          },
          {
            "name": "ir_add",
            "value": 709.633,
            "range": "± 41.9481",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.52388,
            "range": "± 99.8474",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.46594,
            "range": "± 313.008",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.65312,
            "range": "± 145.092",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.93951,
            "range": "± 285.184",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.78409,
            "range": "± 133.666",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.01591,
            "range": "± 143.926",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.3543,
            "range": "± 71.3206",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.77526,
            "range": "± 117.473",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.45312,
            "range": "± 90.0712",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.92445,
            "range": "± 584.986",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 93.0222,
            "range": "± 3.50703",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.7568,
            "range": "± 867.842",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 182.354,
            "range": "± 5.47747",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 177.702,
            "range": "± 7.19669",
            "unit": "ns",
            "extra": "100 samples\n175 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 432.887,
            "range": "± 29.8481",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.24007,
            "range": "± 306.122",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 456.521,
            "range": "± 34.8997",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 867.29,
            "range": "± 54.9955",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 462.775,
            "range": "± 26.2413",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 559.475,
            "range": "± 30.7388",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 462.337,
            "range": "± 21.216",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 474.266,
            "range": "± 28.0694",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 416.026,
            "range": "± 27.8588",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.30816,
            "range": "± 221.899",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.2639,
            "range": "± 213.499",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.48001,
            "range": "± 1.62181",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.5202,
            "range": "± 520.469",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 24.8484,
            "range": "± 3.64799",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.885,
            "range": "± 1.97283",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.31457,
            "range": "± 1.34835",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.64205,
            "range": "± 579.064",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.3724,
            "range": "± 3.26305",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.48641,
            "range": "± 1.22458",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.37684,
            "range": "± 1.45777",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.79798,
            "range": "± 708.86",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.9892,
            "range": "± 2.27989",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.2479,
            "range": "± 1.7374",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.46211,
            "range": "± 890.535",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.47262,
            "range": "± 900.713",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.59929,
            "range": "± 1.31667",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.99547,
            "range": "± 667.202",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.56361,
            "range": "± 1.02127",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.82471,
            "range": "± 403.351",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 40.2904,
            "range": "± 7.50005",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 40.0729,
            "range": "± 7.81706",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.47913,
            "range": "± 34.5694",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.48976,
            "range": "± 29.3028",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 96.1748,
            "range": "± 11.7272",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.0174,
            "range": "± 10.0785",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.5256,
            "range": "± 141.13",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.31372,
            "range": "± 30.7835",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 46.0893,
            "range": "± 17.9752",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.59559,
            "range": "± 122.555",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.4467,
            "range": "± 331.755",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 46.203,
            "range": "± 16.3946",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 61.704,
            "range": "± 17.072",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.61342,
            "range": "± 148.045",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.3216,
            "range": "± 310.967",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 63.3109,
            "range": "± 18.5419",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.9793,
            "range": "± 1.14099",
            "unit": "ns",
            "extra": "100 samples\n8832 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.6223,
            "range": "± 2.15963",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 543.961,
            "range": "± 61.2852",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.51991,
            "range": "± 0.514147",
            "unit": "ns",
            "extra": "100 samples\n7246 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 107.059,
            "range": "± 4.45223",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.5533,
            "range": "± 116.297",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.5913,
            "range": "± 3.92889",
            "unit": "ns",
            "extra": "100 samples\n722 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 677.644,
            "range": "± 10.0958",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 156.901,
            "range": "± 405.013",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.52569,
            "range": "± 0.250923",
            "unit": "ns",
            "extra": "100 samples\n8829 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.5611,
            "range": "± 3.00886",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.29288,
            "range": "± 20.3449",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 43.4519,
            "range": "± 2.53192",
            "unit": "ns",
            "extra": "100 samples\n719 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 44.2186,
            "range": "± 5.82844",
            "unit": "ns",
            "extra": "100 samples\n717 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 685.257,
            "range": "± 20.3716",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 679.894,
            "range": "± 18.866",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 156.884,
            "range": "± 437.923",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 156.91,
            "range": "± 389.384",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 44.9885,
            "range": "± 14.9511",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.10904,
            "range": "± 153.121",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.156,
            "range": "± 157.698",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.74269,
            "range": "± 162.581",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.6081,
            "range": "± 135.146",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.75988,
            "range": "± 173.73",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.1776,
            "range": "± 208.635",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.4592,
            "range": "± 179.244",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.2178,
            "range": "± 211.965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.56878,
            "range": "± 138.768",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.144,
            "range": "± 168.272",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.97434,
            "range": "± 174.766",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.0325,
            "range": "± 164.639",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.2958,
            "range": "± 164.657",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.2674,
            "range": "± 163.801",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.9186,
            "range": "± 192.314",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.6378,
            "range": "± 275.425",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.1893,
            "range": "± 292.347",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.2797,
            "range": "± 273.107",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.3619,
            "range": "± 316.678",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.4808,
            "range": "± 1.21598",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.695,
            "range": "± 366.907",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.7832,
            "range": "± 270.684",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.9003,
            "range": "± 269.573",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.6222,
            "range": "± 707.015",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.2069,
            "range": "± 294.022",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.3589,
            "range": "± 279.494",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.6043,
            "range": "± 361.771",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.723,
            "range": "± 280.841",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.575,
            "range": "± 1.55263",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.2848,
            "range": "± 2.07335",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.6459,
            "range": "± 3.65676",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.2789,
            "range": "± 3.90828",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.1183,
            "range": "± 2.53876",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 18.7999,
            "range": "± 3.95396",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 16.8599,
            "range": "± 4.20688",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 18.1012,
            "range": "± 4.16197",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.377,
            "range": "± 3.58766",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 17.0196,
            "range": "± 4.33979",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 16.0268,
            "range": "± 2.48101",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 30.9154,
            "range": "± 6.02429",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 188.738,
            "range": "± 9.42572",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 41.7196,
            "range": "± 8.34616",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 302.309,
            "range": "± 13.1774",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 17.3136,
            "range": "± 4.9683",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 26.9575,
            "range": "± 6.07966",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 49.558,
            "range": "± 12.19",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 30.1257,
            "range": "± 6.18348",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 49.5736,
            "range": "± 12.7948",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 32.3175,
            "range": "± 6.64558",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 34.7806,
            "range": "± 5.68519",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 23.6859,
            "range": "± 4.57706",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 32.4173,
            "range": "± 6.04141",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 28.8756,
            "range": "± 6.02228",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 95.9749,
            "range": "± 15.2463",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1035.97,
            "range": "± 18995.5",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 148.785,
            "range": "± 14.5985",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.18428,
            "range": "± 36.4572",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "63d7373fcff141e65bca2d458b539bf1d07bdd01",
          "message": "Carry trace-time source locations onto IR operations (#271)",
          "timestamp": "2026-04-26T16:02:49+02:00",
          "tree_id": "9675ec81345a6651480b2e4fd8d09a230c0c5b92",
          "url": "https://github.com/nebulastream/nautilus/commit/63d7373fcff141e65bca2d458b539bf1d07bdd01"
        },
        "date": 1777212575868,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 185.47,
            "range": "± 10.8583",
            "unit": "ns",
            "extra": "100 samples\n157 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 466.434,
            "range": "± 28.938",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.15114,
            "range": "± 103.312",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 505.081,
            "range": "± 57.0378",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 895.76,
            "range": "± 69.727",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 505.763,
            "range": "± 47.5456",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 609.238,
            "range": "± 40.5294",
            "unit": "ns",
            "extra": "100 samples\n52 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 502.653,
            "range": "± 57.8452",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 519.293,
            "range": "± 36.0981",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 456.287,
            "range": "± 29.3576",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 37.1677,
            "range": "± 3.32233",
            "unit": "ns",
            "extra": "100 samples\n783 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 282.01,
            "range": "± 4.78701",
            "unit": "ns",
            "extra": "100 samples\n8277 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.01205,
            "range": "± 0.720511",
            "unit": "ns",
            "extra": "100 samples\n7702 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.3869,
            "range": "± 1.95004",
            "unit": "ns",
            "extra": "100 samples\n757 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.26384,
            "range": "± 134.846",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.26676,
            "range": "± 148.007",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.61794,
            "range": "± 723.043",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.51253,
            "range": "± 329.207",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.896,
            "range": "± 1.68439",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.4744,
            "range": "± 995.991",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.59519,
            "range": "± 749.263",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.58826,
            "range": "± 369.418",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.7598,
            "range": "± 1.62139",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.3764,
            "range": "± 684.152",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.67972,
            "range": "± 776.849",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.76406,
            "range": "± 361.804",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.0504,
            "range": "± 1.74909",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 9.92695,
            "range": "± 796.703",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.43972,
            "range": "± 1.08414",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.45259,
            "range": "± 1.92793",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.1069,
            "range": "± 1.94061",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.9474,
            "range": "± 672.844",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.83767,
            "range": "± 825.448",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.87538,
            "range": "± 299.988",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.3315,
            "range": "± 4.80182",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 38.5176,
            "range": "± 4.55114",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.39058,
            "range": "± 15.8534",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.39501,
            "range": "± 16.4655",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 98.8898,
            "range": "± 4.19717",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.2811,
            "range": "± 4.67785",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.49324,
            "range": "± 78.4984",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.32025,
            "range": "± 19.5336",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.0137,
            "range": "± 1.26432",
            "unit": "ns",
            "extra": "100 samples\n8429 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.8357,
            "range": "± 1.72768",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 515.951,
            "range": "± 15.914",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.68672,
            "range": "± 0.648729",
            "unit": "ns",
            "extra": "100 samples\n6486 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.3115,
            "range": "± 10.4835",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.093,
            "range": "± 638.815",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.3895,
            "range": "± 6.77672",
            "unit": "ns",
            "extra": "100 samples\n679 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 845.409,
            "range": "± 31.7346",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 209.77,
            "range": "± 30.6229",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.46416,
            "range": "± 0.282446",
            "unit": "ns",
            "extra": "100 samples\n8452 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 20.6293,
            "range": "± 2.13057",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.85394,
            "range": "± 30.3596",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 43.9212,
            "range": "± 6.36911",
            "unit": "ns",
            "extra": "100 samples\n684 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 43.6164,
            "range": "± 4.02327",
            "unit": "ns",
            "extra": "100 samples\n674 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 842.767,
            "range": "± 12.0415",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 863.738,
            "range": "± 111.008",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 180.236,
            "range": "± 1.96681",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 181.219,
            "range": "± 9.35965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 54.8657,
            "range": "± 7.52175",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.52498,
            "range": "± 123.995",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.5262,
            "range": "± 56.914",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.16488,
            "range": "± 105.145",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.04256,
            "range": "± 74.5663",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.08001,
            "range": "± 61.7236",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 28.3249,
            "range": "± 150.009",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.7015,
            "range": "± 113.658",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 27.5447,
            "range": "± 381.342",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.02122,
            "range": "± 162.478",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.4735,
            "range": "± 377.84",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.37037,
            "range": "± 138.504",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.5865,
            "range": "± 229.526",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 24.6768,
            "range": "± 589.965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.48194,
            "range": "± 62.9605",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 19.959,
            "range": "± 119.132",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.6217,
            "range": "± 308.195",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.0222,
            "range": "± 293.11",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.0805,
            "range": "± 233.763",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.3597,
            "range": "± 307.485",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.2746,
            "range": "± 321.685",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.6312,
            "range": "± 298.302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.9335,
            "range": "± 569.321",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.0072,
            "range": "± 241.292",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.3594,
            "range": "± 247.301",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.1837,
            "range": "± 291.232",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.1453,
            "range": "± 280.597",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.7491,
            "range": "± 591.868",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.6895,
            "range": "± 430.891",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.2305,
            "range": "± 903.809",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.4327,
            "range": "± 2.07884",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.258,
            "range": "± 2.77051",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.1165,
            "range": "± 2.99661",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.0177,
            "range": "± 2.66056",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.9003,
            "range": "± 3.01096",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.8844,
            "range": "± 3.44792",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.8893,
            "range": "± 3.25031",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.9117,
            "range": "± 2.41531",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.5955,
            "range": "± 3.04047",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 17.8936,
            "range": "± 2.97678",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.051,
            "range": "± 3.55238",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 199.719,
            "range": "± 11.7792",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 50.663,
            "range": "± 6.30623",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 305.604,
            "range": "± 13.5113",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.3122,
            "range": "± 3.65446",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 31.9878,
            "range": "± 3.03052",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 56.1641,
            "range": "± 7.28523",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 34.8279,
            "range": "± 4.21294",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 56.2124,
            "range": "± 6.48303",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 44.9087,
            "range": "± 5.86418",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 49.1392,
            "range": "± 8.29272",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.3256,
            "range": "± 5.54004",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 43.5538,
            "range": "± 6.77694",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 36.3763,
            "range": "± 8.65323",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 102.121,
            "range": "± 7.80504",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1051.8400000000001,
            "range": "± 16503.1",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 155.645,
            "range": "± 13.2491",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.15925,
            "range": "± 31.915",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 53.9144,
            "range": "± 8.01711",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.39304,
            "range": "± 616.124",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 24.3788,
            "range": "± 333.861",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 54.1421,
            "range": "± 9.76956",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 72.8987,
            "range": "± 8.33971",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.29028,
            "range": "± 76.8287",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.3196,
            "range": "± 245.318",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 74.643,
            "range": "± 9.1889",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 732.22,
            "range": "± 43.8461",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.57536,
            "range": "± 118.379",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.31725,
            "range": "± 225.906",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.59726,
            "range": "± 137.68",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.76982,
            "range": "± 169.109",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.73728,
            "range": "± 96.9088",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.95676,
            "range": "± 180.825",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.41797,
            "range": "± 180.493",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.67703,
            "range": "± 82.6636",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.4574,
            "range": "± 156.616",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.6256,
            "range": "± 445.441",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 91.7696,
            "range": "± 6.65442",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.669,
            "range": "± 722.9",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 171.11,
            "range": "± 8.67329",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "dc99ff67846069bcd31420a741057dd4626246ec",
          "message": "Add ENABLE_SHORT_CIRCUIT_BOOL option for short-circuit && / || (#273)",
          "timestamp": "2026-04-26T16:36:34+02:00",
          "tree_id": "c2e1d2fa6bbfb3f9d46bee9efcaee4ac165d5d6d",
          "url": "https://github.com/nebulastream/nautilus/commit/dc99ff67846069bcd31420a741057dd4626246ec"
        },
        "date": 1777214608967,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.88715,
            "range": "± 156.394",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 1.87233,
            "range": "± 140.493",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 7.89554,
            "range": "± 1.30569",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 3.87661,
            "range": "± 459.575",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 23.4754,
            "range": "± 2.33951",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 10.834,
            "range": "± 908.732",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 7.81114,
            "range": "± 909.761",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.07866,
            "range": "± 560.237",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 15.3587,
            "range": "± 1.85009",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 7.42382,
            "range": "± 1.23935",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 7.94756,
            "range": "± 1.14457",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.10163,
            "range": "± 266.166",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 11.9972,
            "range": "± 1.25241",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 8.36639,
            "range": "± 764.998",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 6.46907,
            "range": "± 375.115",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 6.51088,
            "range": "± 425.037",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 8.93424,
            "range": "± 589.798",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.21802,
            "range": "± 732.807",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.24667,
            "range": "± 974.573",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.46068,
            "range": "± 353.775",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 32.6852,
            "range": "± 3.46448",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 33.0774,
            "range": "± 3.17071",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.3007,
            "range": "± 12.8755",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.32384,
            "range": "± 17.9172",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 90.0147,
            "range": "± 8.98255",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 43.6389,
            "range": "± 7.73664",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.02401,
            "range": "± 23.8633",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.12331,
            "range": "± 21.8239",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.74575,
            "range": "± 168.272",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 7.4353,
            "range": "± 304.925",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 6.27329,
            "range": "± 161.956",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.58429,
            "range": "± 216.404",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.091,
            "range": "± 288.094",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.8299,
            "range": "± 191.442",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.2958,
            "range": "± 295.64",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 6.19563,
            "range": "± 188.598",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.7443,
            "range": "± 184.349",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.415,
            "range": "± 200.557",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.4977,
            "range": "± 260.306",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.8696,
            "range": "± 353.174",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.155,
            "range": "± 485.12",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 60.7139,
            "range": "± 320.482",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 23.3483,
            "range": "± 388.779",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 23.7386,
            "range": "± 338.272",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 25.128,
            "range": "± 407.322",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 24.0156,
            "range": "± 420.144",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 24.8608,
            "range": "± 367.393",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 24.2742,
            "range": "± 370.767",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 24.5702,
            "range": "± 386.249",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 23.8852,
            "range": "± 347.302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 24.0337,
            "range": "± 409.265",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 24.0026,
            "range": "± 778.767",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 26.7036,
            "range": "± 321.868",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 59.5103,
            "range": "± 609.644",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 28.9074,
            "range": "± 422.257",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 88.7346,
            "range": "± 814.179",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 10.1865,
            "range": "± 1.71744",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 13.3242,
            "range": "± 2.35422",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 17.2133,
            "range": "± 2.85723",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 12.7378,
            "range": "± 2.70056",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 15.0359,
            "range": "± 2.7018",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 13.2146,
            "range": "± 2.42466",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 14.0688,
            "range": "± 2.4589",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 12.3784,
            "range": "± 2.49359",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 13.1138,
            "range": "± 2.65115",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 12.2534,
            "range": "± 2.26341",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 32.4725,
            "range": "± 5.379",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 199.632,
            "range": "± 11.0866",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 39.7335,
            "range": "± 5.33065",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 305.117,
            "range": "± 12.0584",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 15.0271,
            "range": "± 4.12282",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 27.1317,
            "range": "± 7.3765",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 42.8425,
            "range": "± 7.49401",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 30.3445,
            "range": "± 8.03976",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 41.6922,
            "range": "± 7.33422",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 31.9392,
            "range": "± 7.63028",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 34.4745,
            "range": "± 7.03213",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 22.8331,
            "range": "± 5.89124",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 31.7678,
            "range": "± 7.26336",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 29.4722,
            "range": "± 7.53366",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 85.3111,
            "range": "± 9.72798",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 984.753,
            "range": "± 28.503",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 134.775,
            "range": "± 12.9576",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.13539,
            "range": "± 118.059",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 12.3715,
            "range": "± 1.11485",
            "unit": "ns",
            "extra": "100 samples\n6265 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 27.2559,
            "range": "± 2.07465",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 540.61,
            "range": "± 19.7788",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.25656,
            "range": "± 0.650045",
            "unit": "ns",
            "extra": "100 samples\n3818 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 130.951,
            "range": "± 5.15779",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 25.3331,
            "range": "± 122.454",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 36.0451,
            "range": "± 2.35617",
            "unit": "ns",
            "extra": "100 samples\n515 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 551.221,
            "range": "± 12.3942",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 118.106,
            "range": "± 2.12142",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 2.92109,
            "range": "± 0.132287",
            "unit": "ns",
            "extra": "100 samples\n6286 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 28.6858,
            "range": "± 5.36548",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 6.06047,
            "range": "± 55.0682",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 36.3339,
            "range": "± 4.2149",
            "unit": "ns",
            "extra": "100 samples\n507 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 36.8861,
            "range": "± 6.48166",
            "unit": "ns",
            "extra": "100 samples\n506 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 550.224,
            "range": "± 9.60884",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 553.127,
            "range": "± 13.5432",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 117.817,
            "range": "± 1.80426",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 117.744,
            "range": "± 1.65366",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 33.7678,
            "range": "± 10.7893",
            "unit": "ns",
            "extra": "100 samples\n548 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 422.81,
            "range": "± 11.3708",
            "unit": "ns",
            "extra": "100 samples\n6194 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.65339,
            "range": "± 0.324812",
            "unit": "ns",
            "extra": "100 samples\n5672 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 36.3663,
            "range": "± 2.01444",
            "unit": "ns",
            "extra": "100 samples\n478 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 37.3993,
            "range": "± 10.5748",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.71856,
            "range": "± 126.948",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 22.9516,
            "range": "± 421.053",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 38.0715,
            "range": "± 9.42079",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 55.1349,
            "range": "± 12.5935",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.6831,
            "range": "± 135.743",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 23.7765,
            "range": "± 488.797",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 58.7315,
            "range": "± 13.1658",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 192.023,
            "range": "± 18.772",
            "unit": "ns",
            "extra": "100 samples\n97 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 451.182,
            "range": "± 30.5224",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.4254,
            "range": "± 252.698",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 488.819,
            "range": "± 54.6127",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 1052.08,
            "range": "± 96280",
            "unit": "ns",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 504.765,
            "range": "± 45.3522",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 600.767,
            "range": "± 34.825",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 465.574,
            "range": "± 34.2829",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 511.093,
            "range": "± 42.6039",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 453.689,
            "range": "± 47.4179",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ir_add",
            "value": 819.498,
            "range": "± 72.6208",
            "unit": "ns",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.69289,
            "range": "± 114.732",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.72447,
            "range": "± 235.957",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.76261,
            "range": "± 124.888",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 3.22553,
            "range": "± 206.271",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.02652,
            "range": "± 133.994",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.151,
            "range": "± 111.019",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.55551,
            "range": "± 119.597",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.88514,
            "range": "± 140.152",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.61542,
            "range": "± 125.807",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 8.59845,
            "range": "± 454.674",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 94.955,
            "range": "± 4.79425",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 13.3264,
            "range": "± 918.952",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 169.743,
            "range": "± 3.75303",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 38.7065,
            "range": "± 12.4613",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.79604,
            "range": "± 184.273",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "0ffec01c3c0170e4c70c4533adf49531af5077b6",
          "message": "Add control-flow graph view to the Nautilus IR VS Code extension (#270)",
          "timestamp": "2026-04-26T17:12:23+02:00",
          "tree_id": "a5c059348f5d212e8588a6c9935700c0d86d062f",
          "url": "https://github.com/nebulastream/nautilus/commit/0ffec01c3c0170e4c70c4533adf49531af5077b6"
        },
        "date": 1777216638565,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.18982,
            "range": "± 187.756",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.22448,
            "range": "± 393.004",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.64446,
            "range": "± 697.498",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.34098,
            "range": "± 404.891",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.0545,
            "range": "± 1.76486",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 11.8218,
            "range": "± 1.09712",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.44667,
            "range": "± 926.948",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.465,
            "range": "± 591.802",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.8209,
            "range": "± 2.05085",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.08751,
            "range": "± 975.148",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.48671,
            "range": "± 840.278",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.36908,
            "range": "± 410",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.2025,
            "range": "± 1.30529",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 9.44364,
            "range": "± 1.18662",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.20977,
            "range": "± 701.217",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.14302,
            "range": "± 705.936",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.56472,
            "range": "± 707.034",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.8311,
            "range": "± 698.883",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.64847,
            "range": "± 605.911",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.75943,
            "range": "± 311.686",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.8854,
            "range": "± 10.9115",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 37.3531,
            "range": "± 5.18513",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.33397,
            "range": "± 38.0818",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.34967,
            "range": "± 30.5333",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 96.8947,
            "range": "± 8.50794",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 46.5412,
            "range": "± 4.02544",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.41029,
            "range": "± 30.9476",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.1957,
            "range": "± 46.0778",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 46.8628,
            "range": "± 8.01142",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.1977,
            "range": "± 46.6998",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 24.2926,
            "range": "± 487.043",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 54.7482,
            "range": "± 7.19672",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 76.8317,
            "range": "± 11.6772",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.18857,
            "range": "± 128.107",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.1951,
            "range": "± 421.146",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 74.6329,
            "range": "± 8.47303",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 727.88,
            "range": "± 43.4906",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.55175,
            "range": "± 165.429",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.3041,
            "range": "± 369.714",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.56329,
            "range": "± 74.723",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.81943,
            "range": "± 265.652",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.75574,
            "range": "± 147.031",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.01545,
            "range": "± 334.003",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.44899,
            "range": "± 104.141",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.68009,
            "range": "± 96.785",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.5104,
            "range": "± 232.778",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.87415,
            "range": "± 613.7",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 90.7636,
            "range": "± 5.93156",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.9416,
            "range": "± 1.80149",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 168.779,
            "range": "± 8.18724",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.2733,
            "range": "± 1.33059",
            "unit": "ns",
            "extra": "100 samples\n8442 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.0577,
            "range": "± 947.972",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 517.329,
            "range": "± 14.4095",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.82034,
            "range": "± 0.981281",
            "unit": "ns",
            "extra": "100 samples\n6563 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.4909,
            "range": "± 6.56309",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9955,
            "range": "± 99.6003",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 45.2538,
            "range": "± 7.37928",
            "unit": "ns",
            "extra": "100 samples\n682 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 841.501,
            "range": "± 14.0702",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 178.992,
            "range": "± 584.437",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.46621,
            "range": "± 0.287861",
            "unit": "ns",
            "extra": "100 samples\n8454 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 20.6623,
            "range": "± 1.71176",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.84254,
            "range": "± 13.6746",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 44.0388,
            "range": "± 4.59466",
            "unit": "ns",
            "extra": "100 samples\n680 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 43.8532,
            "range": "± 4.51833",
            "unit": "ns",
            "extra": "100 samples\n682 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 844.052,
            "range": "± 15.3452",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 843.118,
            "range": "± 14.3439",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 179.012,
            "range": "± 301.445",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 178.947,
            "range": "± 298.671",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 199.178,
            "range": "± 15.5178",
            "unit": "ns",
            "extra": "100 samples\n152 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 478.177,
            "range": "± 33.3409",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.21775,
            "range": "± 171.163",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 507.088,
            "range": "± 32.9499",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 946.462,
            "range": "± 74.6908",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 521.143,
            "range": "± 44.1402",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 631.864,
            "range": "± 55.7045",
            "unit": "ns",
            "extra": "100 samples\n52 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 506.542,
            "range": "± 36.0582",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 518.477,
            "range": "± 12.881",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 460.667,
            "range": "± 27.7852",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 37.299,
            "range": "± 6.40189",
            "unit": "ns",
            "extra": "100 samples\n844 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 276.65,
            "range": "± 6.54964",
            "unit": "ns",
            "extra": "100 samples\n8271 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.94038,
            "range": "± 0.513396",
            "unit": "ns",
            "extra": "100 samples\n7714 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.3386,
            "range": "± 1.93248",
            "unit": "ns",
            "extra": "100 samples\n757 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.40013,
            "range": "± 122.264",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 5.96441,
            "range": "± 107.596",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 4.93748,
            "range": "± 117.794",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 6.92046,
            "range": "± 130.579",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 27.8148,
            "range": "± 433.533",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.6022,
            "range": "± 635.829",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 26.7999,
            "range": "± 147.177",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.91213,
            "range": "± 148.387",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.1285,
            "range": "± 81.9816",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.16004,
            "range": "± 86.1717",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.2128,
            "range": "± 98.399",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 24.0339,
            "range": "± 302.071",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.2945,
            "range": "± 127.157",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 19.6223,
            "range": "± 134.326",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.6545,
            "range": "± 481.731",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.1852,
            "range": "± 374.051",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.0505,
            "range": "± 314.349",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.3145,
            "range": "± 377.774",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.1374,
            "range": "± 318.435",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.5951,
            "range": "± 292.818",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.615,
            "range": "± 302.896",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.8811,
            "range": "± 305.443",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.353,
            "range": "± 422.45",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.3132,
            "range": "± 431.055",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.3236,
            "range": "± 469.705",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.6278,
            "range": "± 667.077",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.5583,
            "range": "± 485.619",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.0721,
            "range": "± 451.987",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.3337,
            "range": "± 2.01825",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 17.5959,
            "range": "± 2.13998",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 21.4367,
            "range": "± 2.9043",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 17.8336,
            "range": "± 3.09282",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.5696,
            "range": "± 3.75195",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.2932,
            "range": "± 2.04367",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.2575,
            "range": "± 2.92659",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.6001,
            "range": "± 4.49766",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.1862,
            "range": "± 2.24005",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 17.4518,
            "range": "± 2.62535",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 33.3669,
            "range": "± 3.88272",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 188.495,
            "range": "± 9.87401",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 48.2883,
            "range": "± 7.9286",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 298.514,
            "range": "± 17.8381",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 18.9904,
            "range": "± 2.61791",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 30.0614,
            "range": "± 3.12105",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 52.4358,
            "range": "± 5.54008",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 32.4276,
            "range": "± 4.22956",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 52.9854,
            "range": "± 6.99703",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 41.9688,
            "range": "± 7.38545",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 44.844,
            "range": "± 5.89949",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 25.6842,
            "range": "± 3.68592",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 34.9043,
            "range": "± 4.73136",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 32.8503,
            "range": "± 5.33044",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 99.3643,
            "range": "± 11.9739",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1011.6899999999999,
            "range": "± 15524.800000000001",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 151.116,
            "range": "± 14.1025",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.12703,
            "range": "± 34.89",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 54.0015,
            "range": "± 7.66429",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.39858,
            "range": "± 264.626",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "651cdefad5a0479d10977f3d0ed4033d1e51c88c",
          "message": "Fix VS Code extension: bundle debugAdapter.ts with esbuild to include @vscode/debugadapter\n\nThe extension was failing to activate because debugAdapter.js required\n@vscode/debugadapter at runtime, but the package was moved from dependencies\nto devDependencies without adding a separate esbuild bundle step for the\ndebug adapter process. The bundle script now builds both entry points.\n\nCo-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>",
          "timestamp": "2026-04-26T17:49:34+02:00",
          "tree_id": "0ab3c65d4b4ba13360395f82a7ca3fd58f312642",
          "url": "https://github.com/nebulastream/nautilus/commit/651cdefad5a0479d10977f3d0ed4033d1e51c88c"
        },
        "date": 1777218934573,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 10.5617,
            "range": "± 0.791705",
            "unit": "ns",
            "extra": "100 samples\n8801 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.8603,
            "range": "± 1.622",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 561.956,
            "range": "± 62.0463",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.23499,
            "range": "± 1.38814",
            "unit": "ns",
            "extra": "100 samples\n7203 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 108.142,
            "range": "± 6.44045",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.5716,
            "range": "± 68.2459",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.7707,
            "range": "± 5.59085",
            "unit": "ns",
            "extra": "100 samples\n496 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 682.614,
            "range": "± 37.6757",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 158.674,
            "range": "± 8.42591",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.54284,
            "range": "± 0.268783",
            "unit": "ns",
            "extra": "100 samples\n8810 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.2787,
            "range": "± 1.86563",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.29175,
            "range": "± 32.8696",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 43.2892,
            "range": "± 4.70843",
            "unit": "ns",
            "extra": "100 samples\n714 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 43.6503,
            "range": "± 4.75153",
            "unit": "ns",
            "extra": "100 samples\n722 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 680.134,
            "range": "± 15.8751",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 676.818,
            "range": "± 9.25927",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 157.843,
            "range": "± 1.32747",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 157.556,
            "range": "± 971.527",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 179.278,
            "range": "± 22.6692",
            "unit": "ns",
            "extra": "100 samples\n175 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 433.297,
            "range": "± 24.811",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.10603,
            "range": "± 70.404",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 452.156,
            "range": "± 26.079",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 877.52,
            "range": "± 52.8667",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 566.972,
            "range": "± 183.16",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 565.551,
            "range": "± 30.9565",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 465.894,
            "range": "± 24.1451",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 471.844,
            "range": "± 28.8638",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 422.42,
            "range": "± 35.3335",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.28478,
            "range": "± 193.041",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.27526,
            "range": "± 190.284",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.31724,
            "range": "± 1.60194",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.52264,
            "range": "± 492.402",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 24.3612,
            "range": "± 3.21405",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.5478,
            "range": "± 1.60955",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.14236,
            "range": "± 1.33603",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.59446,
            "range": "± 535.705",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.1145,
            "range": "± 3.28102",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.40583,
            "range": "± 1.1458",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.29543,
            "range": "± 1.4451",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.69248,
            "range": "± 545.469",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.6368,
            "range": "± 2.42292",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.0429,
            "range": "± 1.27491",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.37292,
            "range": "± 835.362",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.4982,
            "range": "± 707.028",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.67363,
            "range": "± 1.42768",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.0313,
            "range": "± 848.962",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.46414,
            "range": "± 1.21754",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.82658,
            "range": "± 389.254",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.4912,
            "range": "± 6.58616",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.0937,
            "range": "± 6.81027",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.45893,
            "range": "± 29.5134",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.46792,
            "range": "± 32.2068",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 95.2784,
            "range": "± 12.1069",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 49.3746,
            "range": "± 8.69424",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.55689,
            "range": "± 126.844",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.29734,
            "range": "± 44.059",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 749.023,
            "range": "± 43.0076",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.57978,
            "range": "± 90.0082",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.33528,
            "range": "± 187.51",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.66135,
            "range": "± 94.2898",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.86001,
            "range": "± 255.321",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.82339,
            "range": "± 119.169",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.98234,
            "range": "± 85.3355",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.45197,
            "range": "± 120.595",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.85269,
            "range": "± 285.212",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.53705,
            "range": "± 143.313",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.68701,
            "range": "± 362.946",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 88.7044,
            "range": "± 2.87152",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.4945,
            "range": "± 678.651",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 162.755,
            "range": "± 6.85308",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.26142,
            "range": "± 233.37",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.9729,
            "range": "± 252.57",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.64642,
            "range": "± 163.826",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.863,
            "range": "± 219.082",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.4365,
            "range": "± 406.056",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.5784,
            "range": "± 270.747",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.3952,
            "range": "± 375.874",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.64765,
            "range": "± 237.329",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.191,
            "range": "± 218.219",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.0184,
            "range": "± 229.324",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.1573,
            "range": "± 288.03",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.4429,
            "range": "± 995.489",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.3788,
            "range": "± 241.518",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.1343,
            "range": "± 215.358",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.0005,
            "range": "± 360.468",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.7378,
            "range": "± 602.228",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.6723,
            "range": "± 399.632",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.6405,
            "range": "± 278.159",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.8125,
            "range": "± 429.883",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.162,
            "range": "± 790.658",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.172,
            "range": "± 443.51",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.4717,
            "range": "± 419.959",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.8591,
            "range": "± 588.384",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.9724,
            "range": "± 581.292",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.1285,
            "range": "± 652.509",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.7417,
            "range": "± 1.04487",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.4974,
            "range": "± 589.493",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.6642,
            "range": "± 881.22",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.4016,
            "range": "± 2.72981",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.5574,
            "range": "± 3.83995",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.0583,
            "range": "± 3.7402",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.4561,
            "range": "± 3.20537",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 18.9611,
            "range": "± 3.92456",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 16.8137,
            "range": "± 3.67381",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 17.619,
            "range": "± 3.74376",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.6925,
            "range": "± 3.58324",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 16.8324,
            "range": "± 3.73012",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 15.8644,
            "range": "± 2.49605",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 29.9517,
            "range": "± 4.22252",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 188.376,
            "range": "± 12.2432",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 41.7394,
            "range": "± 7.9142",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 303.676,
            "range": "± 14.8927",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 17.5067,
            "range": "± 4.87572",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 26.5766,
            "range": "± 5.42466",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 47.9459,
            "range": "± 12.2851",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 28.3678,
            "range": "± 5.53453",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 48.0974,
            "range": "± 11.1934",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 32.0453,
            "range": "± 6.63753",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 34.2252,
            "range": "± 5.95554",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 23.6582,
            "range": "± 5.4189",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 30.9108,
            "range": "± 5.75564",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 28.1275,
            "range": "± 6.47573",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 94.5524,
            "range": "± 13.1086",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1036.6200000000001,
            "range": "± 19366.7",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 146.984,
            "range": "± 18.2413",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.21645,
            "range": "± 22.6791",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 44.5672,
            "range": "± 15.147",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.59025,
            "range": "± 119.063",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.8,
            "range": "± 423.901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 46.812,
            "range": "± 16.4375",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 61.4381,
            "range": "± 17.2025",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.68219,
            "range": "± 158.655",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 28.0078,
            "range": "± 559.442",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 62.4231,
            "range": "± 17.2653",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 43.7666,
            "range": "± 15.238",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.11678,
            "range": "± 153.612",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 35.918,
            "range": "± 3.59784",
            "unit": "ns",
            "extra": "100 samples\n856 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 301.366,
            "range": "± 7.05427",
            "unit": "ns",
            "extra": "100 samples\n8593 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.09714,
            "range": "± 0.329101",
            "unit": "ns",
            "extra": "100 samples\n7910 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 42.2788,
            "range": "± 2.12339",
            "unit": "ns",
            "extra": "100 samples\n735 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "fbd000841f03892c0455d8d530d1d8ca6260ff52",
          "message": "Add DWARF debug info for MLIR-backend JIT'd code (#265)",
          "timestamp": "2026-04-26T21:30:54+02:00",
          "tree_id": "27dda9e19429f89afcb2c43942576f0dc599c10b",
          "url": "https://github.com/nebulastream/nautilus/commit/fbd000841f03892c0455d8d530d1d8ca6260ff52"
        },
        "date": 1777232202184,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 6.377,
            "range": "± 172.925",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 7.01691,
            "range": "± 248.474",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.78417,
            "range": "± 190.535",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.35393,
            "range": "± 324.38",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.2604,
            "range": "± 582.671",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.7503,
            "range": "± 263.994",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.5681,
            "range": "± 948.096",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.64512,
            "range": "± 145.239",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.307,
            "range": "± 187.951",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.1921,
            "range": "± 258.79",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.3222,
            "range": "± 231.663",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.7072,
            "range": "± 515.304",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.7337,
            "range": "± 406.502",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.7938,
            "range": "± 565.933",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 28.7531,
            "range": "± 408.965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.4886,
            "range": "± 1.18424",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.5467,
            "range": "± 432.148",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 29.1782,
            "range": "± 4.03728",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.672,
            "range": "± 404.492",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 29.0444,
            "range": "± 513.671",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.8736,
            "range": "± 550.805",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.3266,
            "range": "± 504.336",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.83,
            "range": "± 600.848",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.6621,
            "range": "± 571.361",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.517,
            "range": "± 1.02842",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 65.1716,
            "range": "± 1.53225",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.5113,
            "range": "± 452.851",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.182,
            "range": "± 436.859",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.3438,
            "range": "± 2.26851",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.1772,
            "range": "± 2.83898",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.3351,
            "range": "± 4.1405",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.6135,
            "range": "± 3.54299",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 18.7632,
            "range": "± 3.64657",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 16.8706,
            "range": "± 3.56488",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 17.8911,
            "range": "± 3.89168",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.4539,
            "range": "± 3.34504",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 17.0151,
            "range": "± 4.24733",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 16.3401,
            "range": "± 3.44193",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 31.9,
            "range": "± 5.1301",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 200.356,
            "range": "± 10.6207",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 44.7201,
            "range": "± 10.3866",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 315.43,
            "range": "± 12.9589",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 17.3357,
            "range": "± 5.7874",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 27.4672,
            "range": "± 7.46295",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 48.3402,
            "range": "± 10.4588",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 29.0349,
            "range": "± 5.82163",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 49.227,
            "range": "± 11.1378",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 32.377,
            "range": "± 6.76528",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 35.1061,
            "range": "± 5.90692",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 24.0122,
            "range": "± 5.12412",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 31.5818,
            "range": "± 5.64908",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 29.06,
            "range": "± 6.34809",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 96.5661,
            "range": "± 12.9167",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1070.25,
            "range": "± 74274.6",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 151.378,
            "range": "± 17.7476",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.27398,
            "range": "± 236.764",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.7525,
            "range": "± 0.679524",
            "unit": "ns",
            "extra": "100 samples\n8803 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 16.9187,
            "range": "± 1.67676",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 560.379,
            "range": "± 61.6932",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.45106,
            "range": "± 0.302066",
            "unit": "ns",
            "extra": "100 samples\n7140 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 109.859,
            "range": "± 8.67151",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.6387,
            "range": "± 305.914",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.1067,
            "range": "± 7.20227",
            "unit": "ns",
            "extra": "100 samples\n724 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 625.821,
            "range": "± 11.0899",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 141.515,
            "range": "± 1.57658",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.57412,
            "range": "± 0.404158",
            "unit": "ns",
            "extra": "100 samples\n8817 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.2244,
            "range": "± 1.57474",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.29957,
            "range": "± 34.5193",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 44.5689,
            "range": "± 5.76884",
            "unit": "ns",
            "extra": "100 samples\n714 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 44.7931,
            "range": "± 6.39022",
            "unit": "ns",
            "extra": "100 samples\n717 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 635.532,
            "range": "± 22.5573",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 624.517,
            "range": "± 11.8616",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 142.153,
            "range": "± 2.08175",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 142.506,
            "range": "± 4.38093",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 37.2303,
            "range": "± 3.51708",
            "unit": "ns",
            "extra": "100 samples\n857 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 271.771,
            "range": "± 6.65227",
            "unit": "ns",
            "extra": "100 samples\n9539 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.75101,
            "range": "± 0.429435",
            "unit": "ns",
            "extra": "100 samples\n8670 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.531,
            "range": "± 1.84049",
            "unit": "ns",
            "extra": "100 samples\n808 iterations"
          },
          {
            "name": "ssa_add",
            "value": 179.069,
            "range": "± 9.40586",
            "unit": "ns",
            "extra": "100 samples\n173 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 435.753,
            "range": "± 26.2967",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.11537,
            "range": "± 91.3448",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 452.529,
            "range": "± 24.0127",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 843.793,
            "range": "± 40.7455",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 462.963,
            "range": "± 24.3206",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 560.052,
            "range": "± 40.0071",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 430.417,
            "range": "± 18.362",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 481.922,
            "range": "± 35.7511",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 420.96,
            "range": "± 26.8031",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 46.9348,
            "range": "± 22.4931",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.58179,
            "range": "± 119.249",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 27.3309,
            "range": "± 434.848",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 45.9956,
            "range": "± 14.7115",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 63.9913,
            "range": "± 18.5359",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.65544,
            "range": "± 147.111",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 28.1095,
            "range": "± 448.351",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 64.9117,
            "range": "± 17.2028",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 45.8324,
            "range": "± 15.2855",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.1304,
            "range": "± 147.647",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.29783,
            "range": "± 194.868",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.28293,
            "range": "± 195.657",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.39651,
            "range": "± 1.51334",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.536,
            "range": "± 579.837",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 24.7254,
            "range": "± 3.66263",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.5865,
            "range": "± 1.72963",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.36665,
            "range": "± 1.57258",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.57594,
            "range": "± 541.226",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.1426,
            "range": "± 3.65332",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.41512,
            "range": "± 1.23609",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.27417,
            "range": "± 1.47989",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.68052,
            "range": "± 712.241",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.824,
            "range": "± 2.78924",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.199,
            "range": "± 1.51772",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.46664,
            "range": "± 792.799",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.4373,
            "range": "± 743.801",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.62993,
            "range": "± 1.75064",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.98329,
            "range": "± 775.486",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.56161,
            "range": "± 1.08357",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.86545,
            "range": "± 481.362",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 39.6688,
            "range": "± 7.93865",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.4348,
            "range": "± 7.44999",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.47933,
            "range": "± 29.4589",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.49096,
            "range": "± 30.1932",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 96.2765,
            "range": "± 10.9483",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.8909,
            "range": "± 9.56979",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.45426,
            "range": "± 60.0025",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.29528,
            "range": "± 43.0008",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 714.188,
            "range": "± 61.9818",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.56722,
            "range": "± 100.633",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.32578,
            "range": "± 194.785",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.58636,
            "range": "± 91.8031",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.8972,
            "range": "± 459.129",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.74208,
            "range": "± 107.84",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.90251,
            "range": "± 88.3959",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.4572,
            "range": "± 92.1571",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.71397,
            "range": "± 107.39",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.42298,
            "range": "± 126.153",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.81615,
            "range": "± 432.449",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 93.3493,
            "range": "± 3.75176",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.0695,
            "range": "± 774.013",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 173.153,
            "range": "± 6.55219",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          }
        ]
      }
    ]
  }
}