#include "SPIDriver.h"
#include "DumpFunctions.h"

#include <stm32f1xx_ll_spi.h>
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_sdmmc.h>

#include <stdio.h>

// Pin constants
static GPIO_TypeDef * const		MISO_PIN_PORT		= GPIOA;
static const uint32_t			MISO_PIN_NUM		= LL_GPIO_PIN_6;
static GPIO_TypeDef * const		MOSI_PIN_PORT		= GPIOA;
static const uint32_t			MOSI_PIN_NUM		= LL_GPIO_PIN_7;
static GPIO_TypeDef * const		SCK_PIN_PORT		= GPIOA;
static const uint32_t			SCK_PIN_NUM			= LL_GPIO_PIN_5;
static GPIO_TypeDef * const		CS_PIN_PORT			= GPIOC;
static const uint32_t			CS_PIN_NUM			= LL_GPIO_PIN_11;

static const uint8_t R1_IDLE_STATE		= 0x01;
static const uint8_t R1_ILLEGAL_COMMAND = 0x04;

static GPIO_TypeDef * const		LED2_PORT		= GPIOB;
static const uint32_t			LED2_PIN		= LL_GPIO_PIN_10;


SPIDriver::SPIDriver()
{
}

void SPIDriver::init(unsigned int prescaler)
{
    // Enable clocking of corresponding periperhal
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA | LL_APB2_GRP1_PERIPH_GPIOC);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

    // Init pins
    LL_GPIO_SetPinMode(MOSI_PIN_PORT, MOSI_PIN_NUM, LL_GPIO_MODE_ALTERNATE);			// MOSI: AF PP
    LL_GPIO_SetPinOutputType(MOSI_PIN_PORT, MOSI_PIN_NUM, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(MOSI_PIN_PORT, MOSI_PIN_NUM, LL_GPIO_SPEED_FREQ_HIGH);

    LL_GPIO_SetPinMode(MISO_PIN_PORT, MISO_PIN_NUM, LL_GPIO_MODE_INPUT);				// MISO: Input Floating

    LL_GPIO_SetPinMode(SCK_PIN_PORT, SCK_PIN_NUM, LL_GPIO_MODE_ALTERNATE);			// SCK: AF PP
    LL_GPIO_SetPinOutputType(SCK_PIN_PORT, SCK_PIN_NUM, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(SCK_PIN_PORT, SCK_PIN_NUM, LL_GPIO_SPEED_FREQ_HIGH);

    LL_GPIO_SetPinMode(CS_PIN_PORT, CS_PIN_NUM, LL_GPIO_MODE_OUTPUT);				// CS
    LL_GPIO_SetPinOutputType(CS_PIN_PORT, CS_PIN_NUM, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(CS_PIN_PORT, CS_PIN_NUM, LL_GPIO_SPEED_FREQ_LOW);

    // Set CS pin High by default
    deselectCard();

    // Disable SPI before reconfiguration
    LL_SPI_Disable(SPI1);

    // Calculate prescaler value
    if(prescaler < 2)
        prescaler = 2;
    unsigned int freq = 72000/prescaler;
    uint32_t div = LL_SPI_BAUDRATEPRESCALER_DIV2;
    for(unsigned int p = prescaler/4; p != 0; p /= 2)
        div++;

    // Configure SPI
    printf("Configuring SPI with prescaler %d (div=%ld, freq=%dkHz)\n", prescaler, div, freq);
    LL_SPI_SetMode(SPI1, LL_SPI_MODE_MASTER);
    LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_1EDGE);
    LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_LOW);
    LL_SPI_SetBaudRatePrescaler(SPI1, div);
    LL_SPI_SetTransferBitOrder(SPI1, LL_SPI_MSB_FIRST);
    LL_SPI_SetTransferDirection(SPI1, LL_SPI_FULL_DUPLEX);
    LL_SPI_SetDataWidth(SPI1, LL_SPI_DATAWIDTH_8BIT);
    LL_SPI_DisableCRC(SPI1);
    LL_SPI_SetNSSMode(SPI1, LL_SPI_NSS_SOFT);

    // Now enable it
    LL_SPI_Enable(SPI1);
}

void SPIDriver::deinit()
{
    // Disable SPI
    deselectCard();
    LL_SPI_Disable(SPI1);

    // Reset pin configuration
    LL_GPIO_SetPinMode(MOSI_PIN_PORT, MOSI_PIN_NUM, LL_GPIO_MODE_FLOATING);
    LL_GPIO_SetPinMode(MISO_PIN_PORT, MISO_PIN_NUM, LL_GPIO_MODE_FLOATING);
    LL_GPIO_SetPinMode(SCK_PIN_PORT, SCK_PIN_NUM, LL_GPIO_MODE_FLOATING);
    LL_GPIO_SetPinMode(CS_PIN_PORT, CS_PIN_NUM, LL_GPIO_MODE_FLOATING);
}

void SPIDriver::reset()
{
    // Card power mgmt requires sending at least 74 clock pulses before the first command while CS is up. We will send 80 pulses
    deselectCard();
    for(int i=0; i<10; i++)
        transmitByte(0xff);
}

inline void SPIDriver::selectCard()
{
    LL_GPIO_ResetOutputPin(CS_PIN_PORT, CS_PIN_NUM);
}

inline void SPIDriver::deselectCard()
{
    LL_GPIO_SetOutputPin(CS_PIN_PORT, CS_PIN_NUM);
}

inline void SPIDriver::transmitByte(uint8_t byte)
{
    // Wait until previous byte is transmitted
    while(!LL_SPI_IsActiveFlag_TXE(SPI1))
        ;

    // Push new byte to output shifting register
    LL_SPI_TransmitData8(SPI1, byte);
}

inline uint8_t SPIDriver::receiveByte()
{
    // Transmit a dummy byte, receiving a byte will happen at the same time
    transmitByte(0xff);

    // Get new byte from input shifting register
    uint8_t r = LL_SPI_ReceiveData8(SPI1);
    return r;
}


void SPIDriver::transmit(const uint8_t * buf, size_t len)
{
    while (len)
    {
        transmitByte(*buf);
        len--;
        buf++;
    }
}

void SPIDriver::receive(uint8_t * buf, size_t len)
{
    while (len)
    {
        *buf = receiveByte();
        len--;
        buf++;
    }
}

void SPIDriver::readData(uint8_t * buf, size_t len)
{
    // Wait for tocken
    while(receiveByte() == 0xff)
        ;

    // Read the data
    receive(buf, len);

    // Receive the CRC
    uint16_t crc;
    receive((uint8_t *)&crc, sizeof(crc));
}

uint8_t SPIDriver::CRC7(uint8_t * buf, size_t len)
{
    uint8_t crc = 0;
    for (uint8_t i = 0; i < len; i++)
    {
        uint8_t d = buf[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            crc <<= 1;
            if ((d & 0x80) ^ (crc & 0x80))
                crc ^= 0x09;
            d <<= 1;
        }
    }
    return (crc << 1) | 1; // Last bit is a stop bit
}

void SPIDriver::sendCommand(int cmd, int arg)
{
    //transmitByte(0xff);

    uint8_t buf[6];
    buf[0] = 0x40 | cmd; // 0x40 - direction bit is 1 - from host to card
    buf[1] = (arg >> 24) & 0xff;
    buf[2] = (arg >> 16) & 0xff;
    buf[3] = (arg >> 8) & 0xff;
    buf[4] = arg & 0xff;
    buf[5] = CRC7(buf, 5);

    transmit(buf, sizeof(buf));
}

uint8_t SPIDriver::waitForR1()
{
    uint8_t r;
    while((r = receiveByte()) == 0xff) //0xff - card has not yet responded
        ;

    return r;
}

uint32_t SPIDriver::waitForR3R7()
{
    // Wait for R7 response (R1 + 4 bytes of data)
    // if CMD8 is not supported - it will return R1 (1 byte) with ILLEGAL_COMMAND flag set
    uint8_t r1 = waitForR1();
    if(r1 == (R1_ILLEGAL_COMMAND | R1_IDLE_STATE))
    {
        printf("R1 = %02x (Illegal command, possibly a SDSC card)... ", r1);
        return 0;
    }

    // if we got valid response - get 4 bytes of payload
    uint32_t r7 = receiveByte() << 24;
    r7 |= receiveByte() << 16;
    r7 |= receiveByte() << 8;
    r7 |= receiveByte();

    return r7;
}

void SPIDriver::cmd0_goIdleState()
{
    // To switch the card to SPI mode we need to send CMD0 with Card Select (CS) signal low
    selectCard();

    // Send CMD0 while card is selected (CS = 0)
    sendCommand(SDMMC_CMD_GO_IDLE_STATE, 0);

    // Wait for R1 response (1 byte)
    waitForR1();
}

bool SPIDriver::cmd8_sendInterfaceConditions()
{
    printf("Sending CMD8 (get interface conditions)... ");

    //CMD8 argunemnts: 0x01 - request voltage3.3V, 0xAA - check pattern
    sendCommand(SDMMC_CMD_HS_SEND_EXT_CSD, 0x1AA);

    // If the card supports CMD8 it will return 5 bytes R7: 1 byte status + 4 bytes data
    LL_GPIO_ResetOutputPin(LED2_PORT, LED2_PIN);
    uint32_t r7 = waitForR3R7();
    LL_GPIO_SetOutputPin(LED2_PORT, LED2_PIN);
    printf("R7 = %08lx\n", r7);

    // Assert r7 = 0x1aa - the card confirms 3.3V voltage and 0xAA check pattern
    return r7 == 0x1aa;
}

void SPIDriver::cmd55_sendAppCommand()
{
    //printf("Sending CMD55\n");
    sendCommand(SDMMC_CMD_APP_CMD, 0);
    waitForR1();
}

bool SPIDriver::acmd41_sendAppOpConditions(bool hostSupportSdhc)
{
    uint32_t arg = SDMMC_VOLTAGE_WINDOW_SD;
    arg |= hostSupportSdhc ? SDMMC_HIGH_CAPACITY : SDMMC_STD_CAPACITY;
    printf("Sending ACMD41 with arg=%08lx\n", arg);
    sendCommand(SDMMC_CMD_SD_APP_OP_COND, arg);

    uint8_t r = waitForR1();
    return r == 0;
}

bool SPIDriver::cmd58_readCCS()
{
    // Send Read OCR command (CMD58)
    sendCommand(58, 0);

    // response is OCR register
    uint32_t ocr = waitForR3R7();
    printf("OCR = %08lx\n", ocr);

    // Bit 30 contains Card Capacity Status (CCS) - true if the card is an SDHC or SDXC
    return ocr & (1 << 30);
}

void SPIDriver::cmd2_getCID()
{
    // CMD10 is the command to get CID
    printf("Sending CMD10 to get CID\n");
    sendCommand(10, 0);
    waitForR1();

    // Receive CID register data
    uint8_t resp[16];
    readData(resp, 16);

    // Parse and print the response
    printCID(resp);
}

uint16_t SPIDriver::cmd3_getRCA()
{
    // Nothing to do in SPI mode
    return 0;
}

void SPIDriver::cmd7_selectCard(uint16_t)
{
    // Nothing to do in SPI mode
}
