#include "BoardInit.h"
#include "SDCard.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_usart.h>
#include <stm32f1xx_ll_bus.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UartUtils.h"
#include "SPIDriver.h"
#include "SDIODriver.h"

SPIDriver spi;
SDIODriver sdio;
IDriver * driver = &spi;

// Pin constants
static GPIO_TypeDef * const		LED1_PORT		= GPIOB;
static const uint32_t			LED1_PIN		= LL_GPIO_PIN_11;
static GPIO_TypeDef * const		LED2_PORT		= GPIOB;
static const uint32_t			LED2_PIN		= LL_GPIO_PIN_10;
static GPIO_TypeDef * const		LED3_PORT		= GPIOC;
static const uint32_t			LED3_PIN		= LL_GPIO_PIN_4;
static GPIO_TypeDef * const		LED4_PORT		= GPIOA;
static const uint32_t			LED4_PIN		= LL_GPIO_PIN_4;


static void initLEDs(void)
{
    // Enable clocking of corresponding periperhal
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);

    LL_GPIO_SetPinMode(LED1_PORT, LED1_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(LED1_PORT, LED1_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinSpeed(LED1_PORT, LED1_PIN, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetOutputPin(LED1_PORT, LED1_PIN);

    LL_GPIO_SetPinMode(LED2_PORT, LED2_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(LED2_PORT, LED2_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinSpeed(LED2_PORT, LED2_PIN, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetOutputPin(LED2_PORT, LED2_PIN);

    LL_GPIO_SetPinMode(LED3_PORT, LED3_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(LED3_PORT, LED3_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinSpeed(LED3_PORT, LED3_PIN, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetOutputPin(LED3_PORT, LED3_PIN);

    LL_GPIO_SetPinMode(LED4_PORT, LED4_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(LED4_PORT, LED4_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinSpeed(LED4_PORT, LED4_PIN, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetOutputPin(LED4_PORT, LED4_PIN);
}

void cmdLoopback(const char * argument)
{
    if(*argument == '\0')
        printf("OK\n");
    else
        printf("%s\n", argument);
}

void cmdVerbose(const char * argument)
{
    // The argument is verbosity level (0 or 1)
    int verbose = atoi(argument);
    setVerboseLevel(verbose);
    printf("OK\n");
}


void reinitDriver(IDriver * newDriver, int prescaler)
{
    driver->deinit();

    // Switch the driver, if necessary
    if(driver != newDriver)
    {
        driver->deinit();
        cardPowerCycle();
        driver = newDriver;
    }

    driver->init(prescaler);
    printf("OK\n");
}

void cmdSpiInit(const char * argument)
{
    // Expect argument that is fPCLK prescaler
    int prescaler = atoi(argument);
    reinitDriver(&spi, prescaler);
}

void cmdSdioInit(const char * argument)
{
    // Expect argument that is fPCLK prescaler
    int prescaler = atoi(argument);
    reinitDriver(&sdio, prescaler);
}

void cmdReset(const char *)
{
    driver->reset();
    printf("OK\n");
}

void cmd0_goIdleState(const char *)
{
    driver->cmd0_goIdleState();
    printf("OK\n");
}

void cmd8_sendInterfaceConditions(const char *)
{
    bool v2card = driver->cmd8_sendInterfaceConditions();
    if(getVerboseLevel())
        printf("Card version - %d\n", v2card ? 2 : 1);
    printf("OK %d\n", v2card ? 2 : 1);
}

void cmd55_sendAppCommand(const char *)
{
    driver->cmd55_sendAppCommand();
    printf("OK\n");
}

void acmd41_sendAppOpConditions(const char * argument)
{
    // Expect argument that is SDHC support from the host side
    int sdhc = atoi(argument);

    bool valid = driver->acmd41_sendAppOpConditions(sdhc);
    printf("OK %s\n", valid ? "Valid" : "Busy");
}

void cmd58_readCCS(const char *)
{
    bool ccs = driver->cmd58_readCCS();
    printf("OK %s\n", ccs ? "SDHC" : "SDSC");
}

void cmd2_getCID(const char *)
{
    driver->cmd2_getCID();
    printf("OK\n");
}

void cmd3_getRCA(const char *)
{
    uint16_t rca = driver->cmd3_getRCA();
    printf("OK %d\n", rca);
}

void cmd7_selectCard(const char * argument)
{
    // Expect argument that is RCA of the card to select
    int rca = atoi(argument);
    driver->cmd7_selectCard(rca);
    printf("OK\n");
}

void cmd9_getCSD(const char * argument)
{
    // Expect argument that is RCA of the card to select
    int rca = atoi(argument);
    driver->cmd9_getCSD(rca);
    printf("OK\n");
}

void cmd10_getCID(const char * argument)
{
    // Expect argument that is RCA of the card to select
    int rca = atoi(argument);
    driver->cmd10_getCID(rca);
    printf("OK\n");
}

void cmdInitCard(const char * argument)
{
    // Perform full init sequence for a card
    driver->reset();
    delayMs(1);

    driver->cmd0_goIdleState();
    bool v2card = driver->cmd8_sendInterfaceConditions();
    if(!v2card)
        driver->cmd0_goIdleState();

    driver->cmd58_readCCS();

    uint32_t tstart = getTick();

    int retries = 0;
    for(;;)
    {
        retries++;

        driver->cmd55_sendAppCommand();
        bool valid = driver->acmd41_sendAppOpConditions(v2card);
        if(valid)
            break;

        if(getTick() - tstart > 3*getTickFreq()) // timeout - 3s
        {
            printf("ERROR Card busy after %d retries\n", retries);
            return;
        }
    }

    driver->cmd58_readCCS();

    uint32_t tend = getTick();

    driver->cmd2_getCID();
    driver->cmd3_getRCA();

    printf("OK %lu %d\n", tend-tstart, retries);
}

struct CommandEntry
{
    const char * command;
    void (*handler)(const char * argument);
};

CommandEntry commandHandlers[] = {
    {"LOOPBACK",    cmdLoopback},
    {"VERBOSE",     cmdVerbose},
    {"SPI_INIT",    cmdSpiInit},
    {"SDIO_INIT",   cmdSdioInit},
    {"RESET",       cmdReset},
    {"CMD0",        cmd0_goIdleState},
    {"CMD8",        cmd8_sendInterfaceConditions},
    {"CMD55",       cmd55_sendAppCommand},
    {"ACMD41",      acmd41_sendAppOpConditions},
    {"CMD58",       cmd58_readCCS},
    {"CMD2",        cmd2_getCID},
    {"CMD3",        cmd3_getRCA},
    {"CMD7",        cmd7_selectCard},
    {"CMD9",        cmd9_getCSD},
    {"CMD10",       cmd10_getCID},
    {"INIT_CARD",   cmdInitCard}
};

void parseCommand(const char * buf)
{
    printf("CMD: %s\n", buf);

    // Tokenize command
    const char * ptr = buf;
    while(*ptr != ' ' && *ptr != '\0')
        ptr++;

    size_t cmdLen = ptr - buf;

    // Search for argument start
    while(*ptr == ' ' && *ptr != '\0')
        ptr++;

    // Dispatch the command
    bool handled = false;
    for(size_t i=0; i<sizeof(commandHandlers)/sizeof(CommandEntry) && cmdLen != 0; i++)
    {
        if(!strncmp(commandHandlers[i].command, buf, cmdLen))
        {
            uint32_t tstart = getTick();
            commandHandlers[i].handler(ptr);
            uint32_t tend = getTick();

            printf("TIMESTATS %lu %lu (%lu)\n", tstart, tend, tend - tstart);

            handled = true;
            break;
        }
    }

    if(!handled)
    {
        printf("ERROR Unknown command: %s\n", buf);
        uint32_t tnow = getTick();
        printf("TIMESTATS %lu %lu (0)\n", tnow, tnow);
    }
}

int main(void)
{
    initBoard();
    initLEDs();

    delayMs(500);
    printf("============== Let the experiment begin ==============\n");

    while(true)
    {
        char buf[80];
        readLine(buf, 80);
        parseCommand(buf);
    }
}
