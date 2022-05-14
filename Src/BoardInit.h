#ifndef BOARDINIT_H
#define BOARDINIT_H

#include <stdint.h>

void initBoard();
void delayMs(uint32_t ms);
void delayUs(uint32_t us);
uint32_t getTick();

#endif // BOARDINIT_H
