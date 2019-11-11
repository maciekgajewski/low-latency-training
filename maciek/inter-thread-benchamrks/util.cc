#include "util.hh"

#include <stdexcept>

#include <sched.h>
#include <string.h>
#include <errno.h>

void pin_to_cpu(int cpu)
{
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu, &set);

    int r = ::sched_setaffinity(0, sizeof(set), &set);
    if (r == -1)
    {
        throw std::runtime_error(::strerror(errno));
    }
}
