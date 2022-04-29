#pragma once

#include "pico/stdlib.h"

static inline uint32_t clock_get_ms(void)
{
    return to_ms_since_boot(get_absolute_time());
}
