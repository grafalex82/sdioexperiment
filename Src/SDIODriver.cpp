#include "SDIODriver.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_sdmmc.h>

// Pins assignment
static GPIO_TypeDef * const  SDIO_D0_PIN_PORT   = GPIOC;
static const uint32_t        SDIO_D0_PIN_NUM    = LL_GPIO_PIN_8;
static GPIO_TypeDef * const  SDIO_D1_PIN_PORT   = GPIOC;
static const uint32_t        SDIO_D1_PIN_NUM    = LL_GPIO_PIN_9;
static GPIO_TypeDef * const  SDIO_D2_PIN_PORT   = GPIOC;
static const uint32_t        SDIO_D2_PIN_NUM    = LL_GPIO_PIN_10;
static GPIO_TypeDef * const  SDIO_D3_PIN_PORT   = GPIOC;
static const uint32_t        SDIO_D3_PIN_NUM    = LL_GPIO_PIN_11;
static GPIO_TypeDef * const  SDIO_CK_PIN_PORT   = GPIOC;
static const uint32_t        SDIO_CK_PIN_NUM    = LL_GPIO_PIN_12;
static GPIO_TypeDef * const  SDIO_CMD_PIN_PORT   = GPIOD;
static const uint32_t        SDIO_CMD_PIN_NUM    = LL_GPIO_PIN_2;


SDIODriver::SDIODriver()
{
}

void SDIODriver::init()
{
    // Enable clocking of corresponding periperhal
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);

    // Initialize SDIO pins:
    LL_GPIO_SetPinMode(SDIO_D0_PIN_PORT, SDIO_D0_PIN_NUM, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinPull(SDIO_D0_PIN_PORT, SDIO_D0_PIN_NUM, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinSpeed(SDIO_D0_PIN_PORT, SDIO_D0_PIN_NUM, LL_GPIO_SPEED_FREQ_HIGH);

    LL_GPIO_SetPinMode(SDIO_D1_PIN_PORT, SDIO_D1_PIN_NUM, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinPull(SDIO_D1_PIN_PORT, SDIO_D1_PIN_NUM, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinSpeed(SDIO_D1_PIN_PORT, SDIO_D1_PIN_NUM, LL_GPIO_SPEED_FREQ_HIGH);

    LL_GPIO_SetPinMode(SDIO_D2_PIN_PORT, SDIO_D2_PIN_NUM, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinPull(SDIO_D2_PIN_PORT, SDIO_D2_PIN_NUM, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinSpeed(SDIO_D2_PIN_PORT, SDIO_D2_PIN_NUM, LL_GPIO_SPEED_FREQ_HIGH);

    LL_GPIO_SetPinMode(SDIO_D3_PIN_PORT, SDIO_D3_PIN_NUM, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinPull(SDIO_D3_PIN_PORT, SDIO_D3_PIN_NUM, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinSpeed(SDIO_D3_PIN_PORT, SDIO_D3_PIN_NUM, LL_GPIO_SPEED_FREQ_HIGH);

    LL_GPIO_SetPinMode(SDIO_CK_PIN_PORT, SDIO_CK_PIN_NUM, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinOutputType(SDIO_CK_PIN_PORT, SDIO_CK_PIN_NUM, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(SDIO_CK_PIN_PORT, SDIO_CK_PIN_NUM, LL_GPIO_SPEED_FREQ_HIGH);

    LL_GPIO_SetPinMode(SDIO_CMD_PIN_PORT, SDIO_CMD_PIN_NUM, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinOutputType(SDIO_CMD_PIN_PORT, SDIO_CMD_PIN_NUM, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(SDIO_CMD_PIN_PORT, SDIO_CMD_PIN_NUM, LL_GPIO_SPEED_FREQ_HIGH);

    // Enable SDIO clocks now
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_SDIO);

    // Initialize SDIO at 400kHz first
    SDIO_InitTypeDef initStruct;
    initStruct.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    initStruct.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    initStruct.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    initStruct.BusWide = SDIO_BUS_WIDE_1B;
    initStruct.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    initStruct.ClockDiv = SDIO_INIT_CLK_DIV;
    SDIO_Init(SDIO, initStruct);

    //Enable SDIO power
    __SDIO_DISABLE(SDIO);
    SDIO_PowerState_ON(SDIO);
    __SDIO_ENABLE(SDIO);

    while(SDIO_GetPowerState(SDIO) != 3)
        ;
}

void SDIODriver::cmd0_goIdleState()
{
    uint32_t r = SDMMC_CmdGoIdleState(SDIO);
    printf("CMD0 error code: %08lx\n", r);
}

bool SDIODriver::cmd8_sendInterfaceConditions()
{
    uint32_t r = SDMMC_CmdOperCond(SDIO);
    printf("CMD8 error code: %08lx\n", r);

    return r == SDMMC_ERROR_NONE;
}

void SDIODriver::cmd55_sendAppCommand()
{
    uint32_t r = SDMMC_CmdAppCommand(SDIO, 0);
    printf("CMD55 error code: %08lx\n", r);
}

bool SDIODriver::acmd41_sendAppOpConditions(bool hostSupportSdhc)
{
    // send our capability of handling SDHC cards
    uint32_t r = SDMMC_CmdAppOperCommand(SDIO, hostSupportSdhc ? SDMMC_HIGH_CAPACITY : SDMMC_STD_CAPACITY);
    printf("ACMD41 error code: %08lx\n", r);

    r = SDIO_GetResponse(SDIO, SDIO_RESP1);
    printf("ACMD41 response: %08lx\n", r);

    return r >> 31; //31 bit is valid status (if low - card is still busy initializing)
}

bool SDIODriver::cmd58_readCCS()
{
    // Little hack: if executed right after acmd41_sendAppOpConditions()
    // response register still contains Operations Conditions Register (OCR)
    uint32_t ocr = SDIO_GetResponse(SDIO, SDIO_RESP1);

    // Bit 30 contains Card Capacity Status (CCS) - true if the card is an SDHC or SDXC
    return ocr & (1<<30);
}
