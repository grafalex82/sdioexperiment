#ifndef UART_H
#define UART_H

#include <string.h>

void initUART();
void readLine(char * buf, size_t size);
void setVerboseLevel(bool verbose);
bool getVerboseLevel();

#endif // UART_H
