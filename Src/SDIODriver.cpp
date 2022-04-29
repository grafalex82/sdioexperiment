#include "SDIODriver.h"
#include "DumpFunctions.h"

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

void SDIODriver::init(unsigned int prescaler)
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

    // Initialize SDIO at <=400kHz first
    printf("Configuring SDIO with prescaler %d (freq=%dkHz)\n", prescaler, 72000/(prescaler+2));
    SDIO_InitTypeDef initStruct;
    initStruct.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    initStruct.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    initStruct.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    initStruct.BusWide = SDIO_BUS_WIDE_1B;
    initStruct.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    initStruct.ClockDiv = prescaler; //SDIO_INIT_CLK_DIV
    SDIO_Init(SDIO, initStruct);

    //Enable SDIO power
    __SDIO_DISABLE(SDIO);
    SDIO_PowerState_ON(SDIO);
    __SDIO_ENABLE(SDIO);

    while(SDIO_GetPowerState(SDIO) != 3)
        ;
}

void SDIODriver::deinit()
{
    // Disable SDIO power
    SDIO_PowerState_OFF(SDIO);
    __SDIO_DISABLE(SDIO);

    // Disable SDIO clocks now
    LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_SDIO);

    // Deinitialize SDIO pins:
    LL_GPIO_SetPinMode(SDIO_D0_PIN_PORT, SDIO_D0_PIN_NUM, LL_GPIO_MODE_FLOATING);
    LL_GPIO_SetPinMode(SDIO_D1_PIN_PORT, SDIO_D1_PIN_NUM, LL_GPIO_MODE_FLOATING);
    LL_GPIO_SetPinMode(SDIO_D2_PIN_PORT, SDIO_D2_PIN_NUM, LL_GPIO_MODE_FLOATING);
    LL_GPIO_SetPinMode(SDIO_D3_PIN_PORT, SDIO_D3_PIN_NUM, LL_GPIO_MODE_FLOATING);
    LL_GPIO_SetPinMode(SDIO_CK_PIN_PORT, SDIO_CK_PIN_NUM, LL_GPIO_MODE_FLOATING);
    LL_GPIO_SetPinMode(SDIO_CMD_PIN_PORT, SDIO_CMD_PIN_NUM, LL_GPIO_MODE_FLOATING);
}


void SDIODriver::reset()
{

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

    uint32_t r7 = SDIO_GetResponse(SDIO, SDIO_RESP1);
    printf("R7 = %08lx\n", r7);

    return r == SDMMC_ERROR_NONE && r7 == 0x1aa;
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
    printf("Hack: catching OCR from previously executed ACMD41. OCR = %08lx\n", ocr);

    // Bit 30 contains Card Capacity Status (CCS) - true if the card is an SDHC or SDXC
    return ocr & (1<<30);
}

void SDIODriver::cmd2_getCID()
{
    // Send the commsnd
    uint32_t r = SDMMC_CmdSendCID(SDIO);
    printf("CMD2 error code: %08lx\n", r);

    // Get the 128-bit response
    uint8_t resp[16];
    get128BitResponse(resp);

    // Print the result
    printCID(resp);
}

uint16_t SDIODriver::cmd3_getRCA()
{
    uint16_t rca;
    uint32_t r = SDMMC_CmdSetRelAdd(SDIO, &rca);
    printf("CMD3 response %08lx. RCA=%04x\n", r, rca);
    return rca;
}

void SDIODriver::cmd7_selectCard(uint16_t rca)
{
    uint32_t r = SDMMC_CmdSelDesel(SDIO, rca << 16);
    printf("CMD7 response %08lx\n", r);
}

void SDIODriver::cmd9_getCSD(uint16_t rca)
{
    // Send the commsnd
    uint32_t r = SDMMC_CmdSendCSD(SDIO, rca << 16);
    printf("CMD9 error code: %08lx\n", r);

    // Get the 128-bit response
    uint8_t csd[16];
    get128BitResponse(csd);

    // Parse the response
    printCSD(csd);
}

void SDIODriver::cmd10_getCID(uint16_t rca)
{
}

void int2bytes(int value, uint8_t * bytes)
{
    bytes[0] = (value >> 24) & 0xff;
    bytes[1] = (value >> 16) & 0xff;
    bytes[2] = (value >> 8) & 0xff;
    bytes[3] = value & 0xff;
}

void SDIODriver::get128BitResponse(uint8_t * resp)
{
    int2bytes(SDIO_GetResponse(SDIO, SDIO_RESP1), resp);
    int2bytes(SDIO_GetResponse(SDIO, SDIO_RESP2), resp + 4);
    int2bytes(SDIO_GetResponse(SDIO, SDIO_RESP3), resp + 8);
    int2bytes(SDIO_GetResponse(SDIO, SDIO_RESP4), resp + 12);
}
