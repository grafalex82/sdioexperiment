#include "SPIDriver.h"

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

SPIDriver::SPIDriver()
{
}

void SPIDriver::init()
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

	// Configure SPI
	LL_SPI_SetMode(SPI1, LL_SPI_MODE_MASTER);
	LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_1EDGE);
	LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_LOW);
	LL_SPI_SetBaudRatePrescaler(SPI1, LL_SPI_BAUDRATEPRESCALER_DIV8);
	LL_SPI_SetTransferBitOrder(SPI1, LL_SPI_MSB_FIRST);
	LL_SPI_SetTransferDirection(SPI1, LL_SPI_FULL_DUPLEX);
	LL_SPI_SetDataWidth(SPI1, LL_SPI_DATAWIDTH_8BIT);
	LL_SPI_DisableCRC(SPI1);
	LL_SPI_SetNSSMode(SPI1, LL_SPI_NSS_SOFT);

	// Now enable it
	LL_SPI_Enable(SPI1);

	// Card power mgmt requires sending at least 74 clock pulses before the first command. We will send 80 pulses
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
	// Wait while byte is received (transmitting a dummy byte at the same time)
	LL_SPI_TransmitData8(SPI1, 0xff);
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1))
		;

	// Push new byte to output shifting register
	return LL_SPI_ReceiveData8(SPI1);
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
	uint8_t buf[6];
	buf[0] = 0x40 | cmd; // 0x40 - direction bit is 1 - from host to card
	buf[1] = (arg >> 24) & 0xff;
	buf[2] = (arg >> 16) & 0xff;
	buf[3] = (arg >> 8) & 0xff;
	buf[4] = arg & 0xff;
	buf[5] = CRC7(buf, 5);

	transmit(buf, sizeof(buf));
}

void SPIDriver::waitForR1()
{
	while(receiveByte() != R1_IDLE_STATE)
		;
}

uint8_t SPIDriver::waitForNonFFByte()
{
	uint8_t ch;
	while((ch = receiveByte()) == 0xff)
		;

	return ch;
}

void SPIDriver::cmd0_goIdleState()
{
	// To switch the card to SPI mode it is required to send at least 74 clock pulses while card is desetected
	selectCard();

	// Send CMD0 while card is selected (CS = 0)
	sendCommand(SDMMC_CMD_GO_IDLE_STATE, 0);

	// Wait for R1 response (1 byte)
	waitForR1();
}

bool SPIDriver::cmd8_sendInterfaceConditions()
{
	sendCommand(SDMMC_CMD_HS_SEND_EXT_CSD, 0x1AA); //0x01 - request voltage3.3V, 0xAA - check pattern

	// Wait for R7 response (R1 + 4 bytes of data)
	// If the card supports CMD8 it will return 5 bytes
	// if not - it will return R1 with ILLEGAL_COMMAND flag set
	if(waitForNonFFByte() == (R1_ILLEGAL_COMMAND | R1_IDLE_STATE))
		return false;

	uint32_t r7 = receiveByte() << 24;
	r7 |= receiveByte() << 16;
	r7 |= receiveByte() << 8;
	r7 |= receiveByte();

	// TODO: Assert r7 = 0x1aa - the card confirms 3.3V voltage and 0xAA check pattern
	 printf("R7 response: 0x%08x\n", (unsigned int)r7);

	return true;
}
