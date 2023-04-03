#include <oc_clock.h>
#include <zephyr/kernel.h>

void
oc_clock_init(void)
{
    // Intentionally empty
}

oc_clock_time_t oc_clock_time(void)
{   
    return k_uptime_get();
}