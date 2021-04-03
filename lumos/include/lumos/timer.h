#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define PIT_CHANNEL0_DATA 0x40
#define PIT_CHANNEL0_COMMAND 0x43

void init_timer(uint32_t targetFrequency);
uint32_t clock();
void sleep(uint32_t ms);

#endif