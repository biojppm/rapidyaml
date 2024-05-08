```
-*- mode: compilation; default-directory: "~/proj/rapidyaml/bm/" -*-
Compilation started at Tue May  7 20:32:44

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
[  1%] Built target jsoncpp_static
[ 11%] Built target ryml
[ 17%] Built target c4fs
[ 29%] Built target benchmark
[ 35%] Built target yaml
[ 49%] Built target fyaml
[ 66%] Built target yaml-cpp
[ 68%] Built target ryml-bm-emit
[ 68%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_seqs_flow_outer1000_inner100_json: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100_json.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100_json.json;/proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner100_json.json;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100_json.json
-----------------------------------
running case: EMIT/style_seqs_flow_outer1000_inner100_json.json
file: /proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner100_json.json
-----------------------------------
2024-05-07T20:32:45+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 0.65, 0.86, 1.03
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve         1344070 ns      1341017 ns          544 bytes_per_second=279.487Mi/s items_per_second=745.703/s
bm_ryml_yaml_json_str_reserve    3046959 ns      3041197 ns          228 bytes_per_second=123.24Mi/s items_per_second=328.818/s
bm_ryml_yaml_str                 1311027 ns      1308109 ns          520 bytes_per_second=286.518Mi/s items_per_second=764.462/s
bm_ryml_yaml_json_str            3102050 ns      3095528 ns          214 bytes_per_second=121.077Mi/s items_per_second=323.047/s
bm_ryml_yaml_ostream             7835994 ns      7813572 ns          100 bytes_per_second=47.9674Mi/s items_per_second=127.982/s
bm_ryml_yaml_json_ostream        7332170 ns      7320624 ns          100 bytes_per_second=51.1974Mi/s items_per_second=136.6/s
bm_fyaml_str_reserve            11697475 ns     11680954 ns           54 bytes_per_second=32.0862Mi/s items_per_second=85.6094/s
bm_fyaml_str                    11436846 ns     11422802 ns           53 bytes_per_second=32.8113Mi/s items_per_second=87.5442/s
bm_fyaml_ostream                17631787 ns     17607594 ns           43 bytes_per_second=21.2861Mi/s items_per_second=56.7937/s
bm_yamlcpp                      88771652 ns     88649696 ns            8 bytes_per_second=4.22784Mi/s items_per_second=11.2804/s
bm_rapidjson                      873921 ns       872767 ns          796 bytes_per_second=429.435Mi/s items_per_second=1.14578k/s
bm_jsoncpp                      14154992 ns     14137525 ns           66 bytes_per_second=26.5108Mi/s items_per_second=70.7337/s
bm_nlohmann                      7918494 ns      7906680 ns          146 bytes_per_second=47.4026Mi/s items_per_second=126.475/s
[ 68%] Built target ryml-bm-emit-style_seqs_flow_outer1000_inner100_json
[ 68%] Built target ryml-bm-parse
[ 68%] Built target ryml-bm-build
[ 68%] ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-appveyor: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-appveyor.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-appveyor.json;/proj/rapidyaml/bm/cases/appveyor.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-appveyor.json
-----------------------------------
running case: PARSE/appveyor.yml
file: /proj/rapidyaml/bm/cases/appveyor.yml
-----------------------------------
2024-05-07T20:32:58+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.10, 0.95, 1.06
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve       8473 ns         8470 ns        80540 bytes_per_second=239.814Mi/s items_per_second=118.058k/s
bm_ryml_yaml_inplace_reuse_nofilter               8391 ns         8391 ns        83265 bytes_per_second=242.083Mi/s items_per_second=119.175k/s
bm_ryml_yaml_inplace_reuse_reserve                8705 ns         8703 ns        80638 bytes_per_second=233.418Mi/s items_per_second=114.909k/s
bm_ryml_yaml_inplace_reuse                        8760 ns         8755 ns        79244 bytes_per_second=232.017Mi/s items_per_second=114.22k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve         8440 ns         8440 ns        83160 bytes_per_second=240.688Mi/s items_per_second=118.488k/s
bm_ryml_yaml_arena_reuse_nofilter                 9823 ns         9830 ns        70461 bytes_per_second=206.649Mi/s items_per_second=101.731k/s
bm_ryml_yaml_arena_reuse_reserve                  9039 ns         9036 ns        80044 bytes_per_second=224.811Mi/s items_per_second=110.672k/s
bm_ryml_yaml_arena_reuse                          8912 ns         8911 ns        79716 bytes_per_second=227.969Mi/s items_per_second=112.227k/s
bm_ryml_yaml_inplace_nofilter_reserve             8741 ns         8734 ns        83195 bytes_per_second=232.573Mi/s items_per_second=114.493k/s
bm_ryml_yaml_inplace_nofilter                     9516 ns         9525 ns        72592 bytes_per_second=213.262Mi/s items_per_second=104.987k/s
bm_ryml_yaml_inplace_reserve                      8861 ns         8859 ns        79583 bytes_per_second=229.302Mi/s items_per_second=112.883k/s
bm_ryml_yaml_inplace                             10301 ns        10311 ns        69442 bytes_per_second=197.004Mi/s items_per_second=96.9827k/s
bm_ryml_yaml_arena_nofilter_reserve               8506 ns         8504 ns        81874 bytes_per_second=238.861Mi/s items_per_second=117.589k/s
bm_ryml_yaml_arena_nofilter                       9750 ns         9760 ns        72138 bytes_per_second=208.133Mi/s items_per_second=102.461k/s
bm_ryml_yaml_arena_reserve                        9005 ns         9004 ns        78837 bytes_per_second=225.602Mi/s items_per_second=111.062k/s
bm_ryml_yaml_arena                                9798 ns         9790 ns        70777 bytes_per_second=207.5Mi/s items_per_second=102.15k/s
bm_libyaml_arena                                 32662 ns        32629 ns        21998 bytes_per_second=62.2559Mi/s items_per_second=30.6479k/s
bm_libyaml_arena_reuse                           31846 ns        31826 ns        22139 bytes_per_second=63.8256Mi/s items_per_second=31.4207k/s
bm_libfyaml_arena                                48906 ns        48862 ns        14425 bytes_per_second=41.5728Mi/s items_per_second=20.4658k/s
bm_yamlcpp_arena                                227852 ns       227582 ns         3160 bytes_per_second=8.92568Mi/s items_per_second=4.39402k/s
[ 68%] Built target ryml-bm-parse-appveyor
[ 68%] ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-appveyor: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-appveyor.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-appveyor.json;/proj/rapidyaml/bm/cases/appveyor.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-appveyor.json
-----------------------------------
running case: EMIT/appveyor.yml
file: /proj/rapidyaml/bm/cases/appveyor.yml
-----------------------------------
2024-05-07T20:33:16+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.13, 0.97, 1.06
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve            2000 ns         1995 ns       353766 bytes_per_second=1018.44Mi/s items_per_second=501.368k/s
bm_ryml_yaml_json_str_reserve       4022 ns         4019 ns       174096 bytes_per_second=505.459Mi/s items_per_second=248.832k/s
bm_ryml_yaml_str                    1958 ns         1957 ns       359239 bytes_per_second=1.01379Gi/s items_per_second=511.057k/s
bm_ryml_yaml_json_str               4017 ns         4014 ns       174348 bytes_per_second=506.067Mi/s items_per_second=249.131k/s
bm_ryml_yaml_ostream              560302 ns       558894 ns         3492 bytes_per_second=3.63454Mi/s items_per_second=1.78925k/s
bm_ryml_yaml_json_ostream        2303131 ns      2297142 ns        13754 bytes_per_second=905.507Ki/s items_per_second=435.324/s
bm_fyaml_str_reserve               29440 ns        29410 ns        24405 bytes_per_second=69.0693Mi/s items_per_second=34.0021k/s
bm_fyaml_str                       28544 ns        28524 ns        24525 bytes_per_second=71.214Mi/s items_per_second=35.0579k/s
bm_fyaml_ostream                  576274 ns       574926 ns         8946 bytes_per_second=3.53319Mi/s items_per_second=1.73935k/s
bm_yamlcpp                        362446 ns       361811 ns         3059 bytes_per_second=5.61433Mi/s items_per_second=2.76387k/s
Built target ryml-bm-emit-appveyor
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-compile_commands: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-compile_commands.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-compile_commands.json;/proj/rapidyaml/bm/cases/compile_commands.json;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-compile_commands.json
-----------------------------------
running case: PARSE/compile_commands.json
file: /proj/rapidyaml/bm/cases/compile_commands.json
-----------------------------------
2024-05-07T20:34:03+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.27, 1.02, 1.07
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_json_inplace_reuse_nofilter_reserve      73279 ns        73226 ns         9013 bytes_per_second=609.848Mi/s items_per_second=13.6563k/s
bm_ryml_json_inplace_reuse_nofilter              73026 ns        72977 ns         9536 bytes_per_second=611.931Mi/s items_per_second=13.703k/s
bm_ryml_json_inplace_reuse_reserve               73247 ns        73198 ns         9543 bytes_per_second=610.085Mi/s items_per_second=13.6617k/s
bm_ryml_json_inplace_reuse                       73274 ns        73223 ns         9532 bytes_per_second=609.877Mi/s items_per_second=13.657k/s
bm_ryml_yaml_inplace_reuse_nofilter_reserve      74173 ns        74128 ns         9455 bytes_per_second=602.429Mi/s items_per_second=13.4902k/s
bm_ryml_yaml_inplace_reuse_nofilter              74140 ns        74094 ns         9414 bytes_per_second=602.703Mi/s items_per_second=13.4963k/s
bm_ryml_yaml_inplace_reuse_reserve               74046 ns        74002 ns         9452 bytes_per_second=603.454Mi/s items_per_second=13.5132k/s
bm_ryml_yaml_inplace_reuse                       74318 ns        74270 ns         9445 bytes_per_second=601.276Mi/s items_per_second=13.4644k/s
bm_ryml_json_arena_reuse_nofilter_reserve        74274 ns        74228 ns         9376 bytes_per_second=601.619Mi/s items_per_second=13.4721k/s
bm_ryml_json_arena_reuse_nofilter                75117 ns        75034 ns         9399 bytes_per_second=595.153Mi/s items_per_second=13.3273k/s
bm_ryml_json_arena_reuse_reserve                 75589 ns        75503 ns         9434 bytes_per_second=591.46Mi/s items_per_second=13.2446k/s
bm_ryml_json_arena_reuse                         74401 ns        74347 ns         9430 bytes_per_second=600.657Mi/s items_per_second=13.4505k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve        75289 ns        75242 ns         9306 bytes_per_second=593.509Mi/s items_per_second=13.2905k/s
bm_ryml_yaml_arena_reuse_nofilter               105484 ns       104560 ns         7013 bytes_per_second=427.09Mi/s items_per_second=9.56384k/s
bm_ryml_yaml_arena_reuse_reserve                 75524 ns        75472 ns         9000 bytes_per_second=591.703Mi/s items_per_second=13.25k/s
bm_ryml_yaml_arena_reuse                         75198 ns        75145 ns         9328 bytes_per_second=594.273Mi/s items_per_second=13.3076k/s
bm_ryml_json_inplace_nofilter_reserve            73471 ns        73416 ns         9534 bytes_per_second=608.27Mi/s items_per_second=13.621k/s
bm_ryml_json_inplace_nofilter                    77761 ns        77712 ns         8973 bytes_per_second=574.642Mi/s items_per_second=12.868k/s
bm_ryml_json_inplace_reserve                     73336 ns        73279 ns         9538 bytes_per_second=609.404Mi/s items_per_second=13.6464k/s
bm_ryml_json_inplace                             79387 ns        79001 ns         8954 bytes_per_second=565.265Mi/s items_per_second=12.658k/s
bm_ryml_yaml_inplace_nofilter_reserve            76987 ns        76914 ns         9324 bytes_per_second=580.604Mi/s items_per_second=13.0015k/s
bm_ryml_yaml_inplace_nofilter                    79094 ns        79042 ns         8755 bytes_per_second=564.974Mi/s items_per_second=12.6515k/s
bm_ryml_yaml_inplace_reserve                     74334 ns        74285 ns         9421 bytes_per_second=601.155Mi/s items_per_second=13.4617k/s
bm_ryml_yaml_inplace                             79726 ns        79673 ns         8859 bytes_per_second=560.497Mi/s items_per_second=12.5512k/s
bm_ryml_json_arena_nofilter_reserve              76309 ns        76219 ns         9389 bytes_per_second=585.899Mi/s items_per_second=13.1201k/s
bm_ryml_json_arena_nofilter                      81705 ns        81636 ns         8253 bytes_per_second=547.023Mi/s items_per_second=12.2495k/s
bm_ryml_json_arena_reserve                       76960 ns        76864 ns         9336 bytes_per_second=580.98Mi/s items_per_second=13.0099k/s
bm_ryml_json_arena                               78670 ns        78604 ns         8877 bytes_per_second=568.121Mi/s items_per_second=12.7219k/s
bm_ryml_yaml_arena_nofilter_reserve              76013 ns        75958 ns         9281 bytes_per_second=587.917Mi/s items_per_second=13.1653k/s
bm_ryml_yaml_arena_nofilter                      81363 ns        81311 ns         8732 bytes_per_second=549.21Mi/s items_per_second=12.2985k/s
bm_ryml_yaml_arena_reserve                       75334 ns        75289 ns         9305 bytes_per_second=593.139Mi/s items_per_second=13.2822k/s
bm_ryml_yaml_arena                               81877 ns        81513 ns         8758 bytes_per_second=547.849Mi/s items_per_second=12.268k/s
bm_libyaml_arena                                370113 ns       369811 ns         1884 bytes_per_second=120.756Mi/s items_per_second=2.70408k/s
bm_libyaml_arena_reuse                          380513 ns       380206 ns         1829 bytes_per_second=117.454Mi/s items_per_second=2.63015k/s
bm_libfyaml_arena                               327627 ns       327333 ns         2143 bytes_per_second=136.426Mi/s items_per_second=3.05499k/s
bm_yamlcpp_arena                               2517648 ns      2514826 ns          290 bytes_per_second=17.7574Mi/s items_per_second=397.642/s
bm_rapidjson_arena                               74924 ns        74835 ns         9904 bytes_per_second=596.737Mi/s items_per_second=13.3627k/s
bm_rapidjson_inplace                             27960 ns        27941 ns        24587 bytes_per_second=1.56079Gi/s items_per_second=35.7896k/s
bm_sajson_arena                                  70109 ns        70045 ns         9599 bytes_per_second=637.546Mi/s items_per_second=14.2766k/s
bm_sajson_inplace                                68589 ns        68548 ns        10227 bytes_per_second=651.463Mi/s items_per_second=14.5882k/s
bm_jsoncpp_arena                                165161 ns       165038 ns         4227 bytes_per_second=270.585Mi/s items_per_second=6.05922k/s
bm_nlohmann_arena                               221951 ns       220182 ns         3235 bytes_per_second=202.817Mi/s items_per_second=4.54169k/s
Built target ryml-bm-parse-compile_commands
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-compile_commands: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-compile_commands.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-compile_commands.json;/proj/rapidyaml/bm/cases/compile_commands.json;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-compile_commands.json
-----------------------------------
running case: EMIT/compile_commands.json
file: /proj/rapidyaml/bm/cases/compile_commands.json
-----------------------------------
2024-05-07T20:34:39+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.20, 1.03, 1.08
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve           57917 ns        57873 ns        12048 bytes_per_second=771.636Mi/s items_per_second=17.2793k/s
bm_ryml_yaml_json_str_reserve      45725 ns        45688 ns        15315 bytes_per_second=977.424Mi/s items_per_second=21.8875k/s
bm_ryml_yaml_str                   57914 ns        57869 ns        12143 bytes_per_second=771.682Mi/s items_per_second=17.2803k/s
bm_ryml_yaml_json_str              45615 ns        45577 ns        15315 bytes_per_second=979.802Mi/s items_per_second=21.9407k/s
bm_ryml_yaml_ostream             6334802 ns      6324493 ns         1045 bytes_per_second=7.06092Mi/s items_per_second=158.115/s
bm_ryml_yaml_json_ostream        3996362 ns      3983540 ns         1000 bytes_per_second=11.2103Mi/s items_per_second=251.033/s
bm_fyaml_str_reserve              111376 ns       111283 ns         6048 bytes_per_second=401.289Mi/s items_per_second=8.98608k/s
bm_fyaml_str                      111047 ns       110962 ns         6267 bytes_per_second=402.45Mi/s items_per_second=9.01207k/s
bm_fyaml_ostream                 4392605 ns      4384315 ns         1000 bytes_per_second=10.1856Mi/s items_per_second=228.086/s
bm_yamlcpp                       7167613 ns      7159986 ns          100 bytes_per_second=6.23699Mi/s items_per_second=139.665/s
bm_rapidjson                       67893 ns        67792 ns         9125 bytes_per_second=658.728Mi/s items_per_second=14.7509k/s
bm_jsoncpp                       4268837 ns      4260701 ns         1000 bytes_per_second=10.4811Mi/s items_per_second=234.703/s
bm_nlohmann                      4038108 ns      4030430 ns         1000 bytes_per_second=11.0799Mi/s items_per_second=248.112/s
Built target ryml-bm-emit-compile_commands
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-travis: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-travis.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-travis.json;/proj/rapidyaml/bm/cases/travis.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-travis.json
-----------------------------------
running case: PARSE/travis.yml
file: /proj/rapidyaml/bm/cases/travis.yml
-----------------------------------
2024-05-07T20:35:10+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.55, 1.13, 1.11
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve      14296 ns        14284 ns        45995 bytes_per_second=271.598Mi/s items_per_second=70.0076k/s
bm_ryml_yaml_inplace_reuse_nofilter              14328 ns        14316 ns        50379 bytes_per_second=270.994Mi/s items_per_second=69.852k/s
bm_ryml_yaml_inplace_reuse_reserve               15168 ns        15158 ns        46437 bytes_per_second=255.938Mi/s items_per_second=65.9712k/s
bm_ryml_yaml_inplace_reuse                       15219 ns        15206 ns        46163 bytes_per_second=255.128Mi/s items_per_second=65.7624k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve        13872 ns        13873 ns        50461 bytes_per_second=279.654Mi/s items_per_second=72.0843k/s
bm_ryml_yaml_arena_reuse_nofilter                15248 ns        15252 ns        45642 bytes_per_second=254.37Mi/s items_per_second=65.567k/s
bm_ryml_yaml_arena_reuse_reserve                 15133 ns        15130 ns        46195 bytes_per_second=256.418Mi/s items_per_second=66.0949k/s
bm_ryml_yaml_arena_reuse                         15114 ns        15112 ns        46274 bytes_per_second=256.714Mi/s items_per_second=66.1712k/s
bm_ryml_yaml_inplace_nofilter_reserve            13836 ns        13838 ns        50589 bytes_per_second=280.363Mi/s items_per_second=72.2669k/s
bm_ryml_yaml_inplace_nofilter                    14978 ns        14983 ns        46622 bytes_per_second=258.935Mi/s items_per_second=66.7436k/s
bm_ryml_yaml_inplace_reserve                     15121 ns        15119 ns        46224 bytes_per_second=256.604Mi/s items_per_second=66.1427k/s
bm_ryml_yaml_inplace                             16440 ns        16447 ns        42920 bytes_per_second=235.881Mi/s items_per_second=60.8013k/s
bm_ryml_yaml_arena_nofilter_reserve              13935 ns        13932 ns        50367 bytes_per_second=278.46Mi/s items_per_second=71.7765k/s
bm_ryml_yaml_arena_nofilter                      15099 ns        15104 ns        46388 bytes_per_second=256.852Mi/s items_per_second=66.2068k/s
bm_ryml_yaml_arena_reserve                       15234 ns        15231 ns        45717 bytes_per_second=254.71Mi/s items_per_second=65.6547k/s
bm_ryml_yaml_arena                               16165 ns        16153 ns        43224 bytes_per_second=240.173Mi/s items_per_second=61.9076k/s
bm_libyaml_arena                                 51449 ns        51408 ns        13556 bytes_per_second=75.4664Mi/s items_per_second=19.4524k/s
bm_libyaml_arena_reuse                           51117 ns        51125 ns        13597 bytes_per_second=75.8834Mi/s items_per_second=19.5599k/s
bm_libfyaml_arena                                72441 ns        72376 ns         9733 bytes_per_second=53.6025Mi/s items_per_second=13.8167k/s
bm_yamlcpp_arena                                385039 ns       384734 ns         1819 bytes_per_second=10.0837Mi/s items_per_second=2.5992k/s
Built target ryml-bm-parse-travis
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-travis: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-travis.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-travis.json;/proj/rapidyaml/bm/cases/travis.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-travis.json
-----------------------------------
running case: EMIT/travis.yml
file: /proj/rapidyaml/bm/cases/travis.yml
-----------------------------------
2024-05-07T20:35:28+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.40, 1.12, 1.10
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve            2706 ns         2687 ns       260281 bytes_per_second=1.41013Gi/s items_per_second=372.202k/s
bm_ryml_yaml_json_str_reserve       6940 ns         6934 ns        98522 bytes_per_second=559.459Mi/s items_per_second=144.207k/s
bm_ryml_yaml_str                    2678 ns         2676 ns       261841 bytes_per_second=1.41597Gi/s items_per_second=373.742k/s
bm_ryml_yaml_json_str               6940 ns         6935 ns       100767 bytes_per_second=559.405Mi/s items_per_second=144.193k/s
bm_ryml_yaml_ostream              326421 ns       325651 ns         1782 bytes_per_second=11.9132Mi/s items_per_second=3.07077k/s
bm_ryml_yaml_json_ostream        2125985 ns      2119783 ns         6474 bytes_per_second=1.83016Mi/s items_per_second=471.746/s
bm_fyaml_str_reserve               65642 ns        65590 ns        10667 bytes_per_second=59.1488Mi/s items_per_second=15.2463k/s
bm_fyaml_str                       65554 ns        65505 ns        10667 bytes_per_second=59.2255Mi/s items_per_second=15.2661k/s
bm_fyaml_ostream                  519671 ns       518247 ns         4027 bytes_per_second=7.4859Mi/s items_per_second=1.92958k/s
bm_yamlcpp                        520403 ns       519491 ns         1566 bytes_per_second=7.46797Mi/s items_per_second=1.92496k/s
Built target ryml-bm-emit-travis
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_dquot_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_multiline.json;/proj/rapidyaml/bm/cases/scalar_dquot_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_multiline.json
-----------------------------------
running case: PARSE/scalar_dquot_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_dquot_multiline.yml
-----------------------------------
2024-05-07T20:35:52+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.34, 1.13, 1.10
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve     130176 ns       130087 ns         5374 bytes_per_second=529.169Mi/s items_per_second=7.68714k/s
bm_ryml_yaml_inplace_reuse_nofilter             133044 ns       132750 ns         5388 bytes_per_second=518.553Mi/s items_per_second=7.53294k/s
bm_ryml_yaml_inplace_reuse_reserve              368365 ns       368096 ns         1849 bytes_per_second=187.011Mi/s items_per_second=2.71668k/s
bm_ryml_yaml_inplace_reuse                      367751 ns       367460 ns         1905 bytes_per_second=187.335Mi/s items_per_second=2.72139k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve       132341 ns       132243 ns         5287 bytes_per_second=520.541Mi/s items_per_second=7.56181k/s
bm_ryml_yaml_arena_reuse_nofilter               134147 ns       134048 ns         5235 bytes_per_second=513.533Mi/s items_per_second=7.46001k/s
bm_ryml_yaml_arena_reuse_reserve                370403 ns       370144 ns         1888 bytes_per_second=185.976Mi/s items_per_second=2.70165k/s
bm_ryml_yaml_arena_reuse                        371284 ns       370931 ns         1852 bytes_per_second=185.582Mi/s items_per_second=2.69592k/s
bm_ryml_yaml_inplace_nofilter_reserve           131478 ns       131389 ns         5330 bytes_per_second=523.925Mi/s items_per_second=7.61097k/s
bm_ryml_yaml_inplace_nofilter                   131746 ns       131659 ns         5331 bytes_per_second=522.851Mi/s items_per_second=7.59537k/s
bm_ryml_yaml_inplace_reserve                    367704 ns       367445 ns         1903 bytes_per_second=187.343Mi/s items_per_second=2.7215k/s
bm_ryml_yaml_inplace                            374994 ns       372608 ns         1852 bytes_per_second=184.747Mi/s items_per_second=2.68379k/s
bm_ryml_yaml_arena_nofilter_reserve             131349 ns       131260 ns         5317 bytes_per_second=524.439Mi/s items_per_second=7.61844k/s
bm_ryml_yaml_arena_nofilter                     132579 ns       132498 ns         5275 bytes_per_second=519.541Mi/s items_per_second=7.54729k/s
bm_ryml_yaml_arena_reserve                      370328 ns       370060 ns         1897 bytes_per_second=186.019Mi/s items_per_second=2.70226k/s
bm_ryml_yaml_arena                              372243 ns       371906 ns         1886 bytes_per_second=185.095Mi/s items_per_second=2.68885k/s
bm_libyaml_arena                                514439 ns       514026 ns         1360 bytes_per_second=133.92Mi/s items_per_second=1.94543k/s
bm_libyaml_arena_reuse                          524413 ns       523846 ns         1362 bytes_per_second=131.409Mi/s items_per_second=1.90896k/s
bm_libfyaml_arena                               437398 ns       437047 ns         1602 bytes_per_second=157.507Mi/s items_per_second=2.28808k/s
bm_yamlcpp_arena                               3319316 ns      3316286 ns          211 bytes_per_second=20.7576Mi/s items_per_second=301.542/s
Built target ryml-bm-parse-scalar_dquot_multiline
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_dquot_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_multiline.json;/proj/rapidyaml/bm/cases/scalar_dquot_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_multiline.json
-----------------------------------
running case: EMIT/scalar_dquot_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_dquot_multiline.yml
-----------------------------------
2024-05-07T20:36:12+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.38, 1.15, 1.11
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve           99484 ns        99396 ns         7052 bytes_per_second=692.564Mi/s items_per_second=10.0608k/s
bm_ryml_yaml_json_str_reserve     106631 ns       106550 ns         6581 bytes_per_second=646.061Mi/s items_per_second=9.38522k/s
bm_ryml_yaml_str                   99465 ns        99388 ns         7037 bytes_per_second=692.62Mi/s items_per_second=10.0616k/s
bm_ryml_yaml_json_str             106847 ns       106761 ns         6608 bytes_per_second=644.787Mi/s items_per_second=9.36672k/s
bm_ryml_yaml_ostream             6635112 ns      6623583 ns          741 bytes_per_second=10.3929Mi/s items_per_second=150.976/s
bm_ryml_yaml_json_ostream        7425280 ns      7414519 ns         1000 bytes_per_second=9.28423Mi/s items_per_second=134.871/s
bm_fyaml_str_reserve             1560492 ns      1559361 ns          445 bytes_per_second=44.1451Mi/s items_per_second=641.288/s
bm_fyaml_str                     1560600 ns      1559486 ns          446 bytes_per_second=44.1415Mi/s items_per_second=641.237/s
bm_fyaml_ostream                 2638169 ns      2633401 ns          371 bytes_per_second=26.1404Mi/s items_per_second=379.737/s
bm_yamlcpp                       7642195 ns      7634236 ns           94 bytes_per_second=9.01703Mi/s items_per_second=130.989/s
Built target ryml-bm-emit-scalar_dquot_multiline
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_dquot_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_singleline.json;/proj/rapidyaml/bm/cases/scalar_dquot_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_dquot_singleline.json
-----------------------------------
running case: PARSE/scalar_dquot_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_dquot_singleline.yml
-----------------------------------
2024-05-07T20:36:32+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.42, 1.17, 1.12
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve      11712 ns        11709 ns        58992 bytes_per_second=392.998Mi/s items_per_second=85.4069k/s
bm_ryml_yaml_inplace_reuse_nofilter              11680 ns        11678 ns        59938 bytes_per_second=394.018Mi/s items_per_second=85.6286k/s
bm_ryml_yaml_inplace_reuse_reserve               11831 ns        11830 ns        58996 bytes_per_second=388.96Mi/s items_per_second=84.5294k/s
bm_ryml_yaml_inplace_reuse                       11834 ns        11831 ns        59179 bytes_per_second=388.924Mi/s items_per_second=84.5214k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve        11779 ns        11781 ns        59482 bytes_per_second=390.572Mi/s items_per_second=84.8797k/s
bm_ryml_yaml_arena_reuse_nofilter                13191 ns        13196 ns        52948 bytes_per_second=348.691Mi/s items_per_second=75.778k/s
bm_ryml_yaml_arena_reuse_reserve                 11933 ns        11935 ns        57051 bytes_per_second=385.551Mi/s items_per_second=83.7886k/s
bm_ryml_yaml_arena_reuse                         11957 ns        11959 ns        58410 bytes_per_second=384.76Mi/s items_per_second=83.6167k/s
bm_ryml_yaml_inplace_nofilter_reserve            11697 ns        11688 ns        59927 bytes_per_second=393.703Mi/s items_per_second=85.5601k/s
bm_ryml_yaml_inplace_nofilter                    12815 ns        12823 ns        54456 bytes_per_second=358.848Mi/s items_per_second=77.9854k/s
bm_ryml_yaml_inplace_reserve                     11942 ns        11938 ns        58775 bytes_per_second=385.441Mi/s items_per_second=83.7647k/s
bm_ryml_yaml_inplace                             13170 ns        13179 ns        52669 bytes_per_second=349.151Mi/s items_per_second=75.8779k/s
bm_ryml_yaml_arena_nofilter_reserve              12489 ns        12485 ns        59198 bytes_per_second=368.573Mi/s items_per_second=80.0988k/s
bm_ryml_yaml_arena_nofilter                      13983 ns        13983 ns        53475 bytes_per_second=329.087Mi/s items_per_second=71.5176k/s
bm_ryml_yaml_arena_reserve                       12198 ns        12198 ns        58257 bytes_per_second=377.221Mi/s items_per_second=81.9781k/s
bm_ryml_yaml_arena                               13025 ns        13014 ns        53691 bytes_per_second=353.569Mi/s items_per_second=76.8381k/s
bm_libyaml_arena                                 61723 ns        61679 ns        10898 bytes_per_second=74.604Mi/s items_per_second=16.2131k/s
bm_libyaml_arena_reuse                           61120 ns        61092 ns        11486 bytes_per_second=75.32Mi/s items_per_second=16.3686k/s
bm_libfyaml_arena                                81802 ns        81740 ns         8580 bytes_per_second=56.2938Mi/s items_per_second=12.2338k/s
bm_yamlcpp_arena                                353076 ns       352788 ns         1985 bytes_per_second=13.0432Mi/s items_per_second=2.83456k/s
Built target ryml-bm-parse-scalar_dquot_singleline
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_dquot_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_singleline.json;/proj/rapidyaml/bm/cases/scalar_dquot_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_dquot_singleline.json
-----------------------------------
running case: EMIT/scalar_dquot_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_dquot_singleline.yml
-----------------------------------
2024-05-07T20:36:49+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.41, 1.18, 1.12
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve            7214 ns         7208 ns        97259 bytes_per_second=638.368Mi/s items_per_second=138.731k/s
bm_ryml_yaml_json_str_reserve       7195 ns         7190 ns        97700 bytes_per_second=640.009Mi/s items_per_second=139.088k/s
bm_ryml_yaml_str                    7208 ns         7203 ns        96796 bytes_per_second=638.872Mi/s items_per_second=138.841k/s
bm_ryml_yaml_json_str               7200 ns         7195 ns        97188 bytes_per_second=639.531Mi/s items_per_second=138.984k/s
bm_ryml_yaml_ostream              586271 ns       584926 ns         1000 bytes_per_second=7.86678Mi/s items_per_second=1.70962k/s
bm_ryml_yaml_json_ostream        1951317 ns      1947219 ns         3722 bytes_per_second=2.3631Mi/s items_per_second=513.553/s
bm_fyaml_str_reserve               25688 ns        25670 ns        27259 bytes_per_second=179.252Mi/s items_per_second=38.9554k/s
bm_fyaml_str                       25705 ns        25688 ns        27260 bytes_per_second=179.133Mi/s items_per_second=38.9294k/s
bm_fyaml_ostream                  642593 ns       641035 ns         3240 bytes_per_second=7.1782Mi/s items_per_second=1.55998k/s
bm_yamlcpp                        758465 ns       757161 ns         1286 bytes_per_second=6.07728Mi/s items_per_second=1.32072k/s
Built target ryml-bm-emit-scalar_dquot_singleline
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_squot_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_multiline.json;/proj/rapidyaml/bm/cases/scalar_squot_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_multiline.json
-----------------------------------
running case: PARSE/scalar_squot_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_squot_multiline.yml
-----------------------------------
2024-05-07T20:37:06+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.76, 1.27, 1.15
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve      98271 ns        97644 ns         7204 bytes_per_second=705.887Mi/s items_per_second=10.2413k/s
bm_ryml_yaml_inplace_reuse_nofilter              97217 ns        97132 ns         7219 bytes_per_second=709.61Mi/s items_per_second=10.2953k/s
bm_ryml_yaml_inplace_reuse_reserve              244990 ns       244810 ns         2816 bytes_per_second=281.549Mi/s items_per_second=4.08481k/s
bm_ryml_yaml_inplace_reuse                      245871 ns       245681 ns         2861 bytes_per_second=280.55Mi/s items_per_second=4.07032k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve        98992 ns        98906 ns         7085 bytes_per_second=696.885Mi/s items_per_second=10.1107k/s
bm_ryml_yaml_arena_reuse_nofilter                99754 ns        99674 ns         6974 bytes_per_second=691.51Mi/s items_per_second=10.0327k/s
bm_ryml_yaml_arena_reuse_reserve                249015 ns       247135 ns         2846 bytes_per_second=278.9Mi/s items_per_second=4.04638k/s
bm_ryml_yaml_arena_reuse                        247095 ns       246902 ns         2814 bytes_per_second=279.163Mi/s items_per_second=4.05019k/s
bm_ryml_yaml_inplace_nofilter_reserve            96682 ns        96612 ns         7213 bytes_per_second=713.432Mi/s items_per_second=10.3507k/s
bm_ryml_yaml_inplace_nofilter                    98373 ns        98313 ns         7140 bytes_per_second=701.085Mi/s items_per_second=10.1716k/s
bm_ryml_yaml_inplace_reserve                    245468 ns       245292 ns         2854 bytes_per_second=280.995Mi/s items_per_second=4.07677k/s
bm_ryml_yaml_inplace                            246470 ns       246295 ns         2836 bytes_per_second=279.851Mi/s items_per_second=4.06018k/s
bm_ryml_yaml_arena_nofilter_reserve              98479 ns        98419 ns         7149 bytes_per_second=700.333Mi/s items_per_second=10.1607k/s
bm_ryml_yaml_arena_nofilter                      98946 ns        98904 ns         7048 bytes_per_second=696.899Mi/s items_per_second=10.1109k/s
bm_ryml_yaml_arena_reserve                      246665 ns       246495 ns         2838 bytes_per_second=279.624Mi/s items_per_second=4.05688k/s
bm_ryml_yaml_arena                              247524 ns       247328 ns         2826 bytes_per_second=278.682Mi/s items_per_second=4.04321k/s
bm_libyaml_arena                                502342 ns       501953 ns         1396 bytes_per_second=137.315Mi/s items_per_second=1.99222k/s
bm_libyaml_arena_reuse                          500246 ns       499869 ns         1396 bytes_per_second=137.888Mi/s items_per_second=2.00052k/s
bm_libfyaml_arena                               434459 ns       434075 ns         1615 bytes_per_second=158.788Mi/s items_per_second=2.30375k/s
bm_yamlcpp_arena                               2991276 ns      2988503 ns          238 bytes_per_second=23.0637Mi/s items_per_second=334.616/s
Built target ryml-bm-parse-scalar_squot_multiline
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_squot_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_multiline.json;/proj/rapidyaml/bm/cases/scalar_squot_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_multiline.json
-----------------------------------
running case: EMIT/scalar_squot_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_squot_multiline.yml
-----------------------------------
2024-05-07T20:37:25+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.67, 1.28, 1.16
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve           53455 ns        53401 ns        13312 bytes_per_second=1.26048Gi/s items_per_second=18.7263k/s
bm_ryml_yaml_json_str_reserve     106217 ns       106133 ns         6560 bytes_per_second=649.427Mi/s items_per_second=9.42211k/s
bm_ryml_yaml_str                   52223 ns        52184 ns        13329 bytes_per_second=1.28987Gi/s items_per_second=19.163k/s
bm_ryml_yaml_json_str             106216 ns       106135 ns         6577 bytes_per_second=649.417Mi/s items_per_second=9.42196k/s
bm_ryml_yaml_ostream             7051860 ns      7040766 ns          781 bytes_per_second=9.78954Mi/s items_per_second=142.03/s
bm_ryml_yaml_json_ostream        7676007 ns      7663695 ns         1000 bytes_per_second=8.99381Mi/s items_per_second=130.485/s
bm_fyaml_str_reserve             1542501 ns      1541219 ns          455 bytes_per_second=44.7216Mi/s items_per_second=648.837/s
bm_fyaml_str                     1549352 ns      1537366 ns          453 bytes_per_second=44.8337Mi/s items_per_second=650.463/s
bm_fyaml_ostream                 2650875 ns      2645374 ns          366 bytes_per_second=26.0552Mi/s items_per_second=378.018/s
bm_yamlcpp                       8392181 ns      8383228 ns           85 bytes_per_second=8.22188Mi/s items_per_second=119.286/s
Built target ryml-bm-emit-scalar_squot_multiline
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_squot_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_singleline.json;/proj/rapidyaml/bm/cases/scalar_squot_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_squot_singleline.json
-----------------------------------
running case: PARSE/scalar_squot_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_squot_singleline.yml
-----------------------------------
2024-05-07T20:37:45+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.48, 1.26, 1.16
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve      10838 ns        10833 ns        64671 bytes_per_second=424.406Mi/s items_per_second=92.3091k/s
bm_ryml_yaml_inplace_reuse_nofilter              10816 ns        10813 ns        64771 bytes_per_second=425.196Mi/s items_per_second=92.4808k/s
bm_ryml_yaml_inplace_reuse_reserve               11436 ns        11432 ns        62026 bytes_per_second=402.171Mi/s items_per_second=87.4729k/s
bm_ryml_yaml_inplace_reuse                       11273 ns        11271 ns        61882 bytes_per_second=407.917Mi/s items_per_second=88.7226k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve        10891 ns        10890 ns        64402 bytes_per_second=422.177Mi/s items_per_second=91.8243k/s
bm_ryml_yaml_arena_reuse_nofilter                12297 ns        12303 ns        56441 bytes_per_second=373.716Mi/s items_per_second=81.2839k/s
bm_ryml_yaml_arena_reuse_reserve                 11456 ns        11457 ns        61416 bytes_per_second=401.291Mi/s items_per_second=87.2816k/s
bm_ryml_yaml_arena_reuse                         11485 ns        11487 ns        61690 bytes_per_second=400.255Mi/s items_per_second=87.0563k/s
bm_ryml_yaml_inplace_nofilter_reserve            10905 ns        10901 ns        64647 bytes_per_second=421.751Mi/s items_per_second=91.7316k/s
bm_ryml_yaml_inplace_nofilter                    12025 ns        12033 ns        57785 bytes_per_second=382.095Mi/s items_per_second=83.1064k/s
bm_ryml_yaml_inplace_reserve                     11380 ns        11376 ns        61838 bytes_per_second=404.159Mi/s items_per_second=87.9054k/s
bm_ryml_yaml_inplace                             12601 ns        12606 ns        55766 bytes_per_second=364.709Mi/s items_per_second=79.3248k/s
bm_ryml_yaml_arena_nofilter_reserve              10981 ns        10977 ns        63703 bytes_per_second=418.841Mi/s items_per_second=91.0988k/s
bm_ryml_yaml_arena_nofilter                      12721 ns        12727 ns        56936 bytes_per_second=361.266Mi/s items_per_second=78.576k/s
bm_ryml_yaml_arena_reserve                       11519 ns        11522 ns        59063 bytes_per_second=399.017Mi/s items_per_second=86.787k/s
bm_ryml_yaml_arena                               12550 ns        12540 ns        55607 bytes_per_second=366.652Mi/s items_per_second=79.7475k/s
bm_libyaml_arena                                 61618 ns        61570 ns        11325 bytes_per_second=74.6737Mi/s items_per_second=16.2417k/s
bm_libyaml_arena_reuse                           59928 ns        59904 ns        11678 bytes_per_second=76.751Mi/s items_per_second=16.6935k/s
bm_libfyaml_arena                                83204 ns        82520 ns         8539 bytes_per_second=55.7157Mi/s items_per_second=12.1183k/s
bm_yamlcpp_arena                                337565 ns       337254 ns         2074 bytes_per_second=13.6327Mi/s items_per_second=2.96513k/s
Built target ryml-bm-parse-scalar_squot_singleline
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_squot_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_singleline.json;/proj/rapidyaml/bm/cases/scalar_squot_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_squot_singleline.json
-----------------------------------
running case: EMIT/scalar_squot_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_squot_singleline.yml
-----------------------------------
2024-05-07T20:38:03+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.75, 1.34, 1.19
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve            5196 ns         5191 ns       135874 bytes_per_second=885.698Mi/s items_per_second=192.641k/s
bm_ryml_yaml_json_str_reserve       7391 ns         7382 ns        95196 bytes_per_second=622.85Mi/s items_per_second=135.471k/s
bm_ryml_yaml_str                    5186 ns         5182 ns       135993 bytes_per_second=887.158Mi/s items_per_second=192.958k/s
bm_ryml_yaml_json_str               7075 ns         7069 ns        98881 bytes_per_second=650.374Mi/s items_per_second=141.458k/s
bm_ryml_yaml_ostream              506829 ns       505529 ns         1000 bytes_per_second=9.09476Mi/s items_per_second=1.97813k/s
bm_ryml_yaml_json_ostream        1981995 ns      1977536 ns         3771 bytes_per_second=2.32495Mi/s items_per_second=505.68/s
bm_fyaml_str_reserve               29140 ns        29119 ns        24074 bytes_per_second=157.89Mi/s items_per_second=34.3414k/s
bm_fyaml_str                       29076 ns        29055 ns        24083 bytes_per_second=158.242Mi/s items_per_second=34.4179k/s
bm_fyaml_ostream                  648829 ns       647256 ns         3175 bytes_per_second=7.10331Mi/s items_per_second=1.54498k/s
bm_yamlcpp                        758148 ns       756849 ns         1288 bytes_per_second=6.07474Mi/s items_per_second=1.32127k/s
Built target ryml-bm-emit-scalar_squot_singleline
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_plain_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_multiline.json;/proj/rapidyaml/bm/cases/scalar_plain_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_multiline.json
-----------------------------------
running case: PARSE/scalar_plain_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_plain_multiline.yml
-----------------------------------
2024-05-07T20:38:19+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.59, 1.33, 1.18
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve     190741 ns       190589 ns         3693 bytes_per_second=360.485Mi/s items_per_second=5.24688k/s
bm_ryml_yaml_inplace_reuse_nofilter             188834 ns       188703 ns         3708 bytes_per_second=364.089Mi/s items_per_second=5.29933k/s
bm_ryml_yaml_inplace_reuse_reserve              345847 ns       345614 ns         2025 bytes_per_second=198.79Mi/s items_per_second=2.8934k/s
bm_ryml_yaml_inplace_reuse                      346569 ns       346307 ns         2025 bytes_per_second=198.392Mi/s items_per_second=2.88761k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve       190136 ns       190011 ns         3681 bytes_per_second=361.583Mi/s items_per_second=5.26286k/s
bm_ryml_yaml_arena_reuse_nofilter               192015 ns       191878 ns         3635 bytes_per_second=358.065Mi/s items_per_second=5.21166k/s
bm_ryml_yaml_arena_reuse_reserve                346978 ns       346741 ns         2011 bytes_per_second=198.144Mi/s items_per_second=2.884k/s
bm_ryml_yaml_arena_reuse                        347731 ns       347496 ns         2016 bytes_per_second=197.713Mi/s items_per_second=2.87773k/s
bm_ryml_yaml_inplace_nofilter_reserve           188540 ns       188414 ns         3715 bytes_per_second=364.648Mi/s items_per_second=5.30747k/s
bm_ryml_yaml_inplace_nofilter                   190357 ns       190241 ns         3683 bytes_per_second=361.145Mi/s items_per_second=5.25649k/s
bm_ryml_yaml_inplace_reserve                    349812 ns       349547 ns         2003 bytes_per_second=196.553Mi/s items_per_second=2.86085k/s
bm_ryml_yaml_inplace                            348242 ns       347977 ns         2017 bytes_per_second=197.44Mi/s items_per_second=2.87376k/s
bm_ryml_yaml_arena_nofilter_reserve             190456 ns       190318 ns         3612 bytes_per_second=360.999Mi/s items_per_second=5.25437k/s
bm_ryml_yaml_arena_nofilter                     192535 ns       191751 ns         3662 bytes_per_second=358.302Mi/s items_per_second=5.21511k/s
bm_ryml_yaml_arena_reserve                      348729 ns       348325 ns         2011 bytes_per_second=197.243Mi/s items_per_second=2.87088k/s
bm_ryml_yaml_arena                              351491 ns       351187 ns         2008 bytes_per_second=195.636Mi/s items_per_second=2.84749k/s
bm_libyaml_arena                                549795 ns       549259 ns         1287 bytes_per_second=125.086Mi/s items_per_second=1.82064k/s
bm_libyaml_arena_reuse                          543718 ns       543283 ns         1288 bytes_per_second=126.462Mi/s items_per_second=1.84066k/s
bm_libfyaml_arena                               437801 ns       437398 ns         1582 bytes_per_second=157.076Mi/s items_per_second=2.28625k/s
bm_yamlcpp_arena                               5759082 ns      5749373 ns          118 bytes_per_second=11.9499Mi/s items_per_second=173.932/s
Built target ryml-bm-parse-scalar_plain_multiline
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_plain_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_multiline.json;/proj/rapidyaml/bm/cases/scalar_plain_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_multiline.json
-----------------------------------
running case: EMIT/scalar_plain_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_plain_multiline.yml
-----------------------------------
2024-05-07T20:38:39+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.42, 1.31, 1.18
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve           27415 ns        27389 ns        25599 bytes_per_second=2.44971Gi/s items_per_second=36.5114k/s
bm_ryml_yaml_json_str_reserve     124236 ns       124131 ns         5520 bytes_per_second=553.483Mi/s items_per_second=8.05598k/s
bm_ryml_yaml_str                   27048 ns        27021 ns        26160 bytes_per_second=2.48302Gi/s items_per_second=37.008k/s
bm_ryml_yaml_json_str             124126 ns       124020 ns         4895 bytes_per_second=553.981Mi/s items_per_second=8.06323k/s
bm_ryml_yaml_ostream             6975580 ns      6962721 ns          798 bytes_per_second=9.86749Mi/s items_per_second=143.622/s
bm_ryml_yaml_json_ostream        7519444 ns      7504758 ns         1000 bytes_per_second=9.15481Mi/s items_per_second=133.249/s
bm_fyaml_str_reserve             1422290 ns      1421264 ns          489 bytes_per_second=48.3405Mi/s items_per_second=703.599/s
bm_fyaml_str                     1424376 ns      1423287 ns          487 bytes_per_second=48.2718Mi/s items_per_second=702.599/s
bm_fyaml_ostream                 2581116 ns      2576158 ns          401 bytes_per_second=26.6694Mi/s items_per_second=388.175/s
bm_yamlcpp                       8744100 ns      8731773 ns           85 bytes_per_second=7.86835Mi/s items_per_second=114.524/s
Built target ryml-bm-emit-scalar_plain_multiline
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_plain_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_singleline.json;/proj/rapidyaml/bm/cases/scalar_plain_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_plain_singleline.json
-----------------------------------
running case: PARSE/scalar_plain_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_plain_singleline.yml
-----------------------------------
2024-05-07T20:39:00+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.52, 1.33, 1.19
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve      16500 ns        16488 ns        42472 bytes_per_second=270.514Mi/s items_per_second=60.6488k/s
bm_ryml_yaml_inplace_reuse_nofilter              16952 ns        16912 ns        42893 bytes_per_second=263.741Mi/s items_per_second=59.1302k/s
bm_ryml_yaml_inplace_reuse_reserve               16778 ns        16756 ns        42261 bytes_per_second=266.186Mi/s items_per_second=59.6786k/s
bm_ryml_yaml_inplace_reuse                       16455 ns        16446 ns        42223 bytes_per_second=271.213Mi/s items_per_second=60.8055k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve        16405 ns        16403 ns        42770 bytes_per_second=271.925Mi/s items_per_second=60.9653k/s
bm_ryml_yaml_arena_reuse_nofilter                17838 ns        17835 ns        38929 bytes_per_second=250.085Mi/s items_per_second=56.0686k/s
bm_ryml_yaml_arena_reuse_reserve                 16587 ns        16584 ns        42254 bytes_per_second=268.953Mi/s items_per_second=60.2989k/s
bm_ryml_yaml_arena_reuse                         16576 ns        16576 ns        42352 bytes_per_second=269.086Mi/s items_per_second=60.3287k/s
bm_ryml_yaml_inplace_nofilter_reserve            16365 ns        16358 ns        42663 bytes_per_second=272.662Mi/s items_per_second=61.1304k/s
bm_ryml_yaml_inplace_nofilter                    17579 ns        17588 ns        39613 bytes_per_second=253.598Mi/s items_per_second=56.8564k/s
bm_ryml_yaml_inplace_reserve                     16529 ns        16520 ns        42262 bytes_per_second=269.991Mi/s items_per_second=60.5316k/s
bm_ryml_yaml_inplace                             17727 ns        17735 ns        39559 bytes_per_second=251.5Mi/s items_per_second=56.386k/s
bm_ryml_yaml_arena_nofilter_reserve              16512 ns        16508 ns        42355 bytes_per_second=270.186Mi/s items_per_second=60.5753k/s
bm_ryml_yaml_arena_nofilter                      17775 ns        17782 ns        39234 bytes_per_second=250.839Mi/s items_per_second=56.2377k/s
bm_ryml_yaml_arena_reserve                       16693 ns        16689 ns        41927 bytes_per_second=267.258Mi/s items_per_second=59.9187k/s
bm_ryml_yaml_arena                               17660 ns        17648 ns        39567 bytes_per_second=252.744Mi/s items_per_second=56.6648k/s
bm_libyaml_arena                                 64483 ns        64436 ns        10825 bytes_per_second=69.2211Mi/s items_per_second=15.5193k/s
bm_libyaml_arena_reuse                           62727 ns        62701 ns        11120 bytes_per_second=71.1366Mi/s items_per_second=15.9487k/s
bm_libfyaml_arena                                81354 ns        81291 ns         8664 bytes_per_second=54.869Mi/s items_per_second=12.3015k/s
bm_yamlcpp_arena                                489846 ns       489453 ns         1429 bytes_per_second=9.1129Mi/s items_per_second=2.0431k/s
Built target ryml-bm-parse-scalar_plain_singleline
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_plain_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_singleline.json;/proj/rapidyaml/bm/cases/scalar_plain_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_plain_singleline.json
-----------------------------------
running case: EMIT/scalar_plain_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_plain_singleline.yml
-----------------------------------
2024-05-07T20:39:19+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.57, 1.36, 1.20
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve            3793 ns         3790 ns       184218 bytes_per_second=1.14923Gi/s items_per_second=263.838k/s
bm_ryml_yaml_json_str_reserve      13430 ns        13419 ns        51288 bytes_per_second=332.393Mi/s items_per_second=74.5219k/s
bm_ryml_yaml_str                    3777 ns         3774 ns       184669 bytes_per_second=1.15413Gi/s items_per_second=264.964k/s
bm_ryml_yaml_json_str              13446 ns        13435 ns        49007 bytes_per_second=331.989Mi/s items_per_second=74.4315k/s
bm_ryml_yaml_ostream              523785 ns       522471 ns         1000 bytes_per_second=8.537Mi/s items_per_second=1.91398k/s
bm_ryml_yaml_json_ostream        1837996 ns      1834103 ns         3659 bytes_per_second=2.43189Mi/s items_per_second=545.226/s
bm_fyaml_str_reserve              102230 ns       102149 ns         6869 bytes_per_second=43.6652Mi/s items_per_second=9.78966k/s
bm_fyaml_str                      101640 ns       101565 ns         6861 bytes_per_second=43.9163Mi/s items_per_second=9.84596k/s
bm_fyaml_ostream                  566094 ns       564656 ns         2579 bytes_per_second=7.8992Mi/s items_per_second=1.77099k/s
bm_yamlcpp                        769356 ns       768142 ns         1228 bytes_per_second=5.80666Mi/s items_per_second=1.30184k/s
Built target ryml-bm-emit-scalar_plain_singleline
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_block_literal_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_multiline.json;/proj/rapidyaml/bm/cases/scalar_block_literal_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_multiline.json
-----------------------------------
running case: PARSE/scalar_block_literal_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_block_literal_multiline.yml
-----------------------------------
2024-05-07T20:39:34+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.44, 1.34, 1.20
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve      56948 ns        56904 ns        11679 bytes_per_second=1.18785Gi/s items_per_second=17.5734k/s
bm_ryml_yaml_inplace_reuse_nofilter              57139 ns        57096 ns        12344 bytes_per_second=1.18386Gi/s items_per_second=17.5143k/s
bm_ryml_yaml_inplace_reuse_reserve              113146 ns       113070 ns         6204 bytes_per_second=612.148Mi/s items_per_second=8.84406k/s
bm_ryml_yaml_inplace_reuse                      112712 ns       112649 ns         6225 bytes_per_second=614.439Mi/s items_per_second=8.87715k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve        57877 ns        57832 ns        12114 bytes_per_second=1.1688Gi/s items_per_second=17.2916k/s
bm_ryml_yaml_arena_reuse_nofilter                59618 ns        59572 ns        11684 bytes_per_second=1.13465Gi/s items_per_second=16.7864k/s
bm_ryml_yaml_arena_reuse_reserve                114150 ns       114075 ns         6164 bytes_per_second=606.755Mi/s items_per_second=8.76613k/s
bm_ryml_yaml_arena_reuse                        113982 ns       113908 ns         6146 bytes_per_second=607.648Mi/s items_per_second=8.77903k/s
bm_ryml_yaml_inplace_nofilter_reserve            56601 ns        56555 ns        12363 bytes_per_second=1.19518Gi/s items_per_second=17.6819k/s
bm_ryml_yaml_inplace_nofilter                    57969 ns        57931 ns        12067 bytes_per_second=1.1668Gi/s items_per_second=17.262k/s
bm_ryml_yaml_inplace_reserve                    112759 ns       112668 ns         6217 bytes_per_second=614.332Mi/s items_per_second=8.8756k/s
bm_ryml_yaml_inplace                            115214 ns       115127 ns         6131 bytes_per_second=601.211Mi/s items_per_second=8.68604k/s
bm_ryml_yaml_arena_nofilter_reserve              58691 ns        58640 ns        11870 bytes_per_second=1.15268Gi/s items_per_second=17.0531k/s
bm_ryml_yaml_arena_nofilter                      60029 ns        59996 ns        11655 bytes_per_second=1.12663Gi/s items_per_second=16.6677k/s
bm_ryml_yaml_arena_reserve                      114334 ns       114252 ns         6108 bytes_per_second=605.815Mi/s items_per_second=8.75256k/s
bm_ryml_yaml_arena                              115480 ns       115397 ns         6065 bytes_per_second=599.806Mi/s items_per_second=8.66574k/s
bm_libyaml_arena                                423859 ns       423542 ns         1654 bytes_per_second=163.421Mi/s items_per_second=2.36104k/s
bm_libyaml_arena_reuse                          422738 ns       422441 ns         1658 bytes_per_second=163.847Mi/s items_per_second=2.36719k/s
bm_libfyaml_arena                               416220 ns       415893 ns         1683 bytes_per_second=166.427Mi/s items_per_second=2.40447k/s
bm_yamlcpp_arena                               2709467 ns      2707350 ns          259 bytes_per_second=25.5659Mi/s items_per_second=369.365/s
Built target ryml-bm-parse-scalar_block_literal_multiline
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_block_literal_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_multiline.json;/proj/rapidyaml/bm/cases/scalar_block_literal_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_multiline.json
-----------------------------------
running case: EMIT/scalar_block_literal_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_block_literal_multiline.yml
-----------------------------------
2024-05-07T20:39:53+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.66, 1.40, 1.22
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve           43857 ns        43827 ns        15967 bytes_per_second=1.54227Gi/s items_per_second=22.8168k/s
bm_ryml_yaml_json_str_reserve     115148 ns       115066 ns         6085 bytes_per_second=601.533Mi/s items_per_second=8.69069k/s
bm_ryml_yaml_str                   43888 ns        43858 ns        15954 bytes_per_second=1.54118Gi/s items_per_second=22.8007k/s
bm_ryml_yaml_json_str             115207 ns       115125 ns         6066 bytes_per_second=601.224Mi/s items_per_second=8.68623k/s
bm_ryml_yaml_ostream             6879048 ns      6869075 ns          767 bytes_per_second=10.0764Mi/s items_per_second=145.58/s
bm_ryml_yaml_json_ostream        7542377 ns      7531520 ns         1000 bytes_per_second=9.19015Mi/s items_per_second=132.775/s
bm_fyaml_str_reserve             1451802 ns      1450738 ns          478 bytes_per_second=47.7107Mi/s items_per_second=689.304/s
bm_fyaml_str                     1454379 ns      1453307 ns          478 bytes_per_second=47.6264Mi/s items_per_second=688.086/s
bm_fyaml_ostream                 2652956 ns      2647835 ns          390 bytes_per_second=26.1405Mi/s items_per_second=377.667/s
bm_yamlcpp                       3146877 ns      3141757 ns          259 bytes_per_second=22.0309Mi/s items_per_second=318.293/s
Built target ryml-bm-emit-scalar_block_literal_multiline
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_block_literal_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_singleline.json;/proj/rapidyaml/bm/cases/scalar_block_literal_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_literal_singleline.json
-----------------------------------
running case: PARSE/scalar_block_literal_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_block_literal_singleline.yml
-----------------------------------
2024-05-07T20:40:14+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.62, 1.41, 1.23
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve      13388 ns        13373 ns        51995 bytes_per_second=374.964Mi/s items_per_second=74.7771k/s
bm_ryml_yaml_inplace_reuse_nofilter              13321 ns        13305 ns        52751 bytes_per_second=376.886Mi/s items_per_second=75.1604k/s
bm_ryml_yaml_inplace_reuse_reserve               18033 ns        18012 ns        38860 bytes_per_second=278.39Mi/s items_per_second=55.5178k/s
bm_ryml_yaml_inplace_reuse                       18052 ns        18038 ns        38842 bytes_per_second=277.997Mi/s items_per_second=55.4396k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve        13417 ns        13410 ns        52032 bytes_per_second=373.935Mi/s items_per_second=74.572k/s
bm_ryml_yaml_arena_reuse_nofilter                14710 ns        14703 ns        47520 bytes_per_second=341.042Mi/s items_per_second=68.0123k/s
bm_ryml_yaml_arena_reuse_reserve                 18145 ns        18136 ns        38641 bytes_per_second=276.495Mi/s items_per_second=55.1399k/s
bm_ryml_yaml_arena_reuse                         18135 ns        18125 ns        38575 bytes_per_second=276.651Mi/s items_per_second=55.1711k/s
bm_ryml_yaml_inplace_nofilter_reserve            13307 ns        13292 ns        52638 bytes_per_second=377.241Mi/s items_per_second=75.2312k/s
bm_ryml_yaml_inplace_nofilter                    14408 ns        14412 ns        48379 bytes_per_second=347.935Mi/s items_per_second=69.3869k/s
bm_ryml_yaml_inplace_reserve                     18058 ns        18044 ns        38612 bytes_per_second=277.898Mi/s items_per_second=55.4198k/s
bm_ryml_yaml_inplace                             19207 ns        19207 ns        36437 bytes_per_second=261.071Mi/s items_per_second=52.064k/s
bm_ryml_yaml_arena_nofilter_reserve              13447 ns        13435 ns        52075 bytes_per_second=373.236Mi/s items_per_second=74.4326k/s
bm_ryml_yaml_arena_nofilter                      14627 ns        14633 ns        47849 bytes_per_second=342.68Mi/s items_per_second=68.3389k/s
bm_ryml_yaml_arena_reserve                       18219 ns        18211 ns        38454 bytes_per_second=275.346Mi/s items_per_second=54.9109k/s
bm_ryml_yaml_arena                               19216 ns        19202 ns        36425 bytes_per_second=261.146Mi/s items_per_second=52.079k/s
bm_libyaml_arena                                 56018 ns        55978 ns        12371 bytes_per_second=89.578Mi/s items_per_second=17.8641k/s
bm_libyaml_arena_reuse                           55804 ns        55771 ns        12536 bytes_per_second=89.9108Mi/s items_per_second=17.9304k/s
bm_libfyaml_arena                                83904 ns        83836 ns         8306 bytes_per_second=59.8121Mi/s items_per_second=11.928k/s
bm_yamlcpp_arena                                333144 ns       332873 ns         2098 bytes_per_second=15.0641Mi/s items_per_second=3.00415k/s
Built target ryml-bm-parse-scalar_block_literal_singleline
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_block_literal_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_singleline.json;/proj/rapidyaml/bm/cases/scalar_block_literal_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_literal_singleline.json
-----------------------------------
running case: EMIT/scalar_block_literal_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_block_literal_singleline.yml
-----------------------------------
2024-05-07T20:40:33+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.44, 1.38, 1.22
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve            5291 ns         5286 ns       134160 bytes_per_second=948.646Mi/s items_per_second=189.184k/s
bm_ryml_yaml_json_str_reserve       8058 ns         8050 ns        85502 bytes_per_second=622.903Mi/s items_per_second=124.222k/s
bm_ryml_yaml_str                    5175 ns         5169 ns       134374 bytes_per_second=970.139Mi/s items_per_second=193.47k/s
bm_ryml_yaml_json_str               7924 ns         7918 ns        85694 bytes_per_second=633.278Mi/s items_per_second=126.291k/s
bm_ryml_yaml_ostream              614933 ns       613353 ns         1000 bytes_per_second=8.17543Mi/s items_per_second=1.63038k/s
bm_ryml_yaml_json_ostream        1933824 ns      1929871 ns         3613 bytes_per_second=2.59832Mi/s items_per_second=518.169/s
bm_fyaml_str_reserve              110796 ns       110721 ns         6301 bytes_per_second=45.2889Mi/s items_per_second=9.03174k/s
bm_fyaml_str                      111063 ns       110986 ns         6318 bytes_per_second=45.1805Mi/s items_per_second=9.01012k/s
bm_fyaml_ostream                  577845 ns       576321 ns         2330 bytes_per_second=8.70074Mi/s items_per_second=1.73514k/s
bm_yamlcpp                        834685 ns       833368 ns         1078 bytes_per_second=6.01705Mi/s items_per_second=1.19995k/s
Built target ryml-bm-emit-scalar_block_literal_singleline
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_block_folded_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_multiline.json;/proj/rapidyaml/bm/cases/scalar_block_folded_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_multiline.json
-----------------------------------
running case: PARSE/scalar_block_folded_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_block_folded_multiline.yml
-----------------------------------
2024-05-07T20:40:49+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.65, 1.43, 1.24
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve      56740 ns        56688 ns        12044 bytes_per_second=1.19239Gi/s items_per_second=17.6406k/s
bm_ryml_yaml_inplace_reuse_nofilter              56596 ns        56556 ns        12340 bytes_per_second=1.19516Gi/s items_per_second=17.6816k/s
bm_ryml_yaml_inplace_reuse_reserve              144497 ns       144395 ns         4850 bytes_per_second=479.349Mi/s items_per_second=6.92543k/s
bm_ryml_yaml_inplace_reuse                      144370 ns       144272 ns         4846 bytes_per_second=479.759Mi/s items_per_second=6.93135k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve        57962 ns        57918 ns        12040 bytes_per_second=1.16706Gi/s items_per_second=17.2658k/s
bm_ryml_yaml_arena_reuse_nofilter                59601 ns        59558 ns        11777 bytes_per_second=1.13492Gi/s items_per_second=16.7904k/s
bm_ryml_yaml_arena_reuse_reserve                145990 ns       145889 ns         4811 bytes_per_second=474.442Mi/s items_per_second=6.85454k/s
bm_ryml_yaml_arena_reuse                        146004 ns       145901 ns         4818 bytes_per_second=474.402Mi/s items_per_second=6.85395k/s
bm_ryml_yaml_inplace_nofilter_reserve            56596 ns        56548 ns        12361 bytes_per_second=1.19533Gi/s items_per_second=17.684k/s
bm_ryml_yaml_inplace_nofilter                    57963 ns        57930 ns        12066 bytes_per_second=1.16682Gi/s items_per_second=17.2622k/s
bm_ryml_yaml_inplace_reserve                    144419 ns       144328 ns         4805 bytes_per_second=479.574Mi/s items_per_second=6.92869k/s
bm_ryml_yaml_inplace                            145807 ns       145725 ns         4807 bytes_per_second=474.977Mi/s items_per_second=6.86226k/s
bm_ryml_yaml_arena_nofilter_reserve              58420 ns        58380 ns        12039 bytes_per_second=1.15781Gi/s items_per_second=17.1291k/s
bm_ryml_yaml_arena_nofilter                      59385 ns        59366 ns        11786 bytes_per_second=1.13859Gi/s items_per_second=16.8447k/s
bm_ryml_yaml_arena_reserve                      146114 ns       146016 ns         4803 bytes_per_second=474.029Mi/s items_per_second=6.84857k/s
bm_ryml_yaml_arena                              147261 ns       147157 ns         4760 bytes_per_second=470.352Mi/s items_per_second=6.79545k/s
bm_libyaml_arena                                423057 ns       422750 ns         1657 bytes_per_second=163.727Mi/s items_per_second=2.36546k/s
bm_libyaml_arena_reuse                          421460 ns       421146 ns         1664 bytes_per_second=164.351Mi/s items_per_second=2.37447k/s
bm_libfyaml_arena                               415152 ns       414845 ns         1688 bytes_per_second=166.847Mi/s items_per_second=2.41054k/s
bm_yamlcpp_arena                               2714758 ns      2712640 ns          258 bytes_per_second=25.516Mi/s items_per_second=368.645/s
Built target ryml-bm-parse-scalar_block_folded_multiline
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_block_folded_multiline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_multiline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_multiline.json;/proj/rapidyaml/bm/cases/scalar_block_folded_multiline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_multiline.json
-----------------------------------
running case: EMIT/scalar_block_folded_multiline.yml
file: /proj/rapidyaml/bm/cases/scalar_block_folded_multiline.yml
-----------------------------------
2024-05-07T20:41:07+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.66, 1.45, 1.25
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve           51232 ns        51193 ns        13630 bytes_per_second=1.32036Gi/s items_per_second=19.5339k/s
bm_ryml_yaml_json_str_reserve     107599 ns       107523 ns         6503 bytes_per_second=643.732Mi/s items_per_second=9.30037k/s
bm_ryml_yaml_str                   51122 ns        51086 ns        13664 bytes_per_second=1.32312Gi/s items_per_second=19.5747k/s
bm_ryml_yaml_json_str             107669 ns       107596 ns         6500 bytes_per_second=643.291Mi/s items_per_second=9.29399k/s
bm_ryml_yaml_ostream             7480991 ns      7470398 ns          885 bytes_per_second=9.26534Mi/s items_per_second=133.862/s
bm_ryml_yaml_json_ostream        7649037 ns      7636454 ns         1000 bytes_per_second=9.06386Mi/s items_per_second=130.951/s
bm_fyaml_str_reserve             1470722 ns      1469477 ns          470 bytes_per_second=47.1023Mi/s items_per_second=680.514/s
bm_fyaml_str                     1469266 ns      1468134 ns          474 bytes_per_second=47.1454Mi/s items_per_second=681.137/s
bm_fyaml_ostream                 2650869 ns      2645659 ns          388 bytes_per_second=26.162Mi/s items_per_second=377.978/s
bm_yamlcpp                       9365995 ns      9356456 ns           76 bytes_per_second=7.39765Mi/s items_per_second=106.878/s
Built target ryml-bm-emit-scalar_block_folded_multiline
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-scalar_block_folded_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_singleline.json;/proj/rapidyaml/bm/cases/scalar_block_folded_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-scalar_block_folded_singleline.json
-----------------------------------
running case: PARSE/scalar_block_folded_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_block_folded_singleline.yml
-----------------------------------
2024-05-07T20:41:29+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.47, 1.42, 1.24
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve      13299 ns        13296 ns        52804 bytes_per_second=377.132Mi/s items_per_second=75.2095k/s
bm_ryml_yaml_inplace_reuse_nofilter              13410 ns        13325 ns        52890 bytes_per_second=376.33Mi/s items_per_second=75.0495k/s
bm_ryml_yaml_inplace_reuse_reserve               19978 ns        19974 ns        35582 bytes_per_second=251.048Mi/s items_per_second=50.0652k/s
bm_ryml_yaml_inplace_reuse                       19700 ns        19695 ns        35503 bytes_per_second=254.599Mi/s items_per_second=50.7733k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve        13405 ns        13409 ns        52280 bytes_per_second=373.967Mi/s items_per_second=74.5784k/s
bm_ryml_yaml_arena_reuse_nofilter                14894 ns        14904 ns        46752 bytes_per_second=336.444Mi/s items_per_second=67.0952k/s
bm_ryml_yaml_arena_reuse_reserve                 19836 ns        19836 ns        35373 bytes_per_second=252.788Mi/s items_per_second=50.4122k/s
bm_ryml_yaml_arena_reuse                         19834 ns        19836 ns        35360 bytes_per_second=252.789Mi/s items_per_second=50.4123k/s
bm_ryml_yaml_inplace_nofilter_reserve            13337 ns        13337 ns        52596 bytes_per_second=375.981Mi/s items_per_second=74.98k/s
bm_ryml_yaml_inplace_nofilter                    14428 ns        14449 ns        48401 bytes_per_second=347.04Mi/s items_per_second=69.2084k/s
bm_ryml_yaml_inplace_reserve                     19958 ns        19837 ns        35367 bytes_per_second=252.785Mi/s items_per_second=50.4117k/s
bm_ryml_yaml_inplace                             21586 ns        21602 ns        33132 bytes_per_second=232.122Mi/s items_per_second=46.291k/s
bm_ryml_yaml_arena_nofilter_reserve              13495 ns        13501 ns        52053 bytes_per_second=371.405Mi/s items_per_second=74.0674k/s
bm_ryml_yaml_arena_nofilter                      14621 ns        14641 ns        47805 bytes_per_second=342.484Mi/s items_per_second=68.2999k/s
bm_ryml_yaml_arena_reserve                       19951 ns        19954 ns        35119 bytes_per_second=251.305Mi/s items_per_second=50.1165k/s
bm_ryml_yaml_arena                               20850 ns        20835 ns        33598 bytes_per_second=240.671Mi/s items_per_second=47.9959k/s
bm_libyaml_arena                                 57313 ns        57266 ns        12259 bytes_per_second=87.5629Mi/s items_per_second=17.4622k/s
bm_libyaml_arena_reuse                           56386 ns        56357 ns        12399 bytes_per_second=88.9755Mi/s items_per_second=17.7439k/s
bm_libfyaml_arena                                89388 ns        89263 ns         8115 bytes_per_second=56.1756Mi/s items_per_second=11.2028k/s
bm_yamlcpp_arena                                335057 ns       334790 ns         2087 bytes_per_second=14.9778Mi/s items_per_second=2.98695k/s
Built target ryml-bm-parse-scalar_block_folded_singleline
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-scalar_block_folded_singleline: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_singleline.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_singleline.json;/proj/rapidyaml/bm/cases/scalar_block_folded_singleline.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-scalar_block_folded_singleline.json
-----------------------------------
running case: EMIT/scalar_block_folded_singleline.yml
file: /proj/rapidyaml/bm/cases/scalar_block_folded_singleline.yml
-----------------------------------
2024-05-07T20:41:48+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.56, 1.44, 1.25
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve            5934 ns         5929 ns       117890 bytes_per_second=845.733Mi/s items_per_second=168.66k/s
bm_ryml_yaml_json_str_reserve       8023 ns         8017 ns        86673 bytes_per_second=625.455Mi/s items_per_second=124.731k/s
bm_ryml_yaml_str                    5976 ns         5970 ns       118272 bytes_per_second=839.9Mi/s items_per_second=167.497k/s
bm_ryml_yaml_json_str               8194 ns         8187 ns        81058 bytes_per_second=612.518Mi/s items_per_second=122.151k/s
bm_ryml_yaml_ostream              608357 ns       606753 ns         1000 bytes_per_second=8.26436Mi/s items_per_second=1.64812k/s
bm_ryml_yaml_json_ostream        2035556 ns      2029467 ns         3611 bytes_per_second=2.47081Mi/s items_per_second=492.74/s
bm_fyaml_str_reserve              121343 ns       121230 ns         5895 bytes_per_second=41.3629Mi/s items_per_second=8.24879k/s
bm_fyaml_str                      120244 ns       120147 ns         5373 bytes_per_second=41.7358Mi/s items_per_second=8.32316k/s
bm_fyaml_ostream                  567138 ns       565508 ns         2276 bytes_per_second=8.8671Mi/s items_per_second=1.76832k/s
bm_yamlcpp                        843857 ns       842443 ns         1048 bytes_per_second=5.95224Mi/s items_per_second=1.18702k/s
Built target ryml-bm-emit-scalar_block_folded_singleline
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_maps_blck_outer1000_inner10: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner10.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner10.json;/proj/rapidyaml/bm/cases/style_maps_blck_outer1000_inner10.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner10.json
-----------------------------------
running case: PARSE/style_maps_blck_outer1000_inner10.yml
file: /proj/rapidyaml/bm/cases/style_maps_blck_outer1000_inner10.yml
-----------------------------------
2024-05-07T20:42:03+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.65, 1.47, 1.27
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve     843182 ns       842236 ns          842 bytes_per_second=100.664Mi/s items_per_second=1.18732k/s
bm_ryml_yaml_inplace_reuse_nofilter             838498 ns       837610 ns          799 bytes_per_second=101.22Mi/s items_per_second=1.19387k/s
bm_ryml_yaml_inplace_reuse_reserve              829361 ns       828517 ns          845 bytes_per_second=102.331Mi/s items_per_second=1.20698k/s
bm_ryml_yaml_inplace_reuse                      839711 ns       838572 ns          845 bytes_per_second=101.104Mi/s items_per_second=1.1925k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve       845846 ns       844921 ns          838 bytes_per_second=100.344Mi/s items_per_second=1.18354k/s
bm_ryml_yaml_arena_reuse_nofilter              2239661 ns      2236003 ns          313 bytes_per_second=37.917Mi/s items_per_second=447.227/s
bm_ryml_yaml_arena_reuse_reserve                835857 ns       835011 ns          839 bytes_per_second=101.535Mi/s items_per_second=1.19759k/s
bm_ryml_yaml_arena_reuse                        838084 ns       837184 ns          840 bytes_per_second=101.271Mi/s items_per_second=1.19448k/s
bm_ryml_yaml_inplace_nofilter_reserve           829823 ns       828920 ns          844 bytes_per_second=102.281Mi/s items_per_second=1.20639k/s
bm_ryml_yaml_inplace_nofilter                  2223362 ns      2220068 ns          316 bytes_per_second=38.1892Mi/s items_per_second=450.437/s
bm_ryml_yaml_inplace_reserve                    831475 ns       830437 ns          844 bytes_per_second=102.094Mi/s items_per_second=1.20419k/s
bm_ryml_yaml_inplace                           2219982 ns      2216834 ns          315 bytes_per_second=38.2449Mi/s items_per_second=451.094/s
bm_ryml_yaml_arena_nofilter_reserve             837748 ns       836766 ns          837 bytes_per_second=101.322Mi/s items_per_second=1.19508k/s
bm_ryml_yaml_arena_nofilter                    2239028 ns      2235733 ns          313 bytes_per_second=37.9216Mi/s items_per_second=447.281/s
bm_ryml_yaml_arena_reserve                      835641 ns       834756 ns          839 bytes_per_second=101.566Mi/s items_per_second=1.19796k/s
bm_ryml_yaml_arena                             2232498 ns      2228986 ns          314 bytes_per_second=38.0364Mi/s items_per_second=448.634/s
bm_libyaml_arena                               4674704 ns      4667979 ns          150 bytes_per_second=18.1626Mi/s items_per_second=214.225/s
bm_libyaml_arena_reuse                         3441967 ns      3438113 ns          203 bytes_per_second=24.6596Mi/s items_per_second=290.857/s
bm_libfyaml_arena                              7968296 ns      7956985 ns           87 bytes_per_second=10.6551Mi/s items_per_second=125.676/s
bm_yamlcpp_arena                              24253804 ns     24223715 ns           29 bytes_per_second=3.49998Mi/s items_per_second=41.2819/s
Built target ryml-bm-parse-style_maps_blck_outer1000_inner10
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_maps_blck_outer1000_inner10: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner10.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner10.json;/proj/rapidyaml/bm/cases/style_maps_blck_outer1000_inner10.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner10.json
-----------------------------------
running case: EMIT/style_maps_blck_outer1000_inner10.yml
file: /proj/rapidyaml/bm/cases/style_maps_blck_outer1000_inner10.yml
-----------------------------------
2024-05-07T20:42:23+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.55, 1.45, 1.27
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve          249079 ns       248817 ns         2881 bytes_per_second=340.742Mi/s items_per_second=4.01901k/s
bm_ryml_yaml_json_str_reserve     397242 ns       396778 ns         1767 bytes_per_second=213.678Mi/s items_per_second=2.5203k/s
bm_ryml_yaml_str                  249084 ns       248800 ns         2826 bytes_per_second=340.765Mi/s items_per_second=4.01929k/s
bm_ryml_yaml_json_str             398405 ns       397899 ns         1738 bytes_per_second=213.076Mi/s items_per_second=2.5132k/s
bm_ryml_yaml_ostream             4970519 ns      4961739 ns          416 bytes_per_second=17.0873Mi/s items_per_second=201.542/s
bm_ryml_yaml_json_ostream        6337114 ns      6326236 ns          603 bytes_per_second=13.4017Mi/s items_per_second=158.072/s
bm_fyaml_str_reserve             3259542 ns      3255073 ns          215 bytes_per_second=26.0463Mi/s items_per_second=307.213/s
bm_fyaml_str                     3265393 ns      3260319 ns          214 bytes_per_second=26.0044Mi/s items_per_second=306.718/s
bm_fyaml_ostream                 4548569 ns      4505924 ns          166 bytes_per_second=18.8158Mi/s items_per_second=221.93/s
bm_yamlcpp                      13246077 ns     13214964 ns           52 bytes_per_second=6.41565Mi/s items_per_second=75.6718/s
Built target ryml-bm-emit-style_maps_blck_outer1000_inner10
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_maps_flow_outer1000_inner10_json: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10_json.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10_json.json;/proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner10_json.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10_json.json
-----------------------------------
running case: PARSE/style_maps_flow_outer1000_inner10_json.yml
file: /proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner10_json.yml
-----------------------------------
2024-05-07T20:42:37+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.42, 1.43, 1.26
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve     671370 ns       670555 ns         1045 bytes_per_second=103.684Mi/s items_per_second=1.4913k/s
bm_ryml_yaml_inplace_reuse_nofilter             672934 ns       672164 ns         1045 bytes_per_second=103.436Mi/s items_per_second=1.48773k/s
bm_ryml_yaml_inplace_reuse_reserve              689256 ns       688284 ns         1046 bytes_per_second=101.013Mi/s items_per_second=1.45289k/s
bm_ryml_yaml_inplace_reuse                      667410 ns       666541 ns         1044 bytes_per_second=104.308Mi/s items_per_second=1.50028k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve       673103 ns       672327 ns         1039 bytes_per_second=103.411Mi/s items_per_second=1.48737k/s
bm_ryml_yaml_arena_reuse_nofilter              2079294 ns      2075885 ns          334 bytes_per_second=33.4921Mi/s items_per_second=481.722/s
bm_ryml_yaml_arena_reuse_reserve                672170 ns       671433 ns         1041 bytes_per_second=103.548Mi/s items_per_second=1.48935k/s
bm_ryml_yaml_arena_reuse                        672508 ns       671675 ns         1044 bytes_per_second=103.511Mi/s items_per_second=1.48881k/s
bm_ryml_yaml_inplace_nofilter_reserve           667360 ns       666633 ns         1046 bytes_per_second=104.294Mi/s items_per_second=1.50008k/s
bm_ryml_yaml_inplace_nofilter                   974748 ns       973838 ns          720 bytes_per_second=71.3935Mi/s items_per_second=1.02686k/s
bm_ryml_yaml_inplace_reserve                    669056 ns       668106 ns         1053 bytes_per_second=104.064Mi/s items_per_second=1.49677k/s
bm_ryml_yaml_inplace                            988840 ns       987721 ns          719 bytes_per_second=70.39Mi/s items_per_second=1.01243k/s
bm_ryml_yaml_arena_nofilter_reserve             687126 ns       686160 ns         1008 bytes_per_second=101.326Mi/s items_per_second=1.45739k/s
bm_ryml_yaml_arena_nofilter                    1001076 ns      1000035 ns          712 bytes_per_second=69.5233Mi/s items_per_second=0.999965k/s
bm_ryml_yaml_arena_reserve                      688776 ns       687782 ns         1038 bytes_per_second=101.087Mi/s items_per_second=1.45395k/s
bm_ryml_yaml_arena                              993519 ns       992144 ns          691 bytes_per_second=70.0763Mi/s items_per_second=1.00792k/s
bm_libyaml_arena                               3918460 ns      3913628 ns          178 bytes_per_second=17.765Mi/s items_per_second=255.517/s
bm_libyaml_arena_reuse                         3825417 ns      3820393 ns          185 bytes_per_second=18.1986Mi/s items_per_second=261.753/s
bm_libfyaml_arena                              7995898 ns      7984066 ns           88 bytes_per_second=8.70806Mi/s items_per_second=125.249/s
bm_yamlcpp_arena                              24662459 ns     24628336 ns           28 bytes_per_second=2.823Mi/s items_per_second=40.6036/s
Built target ryml-bm-parse-style_maps_flow_outer1000_inner10_json
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_maps_flow_outer1000_inner10_json: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10_json.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10_json.json;/proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner10_json.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10_json.json
-----------------------------------
running case: EMIT/style_maps_flow_outer1000_inner10_json.yml
file: /proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner10_json.yml
-----------------------------------
2024-05-07T20:42:57+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.38, 1.42, 1.26
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve          238772 ns       238479 ns         2935 bytes_per_second=291.538Mi/s items_per_second=4.19324k/s
bm_ryml_yaml_json_str_reserve     397109 ns       396675 ns         1769 bytes_per_second=175.271Mi/s items_per_second=2.52096k/s
bm_ryml_yaml_str                  238732 ns       238452 ns         2948 bytes_per_second=291.572Mi/s items_per_second=4.19372k/s
bm_ryml_yaml_json_str             397121 ns       396688 ns         1767 bytes_per_second=175.265Mi/s items_per_second=2.52087k/s
bm_ryml_yaml_ostream             5553934 ns      5544506 ns          553 bytes_per_second=12.5396Mi/s items_per_second=180.359/s
bm_ryml_yaml_json_ostream        6213406 ns      6202786 ns          604 bytes_per_second=11.2088Mi/s items_per_second=161.218/s
bm_fyaml_str_reserve             2031463 ns      2028387 ns          338 bytes_per_second=34.2764Mi/s items_per_second=493.003/s
bm_fyaml_str                     2025199 ns      2022402 ns          343 bytes_per_second=34.3778Mi/s items_per_second=494.462/s
bm_fyaml_ostream                 4046503 ns      4039157 ns          208 bytes_per_second=17.2129Mi/s items_per_second=247.576/s
bm_yamlcpp                      13995342 ns     13975420 ns           51 bytes_per_second=4.97486Mi/s items_per_second=71.5542/s
Built target ryml-bm-emit-style_maps_flow_outer1000_inner10_json
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_maps_blck_outer1000_inner100: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner100.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner100.json;/proj/rapidyaml/bm/cases/style_maps_blck_outer1000_inner100.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner100.json
-----------------------------------
running case: PARSE/style_maps_blck_outer1000_inner100.yml
file: /proj/rapidyaml/bm/cases/style_maps_blck_outer1000_inner100.yml
-----------------------------------
2024-05-07T20:43:12+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.29, 1.40, 1.26
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve    8163668 ns      8150299 ns           88 bytes_per_second=114.554Mi/s items_per_second=122.695/s
bm_ryml_yaml_inplace_reuse_nofilter            8209944 ns      8195126 ns           77 bytes_per_second=113.927Mi/s items_per_second=122.024/s
bm_ryml_yaml_inplace_reuse_reserve             8109596 ns      8093036 ns           77 bytes_per_second=115.364Mi/s items_per_second=123.563/s
bm_ryml_yaml_inplace_reuse                     8352162 ns      8334396 ns           82 bytes_per_second=112.023Mi/s items_per_second=119.985/s
bm_ryml_yaml_arena_reuse_nofilter_reserve      8129584 ns      8115377 ns           79 bytes_per_second=115.047Mi/s items_per_second=123.223/s
bm_ryml_yaml_arena_reuse_nofilter             17326763 ns     17281323 ns           40 bytes_per_second=54.0264Mi/s items_per_second=57.8659/s
bm_ryml_yaml_arena_reuse_reserve               8123226 ns      8108919 ns           86 bytes_per_second=115.138Mi/s items_per_second=123.321/s
bm_ryml_yaml_arena_reuse                       8252643 ns      8237192 ns           87 bytes_per_second=113.345Mi/s items_per_second=121.401/s
bm_ryml_yaml_inplace_nofilter_reserve          8019236 ns      8003983 ns           82 bytes_per_second=116.648Mi/s items_per_second=124.938/s
bm_ryml_yaml_inplace_nofilter                 11200493 ns     11173154 ns           63 bytes_per_second=83.5617Mi/s items_per_second=89.5002/s
bm_ryml_yaml_inplace_reserve                   8032805 ns      8019994 ns           85 bytes_per_second=116.415Mi/s items_per_second=124.688/s
bm_ryml_yaml_inplace                          10997276 ns     10973433 ns           63 bytes_per_second=85.0826Mi/s items_per_second=91.1292/s
bm_ryml_yaml_arena_nofilter_reserve            8059574 ns      8047592 ns           87 bytes_per_second=116.016Mi/s items_per_second=124.261/s
bm_ryml_yaml_arena_nofilter                   11210765 ns     11186161 ns           63 bytes_per_second=83.4646Mi/s items_per_second=89.3962/s
bm_ryml_yaml_arena_reserve                     8078060 ns      8065533 ns           87 bytes_per_second=115.758Mi/s items_per_second=123.984/s
bm_ryml_yaml_arena                            11112154 ns     11088651 ns           63 bytes_per_second=84.1985Mi/s items_per_second=90.1823/s
bm_libyaml_arena                              34923399 ns     34880288 ns           20 bytes_per_second=26.7672Mi/s items_per_second=28.6695/s
bm_libyaml_arena_reuse                        33989362 ns     33938014 ns           21 bytes_per_second=27.5104Mi/s items_per_second=29.4655/s
bm_libfyaml_arena                            246433448 ns    246058352 ns            3 bytes_per_second=3.79442Mi/s items_per_second=4.06408/s
bm_yamlcpp_arena                             263285620 ns    261367208 ns            2 bytes_per_second=3.57217Mi/s items_per_second=3.82603/s
Built target ryml-bm-parse-style_maps_blck_outer1000_inner100
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_maps_blck_outer1000_inner100: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner100.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner100.json;/proj/rapidyaml/bm/cases/style_maps_blck_outer1000_inner100.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_blck_outer1000_inner100.json
-----------------------------------
running case: EMIT/style_maps_blck_outer1000_inner100.yml
file: /proj/rapidyaml/bm/cases/style_maps_blck_outer1000_inner100.yml
-----------------------------------
2024-05-07T20:43:31+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.23, 1.38, 1.25
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve         2440278 ns      2435871 ns          287 bytes_per_second=383.291Mi/s items_per_second=410.531/s
bm_ryml_yaml_json_str_reserve    3935533 ns      3930059 ns          178 bytes_per_second=237.566Mi/s items_per_second=254.449/s
bm_ryml_yaml_str                 2431584 ns      2427389 ns          286 bytes_per_second=384.631Mi/s items_per_second=411.965/s
bm_ryml_yaml_json_str            3957430 ns      3951687 ns          177 bytes_per_second=236.266Mi/s items_per_second=253.056/s
bm_ryml_yaml_ostream            19171801 ns     19140037 ns           99 bytes_per_second=48.7798Mi/s items_per_second=52.2465/s
bm_ryml_yaml_json_ostream       18165543 ns     18138844 ns           85 bytes_per_second=51.4723Mi/s items_per_second=55.1303/s
bm_fyaml_str_reserve            37709911 ns     37660021 ns           17 bytes_per_second=24.7915Mi/s items_per_second=26.5534/s
bm_fyaml_str                    37436786 ns     37390397 ns           18 bytes_per_second=24.9703Mi/s items_per_second=26.7448/s
bm_fyaml_ostream                41578116 ns     41526168 ns           16 bytes_per_second=22.4834Mi/s items_per_second=24.0812/s
bm_yamlcpp                     144830643 ns    144624183 ns            5 bytes_per_second=6.45568Mi/s items_per_second=6.91447/s
Built target ryml-bm-emit-style_maps_blck_outer1000_inner100
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_maps_flow_outer1000_inner100_json: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100_json.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100_json.json;/proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner100_json.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100_json.json
-----------------------------------
running case: PARSE/style_maps_flow_outer1000_inner100_json.yml
file: /proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner100_json.yml
-----------------------------------
2024-05-07T20:43:46+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.40, 1.41, 1.27
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve    6538012 ns      6527537 ns          107 bytes_per_second=114.397Mi/s items_per_second=153.197/s
bm_ryml_yaml_inplace_reuse_nofilter            6522582 ns      6511446 ns          107 bytes_per_second=114.68Mi/s items_per_second=153.576/s
bm_ryml_yaml_inplace_reuse_reserve             6599047 ns      6546248 ns          107 bytes_per_second=114.07Mi/s items_per_second=152.759/s
bm_ryml_yaml_inplace_reuse                     6530632 ns      6518666 ns          107 bytes_per_second=114.553Mi/s items_per_second=153.406/s
bm_ryml_yaml_arena_reuse_nofilter_reserve      6634375 ns      6622492 ns          105 bytes_per_second=112.757Mi/s items_per_second=151.001/s
bm_ryml_yaml_arena_reuse_nofilter             15890378 ns     15848304 ns           43 bytes_per_second=47.1173Mi/s items_per_second=63.0982/s
bm_ryml_yaml_arena_reuse_reserve               6622767 ns      6610922 ns          106 bytes_per_second=112.954Mi/s items_per_second=151.265/s
bm_ryml_yaml_arena_reuse                       6634484 ns      6622798 ns          106 bytes_per_second=112.751Mi/s items_per_second=150.994/s
bm_ryml_yaml_inplace_nofilter_reserve          6570305 ns      6558310 ns          107 bytes_per_second=113.86Mi/s items_per_second=152.478/s
bm_ryml_yaml_inplace_nofilter                  9597337 ns      9574796 ns           73 bytes_per_second=77.9891Mi/s items_per_second=104.441/s
bm_ryml_yaml_inplace_reserve                   6545505 ns      6533938 ns          108 bytes_per_second=114.285Mi/s items_per_second=153.047/s
bm_ryml_yaml_inplace                           9530538 ns      9508494 ns           73 bytes_per_second=78.5329Mi/s items_per_second=105.169/s
bm_ryml_yaml_arena_nofilter_reserve            6628889 ns      6617772 ns          106 bytes_per_second=112.837Mi/s items_per_second=151.108/s
bm_ryml_yaml_arena_nofilter                    9638328 ns      9616068 ns           73 bytes_per_second=77.6544Mi/s items_per_second=103.993/s
bm_ryml_yaml_arena_reserve                     6677391 ns      6637126 ns          105 bytes_per_second=112.508Mi/s items_per_second=150.668/s
bm_ryml_yaml_arena                             9641768 ns      9617503 ns           73 bytes_per_second=77.6428Mi/s items_per_second=103.977/s
bm_libyaml_arena                              37976163 ns     37924839 ns           18 bytes_per_second=19.6897Mi/s items_per_second=26.3679/s
bm_libyaml_arena_reuse                        38026963 ns     37962036 ns           19 bytes_per_second=19.6704Mi/s items_per_second=26.3421/s
bm_libfyaml_arena                            223703139 ns    223416676 ns            3 bytes_per_second=3.34232Mi/s items_per_second=4.47594/s
bm_yamlcpp_arena                             310323729 ns    309958620 ns            3 bytes_per_second=2.40913Mi/s items_per_second=3.22624/s
Built target ryml-bm-parse-style_maps_flow_outer1000_inner100_json
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_maps_flow_outer1000_inner100_json: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100_json.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100_json.json;/proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner100_json.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100_json.json
-----------------------------------
running case: EMIT/style_maps_flow_outer1000_inner100_json.yml
file: /proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner100_json.yml
-----------------------------------
2024-05-07T20:44:06+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.35, 1.40, 1.27
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve         2376711 ns      2372388 ns          278 bytes_per_second=314.759Mi/s items_per_second=421.516/s
bm_ryml_yaml_json_str_reserve    3960632 ns      3954930 ns          169 bytes_per_second=188.81Mi/s items_per_second=252.849/s
bm_ryml_yaml_str                 2373775 ns      2369245 ns          294 bytes_per_second=315.176Mi/s items_per_second=422.075/s
bm_ryml_yaml_json_str            3956327 ns      3950589 ns          177 bytes_per_second=189.017Mi/s items_per_second=253.127/s
bm_ryml_yaml_ostream            18089829 ns     18063072 ns          125 bytes_per_second=41.3401Mi/s items_per_second=55.3616/s
bm_ryml_yaml_json_ostream       18265038 ns     18239093 ns           86 bytes_per_second=40.9412Mi/s items_per_second=54.8273/s
bm_fyaml_str_reserve            28589829 ns     28554340 ns           23 bytes_per_second=26.1512Mi/s items_per_second=35.0209/s
bm_fyaml_str                    29266409 ns     29015697 ns           24 bytes_per_second=25.7354Mi/s items_per_second=34.4641/s
bm_fyaml_ostream                35244634 ns     35198565 ns           19 bytes_per_second=21.2148Mi/s items_per_second=28.4102/s
bm_yamlcpp                     163783098 ns    163572654 ns            4 bytes_per_second=4.56513Mi/s items_per_second=6.11349/s
Built target ryml-bm-emit-style_maps_flow_outer1000_inner100_json
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_maps_flow_outer1000_inner10: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10.json;/proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner10.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10.json
-----------------------------------
running case: PARSE/style_maps_flow_outer1000_inner10.yml
file: /proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner10.yml
-----------------------------------
2024-05-07T20:44:21+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.27, 1.38, 1.26
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve     672868 ns       672044 ns         1040 bytes_per_second=103.451Mi/s items_per_second=1.488k/s
bm_ryml_yaml_inplace_reuse_nofilter             671308 ns       670536 ns         1040 bytes_per_second=103.684Mi/s items_per_second=1.49134k/s
bm_ryml_yaml_inplace_reuse_reserve              671157 ns       670404 ns         1042 bytes_per_second=103.704Mi/s items_per_second=1.49164k/s
bm_ryml_yaml_inplace_reuse                      669043 ns       668387 ns         1048 bytes_per_second=104.017Mi/s items_per_second=1.49614k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve       676808 ns       676020 ns         1039 bytes_per_second=102.843Mi/s items_per_second=1.47925k/s
bm_ryml_yaml_arena_reuse_nofilter              1782479 ns      1777832 ns          394 bytes_per_second=39.1059Mi/s items_per_second=562.483/s
bm_ryml_yaml_arena_reuse_reserve                675162 ns       674397 ns         1036 bytes_per_second=103.09Mi/s items_per_second=1.48281k/s
bm_ryml_yaml_arena_reuse                        675756 ns       674932 ns         1041 bytes_per_second=103.009Mi/s items_per_second=1.48163k/s
bm_ryml_yaml_inplace_nofilter_reserve           717816 ns       716682 ns         1047 bytes_per_second=97.008Mi/s items_per_second=1.39532k/s
bm_ryml_yaml_inplace_nofilter                  1773983 ns      1769624 ns          393 bytes_per_second=39.2873Mi/s items_per_second=565.092/s
bm_ryml_yaml_inplace_reserve                    669359 ns       668521 ns         1045 bytes_per_second=103.996Mi/s items_per_second=1.49584k/s
bm_ryml_yaml_inplace                           1782098 ns      1777739 ns          394 bytes_per_second=39.108Mi/s items_per_second=562.512/s
bm_ryml_yaml_arena_nofilter_reserve             686051 ns       685123 ns         1038 bytes_per_second=101.476Mi/s items_per_second=1.45959k/s
bm_ryml_yaml_arena_nofilter                    1768420 ns      1763843 ns          393 bytes_per_second=39.4161Mi/s items_per_second=566.944/s
bm_ryml_yaml_arena_reserve                      675353 ns       674399 ns          993 bytes_per_second=103.09Mi/s items_per_second=1.4828k/s
bm_ryml_yaml_arena                             1768508 ns      1763726 ns          397 bytes_per_second=39.4187Mi/s items_per_second=566.982/s
bm_libyaml_arena                               4635974 ns      4626273 ns          151 bytes_per_second=15.028Mi/s items_per_second=216.157/s
bm_libyaml_arena_reuse                         3703555 ns      3698767 ns          189 bytes_per_second=18.7965Mi/s items_per_second=270.36/s
bm_libfyaml_arena                              8048050 ns      8033668 ns           87 bytes_per_second=8.65406Mi/s items_per_second=124.476/s
bm_yamlcpp_arena                              24265355 ns     24230372 ns           29 bytes_per_second=2.86928Mi/s items_per_second=41.2705/s
Built target ryml-bm-parse-style_maps_flow_outer1000_inner10
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_maps_flow_outer1000_inner10: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10.json;/proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner10.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10.json
-----------------------------------
running case: EMIT/style_maps_flow_outer1000_inner10.yml
file: /proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner10.yml
-----------------------------------
2024-05-07T20:44:40+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.59, 1.44, 1.28
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve          238233 ns       237939 ns         2936 bytes_per_second=292.191Mi/s items_per_second=4.20275k/s
bm_ryml_yaml_json_str_reserve     397679 ns       397156 ns         1762 bytes_per_second=175.054Mi/s items_per_second=2.5179k/s
bm_ryml_yaml_str                  241406 ns       241083 ns         2917 bytes_per_second=288.381Mi/s items_per_second=4.14795k/s
bm_ryml_yaml_json_str             397172 ns       396735 ns         1765 bytes_per_second=175.24Mi/s items_per_second=2.52058k/s
bm_ryml_yaml_ostream             5378478 ns      5360654 ns          505 bytes_per_second=12.9693Mi/s items_per_second=186.544/s
bm_ryml_yaml_json_ostream        6170364 ns      6159985 ns          596 bytes_per_second=11.2864Mi/s items_per_second=162.338/s
bm_fyaml_str_reserve             2044804 ns      2041978 ns          339 bytes_per_second=34.0473Mi/s items_per_second=489.721/s
bm_fyaml_str                     2056000 ns      2053024 ns          338 bytes_per_second=33.8641Mi/s items_per_second=487.086/s
bm_fyaml_ostream                 3970684 ns      3963621 ns          204 bytes_per_second=17.5405Mi/s items_per_second=252.295/s
bm_yamlcpp                      13946165 ns     13925103 ns           51 bytes_per_second=4.9927Mi/s items_per_second=71.8128/s
Built target ryml-bm-emit-style_maps_flow_outer1000_inner10
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_maps_flow_outer1000_inner100: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100.json;/proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner100.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100.json
-----------------------------------
running case: PARSE/style_maps_flow_outer1000_inner100.yml
file: /proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner100.yml
-----------------------------------
2024-05-07T20:44:55+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.68, 1.46, 1.30
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve    6714258 ns      6702167 ns          107 bytes_per_second=111.416Mi/s items_per_second=149.205/s
bm_ryml_yaml_inplace_reuse_nofilter            6607532 ns      6594725 ns          108 bytes_per_second=113.231Mi/s items_per_second=151.636/s
bm_ryml_yaml_inplace_reuse_reserve             6523109 ns      6511428 ns          107 bytes_per_second=114.68Mi/s items_per_second=153.576/s
bm_ryml_yaml_inplace_reuse                     6525993 ns      6515209 ns          107 bytes_per_second=114.613Mi/s items_per_second=153.487/s
bm_ryml_yaml_arena_reuse_nofilter_reserve      6680384 ns      6642551 ns          105 bytes_per_second=112.416Mi/s items_per_second=150.545/s
bm_ryml_yaml_arena_reuse_nofilter             15780532 ns     15737064 ns           44 bytes_per_second=47.4503Mi/s items_per_second=63.5443/s
bm_ryml_yaml_arena_reuse_reserve               6625617 ns      6614440 ns          105 bytes_per_second=112.894Mi/s items_per_second=151.184/s
bm_ryml_yaml_arena_reuse                       6724353 ns      6712331 ns          105 bytes_per_second=111.247Mi/s items_per_second=148.98/s
bm_ryml_yaml_inplace_nofilter_reserve          6604678 ns      6592521 ns          103 bytes_per_second=113.269Mi/s items_per_second=151.687/s
bm_ryml_yaml_inplace_nofilter                  9566035 ns      9543493 ns           73 bytes_per_second=78.2447Mi/s items_per_second=104.783/s
bm_ryml_yaml_inplace_reserve                   6542379 ns      6530837 ns          107 bytes_per_second=114.339Mi/s items_per_second=153.12/s
bm_ryml_yaml_inplace                           9533492 ns      9511937 ns           74 bytes_per_second=78.5043Mi/s items_per_second=105.131/s
bm_ryml_yaml_arena_nofilter_reserve            6631680 ns      6620344 ns          105 bytes_per_second=112.793Mi/s items_per_second=151.05/s
bm_ryml_yaml_arena_nofilter                    9648438 ns      9626963 ns           73 bytes_per_second=77.5663Mi/s items_per_second=103.875/s
bm_ryml_yaml_arena_reserve                     6617982 ns      6606094 ns          106 bytes_per_second=113.036Mi/s items_per_second=151.375/s
bm_ryml_yaml_arena                             9666939 ns      9643333 ns           71 bytes_per_second=77.4346Mi/s items_per_second=103.699/s
bm_libyaml_arena                              37109270 ns     37056849 ns           18 bytes_per_second=20.1509Mi/s items_per_second=26.9856/s
bm_libyaml_arena_reuse                        36090161 ns     36033299 ns           19 bytes_per_second=20.7233Mi/s items_per_second=27.7521/s
bm_libfyaml_arena                            230556972 ns    230243111 ns            3 bytes_per_second=3.24322Mi/s items_per_second=4.34324/s
bm_yamlcpp_arena                             272837451 ns    272481930 ns            2 bytes_per_second=2.74047Mi/s items_per_second=3.66997/s
Built target ryml-bm-parse-style_maps_flow_outer1000_inner100
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_maps_flow_outer1000_inner100: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100.json;/proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner100.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100.json
-----------------------------------
running case: EMIT/style_maps_flow_outer1000_inner100.yml
file: /proj/rapidyaml/bm/cases/style_maps_flow_outer1000_inner100.yml
-----------------------------------
2024-05-07T20:45:15+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.80, 1.50, 1.31
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve         2301174 ns      2296407 ns          306 bytes_per_second=325.172Mi/s items_per_second=435.463/s
bm_ryml_yaml_json_str_reserve    3932738 ns      3927384 ns          178 bytes_per_second=190.134Mi/s items_per_second=254.622/s
bm_ryml_yaml_str                 2320055 ns      2315725 ns          304 bytes_per_second=322.46Mi/s items_per_second=431.83/s
bm_ryml_yaml_json_str            3956518 ns      3951185 ns          177 bytes_per_second=188.988Mi/s items_per_second=253.089/s
bm_ryml_yaml_ostream            18040060 ns     18014407 ns          123 bytes_per_second=41.4517Mi/s items_per_second=55.5111/s
bm_ryml_yaml_json_ostream       18105030 ns     18079123 ns           85 bytes_per_second=41.3033Mi/s items_per_second=55.3124/s
bm_fyaml_str_reserve            28066251 ns     28026976 ns           23 bytes_per_second=26.6432Mi/s items_per_second=35.6799/s
bm_fyaml_str                    28481163 ns     28446287 ns           24 bytes_per_second=26.2505Mi/s items_per_second=35.154/s
bm_fyaml_ostream                34853002 ns     34808123 ns           19 bytes_per_second=21.4527Mi/s items_per_second=28.7289/s
bm_yamlcpp                     162942614 ns    162730336 ns            4 bytes_per_second=4.58874Mi/s items_per_second=6.14514/s
Built target ryml-bm-emit-style_maps_flow_outer1000_inner100
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_seqs_blck_outer1000_inner10: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner10.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner10.json;/proj/rapidyaml/bm/cases/style_seqs_blck_outer1000_inner10.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner10.json
-----------------------------------
running case: PARSE/style_seqs_blck_outer1000_inner10.yml
file: /proj/rapidyaml/bm/cases/style_seqs_blck_outer1000_inner10.yml
-----------------------------------
2024-05-07T20:45:30+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.69, 1.49, 1.31
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve     621920 ns       617492 ns         1131 bytes_per_second=121.857Mi/s items_per_second=1.61945k/s
bm_ryml_yaml_inplace_reuse_nofilter             622504 ns       621714 ns         1140 bytes_per_second=121.03Mi/s items_per_second=1.60846k/s
bm_ryml_yaml_inplace_reuse_reserve              616422 ns       615699 ns         1136 bytes_per_second=122.212Mi/s items_per_second=1.62417k/s
bm_ryml_yaml_inplace_reuse                      616364 ns       615622 ns         1137 bytes_per_second=122.227Mi/s items_per_second=1.62437k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve       627664 ns       626872 ns         1123 bytes_per_second=120.034Mi/s items_per_second=1.59522k/s
bm_ryml_yaml_arena_reuse_nofilter              2096317 ns      2092246 ns          338 bytes_per_second=35.9642Mi/s items_per_second=477.955/s
bm_ryml_yaml_arena_reuse_reserve                620455 ns       619688 ns         1129 bytes_per_second=121.425Mi/s items_per_second=1.61372k/s
bm_ryml_yaml_arena_reuse                        620102 ns       619305 ns         1129 bytes_per_second=121.5Mi/s items_per_second=1.61471k/s
bm_ryml_yaml_inplace_nofilter_reserve           620300 ns       615775 ns         1140 bytes_per_second=122.197Mi/s items_per_second=1.62397k/s
bm_ryml_yaml_inplace_nofilter                  2058179 ns      2055005 ns          342 bytes_per_second=36.6159Mi/s items_per_second=486.617/s
bm_ryml_yaml_inplace_reserve                    615583 ns       614775 ns         1125 bytes_per_second=122.396Mi/s items_per_second=1.62661k/s
bm_ryml_yaml_inplace                           2053443 ns      2050435 ns          341 bytes_per_second=36.6975Mi/s items_per_second=487.701/s
bm_ryml_yaml_arena_nofilter_reserve             621506 ns       620738 ns         1123 bytes_per_second=121.22Mi/s items_per_second=1.61099k/s
bm_ryml_yaml_arena_nofilter                    2054690 ns      2051485 ns          336 bytes_per_second=36.6787Mi/s items_per_second=487.452/s
bm_ryml_yaml_arena_reserve                      624873 ns       624063 ns         1125 bytes_per_second=120.574Mi/s items_per_second=1.6024k/s
bm_ryml_yaml_arena                             2053322 ns      2049721 ns          340 bytes_per_second=36.7103Mi/s items_per_second=487.871/s
bm_libyaml_arena                               3348945 ns      3343267 ns          209 bytes_per_second=22.5067Mi/s items_per_second=299.109/s
bm_libyaml_arena_reuse                         2092252 ns      2089082 ns          335 bytes_per_second=36.0186Mi/s items_per_second=478.679/s
bm_libfyaml_arena                              3511719 ns      3496378 ns          200 bytes_per_second=21.5211Mi/s items_per_second=286.01/s
bm_yamlcpp_arena                              15802286 ns     15781891 ns           44 bytes_per_second=4.76786Mi/s items_per_second=63.3638/s
Built target ryml-bm-parse-style_seqs_blck_outer1000_inner10
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_seqs_blck_outer1000_inner10: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner10.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner10.json;/proj/rapidyaml/bm/cases/style_seqs_blck_outer1000_inner10.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner10.json
-----------------------------------
running case: EMIT/style_seqs_blck_outer1000_inner10.yml
file: /proj/rapidyaml/bm/cases/style_seqs_blck_outer1000_inner10.yml
-----------------------------------
2024-05-07T20:45:49+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.78, 1.53, 1.33
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve          152416 ns       152232 ns         4536 bytes_per_second=494.283Mi/s items_per_second=6.56891k/s
bm_ryml_yaml_json_str_reserve     297565 ns       297204 ns         2364 bytes_per_second=253.179Mi/s items_per_second=3.36469k/s
bm_ryml_yaml_str                  153478 ns       153297 ns         4571 bytes_per_second=490.849Mi/s items_per_second=6.52327k/s
bm_ryml_yaml_json_str             308222 ns       307639 ns         2355 bytes_per_second=244.591Mi/s items_per_second=3.25056k/s
bm_ryml_yaml_ostream             5684290 ns      5674319 ns          502 bytes_per_second=13.2608Mi/s items_per_second=176.233/s
bm_ryml_yaml_json_ostream        4881222 ns      4869491 ns         1188 bytes_per_second=15.4525Mi/s items_per_second=205.36/s
bm_fyaml_str_reserve             2687251 ns      2683967 ns          262 bytes_per_second=28.0353Mi/s items_per_second=372.583/s
bm_fyaml_str                     2651723 ns      2648655 ns          263 bytes_per_second=28.4091Mi/s items_per_second=377.55/s
bm_fyaml_ostream                 3759484 ns      3752835 ns          211 bytes_per_second=20.0504Mi/s items_per_second=266.465/s
bm_yamlcpp                       7921123 ns      7909357 ns           92 bytes_per_second=9.51352Mi/s items_per_second=126.433/s
Built target ryml-bm-emit-style_seqs_blck_outer1000_inner10
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_seqs_blck_outer1000_inner100: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner100.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner100.json;/proj/rapidyaml/bm/cases/style_seqs_blck_outer1000_inner100.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_blck_outer1000_inner100.json
-----------------------------------
running case: PARSE/style_seqs_blck_outer1000_inner100.yml
file: /proj/rapidyaml/bm/cases/style_seqs_blck_outer1000_inner100.yml
-----------------------------------
2024-05-07T20:46:06+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.76, 1.53, 1.33
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve    5931510 ns      5920724 ns          118 bytes_per_second=127.087Mi/s items_per_second=168.898/s
bm_ryml_yaml_inplace_reuse_nofilter            5938745 ns      5927969 ns          118 bytes_per_second=126.932Mi/s items_per_second=168.692/s
bm_ryml_yaml_inplace_reuse_reserve             5945909 ns      5934906 ns          119 bytes_per_second=126.784Mi/s items_per_second=168.495/s
bm_ryml_yaml_inplace_reuse                     5924915 ns      5913873 ns          118 bytes_per_second=127.235Mi/s items_per_second=169.094/s
bm_ryml_yaml_arena_reuse_nofilter_reserve      6115058 ns      6059679 ns          117 bytes_per_second=124.173Mi/s items_per_second=165.025/s
bm_ryml_yaml_arena_reuse_nofilter             15120757 ns     15082463 ns           46 bytes_per_second=49.8891Mi/s items_per_second=66.3022/s
bm_ryml_yaml_arena_reuse_reserve               6029138 ns      6018200 ns          116 bytes_per_second=125.029Mi/s items_per_second=166.163/s
bm_ryml_yaml_arena_reuse                       6022121 ns      6011168 ns          117 bytes_per_second=125.175Mi/s items_per_second=166.357/s
bm_ryml_yaml_inplace_nofilter_reserve          5935478 ns      5925015 ns          118 bytes_per_second=126.995Mi/s items_per_second=168.776/s
bm_ryml_yaml_inplace_nofilter                  8947928 ns      8927168 ns           78 bytes_per_second=84.2876Mi/s items_per_second=112.018/s
bm_ryml_yaml_inplace_reserve                   5920113 ns      5909319 ns          119 bytes_per_second=127.333Mi/s items_per_second=169.224/s
bm_ryml_yaml_inplace                           8943815 ns      8922735 ns           78 bytes_per_second=84.3295Mi/s items_per_second=112.073/s
bm_ryml_yaml_arena_nofilter_reserve            6014915 ns      6004027 ns          117 bytes_per_second=125.324Mi/s items_per_second=166.555/s
bm_ryml_yaml_arena_nofilter                    9088027 ns      9066450 ns           78 bytes_per_second=82.9928Mi/s items_per_second=110.297/s
bm_ryml_yaml_arena_reserve                     6013400 ns      6002478 ns          116 bytes_per_second=125.357Mi/s items_per_second=166.598/s
bm_ryml_yaml_arena                             9127032 ns      9051190 ns           77 bytes_per_second=83.1327Mi/s items_per_second=110.483/s
bm_libyaml_arena                              20916142 ns     20885531 ns           33 bytes_per_second=36.0273Mi/s items_per_second=47.88/s
bm_libyaml_arena_reuse                        20212884 ns     20171884 ns           35 bytes_per_second=37.3019Mi/s items_per_second=49.574/s
bm_libfyaml_arena                             36274895 ns     36222558 ns           19 bytes_per_second=20.773Mi/s items_per_second=27.6071/s
bm_yamlcpp_arena                             178971137 ns    178701592 ns            4 bytes_per_second=4.21065Mi/s items_per_second=5.59592/s
Built target ryml-bm-parse-style_seqs_blck_outer1000_inner100
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_seqs_blck_outer1000_inner100: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner100.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner100.json;/proj/rapidyaml/bm/cases/style_seqs_blck_outer1000_inner100.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_blck_outer1000_inner100.json
-----------------------------------
running case: EMIT/style_seqs_blck_outer1000_inner100.yml
file: /proj/rapidyaml/bm/cases/style_seqs_blck_outer1000_inner100.yml
-----------------------------------
2024-05-07T20:46:25+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.54, 1.50, 1.32
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve         1408532 ns      1405563 ns          497 bytes_per_second=535.337Mi/s items_per_second=711.458/s
bm_ryml_yaml_json_str_reserve    2916692 ns      2911788 ns          241 bytes_per_second=258.415Mi/s items_per_second=343.432/s
bm_ryml_yaml_str                 1451981 ns      1432352 ns          499 bytes_per_second=525.325Mi/s items_per_second=698.152/s
bm_ryml_yaml_json_str            2921590 ns      2916513 ns          239 bytes_per_second=257.996Mi/s items_per_second=342.875/s
bm_ryml_yaml_ostream            18843857 ns     18815188 ns          136 bytes_per_second=39.9916Mi/s items_per_second=53.1486/s
bm_ryml_yaml_json_ostream        7223020 ns      7210897 ns          100 bytes_per_second=104.349Mi/s items_per_second=138.679/s
bm_fyaml_str_reserve            27423845 ns     27389694 ns           25 bytes_per_second=27.472Mi/s items_per_second=36.5101/s
bm_fyaml_str                    27418261 ns     27384545 ns           25 bytes_per_second=27.4772Mi/s items_per_second=36.5169/s
bm_fyaml_ostream                31866766 ns     31823832 ns           22 bytes_per_second=23.6442Mi/s items_per_second=31.423/s
bm_yamlcpp                      80328484 ns     80220335 ns            8 bytes_per_second=9.37979Mi/s items_per_second=12.4657/s
Built target ryml-bm-emit-style_seqs_blck_outer1000_inner100
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_seqs_flow_outer1000_inner10: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10.json;/proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner10.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10.json
-----------------------------------
running case: PARSE/style_seqs_flow_outer1000_inner10.yml
file: /proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner10.yml
-----------------------------------
2024-05-07T20:46:37+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.66, 1.52, 1.33
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve     361501 ns       359980 ns         1959 bytes_per_second=113.655Mi/s items_per_second=2.77793k/s
bm_ryml_yaml_inplace_reuse_nofilter             359123 ns       358694 ns         1946 bytes_per_second=114.063Mi/s items_per_second=2.78789k/s
bm_ryml_yaml_inplace_reuse_reserve              357860 ns       357332 ns         1953 bytes_per_second=114.497Mi/s items_per_second=2.79852k/s
bm_ryml_yaml_inplace_reuse                      359460 ns       358996 ns         1948 bytes_per_second=113.967Mi/s items_per_second=2.78555k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve       366455 ns       363761 ns         1946 bytes_per_second=112.474Mi/s items_per_second=2.74906k/s
bm_ryml_yaml_arena_reuse_nofilter              1781376 ns      1777759 ns          391 bytes_per_second=23.0141Mi/s items_per_second=562.506/s
bm_ryml_yaml_arena_reuse_reserve                363431 ns       363030 ns         1949 bytes_per_second=112.7Mi/s items_per_second=2.75459k/s
bm_ryml_yaml_arena_reuse                        360973 ns       360602 ns         1943 bytes_per_second=113.459Mi/s items_per_second=2.77314k/s
bm_ryml_yaml_inplace_nofilter_reserve           359058 ns       358633 ns         1964 bytes_per_second=114.082Mi/s items_per_second=2.78836k/s
bm_ryml_yaml_inplace_nofilter                  1733528 ns      1730917 ns          408 bytes_per_second=23.6369Mi/s items_per_second=577.729/s
bm_ryml_yaml_inplace_reserve                    357371 ns       356951 ns         1958 bytes_per_second=114.62Mi/s items_per_second=2.8015k/s
bm_ryml_yaml_inplace                           1716851 ns      1714365 ns          407 bytes_per_second=23.8651Mi/s items_per_second=583.306/s
bm_ryml_yaml_arena_nofilter_reserve             359681 ns       359273 ns         1948 bytes_per_second=113.879Mi/s items_per_second=2.7834k/s
bm_ryml_yaml_arena_nofilter                    1724547 ns      1721935 ns          408 bytes_per_second=23.7602Mi/s items_per_second=580.742/s
bm_ryml_yaml_arena_reserve                      360699 ns       360272 ns         1948 bytes_per_second=113.563Mi/s items_per_second=2.77568k/s
bm_ryml_yaml_arena                             1718131 ns      1715319 ns          407 bytes_per_second=23.8519Mi/s items_per_second=582.982/s
bm_libyaml_arena                               3241938 ns      3236919 ns          216 bytes_per_second=12.6397Mi/s items_per_second=308.936/s
bm_libyaml_arena_reuse                         2052475 ns      2050288 ns          343 bytes_per_second=19.955Mi/s items_per_second=487.736/s
bm_libfyaml_arena                              2921482 ns      2917777 ns          240 bytes_per_second=14.0222Mi/s items_per_second=342.727/s
bm_yamlcpp_arena                              12543483 ns     12528396 ns           56 bytes_per_second=3.26567Mi/s items_per_second=79.8187/s
Built target ryml-bm-parse-style_seqs_flow_outer1000_inner10
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_seqs_flow_outer1000_inner10: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10.json;/proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner10.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10.json
-----------------------------------
running case: EMIT/style_seqs_flow_outer1000_inner10.yml
file: /proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner10.yml
-----------------------------------
2024-05-07T20:47:03+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.64, 1.53, 1.34
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve          137638 ns       137461 ns         5096 bytes_per_second=297.637Mi/s items_per_second=7.27478k/s
bm_ryml_yaml_json_str_reserve     297584 ns       297273 ns         2356 bytes_per_second=137.629Mi/s items_per_second=3.36391k/s
bm_ryml_yaml_str                  137917 ns       137766 ns         5113 bytes_per_second=296.978Mi/s items_per_second=7.25866k/s
bm_ryml_yaml_json_str             297856 ns       297544 ns         2350 bytes_per_second=137.504Mi/s items_per_second=3.36084k/s
bm_ryml_yaml_ostream             5252007 ns      5243524 ns          920 bytes_per_second=7.80269Mi/s items_per_second=190.711/s
bm_ryml_yaml_json_ostream        4379982 ns      4372503 ns         1149 bytes_per_second=9.35702Mi/s items_per_second=228.702/s
bm_fyaml_str_reserve             1182907 ns      1181669 ns          586 bytes_per_second=34.6236Mi/s items_per_second=846.261/s
bm_fyaml_str                     1182544 ns      1181332 ns          583 bytes_per_second=34.6334Mi/s items_per_second=846.502/s
bm_fyaml_ostream                 3224091 ns      3218188 ns          358 bytes_per_second=12.7132Mi/s items_per_second=310.734/s
bm_yamlcpp                       8111950 ns      8100928 ns           89 bytes_per_second=5.05048Mi/s items_per_second=123.443/s
Built target ryml-bm-emit-style_seqs_flow_outer1000_inner10
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_seqs_flow_outer1000_inner10_json: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10_json.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10_json.json;/proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner10_json.json;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10_json.json
-----------------------------------
running case: PARSE/style_seqs_flow_outer1000_inner10_json.json
file: /proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner10_json.json
-----------------------------------
2024-05-07T20:47:21+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.50, 1.50, 1.34
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_json_inplace_reuse_nofilter_reserve     308231 ns       307872 ns         2213 bytes_per_second=132.898Mi/s items_per_second=3.24811k/s
bm_ryml_json_inplace_reuse_nofilter             315760 ns       315434 ns         2246 bytes_per_second=129.712Mi/s items_per_second=3.17024k/s
bm_ryml_json_inplace_reuse_reserve              309540 ns       309170 ns         2190 bytes_per_second=132.34Mi/s items_per_second=3.23447k/s
bm_ryml_json_inplace_reuse                      307555 ns       307207 ns         2287 bytes_per_second=133.185Mi/s items_per_second=3.25513k/s
bm_ryml_yaml_inplace_reuse_nofilter_reserve     361299 ns       360770 ns         2003 bytes_per_second=113.411Mi/s items_per_second=2.77185k/s
bm_ryml_yaml_inplace_reuse_nofilter             351130 ns       350629 ns         1958 bytes_per_second=116.692Mi/s items_per_second=2.85201k/s
bm_ryml_yaml_inplace_reuse_reserve              346344 ns       345938 ns         2025 bytes_per_second=118.274Mi/s items_per_second=2.89069k/s
bm_ryml_yaml_inplace_reuse                      344877 ns       344437 ns         1993 bytes_per_second=118.789Mi/s items_per_second=2.90329k/s
bm_ryml_json_arena_reuse_nofilter_reserve       311415 ns       309921 ns         2261 bytes_per_second=132.019Mi/s items_per_second=3.22662k/s
bm_ryml_json_arena_reuse_nofilter               309225 ns       308898 ns         2266 bytes_per_second=132.456Mi/s items_per_second=3.23732k/s
bm_ryml_json_arena_reuse_reserve                309510 ns       309147 ns         2263 bytes_per_second=132.35Mi/s items_per_second=3.23471k/s
bm_ryml_json_arena_reuse                        311668 ns       311331 ns         2270 bytes_per_second=131.421Mi/s items_per_second=3.21201k/s
bm_ryml_yaml_arena_reuse_nofilter_reserve       347103 ns       346683 ns         2019 bytes_per_second=118.02Mi/s items_per_second=2.88448k/s
bm_ryml_yaml_arena_reuse_nofilter              1838207 ns      1834313 ns          391 bytes_per_second=22.3056Mi/s items_per_second=545.163/s
bm_ryml_yaml_arena_reuse_reserve                347207 ns       346740 ns         2022 bytes_per_second=118.001Mi/s items_per_second=2.88401k/s
bm_ryml_yaml_arena_reuse                        351827 ns       349797 ns         2014 bytes_per_second=116.969Mi/s items_per_second=2.8588k/s
bm_ryml_json_inplace_nofilter_reserve           308275 ns       307880 ns         2292 bytes_per_second=132.894Mi/s items_per_second=3.24802k/s
bm_ryml_json_inplace_nofilter                   620566 ns       620089 ns         1128 bytes_per_second=65.9833Mi/s items_per_second=1.61267k/s
bm_ryml_json_inplace_reserve                    307479 ns       307138 ns         2277 bytes_per_second=133.215Mi/s items_per_second=3.25586k/s
bm_ryml_json_inplace                            615919 ns       615374 ns         1133 bytes_per_second=66.4888Mi/s items_per_second=1.62503k/s
bm_ryml_yaml_inplace_nofilter_reserve           348014 ns       347559 ns         2028 bytes_per_second=117.722Mi/s items_per_second=2.87721k/s
bm_ryml_yaml_inplace_nofilter                   655133 ns       654496 ns         1063 bytes_per_second=62.5145Mi/s items_per_second=1.52789k/s
bm_ryml_yaml_inplace_reserve                    343853 ns       343419 ns         2037 bytes_per_second=119.142Mi/s items_per_second=2.91189k/s
bm_ryml_yaml_inplace                            665139 ns       664298 ns         1074 bytes_per_second=61.5921Mi/s items_per_second=1.50535k/s
bm_ryml_json_arena_nofilter_reserve             312202 ns       311822 ns         2185 bytes_per_second=131.214Mi/s items_per_second=3.20696k/s
bm_ryml_json_arena_nofilter                     629719 ns       624494 ns         1124 bytes_per_second=65.5179Mi/s items_per_second=1.6013k/s
bm_ryml_json_arena_reserve                      310753 ns       310412 ns         2253 bytes_per_second=131.81Mi/s items_per_second=3.22152k/s
bm_ryml_json_arena                              623458 ns       622737 ns         1119 bytes_per_second=65.7026Mi/s items_per_second=1.60581k/s
bm_ryml_yaml_arena_nofilter_reserve             350282 ns       349811 ns         2020 bytes_per_second=116.964Mi/s items_per_second=2.85868k/s
bm_ryml_yaml_arena_nofilter                     660342 ns       659709 ns         1073 bytes_per_second=62.0205Mi/s items_per_second=1.51582k/s
bm_ryml_yaml_arena_reserve                      348541 ns       348130 ns         2016 bytes_per_second=117.529Mi/s items_per_second=2.87249k/s
bm_ryml_yaml_arena                              652899 ns       652109 ns         1076 bytes_per_second=62.7433Mi/s items_per_second=1.53349k/s
bm_libyaml_arena                               2208645 ns      2206186 ns          316 bytes_per_second=18.5458Mi/s items_per_second=453.271/s
bm_libyaml_arena_reuse                         2075397 ns      2073114 ns          338 bytes_per_second=19.7362Mi/s items_per_second=482.366/s
bm_libfyaml_arena                              2891402 ns      2887900 ns          242 bytes_per_second=14.1679Mi/s items_per_second=346.272/s
bm_yamlcpp_arena                              13225852 ns     13209882 ns           53 bytes_per_second=3.09734Mi/s items_per_second=75.7009/s
bm_rapidjson_arena                              117065 ns       116979 ns         5995 bytes_per_second=349.768Mi/s items_per_second=8.54854k/s
bm_rapidjson_inplace                            134558 ns       134471 ns         5208 bytes_per_second=304.27Mi/s items_per_second=7.43654k/s
bm_sajson_arena                                 200201 ns       200049 ns         3495 bytes_per_second=204.527Mi/s items_per_second=4.99877k/s
bm_sajson_inplace                               201920 ns       201786 ns         3464 bytes_per_second=202.767Mi/s items_per_second=4.95574k/s
bm_jsoncpp_arena                               1135972 ns      1134830 ns          622 bytes_per_second=36.0543Mi/s items_per_second=881.189/s
bm_nlohmann_arena                               830410 ns       829558 ns          860 bytes_per_second=49.3221Mi/s items_per_second=1.20546k/s
Built target ryml-bm-parse-style_seqs_flow_outer1000_inner10_json
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_seqs_flow_outer1000_inner10_json: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10_json.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10_json.json;/proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner10_json.json;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10_json.json
-----------------------------------
running case: EMIT/style_seqs_flow_outer1000_inner10_json.json
file: /proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner10_json.json
-----------------------------------
2024-05-07T20:48:14+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.49, 1.51, 1.35
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve          139299 ns       139117 ns         5031 bytes_per_second=294.108Mi/s items_per_second=7.18819k/s
bm_ryml_yaml_json_str_reserve     299486 ns       299065 ns         2347 bytes_per_second=136.811Mi/s items_per_second=3.34375k/s
bm_ryml_yaml_str                  138218 ns       138049 ns         5042 bytes_per_second=296.383Mi/s items_per_second=7.2438k/s
bm_ryml_yaml_json_str             297559 ns       297211 ns         2288 bytes_per_second=137.665Mi/s items_per_second=3.36461k/s
bm_ryml_yaml_ostream             5662188 ns      5651852 ns          989 bytes_per_second=7.23931Mi/s items_per_second=176.933/s
bm_ryml_yaml_json_ostream        4491051 ns      4482876 ns         1139 bytes_per_second=9.12706Mi/s items_per_second=223.071/s
bm_fyaml_str_reserve             1202097 ns      1200638 ns          595 bytes_per_second=34.0781Mi/s items_per_second=832.891/s
bm_fyaml_str                     1182572 ns      1181134 ns          592 bytes_per_second=34.6408Mi/s items_per_second=846.644/s
bm_fyaml_ostream                 3262743 ns      3256017 ns          356 bytes_per_second=12.5661Mi/s items_per_second=307.124/s
bm_yamlcpp                       8559616 ns      8546745 ns           79 bytes_per_second=4.78726Mi/s items_per_second=117.004/s
bm_rapidjson                       98244 ns        98131 ns         7281 bytes_per_second=416.949Mi/s items_per_second=10.1905k/s
bm_jsoncpp                       3490239 ns      3483118 ns          511 bytes_per_second=11.7468Mi/s items_per_second=287.099/s
bm_nlohmann                      3094224 ns      3088662 ns         1000 bytes_per_second=13.247Mi/s items_per_second=323.765/s
Built target ryml-bm-emit-style_seqs_flow_outer1000_inner10_json
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_seqs_flow_outer1000_inner100: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100.json;/proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner100.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100.json
-----------------------------------
running case: PARSE/style_seqs_flow_outer1000_inner100.yml
file: /proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner100.yml
-----------------------------------
2024-05-07T20:48:39+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.46, 1.50, 1.35
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_yaml_inplace_reuse_nofilter_reserve    3236786 ns      3229630 ns          216 bytes_per_second=116.049Mi/s items_per_second=309.633/s
bm_ryml_yaml_inplace_reuse_nofilter            3240468 ns      3233239 ns          217 bytes_per_second=115.919Mi/s items_per_second=309.287/s
bm_ryml_yaml_inplace_reuse_reserve             3261980 ns      3253854 ns          216 bytes_per_second=115.185Mi/s items_per_second=307.328/s
bm_ryml_yaml_inplace_reuse                     3263429 ns      3256002 ns          216 bytes_per_second=115.109Mi/s items_per_second=307.125/s
bm_ryml_yaml_arena_reuse_nofilter_reserve      3312722 ns      3305303 ns          211 bytes_per_second=113.392Mi/s items_per_second=302.544/s
bm_ryml_yaml_arena_reuse_nofilter             12498763 ns     12465270 ns           56 bytes_per_second=30.0671Mi/s items_per_second=80.2229/s
bm_ryml_yaml_arena_reuse_reserve               3291088 ns      3283797 ns          213 bytes_per_second=114.135Mi/s items_per_second=304.526/s
bm_ryml_yaml_arena_reuse                       3268741 ns      3261775 ns          214 bytes_per_second=114.905Mi/s items_per_second=306.582/s
bm_ryml_yaml_inplace_nofilter_reserve          3249878 ns      3242767 ns          217 bytes_per_second=115.579Mi/s items_per_second=308.379/s
bm_ryml_yaml_inplace_nofilter                  6108392 ns      6092846 ns          115 bytes_per_second=61.5139Mi/s items_per_second=164.127/s
bm_ryml_yaml_inplace_reserve                   3253523 ns      3246326 ns          217 bytes_per_second=115.452Mi/s items_per_second=308.041/s
bm_ryml_yaml_inplace                           6123293 ns      6107845 ns           94 bytes_per_second=61.3629Mi/s items_per_second=163.724/s
bm_ryml_yaml_arena_nofilter_reserve            3280005 ns      3272677 ns          214 bytes_per_second=114.522Mi/s items_per_second=305.56/s
bm_ryml_yaml_arena_nofilter                    6148717 ns      6133072 ns          114 bytes_per_second=61.1105Mi/s items_per_second=163.05/s
bm_ryml_yaml_arena_reserve                     3274588 ns      3267418 ns          214 bytes_per_second=114.707Mi/s items_per_second=306.052/s
bm_ryml_yaml_arena                             6135996 ns      6119135 ns          114 bytes_per_second=61.2497Mi/s items_per_second=163.422/s
bm_libyaml_arena                              20149265 ns     20121957 ns           35 bytes_per_second=18.6262Mi/s items_per_second=49.697/s
bm_libyaml_arena_reuse                        19077953 ns     19046432 ns           37 bytes_per_second=19.678Mi/s items_per_second=52.5033/s
bm_libfyaml_arena                             27248408 ns     27213567 ns           26 bytes_per_second=13.7724Mi/s items_per_second=36.7464/s
bm_yamlcpp_arena                             146621726 ns    146435214 ns            5 bytes_per_second=2.55946Mi/s items_per_second=6.82896/s
Built target ryml-bm-parse-style_seqs_flow_outer1000_inner100
ryml: running benchmark ryml-bm-emit, case ryml-bm-emit-style_seqs_flow_outer1000_inner100: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-emit-0.6.0 --benchmark_filter=ryml_yaml|yaml;--benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100.json;/proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner100.yml;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100.json
-----------------------------------
running case: EMIT/style_seqs_flow_outer1000_inner100.yml
file: /proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner100.yml
-----------------------------------
2024-05-07T20:49:04+01:00
Running ../bin/ryml-bm-emit-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.74, 1.57, 1.38
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
bm_ryml_yaml_str_reserve         1250625 ns      1248495 ns          561 bytes_per_second=300.197Mi/s items_per_second=800.964/s
bm_ryml_yaml_json_str_reserve    2920564 ns      2916193 ns          240 bytes_per_second=128.522Mi/s items_per_second=342.913/s
bm_ryml_yaml_str                 1250538 ns      1248331 ns          559 bytes_per_second=300.237Mi/s items_per_second=801.07/s
bm_ryml_yaml_json_str            2933571 ns      2929303 ns          238 bytes_per_second=127.947Mi/s items_per_second=341.378/s
bm_ryml_yaml_ostream             7368493 ns      7357321 ns          100 bytes_per_second=50.9418Mi/s items_per_second=135.919/s
bm_ryml_yaml_json_ostream        7075602 ns      7065361 ns          100 bytes_per_second=53.0468Mi/s items_per_second=141.536/s
bm_fyaml_str_reserve            11396453 ns     11384357 ns           53 bytes_per_second=32.9219Mi/s items_per_second=87.8398/s
bm_fyaml_str                    11754352 ns     11741418 ns           55 bytes_per_second=31.9208Mi/s items_per_second=85.1686/s
bm_fyaml_ostream                17366116 ns     17345378 ns           42 bytes_per_second=21.6078Mi/s items_per_second=57.6522/s
bm_yamlcpp                      88404276 ns     88293730 ns            8 bytes_per_second=4.24486Mi/s items_per_second=11.3258/s
Built target ryml-bm-emit-style_seqs_flow_outer1000_inner100
ryml: running benchmark ryml-bm-parse, case ryml-bm-parse-style_seqs_flow_outer1000_inner100_json: saving results in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100_json.json
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bin/ryml-bm-parse-0.6.0 --benchmark_out_format=json;--benchmark_out=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100_json.json;/proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner100_json.json;OUTPUT_FILE;/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100_json.json
-----------------------------------
running case: PARSE/style_seqs_flow_outer1000_inner100_json.json
file: /proj/rapidyaml/bm/cases/style_seqs_flow_outer1000_inner100_json.json
-----------------------------------
2024-05-07T20:49:14+01:00
Running ../bin/ryml-bm-parse-0.6.0
Run on (20 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 19712 KiB (x1)
Load Average: 1.71, 1.57, 1.38
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------------
bm_ryml_json_inplace_reuse_nofilter_reserve    2955806 ns      2949156 ns          238 bytes_per_second=127.086Mi/s items_per_second=339.08/s
bm_ryml_json_inplace_reuse_nofilter            2948238 ns      2941663 ns          237 bytes_per_second=127.41Mi/s items_per_second=339.944/s
bm_ryml_json_inplace_reuse_reserve             2950828 ns      2944239 ns          238 bytes_per_second=127.298Mi/s items_per_second=339.646/s
bm_ryml_json_inplace_reuse                     2955406 ns      2948938 ns          236 bytes_per_second=127.096Mi/s items_per_second=339.105/s
bm_ryml_yaml_inplace_reuse_nofilter_reserve    3195379 ns      3188618 ns          220 bytes_per_second=117.542Mi/s items_per_second=313.616/s
bm_ryml_yaml_inplace_reuse_nofilter            3190867 ns      3184130 ns          220 bytes_per_second=117.708Mi/s items_per_second=314.058/s
bm_ryml_yaml_inplace_reuse_reserve             3201044 ns      3194254 ns          220 bytes_per_second=117.335Mi/s items_per_second=313.062/s
bm_ryml_yaml_inplace_reuse                     3200807 ns      3194103 ns          219 bytes_per_second=117.34Mi/s items_per_second=313.077/s
bm_ryml_json_arena_reuse_nofilter_reserve      2999912 ns      2993065 ns          228 bytes_per_second=125.222Mi/s items_per_second=334.106/s
bm_ryml_json_arena_reuse_nofilter              3039383 ns      3032439 ns          234 bytes_per_second=123.596Mi/s items_per_second=329.768/s
bm_ryml_json_arena_reuse_reserve               3063382 ns      3055373 ns          234 bytes_per_second=122.668Mi/s items_per_second=327.292/s
bm_ryml_json_arena_reuse                       3003806 ns      2997062 ns          234 bytes_per_second=125.055Mi/s items_per_second=333.66/s
bm_ryml_yaml_arena_reuse_nofilter_reserve      3241736 ns      3234475 ns          217 bytes_per_second=115.876Mi/s items_per_second=309.169/s
bm_ryml_yaml_arena_reuse_nofilter             12069503 ns     12029993 ns           58 bytes_per_second=31.1552Mi/s items_per_second=83.1256/s
bm_ryml_yaml_arena_reuse_reserve               3286018 ns      3278417 ns          211 bytes_per_second=114.323Mi/s items_per_second=305.025/s
bm_ryml_yaml_arena_reuse                       3255393 ns      3247868 ns          216 bytes_per_second=115.398Mi/s items_per_second=307.894/s
bm_ryml_json_inplace_nofilter_reserve          2972187 ns      2964787 ns          235 bytes_per_second=126.416Mi/s items_per_second=337.292/s
bm_ryml_json_inplace_nofilter                  5926879 ns      5909524 ns          118 bytes_per_second=63.4225Mi/s items_per_second=169.218/s
bm_ryml_json_inplace_reserve                   3002431 ns      2995229 ns          235 bytes_per_second=125.131Mi/s items_per_second=333.864/s
bm_ryml_json_inplace                           5950000 ns      5932204 ns          118 bytes_per_second=63.18Mi/s items_per_second=168.571/s
bm_ryml_yaml_inplace_nofilter_reserve          3286335 ns      3277923 ns          218 bytes_per_second=114.34Mi/s items_per_second=305.071/s
bm_ryml_yaml_inplace_nofilter                  6171726 ns      6155062 ns          114 bytes_per_second=60.8925Mi/s items_per_second=162.468/s
bm_ryml_yaml_inplace_reserve                   3215069 ns      3207603 ns          219 bytes_per_second=116.846Mi/s items_per_second=311.759/s
bm_ryml_yaml_inplace                           6185553 ns      6168114 ns          114 bytes_per_second=60.7636Mi/s items_per_second=162.124/s
bm_ryml_json_arena_nofilter_reserve            3036177 ns      3028268 ns          233 bytes_per_second=123.766Mi/s items_per_second=330.222/s
bm_ryml_json_arena_nofilter                    6072495 ns      6055685 ns          117 bytes_per_second=61.8917Mi/s items_per_second=165.134/s
bm_ryml_json_arena_reserve                     3012587 ns      3005883 ns          232 bytes_per_second=124.688Mi/s items_per_second=332.681/s
bm_ryml_json_arena                             5905879 ns      5889326 ns          120 bytes_per_second=63.64Mi/s items_per_second=169.799/s
bm_ryml_yaml_arena_nofilter_reserve            3249801 ns      3242535 ns          209 bytes_per_second=115.588Mi/s items_per_second=308.401/s
bm_ryml_yaml_arena_nofilter                    6112467 ns      6096427 ns          114 bytes_per_second=61.4781Mi/s items_per_second=164.03/s
bm_ryml_yaml_arena_reserve                     3247098 ns      3239982 ns          216 bytes_per_second=115.679Mi/s items_per_second=308.644/s
bm_ryml_yaml_arena                             6106376 ns      6089324 ns          114 bytes_per_second=61.5498Mi/s items_per_second=164.222/s
bm_libyaml_arena                              20525118 ns     20497298 ns           34 bytes_per_second=18.2852Mi/s items_per_second=48.7869/s
bm_libyaml_arena_reuse                        19475156 ns     19442485 ns           36 bytes_per_second=19.2772Mi/s items_per_second=51.4338/s
bm_libfyaml_arena                             27260910 ns     27224896 ns           26 bytes_per_second=13.7667Mi/s items_per_second=36.7311/s
bm_yamlcpp_arena                             150424632 ns    150244008 ns            5 bytes_per_second=2.49459Mi/s items_per_second=6.65584/s
bm_rapidjson_arena                              933567 ns       932484 ns          754 bytes_per_second=401.934Mi/s items_per_second=1.0724k/s
bm_rapidjson_inplace                           1135908 ns      1134816 ns          615 bytes_per_second=330.271Mi/s items_per_second=881.2/s
bm_sajson_arena                                1861535 ns      1858883 ns          384 bytes_per_second=201.625Mi/s items_per_second=537.957/s
bm_sajson_inplace                              1816893 ns      1815089 ns          385 bytes_per_second=206.489Mi/s items_per_second=550.937/s
bm_jsoncpp_arena                              14202053 ns     14186928 ns           49 bytes_per_second=26.4185Mi/s items_per_second=70.4874/s
bm_nlohmann_arena                              6479085 ns      6472119 ns          108 bytes_per_second=57.9095Mi/s items_per_second=154.509/s
Built target ryml-bm-parse-style_seqs_flow_outer1000_inner100_json
CWD=/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm
Built target ryml-bm-run
Returning to directory /proj/rapidyaml (currently in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release)
Build: finished building (17m 22s): linux-x86_64-gxx13.2-Release
-----------------------------------------------

Compilation finished at Tue May  7 20:50:07
-*- mode: compilation; default-directory: "~/proj/rapidyaml/" -*-
Compilation started at Tue May  7 20:50:13

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
style_seqs_flow_outer1000_inner100_json
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100_json.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100_json.json
+--------------------------------------------------------------------------------------------------------+
|                        emit benchmark: style_seqs_flow_outer1000_inner100_json                         |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |   MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  293.06 |  66.11x |  6510.63% |          1.34 |       0.02x |     -98.49% |
| ryml_yaml_json_str_reserve |  129.23 |  29.15x |  2814.96% |          3.04 |       0.03x |     -96.57% |
| ryml_yaml_str              |  300.44 |  67.77x |  6676.94% |          1.31 |       0.01x |     -98.52% |
| ryml_yaml_json_str         |  126.96 |  28.64x |  2763.80% |          3.10 |       0.03x |     -96.51% |
| ryml_yaml_ostream          |   50.30 |  11.35x |  1034.56% |          7.81 |       0.09x |     -91.19% |
| ryml_yaml_json_ostream     |   53.68 |  12.11x |  1110.96% |          7.32 |       0.08x |     -91.74% |
| fyaml_str_reserve          |   33.64 |   7.59x |   658.93% |         11.68 |       0.13x |     -86.82% |
| fyaml_str                  |   34.41 |   7.76x |   676.08% |         11.42 |       0.13x |     -87.11% |
| fyaml_ostream              |   22.32 |   5.03x |   403.47% |         17.61 |       0.20x |     -80.14% |
| yamlcpp_                   |    4.43 |   1.00x |     0.00% |         88.65 |       1.00x |       0.00% |
| rapidjson_                 |  450.30 | 101.57x | 10057.32% |          0.87 |       0.01x |     -99.02% |
| jsoncpp_                   |   27.80 |   6.27x |   527.05% |         14.14 |       0.16x |     -84.05% |
| nlohmann_                  |   49.71 |  11.21x |  1021.20% |          7.91 |       0.09x |     -91.08% |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100_json-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100_json-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100_json-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100_json-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100_json-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100_json-cpu_time_ms.html
Built target ryml-bm-emit-style_seqs_flow_outer1000_inner100_json-plot
/usr/bin/python3.11
Python 3.11.8
parse
appveyor
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-appveyor.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-appveyor.json
+---------------------------------------------------------------------------------------------------------------------+
|                                              parse benchmark: appveyor                                              |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  251.46 |  26.87x | 2586.79% |          0.01 |       0.04x |     -96.28% |
| ryml_yaml_inplace_reuse_nofilter         |  253.84 |  27.12x | 2612.21% |          0.01 |       0.04x |     -96.31% |
| ryml_yaml_inplace_reuse_reserve          |  244.76 |  26.15x | 2515.13% |          0.01 |       0.04x |     -96.18% |
| ryml_yaml_inplace_reuse                  |  243.29 |  25.99x | 2499.44% |          0.01 |       0.04x |     -96.15% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  252.38 |  26.97x | 2596.58% |          0.01 |       0.04x |     -96.29% |
| ryml_yaml_arena_reuse_nofilter           |  216.69 |  23.15x | 2215.22% |          0.01 |       0.04x |     -95.68% |
| ryml_yaml_arena_reuse_reserve            |  235.73 |  25.19x | 2418.70% |          0.01 |       0.04x |     -96.03% |
| ryml_yaml_arena_reuse                    |  239.04 |  25.54x | 2454.08% |          0.01 |       0.04x |     -96.08% |
| ryml_yaml_inplace_nofilter_reserve       |  243.87 |  26.06x | 2505.66% |          0.01 |       0.04x |     -96.16% |
| ryml_yaml_inplace_nofilter               |  223.62 |  23.89x | 2289.31% |          0.01 |       0.04x |     -95.81% |
| ryml_yaml_inplace_reserve                |  240.44 |  25.69x | 2469.01% |          0.01 |       0.04x |     -96.11% |
| ryml_yaml_inplace                        |  206.57 |  22.07x | 2107.15% |          0.01 |       0.05x |     -95.47% |
| ryml_yaml_arena_nofilter_reserve         |  250.46 |  26.76x | 2576.11% |          0.01 |       0.04x |     -96.26% |
| ryml_yaml_arena_nofilter                 |  218.24 |  23.32x | 2231.84% |          0.01 |       0.04x |     -95.71% |
| ryml_yaml_arena_reserve                  |  236.56 |  25.28x | 2427.56% |          0.01 |       0.04x |     -96.04% |
| ryml_yaml_arena                          |  217.58 |  23.25x | 2224.76% |          0.01 |       0.04x |     -95.70% |
| libyaml_arena                            |   65.28 |   6.97x |  597.49% |          0.03 |       0.14x |     -85.66% |
| libyaml_arena_reuse                      |   66.93 |   7.15x |  615.08% |          0.03 |       0.14x |     -86.02% |
| libfyaml_arena                           |   43.59 |   4.66x |  365.77% |          0.05 |       0.21x |     -78.53% |
| yamlcpp_arena                            |    9.36 |   1.00x |    0.00% |          0.23 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+--------------------------------------------------------------------------------------------------------+
|                                        emit benchmark: appveyor                                        |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |   MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      | 1067.91 | 181.40x | 18040.04% |          0.00 |       0.01x |     -99.45% |
| ryml_yaml_json_str_reserve |  530.01 |  90.03x |  8903.01% |          0.00 |       0.01x |     -98.89% |
| ryml_yaml_str              | 1088.55 | 184.91x | 18390.62% |          0.00 |       0.01x |     -99.46% |
| ryml_yaml_json_str         |  530.65 |  90.14x |  8913.84% |          0.00 |       0.01x |     -98.89% |
| ryml_yaml_ostream          |    3.81 |   0.65x |   -35.26% |          0.56 |       1.54x |      54.47% |
| ryml_yaml_json_ostream     |    0.93 |   0.16x |   -84.25% |          2.30 |       6.35x |     534.90% |
| fyaml_str_reserve          |   72.42 |  12.30x |  1130.23% |          0.03 |       0.08x |     -91.87% |
| fyaml_str                  |   74.67 |  12.68x |  1168.43% |          0.03 |       0.08x |     -92.12% |
| fyaml_ostream              |    3.70 |   0.63x |   -37.07% |          0.57 |       1.59x |      58.90% |
| yamlcpp_                   |    5.89 |   1.00x |     0.00% |          0.36 |       1.00x |       0.00% |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+ 


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
+---------------------------------------------------------------------------------------------------------------------+
|                                          parse benchmark: compile_commands                                          |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_json_inplace_reuse_nofilter_reserve |  639.47 |  34.34x | 3334.33% |          0.07 |       0.03x |     -97.09% |
| ryml_json_inplace_reuse_nofilter         |  641.66 |  34.46x | 3346.06% |          0.07 |       0.03x |     -97.10% |
| ryml_json_inplace_reuse_reserve          |  639.72 |  34.36x | 3335.67% |          0.07 |       0.03x |     -97.09% |
| ryml_json_inplace_reuse                  |  639.50 |  34.34x | 3334.50% |          0.07 |       0.03x |     -97.09% |
| ryml_yaml_inplace_reuse_nofilter_reserve |  631.69 |  33.93x | 3292.55% |          0.07 |       0.03x |     -97.05% |
| ryml_yaml_inplace_reuse_nofilter         |  631.98 |  33.94x | 3294.10% |          0.07 |       0.03x |     -97.05% |
| ryml_yaml_inplace_reuse_reserve          |  632.77 |  33.98x | 3298.32% |          0.07 |       0.03x |     -97.06% |
| ryml_yaml_inplace_reuse                  |  630.48 |  33.86x | 3286.06% |          0.07 |       0.03x |     -97.05% |
| ryml_json_arena_reuse_nofilter_reserve   |  630.84 |  33.88x | 3287.99% |          0.07 |       0.03x |     -97.05% |
| ryml_json_arena_reuse_nofilter           |  624.06 |  33.52x | 3251.58% |          0.08 |       0.03x |     -97.02% |
| ryml_json_arena_reuse_reserve            |  620.19 |  33.31x | 3230.78% |          0.08 |       0.03x |     -97.00% |
| ryml_json_arena_reuse                    |  629.83 |  33.83x | 3282.57% |          0.07 |       0.03x |     -97.04% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  622.34 |  33.42x | 3242.32% |          0.08 |       0.03x |     -97.01% |
| ryml_yaml_arena_reuse_nofilter           |  447.84 |  24.05x | 2305.14% |          0.10 |       0.04x |     -95.84% |
| ryml_yaml_arena_reuse_reserve            |  620.45 |  33.32x | 3232.15% |          0.08 |       0.03x |     -97.00% |
| ryml_yaml_arena_reuse                    |  623.14 |  33.47x | 3246.62% |          0.08 |       0.03x |     -97.01% |
| ryml_json_inplace_nofilter_reserve       |  637.82 |  34.25x | 3325.45% |          0.07 |       0.03x |     -97.08% |
| ryml_json_inplace_nofilter               |  602.56 |  32.36x | 3136.07% |          0.08 |       0.03x |     -96.91% |
| ryml_json_inplace_reserve                |  639.01 |  34.32x | 3331.83% |          0.07 |       0.03x |     -97.09% |
| ryml_json_inplace                        |  592.72 |  31.83x | 3083.27% |          0.08 |       0.03x |     -96.86% |
| ryml_yaml_inplace_nofilter_reserve       |  608.81 |  32.70x | 3169.65% |          0.08 |       0.03x |     -96.94% |
| ryml_yaml_inplace_nofilter               |  592.42 |  31.82x | 3081.62% |          0.08 |       0.03x |     -96.86% |
| ryml_yaml_inplace_reserve                |  630.36 |  33.85x | 3285.38% |          0.07 |       0.03x |     -97.05% |
| ryml_yaml_inplace                        |  587.72 |  31.56x | 3056.41% |          0.08 |       0.03x |     -96.83% |
| ryml_json_arena_nofilter_reserve         |  614.36 |  32.99x | 3199.47% |          0.08 |       0.03x |     -96.97% |
| ryml_json_arena_nofilter                 |  573.60 |  30.81x | 2980.54% |          0.08 |       0.03x |     -96.75% |
| ryml_json_arena_reserve                  |  609.20 |  32.72x | 3171.77% |          0.08 |       0.03x |     -96.94% |
| ryml_json_arena                          |  595.72 |  31.99x | 3099.35% |          0.08 |       0.03x |     -96.87% |
| ryml_yaml_arena_nofilter_reserve         |  616.48 |  33.11x | 3210.83% |          0.08 |       0.03x |     -96.98% |
| ryml_yaml_arena_nofilter                 |  575.89 |  30.93x | 2992.85% |          0.08 |       0.03x |     -96.77% |
| ryml_yaml_arena_reserve                  |  621.95 |  33.40x | 3240.24% |          0.08 |       0.03x |     -97.01% |
| ryml_yaml_arena                          |  574.46 |  30.85x | 2985.19% |          0.08 |       0.03x |     -96.76% |
| libyaml_arena                            |  126.62 |   6.80x |  580.03% |          0.37 |       0.15x |     -85.29% |
| libyaml_arena_reuse                      |  123.16 |   6.61x |  561.44% |          0.38 |       0.15x |     -84.88% |
| libfyaml_arena                           |  143.05 |   7.68x |  668.28% |          0.33 |       0.13x |     -86.98% |
| yamlcpp_arena                            |   18.62 |   1.00x |    0.00% |          2.51 |       1.00x |       0.00% |
| rapidjson_arena                          |  625.72 |  33.60x | 3260.50% |          0.07 |       0.03x |     -97.02% |
| rapidjson_inplace                        | 1675.89 |  90.00x | 8900.47% |          0.03 |       0.01x |     -98.89% |
| sajson_arena                             |  668.52 |  35.90x | 3490.31% |          0.07 |       0.03x |     -97.21% |
| sajson_inplace                           |  683.11 |  36.69x | 3568.68% |          0.07 |       0.03x |     -97.27% |
| jsoncpp_arena                            |  283.73 |  15.24x | 1423.79% |          0.17 |       0.07x |     -93.44% |
| nlohmann_arena                           |  212.67 |  11.42x | 1042.16% |          0.22 |       0.09x |     -91.24% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+--------------------------------------------------------------------------------------------------------+
|                                    emit benchmark: compile_commands                                    |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |   MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  809.12 | 123.72x | 12271.93% |          0.06 |       0.01x |     -99.19% |
| ryml_yaml_json_str_reserve | 1024.90 | 156.71x | 15571.40% |          0.05 |       0.01x |     -99.36% |
| ryml_yaml_str              |  809.17 | 123.73x | 12272.66% |          0.06 |       0.01x |     -99.19% |
| ryml_yaml_json_str         | 1027.40 | 157.10x | 15609.53% |          0.05 |       0.01x |     -99.36% |
| ryml_yaml_ostream          |    7.40 |   1.13x |    13.21% |          6.32 |       0.88x |     -11.67% |
| ryml_yaml_json_ostream     |   11.75 |   1.80x |    79.74% |          3.98 |       0.56x |     -44.36% |
| fyaml_str_reserve          |  420.78 |  64.34x |  6334.02% |          0.11 |       0.02x |     -98.45% |
| fyaml_str                  |  422.00 |  64.53x |  6352.63% |          0.11 |       0.02x |     -98.45% |
| fyaml_ostream              |   10.68 |   1.63x |    63.31% |          4.38 |       0.61x |     -38.77% |
| yamlcpp_                   |    6.54 |   1.00x |     0.00% |          7.16 |       1.00x |       0.00% |
| rapidjson_                 |  690.73 | 105.62x | 10461.64% |          0.07 |       0.01x |     -99.05% |
| jsoncpp_                   |   10.99 |   1.68x |    68.05% |          4.26 |       0.60x |     -40.49% |
| nlohmann_                  |   11.62 |   1.78x |    77.65% |          4.03 |       0.56x |     -43.71% |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+ 


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
+---------------------------------------------------------------------------------------------------------------------+
|                                               parse benchmark: travis                                               |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  284.79 |  26.93x | 2593.43% |          0.01 |       0.04x |     -96.29% |
| ryml_yaml_inplace_reuse_nofilter         |  284.16 |  26.87x | 2587.44% |          0.01 |       0.04x |     -96.28% |
| ryml_yaml_inplace_reuse_reserve          |  268.37 |  25.38x | 2438.13% |          0.02 |       0.04x |     -96.06% |
| ryml_yaml_inplace_reuse                  |  267.52 |  25.30x | 2430.10% |          0.02 |       0.04x |     -96.05% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  293.24 |  27.73x | 2673.33% |          0.01 |       0.04x |     -96.39% |
| ryml_yaml_arena_reuse_nofilter           |  266.73 |  25.23x | 2422.59% |          0.02 |       0.04x |     -96.04% |
| ryml_yaml_arena_reuse_reserve            |  268.87 |  25.43x | 2442.89% |          0.02 |       0.04x |     -96.07% |
| ryml_yaml_arena_reuse                    |  269.18 |  25.46x | 2445.83% |          0.02 |       0.04x |     -96.07% |
| ryml_yaml_inplace_nofilter_reserve       |  293.98 |  27.80x | 2680.35% |          0.01 |       0.04x |     -96.40% |
| ryml_yaml_inplace_nofilter               |  271.51 |  25.68x | 2467.85% |          0.01 |       0.04x |     -96.11% |
| ryml_yaml_inplace_reserve                |  269.07 |  25.45x | 2444.73% |          0.02 |       0.04x |     -96.07% |
| ryml_yaml_inplace                        |  247.34 |  23.39x | 2239.23% |          0.02 |       0.04x |     -95.73% |
| ryml_yaml_arena_nofilter_reserve         |  291.99 |  27.61x | 2661.48% |          0.01 |       0.04x |     -96.38% |
| ryml_yaml_arena_nofilter                 |  269.33 |  25.47x | 2447.20% |          0.02 |       0.04x |     -96.07% |
| ryml_yaml_arena_reserve                  |  267.08 |  25.26x | 2425.96% |          0.02 |       0.04x |     -96.04% |
| ryml_yaml_arena                          |  251.84 |  23.82x | 2281.79% |          0.02 |       0.04x |     -95.80% |
| libyaml_arena                            |   79.13 |   7.48x |  648.40% |          0.05 |       0.13x |     -86.64% |
| libyaml_arena_reuse                      |   79.57 |   7.53x |  652.53% |          0.05 |       0.13x |     -86.71% |
| libfyaml_arena                           |   56.21 |   5.32x |  431.58% |          0.07 |       0.19x |     -81.19% |
| yamlcpp_arena                            |   10.57 |   1.00x |    0.00% |          0.38 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+--------------------------------------------------------------------------------------------------------+
|                                         emit benchmark: travis                                         |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |   MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      | 1514.12 | 193.36x | 19235.55% |          0.00 |       0.01x |     -99.48% |
| ryml_yaml_json_str_reserve |  586.64 |  74.91x |  7391.45% |          0.01 |       0.01x |     -98.67% |
| ryml_yaml_str              | 1520.38 | 194.16x | 19315.57% |          0.00 |       0.01x |     -99.48% |
| ryml_yaml_json_str         |  586.58 |  74.91x |  7390.73% |          0.01 |       0.01x |     -98.67% |
| ryml_yaml_ostream          |   12.49 |   1.60x |    59.52% |          0.33 |       0.63x |     -37.31% |
| ryml_yaml_json_ostream     |    1.92 |   0.25x |   -75.49% |          2.12 |       4.08x |     308.05% |
| fyaml_str_reserve          |   62.02 |   7.92x |   692.03% |          0.07 |       0.13x |     -87.37% |
| fyaml_str                  |   62.10 |   7.93x |   693.06% |          0.07 |       0.13x |     -87.39% |
| fyaml_ostream              |    7.85 |   1.00x |     0.24% |          0.52 |       1.00x |      -0.24% |
| yamlcpp_                   |    7.83 |   1.00x |     0.00% |          0.52 |       1.00x |       0.00% |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+ 


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
+---------------------------------------------------------------------------------------------------------------------+
|                                       parse benchmark: scalar_dquot_multiline                                       |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  554.87 |  25.49x | 2449.28% |          0.13 |       0.04x |     -96.08% |
| ryml_yaml_inplace_reuse_nofilter         |  543.74 |  24.98x | 2398.14% |          0.13 |       0.04x |     -96.00% |
| ryml_yaml_inplace_reuse_reserve          |  196.10 |   9.01x |  800.93% |          0.37 |       0.11x |     -88.90% |
| ryml_yaml_inplace_reuse                  |  196.44 |   9.02x |  802.49% |          0.37 |       0.11x |     -88.92% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  545.83 |  25.08x | 2407.71% |          0.13 |       0.04x |     -96.01% |
| ryml_yaml_arena_reuse_nofilter           |  538.48 |  24.74x | 2373.95% |          0.13 |       0.04x |     -95.96% |
| ryml_yaml_arena_reuse_reserve            |  195.01 |   8.96x |  795.94% |          0.37 |       0.11x |     -88.84% |
| ryml_yaml_arena_reuse                    |  194.60 |   8.94x |  794.04% |          0.37 |       0.11x |     -88.81% |
| ryml_yaml_inplace_nofilter_reserve       |  549.37 |  25.24x | 2424.01% |          0.13 |       0.04x |     -96.04% |
| ryml_yaml_inplace_nofilter               |  548.25 |  25.19x | 2418.84% |          0.13 |       0.04x |     -96.03% |
| ryml_yaml_inplace_reserve                |  196.44 |   9.03x |  802.53% |          0.37 |       0.11x |     -88.92% |
| ryml_yaml_inplace                        |  193.72 |   8.90x |  790.02% |          0.37 |       0.11x |     -88.76% |
| ryml_yaml_arena_nofilter_reserve         |  549.91 |  25.26x | 2426.49% |          0.13 |       0.04x |     -96.04% |
| ryml_yaml_arena_nofilter                 |  544.78 |  25.03x | 2402.90% |          0.13 |       0.04x |     -96.00% |
| ryml_yaml_arena_reserve                  |  195.05 |   8.96x |  796.15% |          0.37 |       0.11x |     -88.84% |
| ryml_yaml_arena                          |  194.09 |   8.92x |  791.70% |          0.37 |       0.11x |     -88.79% |
| libyaml_arena                            |  140.42 |   6.45x |  545.16% |          0.51 |       0.16x |     -84.50% |
| libyaml_arena_reuse                      |  137.79 |   6.33x |  533.07% |          0.52 |       0.16x |     -84.20% |
| libfyaml_arena                           |  165.16 |   7.59x |  658.79% |          0.44 |       0.13x |     -86.82% |
| yamlcpp_arena                            |   21.77 |   1.00x |    0.00% |          3.32 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+-------------------------------------------------------------------------------------------------------+
|                                 emit benchmark: scalar_dquot_multiline                                |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  726.21 |  76.81x | 7580.62% |          0.10 |       0.01x |     -98.70% |
| ryml_yaml_json_str_reserve |  677.44 |  71.65x | 7064.90% |          0.11 |       0.01x |     -98.60% |
| ryml_yaml_str              |  726.26 |  76.81x | 7581.24% |          0.10 |       0.01x |     -98.70% |
| ryml_yaml_json_str         |  676.11 |  71.51x | 7050.77% |          0.11 |       0.01x |     -98.60% |
| ryml_yaml_ostream          |   10.90 |   1.15x |   15.26% |          6.62 |       0.87x |     -13.24% |
| ryml_yaml_json_ostream     |    9.74 |   1.03x |    2.96% |          7.41 |       0.97x |      -2.88% |
| fyaml_str_reserve          |   46.29 |   4.90x |  389.57% |          1.56 |       0.20x |     -79.57% |
| fyaml_str                  |   46.29 |   4.90x |  389.54% |          1.56 |       0.20x |     -79.57% |
| fyaml_ostream              |   27.41 |   2.90x |  189.90% |          2.63 |       0.34x |     -65.51% |
| yamlcpp_                   |    9.46 |   1.00x |    0.00% |          7.63 |       1.00x |       0.00% |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+---------------------------------------------------------------------------------------------------------------------+
|                                       parse benchmark: scalar_dquot_singleline                                      |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  412.09 |  30.13x | 2913.05% |          0.01 |       0.03x |     -96.68% |
| ryml_yaml_inplace_reuse_nofilter         |  413.16 |  30.21x | 2920.87% |          0.01 |       0.03x |     -96.69% |
| ryml_yaml_inplace_reuse_reserve          |  407.85 |  29.82x | 2882.09% |          0.01 |       0.03x |     -96.65% |
| ryml_yaml_inplace_reuse                  |  407.82 |  29.82x | 2881.81% |          0.01 |       0.03x |     -96.65% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  409.54 |  29.94x | 2894.45% |          0.01 |       0.03x |     -96.66% |
| ryml_yaml_arena_reuse_nofilter           |  365.63 |  26.73x | 2573.36% |          0.01 |       0.04x |     -96.26% |
| ryml_yaml_arena_reuse_reserve            |  404.28 |  29.56x | 2855.96% |          0.01 |       0.03x |     -96.62% |
| ryml_yaml_arena_reuse                    |  403.45 |  29.50x | 2849.90% |          0.01 |       0.03x |     -96.61% |
| ryml_yaml_inplace_nofilter_reserve       |  412.83 |  30.18x | 2918.46% |          0.01 |       0.03x |     -96.69% |
| ryml_yaml_inplace_nofilter               |  376.28 |  27.51x | 2651.23% |          0.01 |       0.04x |     -96.37% |
| ryml_yaml_inplace_reserve                |  404.16 |  29.55x | 2855.12% |          0.01 |       0.03x |     -96.62% |
| ryml_yaml_inplace                        |  366.11 |  26.77x | 2576.88% |          0.01 |       0.04x |     -96.26% |
| ryml_yaml_arena_nofilter_reserve         |  386.48 |  28.26x | 2725.79% |          0.01 |       0.04x |     -96.46% |
| ryml_yaml_arena_nofilter                 |  345.07 |  25.23x | 2423.05% |          0.01 |       0.04x |     -96.04% |
| ryml_yaml_arena_reserve                  |  395.54 |  28.92x | 2792.09% |          0.01 |       0.03x |     -96.54% |
| ryml_yaml_arena                          |  370.74 |  27.11x | 2610.76% |          0.01 |       0.04x |     -96.31% |
| libyaml_arena                            |   78.23 |   5.72x |  471.98% |          0.06 |       0.17x |     -82.52% |
| libyaml_arena_reuse                      |   78.98 |   5.77x |  477.47% |          0.06 |       0.17x |     -82.68% |
| libfyaml_arena                           |   59.03 |   4.32x |  331.60% |          0.08 |       0.23x |     -76.83% |
| yamlcpp_arena                            |   13.68 |   1.00x |    0.00% |          0.35 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+--------------------------------------------------------------------------------------------------------+
|                                emit benchmark: scalar_dquot_singleline                                 |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |   MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  669.38 | 105.04x | 10404.17% |          0.01 |       0.01x |     -99.05% |
| ryml_yaml_json_str_reserve |  671.10 | 105.31x | 10431.17% |          0.01 |       0.01x |     -99.05% |
| ryml_yaml_str              |  669.91 | 105.12x | 10412.47% |          0.01 |       0.01x |     -99.05% |
| ryml_yaml_json_str         |  670.60 | 105.23x | 10423.31% |          0.01 |       0.01x |     -99.05% |
| ryml_yaml_ostream          |    8.25 |   1.29x |    29.45% |          0.58 |       0.77x |     -22.75% |
| ryml_yaml_json_ostream     |    2.48 |   0.39x |   -61.12% |          1.95 |       2.57x |     157.17% |
| fyaml_str_reserve          |  187.96 |  29.50x |  2849.55% |          0.03 |       0.03x |     -96.61% |
| fyaml_str                  |  187.83 |  29.48x |  2847.58% |          0.03 |       0.03x |     -96.61% |
| fyaml_ostream              |    7.53 |   1.18x |    18.12% |          0.64 |       0.85x |     -15.34% |
| yamlcpp_                   |    6.37 |   1.00x |     0.00% |          0.76 |       1.00x |       0.00% |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+ 


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
+---------------------------------------------------------------------------------------------------------------------+
|                                       parse benchmark: scalar_squot_multiline                                       |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  740.18 |  30.61x | 2960.60% |          0.10 |       0.03x |     -96.73% |
| ryml_yaml_inplace_reuse_nofilter         |  744.08 |  30.77x | 2976.74% |          0.10 |       0.03x |     -96.75% |
| ryml_yaml_inplace_reuse_reserve          |  295.23 |  12.21x | 1120.75% |          0.24 |       0.08x |     -91.81% |
| ryml_yaml_inplace_reuse                  |  294.18 |  12.16x | 1116.42% |          0.25 |       0.08x |     -91.78% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  730.74 |  30.22x | 2921.57% |          0.10 |       0.03x |     -96.69% |
| ryml_yaml_arena_reuse_nofilter           |  725.10 |  29.98x | 2898.26% |          0.10 |       0.03x |     -96.66% |
| ryml_yaml_arena_reuse_reserve            |  292.45 |  12.09x | 1109.26% |          0.25 |       0.08x |     -91.73% |
| ryml_yaml_arena_reuse                    |  292.72 |  12.10x | 1110.40% |          0.25 |       0.08x |     -91.74% |
| ryml_yaml_inplace_nofilter_reserve       |  748.09 |  30.93x | 2993.32% |          0.10 |       0.03x |     -96.77% |
| ryml_yaml_inplace_nofilter               |  735.14 |  30.40x | 2939.78% |          0.10 |       0.03x |     -96.71% |
| ryml_yaml_inplace_reserve                |  294.64 |  12.18x | 1118.35% |          0.25 |       0.08x |     -91.79% |
| ryml_yaml_inplace                        |  293.45 |  12.13x | 1113.39% |          0.25 |       0.08x |     -91.76% |
| ryml_yaml_arena_nofilter_reserve         |  734.35 |  30.37x | 2936.52% |          0.10 |       0.03x |     -96.71% |
| ryml_yaml_arena_nofilter                 |  730.75 |  30.22x | 2921.63% |          0.10 |       0.03x |     -96.69% |
| ryml_yaml_arena_reserve                  |  293.21 |  12.12x | 1112.40% |          0.25 |       0.08x |     -91.75% |
| ryml_yaml_arena                          |  292.22 |  12.08x | 1108.31% |          0.25 |       0.08x |     -91.72% |
| libyaml_arena                            |  143.99 |   5.95x |  495.38% |          0.50 |       0.17x |     -83.20% |
| libyaml_arena_reuse                      |  144.59 |   5.98x |  497.86% |          0.50 |       0.17x |     -83.27% |
| libfyaml_arena                           |  166.50 |   6.88x |  588.48% |          0.43 |       0.15x |     -85.48% |
| yamlcpp_arena                            |   24.18 |   1.00x |    0.00% |          2.99 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+--------------------------------------------------------------------------------------------------------+
|                                 emit benchmark: scalar_squot_multiline                                 |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |   MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      | 1353.43 | 156.99x | 15598.69% |          0.05 |       0.01x |     -99.36% |
| ryml_yaml_json_str_reserve |  680.97 |  78.99x |  7798.77% |          0.11 |       0.01x |     -98.73% |
| ryml_yaml_str              | 1384.99 | 160.65x | 15964.78% |          0.05 |       0.01x |     -99.38% |
| ryml_yaml_json_str         |  680.96 |  78.99x |  7798.64% |          0.11 |       0.01x |     -98.73% |
| ryml_yaml_ostream          |   10.27 |   1.19x |    19.07% |          7.04 |       0.84x |     -16.01% |
| ryml_yaml_json_ostream     |    9.43 |   1.09x |     9.39% |          7.66 |       0.91x |      -8.58% |
| fyaml_str_reserve          |   46.89 |   5.44x |   443.93% |          1.54 |       0.18x |     -81.62% |
| fyaml_str                  |   47.01 |   5.45x |   445.30% |          1.54 |       0.18x |     -81.66% |
| fyaml_ostream              |   27.32 |   3.17x |   216.90% |          2.65 |       0.32x |     -68.44% |
| yamlcpp_                   |    8.62 |   1.00x |     0.00% |          8.38 |       1.00x |       0.00% |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+ 


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
+---------------------------------------------------------------------------------------------------------------------+
|                                       parse benchmark: scalar_squot_singleline                                      |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  445.02 |  31.13x | 3013.16% |          0.01 |       0.03x |     -96.79% |
| ryml_yaml_inplace_reuse_nofilter         |  445.85 |  31.19x | 3018.95% |          0.01 |       0.03x |     -96.79% |
| ryml_yaml_inplace_reuse_reserve          |  421.71 |  29.50x | 2850.05% |          0.01 |       0.03x |     -96.61% |
| ryml_yaml_inplace_reuse                  |  427.73 |  29.92x | 2892.20% |          0.01 |       0.03x |     -96.66% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  442.68 |  30.97x | 2996.81% |          0.01 |       0.03x |     -96.77% |
| ryml_yaml_arena_reuse_nofilter           |  391.87 |  27.41x | 2641.33% |          0.01 |       0.04x |     -96.35% |
| ryml_yaml_arena_reuse_reserve            |  420.78 |  29.44x | 2843.60% |          0.01 |       0.03x |     -96.60% |
| ryml_yaml_arena_reuse                    |  419.70 |  29.36x | 2836.00% |          0.01 |       0.03x |     -96.59% |
| ryml_yaml_inplace_nofilter_reserve       |  442.24 |  30.94x | 2993.68% |          0.01 |       0.03x |     -96.77% |
| ryml_yaml_inplace_nofilter               |  400.66 |  28.03x | 2702.79% |          0.01 |       0.04x |     -96.43% |
| ryml_yaml_inplace_reserve                |  423.79 |  29.65x | 2864.64% |          0.01 |       0.03x |     -96.63% |
| ryml_yaml_inplace                        |  382.42 |  26.75x | 2575.26% |          0.01 |       0.04x |     -96.26% |
| ryml_yaml_arena_nofilter_reserve         |  439.19 |  30.72x | 2972.34% |          0.01 |       0.03x |     -96.75% |
| ryml_yaml_arena_nofilter                 |  378.81 |  26.50x | 2550.00% |          0.01 |       0.04x |     -96.23% |
| ryml_yaml_arena_reserve                  |  418.40 |  29.27x | 2826.92% |          0.01 |       0.03x |     -96.58% |
| ryml_yaml_arena                          |  384.46 |  26.90x | 2589.51% |          0.01 |       0.04x |     -96.28% |
| libyaml_arena                            |   78.30 |   5.48x |  447.76% |          0.06 |       0.18x |     -81.74% |
| libyaml_arena_reuse                      |   80.48 |   5.63x |  462.99% |          0.06 |       0.18x |     -82.24% |
| libfyaml_arena                           |   58.42 |   4.09x |  308.69% |          0.08 |       0.24x |     -75.53% |
| yamlcpp_arena                            |   14.29 |   1.00x |    0.00% |          0.34 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+--------------------------------------------------------------------------------------------------------+
|                                emit benchmark: scalar_squot_singleline                                 |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |   MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  928.72 | 145.80x | 14480.01% |          0.01 |       0.01x |     -99.31% |
| ryml_yaml_json_str_reserve |  653.11 | 102.53x | 10153.12% |          0.01 |       0.01x |     -99.02% |
| ryml_yaml_str              |  930.25 | 146.04x | 14504.04% |          0.01 |       0.01x |     -99.32% |
| ryml_yaml_json_str         |  681.97 | 107.06x | 10606.20% |          0.01 |       0.01x |     -99.07% |
| ryml_yaml_ostream          |    9.54 |   1.50x |    49.71% |          0.51 |       0.67x |     -33.21% |
| ryml_yaml_json_ostream     |    2.44 |   0.38x |   -61.73% |          1.98 |       2.61x |     161.29% |
| fyaml_str_reserve          |  165.56 |  25.99x |  2499.13% |          0.03 |       0.04x |     -96.15% |
| fyaml_str                  |  165.93 |  26.05x |  2504.91% |          0.03 |       0.04x |     -96.16% |
| fyaml_ostream              |    7.45 |   1.17x |    16.93% |          0.65 |       0.86x |     -14.48% |
| yamlcpp_                   |    6.37 |   1.00x |     0.00% |          0.76 |       1.00x |       0.00% |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+ 


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
+---------------------------------------------------------------------------------------------------------------------+
|                                       parse benchmark: scalar_plain_multiline                                       |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  378.00 |  30.17x | 2916.63% |          0.19 |       0.03x |     -96.69% |
| ryml_yaml_inplace_reuse_nofilter         |  381.77 |  30.47x | 2946.78% |          0.19 |       0.03x |     -96.72% |
| ryml_yaml_inplace_reuse_reserve          |  208.45 |  16.64x | 1563.53% |          0.35 |       0.06x |     -93.99% |
| ryml_yaml_inplace_reuse                  |  208.03 |  16.60x | 1560.20% |          0.35 |       0.06x |     -93.98% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  379.15 |  30.26x | 2925.81% |          0.19 |       0.03x |     -96.70% |
| ryml_yaml_arena_reuse_nofilter           |  375.46 |  29.96x | 2896.37% |          0.19 |       0.03x |     -96.66% |
| ryml_yaml_arena_reuse_reserve            |  207.77 |  16.58x | 1558.12% |          0.35 |       0.06x |     -93.97% |
| ryml_yaml_arena_reuse                    |  207.32 |  16.55x | 1554.51% |          0.35 |       0.06x |     -93.96% |
| ryml_yaml_inplace_nofilter_reserve       |  382.36 |  30.51x | 2951.46% |          0.19 |       0.03x |     -96.72% |
| ryml_yaml_inplace_nofilter               |  378.69 |  30.22x | 2922.15% |          0.19 |       0.03x |     -96.69% |
| ryml_yaml_inplace_reserve                |  206.10 |  16.45x | 1544.81% |          0.35 |       0.06x |     -93.92% |
| ryml_yaml_inplace                        |  207.03 |  16.52x | 1552.23% |          0.35 |       0.06x |     -93.95% |
| ryml_yaml_arena_nofilter_reserve         |  378.54 |  30.21x | 2920.93% |          0.19 |       0.03x |     -96.69% |
| ryml_yaml_arena_nofilter                 |  375.71 |  29.98x | 2898.36% |          0.19 |       0.03x |     -96.66% |
| ryml_yaml_arena_reserve                  |  206.82 |  16.51x | 1550.58% |          0.35 |       0.06x |     -93.94% |
| ryml_yaml_arena                          |  205.14 |  16.37x | 1537.13% |          0.35 |       0.06x |     -93.89% |
| libyaml_arena                            |  131.16 |  10.47x |  946.75% |          0.55 |       0.10x |     -90.45% |
| libyaml_arena_reuse                      |  132.60 |  10.58x |  958.26% |          0.54 |       0.09x |     -90.55% |
| libfyaml_arena                           |  164.71 |  13.14x | 1214.45% |          0.44 |       0.08x |     -92.39% |
| yamlcpp_arena                            |   12.53 |   1.00x |    0.00% |          5.75 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+--------------------------------------------------------------------------------------------------------+
|                                 emit benchmark: scalar_plain_multiline                                 |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |   MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      | 2630.35 | 318.81x | 31780.90% |          0.03 |       0.00x |     -99.69% |
| ryml_yaml_json_str_reserve |  580.37 |  70.34x |  6934.30% |          0.12 |       0.01x |     -98.58% |
| ryml_yaml_str              | 2666.13 | 323.15x | 32214.50% |          0.03 |       0.00x |     -99.69% |
| ryml_yaml_json_str         |  580.89 |  70.41x |  6940.63% |          0.12 |       0.01x |     -98.58% |
| ryml_yaml_ostream          |   10.35 |   1.25x |    25.41% |          6.96 |       0.80x |     -20.26% |
| ryml_yaml_json_ostream     |    9.60 |   1.16x |    16.35% |          7.50 |       0.86x |     -14.05% |
| fyaml_str_reserve          |   50.69 |   6.14x |   514.37% |          1.42 |       0.16x |     -83.72% |
| fyaml_str                  |   50.62 |   6.13x |   513.49% |          1.42 |       0.16x |     -83.70% |
| fyaml_ostream              |   27.96 |   3.39x |   238.95% |          2.58 |       0.30x |     -70.50% |
| yamlcpp_                   |    8.25 |   1.00x |     0.00% |          8.73 |       1.00x |       0.00% |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+ 


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
+---------------------------------------------------------------------------------------------------------------------+
|                                       parse benchmark: scalar_plain_singleline                                      |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  283.65 |  29.68x | 2868.47% |          0.02 |       0.03x |     -96.63% |
| ryml_yaml_inplace_reuse_nofilter         |  276.55 |  28.94x | 2794.15% |          0.02 |       0.03x |     -96.54% |
| ryml_yaml_inplace_reuse_reserve          |  279.12 |  29.21x | 2820.99% |          0.02 |       0.03x |     -96.58% |
| ryml_yaml_inplace_reuse                  |  284.39 |  29.76x | 2876.14% |          0.02 |       0.03x |     -96.64% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  285.13 |  29.84x | 2883.96% |          0.02 |       0.03x |     -96.65% |
| ryml_yaml_arena_reuse_nofilter           |  262.23 |  27.44x | 2644.30% |          0.02 |       0.04x |     -96.36% |
| ryml_yaml_arena_reuse_reserve            |  282.02 |  29.51x | 2851.35% |          0.02 |       0.03x |     -96.61% |
| ryml_yaml_arena_reuse                    |  282.16 |  29.53x | 2852.81% |          0.02 |       0.03x |     -96.61% |
| ryml_yaml_inplace_nofilter_reserve       |  285.91 |  29.92x | 2892.05% |          0.02 |       0.03x |     -96.66% |
| ryml_yaml_inplace_nofilter               |  265.92 |  27.83x | 2682.85% |          0.02 |       0.04x |     -96.41% |
| ryml_yaml_inplace_reserve                |  283.11 |  29.63x | 2862.74% |          0.02 |       0.03x |     -96.62% |
| ryml_yaml_inplace                        |  263.72 |  27.60x | 2659.83% |          0.02 |       0.04x |     -96.38% |
| ryml_yaml_arena_nofilter_reserve         |  283.31 |  29.65x | 2864.88% |          0.02 |       0.03x |     -96.63% |
| ryml_yaml_arena_nofilter                 |  263.02 |  27.53x | 2652.57% |          0.02 |       0.04x |     -96.37% |
| ryml_yaml_arena_reserve                  |  280.24 |  29.33x | 2832.74% |          0.02 |       0.03x |     -96.59% |
| ryml_yaml_arena                          |  265.02 |  27.73x | 2673.48% |          0.02 |       0.04x |     -96.39% |
| libyaml_arena                            |   72.58 |   7.60x |  659.60% |          0.06 |       0.13x |     -86.84% |
| libyaml_arena_reuse                      |   74.59 |   7.81x |  680.61% |          0.06 |       0.13x |     -87.19% |
| libfyaml_arena                           |   57.53 |   6.02x |  502.10% |          0.08 |       0.17x |     -83.39% |
| yamlcpp_arena                            |    9.56 |   1.00x |    0.00% |          0.49 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+--------------------------------------------------------------------------------------------------------+
|                                emit benchmark: scalar_plain_singleline                                 |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |   MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      | 1233.97 | 202.67x | 20166.52% |          0.00 |       0.00x |     -99.51% |
| ryml_yaml_json_str_reserve |  348.54 |  57.24x |  5624.34% |          0.01 |       0.02x |     -98.25% |
| ryml_yaml_str              | 1239.24 | 203.53x | 20253.00% |          0.00 |       0.00x |     -99.51% |
| ryml_yaml_json_str         |  348.12 |  57.17x |  5617.39% |          0.01 |       0.02x |     -98.25% |
| ryml_yaml_ostream          |    8.95 |   1.47x |    47.02% |          0.52 |       0.68x |     -31.98% |
| ryml_yaml_json_ostream     |    2.55 |   0.42x |   -58.12% |          1.83 |       2.39x |     138.77% |
| fyaml_str_reserve          |   45.79 |   7.52x |   651.98% |          0.10 |       0.13x |     -86.70% |
| fyaml_str                  |   46.05 |   7.56x |   656.31% |          0.10 |       0.13x |     -86.78% |
| fyaml_ostream              |    8.28 |   1.36x |    36.04% |          0.56 |       0.74x |     -26.49% |
| yamlcpp_                   |    6.09 |   1.00x |     0.00% |          0.77 |       1.00x |       0.00% |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+ 


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
+---------------------------------------------------------------------------------------------------------------------+
|                                   parse benchmark: scalar_block_literal_multiline                                   |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve | 1275.44 |  47.58x | 4657.73% |          0.06 |       0.02x |     -97.90% |
| ryml_yaml_inplace_reuse_nofilter         | 1271.16 |  47.42x | 4641.75% |          0.06 |       0.02x |     -97.89% |
| ryml_yaml_inplace_reuse_reserve          |  641.88 |  23.94x | 2294.40% |          0.11 |       0.04x |     -95.82% |
| ryml_yaml_inplace_reuse                  |  644.29 |  24.03x | 2303.36% |          0.11 |       0.04x |     -95.84% |
| ryml_yaml_arena_reuse_nofilter_reserve   | 1254.99 |  46.81x | 4581.44% |          0.06 |       0.02x |     -97.86% |
| ryml_yaml_arena_reuse_nofilter           | 1218.32 |  45.45x | 4444.67% |          0.06 |       0.02x |     -97.80% |
| ryml_yaml_arena_reuse_reserve            |  636.23 |  23.73x | 2273.30% |          0.11 |       0.04x |     -95.79% |
| ryml_yaml_arena_reuse                    |  637.16 |  23.77x | 2276.79% |          0.11 |       0.04x |     -95.79% |
| ryml_yaml_inplace_nofilter_reserve       | 1283.32 |  47.87x | 4687.11% |          0.06 |       0.02x |     -97.91% |
| ryml_yaml_inplace_nofilter               | 1252.84 |  46.73x | 4573.42% |          0.06 |       0.02x |     -97.86% |
| ryml_yaml_inplace_reserve                |  644.17 |  24.03x | 2302.94% |          0.11 |       0.04x |     -95.84% |
| ryml_yaml_inplace                        |  630.42 |  23.52x | 2251.61% |          0.12 |       0.04x |     -95.75% |
| ryml_yaml_arena_nofilter_reserve         | 1237.68 |  46.17x | 4516.87% |          0.06 |       0.02x |     -97.83% |
| ryml_yaml_arena_nofilter                 | 1209.71 |  45.13x | 4412.54% |          0.06 |       0.02x |     -97.78% |
| ryml_yaml_arena_reserve                  |  635.24 |  23.70x | 2269.62% |          0.11 |       0.04x |     -95.78% |
| ryml_yaml_arena                          |  628.94 |  23.46x | 2246.12% |          0.12 |       0.04x |     -95.74% |
| libyaml_arena                            |  171.36 |   6.39x |  539.22% |          0.42 |       0.16x |     -84.36% |
| libyaml_arena_reuse                      |  171.81 |   6.41x |  540.88% |          0.42 |       0.16x |     -84.40% |
| libfyaml_arena                           |  174.51 |   6.51x |  550.97% |          0.42 |       0.15x |     -84.64% |
| yamlcpp_arena                            |   26.81 |   1.00x |    0.00% |          2.71 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+-------------------------------------------------------------------------------------------------------+
|                             emit benchmark: scalar_block_literal_multiline                            |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      | 1656.00 |  71.68x | 7068.47% |          0.04 |       0.01x |     -98.61% |
| ryml_yaml_json_str_reserve |  630.75 |  27.30x | 2630.40% |          0.12 |       0.04x |     -96.34% |
| ryml_yaml_str              | 1654.83 |  71.63x | 7063.42% |          0.04 |       0.01x |     -98.60% |
| ryml_yaml_json_str         |  630.43 |  27.29x | 2629.00% |          0.12 |       0.04x |     -96.34% |
| ryml_yaml_ostream          |   10.57 |   0.46x |  -54.26% |          6.87 |       2.19x |     118.64% |
| ryml_yaml_json_ostream     |    9.64 |   0.42x |  -58.29% |          7.53 |       2.40x |     139.72% |
| fyaml_str_reserve          |   50.03 |   2.17x |  116.56% |          1.45 |       0.46x |     -53.82% |
| fyaml_str                  |   49.94 |   2.16x |  116.18% |          1.45 |       0.46x |     -53.74% |
| fyaml_ostream              |   27.41 |   1.19x |   18.65% |          2.65 |       0.84x |     -15.72% |
| yamlcpp_                   |   23.10 |   1.00x |    0.00% |          3.14 |       1.00x |       0.00% |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+---------------------------------------------------------------------------------------------------------------------+
|                                   parse benchmark: scalar_block_literal_singleline                                  |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  393.18 |  24.89x | 2389.12% |          0.01 |       0.04x |     -95.98% |
| ryml_yaml_inplace_reuse_nofilter         |  395.19 |  25.02x | 2401.88% |          0.01 |       0.04x |     -96.00% |
| ryml_yaml_inplace_reuse_reserve          |  291.91 |  18.48x | 1748.04% |          0.02 |       0.05x |     -94.59% |
| ryml_yaml_inplace_reuse                  |  291.50 |  18.45x | 1745.43% |          0.02 |       0.05x |     -94.58% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  392.10 |  24.82x | 2382.30% |          0.01 |       0.04x |     -95.97% |
| ryml_yaml_arena_reuse_nofilter           |  357.61 |  22.64x | 2163.94% |          0.01 |       0.04x |     -95.58% |
| ryml_yaml_arena_reuse_reserve            |  289.93 |  18.35x | 1735.46% |          0.02 |       0.05x |     -94.55% |
| ryml_yaml_arena_reuse                    |  290.09 |  18.36x | 1736.49% |          0.02 |       0.05x |     -94.55% |
| ryml_yaml_inplace_nofilter_reserve       |  395.57 |  25.04x | 2404.24% |          0.01 |       0.04x |     -96.01% |
| ryml_yaml_inplace_nofilter               |  364.84 |  23.10x | 2209.70% |          0.01 |       0.04x |     -95.67% |
| ryml_yaml_inplace_reserve                |  291.40 |  18.45x | 1744.77% |          0.02 |       0.05x |     -94.58% |
| ryml_yaml_inplace                        |  273.75 |  17.33x | 1633.07% |          0.02 |       0.06x |     -94.23% |
| ryml_yaml_arena_nofilter_reserve         |  391.37 |  24.78x | 2377.66% |          0.01 |       0.04x |     -95.96% |
| ryml_yaml_arena_nofilter                 |  359.33 |  22.75x | 2174.82% |          0.01 |       0.04x |     -95.60% |
| ryml_yaml_arena_reserve                  |  288.72 |  18.28x | 1727.83% |          0.02 |       0.05x |     -94.53% |
| ryml_yaml_arena                          |  273.83 |  17.34x | 1633.57% |          0.02 |       0.06x |     -94.23% |
| libyaml_arena                            |   93.93 |   5.95x |  494.65% |          0.06 |       0.17x |     -83.18% |
| libyaml_arena_reuse                      |   94.28 |   5.97x |  496.86% |          0.06 |       0.17x |     -83.25% |
| libfyaml_arena                           |   62.72 |   3.97x |  297.05% |          0.08 |       0.25x |     -74.81% |
| yamlcpp_arena                            |   15.80 |   1.00x |    0.00% |          0.33 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+--------------------------------------------------------------------------------------------------------+
|                            emit benchmark: scalar_block_literal_singleline                             |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |   MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  994.73 | 157.66x | 15665.95% |          0.01 |       0.01x |     -99.37% |
| ryml_yaml_json_str_reserve |  653.16 | 103.52x | 10252.29% |          0.01 |       0.01x |     -99.03% |
| ryml_yaml_str              | 1017.26 | 161.23x | 16023.16% |          0.01 |       0.01x |     -99.38% |
| ryml_yaml_json_str         |  664.04 | 105.25x | 10424.72% |          0.01 |       0.01x |     -99.05% |
| ryml_yaml_ostream          |    8.57 |   1.36x |    35.87% |          0.61 |       0.74x |     -26.40% |
| ryml_yaml_json_ostream     |    2.72 |   0.43x |   -56.82% |          1.93 |       2.32x |     131.57% |
| fyaml_str_reserve          |   47.49 |   7.53x |   652.68% |          0.11 |       0.13x |     -86.71% |
| fyaml_str                  |   47.38 |   7.51x |   650.87% |          0.11 |       0.13x |     -86.68% |
| fyaml_ostream              |    9.12 |   1.45x |    44.60% |          0.58 |       0.69x |     -30.84% |
| yamlcpp_                   |    6.31 |   1.00x |     0.00% |          0.83 |       1.00x |       0.00% |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+ 


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
+---------------------------------------------------------------------------------------------------------------------+
|                                    parse benchmark: scalar_block_folded_multiline                                   |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve | 1280.32 |  47.85x | 4685.25% |          0.06 |       0.02x |     -97.91% |
| ryml_yaml_inplace_reuse_nofilter         | 1283.30 |  47.96x | 4696.39% |          0.06 |       0.02x |     -97.92% |
| ryml_yaml_inplace_reuse_reserve          |  502.63 |  18.79x | 1778.62% |          0.14 |       0.05x |     -94.68% |
| ryml_yaml_inplace_reuse                  |  503.06 |  18.80x | 1780.23% |          0.14 |       0.05x |     -94.68% |
| ryml_yaml_arena_reuse_nofilter_reserve   | 1253.12 |  46.84x | 4583.59% |          0.06 |       0.02x |     -97.86% |
| ryml_yaml_arena_reuse_nofilter           | 1218.61 |  45.55x | 4454.63% |          0.06 |       0.02x |     -97.80% |
| ryml_yaml_arena_reuse_reserve            |  497.49 |  18.59x | 1759.39% |          0.15 |       0.05x |     -94.62% |
| ryml_yaml_arena_reuse                    |  497.45 |  18.59x | 1759.23% |          0.15 |       0.05x |     -94.62% |
| ryml_yaml_inplace_nofilter_reserve       | 1283.47 |  47.97x | 4697.04% |          0.06 |       0.02x |     -97.92% |
| ryml_yaml_inplace_nofilter               | 1252.86 |  46.83x | 4582.62% |          0.06 |       0.02x |     -97.86% |
| ryml_yaml_inplace_reserve                |  502.87 |  18.80x | 1779.50% |          0.14 |       0.05x |     -94.68% |
| ryml_yaml_inplace                        |  498.05 |  18.61x | 1761.48% |          0.15 |       0.05x |     -94.63% |
| ryml_yaml_arena_nofilter_reserve         | 1243.19 |  46.47x | 4546.50% |          0.06 |       0.02x |     -97.85% |
| ryml_yaml_arena_nofilter                 | 1222.56 |  45.69x | 4469.36% |          0.06 |       0.02x |     -97.81% |
| ryml_yaml_arena_reserve                  |  497.06 |  18.58x | 1757.77% |          0.15 |       0.05x |     -94.62% |
| ryml_yaml_arena                          |  493.20 |  18.43x | 1743.36% |          0.15 |       0.05x |     -94.58% |
| libyaml_arena                            |  171.68 |   6.42x |  541.67% |          0.42 |       0.16x |     -84.42% |
| libyaml_arena_reuse                      |  172.33 |   6.44x |  544.11% |          0.42 |       0.16x |     -84.47% |
| libfyaml_arena                           |  174.95 |   6.54x |  553.89% |          0.41 |       0.15x |     -84.71% |
| yamlcpp_arena                            |   26.76 |   1.00x |    0.00% |          2.71 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+--------------------------------------------------------------------------------------------------------+
|                             emit benchmark: scalar_block_folded_multiline                              |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |   MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      | 1417.73 | 182.77x | 18176.79% |          0.05 |       0.01x |     -99.45% |
| ryml_yaml_json_str_reserve |  675.00 |  87.02x |  8601.85% |          0.11 |       0.01x |     -98.85% |
| ryml_yaml_str              | 1420.69 | 183.15x | 18214.96% |          0.05 |       0.01x |     -99.45% |
| ryml_yaml_json_str         |  674.54 |  86.96x |  8595.88% |          0.11 |       0.01x |     -98.85% |
| ryml_yaml_ostream          |    9.72 |   1.25x |    25.25% |          7.47 |       0.80x |     -20.16% |
| ryml_yaml_json_ostream     |    9.50 |   1.23x |    22.52% |          7.64 |       0.82x |     -18.38% |
| fyaml_str_reserve          |   49.39 |   6.37x |   536.72% |          1.47 |       0.16x |     -84.29% |
| fyaml_str                  |   49.44 |   6.37x |   537.30% |          1.47 |       0.16x |     -84.31% |
| fyaml_ostream              |   27.43 |   3.54x |   253.65% |          2.65 |       0.28x |     -71.72% |
| yamlcpp_                   |    7.76 |   1.00x |     0.00% |          9.36 |       1.00x |       0.00% |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+ 


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
+---------------------------------------------------------------------------------------------------------------------+
|                                   parse benchmark: scalar_block_folded_singleline                                   |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  395.45 |  25.18x | 2417.94% |          0.01 |       0.04x |     -96.03% |
| ryml_yaml_inplace_reuse_nofilter         |  394.61 |  25.13x | 2412.58% |          0.01 |       0.04x |     -96.02% |
| ryml_yaml_inplace_reuse_reserve          |  263.24 |  16.76x | 1576.13% |          0.02 |       0.06x |     -94.03% |
| ryml_yaml_inplace_reuse                  |  266.97 |  17.00x | 1599.84% |          0.02 |       0.06x |     -94.12% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  392.13 |  24.97x | 2396.81% |          0.01 |       0.04x |     -95.99% |
| ryml_yaml_arena_reuse_nofilter           |  352.79 |  22.46x | 2146.28% |          0.01 |       0.04x |     -95.55% |
| ryml_yaml_arena_reuse_reserve            |  265.07 |  16.88x | 1587.75% |          0.02 |       0.06x |     -94.07% |
| ryml_yaml_arena_reuse                    |  265.07 |  16.88x | 1587.75% |          0.02 |       0.06x |     -94.07% |
| ryml_yaml_inplace_nofilter_reserve       |  394.24 |  25.10x | 2410.25% |          0.01 |       0.04x |     -96.02% |
| ryml_yaml_inplace_nofilter               |  363.90 |  23.17x | 2217.03% |          0.01 |       0.04x |     -95.68% |
| ryml_yaml_inplace_reserve                |  265.06 |  16.88x | 1587.73% |          0.02 |       0.06x |     -94.07% |
| ryml_yaml_inplace                        |  243.40 |  15.50x | 1449.77% |          0.02 |       0.06x |     -93.55% |
| ryml_yaml_arena_nofilter_reserve         |  389.45 |  24.80x | 2379.70% |          0.01 |       0.04x |     -95.97% |
| ryml_yaml_arena_nofilter                 |  359.12 |  22.87x | 2186.61% |          0.01 |       0.04x |     -95.63% |
| ryml_yaml_arena_reserve                  |  263.51 |  16.78x | 1577.85% |          0.02 |       0.06x |     -94.04% |
| ryml_yaml_arena                          |  252.36 |  16.07x | 1506.85% |          0.02 |       0.06x |     -93.78% |
| libyaml_arena                            |   91.82 |   5.85x |  484.62% |          0.06 |       0.17x |     -82.89% |
| libyaml_arena_reuse                      |   93.30 |   5.94x |  494.05% |          0.06 |       0.17x |     -83.17% |
| libfyaml_arena                           |   58.90 |   3.75x |  275.06% |          0.09 |       0.27x |     -73.34% |
| yamlcpp_arena                            |   15.71 |   1.00x |    0.00% |          0.33 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+--------------------------------------------------------------------------------------------------------+
|                             emit benchmark: scalar_block_folded_singleline                             |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |   MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  886.82 | 142.09x | 14108.66% |          0.01 |       0.01x |     -99.30% |
| ryml_yaml_json_str_reserve |  655.84 | 105.08x | 10407.89% |          0.01 |       0.01x |     -99.05% |
| ryml_yaml_str              |  880.70 | 141.11x | 14010.66% |          0.01 |       0.01x |     -99.29% |
| ryml_yaml_json_str         |  642.27 | 102.91x | 10190.55% |          0.01 |       0.01x |     -99.03% |
| ryml_yaml_ostream          |    8.67 |   1.39x |    38.84% |          0.61 |       0.72x |     -27.98% |
| ryml_yaml_json_ostream     |    2.59 |   0.42x |   -58.49% |          2.03 |       2.41x |     140.90% |
| fyaml_str_reserve          |   43.37 |   6.95x |   594.91% |          0.12 |       0.14x |     -85.61% |
| fyaml_str                  |   43.76 |   7.01x |   601.18% |          0.12 |       0.14x |     -85.74% |
| fyaml_ostream              |    9.30 |   1.49x |    48.97% |          0.57 |       0.67x |     -32.87% |
| yamlcpp_                   |    6.24 |   1.00x |     0.00% |          0.84 |       1.00x |       0.00% |
+----------------------------+---------+---------+-----------+---------------+-------------+-------------+ 


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
+---------------------------------------------------------------------------------------------------------------------+
|                                  parse benchmark: style_maps_blck_outer1000_inner10                                 |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  105.55 |  28.76x | 2776.12% |          0.84 |       0.03x |     -96.52% |
| ryml_yaml_inplace_reuse_nofilter         |  106.14 |  28.92x | 2792.01% |          0.84 |       0.03x |     -96.54% |
| ryml_yaml_inplace_reuse_reserve          |  107.30 |  29.24x | 2823.74% |          0.83 |       0.03x |     -96.58% |
| ryml_yaml_inplace_reuse                  |  106.01 |  28.89x | 2788.69% |          0.84 |       0.03x |     -96.54% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  105.22 |  28.67x | 2766.98% |          0.84 |       0.03x |     -96.51% |
| ryml_yaml_arena_reuse_nofilter           |   39.76 |  10.83x |  983.35% |          2.24 |       0.09x |     -90.77% |
| ryml_yaml_arena_reuse_reserve            |  106.47 |  29.01x | 2801.01% |          0.84 |       0.03x |     -96.55% |
| ryml_yaml_arena_reuse                    |  106.19 |  28.93x | 2793.48% |          0.84 |       0.03x |     -96.54% |
| ryml_yaml_inplace_nofilter_reserve       |  107.25 |  29.22x | 2822.32% |          0.83 |       0.03x |     -96.58% |
| ryml_yaml_inplace_nofilter               |   40.04 |  10.91x |  991.13% |          2.22 |       0.09x |     -90.84% |
| ryml_yaml_inplace_reserve                |  107.05 |  29.17x | 2816.99% |          0.83 |       0.03x |     -96.57% |
| ryml_yaml_inplace                        |   40.10 |  10.93x |  992.72% |          2.22 |       0.09x |     -90.85% |
| ryml_yaml_arena_nofilter_reserve         |  106.24 |  28.95x | 2794.92% |          0.84 |       0.03x |     -96.55% |
| ryml_yaml_arena_nofilter                 |   39.76 |  10.83x |  983.48% |          2.24 |       0.09x |     -90.77% |
| ryml_yaml_arena_reserve                  |  106.50 |  29.02x | 2801.89% |          0.83 |       0.03x |     -96.55% |
| ryml_yaml_arena                          |   39.88 |  10.87x |  986.76% |          2.23 |       0.09x |     -90.80% |
| libyaml_arena                            |   19.04 |   5.19x |  418.93% |          4.67 |       0.19x |     -80.73% |
| libyaml_arena_reuse                      |   25.86 |   7.05x |  604.56% |          3.44 |       0.14x |     -85.81% |
| libfyaml_arena                           |   11.17 |   3.04x |  204.43% |          7.96 |       0.33x |     -67.15% |
| yamlcpp_arena                            |    3.67 |   1.00x |    0.00% |         24.22 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+-------------------------------------------------------------------------------------------------------+
|                           emit benchmark: style_maps_blck_outer1000_inner10                           |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  357.29 |  53.11x | 5211.11% |          0.25 |       0.02x |     -98.12% |
| ryml_yaml_json_str_reserve |  224.06 |  33.31x | 3230.57% |          0.40 |       0.03x |     -97.00% |
| ryml_yaml_str              |  357.32 |  53.11x | 5211.47% |          0.25 |       0.02x |     -98.12% |
| ryml_yaml_json_str         |  223.43 |  33.21x | 3221.19% |          0.40 |       0.03x |     -96.99% |
| ryml_yaml_ostream          |   17.92 |   2.66x |  166.34% |          4.96 |       0.38x |     -62.45% |
| ryml_yaml_json_ostream     |   14.05 |   2.09x |  108.89% |          6.33 |       0.48x |     -52.13% |
| fyaml_str_reserve          |   27.31 |   4.06x |  305.98% |          3.26 |       0.25x |     -75.37% |
| fyaml_str                  |   27.27 |   4.05x |  305.33% |          3.26 |       0.25x |     -75.33% |
| fyaml_ostream              |   19.73 |   2.93x |  193.28% |          4.51 |       0.34x |     -65.90% |
| yamlcpp_                   |    6.73 |   1.00x |    0.00% |         13.21 |       1.00x |       0.00% |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
style_maps_flow_outer1000_inner10_json
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10_json.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10_json.json
+---------------------------------------------------------------------------------------------------------------------+
|                               parse benchmark: style_maps_flow_outer1000_inner10_json                               |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  108.72 |  36.73x | 3572.83% |          0.67 |       0.03x |     -97.28% |
| ryml_yaml_inplace_reuse_nofilter         |  108.46 |  36.64x | 3564.04% |          0.67 |       0.03x |     -97.27% |
| ryml_yaml_inplace_reuse_reserve          |  105.92 |  35.78x | 3478.23% |          0.69 |       0.03x |     -97.21% |
| ryml_yaml_inplace_reuse                  |  109.38 |  36.95x | 3594.94% |          0.67 |       0.03x |     -97.29% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  108.43 |  36.63x | 3563.15% |          0.67 |       0.03x |     -97.27% |
| ryml_yaml_arena_reuse_nofilter           |   35.12 |  11.86x | 1086.40% |          2.08 |       0.08x |     -91.57% |
| ryml_yaml_arena_reuse_reserve            |  108.58 |  36.68x | 3568.03% |          0.67 |       0.03x |     -97.27% |
| ryml_yaml_arena_reuse                    |  108.54 |  36.67x | 3566.70% |          0.67 |       0.03x |     -97.27% |
| ryml_yaml_inplace_nofilter_reserve       |  109.36 |  36.94x | 3594.44% |          0.67 |       0.03x |     -97.29% |
| ryml_yaml_inplace_nofilter               |   74.86 |  25.29x | 2429.00% |          0.97 |       0.04x |     -96.05% |
| ryml_yaml_inplace_reserve                |  109.12 |  36.86x | 3586.29% |          0.67 |       0.03x |     -97.29% |
| ryml_yaml_inplace                        |   73.81 |  24.93x | 2393.45% |          0.99 |       0.04x |     -95.99% |
| ryml_yaml_arena_nofilter_reserve         |  106.25 |  35.89x | 3489.30% |          0.69 |       0.03x |     -97.21% |
| ryml_yaml_arena_nofilter                 |   72.90 |  24.63x | 2362.75% |          1.00 |       0.04x |     -95.94% |
| ryml_yaml_arena_reserve                  |  106.00 |  35.81x | 3480.83% |          0.69 |       0.03x |     -97.21% |
| ryml_yaml_arena                          |   73.48 |  24.82x | 2382.34% |          0.99 |       0.04x |     -95.97% |
| libyaml_arena                            |   18.63 |   6.29x |  529.30% |          3.91 |       0.16x |     -84.11% |
| libyaml_arena_reuse                      |   19.08 |   6.45x |  544.65% |          3.82 |       0.16x |     -84.49% |
| libfyaml_arena                           |    9.13 |   3.08x |  208.47% |          7.98 |       0.32x |     -67.58% |
| yamlcpp_arena                            |    2.96 |   1.00x |    0.00% |         24.63 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10_json-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10_json-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10_json-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10_json-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10_json-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10_json-cpu_time_ms.html
Built target ryml-bm-parse-style_maps_flow_outer1000_inner10_json-plot
/usr/bin/python3.11
Python 3.11.8
emit
style_maps_flow_outer1000_inner10_json
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10_json.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10_json.json
+-------------------------------------------------------------------------------------------------------+
|                         emit benchmark: style_maps_flow_outer1000_inner10_json                        |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  305.70 |  58.60x | 5760.24% |          0.24 |       0.02x |     -98.29% |
| ryml_yaml_json_str_reserve |  183.79 |  35.23x | 3423.15% |          0.40 |       0.03x |     -97.16% |
| ryml_yaml_str              |  305.73 |  58.61x | 5760.90% |          0.24 |       0.02x |     -98.29% |
| ryml_yaml_json_str         |  183.78 |  35.23x | 3423.03% |          0.40 |       0.03x |     -97.16% |
| ryml_yaml_ostream          |   13.15 |   2.52x |  152.06% |          5.54 |       0.40x |     -60.33% |
| ryml_yaml_json_ostream     |   11.75 |   2.25x |  125.31% |          6.20 |       0.44x |     -55.62% |
| fyaml_str_reserve          |   35.94 |   6.89x |  588.99% |          2.03 |       0.15x |     -85.49% |
| fyaml_str                  |   36.05 |   6.91x |  591.03% |          2.02 |       0.14x |     -85.53% |
| fyaml_ostream              |   18.05 |   3.46x |  246.00% |          4.04 |       0.29x |     -71.10% |
| yamlcpp_                   |    5.22 |   1.00x |    0.00% |         13.98 |       1.00x |       0.00% |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10_json-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10_json-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10_json-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10_json-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10_json-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner10_json-cpu_time_ms.html
Built target ryml-bm-emit-style_maps_flow_outer1000_inner10_json-plot
/usr/bin/python3.11
Python 3.11.8
parse
style_maps_blck_outer1000_inner100
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner100.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_blck_outer1000_inner100.json
+---------------------------------------------------------------------------------------------------------------------+
|                                 parse benchmark: style_maps_blck_outer1000_inner100                                 |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  120.12 |  32.07x | 3106.84% |          8.15 |       0.03x |     -96.88% |
| ryml_yaml_inplace_reuse_nofilter         |  119.46 |  31.89x | 3089.30% |          8.20 |       0.03x |     -96.86% |
| ryml_yaml_inplace_reuse_reserve          |  120.97 |  32.30x | 3129.53% |          8.09 |       0.03x |     -96.90% |
| ryml_yaml_inplace_reuse                  |  117.47 |  31.36x | 3036.01% |          8.33 |       0.03x |     -96.81% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  120.64 |  32.21x | 3120.64% |          8.12 |       0.03x |     -96.90% |
| ryml_yaml_arena_reuse_nofilter           |   56.65 |  15.12x | 1412.43% |         17.28 |       0.07x |     -93.39% |
| ryml_yaml_arena_reuse_reserve            |  120.73 |  32.23x | 3123.21% |          8.11 |       0.03x |     -96.90% |
| ryml_yaml_arena_reuse                    |  118.85 |  31.73x | 3073.01% |          8.24 |       0.03x |     -96.85% |
| ryml_yaml_inplace_nofilter_reserve       |  122.31 |  32.65x | 3165.46% |          8.00 |       0.03x |     -96.94% |
| ryml_yaml_inplace_nofilter               |   87.62 |  23.39x | 2239.24% |         11.17 |       0.04x |     -95.73% |
| ryml_yaml_inplace_reserve                |  122.07 |  32.59x | 3158.95% |          8.02 |       0.03x |     -96.93% |
| ryml_yaml_inplace                        |   89.22 |  23.82x | 2281.82% |         10.97 |       0.04x |     -95.80% |
| ryml_yaml_arena_nofilter_reserve         |  121.65 |  32.48x | 3147.77% |          8.05 |       0.03x |     -96.92% |
| ryml_yaml_arena_nofilter                 |   87.52 |  23.37x | 2236.52% |         11.19 |       0.04x |     -95.72% |
| ryml_yaml_arena_reserve                  |  121.38 |  32.41x | 3140.54% |          8.07 |       0.03x |     -96.91% |
| ryml_yaml_arena                          |   88.29 |  23.57x | 2257.07% |         11.09 |       0.04x |     -95.76% |
| libyaml_arena                            |   28.07 |   7.49x |  649.33% |         34.88 |       0.13x |     -86.65% |
| libyaml_arena_reuse                      |   28.85 |   7.70x |  670.13% |         33.94 |       0.13x |     -87.02% |
| libfyaml_arena                           |    3.98 |   1.06x |    6.22% |        246.06 |       0.94x |      -5.86% |
| yamlcpp_arena                            |    3.75 |   1.00x |    0.00% |        261.37 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+-------------------------------------------------------------------------------------------------------+
|                           emit benchmark: style_maps_blck_outer1000_inner100                          |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  401.91 |  59.37x | 5837.27% |          2.44 |       0.02x |     -98.32% |
| ryml_yaml_json_str_reserve |  249.11 |  36.80x | 3579.95% |          3.93 |       0.03x |     -97.28% |
| ryml_yaml_str              |  403.31 |  59.58x | 5858.01% |          2.43 |       0.02x |     -98.32% |
| ryml_yaml_json_str         |  247.74 |  36.60x | 3559.81% |          3.95 |       0.03x |     -97.27% |
| ryml_yaml_ostream          |   51.15 |   7.56x |  655.61% |         19.14 |       0.13x |     -86.77% |
| ryml_yaml_json_ostream     |   53.97 |   7.97x |  697.32% |         18.14 |       0.13x |     -87.46% |
| fyaml_str_reserve          |   26.00 |   3.84x |  284.03% |         37.66 |       0.26x |     -73.96% |
| fyaml_str                  |   26.18 |   3.87x |  286.79% |         37.39 |       0.26x |     -74.15% |
| fyaml_ostream              |   23.58 |   3.48x |  248.27% |         41.53 |       0.29x |     -71.29% |
| yamlcpp_                   |    6.77 |   1.00x |    0.00% |        144.62 |       1.00x |       0.00% |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
style_maps_flow_outer1000_inner100_json
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100_json.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100_json.json
+---------------------------------------------------------------------------------------------------------------------+
|                               parse benchmark: style_maps_flow_outer1000_inner100_json                              |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  119.95 |  47.48x | 4648.48% |          6.53 |       0.02x |     -97.89% |
| ryml_yaml_inplace_reuse_nofilter         |  120.25 |  47.60x | 4660.21% |          6.51 |       0.02x |     -97.90% |
| ryml_yaml_inplace_reuse_reserve          |  119.61 |  47.35x | 4634.90% |          6.55 |       0.02x |     -97.89% |
| ryml_yaml_inplace_reuse                  |  120.12 |  47.55x | 4654.94% |          6.52 |       0.02x |     -97.90% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  118.23 |  46.80x | 4580.39% |          6.62 |       0.02x |     -97.86% |
| ryml_yaml_arena_reuse_nofilter           |   49.41 |  19.56x | 1855.78% |         15.85 |       0.05x |     -94.89% |
| ryml_yaml_arena_reuse_reserve            |  118.44 |  46.89x | 4588.58% |          6.61 |       0.02x |     -97.87% |
| ryml_yaml_arena_reuse                    |  118.23 |  46.80x | 4580.18% |          6.62 |       0.02x |     -97.86% |
| ryml_yaml_inplace_nofilter_reserve       |  119.39 |  47.26x | 4626.20% |          6.56 |       0.02x |     -97.88% |
| ryml_yaml_inplace_nofilter               |   81.78 |  32.37x | 3137.23% |          9.57 |       0.03x |     -96.91% |
| ryml_yaml_inplace_reserve                |  119.84 |  47.44x | 4643.83% |          6.53 |       0.02x |     -97.89% |
| ryml_yaml_inplace                        |   82.35 |  32.60x | 3159.81% |          9.51 |       0.03x |     -96.93% |
| ryml_yaml_arena_nofilter_reserve         |  118.32 |  46.84x | 4583.73% |          6.62 |       0.02x |     -97.86% |
| ryml_yaml_arena_nofilter                 |   81.43 |  32.23x | 3123.34% |          9.62 |       0.03x |     -96.90% |
| ryml_yaml_arena_reserve                  |  117.97 |  46.70x | 4570.07% |          6.64 |       0.02x |     -97.86% |
| ryml_yaml_arena                          |   81.41 |  32.23x | 3122.86% |          9.62 |       0.03x |     -96.90% |
| libyaml_arena                            |   20.65 |   8.17x |  717.30% |         37.92 |       0.12x |     -87.76% |
| libyaml_arena_reuse                      |   20.63 |   8.16x |  716.50% |         37.96 |       0.12x |     -87.75% |
| libfyaml_arena                           |    3.50 |   1.39x |   38.74% |        223.42 |       0.72x |     -27.92% |
| yamlcpp_arena                            |    2.53 |   1.00x |    0.00% |        309.96 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100_json-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100_json-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100_json-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100_json-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100_json-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner100_json-cpu_time_ms.html
Built target ryml-bm-parse-style_maps_flow_outer1000_inner100_json-plot
/usr/bin/python3.11
Python 3.11.8
emit
style_maps_flow_outer1000_inner100_json
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100_json.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100_json.json
+-------------------------------------------------------------------------------------------------------+
|                        emit benchmark: style_maps_flow_outer1000_inner100_json                        |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  330.05 |  68.95x | 6794.85% |          2.37 |       0.01x |     -98.55% |
| ryml_yaml_json_str_reserve |  197.98 |  41.36x | 4035.92% |          3.95 |       0.02x |     -97.58% |
| ryml_yaml_str              |  330.49 |  69.04x | 6804.00% |          2.37 |       0.01x |     -98.55% |
| ryml_yaml_json_str         |  198.20 |  41.40x | 4040.46% |          3.95 |       0.02x |     -97.58% |
| ryml_yaml_ostream          |   43.35 |   9.06x |  805.56% |         18.06 |       0.11x |     -88.96% |
| ryml_yaml_json_ostream     |   42.93 |   8.97x |  796.82% |         18.24 |       0.11x |     -88.85% |
| fyaml_str_reserve          |   27.42 |   5.73x |  472.85% |         28.55 |       0.17x |     -82.54% |
| fyaml_str                  |   26.99 |   5.64x |  463.74% |         29.02 |       0.18x |     -82.26% |
| fyaml_ostream              |   22.25 |   4.65x |  364.71% |         35.20 |       0.22x |     -78.48% |
| yamlcpp_                   |    4.79 |   1.00x |    0.00% |        163.57 |       1.00x |       0.00% |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100_json-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100_json-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100_json-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100_json-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100_json-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_maps_flow_outer1000_inner100_json-cpu_time_ms.html
Built target ryml-bm-emit-style_maps_flow_outer1000_inner100_json-plot
/usr/bin/python3.11
Python 3.11.8
parse
style_maps_flow_outer1000_inner10
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_maps_flow_outer1000_inner10.json
+---------------------------------------------------------------------------------------------------------------------+
|                                  parse benchmark: style_maps_flow_outer1000_inner10                                 |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  108.48 |  36.05x | 3505.47% |          0.67 |       0.03x |     -97.23% |
| ryml_yaml_inplace_reuse_nofilter         |  108.72 |  36.14x | 3513.58% |          0.67 |       0.03x |     -97.23% |
| ryml_yaml_inplace_reuse_reserve          |  108.74 |  36.14x | 3514.29% |          0.67 |       0.03x |     -97.23% |
| ryml_yaml_inplace_reuse                  |  109.07 |  36.25x | 3525.20% |          0.67 |       0.03x |     -97.24% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  107.84 |  35.84x | 3484.27% |          0.68 |       0.03x |     -97.21% |
| ryml_yaml_arena_reuse_nofilter           |   41.01 |  13.63x | 1262.92% |          1.78 |       0.07x |     -92.66% |
| ryml_yaml_arena_reuse_reserve            |  108.10 |  35.93x | 3492.90% |          0.67 |       0.03x |     -97.22% |
| ryml_yaml_arena_reuse                    |  108.01 |  35.90x | 3490.05% |          0.67 |       0.03x |     -97.21% |
| ryml_yaml_inplace_nofilter_reserve       |  101.72 |  33.81x | 3280.91% |          0.72 |       0.03x |     -97.04% |
| ryml_yaml_inplace_nofilter               |   41.20 |  13.69x | 1269.24% |          1.77 |       0.07x |     -92.70% |
| ryml_yaml_inplace_reserve                |  109.05 |  36.24x | 3524.47% |          0.67 |       0.03x |     -97.24% |
| ryml_yaml_inplace                        |   41.01 |  13.63x | 1262.99% |          1.78 |       0.07x |     -92.66% |
| ryml_yaml_arena_nofilter_reserve         |  106.41 |  35.37x | 3436.65% |          0.69 |       0.03x |     -97.17% |
| ryml_yaml_arena_nofilter                 |   41.33 |  13.74x | 1273.73% |          1.76 |       0.07x |     -92.72% |
| ryml_yaml_arena_reserve                  |  108.10 |  35.93x | 3492.88% |          0.67 |       0.03x |     -97.22% |
| ryml_yaml_arena                          |   41.33 |  13.74x | 1273.82% |          1.76 |       0.07x |     -92.72% |
| libyaml_arena                            |   15.76 |   5.24x |  423.76% |          4.63 |       0.19x |     -80.91% |
| libyaml_arena_reuse                      |   19.71 |   6.55x |  555.09% |          3.70 |       0.15x |     -84.73% |
| libfyaml_arena                           |    9.07 |   3.02x |  201.61% |          8.03 |       0.33x |     -66.84% |
| yamlcpp_arena                            |    3.01 |   1.00x |    0.00% |         24.23 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+-------------------------------------------------------------------------------------------------------+
|                           emit benchmark: style_maps_flow_outer1000_inner10                           |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  306.38 |  58.52x | 5752.38% |          0.24 |       0.02x |     -98.29% |
| ryml_yaml_json_str_reserve |  183.56 |  35.06x | 3406.21% |          0.40 |       0.03x |     -97.15% |
| ryml_yaml_str              |  302.39 |  57.76x | 5676.07% |          0.24 |       0.02x |     -98.27% |
| ryml_yaml_json_str         |  183.75 |  35.10x | 3409.93% |          0.40 |       0.03x |     -97.15% |
| ryml_yaml_ostream          |   13.60 |   2.60x |  159.76% |          5.36 |       0.38x |     -61.50% |
| ryml_yaml_json_ostream     |   11.83 |   2.26x |  126.06% |          6.16 |       0.44x |     -55.76% |
| fyaml_str_reserve          |   35.70 |   6.82x |  581.94% |          2.04 |       0.15x |     -85.34% |
| fyaml_str                  |   35.51 |   6.78x |  578.27% |          2.05 |       0.15x |     -85.26% |
| fyaml_ostream              |   18.39 |   3.51x |  251.32% |          3.96 |       0.28x |     -71.54% |
| yamlcpp_                   |    5.24 |   1.00x |    0.00% |         13.93 |       1.00x |       0.00% |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+---------------------------------------------------------------------------------------------------------------------+
|                                 parse benchmark: style_maps_flow_outer1000_inner100                                 |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  116.83 |  40.66x | 3965.58% |          6.70 |       0.02x |     -97.54% |
| ryml_yaml_inplace_reuse_nofilter         |  118.73 |  41.32x | 4031.82% |          6.59 |       0.02x |     -97.58% |
| ryml_yaml_inplace_reuse_reserve          |  120.25 |  41.85x | 4084.67% |          6.51 |       0.02x |     -97.61% |
| ryml_yaml_inplace_reuse                  |  120.18 |  41.82x | 4082.24% |          6.52 |       0.02x |     -97.61% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  117.88 |  41.02x | 4002.07% |          6.64 |       0.02x |     -97.56% |
| ryml_yaml_arena_reuse_nofilter           |   49.76 |  17.31x | 1631.47% |         15.74 |       0.06x |     -94.22% |
| ryml_yaml_arena_reuse_reserve            |  118.38 |  41.20x | 4019.50% |          6.61 |       0.02x |     -97.57% |
| ryml_yaml_arena_reuse                    |  116.65 |  40.59x | 3959.42% |          6.71 |       0.02x |     -97.54% |
| ryml_yaml_inplace_nofilter_reserve       |  118.77 |  41.33x | 4033.20% |          6.59 |       0.02x |     -97.58% |
| ryml_yaml_inplace_nofilter               |   82.05 |  28.55x | 2755.16% |          9.54 |       0.04x |     -96.50% |
| ryml_yaml_inplace_reserve                |  119.89 |  41.72x | 4072.24% |          6.53 |       0.02x |     -97.60% |
| ryml_yaml_inplace                        |   82.32 |  28.65x | 2764.63% |          9.51 |       0.03x |     -96.51% |
| ryml_yaml_arena_nofilter_reserve         |  118.27 |  41.16x | 4015.83% |          6.62 |       0.02x |     -97.57% |
| ryml_yaml_arena_nofilter                 |   81.33 |  28.30x | 2730.40% |          9.63 |       0.04x |     -96.47% |
| ryml_yaml_arena_reserve                  |  118.53 |  41.25x | 4024.71% |          6.61 |       0.02x |     -97.58% |
| ryml_yaml_arena                          |   81.20 |  28.26x | 2725.60% |          9.64 |       0.04x |     -96.46% |
| libyaml_arena                            |   21.13 |   7.35x |  635.31% |         37.06 |       0.14x |     -86.40% |
| libyaml_arena_reuse                      |   21.73 |   7.56x |  656.19% |         36.03 |       0.13x |     -86.78% |
| libfyaml_arena                           |    3.40 |   1.18x |   18.35% |        230.24 |       0.84x |     -15.50% |
| yamlcpp_arena                            |    2.87 |   1.00x |    0.00% |        272.48 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+-------------------------------------------------------------------------------------------------------+
|                           emit benchmark: style_maps_flow_outer1000_inner100                          |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  340.97 |  70.86x | 6986.30% |          2.30 |       0.01x |     -98.59% |
| ryml_yaml_json_str_reserve |  199.37 |  41.43x | 4043.48% |          3.93 |       0.02x |     -97.59% |
| ryml_yaml_str              |  338.12 |  70.27x | 6927.19% |          2.32 |       0.01x |     -98.58% |
| ryml_yaml_json_str         |  198.17 |  41.19x | 4018.52% |          3.95 |       0.02x |     -97.57% |
| ryml_yaml_ostream          |   43.47 |   9.03x |  803.33% |         18.01 |       0.11x |     -88.93% |
| ryml_yaml_json_ostream     |   43.31 |   9.00x |  800.10% |         18.08 |       0.11x |     -88.89% |
| fyaml_str_reserve          |   27.94 |   5.81x |  480.62% |         28.03 |       0.17x |     -82.78% |
| fyaml_str                  |   27.53 |   5.72x |  472.06% |         28.45 |       0.17x |     -82.52% |
| fyaml_ostream              |   22.49 |   4.68x |  367.51% |         34.81 |       0.21x |     -78.61% |
| yamlcpp_                   |    4.81 |   1.00x |    0.00% |        162.73 |       1.00x |       0.00% |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+---------------------------------------------------------------------------------------------------------------------+
|                                  parse benchmark: style_seqs_blck_outer1000_inner10                                 |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  127.78 |  25.56x | 2455.80% |          0.62 |       0.04x |     -96.09% |
| ryml_yaml_inplace_reuse_nofilter         |  126.91 |  25.38x | 2438.45% |          0.62 |       0.04x |     -96.06% |
| ryml_yaml_inplace_reuse_reserve          |  128.15 |  25.63x | 2463.25% |          0.62 |       0.04x |     -96.10% |
| ryml_yaml_inplace_reuse                  |  128.16 |  25.64x | 2463.57% |          0.62 |       0.04x |     -96.10% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  125.86 |  25.18x | 2417.56% |          0.63 |       0.04x |     -96.03% |
| ryml_yaml_arena_reuse_nofilter           |   37.71 |   7.54x |  654.30% |          2.09 |       0.13x |     -86.74% |
| ryml_yaml_arena_reuse_reserve            |  127.32 |  25.47x | 2446.75% |          0.62 |       0.04x |     -96.07% |
| ryml_yaml_arena_reuse                    |  127.40 |  25.48x | 2448.32% |          0.62 |       0.04x |     -96.08% |
| ryml_yaml_inplace_nofilter_reserve       |  128.13 |  25.63x | 2462.93% |          0.62 |       0.04x |     -96.10% |
| ryml_yaml_inplace_nofilter               |   38.39 |   7.68x |  667.97% |          2.06 |       0.13x |     -86.98% |
| ryml_yaml_inplace_reserve                |  128.34 |  25.67x | 2467.10% |          0.61 |       0.04x |     -96.10% |
| ryml_yaml_inplace                        |   38.48 |   7.70x |  669.68% |          2.05 |       0.13x |     -87.01% |
| ryml_yaml_arena_nofilter_reserve         |  127.11 |  25.42x | 2442.44% |          0.62 |       0.04x |     -96.07% |
| ryml_yaml_arena_nofilter                 |   38.46 |   7.69x |  669.29% |          2.05 |       0.13x |     -87.00% |
| ryml_yaml_arena_reserve                  |  126.43 |  25.29x | 2428.89% |          0.62 |       0.04x |     -96.05% |
| ryml_yaml_arena                          |   38.49 |   7.70x |  669.95% |          2.05 |       0.13x |     -87.01% |
| libyaml_arena                            |   23.60 |   4.72x |  372.05% |          3.34 |       0.21x |     -78.82% |
| libyaml_arena_reuse                      |   37.77 |   7.55x |  655.45% |          2.09 |       0.13x |     -86.76% |
| libfyaml_arena                           |   22.57 |   4.51x |  351.38% |          3.50 |       0.22x |     -77.85% |
| yamlcpp_arena                            |    5.00 |   1.00x |    0.00% |         15.78 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+-------------------------------------------------------------------------------------------------------+
|                           emit benchmark: style_seqs_blck_outer1000_inner10                           |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  518.29 |  51.96x | 5095.59% |          0.15 |       0.02x |     -98.08% |
| ryml_yaml_json_str_reserve |  265.48 |  26.61x | 2561.26% |          0.30 |       0.04x |     -96.24% |
| ryml_yaml_str              |  514.69 |  51.59x | 5059.49% |          0.15 |       0.02x |     -98.06% |
| ryml_yaml_json_str         |  256.47 |  25.71x | 2470.98% |          0.31 |       0.04x |     -96.11% |
| ryml_yaml_ostream          |   13.90 |   1.39x |   39.39% |          5.67 |       0.72x |     -28.26% |
| ryml_yaml_json_ostream     |   16.20 |   1.62x |   62.43% |          4.87 |       0.62x |     -38.43% |
| fyaml_str_reserve          |   29.40 |   2.95x |  194.69% |          2.68 |       0.34x |     -66.07% |
| fyaml_str                  |   29.79 |   2.99x |  198.62% |          2.65 |       0.33x |     -66.51% |
| fyaml_ostream              |   21.02 |   2.11x |  110.76% |          3.75 |       0.47x |     -52.55% |
| yamlcpp_                   |    9.98 |   1.00x |    0.00% |          7.91 |       1.00x |       0.00% |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+---------------------------------------------------------------------------------------------------------------------+
|                                 parse benchmark: style_seqs_blck_outer1000_inner100                                 |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  133.26 |  30.18x | 2918.24% |          5.92 |       0.03x |     -96.69% |
| ryml_yaml_inplace_reuse_nofilter         |  133.10 |  30.15x | 2914.55% |          5.93 |       0.03x |     -96.68% |
| ryml_yaml_inplace_reuse_reserve          |  132.94 |  30.11x | 2911.03% |          5.93 |       0.03x |     -96.68% |
| ryml_yaml_inplace_reuse                  |  133.42 |  30.22x | 2921.74% |          5.91 |       0.03x |     -96.69% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  130.21 |  29.49x | 2849.03% |          6.06 |       0.03x |     -96.61% |
| ryml_yaml_arena_reuse_nofilter           |   52.31 |  11.85x | 1084.83% |         15.08 |       0.08x |     -91.56% |
| ryml_yaml_arena_reuse_reserve            |  131.10 |  29.69x | 2869.35% |          6.02 |       0.03x |     -96.63% |
| ryml_yaml_arena_reuse                    |  131.26 |  29.73x | 2872.83% |          6.01 |       0.03x |     -96.64% |
| ryml_yaml_inplace_nofilter_reserve       |  133.16 |  30.16x | 2916.05% |          5.93 |       0.03x |     -96.68% |
| ryml_yaml_inplace_nofilter               |   88.38 |  20.02x | 1901.77% |          8.93 |       0.05x |     -95.00% |
| ryml_yaml_inplace_reserve                |  133.52 |  30.24x | 2924.06% |          5.91 |       0.03x |     -96.69% |
| ryml_yaml_inplace                        |   88.43 |  20.03x | 1902.77% |          8.92 |       0.05x |     -95.01% |
| ryml_yaml_arena_nofilter_reserve         |  131.41 |  29.76x | 2876.36% |          6.00 |       0.03x |     -96.64% |
| ryml_yaml_arena_nofilter                 |   87.02 |  19.71x | 1871.02% |          9.07 |       0.05x |     -94.93% |
| ryml_yaml_arena_reserve                  |  131.45 |  29.77x | 2877.13% |          6.00 |       0.03x |     -96.64% |
| ryml_yaml_arena                          |   87.17 |  19.74x | 1874.34% |          9.05 |       0.05x |     -94.94% |
| libyaml_arena                            |   37.78 |   8.56x |  755.62% |         20.89 |       0.12x |     -88.31% |
| libyaml_arena_reuse                      |   39.11 |   8.86x |  785.89% |         20.17 |       0.11x |     -88.71% |
| libfyaml_arena                           |   21.78 |   4.93x |  393.34% |         36.22 |       0.20x |     -79.73% |
| yamlcpp_arena                            |    4.42 |   1.00x |    0.00% |        178.70 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+-------------------------------------------------------------------------------------------------------+
|                           emit benchmark: style_seqs_blck_outer1000_inner100                          |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  561.34 |  57.07x | 5607.34% |          1.41 |       0.02x |     -98.25% |
| ryml_yaml_json_str_reserve |  270.97 |  27.55x | 2655.02% |          2.91 |       0.04x |     -96.37% |
| ryml_yaml_str              |  550.84 |  56.01x | 5500.60% |          1.43 |       0.02x |     -98.21% |
| ryml_yaml_json_str         |  270.53 |  27.51x | 2650.56% |          2.92 |       0.04x |     -96.36% |
| ryml_yaml_ostream          |   41.93 |   4.26x |  326.36% |         18.82 |       0.23x |     -76.55% |
| ryml_yaml_json_ostream     |  109.42 |  11.12x | 1012.49% |          7.21 |       0.09x |     -91.01% |
| fyaml_str_reserve          |   28.81 |   2.93x |  192.89% |         27.39 |       0.34x |     -65.86% |
| fyaml_str                  |   28.81 |   2.93x |  192.94% |         27.38 |       0.34x |     -65.86% |
| fyaml_ostream              |   24.79 |   2.52x |  152.08% |         31.82 |       0.40x |     -60.33% |
| yamlcpp_                   |    9.84 |   1.00x |    0.00% |         80.22 |       1.00x |       0.00% |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+---------------------------------------------------------------------------------------------------------------------+
|                                  parse benchmark: style_seqs_flow_outer1000_inner10                                 |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  119.18 |  34.80x | 3380.30% |          0.36 |       0.03x |     -97.13% |
| ryml_yaml_inplace_reuse_nofilter         |  119.60 |  34.93x | 3392.78% |          0.36 |       0.03x |     -97.14% |
| ryml_yaml_inplace_reuse_reserve          |  120.06 |  35.06x | 3406.09% |          0.36 |       0.03x |     -97.15% |
| ryml_yaml_inplace_reuse                  |  119.50 |  34.90x | 3389.84% |          0.36 |       0.03x |     -97.13% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  117.94 |  34.44x | 3344.13% |          0.36 |       0.03x |     -97.10% |
| ryml_yaml_arena_reuse_nofilter           |   24.13 |   7.05x |  604.73% |          1.78 |       0.14x |     -85.81% |
| ryml_yaml_arena_reuse_reserve            |  118.17 |  34.51x | 3351.06% |          0.36 |       0.03x |     -97.10% |
| ryml_yaml_arena_reuse                    |  118.97 |  34.74x | 3374.30% |          0.36 |       0.03x |     -97.12% |
| ryml_yaml_inplace_nofilter_reserve       |  119.62 |  34.93x | 3393.37% |          0.36 |       0.03x |     -97.14% |
| ryml_yaml_inplace_nofilter               |   24.79 |   7.24x |  623.80% |          1.73 |       0.14x |     -86.18% |
| ryml_yaml_inplace_reserve                |  120.19 |  35.10x | 3409.84% |          0.36 |       0.03x |     -97.15% |
| ryml_yaml_inplace                        |   25.02 |   7.31x |  630.79% |          1.71 |       0.14x |     -86.32% |
| ryml_yaml_arena_nofilter_reserve         |  119.41 |  34.87x | 3387.16% |          0.36 |       0.03x |     -97.13% |
| ryml_yaml_arena_nofilter                 |   24.91 |   7.28x |  627.58% |          1.72 |       0.14x |     -86.26% |
| ryml_yaml_arena_reserve                  |  119.08 |  34.77x | 3377.48% |          0.36 |       0.03x |     -97.12% |
| ryml_yaml_arena                          |   25.01 |   7.30x |  630.38% |          1.72 |       0.14x |     -86.31% |
| libyaml_arena                            |   13.25 |   3.87x |  287.05% |          3.24 |       0.26x |     -74.16% |
| libyaml_arena_reuse                      |   20.92 |   6.11x |  511.06% |          2.05 |       0.16x |     -83.63% |
| libfyaml_arena                           |   14.70 |   4.29x |  329.38% |          2.92 |       0.23x |     -76.71% |
| yamlcpp_arena                            |    3.42 |   1.00x |    0.00% |         12.53 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
+-------------------------------------------------------------------------------------------------------+
|                           emit benchmark: style_seqs_flow_outer1000_inner10                           |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  312.10 |  58.93x | 5793.24% |          0.14 |       0.02x |     -98.30% |
| ryml_yaml_json_str_reserve |  144.31 |  27.25x | 2625.08% |          0.30 |       0.04x |     -96.33% |
| ryml_yaml_str              |  311.40 |  58.80x | 5780.19% |          0.14 |       0.02x |     -98.30% |
| ryml_yaml_json_str         |  144.18 |  27.23x | 2622.60% |          0.30 |       0.04x |     -96.33% |
| ryml_yaml_ostream          |    8.18 |   1.54x |   54.49% |          5.24 |       0.65x |     -35.27% |
| ryml_yaml_json_ostream     |    9.81 |   1.85x |   85.27% |          4.37 |       0.54x |     -46.02% |
| fyaml_str_reserve          |   36.31 |   6.86x |  585.55% |          1.18 |       0.15x |     -85.41% |
| fyaml_str                  |   36.32 |   6.86x |  585.75% |          1.18 |       0.15x |     -85.42% |
| fyaml_ostream              |   13.33 |   2.52x |  151.72% |          3.22 |       0.40x |     -60.27% |
| yamlcpp_                   |    5.30 |   1.00x |    0.00% |          8.10 |       1.00x |       0.00% |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
style_seqs_flow_outer1000_inner10_json
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10_json.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10_json.json
+----------------------------------------------------------------------------------------------------------------------+
|                               parse benchmark: style_seqs_flow_outer1000_inner10_json                                |
+------------------------------------------+---------+---------+-----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |   MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+-----------+---------------+-------------+-------------+
| ryml_json_inplace_reuse_nofilter_reserve |  139.35 |  42.91x |  4190.71% |          0.31 |       0.02x |     -97.67% |
| ryml_json_inplace_reuse_nofilter         |  136.01 |  41.88x |  4087.84% |          0.32 |       0.02x |     -97.61% |
| ryml_json_inplace_reuse_reserve          |  138.77 |  42.73x |  4172.69% |          0.31 |       0.02x |     -97.66% |
| ryml_json_inplace_reuse                  |  139.66 |  43.00x |  4199.99% |          0.31 |       0.02x |     -97.67% |
| ryml_yaml_inplace_reuse_nofilter_reserve |  118.92 |  36.62x |  3561.58% |          0.36 |       0.03x |     -97.27% |
| ryml_yaml_inplace_reuse_nofilter         |  122.36 |  37.67x |  3667.48% |          0.35 |       0.03x |     -97.35% |
| ryml_yaml_inplace_reuse_reserve          |  124.02 |  38.19x |  3718.57% |          0.35 |       0.03x |     -97.38% |
| ryml_yaml_inplace_reuse                  |  124.56 |  38.35x |  3735.21% |          0.34 |       0.03x |     -97.39% |
| ryml_json_arena_reuse_nofilter_reserve   |  138.43 |  42.62x |  4162.33% |          0.31 |       0.02x |     -97.65% |
| ryml_json_arena_reuse_nofilter           |  138.89 |  42.76x |  4176.46% |          0.31 |       0.02x |     -97.66% |
| ryml_json_arena_reuse_reserve            |  138.78 |  42.73x |  4173.01% |          0.31 |       0.02x |     -97.66% |
| ryml_json_arena_reuse                    |  137.81 |  42.43x |  4143.03% |          0.31 |       0.02x |     -97.64% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  123.75 |  38.10x |  3710.36% |          0.35 |       0.03x |     -97.38% |
| ryml_yaml_arena_reuse_nofilter           |   23.39 |   7.20x |   620.15% |          1.83 |       0.14x |     -86.11% |
| ryml_yaml_arena_reuse_reserve            |  123.73 |  38.10x |  3709.74% |          0.35 |       0.03x |     -97.38% |
| ryml_yaml_arena_reuse                    |  122.65 |  37.76x |  3676.45% |          0.35 |       0.03x |     -97.35% |
| ryml_json_inplace_nofilter_reserve       |  139.35 |  42.91x |  4190.59% |          0.31 |       0.02x |     -97.67% |
| ryml_json_inplace_nofilter               |   69.19 |  21.30x |  2030.32% |          0.62 |       0.05x |     -95.31% |
| ryml_json_inplace_reserve                |  139.69 |  43.01x |  4200.96% |          0.31 |       0.02x |     -97.67% |
| ryml_json_inplace                        |   69.72 |  21.47x |  2046.64% |          0.62 |       0.05x |     -95.34% |
| ryml_yaml_inplace_nofilter_reserve       |  123.44 |  38.01x |  3700.76% |          0.35 |       0.03x |     -97.37% |
| ryml_yaml_inplace_nofilter               |   65.55 |  20.18x |  1918.33% |          0.65 |       0.05x |     -95.05% |
| ryml_yaml_inplace_reserve                |  124.93 |  38.47x |  3746.58% |          0.34 |       0.03x |     -97.40% |
| ryml_yaml_inplace                        |   64.58 |  19.89x |  1888.55% |          0.66 |       0.05x |     -94.97% |
| ryml_json_arena_nofilter_reserve         |  137.59 |  42.36x |  4136.36% |          0.31 |       0.02x |     -97.64% |
| ryml_json_arena_nofilter                 |   68.70 |  21.15x |  2015.30% |          0.62 |       0.05x |     -95.27% |
| ryml_json_arena_reserve                  |  138.21 |  42.56x |  4155.59% |          0.31 |       0.02x |     -97.65% |
| ryml_json_arena                          |   68.89 |  21.21x |  2021.26% |          0.62 |       0.05x |     -95.29% |
| ryml_yaml_arena_nofilter_reserve         |  122.65 |  37.76x |  3676.29% |          0.35 |       0.03x |     -97.35% |
| ryml_yaml_arena_nofilter                 |   65.03 |  20.02x |  1902.38% |          0.66 |       0.05x |     -95.01% |
| ryml_yaml_arena_reserve                  |  123.24 |  37.95x |  3694.53% |          0.35 |       0.03x |     -97.36% |
| ryml_yaml_arena                          |   65.79 |  20.26x |  1925.72% |          0.65 |       0.05x |     -95.06% |
| libyaml_arena                            |   19.45 |   5.99x |   498.77% |          2.21 |       0.17x |     -83.30% |
| libyaml_arena_reuse                      |   20.69 |   6.37x |   537.20% |          2.07 |       0.16x |     -84.31% |
| libfyaml_arena                           |   14.86 |   4.57x |   357.42% |          2.89 |       0.22x |     -78.14% |
| yamlcpp_arena                            |    3.25 |   1.00x |     0.00% |         13.21 |       1.00x |       0.00% |
| rapidjson_arena                          |  366.76 | 112.93x | 11192.52% |          0.12 |       0.01x |     -99.11% |
| rapidjson_inplace                        |  319.05 |  98.24x |  9723.58% |          0.13 |       0.01x |     -98.98% |
| sajson_arena                             |  214.46 |  66.03x |  6503.32% |          0.20 |       0.02x |     -98.49% |
| sajson_inplace                           |  212.62 |  65.46x |  6446.48% |          0.20 |       0.02x |     -98.47% |
| jsoncpp_arena                            |   37.81 |  11.64x |  1064.04% |          1.13 |       0.09x |     -91.41% |
| nlohmann_arena                           |   51.72 |  15.92x |  1492.40% |          0.83 |       0.06x |     -93.72% |
+------------------------------------------+---------+---------+-----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10_json-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10_json-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10_json-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10_json-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10_json-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner10_json-cpu_time_ms.html
Built target ryml-bm-parse-style_seqs_flow_outer1000_inner10_json-plot
/usr/bin/python3.11
Python 3.11.8
emit
style_seqs_flow_outer1000_inner10_json
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10_json.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10_json.json
+-------------------------------------------------------------------------------------------------------+
|                         emit benchmark: style_seqs_flow_outer1000_inner10_json                        |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  308.40 |  61.44x | 6043.57% |          0.14 |       0.02x |     -98.37% |
| ryml_yaml_json_str_reserve |  143.46 |  28.58x | 2757.82% |          0.30 |       0.03x |     -96.50% |
| ryml_yaml_str              |  310.78 |  61.91x | 6091.09% |          0.14 |       0.02x |     -98.38% |
| ryml_yaml_json_str         |  144.35 |  28.76x | 2775.65% |          0.30 |       0.03x |     -96.52% |
| ryml_yaml_ostream          |    7.59 |   1.51x |   51.22% |          5.65 |       0.66x |     -33.87% |
| ryml_yaml_json_ostream     |    9.57 |   1.91x |   90.65% |          4.48 |       0.52x |     -47.55% |
| fyaml_str_reserve          |   35.73 |   7.12x |  611.85% |          1.20 |       0.14x |     -85.95% |
| fyaml_str                  |   36.32 |   7.24x |  623.60% |          1.18 |       0.14x |     -86.18% |
| fyaml_ostream              |   13.18 |   2.62x |  162.49% |          3.26 |       0.38x |     -61.90% |
| yamlcpp_                   |    5.02 |   1.00x |    0.00% |          8.55 |       1.00x |       0.00% |
| rapidjson_                 |  437.20 |  87.10x | 8609.55% |          0.10 |       0.01x |     -98.85% |
| jsoncpp_                   |   12.32 |   2.45x |  145.38% |          3.48 |       0.41x |     -59.25% |
| nlohmann_                  |   13.89 |   2.77x |  176.71% |          3.09 |       0.36x |     -63.86% |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10_json-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10_json-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10_json-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10_json-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10_json-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner10_json-cpu_time_ms.html
Built target ryml-bm-emit-style_seqs_flow_outer1000_inner10_json-plot
/usr/bin/python3.11
Python 3.11.8
parse
style_seqs_flow_outer1000_inner100
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100.json
+---------------------------------------------------------------------------------------------------------------------+
|                                 parse benchmark: style_seqs_flow_outer1000_inner100                                 |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_inplace_reuse_nofilter_reserve |  121.69 |  45.34x | 4434.12% |          3.23 |       0.02x |     -97.79% |
| ryml_yaml_inplace_reuse_nofilter         |  121.55 |  45.29x | 4429.06% |          3.23 |       0.02x |     -97.79% |
| ryml_yaml_inplace_reuse_reserve          |  120.78 |  45.00x | 4400.36% |          3.25 |       0.02x |     -97.78% |
| ryml_yaml_inplace_reuse                  |  120.70 |  44.97x | 4397.39% |          3.26 |       0.02x |     -97.78% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  118.90 |  44.30x | 4330.31% |          3.31 |       0.02x |     -97.74% |
| ryml_yaml_arena_reuse_nofilter           |   31.53 |  11.75x | 1074.75% |         12.47 |       0.09x |     -91.49% |
| ryml_yaml_arena_reuse_reserve            |  119.68 |  44.59x | 4359.33% |          3.28 |       0.02x |     -97.76% |
| ryml_yaml_arena_reuse                    |  120.49 |  44.89x | 4389.43% |          3.26 |       0.02x |     -97.77% |
| ryml_yaml_inplace_nofilter_reserve       |  121.19 |  45.16x | 4415.75% |          3.24 |       0.02x |     -97.79% |
| ryml_yaml_inplace_nofilter               |   64.50 |  24.03x | 2303.40% |          6.09 |       0.04x |     -95.84% |
| ryml_yaml_inplace_reserve                |  121.06 |  45.11x | 4410.80% |          3.25 |       0.02x |     -97.78% |
| ryml_yaml_inplace                        |   64.34 |  23.97x | 2297.49% |          6.11 |       0.04x |     -95.83% |
| ryml_yaml_arena_nofilter_reserve         |  120.09 |  44.74x | 4374.48% |          3.27 |       0.02x |     -97.77% |
| ryml_yaml_arena_nofilter                 |   64.08 |  23.88x | 2287.63% |          6.13 |       0.04x |     -95.81% |
| ryml_yaml_arena_reserve                  |  120.28 |  44.82x | 4381.68% |          3.27 |       0.02x |     -97.77% |
| ryml_yaml_arena                          |   64.22 |  23.93x | 2293.07% |          6.12 |       0.04x |     -95.82% |
| libyaml_arena                            |   19.53 |   7.28x |  627.74% |         20.12 |       0.14x |     -86.26% |
| libyaml_arena_reuse                      |   20.63 |   7.69x |  668.83% |         19.05 |       0.13x |     -86.99% |
| libfyaml_arena                           |   14.44 |   5.38x |  438.10% |         27.21 |       0.19x |     -81.42% |
| yamlcpp_arena                            |    2.68 |   1.00x |    0.00% |        146.44 |       1.00x |       0.00% |
+------------------------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
/usr/bin/python3.11
Python 3.11.8
emit
style_seqs_flow_outer1000_inner100
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-emit-style_seqs_flow_outer1000_inner100.json
+-------------------------------------------------------------------------------------------------------+
|                           emit benchmark: style_seqs_flow_outer1000_inner100                          |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| function                   |    MB/s | MB/s(x) |  MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+
| ryml_yaml_str_reserve      |  314.78 |  70.72x | 6972.01% |          1.25 |       0.01x |     -98.59% |
| ryml_yaml_json_str_reserve |  134.77 |  30.28x | 2927.71% |          2.92 |       0.03x |     -96.70% |
| ryml_yaml_str              |  314.82 |  70.73x | 6972.94% |          1.25 |       0.01x |     -98.59% |
| ryml_yaml_json_str         |  134.16 |  30.14x | 2914.15% |          2.93 |       0.03x |     -96.68% |
| ryml_yaml_ostream          |   53.42 |  12.00x | 1100.08% |          7.36 |       0.08x |     -91.67% |
| ryml_yaml_json_ostream     |   55.62 |  12.50x | 1149.67% |          7.07 |       0.08x |     -92.00% |
| fyaml_str_reserve          |   34.52 |   7.76x |  675.57% |         11.38 |       0.13x |     -87.11% |
| fyaml_str                  |   33.47 |   7.52x |  651.99% |         11.74 |       0.13x |     -86.70% |
| fyaml_ostream              |   22.66 |   5.09x |  409.03% |         17.35 |       0.20x |     -80.35% |
| yamlcpp_                   |    4.45 |   1.00x |    0.00% |         88.29 |       1.00x |       0.00% |
+----------------------------+---------+---------+----------+---------------+-------------+-------------+ 


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
style_seqs_flow_outer1000_inner100_json
['/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100_json.json']
jf: /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100_json.json
+----------------------------------------------------------------------------------------------------------------------+
|                               parse benchmark: style_seqs_flow_outer1000_inner100_json                               |
+------------------------------------------+---------+---------+-----------+---------------+-------------+-------------+
| function                                 |    MB/s | MB/s(x) |   MB/s(%) | cpu time (ms) | cpu time(x) | cpu time(%) |
+------------------------------------------+---------+---------+-----------+---------------+-------------+-------------+
| ryml_json_inplace_reuse_nofilter_reserve |  133.26 |  50.94x |  4994.48% |          2.95 |       0.02x |     -98.04% |
| ryml_json_inplace_reuse_nofilter         |  133.60 |  51.07x |  5007.45% |          2.94 |       0.02x |     -98.04% |
| ryml_json_inplace_reuse_reserve          |  133.48 |  51.03x |  5002.98% |          2.94 |       0.02x |     -98.04% |
| ryml_json_inplace_reuse                  |  133.27 |  50.95x |  4994.85% |          2.95 |       0.02x |     -98.04% |
| ryml_yaml_inplace_reuse_nofilter_reserve |  123.25 |  47.12x |  4611.89% |          3.19 |       0.02x |     -97.88% |
| ryml_yaml_inplace_reuse_nofilter         |  123.43 |  47.19x |  4618.53% |          3.18 |       0.02x |     -97.88% |
| ryml_yaml_inplace_reuse_reserve          |  123.03 |  47.04x |  4603.57% |          3.19 |       0.02x |     -97.87% |
| ryml_yaml_inplace_reuse                  |  123.04 |  47.04x |  4603.79% |          3.19 |       0.02x |     -97.87% |
| ryml_json_arena_reuse_nofilter_reserve   |  131.30 |  50.20x |  4919.74% |          2.99 |       0.02x |     -98.01% |
| ryml_json_arena_reuse_nofilter           |  129.60 |  49.55x |  4854.56% |          3.03 |       0.02x |     -97.98% |
| ryml_json_arena_reuse_reserve            |  128.63 |  49.17x |  4817.37% |          3.06 |       0.02x |     -97.97% |
| ryml_json_arena_reuse                    |  131.13 |  50.13x |  4913.04% |          3.00 |       0.02x |     -98.01% |
| ryml_yaml_arena_reuse_nofilter_reserve   |  121.50 |  46.45x |  4545.08% |          3.23 |       0.02x |     -97.85% |
| ryml_yaml_arena_reuse_nofilter           |   32.67 |  12.49x |  1148.91% |         12.03 |       0.08x |     -91.99% |
| ryml_yaml_arena_reuse_reserve            |  119.88 |  45.83x |  4482.82% |          3.28 |       0.02x |     -97.82% |
| ryml_yaml_arena_reuse                    |  121.00 |  46.26x |  4525.93% |          3.25 |       0.02x |     -97.84% |
| ryml_json_inplace_nofilter_reserve       |  132.56 |  50.68x |  4967.62% |          2.96 |       0.02x |     -98.03% |
| ryml_json_inplace_nofilter               |   66.50 |  25.42x |  2442.40% |          5.91 |       0.04x |     -96.07% |
| ryml_json_inplace_reserve                |  131.21 |  50.16x |  4916.11% |          3.00 |       0.02x |     -98.01% |
| ryml_json_inplace                        |   66.25 |  25.33x |  2432.68% |          5.93 |       0.04x |     -96.05% |
| ryml_yaml_inplace_nofilter_reserve       |  119.89 |  45.84x |  4483.51% |          3.28 |       0.02x |     -97.82% |
| ryml_yaml_inplace_nofilter               |   63.85 |  24.41x |  2340.98% |          6.16 |       0.04x |     -95.90% |
| ryml_yaml_inplace_reserve                |  122.52 |  46.84x |  4584.00% |          3.21 |       0.02x |     -97.87% |
| ryml_yaml_inplace                        |   63.72 |  24.36x |  2335.82% |          6.17 |       0.04x |     -95.89% |
| ryml_json_arena_nofilter_reserve         |  129.78 |  49.61x |  4861.38% |          3.03 |       0.02x |     -97.98% |
| ryml_json_arena_nofilter                 |   64.90 |  24.81x |  2381.04% |          6.06 |       0.04x |     -95.97% |
| ryml_json_arena_reserve                  |  130.74 |  49.98x |  4898.33% |          3.01 |       0.02x |     -98.00% |
| ryml_json_arena                          |   66.73 |  25.51x |  2451.12% |          5.89 |       0.04x |     -96.08% |
| ryml_yaml_arena_nofilter_reserve         |  121.20 |  46.34x |  4533.53% |          3.24 |       0.02x |     -97.84% |
| ryml_yaml_arena_nofilter                 |   64.46 |  24.64x |  2364.46% |          6.10 |       0.04x |     -95.94% |
| ryml_yaml_arena_reserve                  |  121.30 |  46.37x |  4537.19% |          3.24 |       0.02x |     -97.84% |
| ryml_yaml_arena                          |   64.54 |  24.67x |  2367.33% |          6.09 |       0.04x |     -95.95% |
| libyaml_arena                            |   19.17 |   7.33x |   632.99% |         20.50 |       0.14x |     -86.36% |
| libyaml_arena_reuse                      |   20.21 |   7.73x |   672.76% |         19.44 |       0.13x |     -87.06% |
| libfyaml_arena                           |   14.44 |   5.52x |   451.86% |         27.22 |       0.18x |     -81.88% |
| yamlcpp_arena                            |    2.62 |   1.00x |     0.00% |        150.24 |       1.00x |       0.00% |
| rapidjson_arena                          |  421.46 | 161.12x | 16012.23% |          0.93 |       0.01x |     -99.38% |
| rapidjson_inplace                        |  346.31 | 132.40x | 13139.51% |          1.13 |       0.01x |     -99.24% |
| sajson_arena                             |  211.42 |  80.82x |  7982.49% |          1.86 |       0.01x |     -98.76% |
| sajson_inplace                           |  216.52 |  82.77x |  8177.50% |          1.82 |       0.01x |     -98.79% |
| jsoncpp_arena                            |   27.70 |  10.59x |   959.03% |         14.19 |       0.09x |     -90.56% |
| nlohmann_arena                           |   60.72 |  23.21x |  2221.40% |          6.47 |       0.04x |     -95.69% |
+------------------------------------------+---------+---------+-----------+---------------+-------------+-------------+ 


/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.click_policy` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/usr/lib/python3.11/site-packages/bokeh/models/plots.py:815: UserWarning: 
You are attempting to set `plot.legend.label_text_font_size` on a plot that has zero legends added, this will have no effect.

Before legend properties can be set, you must add a Legend explicitly, or call a glyph method with a legend parameter set.

  warnings.warn(_LEGEND_EMPTY_WARNING % attr)
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100_json-mega_bytes_per_second.html
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:142: UserWarning: save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN
  warn("save() called but no resources were supplied and output_file(...) was never called, defaulting to resources.CDN")
/usr/lib/python3.11/site-packages/bokeh/io/saving.py:154: UserWarning: save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'
  warn("save() called but no title was supplied and output_file(...) was never called, using default title 'Bokeh Plot'")
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100_json-mega_bytes_per_second.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100_json-mega_bytes_per_second.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100_json-cpu_time_ms.html
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100_json-cpu_time_ms.png
/proj/rapidyaml/build/linux-x86_64-gxx13.2-Release/bm/bm-results/ryml-bm-parse-style_seqs_flow_outer1000_inner100_json-cpu_time_ms.html
Built target ryml-bm-parse-style_seqs_flow_outer1000_inner100_json-plot
Built target ryml-bm-plot
Returning to directory /proj/rapidyaml (currently in /proj/rapidyaml/build/linux-x86_64-gxx13.2-Release)
Build: finished building (1m 20s): linux-x86_64-gxx13.2-Release
-----------------------------------------------

Compilation finished at Tue May  7 20:51:34
```
