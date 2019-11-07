project(low-latency-training)

add_subdirectory(maciek)

# externals
set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "" FORCE)
add_subdirectory(google_benchmark)
