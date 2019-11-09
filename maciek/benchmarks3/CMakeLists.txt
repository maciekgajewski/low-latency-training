find_package(Threads)

add_executable(false_sharing false_sharing.cc util.cc util.hh)
target_link_libraries(false_sharing benchmark Threads::Threads)
