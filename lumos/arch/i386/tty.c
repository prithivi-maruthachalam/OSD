/* 
Implements tty.h 
Functions to initialize and print to it
*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <lumos/tty.h> // Will be needed at other parts of the system
#include "vga.h"       // Only used inside this object file

/* string.h is actually provided by us. We are able to include it without an explicit
path because we're doing this with sysroot. So the library will be linked from our OS's includes
folder */

#define VGA_HEIGHT 25
#define VGA_WIDTH 80
static uint16_t *const VGA_MEMORY = (uint16_t *)0xC03FF000;

static size_t tty_row;
static size_t tty_column;
static uint8_t tty_color;
static uint16_t *tty_buffer;

/* Initialises video memory by filling all of it with the space character */
void terminal_init()
{
    tty_row = 0;
    tty_column = 0;
    tty_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
    tty_buffer = (uint16_t *)VGA_MEMORY;
    for (size_t y = 0; y < VGA_HEIGHT; y++)
    { // y < 25
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            // calculate index
            // set index to empty string
            const size_t index = (y * VGA_WIDTH) + x;
            tty_buffer[index] = vga_entry(' ', tty_color);
        }
    }
}

void terminal_scroll()
{
    for (size_t y = 1; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            const size_t indexFrom = (y * VGA_WIDTH) + x;
            const size_t indexTo = ((y - 1) * VGA_WIDTH) + x;

            tty_buffer[indexTo] = tty_buffer[indexFrom];
        }
    }

    tty_row--;
    for (size_t x = 0; x < VGA_WIDTH; x++)
    {
        const size_t lastIndex = (tty_row * VGA_WIDTH) + x;
        tty_buffer[lastIndex] = vga_entry(' ', tty_color);
    }
}

/* Puts a char in the buffer at the current (row, col) and current color. It also increments current row
and column as required, resetting them to 0 when there's an overflow */
// TODO: Make scrolling better?
void terminal_putchar(char c)
{
    if (tty_row == VGA_HEIGHT)
    {
        terminal_scroll();
    }

    if (c == '\n')
    {
        tty_column = 0;
        tty_row++;
        return;
    }

    if (c == '\t')
    {
        tty_column += 4;
        return;
    }

    if (c == 8)
    {
        if (tty_column-- == 0)
        {
            tty_column = 0;
        }
        const size_t index = tty_row * VGA_WIDTH + tty_column;
        tty_buffer[index] = vga_entry(' ', tty_color);
        return;
    }

    const size_t index = tty_row * VGA_WIDTH + tty_column;
    tty_buffer[index] = vga_entry((unsigned char)c, tty_color);

    if (++tty_column >= VGA_WIDTH)
    {
        tty_column = 0;
        if (++tty_row >= VGA_HEIGHT)
        {
            terminal_scroll();
        }
    }
}

void terminal_write(const char *str, size_t size)
{
    for (size_t i = 0; i < size; i++)
        terminal_putchar(str[i]);
}

void terminal_writestring(const char *str)
{
    terminal_write(str, strlen(str));
}
