project(low-latency-training)
set(CMAKE_CXX_STANDARD 17)

option(NO_OMIT_FP "Add -fno-omit-frame-pointer" FALSE)
if(NO_OMIT_FP)
  add_compile_options(-fno-omit-frame-pointer)
endif()

add_subdirectory(maciek)

# externals
set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "" FORCE)
add_subdirectory(google_benchmark)
