#pragma once

#include <cstdint>

#include <x86intrin.h>

static std::uint64_t rdtsc()
{
    return __rdtsc();
}

static std::uint64_t rdtscf()
{
    _mm_lfence();
    auto tsc = __rdtsc();
    _mm_lfence();
    return tsc;
}

void pin_to_cpu(int cpu);
