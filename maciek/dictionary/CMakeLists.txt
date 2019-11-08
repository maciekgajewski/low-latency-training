add_executable(dictionary_benchmark
    benchmark.cc dict.hh utils.h hash.hh)
target_link_libraries(dictionary_benchmark benchmark papi)

