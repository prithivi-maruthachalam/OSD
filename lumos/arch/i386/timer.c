#include <lumos/timer.h>
#include <lumos/isr.h>
#include <lumos/ports.h>

#include <stdio.h>

uint32_t ticks = 0;
uint32_t CLOCKS_PER_SECOND;

void timer_tick(struct registers_state regs)
{
    ticks++;
    UNUSED(regs);
}

void sleep(uint32_t ms)
{
    uint32_t target_ticks = ticks + (ms * CLOCKS_PER_SECOND / 1000);
    uint32_t t;
    while (ticks <= target_ticks)
    {
        asm volatile("hlt"); // To deal with compiler optimisation
    };
}

uint32_t clock_ticks()
{
    return ticks;
}

void init_timer(uint32_t targetFrequency)
{
    outb(PIC_MASTER_DATA_PORT, 0xFE);
    outb(PIC_SLAVE_DATA_PORT, 0x00);

    asm volatile("cli");
    register_interrupt_handler(32, timer_tick);

    CLOCKS_PER_SECOND = targetFrequency;
    uint16_t divider = (uint16_t)(1193182 / targetFrequency);

    outb(PIT_CHANNEL0_COMMAND, 0x36);
    outb(PIT_CHANNEL0_DATA, (uint8_t)(divider & 0x00FF));
    outb(PIT_CHANNEL0_DATA, (uint8_t)(divider >> 8) & 0x00FF);

    asm volatile("sti");
}