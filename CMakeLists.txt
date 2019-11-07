project(low-latency-training)
set(CMAKE_CXX_STANDARD 17)
add_subdirectory(maciek)

# externals
set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "" FORCE)
add_subdirectory(google_benchmark)
