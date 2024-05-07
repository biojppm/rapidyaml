```
-*- mode: compilation; default-directory: "~/proj/rapidyaml/bm/" -*-
Compilation started at Tue May  7 21:10:02

set -xe ; cd ~/proj/rapidyaml ; export C4_EXTERN_DIR=`pwd`/build/extern ; cmany b ryml-bm-run -j1 -t 'Release: -X " -march=native" ' -c c++ -V C4_CXX_STANDARD=17 -V RYML_DBG=OFF  -V RYML_DEV=ON -V RYML_VALGRIND=OFF
+ cd /proj/rapidyaml
++ pwd
+ export C4_EXTERN_DIR=/proj/rapidyaml/build/extern
+ C4_EXTERN_DIR=/proj/rapidyaml/build/extern
+ cmany b ryml-bm-run -j1 -t 'Release: -X " -march=native" ' -c c++ -V C4_CXX_STANDARD=17 -V RYML_DBG=OFF -V RYML_DEV=ON -V RYML_VALGRIND=OFF
-----------------------------------------------
Build linux-x86_64-gxx13.2-Release
-----------------------------------------------
Entering directory /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release (was in /proj/rapidyaml)
--------
$ cd /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release && make -j 1 ryml-bm-run
--------
[  3%] Built target jsoncpp_static
[ 12%] Built target ryml
[ 20%] Built target c4fs
[ 31%] Built target benchmark
[ 37%] Built target yaml
[ 51%] Built target fyaml
[ 70%] Built target yaml-cpp
[ 72%] Built target ryml-bm-emit
[ 74%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_seqs_flow_outer1000_inner100: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100.json;/proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner100.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100.json
-----------------------------------
running case: EMIT/style_seqs_flow_outer1000_inner100.yml
file: /proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner100.yml
-----------------------------------
2024-05-07T21:10:02+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.38, 1.30, 1.27
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve     2744259 ns      2739209 ns          241 bytes_per_second=136.826Mi/s items_per_second=365.069/s
bm_ryml_str             2714979 ns      2710581 ns          256 bytes_per_second=138.271Mi/s items_per_second=368.925/s
bm_ryml_ostream        16798568 ns     16776012 ns          108 bytes_per_second=22.3411Mi/s items_per_second=59.6089/s
bm_fyaml_str_reserve   11291781 ns     11275023 ns           54 bytes_per_second=33.2412Mi/s items_per_second=88.6916/s
bm_fyaml_str           11120864 ns     11108480 ns           55 bytes_per_second=33.7395Mi/s items_per_second=90.0213/s
bm_fyaml_ostream       17423379 ns     17401631 ns           43 bytes_per_second=21.5379Mi/s items_per_second=57.4659/s
bm_yamlcpp             90292289 ns     90176454 ns            8 bytes_per_second=4.15624Mi/s items_per_second=11.0894/s
[ 74%] Built target ryml-bm-emit-style_seqs_flow_outer1000_inner100
[ 75%] Built target ryml-bm-parse
[ 75%] Built target ryml-bm-build
[ 75%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-appveyor: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-appveyor.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-appveyor.json;/proj/rapidyaml/bm/cases/appveyor.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-appveyor.json
-----------------------------------
running case: PARSE/appveyor.yml
file: /proj/rapidyaml/bm/cases/appveyor.yml
-----------------------------------
2024-05-07T21:10:10+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.32, 1.29, 1.26
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse       12053 ns        12046 ns        55399 bytes_per_second=168.628Mi/s items_per_second=83.014k/s
bm_ryml_arena_reuse         12391 ns        12346 ns        57329 bytes_per_second=164.532Mi/s items_per_second=80.9975k/s
bm_ryml_inplace             13466 ns        13482 ns        52476 bytes_per_second=150.672Mi/s items_per_second=74.1743k/s
bm_ryml_arena               13171 ns        13160 ns        53172 bytes_per_second=154.357Mi/s items_per_second=75.9882k/s
bm_libyaml_arena            32162 ns        32134 ns        22041 bytes_per_second=63.215Mi/s items_per_second=31.1201k/s
bm_libyaml_arena_reuse      32187 ns        32196 ns        22128 bytes_per_second=63.0923Mi/s items_per_second=31.0596k/s
bm_libfyaml_arena           48605 ns        48562 ns        14078 bytes_per_second=41.8295Mi/s items_per_second=20.5922k/s
bm_yamlcpp_arena           219941 ns       219743 ns         3168 bytes_per_second=9.24411Mi/s items_per_second=4.55078k/s
[ 75%] Built target ryml-bm-parse-appveyor
[ 75%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-appveyor: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-appveyor.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-appveyor.json;/proj/rapidyaml/bm/cases/appveyor.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-appveyor.json
-----------------------------------
running case: EMIT/appveyor.yml
file: /proj/rapidyaml/bm/cases/appveyor.yml
-----------------------------------
2024-05-07T21:10:18+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.66, 1.36, 1.29
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve        6523 ns         6517 ns        98615 bytes_per_second=311.718Mi/s items_per_second=153.455k/s
bm_ryml_str                6693 ns         6685 ns       109483 bytes_per_second=303.847Mi/s items_per_second=149.581k/s
bm_ryml_ostream          539946 ns       538612 ns         3475 bytes_per_second=3.77141Mi/s items_per_second=1.85663k/s
bm_fyaml_str_reserve      28817 ns        28797 ns        24371 bytes_per_second=70.5384Mi/s items_per_second=34.7253k/s
bm_fyaml_str              28815 ns        28795 ns        24319 bytes_per_second=70.5453Mi/s items_per_second=34.7287k/s
bm_fyaml_ostream        1287595 ns      1284361 ns         9378 bytes_per_second=1.58158Mi/s items_per_second=778.597/s
bm_yamlcpp               365285 ns       364576 ns         3012 bytes_per_second=5.57175Mi/s items_per_second=2.74291k/s
[ 75%] Built target ryml-bm-emit-appveyor
[ 75%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-compile_commands: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-compile_commands.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-compile_commands.json;/proj/rapidyaml/bm/cases/compile_commands.json;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-compile_commands.json
-----------------------------------
running case: PARSE/compile_commands.json
file: /proj/rapidyaml/bm/cases/compile_commands.json
-----------------------------------
2024-05-07T21:10:37+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.86, 1.43, 1.31
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse       70913 ns        70865 ns         9837 bytes_per_second=630.17Mi/s items_per_second=14.1114k/s
bm_ryml_arena_reuse         71669 ns        71627 ns         9765 bytes_per_second=623.46Mi/s items_per_second=13.9611k/s
bm_ryml_inplace             90684 ns        90634 ns         7714 bytes_per_second=492.718Mi/s items_per_second=11.0334k/s
bm_ryml_arena               91344 ns        91278 ns         7675 bytes_per_second=489.238Mi/s items_per_second=10.9555k/s
bm_libyaml_arena           392679 ns       391592 ns         1813 bytes_per_second=114.039Mi/s items_per_second=2.55368k/s
bm_libyaml_arena_reuse     383749 ns       383490 ns         1821 bytes_per_second=116.448Mi/s items_per_second=2.60763k/s
bm_libfyaml_arena          315449 ns       315201 ns         2224 bytes_per_second=141.677Mi/s items_per_second=3.17258k/s
bm_yamlcpp_arena          2381518 ns      2379488 ns          294 bytes_per_second=18.7674Mi/s items_per_second=420.259/s
bm_rapidjson_arena          69976 ns        69926 ns         9993 bytes_per_second=638.628Mi/s items_per_second=14.3008k/s
bm_rapidjson_inplace        27359 ns        27344 ns        25607 bytes_per_second=1.59488Gi/s items_per_second=36.5712k/s
bm_sajson_arena             80111 ns        80053 ns         8732 bytes_per_second=557.843Mi/s items_per_second=12.4918k/s
bm_sajson_inplace           79574 ns        79523 ns         8788 bytes_per_second=561.556Mi/s items_per_second=12.575k/s
bm_jsoncpp_arena           165919 ns       165799 ns         4205 bytes_per_second=269.342Mi/s items_per_second=6.03139k/s
bm_nlohmann_arena          217286 ns       217120 ns         3237 bytes_per_second=205.677Mi/s items_per_second=4.60574k/s
[ 75%] Built target ryml-bm-parse-compile_commands
[ 75%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-compile_commands: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-compile_commands.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-compile_commands.json;/proj/rapidyaml/bm/cases/compile_commands.json;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-compile_commands.json
-----------------------------------
running case: EMIT/compile_commands.json
file: /proj/rapidyaml/bm/cases/compile_commands.json
-----------------------------------
2024-05-07T21:10:50+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.67, 1.40, 1.30
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve      102922 ns       102847 ns         6801 bytes_per_second=434.206Mi/s items_per_second=9.72318k/s
bm_ryml_str              102930 ns       102857 ns         6803 bytes_per_second=434.162Mi/s items_per_second=9.7222k/s
bm_ryml_ostream         6027322 ns      6017947 ns         1046 bytes_per_second=7.4206Mi/s items_per_second=166.17/s
bm_fyaml_str_reserve     108551 ns       108472 ns         6395 bytes_per_second=411.691Mi/s items_per_second=9.21901k/s
bm_fyaml_str             108619 ns       108539 ns         6416 bytes_per_second=411.433Mi/s items_per_second=9.21324k/s
bm_fyaml_ostream        4155710 ns      4148666 ns         1000 bytes_per_second=10.7641Mi/s items_per_second=241.041/s
bm_yamlcpp              7314775 ns      7306856 ns           98 bytes_per_second=6.11162Mi/s items_per_second=136.858/s
bm_rapidjson              67142 ns        67065 ns         9200 bytes_per_second=665.874Mi/s items_per_second=14.9109k/s
bm_jsoncpp              4196009 ns      4187862 ns         1000 bytes_per_second=10.6634Mi/s items_per_second=238.785/s
bm_nlohmann             4032560 ns      4025199 ns         1000 bytes_per_second=11.0943Mi/s items_per_second=248.435/s
[ 75%] Built target ryml-bm-emit-compile_commands
[ 77%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-travis: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-travis.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-travis.json;/proj/rapidyaml/bm/cases/travis.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-travis.json
-----------------------------------
running case: PARSE/travis.yml
file: /proj/rapidyaml/bm/cases/travis.yml
-----------------------------------
2024-05-07T21:11:13+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.84, 1.45, 1.32
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse       20046 ns        20031 ns        34799 bytes_per_second=193.674Mi/s items_per_second=49.9217k/s
bm_ryml_arena_reuse         20107 ns        20098 ns        34933 bytes_per_second=193.035Mi/s items_per_second=49.7572k/s
bm_ryml_inplace             23718 ns        23643 ns        29557 bytes_per_second=164.089Mi/s items_per_second=42.2959k/s
bm_ryml_arena               23325 ns        23305 ns        29958 bytes_per_second=166.467Mi/s items_per_second=42.9088k/s
bm_libyaml_arena            52241 ns        52201 ns        13426 bytes_per_second=74.3199Mi/s items_per_second=19.1568k/s
bm_libyaml_arena_reuse      52905 ns        52882 ns        13414 bytes_per_second=73.3629Mi/s items_per_second=18.9102k/s
bm_libfyaml_arena           71287 ns        71212 ns         9847 bytes_per_second=54.4787Mi/s items_per_second=14.0425k/s
bm_yamlcpp_arena           392885 ns       392356 ns         1826 bytes_per_second=9.88782Mi/s items_per_second=2.5487k/s
[ 77%] Built target ryml-bm-parse-travis
[ 77%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-travis: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-travis.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-travis.json;/proj/rapidyaml/bm/cases/travis.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-travis.json
-----------------------------------
running case: EMIT/travis.yml
file: /proj/rapidyaml/bm/cases/travis.yml
-----------------------------------
2024-05-07T21:11:21+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.93, 1.48, 1.33
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve       11651 ns        11639 ns        60750 bytes_per_second=333.322Mi/s items_per_second=85.9176k/s
bm_ryml_str               11538 ns        11526 ns        59268 bytes_per_second=336.579Mi/s items_per_second=86.7572k/s
bm_ryml_ostream          317597 ns       316763 ns         1680 bytes_per_second=12.2475Mi/s items_per_second=3.15693k/s
bm_fyaml_str_reserve      66104 ns        66044 ns        10728 bytes_per_second=58.7417Mi/s items_per_second=15.1414k/s
bm_fyaml_str              68397 ns        68316 ns        10729 bytes_per_second=56.7884Mi/s items_per_second=14.6379k/s
bm_fyaml_ostream         977437 ns       973452 ns         4151 bytes_per_second=3.98535Mi/s items_per_second=1.02727k/s
bm_yamlcpp               513578 ns       512793 ns         1532 bytes_per_second=7.56553Mi/s items_per_second=1.95011k/s
[ 77%] Built target ryml-bm-emit-travis
[ 77%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_dquot_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_multiline.json;/proj/rapidyaml/bm/cases/scalar_dquot_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_multiline.json
-----------------------------------
running case: PARSE/scalar_dquot_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_dquot_multiline.yml
-----------------------------------
2024-05-07T21:11:31+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.87, 1.48, 1.33
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse      306464 ns       306225 ns         2275 bytes_per_second=224.796Mi/s items_per_second=3.26557k/s
bm_ryml_arena_reuse        318470 ns       318157 ns         2276 bytes_per_second=216.365Mi/s items_per_second=3.14311k/s
bm_ryml_inplace            348852 ns       348497 ns         2069 bytes_per_second=197.529Mi/s items_per_second=2.86946k/s
bm_ryml_arena              362208 ns       360670 ns         1877 bytes_per_second=190.862Mi/s items_per_second=2.77262k/s
bm_libyaml_arena           523817 ns       523342 ns         1345 bytes_per_second=131.536Mi/s items_per_second=1.9108k/s
bm_libyaml_arena_reuse     518105 ns       517732 ns         1352 bytes_per_second=132.961Mi/s items_per_second=1.9315k/s
bm_libfyaml_arena          423116 ns       422728 ns         1608 bytes_per_second=162.842Mi/s items_per_second=2.36559k/s
bm_yamlcpp_arena          3418213 ns      3395326 ns          212 bytes_per_second=20.2744Mi/s items_per_second=294.523/s
[ 77%] Built target ryml-bm-parse-scalar_dquot_multiline
[ 77%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_dquot_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_multiline.json;/proj/rapidyaml/bm/cases/scalar_dquot_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_multiline.json
-----------------------------------
running case: EMIT/scalar_dquot_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_dquot_multiline.yml
-----------------------------------
2024-05-07T21:11:39+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.73, 1.46, 1.33
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve       79322 ns        79252 ns         8818 bytes_per_second=868.6Mi/s items_per_second=12.618k/s
bm_ryml_str               79097 ns        79031 ns         8849 bytes_per_second=871.026Mi/s items_per_second=12.6532k/s
bm_ryml_ostream         7360140 ns      7348190 ns          826 bytes_per_second=9.36804Mi/s items_per_second=136.088/s
bm_fyaml_str_reserve    1596307 ns      1594844 ns          436 bytes_per_second=43.1629Mi/s items_per_second=627.021/s
bm_fyaml_str            1595253 ns      1594000 ns          435 bytes_per_second=43.1858Mi/s items_per_second=627.352/s
bm_fyaml_ostream        2707803 ns      2701930 ns          365 bytes_per_second=25.4774Mi/s items_per_second=370.106/s
bm_yamlcpp              8241043 ns      8226785 ns           90 bytes_per_second=8.36756Mi/s items_per_second=121.554/s
[ 77%] Built target ryml-bm-emit-scalar_dquot_multiline
[ 77%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_dquot_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_singleline.json;/proj/rapidyaml/bm/cases/scalar_dquot_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_singleline.json
-----------------------------------
running case: PARSE/scalar_dquot_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_dquot_singleline.yml
-----------------------------------
2024-05-07T21:11:50+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.62, 1.45, 1.33
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse       13603 ns        13538 ns        51921 bytes_per_second=339.889Mi/s items_per_second=73.8652k/s
bm_ryml_arena_reuse         13522 ns        13521 ns        51684 bytes_per_second=340.31Mi/s items_per_second=73.9567k/s
bm_ryml_inplace             16074 ns        16070 ns        43501 bytes_per_second=286.335Mi/s items_per_second=62.2268k/s
bm_ryml_arena               15896 ns        15883 ns        43493 bytes_per_second=289.706Mi/s items_per_second=62.9594k/s
bm_libyaml_arena            63911 ns        63862 ns        10962 bytes_per_second=72.0538Mi/s items_per_second=15.6588k/s
bm_libyaml_arena_reuse      63177 ns        63136 ns        11074 bytes_per_second=72.8823Mi/s items_per_second=15.8389k/s
bm_libfyaml_arena           82805 ns        82735 ns         8422 bytes_per_second=55.6172Mi/s items_per_second=12.0868k/s
bm_yamlcpp_arena           347077 ns       346777 ns         2003 bytes_per_second=13.2693Mi/s items_per_second=2.8837k/s
[ 77%] Built target ryml-bm-parse-scalar_dquot_singleline
[ 77%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_dquot_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_singleline.json;/proj/rapidyaml/bm/cases/scalar_dquot_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_singleline.json
-----------------------------------
running case: EMIT/scalar_dquot_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_dquot_singleline.yml
-----------------------------------
2024-05-07T21:11:57+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.57, 1.44, 1.32
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve       13666 ns        13655 ns        48932 bytes_per_second=336.98Mi/s items_per_second=73.2329k/s
bm_ryml_str               13604 ns        13593 ns        51525 bytes_per_second=338.518Mi/s items_per_second=73.5672k/s
bm_ryml_ostream          605484 ns       603851 ns         1000 bytes_per_second=7.62022Mi/s items_per_second=1.65604k/s
bm_fyaml_str_reserve      25418 ns        25398 ns        27456 bytes_per_second=181.177Mi/s items_per_second=39.3737k/s
bm_fyaml_str              25405 ns        25385 ns        27670 bytes_per_second=181.27Mi/s items_per_second=39.3938k/s
bm_fyaml_ostream        1738748 ns      1733993 ns         3387 bytes_per_second=2.65369Mi/s items_per_second=576.704/s
bm_yamlcpp               766984 ns       765600 ns         1281 bytes_per_second=6.01029Mi/s items_per_second=1.30616k/s
[ 77%] Built target ryml-bm-emit-scalar_dquot_singleline
[ 77%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_squot_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_multiline.json;/proj/rapidyaml/bm/cases/scalar_squot_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_multiline.json
-----------------------------------
running case: PARSE/scalar_squot_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_squot_multiline.yml
-----------------------------------
2024-05-07T21:12:09+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.44, 1.42, 1.32
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse      192174 ns       191965 ns         3726 bytes_per_second=359.053Mi/s items_per_second=5.20927k/s
bm_ryml_arena_reuse        190292 ns       190126 ns         3701 bytes_per_second=362.528Mi/s items_per_second=5.25968k/s
bm_ryml_inplace            223740 ns       223544 ns         3150 bytes_per_second=308.333Mi/s items_per_second=4.4734k/s
bm_ryml_arena              250243 ns       248998 ns         2823 bytes_per_second=276.813Mi/s items_per_second=4.0161k/s
bm_libyaml_arena           512111 ns       511558 ns         1379 bytes_per_second=134.737Mi/s items_per_second=1.95481k/s
bm_libyaml_arena_reuse     505744 ns       505361 ns         1392 bytes_per_second=136.389Mi/s items_per_second=1.97879k/s
bm_libfyaml_arena          424660 ns       424218 ns         1653 bytes_per_second=162.477Mi/s items_per_second=2.35728k/s
bm_yamlcpp_arena          2951897 ns      2948179 ns          239 bytes_per_second=23.3791Mi/s items_per_second=339.192/s
[ 77%] Built target ryml-bm-parse-scalar_squot_multiline
[ 79%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_squot_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_multiline.json;/proj/rapidyaml/bm/cases/scalar_squot_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_multiline.json
-----------------------------------
running case: EMIT/scalar_squot_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_squot_multiline.yml
-----------------------------------
2024-05-07T21:12:16+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.49, 1.43, 1.32
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve       87252 ns        87167 ns         7801 bytes_per_second=790.731Mi/s items_per_second=11.4722k/s
bm_ryml_str               87401 ns        87314 ns         8227 bytes_per_second=789.404Mi/s items_per_second=11.453k/s
bm_ryml_ostream         7442306 ns      7429152 ns          836 bytes_per_second=9.27776Mi/s items_per_second=134.605/s
bm_fyaml_str_reserve    1592806 ns      1591155 ns          448 bytes_per_second=43.3181Mi/s items_per_second=628.474/s
bm_fyaml_str            1560259 ns      1558928 ns          414 bytes_per_second=44.2136Mi/s items_per_second=641.466/s
bm_fyaml_ostream        2660061 ns      2655004 ns          374 bytes_per_second=25.9607Mi/s items_per_second=376.647/s
bm_yamlcpp              8552306 ns      8542833 ns           83 bytes_per_second=8.06827Mi/s items_per_second=117.057/s
[ 79%] Built target ryml-bm-emit-scalar_squot_multiline
[ 81%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_squot_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_singleline.json;/proj/rapidyaml/bm/cases/scalar_squot_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_singleline.json
-----------------------------------
running case: PARSE/scalar_squot_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_squot_singleline.yml
-----------------------------------
2024-05-07T21:12:28+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.52, 1.44, 1.33
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse       13399 ns        13383 ns        52974 bytes_per_second=343.538Mi/s items_per_second=74.72k/s
bm_ryml_arena_reuse         13603 ns        13591 ns        51907 bytes_per_second=338.275Mi/s items_per_second=73.5755k/s
bm_ryml_inplace             16075 ns        16074 ns        43309 bytes_per_second=286.023Mi/s items_per_second=62.2106k/s
bm_ryml_arena               15625 ns        15611 ns        42280 bytes_per_second=294.518Mi/s items_per_second=64.0582k/s
bm_libyaml_arena            60552 ns        60497 ns        11120 bytes_per_second=75.9982Mi/s items_per_second=16.5297k/s
bm_libyaml_arena_reuse      59880 ns        59849 ns        11758 bytes_per_second=76.8212Mi/s items_per_second=16.7087k/s
bm_libfyaml_arena           83796 ns        83711 ns         8536 bytes_per_second=54.9229Mi/s items_per_second=11.9458k/s
bm_yamlcpp_arena           336869 ns       336606 ns         2074 bytes_per_second=13.6589Mi/s items_per_second=2.97083k/s
[ 81%] Built target ryml-bm-parse-scalar_squot_singleline
[ 81%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_squot_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_singleline.json;/proj/rapidyaml/bm/cases/scalar_squot_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_singleline.json
-----------------------------------
running case: EMIT/scalar_squot_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_squot_singleline.yml
-----------------------------------
2024-05-07T21:12:35+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.48, 1.43, 1.33
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve       13730 ns        13706 ns        51212 bytes_per_second=335.445Mi/s items_per_second=72.9598k/s
bm_ryml_str               13420 ns        13409 ns        49675 bytes_per_second=342.878Mi/s items_per_second=74.5765k/s
bm_ryml_ostream          659467 ns       657704 ns         1000 bytes_per_second=6.99047Mi/s items_per_second=1.52044k/s
bm_fyaml_str_reserve      28611 ns        28591 ns        24327 bytes_per_second=160.811Mi/s items_per_second=34.9766k/s
bm_fyaml_str              28655 ns        28634 ns        24396 bytes_per_second=160.567Mi/s items_per_second=34.9237k/s
bm_fyaml_ostream        1731113 ns      1726717 ns         3352 bytes_per_second=2.66266Mi/s items_per_second=579.134/s
bm_yamlcpp               762206 ns       760941 ns         1258 bytes_per_second=6.04207Mi/s items_per_second=1.31416k/s
[ 81%] Built target ryml-bm-emit-scalar_squot_singleline
[ 83%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_plain_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_multiline.json;/proj/rapidyaml/bm/cases/scalar_plain_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_multiline.json
-----------------------------------
running case: PARSE/scalar_plain_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_plain_multiline.yml
-----------------------------------
2024-05-07T21:12:47+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.48, 1.43, 1.33
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse      335243 ns       334285 ns         2112 bytes_per_second=205.527Mi/s items_per_second=2.99146k/s
bm_ryml_arena_reuse        334825 ns       334565 ns         2074 bytes_per_second=205.355Mi/s items_per_second=2.98895k/s
bm_ryml_inplace            368636 ns       368323 ns         1867 bytes_per_second=186.534Mi/s items_per_second=2.71501k/s
bm_ryml_arena              395595 ns       393695 ns         1760 bytes_per_second=174.512Mi/s items_per_second=2.54004k/s
bm_libyaml_arena           533041 ns       532581 ns         1317 bytes_per_second=129.003Mi/s items_per_second=1.87765k/s
bm_libyaml_arena_reuse     531141 ns       530741 ns         1317 bytes_per_second=129.45Mi/s items_per_second=1.88416k/s
bm_libfyaml_arena          438295 ns       437947 ns         1596 bytes_per_second=156.879Mi/s items_per_second=2.28338k/s
bm_yamlcpp_arena          5407322 ns      5402064 ns          129 bytes_per_second=12.7182Mi/s items_per_second=185.114/s
[ 83%] Built target ryml-bm-parse-scalar_plain_multiline
[ 85%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_plain_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_multiline.json;/proj/rapidyaml/bm/cases/scalar_plain_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_multiline.json
-----------------------------------
running case: EMIT/scalar_plain_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_plain_multiline.yml
-----------------------------------
2024-05-07T21:12:55+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.41, 1.42, 1.32
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve      104212 ns       104133 ns         6710 bytes_per_second=659.778Mi/s items_per_second=9.60311k/s
bm_ryml_str              105329 ns       104638 ns         6700 bytes_per_second=656.591Mi/s items_per_second=9.55673k/s
bm_ryml_ostream         6687890 ns      6676579 ns          735 bytes_per_second=10.2904Mi/s items_per_second=149.777/s
bm_fyaml_str_reserve    1435035 ns      1433863 ns          466 bytes_per_second=47.9157Mi/s items_per_second=697.417/s
bm_fyaml_str            1425131 ns      1423928 ns          489 bytes_per_second=48.2501Mi/s items_per_second=702.283/s
bm_fyaml_ostream        2672477 ns      2666956 ns          396 bytes_per_second=25.7614Mi/s items_per_second=374.959/s
bm_yamlcpp              8757269 ns      8687103 ns           83 bytes_per_second=7.9088Mi/s items_per_second=115.113/s
[ 85%] Built target ryml-bm-emit-scalar_plain_multiline
[ 85%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_plain_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_singleline.json;/proj/rapidyaml/bm/cases/scalar_plain_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_singleline.json
-----------------------------------
running case: PARSE/scalar_plain_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_plain_singleline.yml
-----------------------------------
2024-05-07T21:13:05+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.34, 1.40, 1.32
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse       29021 ns        29001 ns        24127 bytes_per_second=153.801Mi/s items_per_second=34.482k/s
bm_ryml_arena_reuse         29114 ns        29098 ns        24110 bytes_per_second=153.286Mi/s items_per_second=34.3664k/s
bm_ryml_inplace             31611 ns        31599 ns        22155 bytes_per_second=141.154Mi/s items_per_second=31.6465k/s
bm_ryml_arena               31297 ns        31274 ns        22262 bytes_per_second=142.62Mi/s items_per_second=31.9752k/s
bm_libyaml_arena            64662 ns        64606 ns        11078 bytes_per_second=69.0386Mi/s items_per_second=15.4783k/s
bm_libyaml_arena_reuse      63110 ns        63065 ns        11372 bytes_per_second=70.7261Mi/s items_per_second=15.8567k/s
bm_libfyaml_arena           84346 ns        84262 ns         8118 bytes_per_second=52.9342Mi/s items_per_second=11.8678k/s
bm_yamlcpp_arena           487076 ns       486647 ns         1439 bytes_per_second=9.16545Mi/s items_per_second=2.05488k/s
[ 85%] Built target ryml-bm-parse-scalar_plain_singleline
[ 85%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_plain_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_singleline.json;/proj/rapidyaml/bm/cases/scalar_plain_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_singleline.json
-----------------------------------
running case: EMIT/scalar_plain_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_plain_singleline.yml
-----------------------------------
2024-05-07T21:13:13+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.44, 1.42, 1.33
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve       18745 ns        18723 ns        39219 bytes_per_second=238.227Mi/s items_per_second=53.4102k/s
bm_ryml_str               17753 ns        17738 ns        39928 bytes_per_second=251.457Mi/s items_per_second=56.3763k/s
bm_ryml_ostream          558422 ns       556609 ns         1000 bytes_per_second=8.0134Mi/s items_per_second=1.79659k/s
bm_fyaml_str_reserve     106034 ns       105879 ns         6748 bytes_per_second=42.1269Mi/s items_per_second=9.44478k/s
bm_fyaml_str             106888 ns       106745 ns         6732 bytes_per_second=41.785Mi/s items_per_second=9.36812k/s
bm_fyaml_ostream         926804 ns       924136 ns         2443 bytes_per_second=4.82649Mi/s items_per_second=1.08209k/s
bm_yamlcpp               776639 ns       775424 ns         1219 bytes_per_second=5.75213Mi/s items_per_second=1.28962k/s
[ 85%] Built target ryml-bm-emit-scalar_plain_singleline
[ 85%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_block_literal_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_multiline.json;/proj/rapidyaml/bm/cases/scalar_block_literal_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_multiline.json
-----------------------------------
running case: PARSE/scalar_block_literal_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_block_literal_multiline.yml
-----------------------------------
2024-05-07T21:13:21+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.49, 1.43, 1.33
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse      117902 ns       117819 ns         5932 bytes_per_second=587.474Mi/s items_per_second=8.48758k/s
bm_ryml_arena_reuse        119452 ns       119379 ns         5876 bytes_per_second=579.8Mi/s items_per_second=8.37671k/s
bm_ryml_inplace            151912 ns       151811 ns         4609 bytes_per_second=455.933Mi/s items_per_second=6.58713k/s
bm_ryml_arena              174816 ns       173778 ns         4074 bytes_per_second=398.299Mi/s items_per_second=5.75445k/s
bm_libyaml_arena           446503 ns       443664 ns         1592 bytes_per_second=156.01Mi/s items_per_second=2.25396k/s
bm_libyaml_arena_reuse     438019 ns       437704 ns         1601 bytes_per_second=158.134Mi/s items_per_second=2.28465k/s
bm_libfyaml_arena          409948 ns       409598 ns         1712 bytes_per_second=168.984Mi/s items_per_second=2.44142k/s
bm_yamlcpp_arena          2705720 ns      2702484 ns          260 bytes_per_second=25.6119Mi/s items_per_second=370.03/s
[ 85%] Built target ryml-bm-parse-scalar_block_literal_multiline
[ 85%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_block_literal_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_multiline.json;/proj/rapidyaml/bm/cases/scalar_block_literal_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_multiline.json
-----------------------------------
running case: EMIT/scalar_block_literal_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_block_literal_multiline.yml
-----------------------------------
2024-05-07T21:13:29+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.65, 1.47, 1.34
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve       49777 ns        49733 ns        14107 bytes_per_second=1.35914Gi/s items_per_second=20.1075k/s
bm_ryml_str               48675 ns        48637 ns        14176 bytes_per_second=1.38975Gi/s items_per_second=20.5604k/s
bm_ryml_ostream         6820343 ns      6799457 ns          724 bytes_per_second=10.1796Mi/s items_per_second=147.071/s
bm_fyaml_str_reserve    1473487 ns      1472310 ns          473 bytes_per_second=47.0117Mi/s items_per_second=679.205/s
bm_fyaml_str            1474645 ns      1473459 ns          471 bytes_per_second=46.975Mi/s items_per_second=678.675/s
bm_fyaml_ostream        2681343 ns      2676066 ns          388 bytes_per_second=25.8647Mi/s items_per_second=373.683/s
bm_yamlcpp              3199211 ns      3193927 ns          255 bytes_per_second=21.6711Mi/s items_per_second=313.094/s
[ 85%] Built target ryml-bm-emit-scalar_block_literal_multiline
[ 87%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_block_literal_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_singleline.json;/proj/rapidyaml/bm/cases/scalar_block_literal_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_singleline.json
-----------------------------------
running case: PARSE/scalar_block_literal_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_block_literal_singleline.yml
-----------------------------------
2024-05-07T21:13:40+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.63, 1.47, 1.35
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse       19771 ns        19758 ns        35513 bytes_per_second=253.795Mi/s items_per_second=50.6129k/s
bm_ryml_arena_reuse         19864 ns        19854 ns        35243 bytes_per_second=252.56Mi/s items_per_second=50.3667k/s
bm_ryml_inplace             22809 ns        22804 ns        30599 bytes_per_second=219.894Mi/s items_per_second=43.8523k/s
bm_ryml_arena               22673 ns        22658 ns        30925 bytes_per_second=221.313Mi/s items_per_second=44.1353k/s
bm_libyaml_arena            58942 ns        58904 ns        11920 bytes_per_second=85.1293Mi/s items_per_second=16.9769k/s
bm_libyaml_arena_reuse      56825 ns        56800 ns        12226 bytes_per_second=88.2815Mi/s items_per_second=17.6055k/s
bm_libfyaml_arena           82754 ns        82694 ns         8473 bytes_per_second=60.6383Mi/s items_per_second=12.0928k/s
bm_yamlcpp_arena           334028 ns       333766 ns         2103 bytes_per_second=15.0238Mi/s items_per_second=2.99611k/s
[ 87%] Built target ryml-bm-parse-scalar_block_literal_singleline
[ 88%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_block_literal_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_singleline.json;/proj/rapidyaml/bm/cases/scalar_block_literal_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_singleline.json
-----------------------------------
running case: EMIT/scalar_block_literal_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_block_literal_singleline.yml
-----------------------------------
2024-05-07T21:13:48+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.53, 1.46, 1.34
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve        8821 ns         8815 ns        79184 bytes_per_second=568.877Mi/s items_per_second=113.448k/s
bm_ryml_str                8842 ns         8836 ns        79542 bytes_per_second=567.516Mi/s items_per_second=113.177k/s
bm_ryml_ostream          619730 ns       618267 ns         1000 bytes_per_second=8.11044Mi/s items_per_second=1.61742k/s
bm_fyaml_str_reserve     112823 ns       112747 ns         6208 bytes_per_second=44.4749Mi/s items_per_second=8.86941k/s
bm_fyaml_str             112957 ns       112882 ns         6190 bytes_per_second=44.422Mi/s items_per_second=8.85884k/s
bm_fyaml_ostream        1013224 ns      1010699 ns         2305 bytes_per_second=4.96134Mi/s items_per_second=989.414/s
bm_yamlcpp               851547 ns       849677 ns         1070 bytes_per_second=5.90156Mi/s items_per_second=1.17692k/s
[ 88%] Built target ryml-bm-emit-scalar_block_literal_singleline
[ 90%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_block_folded_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_multiline.json;/proj/rapidyaml/bm/cases/scalar_block_folded_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_multiline.json
-----------------------------------
running case: PARSE/scalar_block_folded_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_block_folded_multiline.yml
-----------------------------------
2024-05-07T21:13:55+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.49, 1.45, 1.34
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse      114337 ns       114262 ns         6118 bytes_per_second=605.761Mi/s items_per_second=8.75178k/s
bm_ryml_arena_reuse        115737 ns       115676 ns         5945 bytes_per_second=598.36Mi/s items_per_second=8.64485k/s
bm_ryml_inplace            148669 ns       148592 ns         4713 bytes_per_second=465.811Mi/s items_per_second=6.72985k/s
bm_ryml_arena              175107 ns       173858 ns         4143 bytes_per_second=398.116Mi/s items_per_second=5.75182k/s
bm_libyaml_arena           440233 ns       439896 ns         1572 bytes_per_second=157.346Mi/s items_per_second=2.27326k/s
bm_libyaml_arena_reuse     438742 ns       438453 ns         1601 bytes_per_second=157.864Mi/s items_per_second=2.28075k/s
bm_libfyaml_arena          411574 ns       411229 ns         1684 bytes_per_second=168.315Mi/s items_per_second=2.43174k/s
bm_yamlcpp_arena          2715583 ns      2711961 ns          259 bytes_per_second=25.5224Mi/s items_per_second=368.737/s
[ 90%] Built target ryml-bm-parse-scalar_block_folded_multiline
[ 92%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_block_folded_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_multiline.json;/proj/rapidyaml/bm/cases/scalar_block_folded_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_multiline.json
-----------------------------------
running case: EMIT/scalar_block_folded_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_block_folded_multiline.yml
-----------------------------------
2024-05-07T21:14:03+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.49, 1.45, 1.34
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve       69014 ns        68962 ns         9991 bytes_per_second=1003.69Mi/s items_per_second=14.5008k/s
bm_ryml_str               69132 ns        69082 ns        10131 bytes_per_second=1001.93Mi/s items_per_second=14.4755k/s
bm_ryml_ostream         7002904 ns      6984224 ns          787 bytes_per_second=9.9103Mi/s items_per_second=143.18/s
bm_fyaml_str_reserve    1495938 ns      1494747 ns          465 bytes_per_second=46.306Mi/s items_per_second=669.01/s
bm_fyaml_str            1506537 ns      1505210 ns          450 bytes_per_second=45.9841Mi/s items_per_second=664.359/s
bm_fyaml_ostream        2634412 ns      2629351 ns          388 bytes_per_second=26.3243Mi/s items_per_second=380.322/s
bm_yamlcpp              9598931 ns      9589014 ns           75 bytes_per_second=7.21824Mi/s items_per_second=104.286/s
[ 92%] Built target ryml-bm-emit-scalar_block_folded_multiline
[ 92%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_block_folded_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_singleline.json;/proj/rapidyaml/bm/cases/scalar_block_folded_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_singleline.json
-----------------------------------
running case: PARSE/scalar_block_folded_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_block_folded_singleline.yml
-----------------------------------
2024-05-07T21:14:14+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.57, 1.47, 1.35
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse       19596 ns        19580 ns        32072 bytes_per_second=256.098Mi/s items_per_second=51.0724k/s
bm_ryml_arena_reuse         19685 ns        19676 ns        35633 bytes_per_second=254.845Mi/s items_per_second=50.8224k/s
bm_ryml_inplace             23079 ns        23076 ns        30833 bytes_per_second=217.299Mi/s items_per_second=43.3349k/s
bm_ryml_arena               23207 ns        23184 ns        30335 bytes_per_second=216.286Mi/s items_per_second=43.1328k/s
bm_libyaml_arena            59214 ns        59166 ns        11896 bytes_per_second=84.7524Mi/s items_per_second=16.9017k/s
bm_libyaml_arena_reuse      56986 ns        56954 ns        12191 bytes_per_second=88.0433Mi/s items_per_second=17.558k/s
bm_libfyaml_arena           86044 ns        85915 ns         8383 bytes_per_second=58.3651Mi/s items_per_second=11.6395k/s
bm_yamlcpp_arena           333013 ns       332738 ns         2103 bytes_per_second=15.0702Mi/s items_per_second=3.00536k/s
[ 92%] Built target ryml-bm-parse-scalar_block_folded_singleline
[ 92%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_block_folded_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_singleline.json;/proj/rapidyaml/bm/cases/scalar_block_folded_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_singleline.json
-----------------------------------
running case: EMIT/scalar_block_folded_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_block_folded_singleline.yml
-----------------------------------
2024-05-07T21:14:22+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.61, 1.48, 1.35
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve        8855 ns         8848 ns        79156 bytes_per_second=566.738Mi/s items_per_second=113.022k/s
bm_ryml_str                8851 ns         8844 ns        79264 bytes_per_second=566.98Mi/s items_per_second=113.07k/s
bm_ryml_ostream          606375 ns       604719 ns         1000 bytes_per_second=8.29215Mi/s items_per_second=1.65366k/s
bm_fyaml_str_reserve     121217 ns       120222 ns         5830 bytes_per_second=41.7097Mi/s items_per_second=8.31795k/s
bm_fyaml_str             119786 ns       119694 ns         5847 bytes_per_second=41.8936Mi/s items_per_second=8.35462k/s
bm_fyaml_ostream         957141 ns       954462 ns         2237 bytes_per_second=5.25366Mi/s items_per_second=1.04771k/s
bm_yamlcpp               842284 ns       840864 ns         1051 bytes_per_second=5.96341Mi/s items_per_second=1.18925k/s
[ 92%] Built target ryml-bm-emit-scalar_block_folded_singleline
[ 92%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_maps_blck_outer1000_inner10: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner10.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner10.json;/proj/rapidyaml/bm/cases/style_maps_blck_outer1000_inner10.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner10.json
-----------------------------------
running case: PARSE/style_maps_blck_outer1000_inner10.yml
file: /proj/rapidyaml/bm/cases/style_maps_blck_outer1000_inner10.yml
-----------------------------------
2024-05-07T21:14:29+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.98, 1.56, 1.38
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse     1211578 ns      1210125 ns          578 bytes_per_second=70.061Mi/s items_per_second=826.361/s
bm_ryml_arena_reuse       1219340 ns      1217880 ns          576 bytes_per_second=69.6149Mi/s items_per_second=821.099/s
bm_ryml_inplace           1584684 ns      1581898 ns          443 bytes_per_second=53.5955Mi/s items_per_second=632.152/s
bm_ryml_arena             1609274 ns      1593283 ns          439 bytes_per_second=53.2125Mi/s items_per_second=627.635/s
bm_libyaml_arena          3648314 ns      3643511 ns          194 bytes_per_second=23.2695Mi/s items_per_second=274.461/s
bm_libyaml_arena_reuse    3540938 ns      3536838 ns          200 bytes_per_second=23.9713Mi/s items_per_second=282.738/s
bm_libfyaml_arena         7990671 ns      7978355 ns           88 bytes_per_second=10.6266Mi/s items_per_second=125.339/s
bm_yamlcpp_arena         24640948 ns     24608669 ns           29 bytes_per_second=3.44523Mi/s items_per_second=40.6361/s
[ 92%] Built target ryml-bm-parse-style_maps_blck_outer1000_inner10
[ 92%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_maps_blck_outer1000_inner10: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner10.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner10.json;/proj/rapidyaml/bm/cases/style_maps_blck_outer1000_inner10.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner10.json
-----------------------------------
running case: EMIT/style_maps_blck_outer1000_inner10.yml
file: /proj/rapidyaml/bm/cases/style_maps_blck_outer1000_inner10.yml
-----------------------------------
2024-05-07T21:14:37+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.90, 1.55, 1.38
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve      457268 ns       456772 ns         1559 bytes_per_second=185.613Mi/s items_per_second=2.18928k/s
bm_ryml_str              458267 ns       457610 ns         1504 bytes_per_second=185.272Mi/s items_per_second=2.18527k/s
bm_ryml_ostream         4663273 ns      4655040 ns          378 bytes_per_second=18.2131Mi/s items_per_second=214.821/s
bm_fyaml_str_reserve    3262416 ns      3258007 ns          214 bytes_per_second=26.0228Mi/s items_per_second=306.936/s
bm_fyaml_str            3306772 ns      3301883 ns          214 bytes_per_second=25.677Mi/s items_per_second=302.857/s
bm_fyaml_ostream        4474431 ns      4467198 ns          166 bytes_per_second=18.9789Mi/s items_per_second=223.854/s
bm_yamlcpp             13030150 ns     13004862 ns           57 bytes_per_second=6.5193Mi/s items_per_second=76.8943/s
[ 92%] Built target ryml-bm-emit-style_maps_blck_outer1000_inner10
[ 92%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_maps_blck_outer1000_inner100: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner100.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner100.json;/proj/rapidyaml/bm/cases/style_maps_blck_outer1000_inner100.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner100.json
-----------------------------------
running case: PARSE/style_maps_blck_outer1000_inner100.yml
file: /proj/rapidyaml/bm/cases/style_maps_blck_outer1000_inner100.yml
-----------------------------------
2024-05-07T21:14:46+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.91, 1.57, 1.39
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse    11850371 ns     11831296 ns           59 bytes_per_second=78.9134Mi/s items_per_second=84.5216/s
bm_ryml_arena_reuse      12106299 ns     12086567 ns           58 bytes_per_second=77.2468Mi/s items_per_second=82.7365/s
bm_ryml_inplace          16523873 ns     16493769 ns           42 bytes_per_second=56.6061Mi/s items_per_second=60.629/s
bm_ryml_arena            16370358 ns     16339729 ns           43 bytes_per_second=57.1398Mi/s items_per_second=61.2005/s
bm_libyaml_arena         35526144 ns     35482593 ns           20 bytes_per_second=26.3128Mi/s items_per_second=28.1828/s
bm_libyaml_arena_reuse   34849249 ns     34790787 ns           20 bytes_per_second=26.8361Mi/s items_per_second=28.7432/s
bm_libfyaml_arena       236832480 ns    236554424 ns            3 bytes_per_second=3.94686Mi/s items_per_second=4.22736/s
bm_yamlcpp_arena        278835352 ns    278484987 ns            3 bytes_per_second=3.3526Mi/s items_per_second=3.59086/s
[ 92%] Built target ryml-bm-parse-style_maps_blck_outer1000_inner100
[ 94%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_maps_blck_outer1000_inner100: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner100.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner100.json;/proj/rapidyaml/bm/cases/style_maps_blck_outer1000_inner100.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner100.json
-----------------------------------
running case: EMIT/style_maps_blck_outer1000_inner100.yml
file: /proj/rapidyaml/bm/cases/style_maps_blck_outer1000_inner100.yml
-----------------------------------
2024-05-07T21:14:54+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.77, 1.55, 1.38
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve     4636396 ns      4629175 ns          131 bytes_per_second=201.688Mi/s items_per_second=216.021/s
bm_ryml_str             4647489 ns      4641393 ns          139 bytes_per_second=201.157Mi/s items_per_second=215.453/s
bm_ryml_ostream        17210166 ns     17186902 ns           70 bytes_per_second=54.3232Mi/s items_per_second=58.1838/s
bm_fyaml_str_reserve   37241161 ns     37198766 ns           18 bytes_per_second=25.0989Mi/s items_per_second=26.8826/s
bm_fyaml_str           37378917 ns     37337167 ns           18 bytes_per_second=25.0059Mi/s items_per_second=26.783/s
bm_fyaml_ostream       42213146 ns     42158860 ns           16 bytes_per_second=22.146Mi/s items_per_second=23.7198/s
bm_yamlcpp            147866200 ns    147694369 ns            5 bytes_per_second=6.32149Mi/s items_per_second=6.77074/s
[ 94%] Built target ryml-bm-emit-style_maps_blck_outer1000_inner100
[ 96%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_maps_flow_outer1000_inner10: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10.json;/proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner10.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10.json
-----------------------------------
running case: PARSE/style_maps_flow_outer1000_inner10.yml
file: /proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner10.yml
-----------------------------------
2024-05-07T21:15:04+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.65, 1.53, 1.38
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse      843523 ns       842599 ns          828 bytes_per_second=82.5111Mi/s items_per_second=1.1868k/s
bm_ryml_arena_reuse        847581 ns       846647 ns          824 bytes_per_second=82.1166Mi/s items_per_second=1.18113k/s
bm_ryml_inplace           1191426 ns      1190297 ns          591 bytes_per_second=58.4088Mi/s items_per_second=840.126/s
bm_ryml_arena             1183088 ns      1181718 ns          591 bytes_per_second=58.8328Mi/s items_per_second=846.225/s
bm_libyaml_arena          4997759 ns      4990397 ns          141 bytes_per_second=13.9315Mi/s items_per_second=200.385/s
bm_libyaml_arena_reuse    3730590 ns      3726520 ns          187 bytes_per_second=18.6565Mi/s items_per_second=268.347/s
bm_libfyaml_arena         8013183 ns      8001522 ns           87 bytes_per_second=8.68882Mi/s items_per_second=124.976/s
bm_yamlcpp_arena         24122195 ns     24090981 ns           29 bytes_per_second=2.88589Mi/s items_per_second=41.5093/s
[ 96%] Built target ryml-bm-parse-style_maps_flow_outer1000_inner10
[ 96%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_maps_flow_outer1000_inner10: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10.json;/proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner10.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10.json
-----------------------------------
running case: EMIT/style_maps_flow_outer1000_inner10.yml
file: /proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner10.yml
-----------------------------------
2024-05-07T21:15:11+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.68, 1.54, 1.38
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve      451771 ns       451261 ns         1549 bytes_per_second=154.066Mi/s items_per_second=2.21601k/s
bm_ryml_str              451441 ns       450964 ns         1551 bytes_per_second=154.167Mi/s items_per_second=2.21747k/s
bm_ryml_ostream         4406951 ns      4399915 ns          370 bytes_per_second=15.8012Mi/s items_per_second=227.277/s
bm_fyaml_str_reserve    1951145 ns      1948674 ns          355 bytes_per_second=35.6775Mi/s items_per_second=513.17/s
bm_fyaml_str            1961317 ns      1958861 ns          354 bytes_per_second=35.492Mi/s items_per_second=510.501/s
bm_fyaml_ostream        3975449 ns      3968657 ns          213 bytes_per_second=17.5182Mi/s items_per_second=251.974/s
bm_yamlcpp             14011076 ns     13993082 ns           50 bytes_per_second=4.96844Mi/s items_per_second=71.4639/s
[ 96%] Built target ryml-bm-emit-style_maps_flow_outer1000_inner10
[ 96%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_maps_flow_outer1000_inner100: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100.json;/proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner100.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100.json
-----------------------------------
running case: PARSE/style_maps_flow_outer1000_inner100.yml
file: /proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner100.yml
-----------------------------------
2024-05-07T21:15:20+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 2.03, 1.62, 1.41
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse     7933553 ns      7922166 ns           88 bytes_per_second=94.2581Mi/s items_per_second=126.228/s
bm_ryml_arena_reuse       8034669 ns      8023318 ns           88 bytes_per_second=93.0697Mi/s items_per_second=124.637/s
bm_ryml_inplace          11302803 ns     11279884 ns           61 bytes_per_second=66.2Mi/s items_per_second=88.6534/s
bm_ryml_arena            16914782 ns     16883296 ns           41 bytes_per_second=44.2288Mi/s items_per_second=59.2301/s
bm_libyaml_arena         43630828 ns     43568870 ns           16 bytes_per_second=17.139Mi/s items_per_second=22.9522/s
bm_libyaml_arena_reuse   36839056 ns     36783997 ns           19 bytes_per_second=20.3003Mi/s items_per_second=27.1857/s
bm_libfyaml_arena       228874480 ns    228605946 ns            3 bytes_per_second=3.26644Mi/s items_per_second=4.37434/s
bm_yamlcpp_arena        290241553 ns    289390349 ns            3 bytes_per_second=2.58035Mi/s items_per_second=3.45554/s
[ 96%] Built target ryml-bm-parse-style_maps_flow_outer1000_inner100
[ 96%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_maps_flow_outer1000_inner100: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100.json;/proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner100.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100.json
-----------------------------------
running case: EMIT/style_maps_flow_outer1000_inner100.yml
file: /proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner100.yml
-----------------------------------
2024-05-07T21:15:28+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 2.03, 1.63, 1.42
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve     4566512 ns      4561052 ns          147 bytes_per_second=163.718Mi/s items_per_second=219.248/s
bm_ryml_str             4598803 ns      4593288 ns          140 bytes_per_second=162.569Mi/s items_per_second=217.709/s
bm_ryml_ostream        18068291 ns     18045408 ns           72 bytes_per_second=41.3805Mi/s items_per_second=55.4158/s
bm_fyaml_str_reserve   29397396 ns     29351016 ns           24 bytes_per_second=25.4413Mi/s items_per_second=34.0704/s
bm_fyaml_str           28566395 ns     28525814 ns           24 bytes_per_second=26.1773Mi/s items_per_second=35.056/s
bm_fyaml_ostream       34862147 ns     34818021 ns           19 bytes_per_second=21.4466Mi/s items_per_second=28.7208/s
bm_yamlcpp            169192660 ns    168701554 ns            4 bytes_per_second=4.42633Mi/s items_per_second=5.92763/s
[ 96%] Built target ryml-bm-emit-style_maps_flow_outer1000_inner100
[ 96%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_seqs_blck_outer1000_inner10: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner10.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner10.json;/proj/rapidyaml/bm/cases/style_seqs_blck_outer1000_inner10.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner10.json
-----------------------------------
running case: PARSE/style_seqs_blck_outer1000_inner10.yml
file: /proj/rapidyaml/bm/cases/style_seqs_blck_outer1000_inner10.yml
-----------------------------------
2024-05-07T21:15:38+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.87, 1.61, 1.41
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse      958502 ns       957370 ns          735 bytes_per_second=78.5964Mi/s items_per_second=1.04453k/s
bm_ryml_arena_reuse        959693 ns       958594 ns          730 bytes_per_second=78.4961Mi/s items_per_second=1.04319k/s
bm_ryml_inplace           1353960 ns      1350222 ns          524 bytes_per_second=55.7285Mi/s items_per_second=740.619/s
bm_ryml_arena             1390892 ns      1388476 ns          459 bytes_per_second=54.1931Mi/s items_per_second=720.214/s
bm_libyaml_arena          2283765 ns      2281004 ns          309 bytes_per_second=32.988Mi/s items_per_second=438.403/s
bm_libyaml_arena_reuse    2142309 ns      2139811 ns          327 bytes_per_second=35.1647Mi/s items_per_second=467.331/s
bm_libfyaml_arena         3597652 ns      3592116 ns          202 bytes_per_second=20.9475Mi/s items_per_second=278.387/s
bm_yamlcpp_arena         16105043 ns     16078537 ns           44 bytes_per_second=4.67989Mi/s items_per_second=62.1947/s
[ 96%] Built target ryml-bm-parse-style_seqs_blck_outer1000_inner10
[ 98%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_seqs_blck_outer1000_inner10: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner10.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner10.json;/proj/rapidyaml/bm/cases/style_seqs_blck_outer1000_inner10.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner10.json
-----------------------------------
running case: EMIT/style_seqs_blck_outer1000_inner10.yml
file: /proj/rapidyaml/bm/cases/style_seqs_blck_outer1000_inner10.yml
-----------------------------------
2024-05-07T21:15:46+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.80, 1.60, 1.41
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve      283463 ns       283169 ns         2467 bytes_per_second=265.728Mi/s items_per_second=3.53147k/s
bm_ryml_str              284010 ns       283397 ns         2426 bytes_per_second=265.514Mi/s items_per_second=3.52861k/s
bm_ryml_ostream         5275996 ns      5267587 ns          473 bytes_per_second=14.2847Mi/s items_per_second=189.84/s
bm_fyaml_str_reserve    2679428 ns      2676613 ns          261 bytes_per_second=28.1123Mi/s items_per_second=373.607/s
bm_fyaml_str            2687224 ns      2684320 ns          259 bytes_per_second=28.0316Mi/s items_per_second=372.534/s
bm_fyaml_ostream        3712002 ns      3706126 ns          209 bytes_per_second=20.3031Mi/s items_per_second=269.824/s
bm_yamlcpp              7985088 ns      7974420 ns           92 bytes_per_second=9.4359Mi/s items_per_second=125.401/s
[ 98%] Built target ryml-bm-emit-style_seqs_blck_outer1000_inner10
[100%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_seqs_blck_outer1000_inner100: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner100.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner100.json;/proj/rapidyaml/bm/cases/style_seqs_blck_outer1000_inner100.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner100.json
-----------------------------------
running case: PARSE/style_seqs_blck_outer1000_inner100.yml
file: /proj/rapidyaml/bm/cases/style_seqs_blck_outer1000_inner100.yml
-----------------------------------
2024-05-07T21:15:55+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.68, 1.58, 1.40
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse     9133722 ns      9120508 ns           77 bytes_per_second=82.5009Mi/s items_per_second=109.643/s
bm_ryml_arena_reuse       9228534 ns      9214135 ns           76 bytes_per_second=81.6626Mi/s items_per_second=108.529/s
bm_ryml_inplace          13320775 ns     13297585 ns           51 bytes_per_second=56.5855Mi/s items_per_second=75.2016/s
bm_ryml_arena            13365580 ns     13339917 ns           52 bytes_per_second=56.4059Mi/s items_per_second=74.963/s
bm_libyaml_arena         21748758 ns     21714428 ns           33 bytes_per_second=34.6521Mi/s items_per_second=46.0523/s
bm_libyaml_arena_reuse   20594763 ns     20560728 ns           34 bytes_per_second=36.5965Mi/s items_per_second=48.6364/s
bm_libfyaml_arena        36302249 ns     36257458 ns           19 bytes_per_second=20.753Mi/s items_per_second=27.5805/s
bm_yamlcpp_arena        172786553 ns    172579755 ns            4 bytes_per_second=4.36001Mi/s items_per_second=5.79442/s
[100%] Built target ryml-bm-parse-style_seqs_blck_outer1000_inner100
[100%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_seqs_blck_outer1000_inner100: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner100.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner100.json;/proj/rapidyaml/bm/cases/style_seqs_blck_outer1000_inner100.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner100.json
-----------------------------------
running case: EMIT/style_seqs_blck_outer1000_inner100.yml
file: /proj/rapidyaml/bm/cases/style_seqs_blck_outer1000_inner100.yml
-----------------------------------
2024-05-07T21:16:02+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.62, 1.57, 1.40
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve     2739924 ns      2735145 ns          256 bytes_per_second=275.104Mi/s items_per_second=365.611/s
bm_ryml_str             2834738 ns      2828259 ns          254 bytes_per_second=266.047Mi/s items_per_second=353.574/s
bm_ryml_ostream        16512871 ns     16488065 ns          104 bytes_per_second=45.636Mi/s items_per_second=60.6499/s
bm_fyaml_str_reserve   27918761 ns     27881234 ns           24 bytes_per_second=26.9877Mi/s items_per_second=35.8664/s
bm_fyaml_str           27655848 ns     27625769 ns           24 bytes_per_second=27.2373Mi/s items_per_second=36.1981/s
bm_fyaml_ostream       32217411 ns     32178124 ns           21 bytes_per_second=23.3839Mi/s items_per_second=31.077/s
bm_yamlcpp             81310837 ns     81213692 ns            8 bytes_per_second=9.26506Mi/s items_per_second=12.3132/s
[100%] Built target ryml-bm-emit-style_seqs_blck_outer1000_inner100
[100%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_seqs_flow_outer1000_inner10: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10.json;/proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner10.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10.json
-----------------------------------
running case: PARSE/style_seqs_flow_outer1000_inner10.yml
file: /proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner10.yml
-----------------------------------
2024-05-07T21:16:11+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.90, 1.63, 1.42
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse      543164 ns       542536 ns         1238 bytes_per_second=75.4117Mi/s items_per_second=1.8432k/s
bm_ryml_arena_reuse        561535 ns       560661 ns         1290 bytes_per_second=72.9738Mi/s items_per_second=1.78361k/s
bm_ryml_inplace            883627 ns       882705 ns          793 bytes_per_second=46.3502Mi/s items_per_second=1.13288k/s
bm_ryml_arena              882847 ns       881755 ns          794 bytes_per_second=46.4002Mi/s items_per_second=1.1341k/s
bm_libyaml_arena          3354523 ns      3346645 ns          209 bytes_per_second=12.2253Mi/s items_per_second=298.807/s
bm_libyaml_arena_reuse    2072317 ns      2069775 ns          339 bytes_per_second=19.7672Mi/s items_per_second=483.144/s
bm_libfyaml_arena         2925219 ns      2921318 ns          233 bytes_per_second=14.0052Mi/s items_per_second=342.311/s
bm_yamlcpp_arena         12576141 ns     12560253 ns           56 bytes_per_second=3.25739Mi/s items_per_second=79.6162/s
[100%] Built target ryml-bm-parse-style_seqs_flow_outer1000_inner10
[100%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_seqs_flow_outer1000_inner10: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10.json;/proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner10.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10.json
-----------------------------------
running case: EMIT/style_seqs_flow_outer1000_inner10.yml
file: /proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner10.yml
-----------------------------------
2024-05-07T21:16:18+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 2.15, 1.69, 1.45
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
bm_ryml_str_reserve      283984 ns       283651 ns         2409 bytes_per_second=144.239Mi/s items_per_second=3.52546k/s
bm_ryml_str              283453 ns       283115 ns         2465 bytes_per_second=144.512Mi/s items_per_second=3.53214k/s
bm_ryml_ostream         5319742 ns      5310560 ns          455 bytes_per_second=7.70419Mi/s items_per_second=188.304/s
bm_fyaml_str_reserve    1146021 ns      1144679 ns          606 bytes_per_second=35.7424Mi/s items_per_second=873.608/s
bm_fyaml_str            1145572 ns      1144360 ns          608 bytes_per_second=35.7524Mi/s items_per_second=873.851/s
bm_fyaml_ostream        3094044 ns      3087103 ns          339 bytes_per_second=13.2531Mi/s items_per_second=323.928/s
bm_yamlcpp              8406380 ns      8394250 ns           85 bytes_per_second=4.874Mi/s items_per_second=119.129/s
[100%] Built target ryml-bm-emit-style_seqs_flow_outer1000_inner10
[100%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_seqs_flow_outer1000_inner100: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=yml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100.json;/proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner100.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100.json
-----------------------------------
running case: PARSE/style_seqs_flow_outer1000_inner100.yml
file: /proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner100.yml
-----------------------------------
2024-05-07T21:16:27+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 2.22, 1.72, 1.46
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
---------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------
bm_ryml_inplace_reuse     4913173 ns      4879592 ns          144 bytes_per_second=76.8087Mi/s items_per_second=204.935/s
bm_ryml_arena_reuse       4897882 ns      4889202 ns          143 bytes_per_second=76.6577Mi/s items_per_second=204.532/s
bm_ryml_inplace           8148632 ns      8126099 ns           84 bytes_per_second=46.1224Mi/s items_per_second=123.06/s
bm_ryml_arena            14078635 ns     14046558 ns           50 bytes_per_second=26.6823Mi/s items_per_second=71.1918/s
bm_libyaml_arena         28113150 ns     28067907 ns           25 bytes_per_second=13.3531Mi/s items_per_second=35.6279/s
bm_libyaml_arena_reuse   20817333 ns     20783207 ns           34 bytes_per_second=18.0335Mi/s items_per_second=48.1158/s
bm_libfyaml_arena        27715699 ns     27623908 ns           25 bytes_per_second=13.5678Mi/s items_per_second=36.2005/s
bm_yamlcpp_arena        151620839 ns    151316234 ns            5 bytes_per_second=2.4769Mi/s items_per_second=6.60868/s
[100%] Built target ryml-bm-parse-style_seqs_flow_outer1000_inner100
CWD=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm
[100%] Built target ryml-bm-run
Returning to directory /proj/rapidyaml (currently in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release)
Build: finished building (6m 34s): linux-x86_64-gxx13.2-Release
-----------------------------------------------

Compilation finished at Tue May  7 21:16:36
-*- mode: compilation; default-directory: "~/proj/rapidyaml/build/" -*-
Compilation started at Tue May  7 21:45:45

set -xe ; cd ~/proj/rapidyaml ; export C4_EXTERN_DIR=`pwd`/build/extern ; cmany b ryml-bm-plot -j1 -t 'Release: -X " -march=native" ' -c c++ -V C4_CXX_STANDARD=17 -V RYML_DBG=OFF  -V RYML_DEV=ON -V RYML_VALGRIND=OFF 
+ cd /proj/rapidyaml
++ pwd
+ export C4_EXTERN_DIR=/proj/rapidyaml/build/extern
+ C4_EXTERN_DIR=/proj/rapidyaml/build/extern
+ cmany b ryml-bm-plot -j1 -t 'Release: -X " -march=native" ' -c c++ -V C4_CXX_STANDARD=17 -V RYML_DBG=OFF -V RYML_DEV=ON -V RYML_VALGRIND=OFF
-----------------------------------------------
Build linux-x86_64-gxx13.2-Release
-----------------------------------------------
Entering directory /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release (was in /proj/rapidyaml)
--------
$ cd /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release && make -j 1 ryml-bm-plot
--------
/usr/bin/python3.11
Python 3.11.8
emit
style_seqs_flow_outer1000_inner100
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100.json
+----------------------------------------------------------------------------------------------+
|                      emit benchmark: style_seqs_flow_outer1000_inner100                      |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  |  143.47 |  32.92x | 3192.06% |          2.74 |       0.03x |     -96.96% |
| ryml_str          |  144.99 |  33.27x | 3226.83% |          2.71 |       0.03x |     -96.99% |
| ryml_ostream      |   23.43 |   5.38x |  437.53% |         16.78 |       0.19x |     -81.40% |
| fyaml_str_reserve |   34.86 |   8.00x |  699.79% |         11.28 |       0.13x |     -87.50% |
| fyaml_str         |   35.38 |   8.12x |  711.78% |         11.11 |       0.12x |     -87.68% |
| fyaml_ostream     |   22.58 |   5.18x |  418.21% |         17.40 |       0.19x |     -80.70% |
| yamlcpp_          |    4.36 |   1.00x |    0.00% |         90.18 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100-cpu_time_ms.html
Built target ryml-bm-emit-style_seqs_flow_outer1000_inner100-plot
/usr/bin/python3.11
Python 3.11.8
parse
appveyor
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-appveyor.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-appveyor.json
+------------------------------------------------------------------------------------------------+
|                                   parse benchmark: appveyor                                    |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |  176.82 |  18.24x | 1724.17% |          0.01 |       0.05x |     -94.52% |
| ryml_arena_reuse    |  172.52 |  17.80x | 1679.86% |          0.01 |       0.06x |     -94.38% |
| ryml_inplace        |  157.99 |  16.30x | 1529.93% |          0.01 |       0.06x |     -93.86% |
| ryml_arena          |  161.85 |  16.70x | 1569.78% |          0.01 |       0.06x |     -94.01% |
| libyaml_arena       |   66.29 |   6.84x |  583.84% |          0.03 |       0.15x |     -85.38% |
| libyaml_arena_reuse |   66.16 |   6.83x |  582.51% |          0.03 |       0.15x |     -85.35% |
| libfyaml_arena      |   43.86 |   4.52x |  352.50% |          0.05 |       0.22x |     -77.90% |
| yamlcpp_arena       |    9.69 |   1.00x |    0.00% |          0.22 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-appveyor-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-appveyor-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-appveyor-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-appveyor-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-appveyor-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-appveyor-cpu_time_ms.html
Built target ryml-bm-parse-appveyor-plot
/usr/bin/python3.11
Python 3.11.8
emit
appveyor
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-appveyor.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-appveyor.json
+----------------------------------------------------------------------------------------------+
|                                   emit benchmark: appveyor                                   |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  |  326.86 |  55.95x | 5494.61% |          0.01 |       0.02x |     -98.21% |
| ryml_str          |  318.61 |  54.53x | 5353.35% |          0.01 |       0.02x |     -98.17% |
| ryml_ostream      |    3.95 |   0.68x |  -32.31% |          0.54 |       1.48x |      47.74% |
| fyaml_str_reserve |   73.96 |  12.66x | 1166.00% |          0.03 |       0.08x |     -92.10% |
| fyaml_str         |   73.97 |  12.66x | 1166.12% |          0.03 |       0.08x |     -92.10% |
| fyaml_ostream     |    1.66 |   0.28x |  -71.61% |          1.28 |       3.52x |     252.29% |
| yamlcpp_          |    5.84 |   1.00x |    0.00% |          0.36 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-appveyor-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-appveyor-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-appveyor-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-appveyor-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-appveyor-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-appveyor-cpu_time_ms.html
Built target ryml-bm-emit-appveyor-plot
/usr/bin/python3.11
Python 3.11.8
parse
compile_commands
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-compile_commands.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-compile_commands.json
+------------------------------------------------------------------------------------------------+
|                               parse benchmark: compile_commands                                |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |  660.78 |  33.58x | 3257.79% |          0.07 |       0.03x |     -97.02% |
| ryml_arena_reuse    |  653.74 |  33.22x | 3222.04% |          0.07 |       0.03x |     -96.99% |
| ryml_inplace        |  516.65 |  26.25x | 2525.39% |          0.09 |       0.04x |     -96.19% |
| ryml_arena          |  513.00 |  26.07x | 2506.85% |          0.09 |       0.04x |     -96.16% |
| libyaml_arena       |  119.58 |   6.08x |  507.64% |          0.39 |       0.16x |     -83.54% |
| libyaml_arena_reuse |  122.10 |   6.20x |  520.48% |          0.38 |       0.16x |     -83.88% |
| libfyaml_arena      |  148.56 |   7.55x |  654.91% |          0.32 |       0.13x |     -86.75% |
| yamlcpp_arena       |   19.68 |   1.00x |    0.00% |          2.38 |       1.00x |       0.00% |
| rapidjson_arena     |  669.65 |  34.03x | 3302.86% |          0.07 |       0.03x |     -97.06% |
| rapidjson_inplace   | 1712.48 |  87.02x | 8602.08% |          0.03 |       0.01x |     -98.85% |
| sajson_arena        |  584.94 |  29.72x | 2872.41% |          0.08 |       0.03x |     -96.64% |
| sajson_inplace      |  588.83 |  29.92x | 2892.19% |          0.08 |       0.03x |     -96.66% |
| jsoncpp_arena       |  282.43 |  14.35x | 1335.16% |          0.17 |       0.07x |     -93.03% |
| nlohmann_arena      |  215.67 |  10.96x |  995.93% |          0.22 |       0.09x |     -90.88% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-compile_commands-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-compile_commands-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-compile_commands-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-compile_commands-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-compile_commands-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-compile_commands-cpu_time_ms.html
Built target ryml-bm-parse-compile_commands-plot
/usr/bin/python3.11
Python 3.11.8
emit
compile_commands
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-compile_commands.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-compile_commands.json
+-----------------------------------------------------------------------------------------------+
|                                emit benchmark: compile_commands                               |
+-------------------+---------+---------+-----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |   MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+-----------+---------------+-------------+-------------+
| ryml_str_reserve  |  455.30 |  71.05x |  7004.59% |          0.10 |       0.01x |     -98.59% |
| ryml_str          |  455.25 |  71.04x |  7003.87% |          0.10 |       0.01x |     -98.59% |
| ryml_ostream      |    7.78 |   1.21x |    21.42% |          6.02 |       0.82x |     -17.64% |
| fyaml_str_reserve |  431.69 |  67.36x |  6636.20% |          0.11 |       0.01x |     -98.52% |
| fyaml_str         |  431.42 |  67.32x |  6631.98% |          0.11 |       0.01x |     -98.51% |
| fyaml_ostream     |   11.29 |   1.76x |    76.13% |          4.15 |       0.57x |     -43.22% |
| yamlcpp_          |    6.41 |   1.00x |     0.00% |          7.31 |       1.00x |       0.00% |
| rapidjson_        |  698.22 | 108.95x | 10795.20% |          0.07 |       0.01x |     -99.08% |
| jsoncpp_          |   11.18 |   1.74x |    74.48% |          4.19 |       0.57x |     -42.69% |
| nlohmann_         |   11.63 |   1.82x |    81.53% |          4.03 |       0.55x |     -44.91% |
+-------------------+---------+---------+-----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-compile_commands-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-compile_commands-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-compile_commands-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-compile_commands-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-compile_commands-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-compile_commands-cpu_time_ms.html
Built target ryml-bm-emit-compile_commands-plot
/usr/bin/python3.11
Python 3.11.8
parse
travis
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-travis.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-travis.json
+------------------------------------------------------------------------------------------------+
|                                    parse benchmark: travis                                     |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |  203.08 |  19.59x | 1858.71% |          0.02 |       0.05x |     -94.89% |
| ryml_arena_reuse    |  202.41 |  19.52x | 1852.25% |          0.02 |       0.05x |     -94.88% |
| ryml_inplace        |  172.06 |  16.60x | 1559.50% |          0.02 |       0.06x |     -93.97% |
| ryml_arena          |  174.55 |  16.84x | 1583.55% |          0.02 |       0.06x |     -94.06% |
| libyaml_arena       |   77.93 |   7.52x |  651.63% |          0.05 |       0.13x |     -86.70% |
| libyaml_arena_reuse |   76.93 |   7.42x |  641.95% |          0.05 |       0.13x |     -86.52% |
| libfyaml_arena      |   57.13 |   5.51x |  450.97% |          0.07 |       0.18x |     -81.85% |
| yamlcpp_arena       |   10.37 |   1.00x |    0.00% |          0.39 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-travis-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-travis-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-travis-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-travis-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-travis-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-travis-cpu_time_ms.html
Built target ryml-bm-parse-travis-plot
/usr/bin/python3.11
Python 3.11.8
emit
travis
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-travis.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-travis.json
+----------------------------------------------------------------------------------------------+
|                                    emit benchmark: travis                                    |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  |  349.51 |  44.06x | 4305.79% |          0.01 |       0.02x |     -97.73% |
| ryml_str          |  352.93 |  44.49x | 4348.84% |          0.01 |       0.02x |     -97.75% |
| ryml_ostream      |   12.84 |   1.62x |   61.89% |          0.32 |       0.62x |     -38.23% |
| fyaml_str_reserve |   61.60 |   7.76x |  676.44% |          0.07 |       0.13x |     -87.12% |
| fyaml_str         |   59.55 |   7.51x |  650.62% |          0.07 |       0.13x |     -86.68% |
| fyaml_ostream     |    4.18 |   0.53x |  -47.32% |          0.97 |       1.90x |      89.83% |
| yamlcpp_          |    7.93 |   1.00x |    0.00% |          0.51 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-travis-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-travis-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-travis-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-travis-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-travis-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-travis-cpu_time_ms.html
Built target ryml-bm-emit-travis-plot
/usr/bin/python3.11
Python 3.11.8
parse
scalar_dquot_multiline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_multiline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_multiline.json
+------------------------------------------------------------------------------------------------+
|                            parse benchmark: scalar_dquot_multiline                             |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |  235.72 |  11.09x | 1008.77% |          0.31 |       0.09x |     -90.98% |
| ryml_arena_reuse    |  226.88 |  10.67x |  967.19% |          0.32 |       0.09x |     -90.63% |
| ryml_inplace        |  207.12 |   9.74x |  874.28% |          0.35 |       0.10x |     -89.74% |
| ryml_arena          |  200.13 |   9.41x |  841.39% |          0.36 |       0.11x |     -89.38% |
| libyaml_arena       |  137.93 |   6.49x |  548.78% |          0.52 |       0.15x |     -84.59% |
| libyaml_arena_reuse |  139.42 |   6.56x |  555.81% |          0.52 |       0.15x |     -84.75% |
| libfyaml_arena      |  170.75 |   8.03x |  703.19% |          0.42 |       0.12x |     -87.55% |
| yamlcpp_arena       |   21.26 |   1.00x |    0.00% |          3.40 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_multiline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_multiline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_multiline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_multiline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_multiline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_multiline-cpu_time_ms.html
Built target ryml-bm-parse-scalar_dquot_multiline-plot
/usr/bin/python3.11
Python 3.11.8
emit
scalar_dquot_multiline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_multiline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_multiline.json
+-----------------------------------------------------------------------------------------------+
|                             emit benchmark: scalar_dquot_multiline                            |
+-------------------+---------+---------+-----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |   MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+-----------+---------------+-------------+-------------+
| ryml_str_reserve  |  910.79 | 103.81x | 10280.56% |          0.08 |       0.01x |     -99.04% |
| ryml_str          |  913.34 | 104.10x | 10309.55% |          0.08 |       0.01x |     -99.04% |
| ryml_ostream      |    9.82 |   1.12x |    11.96% |          7.35 |       0.89x |     -10.68% |
| fyaml_str_reserve |   45.26 |   5.16x |   415.84% |          1.59 |       0.19x |     -80.61% |
| fyaml_str         |   45.28 |   5.16x |   416.11% |          1.59 |       0.19x |     -80.62% |
| fyaml_ostream     |   26.71 |   3.04x |   204.48% |          2.70 |       0.33x |     -67.16% |
| yamlcpp_          |    8.77 |   1.00x |     0.00% |          8.23 |       1.00x |       0.00% |
+-------------------+---------+---------+-----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_multiline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_multiline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_multiline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_multiline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_multiline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_multiline-cpu_time_ms.html
Built target ryml-bm-emit-scalar_dquot_multiline-plot
/usr/bin/python3.11
Python 3.11.8
parse
scalar_dquot_singleline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_singleline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_singleline.json
+------------------------------------------------------------------------------------------------+
|                            parse benchmark: scalar_dquot_singleline                            |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |  356.40 |  25.61x | 2461.47% |          0.01 |       0.04x |     -96.10% |
| ryml_arena_reuse    |  356.84 |  25.65x | 2464.65% |          0.01 |       0.04x |     -96.10% |
| ryml_inplace        |  300.24 |  21.58x | 2057.88% |          0.02 |       0.05x |     -95.37% |
| ryml_arena          |  303.78 |  21.83x | 2083.29% |          0.02 |       0.05x |     -95.42% |
| libyaml_arena       |   75.55 |   5.43x |  443.01% |          0.06 |       0.18x |     -81.58% |
| libyaml_arena_reuse |   76.42 |   5.49x |  449.26% |          0.06 |       0.18x |     -81.79% |
| libfyaml_arena      |   58.32 |   4.19x |  319.14% |          0.08 |       0.24x |     -76.14% |
| yamlcpp_arena       |   13.91 |   1.00x |    0.00% |          0.35 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_singleline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_singleline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_singleline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_singleline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_singleline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_singleline-cpu_time_ms.html
Built target ryml-bm-parse-scalar_dquot_singleline-plot
/usr/bin/python3.11
Python 3.11.8
emit
scalar_dquot_singleline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_singleline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_singleline.json
+----------------------------------------------------------------------------------------------+
|                           emit benchmark: scalar_dquot_singleline                            |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  |  353.35 |  56.07x | 5506.71% |          0.01 |       0.02x |     -98.22% |
| ryml_str          |  354.96 |  56.32x | 5532.31% |          0.01 |       0.02x |     -98.22% |
| ryml_ostream      |    7.99 |   1.27x |   26.79% |          0.60 |       0.79x |     -21.13% |
| fyaml_str_reserve |  189.98 |  30.14x | 2914.45% |          0.03 |       0.03x |     -96.68% |
| fyaml_str         |  190.07 |  30.16x | 2915.99% |          0.03 |       0.03x |     -96.68% |
| fyaml_ostream     |    2.78 |   0.44x |  -55.85% |          1.73 |       2.26x |     126.49% |
| yamlcpp_          |    6.30 |   1.00x |    0.00% |          0.77 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_singleline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_singleline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_singleline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_singleline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_singleline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_singleline-cpu_time_ms.html
Built target ryml-bm-emit-scalar_dquot_singleline-plot
/usr/bin/python3.11
Python 3.11.8
parse
scalar_squot_multiline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_multiline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_multiline.json
+------------------------------------------------------------------------------------------------+
|                            parse benchmark: scalar_squot_multiline                             |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |  376.49 |  15.36x | 1435.79% |          0.19 |       0.07x |     -93.49% |
| ryml_arena_reuse    |  380.14 |  15.51x | 1450.65% |          0.19 |       0.06x |     -93.55% |
| ryml_inplace        |  323.31 |  13.19x | 1218.84% |          0.22 |       0.08x |     -92.42% |
| ryml_arena          |  290.26 |  11.84x | 1084.02% |          0.25 |       0.08x |     -91.55% |
| libyaml_arena       |  141.28 |   5.76x |  476.31% |          0.51 |       0.17x |     -82.65% |
| libyaml_arena_reuse |  143.01 |   5.83x |  483.38% |          0.51 |       0.17x |     -82.86% |
| libfyaml_arena      |  170.37 |   6.95x |  594.97% |          0.42 |       0.14x |     -85.61% |
| yamlcpp_arena       |   24.51 |   1.00x |    0.00% |          2.95 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_multiline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_multiline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_multiline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_multiline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_multiline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_multiline-cpu_time_ms.html
Built target ryml-bm-parse-scalar_squot_multiline-plot
/usr/bin/python3.11
Python 3.11.8
emit
scalar_squot_multiline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_multiline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_multiline.json
+----------------------------------------------------------------------------------------------+
|                            emit benchmark: scalar_squot_multiline                            |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  |  829.14 |  98.01x | 9700.51% |          0.09 |       0.01x |     -98.98% |
| ryml_str          |  827.75 |  97.84x | 9684.06% |          0.09 |       0.01x |     -98.98% |
| ryml_ostream      |    9.73 |   1.15x |   14.99% |          7.43 |       0.87x |     -13.04% |
| fyaml_str_reserve |   45.42 |   5.37x |  436.89% |          1.59 |       0.19x |     -81.37% |
| fyaml_str         |   46.36 |   5.48x |  447.99% |          1.56 |       0.18x |     -81.75% |
| fyaml_ostream     |   27.22 |   3.22x |  221.76% |          2.66 |       0.31x |     -68.92% |
| yamlcpp_          |    8.46 |   1.00x |    0.00% |          8.54 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_multiline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_multiline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_multiline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_multiline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_multiline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_multiline-cpu_time_ms.html
Built target ryml-bm-emit-scalar_squot_multiline-plot
/usr/bin/python3.11
Python 3.11.8
parse
scalar_squot_singleline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_singleline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_singleline.json
+------------------------------------------------------------------------------------------------+
|                            parse benchmark: scalar_squot_singleline                            |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |  360.23 |  25.15x | 2415.12% |          0.01 |       0.04x |     -96.02% |
| ryml_arena_reuse    |  354.71 |  24.77x | 2376.60% |          0.01 |       0.04x |     -95.96% |
| ryml_inplace        |  299.92 |  20.94x | 1994.05% |          0.02 |       0.05x |     -95.22% |
| ryml_arena          |  308.82 |  21.56x | 2056.24% |          0.02 |       0.05x |     -95.36% |
| libyaml_arena       |   79.69 |   5.56x |  456.40% |          0.06 |       0.18x |     -82.03% |
| libyaml_arena_reuse |   80.55 |   5.62x |  462.43% |          0.06 |       0.18x |     -82.22% |
| libfyaml_arena      |   57.59 |   4.02x |  302.10% |          0.08 |       0.25x |     -75.13% |
| yamlcpp_arena       |   14.32 |   1.00x |    0.00% |          0.34 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_singleline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_singleline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_singleline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_singleline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_singleline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_singleline-cpu_time_ms.html
Built target ryml-bm-parse-scalar_squot_singleline-plot
/usr/bin/python3.11
Python 3.11.8
emit
scalar_squot_singleline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_singleline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_singleline.json
+----------------------------------------------------------------------------------------------+
|                           emit benchmark: scalar_squot_singleline                            |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  |  351.74 |  55.52x | 5451.81% |          0.01 |       0.02x |     -98.20% |
| ryml_str          |  359.53 |  56.75x | 5574.83% |          0.01 |       0.02x |     -98.24% |
| ryml_ostream      |    7.33 |   1.16x |   15.70% |          0.66 |       0.86x |     -13.57% |
| fyaml_str_reserve |  168.62 |  26.62x | 2561.52% |          0.03 |       0.04x |     -96.24% |
| fyaml_str         |  168.37 |  26.57x | 2557.49% |          0.03 |       0.04x |     -96.24% |
| fyaml_ostream     |    2.79 |   0.44x |  -55.93% |          1.73 |       2.27x |     126.92% |
| yamlcpp_          |    6.34 |   1.00x |    0.00% |          0.76 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_singleline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_singleline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_singleline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_singleline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_singleline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_singleline-cpu_time_ms.html
Built target ryml-bm-emit-scalar_squot_singleline-plot
/usr/bin/python3.11
Python 3.11.8
parse
scalar_plain_multiline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_multiline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_multiline.json
+------------------------------------------------------------------------------------------------+
|                            parse benchmark: scalar_plain_multiline                             |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |  215.51 |  16.16x | 1516.00% |          0.33 |       0.06x |     -93.81% |
| ryml_arena_reuse    |  215.33 |  16.15x | 1514.65% |          0.33 |       0.06x |     -93.81% |
| ryml_inplace        |  195.59 |  14.67x | 1366.67% |          0.37 |       0.07x |     -93.18% |
| ryml_arena          |  182.99 |  13.72x | 1272.15% |          0.39 |       0.07x |     -92.71% |
| libyaml_arena       |  135.27 |  10.14x |  914.32% |          0.53 |       0.10x |     -90.14% |
| libyaml_arena_reuse |  135.74 |  10.18x |  917.83% |          0.53 |       0.10x |     -90.18% |
| libfyaml_arena      |  164.50 |  12.33x | 1133.50% |          0.44 |       0.08x |     -91.89% |
| yamlcpp_arena       |   13.34 |   1.00x |    0.00% |          5.40 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_multiline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_multiline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_multiline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_multiline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_multiline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_multiline-cpu_time_ms.html
Built target ryml-bm-parse-scalar_plain_multiline-plot
/usr/bin/python3.11
Python 3.11.8
emit
scalar_plain_multiline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_multiline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_multiline.json
+----------------------------------------------------------------------------------------------+
|                            emit benchmark: scalar_plain_multiline                            |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  |  691.83 |  83.42x | 8242.32% |          0.10 |       0.01x |     -98.80% |
| ryml_str          |  688.49 |  83.02x | 8202.03% |          0.10 |       0.01x |     -98.80% |
| ryml_ostream      |   10.79 |   1.30x |   30.11% |          6.68 |       0.77x |     -23.14% |
| fyaml_str_reserve |   50.24 |   6.06x |  505.85% |          1.43 |       0.17x |     -83.49% |
| fyaml_str         |   50.59 |   6.10x |  510.08% |          1.42 |       0.16x |     -83.61% |
| fyaml_ostream     |   27.01 |   3.26x |  225.73% |          2.67 |       0.31x |     -69.30% |
| yamlcpp_          |    8.29 |   1.00x |    0.00% |          8.69 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_multiline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_multiline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_multiline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_multiline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_multiline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_multiline-cpu_time_ms.html
Built target ryml-bm-emit-scalar_plain_multiline-plot
/usr/bin/python3.11
Python 3.11.8
parse
scalar_plain_singleline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_singleline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_singleline.json
+------------------------------------------------------------------------------------------------+
|                            parse benchmark: scalar_plain_singleline                            |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |  161.27 |  16.78x | 1578.05% |          0.03 |       0.06x |     -94.04% |
| ryml_arena_reuse    |  160.73 |  16.72x | 1572.43% |          0.03 |       0.06x |     -94.02% |
| ryml_inplace        |  148.01 |  15.40x | 1440.07% |          0.03 |       0.06x |     -93.51% |
| ryml_arena          |  149.55 |  15.56x | 1456.06% |          0.03 |       0.06x |     -93.57% |
| libyaml_arena       |   72.39 |   7.53x |  653.25% |          0.06 |       0.13x |     -86.72% |
| libyaml_arena_reuse |   74.16 |   7.72x |  671.66% |          0.06 |       0.13x |     -87.04% |
| libfyaml_arena      |   55.51 |   5.78x |  477.54% |          0.08 |       0.17x |     -82.69% |
| yamlcpp_arena       |    9.61 |   1.00x |    0.00% |          0.49 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_singleline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_singleline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_singleline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_singleline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_singleline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_singleline-cpu_time_ms.html
Built target ryml-bm-parse-scalar_plain_singleline-plot
/usr/bin/python3.11
Python 3.11.8
emit
scalar_plain_singleline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_singleline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_singleline.json
+----------------------------------------------------------------------------------------------+
|                           emit benchmark: scalar_plain_singleline                            |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  |  249.80 |  41.42x | 4041.55% |          0.02 |       0.02x |     -97.59% |
| ryml_str          |  263.67 |  43.72x | 4271.55% |          0.02 |       0.02x |     -97.71% |
| ryml_ostream      |    8.40 |   1.39x |   39.31% |          0.56 |       0.72x |     -28.22% |
| fyaml_str_reserve |   44.17 |   7.32x |  632.37% |          0.11 |       0.14x |     -86.35% |
| fyaml_str         |   43.81 |   7.26x |  626.43% |          0.11 |       0.14x |     -86.23% |
| fyaml_ostream     |    5.06 |   0.84x |  -16.09% |          0.92 |       1.19x |      19.18% |
| yamlcpp_          |    6.03 |   1.00x |    0.00% |          0.78 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_singleline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_singleline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_singleline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_singleline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_singleline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_singleline-cpu_time_ms.html
Built target ryml-bm-emit-scalar_plain_singleline-plot
/usr/bin/python3.11
Python 3.11.8
parse
scalar_block_literal_multiline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_multiline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_multiline.json
+------------------------------------------------------------------------------------------------+
|                        parse benchmark: scalar_block_literal_multiline                         |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |  616.01 |  22.94x | 2193.75% |          0.12 |       0.04x |     -95.64% |
| ryml_arena_reuse    |  607.96 |  22.64x | 2163.79% |          0.12 |       0.04x |     -95.58% |
| ryml_inplace        |  478.08 |  17.80x | 1680.16% |          0.15 |       0.06x |     -94.38% |
| ryml_arena          |  417.65 |  15.55x | 1455.13% |          0.17 |       0.06x |     -93.57% |
| libyaml_arena       |  163.59 |   6.09x |  509.13% |          0.44 |       0.16x |     -83.58% |
| libyaml_arena_reuse |  165.82 |   6.17x |  517.42% |          0.44 |       0.16x |     -83.80% |
| libfyaml_arena      |  177.19 |   6.60x |  559.79% |          0.41 |       0.15x |     -84.84% |
| yamlcpp_arena       |   26.86 |   1.00x |    0.00% |          2.70 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_multiline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_multiline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_multiline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_multiline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_multiline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_multiline-cpu_time_ms.html
Built target ryml-bm-parse-scalar_block_literal_multiline-plot
/usr/bin/python3.11
Python 3.11.8
emit
scalar_block_literal_multiline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_multiline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_multiline.json
+----------------------------------------------------------------------------------------------+
|                        emit benchmark: scalar_block_literal_multiline                        |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  | 1459.36 |  64.22x | 6322.20% |          0.05 |       0.02x |     -98.44% |
| ryml_str          | 1492.24 |  65.67x | 6466.85% |          0.05 |       0.02x |     -98.48% |
| ryml_ostream      |   10.67 |   0.47x |  -53.03% |          6.80 |       2.13x |     112.89% |
| fyaml_str_reserve |   49.30 |   2.17x |  116.93% |          1.47 |       0.46x |     -53.90% |
| fyaml_str         |   49.26 |   2.17x |  116.76% |          1.47 |       0.46x |     -53.87% |
| fyaml_ostream     |   27.12 |   1.19x |   19.35% |          2.68 |       0.84x |     -16.21% |
| yamlcpp_          |   22.72 |   1.00x |    0.00% |          3.19 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_multiline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_multiline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_multiline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_multiline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_multiline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_multiline-cpu_time_ms.html
Built target ryml-bm-emit-scalar_block_literal_multiline-plot
/usr/bin/python3.11
Python 3.11.8
parse
scalar_block_literal_singleline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_singleline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_singleline.json
+------------------------------------------------------------------------------------------------+
|                        parse benchmark: scalar_block_literal_singleline                        |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |  266.12 |  16.89x | 1589.29% |          0.02 |       0.06x |     -94.08% |
| ryml_arena_reuse    |  264.83 |  16.81x | 1581.07% |          0.02 |       0.06x |     -94.05% |
| ryml_inplace        |  230.58 |  14.64x | 1363.64% |          0.02 |       0.07x |     -93.17% |
| ryml_arena          |  232.06 |  14.73x | 1373.09% |          0.02 |       0.07x |     -93.21% |
| libyaml_arena       |   89.26 |   5.67x |  466.63% |          0.06 |       0.18x |     -82.35% |
| libyaml_arena_reuse |   92.57 |   5.88x |  487.61% |          0.06 |       0.17x |     -82.98% |
| libfyaml_arena      |   63.58 |   4.04x |  303.62% |          0.08 |       0.25x |     -75.22% |
| yamlcpp_arena       |   15.75 |   1.00x |    0.00% |          0.33 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_singleline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_singleline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_singleline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_singleline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_singleline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_singleline-cpu_time_ms.html
Built target ryml-bm-parse-scalar_block_literal_singleline-plot
/usr/bin/python3.11
Python 3.11.8
emit
scalar_block_literal_singleline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_singleline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_singleline.json
+----------------------------------------------------------------------------------------------+
|                       emit benchmark: scalar_block_literal_singleline                        |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  |  596.51 |  96.39x | 9539.43% |          0.01 |       0.01x |     -98.96% |
| ryml_str          |  595.08 |  96.16x | 9516.37% |          0.01 |       0.01x |     -98.96% |
| ryml_ostream      |    8.50 |   1.37x |   37.43% |          0.62 |       0.73x |     -27.24% |
| fyaml_str_reserve |   46.64 |   7.54x |  653.61% |          0.11 |       0.13x |     -86.73% |
| fyaml_str         |   46.58 |   7.53x |  652.72% |          0.11 |       0.13x |     -86.71% |
| fyaml_ostream     |    5.20 |   0.84x |  -15.93% |          1.01 |       1.19x |      18.95% |
| yamlcpp_          |    6.19 |   1.00x |    0.00% |          0.85 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_singleline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_singleline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_singleline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_singleline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_singleline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_singleline-cpu_time_ms.html
Built target ryml-bm-emit-scalar_block_literal_singleline-plot
/usr/bin/python3.11
Python 3.11.8
parse
scalar_block_folded_multiline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_multiline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_multiline.json
+------------------------------------------------------------------------------------------------+
|                         parse benchmark: scalar_block_folded_multiline                         |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |  635.19 |  23.73x | 2273.45% |          0.11 |       0.04x |     -95.79% |
| ryml_arena_reuse    |  627.43 |  23.44x | 2244.45% |          0.12 |       0.04x |     -95.73% |
| ryml_inplace        |  488.44 |  18.25x | 1725.11% |          0.15 |       0.05x |     -94.52% |
| ryml_arena          |  417.46 |  15.60x | 1459.87% |          0.17 |       0.06x |     -93.59% |
| libyaml_arena       |  164.99 |   6.17x |  516.50% |          0.44 |       0.16x |     -83.78% |
| libyaml_arena_reuse |  165.53 |   6.19x |  518.53% |          0.44 |       0.16x |     -83.83% |
| libfyaml_arena      |  176.49 |   6.59x |  559.48% |          0.41 |       0.15x |     -84.84% |
| yamlcpp_arena       |   26.76 |   1.00x |    0.00% |          2.71 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_multiline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_multiline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_multiline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_multiline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_multiline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_multiline-cpu_time_ms.html
Built target ryml-bm-parse-scalar_block_folded_multiline-plot
/usr/bin/python3.11
Python 3.11.8
emit
scalar_block_folded_multiline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_multiline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_multiline.json
+-----------------------------------------------------------------------------------------------+
|                         emit benchmark: scalar_block_folded_multiline                         |
+-------------------+---------+---------+-----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |   MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+-----------+---------------+-------------+-------------+
| ryml_str_reserve  | 1052.44 | 139.05x | 13804.87% |          0.07 |       0.01x |     -99.28% |
| ryml_str          | 1050.60 | 138.81x | 13780.56% |          0.07 |       0.01x |     -99.28% |
| ryml_ostream      |   10.39 |   1.37x |    37.30% |          6.98 |       0.73x |     -27.16% |
| fyaml_str_reserve |   48.56 |   6.42x |   541.51% |          1.49 |       0.16x |     -84.41% |
| fyaml_str         |   48.22 |   6.37x |   537.05% |          1.51 |       0.16x |     -84.30% |
| fyaml_ostream     |   27.60 |   3.65x |   264.69% |          2.63 |       0.27x |     -72.58% |
| yamlcpp_          |    7.57 |   1.00x |     0.00% |          9.59 |       1.00x |       0.00% |
+-------------------+---------+---------+-----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_multiline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_multiline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_multiline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_multiline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_multiline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_multiline-cpu_time_ms.html
Built target ryml-bm-emit-scalar_block_folded_multiline-plot
/usr/bin/python3.11
Python 3.11.8
parse
scalar_block_folded_singleline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_singleline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_singleline.json
+------------------------------------------------------------------------------------------------+
|                        parse benchmark: scalar_block_folded_singleline                         |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |  268.54 |  16.99x | 1599.38% |          0.02 |       0.06x |     -94.12% |
| ryml_arena_reuse    |  267.22 |  16.91x | 1591.06% |          0.02 |       0.06x |     -94.09% |
| ryml_inplace        |  227.85 |  14.42x | 1341.92% |          0.02 |       0.07x |     -93.06% |
| ryml_arena          |  226.79 |  14.35x | 1335.19% |          0.02 |       0.07x |     -93.03% |
| libyaml_arena       |   88.87 |   5.62x |  462.39% |          0.06 |       0.18x |     -82.22% |
| libyaml_arena_reuse |   92.32 |   5.84x |  484.22% |          0.06 |       0.17x |     -82.88% |
| libfyaml_arena      |   61.20 |   3.87x |  287.29% |          0.09 |       0.26x |     -74.18% |
| yamlcpp_arena       |   15.80 |   1.00x |    0.00% |          0.33 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_singleline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_singleline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_singleline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_singleline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_singleline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_singleline-cpu_time_ms.html
Built target ryml-bm-parse-scalar_block_folded_singleline-plot
/usr/bin/python3.11
Python 3.11.8
emit
scalar_block_folded_singleline
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_singleline.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_singleline.json
+----------------------------------------------------------------------------------------------+
|                        emit benchmark: scalar_block_folded_singleline                        |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  |  594.27 |  95.04x | 9403.59% |          0.01 |       0.01x |     -98.95% |
| ryml_str          |  594.52 |  95.08x | 9407.64% |          0.01 |       0.01x |     -98.95% |
| ryml_ostream      |    8.69 |   1.39x |   39.05% |          0.60 |       0.72x |     -28.08% |
| fyaml_str_reserve |   43.74 |   6.99x |  599.43% |          0.12 |       0.14x |     -85.70% |
| fyaml_str         |   43.93 |   7.03x |  602.51% |          0.12 |       0.14x |     -85.77% |
| fyaml_ostream     |    5.51 |   0.88x |  -11.90% |          0.95 |       1.14x |      13.51% |
| yamlcpp_          |    6.25 |   1.00x |    0.00% |          0.84 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_singleline-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_singleline-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_singleline-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_singleline-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_singleline-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_singleline-cpu_time_ms.html
Built target ryml-bm-emit-scalar_block_folded_singleline-plot
/usr/bin/python3.11
Python 3.11.8
parse
style_maps_blck_outer1000_inner10
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner10.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner10.json
+------------------------------------------------------------------------------------------------+
|                       parse benchmark: style_maps_blck_outer1000_inner10                       |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |   73.46 |  20.34x | 1933.56% |          1.21 |       0.05x |     -95.08% |
| ryml_arena_reuse    |   73.00 |  20.21x | 1920.62% |          1.22 |       0.05x |     -95.05% |
| ryml_inplace        |   56.20 |  15.56x | 1455.64% |          1.58 |       0.06x |     -93.57% |
| ryml_arena          |   55.80 |  15.45x | 1444.53% |          1.59 |       0.06x |     -93.53% |
| libyaml_arena       |   24.40 |   6.75x |  575.41% |          3.64 |       0.15x |     -85.19% |
| libyaml_arena_reuse |   25.14 |   6.96x |  595.78% |          3.54 |       0.14x |     -85.63% |
| libfyaml_arena      |   11.14 |   3.08x |  208.44% |          7.98 |       0.32x |     -67.58% |
| yamlcpp_arena       |    3.61 |   1.00x |    0.00% |         24.61 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner10-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner10-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner10-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner10-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner10-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner10-cpu_time_ms.html
Built target ryml-bm-parse-style_maps_blck_outer1000_inner10-plot
/usr/bin/python3.11
Python 3.11.8
emit
style_maps_blck_outer1000_inner10
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner10.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner10.json
+----------------------------------------------------------------------------------------------+
|                      emit benchmark: style_maps_blck_outer1000_inner10                       |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  |  194.63 |  28.47x | 2747.13% |          0.46 |       0.04x |     -96.49% |
| ryml_str          |  194.27 |  28.42x | 2741.91% |          0.46 |       0.04x |     -96.48% |
| ryml_ostream      |   19.10 |   2.79x |  179.37% |          4.66 |       0.36x |     -64.21% |
| fyaml_str_reserve |   27.29 |   3.99x |  299.17% |          3.26 |       0.25x |     -74.95% |
| fyaml_str         |   26.92 |   3.94x |  293.86% |          3.30 |       0.25x |     -74.61% |
| fyaml_ostream     |   19.90 |   2.91x |  191.12% |          4.47 |       0.34x |     -65.65% |
| yamlcpp_          |    6.84 |   1.00x |    0.00% |         13.00 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner10-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner10-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner10-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner10-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner10-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner10-cpu_time_ms.html
Built target ryml-bm-emit-style_maps_blck_outer1000_inner10-plot
/usr/bin/python3.11
Python 3.11.8
parse
style_maps_blck_outer1000_inner100
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner100.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner100.json
+------------------------------------------------------------------------------------------------+
|                      parse benchmark: style_maps_blck_outer1000_inner100                       |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |   82.75 |  23.54x | 2253.80% |         11.83 |       0.04x |     -95.75% |
| ryml_arena_reuse    |   81.00 |  23.04x | 2204.09% |         12.09 |       0.04x |     -95.66% |
| ryml_inplace        |   59.36 |  16.88x | 1588.43% |         16.49 |       0.06x |     -94.08% |
| ryml_arena          |   59.92 |  17.04x | 1604.34% |         16.34 |       0.06x |     -94.13% |
| libyaml_arena       |   27.59 |   7.85x |  684.85% |         35.48 |       0.13x |     -87.26% |
| libyaml_arena_reuse |   28.14 |   8.00x |  700.46% |         34.79 |       0.12x |     -87.51% |
| libfyaml_arena      |    4.14 |   1.18x |   17.73% |        236.55 |       0.85x |     -15.06% |
| yamlcpp_arena       |    3.52 |   1.00x |    0.00% |        278.48 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner100-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner100-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner100-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner100-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner100-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner100-cpu_time_ms.html
Built target ryml-bm-parse-style_maps_blck_outer1000_inner100-plot
/usr/bin/python3.11
Python 3.11.8
emit
style_maps_blck_outer1000_inner100
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner100.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner100.json
+----------------------------------------------------------------------------------------------+
|                      emit benchmark: style_maps_blck_outer1000_inner100                      |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  |  211.48 |  31.91x | 3090.51% |          4.63 |       0.03x |     -96.87% |
| ryml_str          |  210.93 |  31.82x | 3082.11% |          4.64 |       0.03x |     -96.86% |
| ryml_ostream      |   56.96 |   8.59x |  759.34% |         17.19 |       0.12x |     -88.36% |
| fyaml_str_reserve |   26.32 |   3.97x |  297.04% |         37.20 |       0.25x |     -74.81% |
| fyaml_str         |   26.22 |   3.96x |  295.57% |         37.34 |       0.25x |     -74.72% |
| fyaml_ostream     |   23.22 |   3.50x |  250.33% |         42.16 |       0.29x |     -71.46% |
| yamlcpp_          |    6.63 |   1.00x |    0.00% |        147.69 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner100-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner100-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner100-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner100-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner100-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner100-cpu_time_ms.html
Built target ryml-bm-emit-style_maps_blck_outer1000_inner100-plot
/usr/bin/python3.11
Python 3.11.8
parse
style_maps_flow_outer1000_inner10
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10.json
+------------------------------------------------------------------------------------------------+
|                       parse benchmark: style_maps_flow_outer1000_inner10                       |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |   86.52 |  28.59x | 2759.13% |          0.84 |       0.03x |     -96.50% |
| ryml_arena_reuse    |   86.11 |  28.45x | 2745.46% |          0.85 |       0.04x |     -96.49% |
| ryml_inplace        |   61.25 |  20.24x | 1923.95% |          1.19 |       0.05x |     -95.06% |
| ryml_arena          |   61.69 |  20.39x | 1938.64% |          1.18 |       0.05x |     -95.09% |
| libyaml_arena       |   14.61 |   4.83x |  382.75% |          4.99 |       0.21x |     -79.29% |
| libyaml_arena_reuse |   19.56 |   6.46x |  546.47% |          3.73 |       0.15x |     -84.53% |
| libfyaml_arena      |    9.11 |   3.01x |  201.08% |          8.00 |       0.33x |     -66.79% |
| yamlcpp_arena       |    3.03 |   1.00x |    0.00% |         24.09 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10-cpu_time_ms.html
Built target ryml-bm-parse-style_maps_flow_outer1000_inner10-plot
/usr/bin/python3.11
Python 3.11.8
emit
style_maps_flow_outer1000_inner10
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10.json
+----------------------------------------------------------------------------------------------+
|                      emit benchmark: style_maps_flow_outer1000_inner10                       |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  |  161.55 |  31.01x | 3000.89% |          0.45 |       0.03x |     -96.78% |
| ryml_str          |  161.66 |  31.03x | 3002.93% |          0.45 |       0.03x |     -96.78% |
| ryml_ostream      |   16.57 |   3.18x |  218.03% |          4.40 |       0.31x |     -68.56% |
| fyaml_str_reserve |   37.41 |   7.18x |  618.08% |          1.95 |       0.14x |     -86.07% |
| fyaml_str         |   37.22 |   7.14x |  614.35% |          1.96 |       0.14x |     -86.00% |
| fyaml_ostream     |   18.37 |   3.53x |  252.59% |          3.97 |       0.28x |     -71.64% |
| yamlcpp_          |    5.21 |   1.00x |    0.00% |         13.99 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10-cpu_time_ms.html
Built target ryml-bm-emit-style_maps_flow_outer1000_inner10-plot
/usr/bin/python3.11
Python 3.11.8
parse
style_maps_flow_outer1000_inner100
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100.json
+------------------------------------------------------------------------------------------------+
|                      parse benchmark: style_maps_flow_outer1000_inner100                       |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |   98.84 |  36.53x | 3552.92% |          7.92 |       0.03x |     -97.26% |
| ryml_arena_reuse    |   97.59 |  36.07x | 3506.87% |          8.02 |       0.03x |     -97.23% |
| ryml_inplace        |   69.42 |  25.66x | 2465.54% |         11.28 |       0.04x |     -96.10% |
| ryml_arena          |   46.38 |  17.14x | 1614.06% |         16.88 |       0.06x |     -94.17% |
| libyaml_arena       |   17.97 |   6.64x |  564.21% |         43.57 |       0.15x |     -84.94% |
| libyaml_arena_reuse |   21.29 |   7.87x |  686.73% |         36.78 |       0.13x |     -87.29% |
| libfyaml_arena      |    3.43 |   1.27x |   26.59% |        228.61 |       0.79x |     -21.00% |
| yamlcpp_arena       |    2.71 |   1.00x |    0.00% |        289.39 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100-cpu_time_ms.html
Built target ryml-bm-parse-style_maps_flow_outer1000_inner100-plot
/usr/bin/python3.11
Python 3.11.8
emit
style_maps_flow_outer1000_inner100
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100.json
+----------------------------------------------------------------------------------------------+
|                      emit benchmark: style_maps_flow_outer1000_inner100                      |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  |  171.67 |  36.99x | 3598.74% |          4.56 |       0.03x |     -97.30% |
| ryml_str          |  170.47 |  36.73x | 3572.78% |          4.59 |       0.03x |     -97.28% |
| ryml_ostream      |   43.39 |   9.35x |  834.87% |         18.05 |       0.11x |     -89.30% |
| fyaml_str_reserve |   26.68 |   5.75x |  474.77% |         29.35 |       0.17x |     -82.60% |
| fyaml_str         |   27.45 |   5.91x |  491.40% |         28.53 |       0.17x |     -83.09% |
| fyaml_ostream     |   22.49 |   4.85x |  384.52% |         34.82 |       0.21x |     -79.36% |
| yamlcpp_          |    4.64 |   1.00x |    0.00% |        168.70 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100-cpu_time_ms.html
Built target ryml-bm-emit-style_maps_flow_outer1000_inner100-plot
/usr/bin/python3.11
Python 3.11.8
parse
style_seqs_blck_outer1000_inner10
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner10.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner10.json
+------------------------------------------------------------------------------------------------+
|                       parse benchmark: style_seqs_blck_outer1000_inner10                       |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |   82.41 |  16.79x | 1579.45% |          0.96 |       0.06x |     -94.05% |
| ryml_arena_reuse    |   82.31 |  16.77x | 1577.30% |          0.96 |       0.06x |     -94.04% |
| ryml_inplace        |   58.44 |  11.91x | 1090.81% |          1.35 |       0.08x |     -91.60% |
| ryml_arena          |   56.83 |  11.58x | 1058.00% |          1.39 |       0.09x |     -91.36% |
| libyaml_arena       |   34.59 |   7.05x |  604.89% |          2.28 |       0.14x |     -85.81% |
| libyaml_arena_reuse |   36.87 |   7.51x |  651.40% |          2.14 |       0.13x |     -86.69% |
| libfyaml_arena      |   21.97 |   4.48x |  347.61% |          3.59 |       0.22x |     -77.66% |
| yamlcpp_arena       |    4.91 |   1.00x |    0.00% |         16.08 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner10-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner10-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner10-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner10-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner10-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner10-cpu_time_ms.html
Built target ryml-bm-parse-style_seqs_blck_outer1000_inner10-plot
/usr/bin/python3.11
Python 3.11.8
emit
style_seqs_blck_outer1000_inner10
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner10.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner10.json
+----------------------------------------------------------------------------------------------+
|                      emit benchmark: style_seqs_blck_outer1000_inner10                       |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  |  278.64 |  28.16x | 2716.14% |          0.28 |       0.04x |     -96.45% |
| ryml_str          |  278.41 |  28.14x | 2713.86% |          0.28 |       0.04x |     -96.45% |
| ryml_ostream      |   14.98 |   1.51x |   51.39% |          5.27 |       0.66x |     -33.94% |
| fyaml_str_reserve |   29.48 |   2.98x |  197.93% |          2.68 |       0.34x |     -66.44% |
| fyaml_str         |   29.39 |   2.97x |  197.07% |          2.68 |       0.34x |     -66.34% |
| fyaml_ostream     |   21.29 |   2.15x |  115.17% |          3.71 |       0.46x |     -53.52% |
| yamlcpp_          |    9.89 |   1.00x |    0.00% |          7.97 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner10-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner10-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner10-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner10-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner10-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner10-cpu_time_ms.html
Built target ryml-bm-emit-style_seqs_blck_outer1000_inner10-plot
/usr/bin/python3.11
Python 3.11.8
parse
style_seqs_blck_outer1000_inner100
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner100.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner100.json
+------------------------------------------------------------------------------------------------+
|                      parse benchmark: style_seqs_blck_outer1000_inner100                       |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |   86.51 |  18.92x | 1792.22% |          9.12 |       0.05x |     -94.72% |
| ryml_arena_reuse    |   85.63 |  18.73x | 1772.99% |          9.21 |       0.05x |     -94.66% |
| ryml_inplace        |   59.33 |  12.98x | 1197.83% |         13.30 |       0.08x |     -92.29% |
| ryml_arena          |   59.15 |  12.94x | 1193.71% |         13.34 |       0.08x |     -92.27% |
| libyaml_arena       |   36.34 |   7.95x |  694.77% |         21.71 |       0.13x |     -87.42% |
| libyaml_arena_reuse |   38.37 |   8.39x |  739.37% |         20.56 |       0.12x |     -88.09% |
| libfyaml_arena      |   21.76 |   4.76x |  375.98% |         36.26 |       0.21x |     -78.99% |
| yamlcpp_arena       |    4.57 |   1.00x |    0.00% |        172.58 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner100-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner100-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner100-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner100-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner100-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner100-cpu_time_ms.html
Built target ryml-bm-parse-style_seqs_blck_outer1000_inner100-plot
/usr/bin/python3.11
Python 3.11.8
emit
style_seqs_blck_outer1000_inner100
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner100.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner100.json
+----------------------------------------------------------------------------------------------+
|                      emit benchmark: style_seqs_blck_outer1000_inner100                      |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  |  288.47 |  29.69x | 2869.26% |          2.74 |       0.03x |     -96.63% |
| ryml_str          |  278.97 |  28.72x | 2771.51% |          2.83 |       0.03x |     -96.52% |
| ryml_ostream      |   47.85 |   4.93x |  392.56% |         16.49 |       0.20x |     -79.70% |
| fyaml_str_reserve |   28.30 |   2.91x |  191.28% |         27.88 |       0.34x |     -65.67% |
| fyaml_str         |   28.56 |   2.94x |  193.98% |         27.63 |       0.34x |     -65.98% |
| fyaml_ostream     |   24.52 |   2.52x |  152.39% |         32.18 |       0.40x |     -60.38% |
| yamlcpp_          |    9.72 |   1.00x |    0.00% |         81.21 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner100-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner100-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner100-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner100-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner100-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner100-cpu_time_ms.html
Built target ryml-bm-emit-style_seqs_blck_outer1000_inner100-plot
/usr/bin/python3.11
Python 3.11.8
parse
style_seqs_flow_outer1000_inner10
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10.json
+------------------------------------------------------------------------------------------------+
|                       parse benchmark: style_seqs_flow_outer1000_inner10                       |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |   79.07 |  23.15x | 2215.10% |          0.54 |       0.04x |     -95.68% |
| ryml_arena_reuse    |   76.52 |  22.40x | 2140.26% |          0.56 |       0.04x |     -95.54% |
| ryml_inplace        |   48.60 |  14.23x | 1322.93% |          0.88 |       0.07x |     -92.97% |
| ryml_arena          |   48.65 |  14.24x | 1324.46% |          0.88 |       0.07x |     -92.98% |
| libyaml_arena       |   12.82 |   3.75x |  275.31% |          3.35 |       0.27x |     -73.36% |
| libyaml_arena_reuse |   20.73 |   6.07x |  506.84% |          2.07 |       0.16x |     -83.52% |
| libfyaml_arena      |   14.69 |   4.30x |  329.95% |          2.92 |       0.23x |     -76.74% |
| yamlcpp_arena       |    3.42 |   1.00x |    0.00% |         12.56 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10-cpu_time_ms.html
Built target ryml-bm-parse-style_seqs_flow_outer1000_inner10-plot
/usr/bin/python3.11
Python 3.11.8
emit
style_seqs_flow_outer1000_inner10
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10.json
+----------------------------------------------------------------------------------------------+
|                      emit benchmark: style_seqs_flow_outer1000_inner10                       |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| function          |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+-------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_str_reserve  |  151.25 |  29.59x | 2859.36% |          0.28 |       0.03x |     -96.62% |
| ryml_str          |  151.53 |  29.65x | 2864.96% |          0.28 |       0.03x |     -96.63% |
| ryml_ostream      |    8.08 |   1.58x |   58.07% |          5.31 |       0.63x |     -36.74% |
| fyaml_str_reserve |   37.48 |   7.33x |  633.33% |          1.14 |       0.14x |     -86.36% |
| fyaml_str         |   37.49 |   7.34x |  633.53% |          1.14 |       0.14x |     -86.37% |
| fyaml_ostream     |   13.90 |   2.72x |  171.91% |          3.09 |       0.37x |     -63.22% |
| yamlcpp_          |    5.11 |   1.00x |    0.00% |          8.39 |       1.00x |       0.00% |
+-------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10-cpu_time_ms.html
Built target ryml-bm-emit-style_seqs_flow_outer1000_inner10-plot
/usr/bin/python3.11
Python 3.11.8
parse
style_seqs_flow_outer1000_inner100
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100.json
+------------------------------------------------------------------------------------------------+
|                      parse benchmark: style_seqs_flow_outer1000_inner100                       |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| function            |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+---------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_inplace_reuse  |   80.54 |  31.01x | 3001.00% |          4.88 |       0.03x |     -96.78% |
| ryml_arena_reuse    |   80.38 |  30.95x | 2994.91% |          4.89 |       0.03x |     -96.77% |
| ryml_inplace        |   48.36 |  18.62x | 1762.10% |          8.13 |       0.05x |     -94.63% |
| ryml_arena          |   27.98 |  10.77x |  977.25% |         14.05 |       0.09x |     -90.72% |
| libyaml_arena       |   14.00 |   5.39x |  439.11% |         28.07 |       0.19x |     -81.45% |
| libyaml_arena_reuse |   18.91 |   7.28x |  628.07% |         20.78 |       0.14x |     -86.27% |
| libfyaml_arena      |   14.23 |   5.48x |  447.77% |         27.62 |       0.18x |     -81.74% |
| yamlcpp_arena       |    2.60 |   1.00x |    0.00% |        151.32 |       1.00x |       0.00% |
+---------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100-cpu_time_ms.html
Built target ryml-bm-parse-style_seqs_flow_outer1000_inner100-plot
Built target ryml-bm-plot
Returning to directory /proj/rapidyaml (currently in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release)
Build: finished building (1m 3s): linux-x86_64-gxx13.2-Release
-----------------------------------------------

Compilation finished at Tue May  7 21:46:49
```
