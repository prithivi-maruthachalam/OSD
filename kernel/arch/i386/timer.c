#include <kernel/timer.h>
#include <kernel/isr.h>
#include <kernel/ports.h>

#include <stdio.h>

uint32_t ticks = 0;

void timer_tick(struct registers_state regs)
{
    ticks++;
    UNUSED(regs);
}

uint32_t clock_ticks()
{
    return ticks;
}

void init_timer(uint32_t targetFrequency)
{
    asm volatile("cli");
    register_interrupt_handler(32, timer_tick);

    uint16_t divider = (uint16_t)(1193182 / targetFrequency);
    printf("%d", divider);

    outb(PIT_CHANNEL0_COMMAND, 0x36);
    outb(PIT_CHANNEL0_DATA, (uint8_t)(divider & 0x00FF));
    outb(PIT_CHANNEL0_DATA, (uint8_t)(divider >> 8) & 0x00FF);

    asm volatile("sti");
}