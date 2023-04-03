#include "port/oc_assert.h"

void
abort_impl(void)
{
    printk("abort();");

    while(1)
    {
        // Intentionally empty
    }
}

void
exit_impl(int status)
{
    printk("exit();");

    while(1)
    {
        // Intentionally empty
    }
}
