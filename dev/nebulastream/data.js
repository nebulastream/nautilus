window.BENCHMARK_DATA = {
  "lastUpdate": 1791888801424,
  "repoUrl": "https://github.com/nebulastream/nautilus",
  "entries": {
    "NebulaStream Systests": [
      {
        "commit": {
          "author": {
            "name": "Philipp Grulich",
            "username": "PhilippGrulich",
            "email": "philippgrulich@hotmail.de"
          },
          "committer": {
            "name": "GitHub",
            "username": "web-flow",
            "email": "noreply@github.com"
          },
          "id": "bf1d5febed3639fbbd19c879fc1e94e85c21c662",
          "message": "NebulaStream job: record benchmarks on manual runs; run nightly when NebulaStream main changed too (#555)",
          "timestamp": "2026-10-13T09:51:24Z",
          "url": "https://github.com/nebulastream/nautilus/commit/bf1d5febed3639fbbd19c879fc1e94e85c21c662"
        },
        "date": 1791888799636,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "mlir query nexmark scan: compilation",
            "value": 107.969,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:1, 1 pipelines, 5 rounds\npasses tracing=4.11 ssaCreation=1.4 irGeneration=1.17 irPasses.noThrowInference=0.0111 irPasses.exceptionRegionPreparation=0.106 mlir.loweringFromIR=7.44 mlir.pipeline=16.5 llvm.optimizerBuild=0.00548 jit.compile=75 backend=0.894 other=1.18"
          },
          {
            "name": "mlir query nexmark scan: execution",
            "value": 3.359,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:1, 1 pipelines, 5 rounds"
          },
          {
            "name": "mlir query nexmark q1 map: compilation",
            "value": 69.982,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:2, 2 pipelines, 5 rounds\npasses tracing=1.42 ssaCreation=0.395 irGeneration=0.405 irPasses.noThrowInference=0.00521 irPasses.exceptionRegionPreparation=0.0487 mlir.loweringFromIR=2.81 mlir.pipeline=9.17 llvm.optimizerBuild=0.01 jit.compile=53.1 backend=1.73 other=0.94"
          },
          {
            "name": "mlir query nexmark q1 map: execution",
            "value": 2.59,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:2, 2 pipelines, 5 rounds"
          },
          {
            "name": "mlir query nexmark q2 filter: compilation",
            "value": 113.292,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:3, 1 pipelines, 5 rounds\npasses tracing=5.2 ssaCreation=1.69 irGeneration=1.25 irPasses.noThrowInference=0.0131 irPasses.exceptionRegionPreparation=0.113 mlir.loweringFromIR=8.15 mlir.pipeline=18.1 llvm.optimizerBuild=0.00548 jit.compile=76.7 backend=0.873 other=1.19"
          },
          {
            "name": "mlir query nexmark q2 filter: execution",
            "value": 1.787,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:3, 1 pipelines, 5 rounds"
          },
          {
            "name": "mlir query nexmark q5 sliding aggregation join: compilation",
            "value": 901,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:4, 12 pipelines, 5 rounds\npasses tracing=25.9 ssaCreation=8.65 irGeneration=7.23 irPasses.noThrowInference=0.0762 irPasses.exceptionRegionPreparation=0.882 mlir.loweringFromIR=45.5 mlir.pipeline=146 llvm.optimizerBuild=0.0694 jit.compile=647 backend=8.6 other=10.6"
          },
          {
            "name": "mlir query nexmark q5 sliding aggregation join: execution",
            "value": 12.991,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:4, 12 pipelines, 5 rounds"
          },
          {
            "name": "mlir query nexmark q8 tumbling join: compilation",
            "value": 2317.03,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:5, 3 pipelines, 5 rounds\npasses tracing=107 ssaCreation=55.4 irGeneration=36.2 irPasses.noThrowInference=0.248 irPasses.exceptionRegionPreparation=4.55 mlir.loweringFromIR=236 mlir.pipeline=680 llvm.optimizerBuild=0.0255 jit.compile=1.15e+03 backend=3.59 other=47.6"
          },
          {
            "name": "mlir query nexmark q8 tumbling join: execution",
            "value": 13.671,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:5, 3 pipelines, 5 rounds"
          },
          {
            "name": "mlir query nexmark q8 tumbling join varsized: compilation",
            "value": 1970.777,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark_with_varsized:6, 3 pipelines, 5 rounds\npasses tracing=92.3 ssaCreation=47.7 irGeneration=30.5 irPasses.noThrowInference=0.222 irPasses.exceptionRegionPreparation=4.1 mlir.loweringFromIR=188 mlir.pipeline=543 llvm.optimizerBuild=0.0242 jit.compile=1.02e+03 backend=3.04 other=34.7"
          },
          {
            "name": "mlir query nexmark q8 tumbling join varsized: execution",
            "value": 5.671,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark_with_varsized:6, 3 pipelines, 5 rounds"
          },
          {
            "name": "mlir query ysb filter group sum: compilation",
            "value": 184.466,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark:1, 2 pipelines, 5 rounds\npasses tracing=10.2 ssaCreation=1.76 irGeneration=1.5 irPasses.noThrowInference=0.0136 irPasses.exceptionRegionPreparation=0.0866 mlir.loweringFromIR=8.39 mlir.pipeline=23.4 llvm.optimizerBuild=0.0112 jit.compile=136 backend=1.67 other=1.72"
          },
          {
            "name": "mlir query ysb filter group sum: execution",
            "value": 1.704,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "mlir query ysb varsized filter group count: compilation",
            "value": 189.967,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark_with_varsized:1, 2 pipelines, 5 rounds\npasses tracing=10.8 ssaCreation=2.27 irGeneration=1.75 irPasses.noThrowInference=0.0138 irPasses.exceptionRegionPreparation=0.0927 mlir.loweringFromIR=9.13 mlir.pipeline=25.7 llvm.optimizerBuild=0.0117 jit.compile=137 backend=1.69 other=1.86"
          },
          {
            "name": "mlir query ysb varsized filter group count: execution",
            "value": 1.177,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark_with_varsized:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "mlir query lrb sliding group avg: compilation",
            "value": 196.247,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:1, 3 pipelines, 5 rounds\npasses tracing=7.06 ssaCreation=2.63 irGeneration=1.81 irPasses.noThrowInference=0.0185 irPasses.exceptionRegionPreparation=0.166 mlir.loweringFromIR=11.6 mlir.pipeline=30.7 llvm.optimizerBuild=0.0168 jit.compile=137 backend=2.18 other=2.29"
          },
          {
            "name": "mlir query lrb sliding group avg: execution",
            "value": 62.665,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "mlir query lrb sliding group count: compilation",
            "value": 186.915,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:2, 3 pipelines, 5 rounds\npasses tracing=6.05 ssaCreation=1.81 irGeneration=1.46 irPasses.noThrowInference=0.015 irPasses.exceptionRegionPreparation=0.121 mlir.loweringFromIR=8.77 mlir.pipeline=24.7 llvm.optimizerBuild=0.0155 jit.compile=140 backend=2.22 other=1.85"
          },
          {
            "name": "mlir query lrb sliding group count: execution",
            "value": 39.434,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:2, 3 pipelines, 5 rounds"
          },
          {
            "name": "mlir query cluster monitoring sliding group sum: compilation",
            "value": 188.06,
            "unit": "ms",
            "extra": "benchmark_small/ClusterMonitoring:1, 2 pipelines, 5 rounds\npasses tracing=11 ssaCreation=1.76 irGeneration=1.48 irPasses.noThrowInference=0.0132 irPasses.exceptionRegionPreparation=0.0915 mlir.loweringFromIR=8.54 mlir.pipeline=23.7 llvm.optimizerBuild=0.0102 jit.compile=138 backend=1.67 other=1.77"
          },
          {
            "name": "mlir query cluster monitoring sliding group sum: execution",
            "value": 4.302,
            "unit": "ms",
            "extra": "benchmark_small/ClusterMonitoring:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "mlir query manufacturing sliding multi aggregate: compilation",
            "value": 270.733,
            "unit": "ms",
            "extra": "benchmark_small/Manufacturing:1, 2 pipelines, 5 rounds\npasses tracing=14.2 ssaCreation=3.22 irGeneration=2.37 irPasses.noThrowInference=0.0254 irPasses.exceptionRegionPreparation=0.154 mlir.loweringFromIR=14.9 mlir.pipeline=39.1 llvm.optimizerBuild=0.0123 jit.compile=192 backend=1.57 other=2.65"
          },
          {
            "name": "mlir query manufacturing sliding multi aggregate: execution",
            "value": 3.014,
            "unit": "ms",
            "extra": "benchmark_small/Manufacturing:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "mlir query debs range filter: compilation",
            "value": 113.899,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:3, 1 pipelines, 5 rounds\npasses tracing=5.07 ssaCreation=1.71 irGeneration=1.26 irPasses.noThrowInference=0.00988 irPasses.exceptionRegionPreparation=0.0986 mlir.loweringFromIR=7.81 mlir.pipeline=18 llvm.optimizerBuild=0.00572 jit.compile=77.7 backend=0.892 other=1.2"
          },
          {
            "name": "mlir query debs range filter: execution",
            "value": 1.924,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:3, 1 pipelines, 5 rounds"
          },
          {
            "name": "mlir query debs union: compilation",
            "value": 150.839,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:8, 4 pipelines, 5 rounds\npasses tracing=4.09 ssaCreation=1.01 irGeneration=1.02 irPasses.noThrowInference=0.0127 irPasses.exceptionRegionPreparation=0.0865 mlir.loweringFromIR=6.52 mlir.pipeline=20 llvm.optimizerBuild=0.0207 jit.compile=113 backend=2.99 other=1.98"
          },
          {
            "name": "mlir query debs union: execution",
            "value": 6.04,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:8, 4 pipelines, 5 rounds"
          },
          {
            "name": "mlir query wide arithmetic expression: compilation",
            "value": 62.164,
            "unit": "ms",
            "extra": "operator/projection/Filter_Map_Queries:4, 1 pipelines, 5 rounds\npasses tracing=2.32 ssaCreation=0.598 irGeneration=0.548 irPasses.noThrowInference=0.00604 irPasses.exceptionRegionPreparation=0.0494 mlir.loweringFromIR=3.5 mlir.pipeline=8.04 jit.compile=45.3 backend=1.01 other=0.592"
          },
          {
            "name": "mlir query wide arithmetic expression: execution",
            "value": 0.569,
            "unit": "ms",
            "extra": "operator/projection/Filter_Map_Queries:4, 1 pipelines, 5 rounds"
          },
          {
            "name": "mlir query nested maps: compilation",
            "value": 92.059,
            "unit": "ms",
            "extra": "operator/map/Map_Add:5, 3 pipelines, 5 rounds\npasses tracing=1.82 ssaCreation=0.566 irGeneration=0.572 irPasses.noThrowInference=0.00706 irPasses.exceptionRegionPreparation=0.0778 mlir.loweringFromIR=3.77 mlir.pipeline=10.9 llvm.optimizerBuild=0.0151 jit.compile=70.5 backend=2.35 other=1.31"
          },
          {
            "name": "mlir query nested maps: execution",
            "value": 0.699,
            "unit": "ms",
            "extra": "operator/map/Map_Add:5, 3 pipelines, 5 rounds"
          },
          {
            "name": "mlir query round all integer widths: compilation",
            "value": 125.185,
            "unit": "ms",
            "extra": "function/arithmetical/FunctionRound:1, 1 pipelines, 5 rounds\npasses tracing=8.26 ssaCreation=1.53 irGeneration=1.34 irPasses.noThrowInference=0.0147 irPasses.exceptionRegionPreparation=0.123 mlir.loweringFromIR=8.65 mlir.pipeline=17.1 llvm.optimizerBuild=0.00544 jit.compile=86.2 backend=0.895 other=1.15"
          },
          {
            "name": "mlir query round all integer widths: execution",
            "value": 0.678,
            "unit": "ms",
            "extra": "function/arithmetical/FunctionRound:1, 1 pipelines, 5 rounds"
          },
          {
            "name": "mlir query string concat: compilation",
            "value": 85.564,
            "unit": "ms",
            "extra": "operator/selection/SelectionNull:7, 2 pipelines, 5 rounds\npasses tracing=1.89 ssaCreation=0.617 irGeneration=0.586 irPasses.noThrowInference=0.00715 irPasses.exceptionRegionPreparation=0.0765 mlir.loweringFromIR=3.9 mlir.pipeline=11.2 llvm.optimizerBuild=0.00996 jit.compile=64.4 backend=1.69 other=1.05"
          },
          {
            "name": "mlir query string concat: execution",
            "value": 0.626,
            "unit": "ms",
            "extra": "operator/selection/SelectionNull:7, 2 pipelines, 5 rounds"
          },
          {
            "name": "mlir query filter on string concat: compilation",
            "value": 77.083,
            "unit": "ms",
            "extra": "operator/selection/Selection_NotEquals:7, 1 pipelines, 5 rounds\npasses tracing=3.54 ssaCreation=1.1 irGeneration=0.932 irPasses.noThrowInference=0.00818 irPasses.exceptionRegionPreparation=0.053 mlir.loweringFromIR=5.6 mlir.pipeline=11 jit.compile=53 backend=0.935 other=0.758"
          },
          {
            "name": "mlir query filter on string concat: execution",
            "value": 0.534,
            "unit": "ms",
            "extra": "operator/selection/Selection_NotEquals:7, 1 pipelines, 5 rounds"
          },
          {
            "name": "mlir query varsized in list filter: compilation",
            "value": 71.273,
            "unit": "ms",
            "extra": "parser/VarSizedSupport:3, 1 pipelines, 5 rounds\npasses tracing=2.75 ssaCreation=0.921 irGeneration=0.795 irPasses.noThrowInference=0.00869 irPasses.exceptionRegionPreparation=0.0569 mlir.loweringFromIR=4.75 mlir.pipeline=9.6 llvm.optimizerBuild=0.00505 jit.compile=50.4 backend=0.965 other=0.676"
          },
          {
            "name": "mlir query varsized in list filter: execution",
            "value": 0.559,
            "unit": "ms",
            "extra": "parser/VarSizedSupport:3, 1 pipelines, 5 rounds"
          },
          {
            "name": "mlir query nested json scan: compilation",
            "value": 69.468,
            "unit": "ms",
            "extra": "formatter/JSON/NestedJSON:4, 1 pipelines, 5 rounds\npasses tracing=1.93 ssaCreation=0.701 irGeneration=0.649 irPasses.noThrowInference=0.00694 irPasses.exceptionRegionPreparation=0.0562 mlir.loweringFromIR=4.16 mlir.pipeline=8.7 jit.compile=51.7 backend=1.03 other=0.628"
          },
          {
            "name": "mlir query nested json scan: execution",
            "value": 0.603,
            "unit": "ms",
            "extra": "formatter/JSON/NestedJSON:4, 1 pipelines, 5 rounds"
          },
          {
            "name": "mlir query widest schema scan: compilation",
            "value": 23177.85,
            "unit": "ms",
            "extra": "formatter/MaxNumberOfSupportedFields:1, 1 pipelines, 2 rounds\npasses tracing=1.65e+04 ssaCreation=387 irGeneration=196 irPasses.noThrowInference=0.547 irPasses.exceptionRegionPreparation=3.23 mlir.loweringFromIR=1.26e+03 mlir.pipeline=756 llvm.optimizerBuild=0.00955 jit.compile=3.81e+03 backend=6.38 other=292"
          },
          {
            "name": "mlir query widest schema scan: execution",
            "value": 1.109,
            "unit": "ms",
            "extra": "formatter/MaxNumberOfSupportedFields:1, 1 pipelines, 2 rounds"
          },
          {
            "name": "mlir query tumbling all aggregates: compilation",
            "value": 281.774,
            "unit": "ms",
            "extra": "operator/aggregation/ColumnNames:1, 2 pipelines, 5 rounds\npasses tracing=9.66 ssaCreation=3.68 irGeneration=2.56 irPasses.noThrowInference=0.0226 irPasses.exceptionRegionPreparation=0.182 mlir.loweringFromIR=15.5 mlir.pipeline=39.9 llvm.optimizerBuild=0.0115 jit.compile=205 backend=1.49 other=2.7"
          },
          {
            "name": "mlir query tumbling all aggregates: execution",
            "value": 0.967,
            "unit": "ms",
            "extra": "operator/aggregation/ColumnNames:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "mlir query tumbling median of expression: compilation",
            "value": 217.268,
            "unit": "ms",
            "extra": "operator/aggregation/ExpressionInsideAggregation:3, 3 pipelines, 5 rounds\npasses tracing=4.49 ssaCreation=2.71 irGeneration=1.77 irPasses.noThrowInference=0.0171 irPasses.exceptionRegionPreparation=0.156 mlir.loweringFromIR=11.6 mlir.pipeline=30.3 llvm.optimizerBuild=0.0166 jit.compile=161 backend=2.12 other=2.3"
          },
          {
            "name": "mlir query tumbling median of expression: execution",
            "value": 1.079,
            "unit": "ms",
            "extra": "operator/aggregation/ExpressionInsideAggregation:3, 3 pipelines, 5 rounds"
          },
          {
            "name": "mlir query sliding group count: compilation",
            "value": 158.833,
            "unit": "ms",
            "extra": "operator/aggregation/WindowAggregationCount:6, 2 pipelines, 5 rounds\npasses tracing=6.13 ssaCreation=1.58 irGeneration=1.32 irPasses.noThrowInference=0.0118 irPasses.exceptionRegionPreparation=0.0642 mlir.loweringFromIR=7.21 mlir.pipeline=18.9 llvm.optimizerBuild=0.01 jit.compile=120 backend=1.71 other=1.38"
          },
          {
            "name": "mlir query sliding group count: execution",
            "value": 0.849,
            "unit": "ms",
            "extra": "operator/aggregation/WindowAggregationCount:6, 2 pipelines, 5 rounds"
          },
          {
            "name": "mlir query aggregation of union: compilation",
            "value": 183.003,
            "unit": "ms",
            "extra": "operator/aggregation/AggregationOfUnion:1, 5 pipelines, 5 rounds\npasses tracing=3.22 ssaCreation=1.15 irGeneration=1.07 irPasses.noThrowInference=0.0135 irPasses.exceptionRegionPreparation=0.114 mlir.loweringFromIR=6.51 mlir.pipeline=21.8 llvm.optimizerBuild=0.0258 jit.compile=143 backend=3.7 other=2.45"
          },
          {
            "name": "mlir query aggregation of union: execution",
            "value": 1.161,
            "unit": "ms",
            "extra": "operator/aggregation/AggregationOfUnion:1, 5 pipelines, 5 rounds"
          },
          {
            "name": "mlir query tumbling left outer join: compilation",
            "value": 1407.207,
            "unit": "ms",
            "extra": "operator/join/LeftOuterJoin:1, 3 pipelines, 5 rounds\npasses tracing=49.6 ssaCreation=23.4 irGeneration=16.1 irPasses.noThrowInference=0.13 irPasses.exceptionRegionPreparation=2.03 mlir.loweringFromIR=104 mlir.pipeline=281 llvm.optimizerBuild=0.0218 jit.compile=905 backend=2.76 other=15.8"
          },
          {
            "name": "mlir query tumbling left outer join: execution",
            "value": 26.034,
            "unit": "ms",
            "extra": "operator/join/LeftOuterJoin:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "mlir query tumbling full outer join: compilation",
            "value": 1406.586,
            "unit": "ms",
            "extra": "operator/join/FullOuterJoin:1, 3 pipelines, 5 rounds\npasses tracing=49.9 ssaCreation=23.5 irGeneration=16 irPasses.noThrowInference=0.139 irPasses.exceptionRegionPreparation=2.09 mlir.loweringFromIR=102 mlir.pipeline=284 llvm.optimizerBuild=0.0217 jit.compile=909 backend=2.79 other=16.5"
          },
          {
            "name": "mlir query tumbling full outer join: execution",
            "value": 29.267,
            "unit": "ms",
            "extra": "operator/join/FullOuterJoin:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "mlir query sliding right outer join: compilation",
            "value": 1406.615,
            "unit": "ms",
            "extra": "operator/join/SlidingWindowRightOuterJoin:1, 3 pipelines, 5 rounds\npasses tracing=49.8 ssaCreation=23.3 irGeneration=15.9 irPasses.noThrowInference=0.126 irPasses.exceptionRegionPreparation=2.01 mlir.loweringFromIR=103 mlir.pipeline=284 llvm.optimizerBuild=0.0214 jit.compile=910 backend=2.79 other=15.9"
          },
          {
            "name": "mlir query sliding right outer join: execution",
            "value": 1.553,
            "unit": "ms",
            "extra": "operator/join/SlidingWindowRightOuterJoin:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "mlir query sliding inner join: compilation",
            "value": 752.358,
            "unit": "ms",
            "extra": "guide/guide12:1, 4 pipelines, 5 rounds\npasses tracing=27.7 ssaCreation=7.99 irGeneration=6.5 irPasses.noThrowInference=0.0659 irPasses.exceptionRegionPreparation=0.944 mlir.loweringFromIR=43.8 mlir.pipeline=143 llvm.optimizerBuild=0.0256 jit.compile=510 backend=3.39 other=7.57"
          },
          {
            "name": "mlir query sliding inner join: execution",
            "value": 1.139,
            "unit": "ms",
            "extra": "guide/guide12:1, 4 pipelines, 5 rounds"
          },
          {
            "name": "mlir query nested join quoted identifiers: compilation",
            "value": 764.204,
            "unit": "ms",
            "extra": "datatype/QuotedIdentifiers:5, 5 pipelines, 5 rounds\npasses tracing=26.9 ssaCreation=8.26 irGeneration=6.63 irPasses.noThrowInference=0.0727 irPasses.exceptionRegionPreparation=0.904 mlir.loweringFromIR=43.8 mlir.pipeline=145 llvm.optimizerBuild=0.0309 jit.compile=521 backend=4.01 other=7.93"
          },
          {
            "name": "mlir query nested join quoted identifiers: execution",
            "value": 1.329,
            "unit": "ms",
            "extra": "datatype/QuotedIdentifiers:5, 5 pipelines, 5 rounds"
          },
          {
            "name": "mlir query medical cast filter: compilation",
            "value": 274.731,
            "unit": "ms",
            "extra": "medical/MedicalUseCase:1, 2 pipelines, 5 rounds\npasses tracing=16.9 ssaCreation=4.56 irGeneration=3.59 irPasses.noThrowInference=0.0236 irPasses.exceptionRegionPreparation=0.195 mlir.loweringFromIR=18 mlir.pipeline=44.7 llvm.optimizerBuild=0.0136 jit.compile=182 backend=1.57 other=3.23"
          },
          {
            "name": "mlir query medical cast filter: execution",
            "value": 0.972,
            "unit": "ms",
            "extra": "medical/MedicalUseCase:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query nexmark scan: compilation",
            "value": 12.812,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:1, 1 pipelines, 5 rounds\npasses tracing=4.19 ssaCreation=1.38 irGeneration=1.18 irPasses.noThrowInference=0.011 irPasses.exceptionRegionPreparation=0.106 asmjit.compile=5.7 backend=0.057 other=0.203"
          },
          {
            "name": "asmjit query nexmark scan: execution",
            "value": 3.64,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:1, 1 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query nexmark q1 map: compilation",
            "value": 5.113,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:2, 2 pipelines, 5 rounds\npasses tracing=1.44 ssaCreation=0.37 irGeneration=0.496 irPasses.noThrowInference=0.00614 irPasses.exceptionRegionPreparation=0.0554 asmjit.compile=2.51 backend=0.0547 other=0.193"
          },
          {
            "name": "asmjit query nexmark q1 map: execution",
            "value": 2.541,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:2, 2 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query nexmark q2 filter: compilation",
            "value": 14.725,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:3, 1 pipelines, 5 rounds\npasses tracing=5.14 ssaCreation=1.68 irGeneration=1.26 irPasses.noThrowInference=0.0143 irPasses.exceptionRegionPreparation=0.114 asmjit.compile=6.19 backend=0.058 other=0.228"
          },
          {
            "name": "asmjit query nexmark q2 filter: execution",
            "value": 1.801,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:3, 1 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query nexmark q5 sliding aggregation join: compilation",
            "value": 81.636,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:4, 12 pipelines, 5 rounds\npasses tracing=26.8 ssaCreation=9.33 irGeneration=7.67 irPasses.noThrowInference=0.0718 irPasses.exceptionRegionPreparation=1.03 asmjit.compile=34.9 backend=0.0721 other=1.75"
          },
          {
            "name": "asmjit query nexmark q5 sliding aggregation join: execution",
            "value": 14.044,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:4, 12 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query nexmark q8 tumbling join: compilation",
            "value": 486.241,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:5, 3 pipelines, 5 rounds\npasses tracing=107 ssaCreation=58.5 irGeneration=36.5 irPasses.noThrowInference=0.241 irPasses.exceptionRegionPreparation=4.59 asmjit.compile=272 backend=0.0692 other=5.68"
          },
          {
            "name": "asmjit query nexmark q8 tumbling join: execution",
            "value": 14.476,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:5, 3 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query nexmark q8 tumbling join varsized: compilation",
            "value": 399.342,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark_with_varsized:6, 3 pipelines, 5 rounds\npasses tracing=91.5 ssaCreation=48.8 irGeneration=30.6 irPasses.noThrowInference=0.216 irPasses.exceptionRegionPreparation=4.13 asmjit.compile=220 backend=0.0677 other=5.26"
          },
          {
            "name": "asmjit query nexmark q8 tumbling join varsized: execution",
            "value": 5.856,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark_with_varsized:6, 3 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query ysb filter group sum: compilation",
            "value": 20.831,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark:1, 2 pipelines, 5 rounds\npasses tracing=10.1 ssaCreation=1.87 irGeneration=1.66 irPasses.noThrowInference=0.0138 irPasses.exceptionRegionPreparation=0.0909 asmjit.compile=6.53 backend=0.0569 other=0.418"
          },
          {
            "name": "asmjit query ysb filter group sum: execution",
            "value": 1.744,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query ysb varsized filter group count: compilation",
            "value": 22.747,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark_with_varsized:1, 2 pipelines, 5 rounds\npasses tracing=10.8 ssaCreation=2.46 irGeneration=1.84 irPasses.noThrowInference=0.0199 irPasses.exceptionRegionPreparation=0.0958 asmjit.compile=6.91 backend=0.0557 other=0.425"
          },
          {
            "name": "asmjit query ysb varsized filter group count: execution",
            "value": 1.161,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark_with_varsized:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query lrb sliding group avg: compilation",
            "value": 20.588,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:1, 3 pipelines, 5 rounds\npasses tracing=6.88 ssaCreation=2.59 irGeneration=1.84 irPasses.noThrowInference=0.0178 irPasses.exceptionRegionPreparation=0.16 asmjit.compile=8.37 backend=0.0599 other=0.411"
          },
          {
            "name": "asmjit query lrb sliding group avg: execution",
            "value": 74.103,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query lrb sliding group count: compilation",
            "value": 16.301,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:2, 3 pipelines, 5 rounds\npasses tracing=6.03 ssaCreation=1.8 irGeneration=1.47 irPasses.noThrowInference=0.0154 irPasses.exceptionRegionPreparation=0.127 asmjit.compile=6.4 backend=0.0576 other=0.378"
          },
          {
            "name": "asmjit query lrb sliding group count: execution",
            "value": 47.667,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:2, 3 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query cluster monitoring sliding group sum: compilation",
            "value": 21.877,
            "unit": "ms",
            "extra": "benchmark_small/ClusterMonitoring:1, 2 pipelines, 5 rounds\npasses tracing=10.8 ssaCreation=1.89 irGeneration=1.65 irPasses.noThrowInference=0.0142 irPasses.exceptionRegionPreparation=0.0929 asmjit.compile=6.93 backend=0.0568 other=0.411"
          },
          {
            "name": "asmjit query cluster monitoring sliding group sum: execution",
            "value": 4.691,
            "unit": "ms",
            "extra": "benchmark_small/ClusterMonitoring:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query manufacturing sliding multi aggregate: compilation",
            "value": 33.505,
            "unit": "ms",
            "extra": "benchmark_small/Manufacturing:1, 2 pipelines, 5 rounds\npasses tracing=14.1 ssaCreation=3.36 irGeneration=2.51 irPasses.noThrowInference=0.0251 irPasses.exceptionRegionPreparation=0.16 asmjit.compile=12.5 backend=0.0552 other=0.628"
          },
          {
            "name": "asmjit query manufacturing sliding multi aggregate: execution",
            "value": 3.286,
            "unit": "ms",
            "extra": "benchmark_small/Manufacturing:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query debs range filter: compilation",
            "value": 14.634,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:3, 1 pipelines, 5 rounds\npasses tracing=5.12 ssaCreation=1.75 irGeneration=1.27 irPasses.noThrowInference=0.0111 irPasses.exceptionRegionPreparation=0.101 asmjit.compile=6.12 backend=0.056 other=0.208"
          },
          {
            "name": "asmjit query debs range filter: execution",
            "value": 1.933,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:3, 1 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query debs union: compilation",
            "value": 11.795,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:8, 4 pipelines, 5 rounds\npasses tracing=4.03 ssaCreation=1.03 irGeneration=1.02 irPasses.noThrowInference=0.0117 irPasses.exceptionRegionPreparation=0.0852 asmjit.compile=5.23 backend=0.059 other=0.382"
          },
          {
            "name": "asmjit query debs union: execution",
            "value": 6.445,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:8, 4 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query wide arithmetic expression: compilation",
            "value": 6.501,
            "unit": "ms",
            "extra": "operator/projection/Filter_Map_Queries:4, 1 pipelines, 5 rounds\npasses tracing=2.31 ssaCreation=0.602 irGeneration=0.529 irPasses.noThrowInference=0.00582 irPasses.exceptionRegionPreparation=0.0543 asmjit.compile=2.75 backend=0.0507 other=0.147"
          },
          {
            "name": "asmjit query wide arithmetic expression: execution",
            "value": 0.417,
            "unit": "ms",
            "extra": "operator/projection/Filter_Map_Queries:4, 1 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query nested maps: compilation",
            "value": 6.547,
            "unit": "ms",
            "extra": "operator/map/Map_Add:5, 3 pipelines, 5 rounds\npasses tracing=1.84 ssaCreation=0.559 irGeneration=0.613 irPasses.noThrowInference=0.00711 irPasses.exceptionRegionPreparation=0.0762 asmjit.compile=3.08 backend=0.0561 other=0.287"
          },
          {
            "name": "asmjit query nested maps: execution",
            "value": 0.419,
            "unit": "ms",
            "extra": "operator/map/Map_Add:5, 3 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query round all integer widths: compilation",
            "value": 18.336,
            "unit": "ms",
            "extra": "function/arithmetical/FunctionRound:1, 1 pipelines, 5 rounds\npasses tracing=8.21 ssaCreation=1.51 irGeneration=1.33 irPasses.noThrowInference=0.0148 irPasses.exceptionRegionPreparation=0.122 asmjit.compile=6.93 backend=0.0528 other=0.23"
          },
          {
            "name": "asmjit query round all integer widths: execution",
            "value": 0.546,
            "unit": "ms",
            "extra": "function/arithmetical/FunctionRound:1, 1 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query string concat: compilation",
            "value": 6.738,
            "unit": "ms",
            "extra": "operator/selection/SelectionNull:7, 2 pipelines, 5 rounds\npasses tracing=2.01 ssaCreation=0.625 irGeneration=0.58 irPasses.noThrowInference=0.00729 irPasses.exceptionRegionPreparation=0.0785 asmjit.compile=3.17 backend=0.0534 other=0.216"
          },
          {
            "name": "asmjit query string concat: execution",
            "value": 0.412,
            "unit": "ms",
            "extra": "operator/selection/SelectionNull:7, 2 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query filter on string concat: compilation",
            "value": 10.098,
            "unit": "ms",
            "extra": "operator/selection/Selection_NotEquals:7, 1 pipelines, 5 rounds\npasses tracing=3.57 ssaCreation=1.13 irGeneration=0.945 irPasses.noThrowInference=0.00908 irPasses.exceptionRegionPreparation=0.0495 asmjit.compile=4.15 backend=0.0499 other=0.185"
          },
          {
            "name": "asmjit query filter on string concat: execution",
            "value": 0.431,
            "unit": "ms",
            "extra": "operator/selection/Selection_NotEquals:7, 1 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query varsized in list filter: compilation",
            "value": 8.246,
            "unit": "ms",
            "extra": "parser/VarSizedSupport:3, 1 pipelines, 5 rounds\npasses tracing=2.74 ssaCreation=0.899 irGeneration=0.768 irPasses.noThrowInference=0.0078 irPasses.exceptionRegionPreparation=0.0559 asmjit.compile=3.54 backend=0.0566 other=0.184"
          },
          {
            "name": "asmjit query varsized in list filter: execution",
            "value": 0.367,
            "unit": "ms",
            "extra": "parser/VarSizedSupport:3, 1 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query nested json scan: compilation",
            "value": 6.829,
            "unit": "ms",
            "extra": "formatter/JSON/NestedJSON:4, 1 pipelines, 5 rounds\npasses tracing=1.9 ssaCreation=0.695 irGeneration=0.653 irPasses.noThrowInference=0.00682 irPasses.exceptionRegionPreparation=0.0598 asmjit.compile=3.34 backend=0.0531 other=0.15"
          },
          {
            "name": "asmjit query nested json scan: execution",
            "value": 0.464,
            "unit": "ms",
            "extra": "formatter/JSON/NestedJSON:4, 1 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query widest schema scan: compilation",
            "value": 19579.6,
            "unit": "ms",
            "extra": "formatter/MaxNumberOfSupportedFields:1, 1 pipelines, 2 rounds\npasses tracing=1.81e+04 ssaCreation=369 irGeneration=195 irPasses.noThrowInference=0.593 irPasses.exceptionRegionPreparation=3.2 asmjit.compile=906 backend=0.0654 other=10.6"
          },
          {
            "name": "asmjit query widest schema scan: execution",
            "value": 0.976,
            "unit": "ms",
            "extra": "formatter/MaxNumberOfSupportedFields:1, 1 pipelines, 2 rounds"
          },
          {
            "name": "asmjit query tumbling all aggregates: compilation",
            "value": 29.014,
            "unit": "ms",
            "extra": "operator/aggregation/ColumnNames:1, 2 pipelines, 5 rounds\npasses tracing=9.83 ssaCreation=3.7 irGeneration=2.56 irPasses.noThrowInference=0.0231 irPasses.exceptionRegionPreparation=0.187 asmjit.compile=11.8 backend=0.0589 other=0.608"
          },
          {
            "name": "asmjit query tumbling all aggregates: execution",
            "value": 0.796,
            "unit": "ms",
            "extra": "operator/aggregation/ColumnNames:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query tumbling median of expression: compilation",
            "value": 17.063,
            "unit": "ms",
            "extra": "operator/aggregation/ExpressionInsideAggregation:3, 3 pipelines, 5 rounds\npasses tracing=4.59 ssaCreation=2.73 irGeneration=1.75 irPasses.noThrowInference=0.0164 irPasses.exceptionRegionPreparation=0.16 asmjit.compile=7.17 backend=0.0611 other=0.418"
          },
          {
            "name": "asmjit query tumbling median of expression: execution",
            "value": 0.851,
            "unit": "ms",
            "extra": "operator/aggregation/ExpressionInsideAggregation:3, 3 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query sliding group count: compilation",
            "value": 15.045,
            "unit": "ms",
            "extra": "operator/aggregation/WindowAggregationCount:6, 2 pipelines, 5 rounds\npasses tracing=6.02 ssaCreation=1.71 irGeneration=1.42 irPasses.noThrowInference=0.0161 irPasses.exceptionRegionPreparation=0.0657 asmjit.compile=5.39 backend=0.0515 other=0.368"
          },
          {
            "name": "asmjit query sliding group count: execution",
            "value": 0.868,
            "unit": "ms",
            "extra": "operator/aggregation/WindowAggregationCount:6, 2 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query aggregation of union: compilation",
            "value": 11.637,
            "unit": "ms",
            "extra": "operator/aggregation/AggregationOfUnion:1, 5 pipelines, 5 rounds\npasses tracing=3.23 ssaCreation=1.11 irGeneration=1.13 irPasses.noThrowInference=0.0137 irPasses.exceptionRegionPreparation=0.121 asmjit.compile=5.51 backend=0.0639 other=0.462"
          },
          {
            "name": "asmjit query aggregation of union: execution",
            "value": 0.626,
            "unit": "ms",
            "extra": "operator/aggregation/AggregationOfUnion:1, 5 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query tumbling left outer join: compilation",
            "value": 212.835,
            "unit": "ms",
            "extra": "operator/join/LeftOuterJoin:1, 3 pipelines, 5 rounds\npasses tracing=49 ssaCreation=23.6 irGeneration=16.1 irPasses.noThrowInference=0.122 irPasses.exceptionRegionPreparation=2.05 asmjit.compile=118 backend=0.0624 other=2.78"
          },
          {
            "name": "asmjit query tumbling left outer join: execution",
            "value": 29.921,
            "unit": "ms",
            "extra": "operator/join/LeftOuterJoin:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query tumbling full outer join: compilation",
            "value": 209.55,
            "unit": "ms",
            "extra": "operator/join/FullOuterJoin:1, 3 pipelines, 5 rounds\npasses tracing=49 ssaCreation=22.9 irGeneration=16.1 irPasses.noThrowInference=0.131 irPasses.exceptionRegionPreparation=2.01 asmjit.compile=117 backend=0.0659 other=2.9"
          },
          {
            "name": "asmjit query tumbling full outer join: execution",
            "value": 33.08,
            "unit": "ms",
            "extra": "operator/join/FullOuterJoin:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query sliding right outer join: compilation",
            "value": 215.318,
            "unit": "ms",
            "extra": "operator/join/SlidingWindowRightOuterJoin:1, 3 pipelines, 5 rounds\npasses tracing=49.8 ssaCreation=24 irGeneration=16.1 irPasses.noThrowInference=0.143 irPasses.exceptionRegionPreparation=2.1 asmjit.compile=119 backend=0.0649 other=3.19"
          },
          {
            "name": "asmjit query sliding right outer join: execution",
            "value": 1.092,
            "unit": "ms",
            "extra": "operator/join/SlidingWindowRightOuterJoin:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query sliding inner join: compilation",
            "value": 83.985,
            "unit": "ms",
            "extra": "guide/guide12:1, 4 pipelines, 5 rounds\npasses tracing=27.4 ssaCreation=8.53 irGeneration=6.66 irPasses.noThrowInference=0.0651 irPasses.exceptionRegionPreparation=0.96 asmjit.compile=38.5 backend=0.0625 other=1.55"
          },
          {
            "name": "asmjit query sliding inner join: execution",
            "value": 0.873,
            "unit": "ms",
            "extra": "guide/guide12:1, 4 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query nested join quoted identifiers: compilation",
            "value": 83.058,
            "unit": "ms",
            "extra": "datatype/QuotedIdentifiers:5, 5 pipelines, 5 rounds\npasses tracing=27.2 ssaCreation=8.62 irGeneration=6.69 irPasses.noThrowInference=0.0622 irPasses.exceptionRegionPreparation=0.995 asmjit.compile=37.6 backend=0.0637 other=1.58"
          },
          {
            "name": "asmjit query nested join quoted identifiers: execution",
            "value": 1.014,
            "unit": "ms",
            "extra": "datatype/QuotedIdentifiers:5, 5 pipelines, 5 rounds"
          },
          {
            "name": "asmjit query medical cast filter: compilation",
            "value": 39.153,
            "unit": "ms",
            "extra": "medical/MedicalUseCase:1, 2 pipelines, 5 rounds\npasses tracing=17.2 ssaCreation=4.49 irGeneration=3.55 irPasses.noThrowInference=0.0229 irPasses.exceptionRegionPreparation=0.216 asmjit.compile=13.1 backend=0.0533 other=0.555"
          },
          {
            "name": "asmjit query medical cast filter: execution",
            "value": 0.92,
            "unit": "ms",
            "extra": "medical/MedicalUseCase:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "bc query nexmark scan: compilation",
            "value": 9.655,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:1, 1 pipelines, 5 rounds\npasses tracing=4.12 ssaCreation=1.41 irGeneration=1.19 irPasses.noThrowInference=0.0109 irPasses.exceptionRegionPreparation=0.106 backend=2.63 other=0.195"
          },
          {
            "name": "bc query nexmark scan: execution",
            "value": 33.128,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:1, 1 pipelines, 5 rounds"
          },
          {
            "name": "bc query nexmark q1 map: compilation",
            "value": 3.73,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:2, 2 pipelines, 5 rounds\npasses tracing=1.62 ssaCreation=0.425 irGeneration=0.488 irPasses.noThrowInference=0.00571 irPasses.exceptionRegionPreparation=0.0587 backend=0.923 other=0.193"
          },
          {
            "name": "bc query nexmark q1 map: execution",
            "value": 17.169,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:2, 2 pipelines, 5 rounds"
          },
          {
            "name": "bc query nexmark q5 sliding aggregation join: compilation",
            "value": 65.761,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:4, 12 pipelines, 5 rounds\npasses tracing=28.4 ssaCreation=9.14 irGeneration=8.08 irPasses.noThrowInference=0.0637 irPasses.exceptionRegionPreparation=1.02 backend=17.5 other=1.66"
          },
          {
            "name": "bc query nexmark q5 sliding aggregation join: execution",
            "value": 194.083,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:4, 12 pipelines, 5 rounds"
          },
          {
            "name": "bc query lrb sliding group count: compilation",
            "value": 13.571,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:2, 3 pipelines, 5 rounds\npasses tracing=6.27 ssaCreation=1.78 irGeneration=1.59 irPasses.noThrowInference=0.0142 irPasses.exceptionRegionPreparation=0.123 backend=3.36 other=0.36"
          },
          {
            "name": "bc query lrb sliding group count: execution",
            "value": 1029.502,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:2, 3 pipelines, 5 rounds"
          },
          {
            "name": "bc query manufacturing sliding multi aggregate: compilation",
            "value": 27.613,
            "unit": "ms",
            "extra": "benchmark_small/Manufacturing:1, 2 pipelines, 5 rounds\npasses tracing=14.9 ssaCreation=3.51 irGeneration=2.58 irPasses.noThrowInference=0.0317 irPasses.exceptionRegionPreparation=0.16 backend=5.87 other=0.67"
          },
          {
            "name": "bc query manufacturing sliding multi aggregate: execution",
            "value": 24.032,
            "unit": "ms",
            "extra": "benchmark_small/Manufacturing:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "bc query debs union: compilation",
            "value": 9.526,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:8, 4 pipelines, 5 rounds\npasses tracing=4.44 ssaCreation=1.07 irGeneration=1.2 irPasses.noThrowInference=0.0135 irPasses.exceptionRegionPreparation=0.0908 backend=2.32 other=0.377"
          },
          {
            "name": "bc query debs union: execution",
            "value": 60.313,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:8, 4 pipelines, 5 rounds"
          },
          {
            "name": "bc query wide arithmetic expression: compilation",
            "value": 4.73,
            "unit": "ms",
            "extra": "operator/projection/Filter_Map_Queries:4, 1 pipelines, 5 rounds\npasses tracing=2.32 ssaCreation=0.598 irGeneration=0.553 irPasses.noThrowInference=0.00667 irPasses.exceptionRegionPreparation=0.0517 backend=1.08 other=0.141"
          },
          {
            "name": "bc query wide arithmetic expression: execution",
            "value": 0.461,
            "unit": "ms",
            "extra": "operator/projection/Filter_Map_Queries:4, 1 pipelines, 5 rounds"
          },
          {
            "name": "bc query nested maps: compilation",
            "value": 4.906,
            "unit": "ms",
            "extra": "operator/map/Map_Add:5, 3 pipelines, 5 rounds\npasses tracing=2.07 ssaCreation=0.615 irGeneration=0.623 irPasses.noThrowInference=0.00728 irPasses.exceptionRegionPreparation=0.0775 backend=1.25 other=0.255"
          },
          {
            "name": "bc query nested maps: execution",
            "value": 0.534,
            "unit": "ms",
            "extra": "operator/map/Map_Add:5, 3 pipelines, 5 rounds"
          },
          {
            "name": "bc query round all integer widths: compilation",
            "value": 14.191,
            "unit": "ms",
            "extra": "function/arithmetical/FunctionRound:1, 1 pipelines, 5 rounds\npasses tracing=8.11 ssaCreation=1.5 irGeneration=1.33 irPasses.noThrowInference=0.0145 irPasses.exceptionRegionPreparation=0.121 backend=2.86 other=0.239"
          },
          {
            "name": "bc query round all integer widths: execution",
            "value": 0.613,
            "unit": "ms",
            "extra": "function/arithmetical/FunctionRound:1, 1 pipelines, 5 rounds"
          },
          {
            "name": "bc query string concat: compilation",
            "value": 5.226,
            "unit": "ms",
            "extra": "operator/selection/SelectionNull:7, 2 pipelines, 5 rounds\npasses tracing=2.18 ssaCreation=0.648 irGeneration=0.734 irPasses.noThrowInference=0.00766 irPasses.exceptionRegionPreparation=0.0817 backend=1.34 other=0.209"
          },
          {
            "name": "bc query string concat: execution",
            "value": 0.521,
            "unit": "ms",
            "extra": "operator/selection/SelectionNull:7, 2 pipelines, 5 rounds"
          },
          {
            "name": "bc query nested json scan: compilation",
            "value": 4.74,
            "unit": "ms",
            "extra": "formatter/JSON/NestedJSON:4, 1 pipelines, 5 rounds\npasses tracing=1.92 ssaCreation=0.701 irGeneration=0.654 irPasses.noThrowInference=0.00668 irPasses.exceptionRegionPreparation=0.0595 backend=1.26 other=0.15"
          },
          {
            "name": "bc query nested json scan: execution",
            "value": 0.488,
            "unit": "ms",
            "extra": "formatter/JSON/NestedJSON:4, 1 pipelines, 5 rounds"
          },
          {
            "name": "bc query sliding group count: compilation",
            "value": 13.175,
            "unit": "ms",
            "extra": "operator/aggregation/WindowAggregationCount:6, 2 pipelines, 5 rounds\npasses tracing=6.4 ssaCreation=1.83 irGeneration=1.47 irPasses.noThrowInference=0.0155 irPasses.exceptionRegionPreparation=0.0632 backend=2.95 other=0.356"
          },
          {
            "name": "bc query sliding group count: execution",
            "value": 1.161,
            "unit": "ms",
            "extra": "operator/aggregation/WindowAggregationCount:6, 2 pipelines, 5 rounds"
          },
          {
            "name": "bc query aggregation of union: compilation",
            "value": 9.357,
            "unit": "ms",
            "extra": "operator/aggregation/AggregationOfUnion:1, 5 pipelines, 5 rounds\npasses tracing=3.7 ssaCreation=1.28 irGeneration=1.31 irPasses.noThrowInference=0.0144 irPasses.exceptionRegionPreparation=0.13 backend=2.55 other=0.425"
          },
          {
            "name": "bc query aggregation of union: execution",
            "value": 1.014,
            "unit": "ms",
            "extra": "operator/aggregation/AggregationOfUnion:1, 5 pipelines, 5 rounds"
          },
          {
            "name": "bc query sliding inner join: compilation",
            "value": 66.164,
            "unit": "ms",
            "extra": "guide/guide12:1, 4 pipelines, 5 rounds\npasses tracing=30.1 ssaCreation=8.9 irGeneration=7.37 irPasses.noThrowInference=0.0657 irPasses.exceptionRegionPreparation=1.03 backend=17.2 other=1.53"
          },
          {
            "name": "bc query sliding inner join: execution",
            "value": 1.322,
            "unit": "ms",
            "extra": "guide/guide12:1, 4 pipelines, 5 rounds"
          },
          {
            "name": "bc query nested join quoted identifiers: compilation",
            "value": 66.17,
            "unit": "ms",
            "extra": "datatype/QuotedIdentifiers:5, 5 pipelines, 5 rounds\npasses tracing=29.8 ssaCreation=8.77 irGeneration=7.31 irPasses.noThrowInference=0.0632 irPasses.exceptionRegionPreparation=0.972 backend=17.4 other=1.51"
          },
          {
            "name": "bc query nested join quoted identifiers: execution",
            "value": 1.743,
            "unit": "ms",
            "extra": "datatype/QuotedIdentifiers:5, 5 pipelines, 5 rounds"
          },
          {
            "name": "tbc query nexmark scan: compilation",
            "value": 9.342,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:1, 1 pipelines, 5 rounds\npasses tracing=4.12 ssaCreation=1.38 irGeneration=1.19 irPasses.noThrowInference=0.0119 irPasses.exceptionRegionPreparation=0.106 backend=2.32 other=0.2"
          },
          {
            "name": "tbc query nexmark scan: execution",
            "value": 9.287,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:1, 1 pipelines, 5 rounds"
          },
          {
            "name": "tbc query nexmark q1 map: compilation",
            "value": 3.782,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:2, 2 pipelines, 5 rounds\npasses tracing=1.62 ssaCreation=0.441 irGeneration=0.519 irPasses.noThrowInference=0.0061 irPasses.exceptionRegionPreparation=0.0571 backend=0.969 other=0.186"
          },
          {
            "name": "tbc query nexmark q1 map: execution",
            "value": 6.919,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:2, 2 pipelines, 5 rounds"
          },
          {
            "name": "tbc query nexmark q2 filter: compilation",
            "value": 11.202,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:3, 1 pipelines, 5 rounds\npasses tracing=5.24 ssaCreation=1.67 irGeneration=1.26 irPasses.noThrowInference=0.0125 irPasses.exceptionRegionPreparation=0.111 backend=2.65 other=0.21"
          },
          {
            "name": "tbc query nexmark q2 filter: execution",
            "value": 4.896,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:3, 1 pipelines, 5 rounds"
          },
          {
            "name": "tbc query nexmark q5 sliding aggregation join: compilation",
            "value": 66.166,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:4, 12 pipelines, 5 rounds\npasses tracing=28.5 ssaCreation=9.35 irGeneration=8.19 irPasses.noThrowInference=0.0678 irPasses.exceptionRegionPreparation=1 backend=17 other=1.62"
          },
          {
            "name": "tbc query nexmark q5 sliding aggregation join: execution",
            "value": 36.988,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:4, 12 pipelines, 5 rounds"
          },
          {
            "name": "tbc query nexmark q8 tumbling join: compilation",
            "value": 308.126,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:5, 3 pipelines, 5 rounds\npasses tracing=109 ssaCreation=60.1 irGeneration=37.1 irPasses.noThrowInference=0.243 irPasses.exceptionRegionPreparation=4.46 backend=90.3 other=5.75"
          },
          {
            "name": "tbc query nexmark q8 tumbling join: execution",
            "value": 37.972,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:5, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc query nexmark q8 tumbling join varsized: compilation",
            "value": 255.125,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark_with_varsized:6, 3 pipelines, 5 rounds\npasses tracing=94.7 ssaCreation=48.2 irGeneration=31.7 irPasses.noThrowInference=0.216 irPasses.exceptionRegionPreparation=4.17 backend=68.5 other=5.31"
          },
          {
            "name": "tbc query nexmark q8 tumbling join varsized: execution",
            "value": 12.607,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark_with_varsized:6, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc query ysb filter group sum: compilation",
            "value": 18.333,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark:1, 2 pipelines, 5 rounds\npasses tracing=10.7 ssaCreation=2.01 irGeneration=1.67 irPasses.noThrowInference=0.0135 irPasses.exceptionRegionPreparation=0.0908 backend=3.5 other=0.406"
          },
          {
            "name": "tbc query ysb filter group sum: execution",
            "value": 4.229,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "tbc query ysb varsized filter group count: compilation",
            "value": 20.102,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark_with_varsized:1, 2 pipelines, 5 rounds\npasses tracing=11.2 ssaCreation=2.59 irGeneration=1.84 irPasses.noThrowInference=0.0143 irPasses.exceptionRegionPreparation=0.0993 backend=3.8 other=0.423"
          },
          {
            "name": "tbc query ysb varsized filter group count: execution",
            "value": 2.223,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark_with_varsized:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "tbc query lrb sliding group avg: compilation",
            "value": 16.609,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:1, 3 pipelines, 5 rounds\npasses tracing=7.3 ssaCreation=2.58 irGeneration=1.97 irPasses.noThrowInference=0.02 irPasses.exceptionRegionPreparation=0.163 backend=4.21 other=0.389"
          },
          {
            "name": "tbc query lrb sliding group avg: execution",
            "value": 363.588,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc query lrb sliding group count: compilation",
            "value": 13.535,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:2, 3 pipelines, 5 rounds\npasses tracing=6.38 ssaCreation=1.77 irGeneration=1.57 irPasses.noThrowInference=0.0162 irPasses.exceptionRegionPreparation=0.12 backend=3.29 other=0.346"
          },
          {
            "name": "tbc query lrb sliding group count: execution",
            "value": 186.027,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:2, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc query cluster monitoring sliding group sum: compilation",
            "value": 19.188,
            "unit": "ms",
            "extra": "benchmark_small/ClusterMonitoring:1, 2 pipelines, 5 rounds\npasses tracing=11.4 ssaCreation=2 irGeneration=1.73 irPasses.noThrowInference=0.0141 irPasses.exceptionRegionPreparation=0.0933 backend=3.54 other=0.394"
          },
          {
            "name": "tbc query cluster monitoring sliding group sum: execution",
            "value": 8.708,
            "unit": "ms",
            "extra": "benchmark_small/ClusterMonitoring:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "tbc query manufacturing sliding multi aggregate: compilation",
            "value": 27.963,
            "unit": "ms",
            "extra": "benchmark_small/Manufacturing:1, 2 pipelines, 5 rounds\npasses tracing=14.7 ssaCreation=3.47 irGeneration=2.57 irPasses.noThrowInference=0.0274 irPasses.exceptionRegionPreparation=0.178 backend=6.25 other=0.492"
          },
          {
            "name": "tbc query manufacturing sliding multi aggregate: execution",
            "value": 7.243,
            "unit": "ms",
            "extra": "benchmark_small/Manufacturing:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "tbc query debs range filter: compilation",
            "value": 11.026,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:3, 1 pipelines, 5 rounds\npasses tracing=5.06 ssaCreation=1.73 irGeneration=1.27 irPasses.noThrowInference=0.0112 irPasses.exceptionRegionPreparation=0.0988 backend=2.65 other=0.21"
          },
          {
            "name": "tbc query debs range filter: execution",
            "value": 4.52,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:3, 1 pipelines, 5 rounds"
          },
          {
            "name": "tbc query debs union: compilation",
            "value": 9.653,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:8, 4 pipelines, 5 rounds\npasses tracing=4.45 ssaCreation=1.03 irGeneration=1.21 irPasses.noThrowInference=0.0139 irPasses.exceptionRegionPreparation=0.0958 backend=2.42 other=0.377"
          },
          {
            "name": "tbc query debs union: execution",
            "value": 18.441,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:8, 4 pipelines, 5 rounds"
          },
          {
            "name": "tbc query wide arithmetic expression: compilation",
            "value": 4.795,
            "unit": "ms",
            "extra": "operator/projection/Filter_Map_Queries:4, 1 pipelines, 5 rounds\npasses tracing=2.35 ssaCreation=0.617 irGeneration=0.522 irPasses.noThrowInference=0.00604 irPasses.exceptionRegionPreparation=0.0523 backend=1.09 other=0.138"
          },
          {
            "name": "tbc query wide arithmetic expression: execution",
            "value": 0.884,
            "unit": "ms",
            "extra": "operator/projection/Filter_Map_Queries:4, 1 pipelines, 5 rounds"
          },
          {
            "name": "tbc query nested maps: compilation",
            "value": 4.964,
            "unit": "ms",
            "extra": "operator/map/Map_Add:5, 3 pipelines, 5 rounds\npasses tracing=2.08 ssaCreation=0.607 irGeneration=0.669 irPasses.noThrowInference=0.00689 irPasses.exceptionRegionPreparation=0.0814 backend=1.23 other=0.256"
          },
          {
            "name": "tbc query nested maps: execution",
            "value": 0.965,
            "unit": "ms",
            "extra": "operator/map/Map_Add:5, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc query round all integer widths: compilation",
            "value": 14.053,
            "unit": "ms",
            "extra": "function/arithmetical/FunctionRound:1, 1 pipelines, 5 rounds\npasses tracing=8.18 ssaCreation=1.5 irGeneration=1.32 irPasses.noThrowInference=0.0131 irPasses.exceptionRegionPreparation=0.122 backend=2.68 other=0.215"
          },
          {
            "name": "tbc query round all integer widths: execution",
            "value": 1.029,
            "unit": "ms",
            "extra": "function/arithmetical/FunctionRound:1, 1 pipelines, 5 rounds"
          },
          {
            "name": "tbc query string concat: compilation",
            "value": 5.154,
            "unit": "ms",
            "extra": "operator/selection/SelectionNull:7, 2 pipelines, 5 rounds\npasses tracing=2.17 ssaCreation=0.647 irGeneration=0.67 irPasses.noThrowInference=0.00742 irPasses.exceptionRegionPreparation=0.0785 backend=1.36 other=0.21"
          },
          {
            "name": "tbc query string concat: execution",
            "value": 0.944,
            "unit": "ms",
            "extra": "operator/selection/SelectionNull:7, 2 pipelines, 5 rounds"
          },
          {
            "name": "tbc query filter on string concat: compilation",
            "value": 7.549,
            "unit": "ms",
            "extra": "operator/selection/Selection_NotEquals:7, 1 pipelines, 5 rounds\npasses tracing=3.54 ssaCreation=1.12 irGeneration=0.932 irPasses.noThrowInference=0.00917 irPasses.exceptionRegionPreparation=0.0484 backend=1.73 other=0.181"
          },
          {
            "name": "tbc query filter on string concat: execution",
            "value": 0.958,
            "unit": "ms",
            "extra": "operator/selection/Selection_NotEquals:7, 1 pipelines, 5 rounds"
          },
          {
            "name": "tbc query varsized in list filter: compilation",
            "value": 6.156,
            "unit": "ms",
            "extra": "parser/VarSizedSupport:3, 1 pipelines, 5 rounds\npasses tracing=2.74 ssaCreation=0.883 irGeneration=0.795 irPasses.noThrowInference=0.00852 irPasses.exceptionRegionPreparation=0.0565 backend=1.47 other=0.161"
          },
          {
            "name": "tbc query varsized in list filter: execution",
            "value": 0.924,
            "unit": "ms",
            "extra": "parser/VarSizedSupport:3, 1 pipelines, 5 rounds"
          },
          {
            "name": "tbc query nested json scan: compilation",
            "value": 4.705,
            "unit": "ms",
            "extra": "formatter/JSON/NestedJSON:4, 1 pipelines, 5 rounds\npasses tracing=1.92 ssaCreation=0.696 irGeneration=0.653 irPasses.noThrowInference=0.00694 irPasses.exceptionRegionPreparation=0.0573 backend=1.24 other=0.145"
          },
          {
            "name": "tbc query nested json scan: execution",
            "value": 0.938,
            "unit": "ms",
            "extra": "formatter/JSON/NestedJSON:4, 1 pipelines, 5 rounds"
          },
          {
            "name": "tbc query widest schema scan: compilation",
            "value": 17616.561,
            "unit": "ms",
            "extra": "formatter/MaxNumberOfSupportedFields:1, 1 pipelines, 2 rounds\npasses tracing=1.63e+04 ssaCreation=375 irGeneration=193 irPasses.noThrowInference=0.531 irPasses.exceptionRegionPreparation=3.07 backend=741 other=9.34"
          },
          {
            "name": "tbc query widest schema scan: execution",
            "value": 1.475,
            "unit": "ms",
            "extra": "formatter/MaxNumberOfSupportedFields:1, 1 pipelines, 2 rounds"
          },
          {
            "name": "tbc query tumbling all aggregates: compilation",
            "value": 23.352,
            "unit": "ms",
            "extra": "operator/aggregation/ColumnNames:1, 2 pipelines, 5 rounds\npasses tracing=10.5 ssaCreation=3.69 irGeneration=2.67 irPasses.noThrowInference=0.0252 irPasses.exceptionRegionPreparation=0.192 backend=5.66 other=0.606"
          },
          {
            "name": "tbc query tumbling all aggregates: execution",
            "value": 1.591,
            "unit": "ms",
            "extra": "operator/aggregation/ColumnNames:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "tbc query tumbling median of expression: compilation",
            "value": 13.926,
            "unit": "ms",
            "extra": "operator/aggregation/ExpressionInsideAggregation:3, 3 pipelines, 5 rounds\npasses tracing=4.93 ssaCreation=2.7 irGeneration=1.84 irPasses.noThrowInference=0.0168 irPasses.exceptionRegionPreparation=0.16 backend=3.79 other=0.358"
          },
          {
            "name": "tbc query tumbling median of expression: execution",
            "value": 1.517,
            "unit": "ms",
            "extra": "operator/aggregation/ExpressionInsideAggregation:3, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc query sliding group count: compilation",
            "value": 13.044,
            "unit": "ms",
            "extra": "operator/aggregation/WindowAggregationCount:6, 2 pipelines, 5 rounds\npasses tracing=6.39 ssaCreation=1.82 irGeneration=1.47 irPasses.noThrowInference=0.0154 irPasses.exceptionRegionPreparation=0.0634 backend=2.92 other=0.348"
          },
          {
            "name": "tbc query sliding group count: execution",
            "value": 1.436,
            "unit": "ms",
            "extra": "operator/aggregation/WindowAggregationCount:6, 2 pipelines, 5 rounds"
          },
          {
            "name": "tbc query aggregation of union: compilation",
            "value": 9.294,
            "unit": "ms",
            "extra": "operator/aggregation/AggregationOfUnion:1, 5 pipelines, 5 rounds\npasses tracing=3.7 ssaCreation=1.28 irGeneration=1.29 irPasses.noThrowInference=0.0151 irPasses.exceptionRegionPreparation=0.133 backend=2.49 other=0.41"
          },
          {
            "name": "tbc query aggregation of union: execution",
            "value": 1.386,
            "unit": "ms",
            "extra": "operator/aggregation/AggregationOfUnion:1, 5 pipelines, 5 rounds"
          },
          {
            "name": "tbc query tumbling left outer join: compilation",
            "value": 133.675,
            "unit": "ms",
            "extra": "operator/join/LeftOuterJoin:1, 3 pipelines, 5 rounds\npasses tracing=51.2 ssaCreation=23.4 irGeneration=16.7 irPasses.noThrowInference=0.13 irPasses.exceptionRegionPreparation=2.09 backend=36.7 other=2.97"
          },
          {
            "name": "tbc query tumbling left outer join: execution",
            "value": 98.704,
            "unit": "ms",
            "extra": "operator/join/LeftOuterJoin:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc query tumbling full outer join: compilation",
            "value": 133.939,
            "unit": "ms",
            "extra": "operator/join/FullOuterJoin:1, 3 pipelines, 5 rounds\npasses tracing=51 ssaCreation=24.2 irGeneration=16.9 irPasses.noThrowInference=0.141 irPasses.exceptionRegionPreparation=2.12 backend=37.2 other=3.28"
          },
          {
            "name": "tbc query tumbling full outer join: execution",
            "value": 110.768,
            "unit": "ms",
            "extra": "operator/join/FullOuterJoin:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc query sliding right outer join: compilation",
            "value": 134.329,
            "unit": "ms",
            "extra": "operator/join/SlidingWindowRightOuterJoin:1, 3 pipelines, 5 rounds\npasses tracing=50.9 ssaCreation=23.8 irGeneration=16.8 irPasses.noThrowInference=0.138 irPasses.exceptionRegionPreparation=2.07 backend=37 other=3.11"
          },
          {
            "name": "tbc query sliding right outer join: execution",
            "value": 2.513,
            "unit": "ms",
            "extra": "operator/join/SlidingWindowRightOuterJoin:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc query sliding inner join: compilation",
            "value": 65.827,
            "unit": "ms",
            "extra": "guide/guide12:1, 4 pipelines, 5 rounds\npasses tracing=30.1 ssaCreation=8.86 irGeneration=7.31 irPasses.noThrowInference=0.061 irPasses.exceptionRegionPreparation=0.999 backend=17 other=1.55"
          },
          {
            "name": "tbc query sliding inner join: execution",
            "value": 2.27,
            "unit": "ms",
            "extra": "guide/guide12:1, 4 pipelines, 5 rounds"
          },
          {
            "name": "tbc query nested join quoted identifiers: compilation",
            "value": 66.054,
            "unit": "ms",
            "extra": "datatype/QuotedIdentifiers:5, 5 pipelines, 5 rounds\npasses tracing=29.8 ssaCreation=9.01 irGeneration=7.39 irPasses.noThrowInference=0.068 irPasses.exceptionRegionPreparation=1.01 backend=17.1 other=1.61"
          },
          {
            "name": "tbc query nested join quoted identifiers: execution",
            "value": 2.393,
            "unit": "ms",
            "extra": "datatype/QuotedIdentifiers:5, 5 pipelines, 5 rounds"
          },
          {
            "name": "tbc query medical cast filter: compilation",
            "value": 33.698,
            "unit": "ms",
            "extra": "medical/MedicalUseCase:1, 2 pipelines, 5 rounds\npasses tracing=17.9 ssaCreation=4.6 irGeneration=3.59 irPasses.noThrowInference=0.029 irPasses.exceptionRegionPreparation=0.212 backend=6.79 other=0.565"
          },
          {
            "name": "tbc query medical cast filter: execution",
            "value": 1.744,
            "unit": "ms",
            "extra": "medical/MedicalUseCase:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query nexmark scan: compilation",
            "value": 9.825,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:1, 1 pipelines, 5 rounds\npasses tracing=4.14 ssaCreation=1.36 irGeneration=1.18 irPasses.noThrowInference=0.0103 irPasses.exceptionRegionPreparation=0.102 backend=2.84 other=0.198"
          },
          {
            "name": "tbc-jit query nexmark scan: execution",
            "value": 8.672,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:1, 1 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query nexmark q1 map: compilation",
            "value": 4.149,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:2, 2 pipelines, 5 rounds\npasses tracing=1.56 ssaCreation=0.434 irGeneration=0.52 irPasses.noThrowInference=0.0058 irPasses.exceptionRegionPreparation=0.0586 backend=1.36 other=0.195"
          },
          {
            "name": "tbc-jit query nexmark q1 map: execution",
            "value": 6.181,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:2, 2 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query nexmark q2 filter: compilation",
            "value": 11.63,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:3, 1 pipelines, 5 rounds\npasses tracing=5.19 ssaCreation=1.66 irGeneration=1.24 irPasses.noThrowInference=0.013 irPasses.exceptionRegionPreparation=0.124 backend=3.14 other=0.21"
          },
          {
            "name": "tbc-jit query nexmark q2 filter: execution",
            "value": 4.264,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:3, 1 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query nexmark q5 sliding aggregation join: compilation",
            "value": 67.524,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:4, 12 pipelines, 5 rounds\npasses tracing=28.2 ssaCreation=9.58 irGeneration=8.1 irPasses.noThrowInference=0.0625 irPasses.exceptionRegionPreparation=0.975 backend=18.8 other=1.66"
          },
          {
            "name": "tbc-jit query nexmark q5 sliding aggregation join: execution",
            "value": 29.072,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:4, 12 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query nexmark q8 tumbling join: compilation",
            "value": 310.268,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:5, 3 pipelines, 5 rounds\npasses tracing=109 ssaCreation=57.2 irGeneration=37.4 irPasses.noThrowInference=0.236 irPasses.exceptionRegionPreparation=4.6 backend=95.8 other=6.04"
          },
          {
            "name": "tbc-jit query nexmark q8 tumbling join: execution",
            "value": 32.54,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:5, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query nexmark q8 tumbling join varsized: compilation",
            "value": 253.973,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark_with_varsized:6, 3 pipelines, 5 rounds\npasses tracing=93.8 ssaCreation=48.9 irGeneration=31.1 irPasses.noThrowInference=0.209 irPasses.exceptionRegionPreparation=4.08 backend=71.6 other=5.04"
          },
          {
            "name": "tbc-jit query nexmark q8 tumbling join varsized: execution",
            "value": 10.843,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark_with_varsized:6, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query ysb filter group sum: compilation",
            "value": 18.894,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark:1, 2 pipelines, 5 rounds\npasses tracing=10.6 ssaCreation=1.98 irGeneration=1.67 irPasses.noThrowInference=0.0161 irPasses.exceptionRegionPreparation=0.0873 backend=4.09 other=0.416"
          },
          {
            "name": "tbc-jit query ysb filter group sum: execution",
            "value": 3.65,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query ysb varsized filter group count: compilation",
            "value": 20.705,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark_with_varsized:1, 2 pipelines, 5 rounds\npasses tracing=11.3 ssaCreation=2.65 irGeneration=1.85 irPasses.noThrowInference=0.0168 irPasses.exceptionRegionPreparation=0.1 backend=4.39 other=0.385"
          },
          {
            "name": "tbc-jit query ysb varsized filter group count: execution",
            "value": 2.028,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark_with_varsized:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query lrb sliding group avg: compilation",
            "value": 17.395,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:1, 3 pipelines, 5 rounds\npasses tracing=7.35 ssaCreation=2.65 irGeneration=1.94 irPasses.noThrowInference=0.0176 irPasses.exceptionRegionPreparation=0.163 backend=4.85 other=0.402"
          },
          {
            "name": "tbc-jit query lrb sliding group avg: execution",
            "value": 202.047,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query lrb sliding group count: compilation",
            "value": 14.178,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:2, 3 pipelines, 5 rounds\npasses tracing=6.38 ssaCreation=1.77 irGeneration=1.57 irPasses.noThrowInference=0.0151 irPasses.exceptionRegionPreparation=0.118 backend=3.95 other=0.354"
          },
          {
            "name": "tbc-jit query lrb sliding group count: execution",
            "value": 139.976,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:2, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query cluster monitoring sliding group sum: compilation",
            "value": 19.605,
            "unit": "ms",
            "extra": "benchmark_small/ClusterMonitoring:1, 2 pipelines, 5 rounds\npasses tracing=11.2 ssaCreation=1.96 irGeneration=1.68 irPasses.noThrowInference=0.0145 irPasses.exceptionRegionPreparation=0.0917 backend=4.1 other=0.438"
          },
          {
            "name": "tbc-jit query cluster monitoring sliding group sum: execution",
            "value": 7.755,
            "unit": "ms",
            "extra": "benchmark_small/ClusterMonitoring:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query manufacturing sliding multi aggregate: compilation",
            "value": 28.748,
            "unit": "ms",
            "extra": "benchmark_small/Manufacturing:1, 2 pipelines, 5 rounds\npasses tracing=14.8 ssaCreation=3.45 irGeneration=2.58 irPasses.noThrowInference=0.0259 irPasses.exceptionRegionPreparation=0.161 backend=7.21 other=0.498"
          },
          {
            "name": "tbc-jit query manufacturing sliding multi aggregate: execution",
            "value": 6.238,
            "unit": "ms",
            "extra": "benchmark_small/Manufacturing:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query debs range filter: compilation",
            "value": 11.519,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:3, 1 pipelines, 5 rounds\npasses tracing=5.1 ssaCreation=1.72 irGeneration=1.29 irPasses.noThrowInference=0.0114 irPasses.exceptionRegionPreparation=0.101 backend=3.1 other=0.21"
          },
          {
            "name": "tbc-jit query debs range filter: execution",
            "value": 4.122,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:3, 1 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query debs union: compilation",
            "value": 10.201,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:8, 4 pipelines, 5 rounds\npasses tracing=4.44 ssaCreation=1.05 irGeneration=1.2 irPasses.noThrowInference=0.0138 irPasses.exceptionRegionPreparation=0.0882 backend=3.03 other=0.412"
          },
          {
            "name": "tbc-jit query debs union: execution",
            "value": 16.287,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:8, 4 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query wide arithmetic expression: compilation",
            "value": 5.137,
            "unit": "ms",
            "extra": "operator/projection/Filter_Map_Queries:4, 1 pipelines, 5 rounds\npasses tracing=2.3 ssaCreation=0.601 irGeneration=0.55 irPasses.noThrowInference=0.00599 irPasses.exceptionRegionPreparation=0.0522 backend=1.49 other=0.145"
          },
          {
            "name": "tbc-jit query wide arithmetic expression: execution",
            "value": 0.914,
            "unit": "ms",
            "extra": "operator/projection/Filter_Map_Queries:4, 1 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query nested maps: compilation",
            "value": 5.375,
            "unit": "ms",
            "extra": "operator/map/Map_Add:5, 3 pipelines, 5 rounds\npasses tracing=2.06 ssaCreation=0.571 irGeneration=0.665 irPasses.noThrowInference=0.00708 irPasses.exceptionRegionPreparation=0.082 backend=1.71 other=0.259"
          },
          {
            "name": "tbc-jit query nested maps: execution",
            "value": 0.959,
            "unit": "ms",
            "extra": "operator/map/Map_Add:5, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query round all integer widths: compilation",
            "value": 14.544,
            "unit": "ms",
            "extra": "function/arithmetical/FunctionRound:1, 1 pipelines, 5 rounds\npasses tracing=8.15 ssaCreation=1.48 irGeneration=1.32 irPasses.noThrowInference=0.0144 irPasses.exceptionRegionPreparation=0.122 backend=3.19 other=0.219"
          },
          {
            "name": "tbc-jit query round all integer widths: execution",
            "value": 0.993,
            "unit": "ms",
            "extra": "function/arithmetical/FunctionRound:1, 1 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query string concat: compilation",
            "value": 5.647,
            "unit": "ms",
            "extra": "operator/selection/SelectionNull:7, 2 pipelines, 5 rounds\npasses tracing=2.19 ssaCreation=0.67 irGeneration=0.694 irPasses.noThrowInference=0.00756 irPasses.exceptionRegionPreparation=0.0848 backend=1.79 other=0.211"
          },
          {
            "name": "tbc-jit query string concat: execution",
            "value": 0.96,
            "unit": "ms",
            "extra": "operator/selection/SelectionNull:7, 2 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query filter on string concat: compilation",
            "value": 8.229,
            "unit": "ms",
            "extra": "operator/selection/Selection_NotEquals:7, 1 pipelines, 5 rounds\npasses tracing=3.64 ssaCreation=1.18 irGeneration=0.958 irPasses.noThrowInference=0.00948 irPasses.exceptionRegionPreparation=0.0545 backend=2.25 other=0.179"
          },
          {
            "name": "tbc-jit query filter on string concat: execution",
            "value": 0.946,
            "unit": "ms",
            "extra": "operator/selection/Selection_NotEquals:7, 1 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query varsized in list filter: compilation",
            "value": 6.467,
            "unit": "ms",
            "extra": "parser/VarSizedSupport:3, 1 pipelines, 5 rounds\npasses tracing=2.68 ssaCreation=0.891 irGeneration=0.793 irPasses.noThrowInference=0.00805 irPasses.exceptionRegionPreparation=0.0564 backend=1.87 other=0.161"
          },
          {
            "name": "tbc-jit query varsized in list filter: execution",
            "value": 0.872,
            "unit": "ms",
            "extra": "parser/VarSizedSupport:3, 1 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query nested json scan: compilation",
            "value": 5.096,
            "unit": "ms",
            "extra": "formatter/JSON/NestedJSON:4, 1 pipelines, 5 rounds\npasses tracing=1.93 ssaCreation=0.72 irGeneration=0.624 irPasses.noThrowInference=0.00663 irPasses.exceptionRegionPreparation=0.0536 backend=1.6 other=0.16"
          },
          {
            "name": "tbc-jit query nested json scan: execution",
            "value": 0.902,
            "unit": "ms",
            "extra": "formatter/JSON/NestedJSON:4, 1 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query widest schema scan: compilation",
            "value": 17859.55,
            "unit": "ms",
            "extra": "formatter/MaxNumberOfSupportedFields:1, 1 pipelines, 2 rounds\npasses tracing=1.65e+04 ssaCreation=371 irGeneration=194 irPasses.noThrowInference=0.526 irPasses.exceptionRegionPreparation=3.13 backend=787 other=9.4"
          },
          {
            "name": "tbc-jit query widest schema scan: execution",
            "value": 1.775,
            "unit": "ms",
            "extra": "formatter/MaxNumberOfSupportedFields:1, 1 pipelines, 2 rounds"
          },
          {
            "name": "tbc-jit query tumbling all aggregates: compilation",
            "value": 24.306,
            "unit": "ms",
            "extra": "operator/aggregation/ColumnNames:1, 2 pipelines, 5 rounds\npasses tracing=10.4 ssaCreation=3.8 irGeneration=2.66 irPasses.noThrowInference=0.0259 irPasses.exceptionRegionPreparation=0.198 backend=6.48 other=0.585"
          },
          {
            "name": "tbc-jit query tumbling all aggregates: execution",
            "value": 1.517,
            "unit": "ms",
            "extra": "operator/aggregation/ColumnNames:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query tumbling median of expression: compilation",
            "value": 14.377,
            "unit": "ms",
            "extra": "operator/aggregation/ExpressionInsideAggregation:3, 3 pipelines, 5 rounds\npasses tracing=4.99 ssaCreation=2.71 irGeneration=1.85 irPasses.noThrowInference=0.0162 irPasses.exceptionRegionPreparation=0.163 backend=4.3 other=0.374"
          },
          {
            "name": "tbc-jit query tumbling median of expression: execution",
            "value": 1.51,
            "unit": "ms",
            "extra": "operator/aggregation/ExpressionInsideAggregation:3, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query sliding group count: compilation",
            "value": 13.446,
            "unit": "ms",
            "extra": "operator/aggregation/WindowAggregationCount:6, 2 pipelines, 5 rounds\npasses tracing=6.31 ssaCreation=1.81 irGeneration=1.47 irPasses.noThrowInference=0.0158 irPasses.exceptionRegionPreparation=0.0638 backend=3.39 other=0.349"
          },
          {
            "name": "tbc-jit query sliding group count: execution",
            "value": 1.412,
            "unit": "ms",
            "extra": "operator/aggregation/WindowAggregationCount:6, 2 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query aggregation of union: compilation",
            "value": 9.8,
            "unit": "ms",
            "extra": "operator/aggregation/AggregationOfUnion:1, 5 pipelines, 5 rounds\npasses tracing=3.68 ssaCreation=1.22 irGeneration=1.3 irPasses.noThrowInference=0.0136 irPasses.exceptionRegionPreparation=0.134 backend=3.07 other=0.433"
          },
          {
            "name": "tbc-jit query aggregation of union: execution",
            "value": 1.336,
            "unit": "ms",
            "extra": "operator/aggregation/AggregationOfUnion:1, 5 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query tumbling left outer join: compilation",
            "value": 135.93,
            "unit": "ms",
            "extra": "operator/join/LeftOuterJoin:1, 3 pipelines, 5 rounds\npasses tracing=50.9 ssaCreation=23.4 irGeneration=16.8 irPasses.noThrowInference=0.143 irPasses.exceptionRegionPreparation=2.05 backend=40 other=3.02"
          },
          {
            "name": "tbc-jit query tumbling left outer join: execution",
            "value": 82.851,
            "unit": "ms",
            "extra": "operator/join/LeftOuterJoin:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query tumbling full outer join: compilation",
            "value": 136.434,
            "unit": "ms",
            "extra": "operator/join/FullOuterJoin:1, 3 pipelines, 5 rounds\npasses tracing=50.6 ssaCreation=23.7 irGeneration=16.8 irPasses.noThrowInference=0.132 irPasses.exceptionRegionPreparation=2.08 backend=39.7 other=2.95"
          },
          {
            "name": "tbc-jit query tumbling full outer join: execution",
            "value": 94.159,
            "unit": "ms",
            "extra": "operator/join/FullOuterJoin:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query sliding right outer join: compilation",
            "value": 137.269,
            "unit": "ms",
            "extra": "operator/join/SlidingWindowRightOuterJoin:1, 3 pipelines, 5 rounds\npasses tracing=51.3 ssaCreation=23.8 irGeneration=16.8 irPasses.noThrowInference=0.133 irPasses.exceptionRegionPreparation=2.08 backend=40 other=3.05"
          },
          {
            "name": "tbc-jit query sliding right outer join: execution",
            "value": 2.573,
            "unit": "ms",
            "extra": "operator/join/SlidingWindowRightOuterJoin:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query sliding inner join: compilation",
            "value": 67.38,
            "unit": "ms",
            "extra": "guide/guide12:1, 4 pipelines, 5 rounds\npasses tracing=30 ssaCreation=8.81 irGeneration=7.3 irPasses.noThrowInference=0.0608 irPasses.exceptionRegionPreparation=1.02 backend=18.7 other=1.48"
          },
          {
            "name": "tbc-jit query sliding inner join: execution",
            "value": 2.294,
            "unit": "ms",
            "extra": "guide/guide12:1, 4 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query nested join quoted identifiers: compilation",
            "value": 67.889,
            "unit": "ms",
            "extra": "datatype/QuotedIdentifiers:5, 5 pipelines, 5 rounds\npasses tracing=29.8 ssaCreation=8.89 irGeneration=7.45 irPasses.noThrowInference=0.0742 irPasses.exceptionRegionPreparation=1.03 backend=18.9 other=1.6"
          },
          {
            "name": "tbc-jit query nested join quoted identifiers: execution",
            "value": 2.306,
            "unit": "ms",
            "extra": "datatype/QuotedIdentifiers:5, 5 pipelines, 5 rounds"
          },
          {
            "name": "tbc-jit query medical cast filter: compilation",
            "value": 34.568,
            "unit": "ms",
            "extra": "medical/MedicalUseCase:1, 2 pipelines, 5 rounds\npasses tracing=17.8 ssaCreation=4.7 irGeneration=3.59 irPasses.noThrowInference=0.0299 irPasses.exceptionRegionPreparation=0.214 backend=7.56 other=0.526"
          },
          {
            "name": "tbc-jit query medical cast filter: execution",
            "value": 1.709,
            "unit": "ms",
            "extra": "medical/MedicalUseCase:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "cpp query nexmark scan: compilation",
            "value": 273.593,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:1, 1 pipelines, 5 rounds\npasses tracing=4.1 ssaCreation=1.36 irGeneration=1.18 irPasses.noThrowInference=0.0111 irPasses.exceptionRegionPreparation=0.107 cpp.loweringFromIR=2.29 cpp.compile=264 other=0.24"
          },
          {
            "name": "cpp query nexmark scan: execution",
            "value": 5.16,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:1, 1 pipelines, 5 rounds"
          },
          {
            "name": "cpp query nexmark q1 map: compilation",
            "value": 280.395,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:2, 2 pipelines, 5 rounds\npasses tracing=1.6 ssaCreation=0.434 irGeneration=0.497 irPasses.noThrowInference=0.00582 irPasses.exceptionRegionPreparation=0.056 cpp.loweringFromIR=1.27 cpp.compile=276 other=0.267"
          },
          {
            "name": "cpp query nexmark q1 map: execution",
            "value": 4.207,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:2, 2 pipelines, 5 rounds"
          },
          {
            "name": "cpp query nexmark q2 filter: compilation",
            "value": 285.951,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:3, 1 pipelines, 5 rounds\npasses tracing=5.23 ssaCreation=1.69 irGeneration=1.26 irPasses.noThrowInference=0.0127 irPasses.exceptionRegionPreparation=0.112 cpp.loweringFromIR=2.62 cpp.compile=275 other=0.252"
          },
          {
            "name": "cpp query nexmark q2 filter: execution",
            "value": 3.18,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:3, 1 pipelines, 5 rounds"
          },
          {
            "name": "cpp query nexmark q5 sliding aggregation join: compilation",
            "value": 2242.552,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:4, 12 pipelines, 5 rounds\npasses tracing=28.1 ssaCreation=9.38 irGeneration=8.23 irPasses.noThrowInference=0.0713 irPasses.exceptionRegionPreparation=1.02 cpp.loweringFromIR=16.8 cpp.compile=2.18e+03 backend=0.0303 other=2.2"
          },
          {
            "name": "cpp query nexmark q5 sliding aggregation join: execution",
            "value": 22.107,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:4, 12 pipelines, 5 rounds"
          },
          {
            "name": "cpp query nexmark q8 tumbling join: compilation",
            "value": 4054.184,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:5, 3 pipelines, 5 rounds\npasses tracing=108 ssaCreation=55.5 irGeneration=36.6 irPasses.noThrowInference=0.239 irPasses.exceptionRegionPreparation=4.51 cpp.loweringFromIR=52.3 cpp.compile=3.79e+03 backend=0.00985 other=5.97"
          },
          {
            "name": "cpp query nexmark q8 tumbling join: execution",
            "value": 21.371,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:5, 3 pipelines, 5 rounds"
          },
          {
            "name": "cpp query nexmark q8 tumbling join varsized: compilation",
            "value": 3683.73,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark_with_varsized:6, 3 pipelines, 5 rounds\npasses tracing=92.3 ssaCreation=49.2 irGeneration=30.9 irPasses.noThrowInference=0.222 irPasses.exceptionRegionPreparation=4.16 cpp.loweringFromIR=46.3 cpp.compile=3.46e+03 backend=0.0102 other=5.48"
          },
          {
            "name": "cpp query nexmark q8 tumbling join varsized: execution",
            "value": 8.38,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark_with_varsized:6, 3 pipelines, 5 rounds"
          },
          {
            "name": "cpp query ysb filter group sum: compilation",
            "value": 420,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark:1, 2 pipelines, 5 rounds\npasses tracing=10.6 ssaCreation=1.97 irGeneration=1.69 irPasses.noThrowInference=0.0158 irPasses.exceptionRegionPreparation=0.0921 cpp.loweringFromIR=3.51 cpp.compile=401 backend=0.00561 other=0.515"
          },
          {
            "name": "cpp query ysb filter group sum: execution",
            "value": 2.841,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "cpp query ysb varsized filter group count: compilation",
            "value": 421.787,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark_with_varsized:1, 2 pipelines, 5 rounds\npasses tracing=11.2 ssaCreation=2.63 irGeneration=1.86 irPasses.noThrowInference=0.0176 irPasses.exceptionRegionPreparation=0.101 cpp.loweringFromIR=3.66 cpp.compile=402 backend=0.00585 other=0.542"
          },
          {
            "name": "cpp query ysb varsized filter group count: execution",
            "value": 1.652,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark_with_varsized:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "cpp query lrb sliding group avg: compilation",
            "value": 560.26,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:1, 3 pipelines, 5 rounds\npasses tracing=7.17 ssaCreation=2.62 irGeneration=1.93 irPasses.noThrowInference=0.0186 irPasses.exceptionRegionPreparation=0.164 cpp.loweringFromIR=4.22 cpp.compile=544 backend=0.00814 other=0.485"
          },
          {
            "name": "cpp query lrb sliding group avg: execution",
            "value": 191.232,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "cpp query lrb sliding group count: compilation",
            "value": 507.228,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:2, 3 pipelines, 5 rounds\npasses tracing=6.47 ssaCreation=1.81 irGeneration=1.6 irPasses.noThrowInference=0.0177 irPasses.exceptionRegionPreparation=0.126 cpp.loweringFromIR=3.58 cpp.compile=493 backend=0.00738 other=0.503"
          },
          {
            "name": "cpp query lrb sliding group count: execution",
            "value": 77.169,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:2, 3 pipelines, 5 rounds"
          },
          {
            "name": "cpp query cluster monitoring sliding group sum: compilation",
            "value": 425.803,
            "unit": "ms",
            "extra": "benchmark_small/ClusterMonitoring:1, 2 pipelines, 5 rounds\npasses tracing=11.2 ssaCreation=1.98 irGeneration=1.69 irPasses.noThrowInference=0.0153 irPasses.exceptionRegionPreparation=0.0925 cpp.loweringFromIR=3.85 cpp.compile=406 backend=0.00522 other=0.516"
          },
          {
            "name": "cpp query cluster monitoring sliding group sum: execution",
            "value": 6.238,
            "unit": "ms",
            "extra": "benchmark_small/ClusterMonitoring:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "cpp query manufacturing sliding multi aggregate: compilation",
            "value": 602.717,
            "unit": "ms",
            "extra": "benchmark_small/Manufacturing:1, 2 pipelines, 5 rounds\npasses tracing=14.8 ssaCreation=3.44 irGeneration=2.55 irPasses.noThrowInference=0.0244 irPasses.exceptionRegionPreparation=0.162 cpp.loweringFromIR=5.94 cpp.compile=575 backend=0.00537 other=0.717"
          },
          {
            "name": "cpp query manufacturing sliding multi aggregate: execution",
            "value": 5.143,
            "unit": "ms",
            "extra": "benchmark_small/Manufacturing:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "cpp query debs range filter: compilation",
            "value": 283.126,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:3, 1 pipelines, 5 rounds\npasses tracing=5.08 ssaCreation=1.75 irGeneration=1.27 irPasses.noThrowInference=0.0128 irPasses.exceptionRegionPreparation=0.101 cpp.loweringFromIR=2.59 cpp.compile=272 other=0.256"
          },
          {
            "name": "cpp query debs range filter: execution",
            "value": 2.931,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:3, 1 pipelines, 5 rounds"
          },
          {
            "name": "cpp query debs union: compilation",
            "value": 582.799,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:8, 4 pipelines, 5 rounds\npasses tracing=4.45 ssaCreation=1.11 irGeneration=1.17 irPasses.noThrowInference=0.0132 irPasses.exceptionRegionPreparation=0.105 cpp.loweringFromIR=3 cpp.compile=572 backend=0.00928 other=0.55"
          },
          {
            "name": "cpp query debs union: execution",
            "value": 9.718,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:8, 4 pipelines, 5 rounds"
          },
          {
            "name": "cpp query wide arithmetic expression: compilation",
            "value": 196.895,
            "unit": "ms",
            "extra": "operator/projection/Filter_Map_Queries:4, 1 pipelines, 5 rounds\npasses tracing=2.31 ssaCreation=0.598 irGeneration=0.55 irPasses.noThrowInference=0.00586 irPasses.exceptionRegionPreparation=0.0519 cpp.loweringFromIR=1.29 cpp.compile=192 other=0.194"
          },
          {
            "name": "cpp query wide arithmetic expression: execution",
            "value": 0.526,
            "unit": "ms",
            "extra": "operator/projection/Filter_Map_Queries:4, 1 pipelines, 5 rounds"
          },
          {
            "name": "cpp query nested maps: compilation",
            "value": 396.236,
            "unit": "ms",
            "extra": "operator/map/Map_Add:5, 3 pipelines, 5 rounds\npasses tracing=2.09 ssaCreation=0.598 irGeneration=0.711 irPasses.noThrowInference=0.00863 irPasses.exceptionRegionPreparation=0.0821 cpp.loweringFromIR=1.59 cpp.compile=391 backend=0.00701 other=0.373"
          },
          {
            "name": "cpp query nested maps: execution",
            "value": 0.75,
            "unit": "ms",
            "extra": "operator/map/Map_Add:5, 3 pipelines, 5 rounds"
          },
          {
            "name": "cpp query round all integer widths: compilation",
            "value": 347.403,
            "unit": "ms",
            "extra": "function/arithmetical/FunctionRound:1, 1 pipelines, 5 rounds\npasses tracing=8.23 ssaCreation=1.47 irGeneration=1.32 irPasses.noThrowInference=0.0145 irPasses.exceptionRegionPreparation=0.123 cpp.loweringFromIR=3.1 cpp.compile=333 other=0.276"
          },
          {
            "name": "cpp query round all integer widths: execution",
            "value": 0.697,
            "unit": "ms",
            "extra": "function/arithmetical/FunctionRound:1, 1 pipelines, 5 rounds"
          },
          {
            "name": "cpp query string concat: compilation",
            "value": 314.329,
            "unit": "ms",
            "extra": "operator/selection/SelectionNull:7, 2 pipelines, 5 rounds\npasses tracing=2.17 ssaCreation=0.683 irGeneration=0.688 irPasses.noThrowInference=0.00757 irPasses.exceptionRegionPreparation=0.0831 cpp.loweringFromIR=1.6 cpp.compile=309 other=0.293"
          },
          {
            "name": "cpp query string concat: execution",
            "value": 0.646,
            "unit": "ms",
            "extra": "operator/selection/SelectionNull:7, 2 pipelines, 5 rounds"
          },
          {
            "name": "cpp query filter on string concat: compilation",
            "value": 227.351,
            "unit": "ms",
            "extra": "operator/selection/Selection_NotEquals:7, 1 pipelines, 5 rounds\npasses tracing=3.53 ssaCreation=1.11 irGeneration=0.934 irPasses.noThrowInference=0.00878 irPasses.exceptionRegionPreparation=0.0486 cpp.loweringFromIR=1.97 cpp.compile=220 other=0.22"
          },
          {
            "name": "cpp query filter on string concat: execution",
            "value": 0.553,
            "unit": "ms",
            "extra": "operator/selection/Selection_NotEquals:7, 1 pipelines, 5 rounds"
          },
          {
            "name": "cpp query varsized in list filter: compilation",
            "value": 215.126,
            "unit": "ms",
            "extra": "parser/VarSizedSupport:3, 1 pipelines, 5 rounds\npasses tracing=2.72 ssaCreation=0.883 irGeneration=0.794 irPasses.noThrowInference=0.00758 irPasses.exceptionRegionPreparation=0.0561 cpp.loweringFromIR=1.73 cpp.compile=209 other=0.199"
          },
          {
            "name": "cpp query varsized in list filter: execution",
            "value": 0.528,
            "unit": "ms",
            "extra": "parser/VarSizedSupport:3, 1 pipelines, 5 rounds"
          },
          {
            "name": "cpp query nested json scan: compilation",
            "value": 218.383,
            "unit": "ms",
            "extra": "formatter/JSON/NestedJSON:4, 1 pipelines, 5 rounds\npasses tracing=1.94 ssaCreation=0.722 irGeneration=0.66 irPasses.noThrowInference=0.00728 irPasses.exceptionRegionPreparation=0.0597 cpp.loweringFromIR=1.49 cpp.compile=213 other=0.2"
          },
          {
            "name": "cpp query nested json scan: execution",
            "value": 0.576,
            "unit": "ms",
            "extra": "formatter/JSON/NestedJSON:4, 1 pipelines, 5 rounds"
          },
          {
            "name": "cpp query widest schema scan: compilation",
            "value": 24432.986,
            "unit": "ms",
            "extra": "formatter/MaxNumberOfSupportedFields:1, 1 pipelines, 2 rounds\npasses tracing=1.66e+04 ssaCreation=386 irGeneration=195 irPasses.noThrowInference=0.538 irPasses.exceptionRegionPreparation=3.25 cpp.loweringFromIR=138 cpp.compile=7.12e+03 other=10.5"
          },
          {
            "name": "cpp query widest schema scan: execution",
            "value": 1.647,
            "unit": "ms",
            "extra": "formatter/MaxNumberOfSupportedFields:1, 1 pipelines, 2 rounds"
          },
          {
            "name": "cpp query tumbling all aggregates: compilation",
            "value": 564.102,
            "unit": "ms",
            "extra": "operator/aggregation/ColumnNames:1, 2 pipelines, 5 rounds\npasses tracing=10.3 ssaCreation=3.87 irGeneration=2.65 irPasses.noThrowInference=0.0238 irPasses.exceptionRegionPreparation=0.197 cpp.loweringFromIR=5.49 cpp.compile=541 backend=0.00546 other=0.698"
          },
          {
            "name": "cpp query tumbling all aggregates: execution",
            "value": 1.254,
            "unit": "ms",
            "extra": "operator/aggregation/ColumnNames:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "cpp query tumbling median of expression: compilation",
            "value": 510.486,
            "unit": "ms",
            "extra": "operator/aggregation/ExpressionInsideAggregation:3, 3 pipelines, 5 rounds\npasses tracing=4.88 ssaCreation=2.74 irGeneration=1.87 irPasses.noThrowInference=0.0174 irPasses.exceptionRegionPreparation=0.159 cpp.loweringFromIR=3.61 cpp.compile=497 backend=0.00761 other=0.471"
          },
          {
            "name": "cpp query tumbling median of expression: execution",
            "value": 1.198,
            "unit": "ms",
            "extra": "operator/aggregation/ExpressionInsideAggregation:3, 3 pipelines, 5 rounds"
          },
          {
            "name": "cpp query sliding group count: compilation",
            "value": 362.108,
            "unit": "ms",
            "extra": "operator/aggregation/WindowAggregationCount:6, 2 pipelines, 5 rounds\npasses tracing=6.32 ssaCreation=1.78 irGeneration=1.45 irPasses.noThrowInference=0.0153 irPasses.exceptionRegionPreparation=0.0658 cpp.loweringFromIR=3.05 cpp.compile=349 backend=0.00508 other=0.453"
          },
          {
            "name": "cpp query sliding group count: execution",
            "value": 1.123,
            "unit": "ms",
            "extra": "operator/aggregation/WindowAggregationCount:6, 2 pipelines, 5 rounds"
          },
          {
            "name": "cpp query aggregation of union: compilation",
            "value": 663.804,
            "unit": "ms",
            "extra": "operator/aggregation/AggregationOfUnion:1, 5 pipelines, 5 rounds\npasses tracing=3.74 ssaCreation=1.31 irGeneration=1.29 irPasses.noThrowInference=0.015 irPasses.exceptionRegionPreparation=0.136 cpp.loweringFromIR=3.06 cpp.compile=654 backend=0.0123 other=0.627"
          },
          {
            "name": "cpp query aggregation of union: execution",
            "value": 1.296,
            "unit": "ms",
            "extra": "operator/aggregation/AggregationOfUnion:1, 5 pipelines, 5 rounds"
          },
          {
            "name": "cpp query tumbling left outer join: compilation",
            "value": 2654.566,
            "unit": "ms",
            "extra": "operator/join/LeftOuterJoin:1, 3 pipelines, 5 rounds\npasses tracing=51.1 ssaCreation=23.5 irGeneration=17 irPasses.noThrowInference=0.149 irPasses.exceptionRegionPreparation=2.13 cpp.loweringFromIR=29.6 cpp.compile=2.53e+03 backend=0.0092 other=3.42"
          },
          {
            "name": "cpp query tumbling left outer join: execution",
            "value": 46.698,
            "unit": "ms",
            "extra": "operator/join/LeftOuterJoin:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "cpp query tumbling full outer join: compilation",
            "value": 2659.381,
            "unit": "ms",
            "extra": "operator/join/FullOuterJoin:1, 3 pipelines, 5 rounds\npasses tracing=51 ssaCreation=23.5 irGeneration=16.8 irPasses.noThrowInference=0.137 irPasses.exceptionRegionPreparation=2.06 cpp.loweringFromIR=29.4 cpp.compile=2.53e+03 backend=0.00884 other=3.2"
          },
          {
            "name": "cpp query tumbling full outer join: execution",
            "value": 50.54,
            "unit": "ms",
            "extra": "operator/join/FullOuterJoin:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "cpp query sliding right outer join: compilation",
            "value": 2628.468,
            "unit": "ms",
            "extra": "operator/join/SlidingWindowRightOuterJoin:1, 3 pipelines, 5 rounds\npasses tracing=50.4 ssaCreation=23.1 irGeneration=16.7 irPasses.noThrowInference=0.137 irPasses.exceptionRegionPreparation=2.1 cpp.loweringFromIR=29.1 cpp.compile=2.5e+03 backend=0.00869 other=3.12"
          },
          {
            "name": "cpp query sliding right outer join: execution",
            "value": 1.895,
            "unit": "ms",
            "extra": "operator/join/SlidingWindowRightOuterJoin:1, 3 pipelines, 5 rounds"
          },
          {
            "name": "cpp query sliding inner join: compilation",
            "value": 1627.455,
            "unit": "ms",
            "extra": "guide/guide12:1, 4 pipelines, 5 rounds\npasses tracing=29.6 ssaCreation=8.68 irGeneration=7.4 irPasses.noThrowInference=0.064 irPasses.exceptionRegionPreparation=0.977 cpp.loweringFromIR=16.2 cpp.compile=1.56e+03 backend=0.0105 other=1.76"
          },
          {
            "name": "cpp query sliding inner join: execution",
            "value": 1.872,
            "unit": "ms",
            "extra": "guide/guide12:1, 4 pipelines, 5 rounds"
          },
          {
            "name": "cpp query nested join quoted identifiers: compilation",
            "value": 1723.632,
            "unit": "ms",
            "extra": "datatype/QuotedIdentifiers:5, 5 pipelines, 5 rounds\npasses tracing=29 ssaCreation=8.71 irGeneration=7.44 irPasses.noThrowInference=0.0688 irPasses.exceptionRegionPreparation=1.03 cpp.loweringFromIR=16.5 cpp.compile=1.66e+03 backend=0.0134 other=1.84"
          },
          {
            "name": "cpp query nested join quoted identifiers: execution",
            "value": 2.034,
            "unit": "ms",
            "extra": "datatype/QuotedIdentifiers:5, 5 pipelines, 5 rounds"
          },
          {
            "name": "cpp query medical cast filter: compilation",
            "value": 626.033,
            "unit": "ms",
            "extra": "medical/MedicalUseCase:1, 2 pipelines, 5 rounds\npasses tracing=17.6 ssaCreation=4.69 irGeneration=3.61 irPasses.noThrowInference=0.0242 irPasses.exceptionRegionPreparation=0.21 cpp.loweringFromIR=6.23 cpp.compile=593 backend=0.0055 other=0.677"
          },
          {
            "name": "cpp query medical cast filter: execution",
            "value": 1.318,
            "unit": "ms",
            "extra": "medical/MedicalUseCase:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query nexmark scan: compilation",
            "value": 63.705,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:1, 2 pipelines, 5 rounds\npasses tracing=4.13 ssaCreation=1.39 irGeneration=1.2 irPasses.noThrowInference=0.0116 irPasses.exceptionRegionPreparation=0.103 asmjit.compile=5.61 mlir.loweringFromIR=7.32 mlir.pipeline=16.8 llvm.optimizerBuild=0.00752 jit.compile=24.9 backend=1.3 other=1.09"
          },
          {
            "name": "tiered-asmjit-mlir query nexmark scan: execution",
            "value": 51.136,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query nexmark q1 map: compilation",
            "value": 41.432,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:2, 4 pipelines, 5 rounds\npasses tracing=1.46 ssaCreation=0.421 irGeneration=0.498 irPasses.noThrowInference=0.00715 irPasses.exceptionRegionPreparation=0.0635 asmjit.compile=2.53 mlir.loweringFromIR=4.68 mlir.pipeline=9.59 llvm.optimizerBuild=0.0122 jit.compile=19.1 backend=2.29 other=0.778"
          },
          {
            "name": "tiered-asmjit-mlir query nexmark q1 map: execution",
            "value": 22.363,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:2, 4 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query nexmark q2 filter: compilation",
            "value": 69.818,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:3, 2 pipelines, 5 rounds\npasses tracing=5.25 ssaCreation=1.7 irGeneration=1.25 irPasses.noThrowInference=0.0129 irPasses.exceptionRegionPreparation=0.109 asmjit.compile=6.23 mlir.loweringFromIR=8.19 mlir.pipeline=18.3 llvm.optimizerBuild=0.0072 jit.compile=26.2 backend=1.28 other=1.22"
          },
          {
            "name": "tiered-asmjit-mlir query nexmark q2 filter: execution",
            "value": 55.522,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:3, 2 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query nexmark q5 sliding aggregation join: compilation",
            "value": 1252.018,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:4, 24 pipelines, 5 rounds\npasses tracing=54.5 ssaCreation=14.6 irGeneration=12.1 irPasses.noThrowInference=0.0905 irPasses.exceptionRegionPreparation=1.17 asmjit.compile=59.4 mlir.loweringFromIR=113 mlir.pipeline=398 llvm.optimizerBuild=0.111 jit.compile=546 backend=16.6 other=11.6"
          },
          {
            "name": "tiered-asmjit-mlir query nexmark q5 sliding aggregation join: execution",
            "value": 422.426,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:4, 24 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query nexmark q8 tumbling join: compilation",
            "value": 2156.032,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:5, 6 pipelines, 5 rounds\npasses tracing=121 ssaCreation=60.5 irGeneration=38 irPasses.noThrowInference=0.272 irPasses.exceptionRegionPreparation=4.66 asmjit.compile=278 mlir.loweringFromIR=348 mlir.pipeline=716 llvm.optimizerBuild=0.0348 jit.compile=534 backend=5.8 other=45.9"
          },
          {
            "name": "tiered-asmjit-mlir query nexmark q8 tumbling join: execution",
            "value": 1135.564,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark:5, 6 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query nexmark q8 tumbling join varsized: compilation",
            "value": 1794.944,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark_with_varsized:6, 6 pipelines, 5 rounds\npasses tracing=104 ssaCreation=48.9 irGeneration=31.5 irPasses.noThrowInference=0.227 irPasses.exceptionRegionPreparation=4.11 asmjit.compile=228 mlir.loweringFromIR=287 mlir.pipeline=573 llvm.optimizerBuild=0.0358 jit.compile=465 backend=5.23 other=35.5"
          },
          {
            "name": "tiered-asmjit-mlir query nexmark q8 tumbling join varsized: execution",
            "value": 931.62,
            "unit": "ms",
            "extra": "benchmark_small/Nexmark_with_varsized:6, 6 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query ysb filter group sum: compilation",
            "value": 116.112,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark:1, 4 pipelines, 5 rounds\npasses tracing=10.3 ssaCreation=1.9 irGeneration=1.68 irPasses.noThrowInference=0.0154 irPasses.exceptionRegionPreparation=0.0902 asmjit.compile=6.79 mlir.loweringFromIR=10.3 mlir.pipeline=23.6 llvm.optimizerBuild=0.0135 jit.compile=57.1 backend=2.44 other=1.67"
          },
          {
            "name": "tiered-asmjit-mlir query ysb filter group sum: execution",
            "value": 90.54,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark:1, 4 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query ysb varsized filter group count: compilation",
            "value": 121.285,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark_with_varsized:1, 4 pipelines, 5 rounds\npasses tracing=11 ssaCreation=2.5 irGeneration=1.88 irPasses.noThrowInference=0.0197 irPasses.exceptionRegionPreparation=0.0946 asmjit.compile=7.24 mlir.loweringFromIR=11.3 mlir.pipeline=26.1 llvm.optimizerBuild=0.0134 jit.compile=57.5 backend=2.41 other=1.76"
          },
          {
            "name": "tiered-asmjit-mlir query ysb varsized filter group count: execution",
            "value": 94.577,
            "unit": "ms",
            "extra": "benchmark_small/YahooStreamingBenchmark_with_varsized:1, 4 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query lrb sliding group avg: compilation",
            "value": 179.427,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:1, 6 pipelines, 5 rounds\npasses tracing=8.09 ssaCreation=2.74 irGeneration=2.01 irPasses.noThrowInference=0.0197 irPasses.exceptionRegionPreparation=0.175 asmjit.compile=9.46 mlir.loweringFromIR=16.5 mlir.pipeline=49.1 llvm.optimizerBuild=0.0248 jit.compile=84 backend=3.69 other=2.31"
          },
          {
            "name": "tiered-asmjit-mlir query lrb sliding group avg: execution",
            "value": 125.981,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:1, 6 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query lrb sliding group count: compilation",
            "value": 169.006,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:2, 6 pipelines, 5 rounds\npasses tracing=7.34 ssaCreation=1.94 irGeneration=1.75 irPasses.noThrowInference=0.0163 irPasses.exceptionRegionPreparation=0.138 asmjit.compile=7.5 mlir.loweringFromIR=13.7 mlir.pipeline=37.8 llvm.optimizerBuild=0.0237 jit.compile=91.8 backend=3.69 other=1.98"
          },
          {
            "name": "tiered-asmjit-mlir query lrb sliding group count: execution",
            "value": 141.921,
            "unit": "ms",
            "extra": "benchmark_small/LinearRoadBenchmark:2, 6 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query cluster monitoring sliding group sum: compilation",
            "value": 120.505,
            "unit": "ms",
            "extra": "benchmark_small/ClusterMonitoring:1, 4 pipelines, 5 rounds\npasses tracing=11.1 ssaCreation=1.89 irGeneration=1.67 irPasses.noThrowInference=0.0144 irPasses.exceptionRegionPreparation=0.0924 asmjit.compile=7.14 mlir.loweringFromIR=12.4 mlir.pipeline=24.1 llvm.optimizerBuild=0.0124 jit.compile=57.5 backend=2.45 other=1.64"
          },
          {
            "name": "tiered-asmjit-mlir query cluster monitoring sliding group sum: execution",
            "value": 93.076,
            "unit": "ms",
            "extra": "benchmark_small/ClusterMonitoring:1, 4 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query manufacturing sliding multi aggregate: compilation",
            "value": 172.872,
            "unit": "ms",
            "extra": "benchmark_small/Manufacturing:1, 4 pipelines, 5 rounds\npasses tracing=14.4 ssaCreation=3.38 irGeneration=2.56 irPasses.noThrowInference=0.0281 irPasses.exceptionRegionPreparation=0.162 asmjit.compile=13.2 mlir.loweringFromIR=18.2 mlir.pipeline=39.3 llvm.optimizerBuild=0.0148 jit.compile=76.3 backend=2.7 other=2.43"
          },
          {
            "name": "tiered-asmjit-mlir query manufacturing sliding multi aggregate: execution",
            "value": 130.59,
            "unit": "ms",
            "extra": "benchmark_small/Manufacturing:1, 4 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query debs range filter: compilation",
            "value": 68.474,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:3, 2 pipelines, 5 rounds\npasses tracing=5.06 ssaCreation=1.72 irGeneration=1.26 irPasses.noThrowInference=0.0111 irPasses.exceptionRegionPreparation=0.099 asmjit.compile=6 mlir.loweringFromIR=8.19 mlir.pipeline=17.9 llvm.optimizerBuild=0.00565 jit.compile=25.9 backend=1.31 other=1.18"
          },
          {
            "name": "tiered-asmjit-mlir query debs range filter: execution",
            "value": 53.584,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:3, 2 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query debs union: compilation",
            "value": 128.461,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:8, 8 pipelines, 5 rounds\npasses tracing=5.66 ssaCreation=1.26 irGeneration=1.32 irPasses.noThrowInference=0.0154 irPasses.exceptionRegionPreparation=0.109 asmjit.compile=7.14 mlir.loweringFromIR=13.2 mlir.pipeline=32.3 llvm.optimizerBuild=0.0298 jit.compile=59.9 backend=5.5 other=2.06"
          },
          {
            "name": "tiered-asmjit-mlir query debs union: execution",
            "value": 94.216,
            "unit": "ms",
            "extra": "benchmark_small/DEBS:8, 8 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query wide arithmetic expression: compilation",
            "value": 34.127,
            "unit": "ms",
            "extra": "operator/projection/Filter_Map_Queries:4, 2 pipelines, 5 rounds\npasses tracing=2.31 ssaCreation=0.603 irGeneration=0.55 irPasses.noThrowInference=0.0066 irPasses.exceptionRegionPreparation=0.0539 asmjit.compile=2.78 mlir.loweringFromIR=3.61 mlir.pipeline=7.93 llvm.optimizerBuild=0.0051 jit.compile=14.5 backend=1.16 other=0.528"
          },
          {
            "name": "tiered-asmjit-mlir query wide arithmetic expression: execution",
            "value": 27.8,
            "unit": "ms",
            "extra": "operator/projection/Filter_Map_Queries:4, 2 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query nested maps: compilation",
            "value": 65.852,
            "unit": "ms",
            "extra": "operator/map/Map_Add:5, 6 pipelines, 5 rounds\npasses tracing=2.39 ssaCreation=0.681 irGeneration=0.708 irPasses.noThrowInference=0.00816 irPasses.exceptionRegionPreparation=0.0928 asmjit.compile=3.87 mlir.loweringFromIR=5.96 mlir.pipeline=16.7 llvm.optimizerBuild=0.021 jit.compile=30.5 backend=3.6 other=1.23"
          },
          {
            "name": "tiered-asmjit-mlir query nested maps: execution",
            "value": 25.127,
            "unit": "ms",
            "extra": "operator/map/Map_Add:5, 6 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query round all integer widths: compilation",
            "value": 73.14,
            "unit": "ms",
            "extra": "function/arithmetical/FunctionRound:1, 2 pipelines, 5 rounds\npasses tracing=8.21 ssaCreation=1.48 irGeneration=1.34 irPasses.noThrowInference=0.014 irPasses.exceptionRegionPreparation=0.118 asmjit.compile=7 mlir.loweringFromIR=8.47 mlir.pipeline=17 llvm.optimizerBuild=0.00597 jit.compile=27.2 backend=1.35 other=1.07"
          },
          {
            "name": "tiered-asmjit-mlir query round all integer widths: execution",
            "value": 55.145,
            "unit": "ms",
            "extra": "function/arithmetical/FunctionRound:1, 2 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query string concat: compilation",
            "value": 48.443,
            "unit": "ms",
            "extra": "operator/selection/SelectionNull:7, 4 pipelines, 5 rounds\npasses tracing=2.03 ssaCreation=0.604 irGeneration=0.591 irPasses.noThrowInference=0.00696 irPasses.exceptionRegionPreparation=0.0754 asmjit.compile=3.27 mlir.loweringFromIR=4.5 mlir.pipeline=11.7 llvm.optimizerBuild=0.012 jit.compile=22.5 backend=2.14 other=0.844"
          },
          {
            "name": "tiered-asmjit-mlir query string concat: execution",
            "value": 22.256,
            "unit": "ms",
            "extra": "operator/selection/SelectionNull:7, 4 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query filter on string concat: compilation",
            "value": 45.632,
            "unit": "ms",
            "extra": "operator/selection/Selection_NotEquals:7, 2 pipelines, 5 rounds\npasses tracing=3.54 ssaCreation=1.12 irGeneration=0.936 irPasses.noThrowInference=0.0091 irPasses.exceptionRegionPreparation=0.053 asmjit.compile=4.17 mlir.loweringFromIR=5.59 mlir.pipeline=11.2 llvm.optimizerBuild=0.00576 jit.compile=17.1 backend=1.25 other=0.708"
          },
          {
            "name": "tiered-asmjit-mlir query filter on string concat: execution",
            "value": 35.884,
            "unit": "ms",
            "extra": "operator/selection/Selection_NotEquals:7, 2 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query varsized in list filter: compilation",
            "value": 40.886,
            "unit": "ms",
            "extra": "parser/VarSizedSupport:3, 2 pipelines, 5 rounds\npasses tracing=2.75 ssaCreation=0.925 irGeneration=0.798 irPasses.noThrowInference=0.00816 irPasses.exceptionRegionPreparation=0.0595 asmjit.compile=3.63 mlir.loweringFromIR=4.77 mlir.pipeline=9.6 llvm.optimizerBuild=0.00511 jit.compile=16.6 backend=1.21 other=0.597"
          },
          {
            "name": "tiered-asmjit-mlir query varsized in list filter: execution",
            "value": 32.838,
            "unit": "ms",
            "extra": "parser/VarSizedSupport:3, 2 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query nested json scan: compilation",
            "value": 36.626,
            "unit": "ms",
            "extra": "formatter/JSON/NestedJSON:4, 2 pipelines, 5 rounds\npasses tracing=1.95 ssaCreation=0.705 irGeneration=0.644 irPasses.noThrowInference=0.00653 irPasses.exceptionRegionPreparation=0.053 asmjit.compile=3.39 mlir.loweringFromIR=4.21 mlir.pipeline=8.64 llvm.optimizerBuild=0.00523 jit.compile=15.1 backend=1.2 other=0.567"
          },
          {
            "name": "tiered-asmjit-mlir query nested json scan: execution",
            "value": 29.52,
            "unit": "ms",
            "extra": "formatter/JSON/NestedJSON:4, 2 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query widest schema scan: compilation",
            "value": 21034.487,
            "unit": "ms",
            "extra": "formatter/MaxNumberOfSupportedFields:1, 2 pipelines, 2 rounds\npasses tracing=1.65e+04 ssaCreation=379 irGeneration=200 irPasses.noThrowInference=0.543 irPasses.exceptionRegionPreparation=3.13 asmjit.compile=887 mlir.loweringFromIR=1.33e+03 mlir.pipeline=701 llvm.optimizerBuild=0.0138 jit.compile=718 backend=6.28 other=283"
          },
          {
            "name": "tiered-asmjit-mlir query widest schema scan: execution",
            "value": 2906.797,
            "unit": "ms",
            "extra": "formatter/MaxNumberOfSupportedFields:1, 2 pipelines, 2 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query tumbling all aggregates: compilation",
            "value": 171.217,
            "unit": "ms",
            "extra": "operator/aggregation/ColumnNames:1, 4 pipelines, 5 rounds\npasses tracing=9.93 ssaCreation=3.7 irGeneration=2.63 irPasses.noThrowInference=0.0218 irPasses.exceptionRegionPreparation=0.19 asmjit.compile=11.7 mlir.loweringFromIR=17.6 mlir.pipeline=40.7 llvm.optimizerBuild=0.0168 jit.compile=79.9 backend=2.39 other=2.47"
          },
          {
            "name": "tiered-asmjit-mlir query tumbling all aggregates: execution",
            "value": 63.836,
            "unit": "ms",
            "extra": "operator/aggregation/ColumnNames:1, 4 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query tumbling median of expression: compilation",
            "value": 155.063,
            "unit": "ms",
            "extra": "operator/aggregation/ExpressionInsideAggregation:3, 6 pipelines, 5 rounds\npasses tracing=5.14 ssaCreation=2.81 irGeneration=1.9 irPasses.noThrowInference=0.0173 irPasses.exceptionRegionPreparation=0.172 asmjit.compile=7.95 mlir.loweringFromIR=16.9 mlir.pipeline=45.5 llvm.optimizerBuild=0.023 jit.compile=68.6 backend=3.73 other=2.17"
          },
          {
            "name": "tiered-asmjit-mlir query tumbling median of expression: execution",
            "value": 73.816,
            "unit": "ms",
            "extra": "operator/aggregation/ExpressionInsideAggregation:3, 6 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query sliding group count: compilation",
            "value": 96.26,
            "unit": "ms",
            "extra": "operator/aggregation/WindowAggregationCount:6, 4 pipelines, 5 rounds\npasses tracing=6.14 ssaCreation=1.74 irGeneration=1.44 irPasses.noThrowInference=0.0165 irPasses.exceptionRegionPreparation=0.0672 asmjit.compile=5.42 mlir.loweringFromIR=8.61 mlir.pipeline=19 llvm.optimizerBuild=0.0125 jit.compile=50.2 backend=2.32 other=1.3"
          },
          {
            "name": "tiered-asmjit-mlir query sliding group count: execution",
            "value": 77.483,
            "unit": "ms",
            "extra": "operator/aggregation/WindowAggregationCount:6, 4 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query aggregation of union: compilation",
            "value": 159.446,
            "unit": "ms",
            "extra": "operator/aggregation/AggregationOfUnion:1, 10 pipelines, 5 rounds\npasses tracing=4.84 ssaCreation=1.59 irGeneration=1.47 irPasses.noThrowInference=0.0162 irPasses.exceptionRegionPreparation=0.159 asmjit.compile=8.02 mlir.loweringFromIR=15.4 mlir.pipeline=37.1 llvm.optimizerBuild=0.0373 jit.compile=80.1 backend=5.75 other=2.35"
          },
          {
            "name": "tiered-asmjit-mlir query aggregation of union: execution",
            "value": 67.251,
            "unit": "ms",
            "extra": "operator/aggregation/AggregationOfUnion:1, 10 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query tumbling left outer join: compilation",
            "value": 1142.505,
            "unit": "ms",
            "extra": "operator/join/LeftOuterJoin:1, 6 pipelines, 5 rounds\npasses tracing=56.3 ssaCreation=24.6 irGeneration=16.8 irPasses.noThrowInference=0.142 irPasses.exceptionRegionPreparation=2.1 asmjit.compile=125 mlir.loweringFromIR=160 mlir.pipeline=357 llvm.optimizerBuild=0.0332 jit.compile=379 backend=4.78 other=16.1"
          },
          {
            "name": "tiered-asmjit-mlir query tumbling left outer join: execution",
            "value": 517.934,
            "unit": "ms",
            "extra": "operator/join/LeftOuterJoin:1, 6 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query tumbling full outer join: compilation",
            "value": 1161.611,
            "unit": "ms",
            "extra": "operator/join/FullOuterJoin:1, 6 pipelines, 5 rounds\npasses tracing=56.7 ssaCreation=24.8 irGeneration=16.9 irPasses.noThrowInference=0.144 irPasses.exceptionRegionPreparation=2.11 asmjit.compile=124 mlir.loweringFromIR=161 mlir.pipeline=364 llvm.optimizerBuild=0.035 jit.compile=387 backend=5.22 other=16.7"
          },
          {
            "name": "tiered-asmjit-mlir query tumbling full outer join: execution",
            "value": 518.372,
            "unit": "ms",
            "extra": "operator/join/FullOuterJoin:1, 6 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query sliding right outer join: compilation",
            "value": 1125.31,
            "unit": "ms",
            "extra": "operator/join/SlidingWindowRightOuterJoin:1, 6 pipelines, 5 rounds\npasses tracing=56.5 ssaCreation=24.4 irGeneration=17.1 irPasses.noThrowInference=0.147 irPasses.exceptionRegionPreparation=2.18 asmjit.compile=122 mlir.loweringFromIR=159 mlir.pipeline=341 llvm.optimizerBuild=0.0343 jit.compile=377 backend=4.88 other=17.3"
          },
          {
            "name": "tiered-asmjit-mlir query sliding right outer join: execution",
            "value": 518.225,
            "unit": "ms",
            "extra": "operator/join/SlidingWindowRightOuterJoin:1, 6 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query sliding inner join: compilation",
            "value": 660.342,
            "unit": "ms",
            "extra": "guide/guide12:1, 8 pipelines, 5 rounds\npasses tracing=34.6 ssaCreation=9.9 irGeneration=7.5 irPasses.noThrowInference=0.0804 irPasses.exceptionRegionPreparation=1.01 asmjit.compile=46.1 mlir.loweringFromIR=69.9 mlir.pipeline=196 llvm.optimizerBuild=0.0414 jit.compile=275 backend=4.83 other=8.17"
          },
          {
            "name": "tiered-asmjit-mlir query sliding inner join: execution",
            "value": 269.784,
            "unit": "ms",
            "extra": "guide/guide12:1, 8 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query nested join quoted identifiers: compilation",
            "value": 678.254,
            "unit": "ms",
            "extra": "datatype/QuotedIdentifiers:5, 10 pipelines, 5 rounds\npasses tracing=40.4 ssaCreation=10 irGeneration=7.04 irPasses.noThrowInference=0.0801 irPasses.exceptionRegionPreparation=1.04 asmjit.compile=45.8 mlir.loweringFromIR=70.6 mlir.pipeline=206 llvm.optimizerBuild=0.0517 jit.compile=282 backend=5.67 other=8.65"
          },
          {
            "name": "tiered-asmjit-mlir query nested join quoted identifiers: execution",
            "value": 218.924,
            "unit": "ms",
            "extra": "datatype/QuotedIdentifiers:5, 10 pipelines, 5 rounds"
          },
          {
            "name": "tiered-asmjit-mlir query medical cast filter: compilation",
            "value": 173.958,
            "unit": "ms",
            "extra": "medical/MedicalUseCase:1, 4 pipelines, 5 rounds\npasses tracing=17.4 ssaCreation=4.64 irGeneration=3.57 irPasses.noThrowInference=0.0277 irPasses.exceptionRegionPreparation=0.214 asmjit.compile=13.5 mlir.loweringFromIR=21.1 mlir.pipeline=45.4 llvm.optimizerBuild=0.0179 jit.compile=62.4 backend=2.44 other=3.25"
          },
          {
            "name": "tiered-asmjit-mlir query medical cast filter: execution",
            "value": 63.458,
            "unit": "ms",
            "extra": "medical/MedicalUseCase:1, 4 pipelines, 5 rounds"
          },
          {
            "name": "mlir suite compilation: total",
            "value": 169510.572,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "mlir suite compilation: median per pipeline",
            "value": 53.198,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "mlir suite compilation phase: backend.totalMs",
            "value": 140820.252,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "mlir suite compilation phase: frontend.totalMs",
            "value": 26681.671,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "mlir suite compilation phase: irGeneration.ms",
            "value": 1263.205,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "mlir suite compilation phase: irPasses.exceptionRegionPreparation.ms",
            "value": 104.183,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "mlir suite compilation phase: irPasses.noThrowInference.ms",
            "value": 12.719,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "mlir suite compilation phase: irPasses.totalMs",
            "value": 119.812,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "mlir suite compilation phase: jit.compile.ms",
            "value": 108590.65,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "mlir suite compilation phase: llvm.optimizerBuild.ms",
            "value": 6.161,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "mlir suite compilation phase: mlir.loweringFromIR.ms",
            "value": 9068.268,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "mlir suite compilation phase: mlir.pipeline.ms",
            "value": 22394.431,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "mlir suite compilation phase: ssaCreation.ms",
            "value": 2237.532,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "mlir suite compilation phase: tracing.ms",
            "value": 23023.934,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "mlir suite generated code: ir.operations",
            "value": 2116757,
            "unit": "ops",
            "extra": "1044 pipelines"
          },
          {
            "name": "mlir suite execution: geometric mean per query",
            "value": 0.813,
            "unit": "ms",
            "extra": "570 queries"
          },
          {
            "name": "mlir suite execution: benchmark_small/ geometric mean",
            "value": 4.189,
            "unit": "ms",
            "extra": "31 queries"
          },
          {
            "name": "mlir suite execution: bug/ geometric mean",
            "value": 0.475,
            "unit": "ms",
            "extra": "5 queries"
          },
          {
            "name": "mlir suite execution: datatype/ geometric mean",
            "value": 0.607,
            "unit": "ms",
            "extra": "18 queries"
          },
          {
            "name": "mlir suite execution: formatter/ geometric mean",
            "value": 0.609,
            "unit": "ms",
            "extra": "58 queries"
          },
          {
            "name": "mlir suite execution: function/ geometric mean",
            "value": 0.521,
            "unit": "ms",
            "extra": "172 queries"
          },
          {
            "name": "mlir suite execution: guide/ geometric mean",
            "value": 0.701,
            "unit": "ms",
            "extra": "14 queries"
          },
          {
            "name": "mlir suite execution: inference/ geometric mean",
            "value": 0.829,
            "unit": "ms",
            "extra": "4 queries"
          },
          {
            "name": "mlir suite execution: medical/ geometric mean",
            "value": 0.812,
            "unit": "ms",
            "extra": "33 queries"
          },
          {
            "name": "mlir suite execution: operator/ geometric mean",
            "value": 0.753,
            "unit": "ms",
            "extra": "162 queries"
          },
          {
            "name": "mlir suite execution: parser/ geometric mean",
            "value": 0.637,
            "unit": "ms",
            "extra": "20 queries"
          },
          {
            "name": "mlir suite execution: regression/ geometric mean",
            "value": 0.866,
            "unit": "ms",
            "extra": "11 queries"
          },
          {
            "name": "mlir suite execution: sinks/ geometric mean",
            "value": 0.453,
            "unit": "ms",
            "extra": "5 queries"
          },
          {
            "name": "mlir suite execution: sources/ geometric mean",
            "value": 7.589,
            "unit": "ms",
            "extra": "33 queries"
          },
          {
            "name": "mlir suite execution: tuples/ geometric mean",
            "value": 0.482,
            "unit": "ms",
            "extra": "4 queries"
          },
          {
            "name": "asmjit suite compilation: total",
            "value": 36569.123,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "asmjit suite compilation: median per pipeline",
            "value": 4.273,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "asmjit suite compilation phase: asmjit.compile.ms",
            "value": 10628.566,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "asmjit suite compilation phase: backend.totalMs",
            "value": 10630.465,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "asmjit suite compilation phase: frontend.totalMs",
            "value": 25674.993,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "asmjit suite compilation phase: irGeneration.ms",
            "value": 1263.639,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "asmjit suite compilation phase: irPasses.exceptionRegionPreparation.ms",
            "value": 101.722,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "asmjit suite compilation phase: irPasses.noThrowInference.ms",
            "value": 12.476,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "asmjit suite compilation phase: irPasses.totalMs",
            "value": 116.656,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "asmjit suite compilation phase: ssaCreation.ms",
            "value": 2277.984,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "asmjit suite compilation phase: tracing.ms",
            "value": 21977.82,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "asmjit suite generated code: ir.operations",
            "value": 2116757,
            "unit": "ops",
            "extra": "1044 pipelines"
          },
          {
            "name": "asmjit suite execution: geometric mean per query",
            "value": 0.454,
            "unit": "ms",
            "extra": "570 queries"
          },
          {
            "name": "asmjit suite execution: benchmark_small/ geometric mean",
            "value": 4.12,
            "unit": "ms",
            "extra": "31 queries"
          },
          {
            "name": "asmjit suite execution: bug/ geometric mean",
            "value": 0.159,
            "unit": "ms",
            "extra": "5 queries"
          },
          {
            "name": "asmjit suite execution: datatype/ geometric mean",
            "value": 0.295,
            "unit": "ms",
            "extra": "18 queries"
          },
          {
            "name": "asmjit suite execution: formatter/ geometric mean",
            "value": 0.448,
            "unit": "ms",
            "extra": "58 queries"
          },
          {
            "name": "asmjit suite execution: function/ geometric mean",
            "value": 0.31,
            "unit": "ms",
            "extra": "172 queries"
          },
          {
            "name": "asmjit suite execution: guide/ geometric mean",
            "value": 0.342,
            "unit": "ms",
            "extra": "14 queries"
          },
          {
            "name": "asmjit suite execution: inference/ geometric mean",
            "value": 0.538,
            "unit": "ms",
            "extra": "4 queries"
          },
          {
            "name": "asmjit suite execution: medical/ geometric mean",
            "value": 0.581,
            "unit": "ms",
            "extra": "33 queries"
          },
          {
            "name": "asmjit suite execution: operator/ geometric mean",
            "value": 0.328,
            "unit": "ms",
            "extra": "162 queries"
          },
          {
            "name": "asmjit suite execution: parser/ geometric mean",
            "value": 0.256,
            "unit": "ms",
            "extra": "20 queries"
          },
          {
            "name": "asmjit suite execution: regression/ geometric mean",
            "value": 0.515,
            "unit": "ms",
            "extra": "11 queries"
          },
          {
            "name": "asmjit suite execution: sinks/ geometric mean",
            "value": 0.237,
            "unit": "ms",
            "extra": "5 queries"
          },
          {
            "name": "asmjit suite execution: sources/ geometric mean",
            "value": 4.443,
            "unit": "ms",
            "extra": "33 queries"
          },
          {
            "name": "asmjit suite execution: tuples/ geometric mean",
            "value": 0.259,
            "unit": "ms",
            "extra": "4 queries"
          },
          {
            "name": "tbc suite compilation: total",
            "value": 30598.487,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "tbc suite compilation: median per pipeline",
            "value": 3.746,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "tbc suite compilation phase: backend.totalMs",
            "value": 4445.506,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "tbc suite compilation phase: frontend.totalMs",
            "value": 25895.115,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "tbc suite compilation phase: irGeneration.ms",
            "value": 1272.212,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "tbc suite compilation phase: irPasses.exceptionRegionPreparation.ms",
            "value": 103.418,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "tbc suite compilation phase: irPasses.noThrowInference.ms",
            "value": 13.403,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "tbc suite compilation phase: irPasses.totalMs",
            "value": 119.388,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "tbc suite compilation phase: ssaCreation.ms",
            "value": 2255.202,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "tbc suite compilation phase: tracing.ms",
            "value": 22207.596,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "tbc suite generated code: ir.operations",
            "value": 2116757,
            "unit": "ops",
            "extra": "1044 pipelines"
          },
          {
            "name": "tbc suite execution: geometric mean per query",
            "value": 0.552,
            "unit": "ms",
            "extra": "570 queries"
          },
          {
            "name": "tbc suite execution: benchmark_small/ geometric mean",
            "value": 10.511,
            "unit": "ms",
            "extra": "31 queries"
          },
          {
            "name": "tbc suite execution: bug/ geometric mean",
            "value": 0.199,
            "unit": "ms",
            "extra": "5 queries"
          },
          {
            "name": "tbc suite execution: datatype/ geometric mean",
            "value": 0.316,
            "unit": "ms",
            "extra": "18 queries"
          },
          {
            "name": "tbc suite execution: formatter/ geometric mean",
            "value": 0.542,
            "unit": "ms",
            "extra": "58 queries"
          },
          {
            "name": "tbc suite execution: function/ geometric mean",
            "value": 0.326,
            "unit": "ms",
            "extra": "172 queries"
          },
          {
            "name": "tbc suite execution: guide/ geometric mean",
            "value": 0.406,
            "unit": "ms",
            "extra": "14 queries"
          },
          {
            "name": "tbc suite execution: inference/ geometric mean",
            "value": 0.564,
            "unit": "ms",
            "extra": "4 queries"
          },
          {
            "name": "tbc suite execution: medical/ geometric mean",
            "value": 0.868,
            "unit": "ms",
            "extra": "33 queries"
          },
          {
            "name": "tbc suite execution: operator/ geometric mean",
            "value": 0.413,
            "unit": "ms",
            "extra": "162 queries"
          },
          {
            "name": "tbc suite execution: parser/ geometric mean",
            "value": 0.279,
            "unit": "ms",
            "extra": "20 queries"
          },
          {
            "name": "tbc suite execution: regression/ geometric mean",
            "value": 0.613,
            "unit": "ms",
            "extra": "11 queries"
          },
          {
            "name": "tbc suite execution: sinks/ geometric mean",
            "value": 0.22,
            "unit": "ms",
            "extra": "5 queries"
          },
          {
            "name": "tbc suite execution: sources/ geometric mean",
            "value": 4.795,
            "unit": "ms",
            "extra": "33 queries"
          },
          {
            "name": "tbc suite execution: tuples/ geometric mean",
            "value": 0.323,
            "unit": "ms",
            "extra": "4 queries"
          },
          {
            "name": "cpp suite compilation: total",
            "value": 322761.321,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "cpp suite compilation: median per pipeline",
            "value": 180.982,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "cpp suite compilation phase: backend.totalMs",
            "value": 297132.044,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "cpp suite compilation phase: cpp.compile.ms",
            "value": 294664.832,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "cpp suite compilation phase: cpp.loweringFromIR.ms",
            "value": 2464.691,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "cpp suite compilation phase: frontend.totalMs",
            "value": 25378.686,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "cpp suite compilation phase: irGeneration.ms",
            "value": 1228.256,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "cpp suite compilation phase: irPasses.exceptionRegionPreparation.ms",
            "value": 94.735,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "cpp suite compilation phase: irPasses.noThrowInference.ms",
            "value": 11.379,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "cpp suite compilation phase: irPasses.totalMs",
            "value": 109.065,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "cpp suite compilation phase: ssaCreation.ms",
            "value": 2057.928,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "cpp suite compilation phase: tracing.ms",
            "value": 21947.439,
            "unit": "ms",
            "extra": "1044 pipelines"
          },
          {
            "name": "cpp suite generated code: ir.operations",
            "value": 2116757,
            "unit": "ops",
            "extra": "1044 pipelines"
          },
          {
            "name": "cpp suite execution: geometric mean per query",
            "value": 0.74,
            "unit": "ms",
            "extra": "570 queries"
          },
          {
            "name": "cpp suite execution: benchmark_small/ geometric mean",
            "value": 6.605,
            "unit": "ms",
            "extra": "31 queries"
          },
          {
            "name": "cpp suite execution: bug/ geometric mean",
            "value": 0.405,
            "unit": "ms",
            "extra": "5 queries"
          },
          {
            "name": "cpp suite execution: datatype/ geometric mean",
            "value": 0.532,
            "unit": "ms",
            "extra": "18 queries"
          },
          {
            "name": "cpp suite execution: formatter/ geometric mean",
            "value": 0.593,
            "unit": "ms",
            "extra": "58 queries"
          },
          {
            "name": "cpp suite execution: function/ geometric mean",
            "value": 0.476,
            "unit": "ms",
            "extra": "172 queries"
          },
          {
            "name": "cpp suite execution: guide/ geometric mean",
            "value": 0.573,
            "unit": "ms",
            "extra": "14 queries"
          },
          {
            "name": "cpp suite execution: inference/ geometric mean",
            "value": 0.674,
            "unit": "ms",
            "extra": "4 queries"
          },
          {
            "name": "cpp suite execution: medical/ geometric mean",
            "value": 0.807,
            "unit": "ms",
            "extra": "33 queries"
          },
          {
            "name": "cpp suite execution: operator/ geometric mean",
            "value": 0.618,
            "unit": "ms",
            "extra": "162 queries"
          },
          {
            "name": "cpp suite execution: parser/ geometric mean",
            "value": 0.502,
            "unit": "ms",
            "extra": "20 queries"
          },
          {
            "name": "cpp suite execution: regression/ geometric mean",
            "value": 0.797,
            "unit": "ms",
            "extra": "11 queries"
          },
          {
            "name": "cpp suite execution: sinks/ geometric mean",
            "value": 0.36,
            "unit": "ms",
            "extra": "5 queries"
          },
          {
            "name": "cpp suite execution: sources/ geometric mean",
            "value": 6.793,
            "unit": "ms",
            "extra": "33 queries"
          },
          {
            "name": "cpp suite execution: tuples/ geometric mean",
            "value": 0.438,
            "unit": "ms",
            "extra": "4 queries"
          }
        ]
      }
    ]
  }
}