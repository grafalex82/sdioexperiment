#include "SPIDriver.h"

#include <stm32f1xx_ll_spi.h>
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>

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

static GPIO_TypeDef * const		ENABLE_PIN_PORT		= GPIOA;
static const uint32_t			ENABLE_PIN_NUM		= LL_GPIO_PIN_15;


SPIDriver::SPIDriver()
{
	// Enable clocking of corresponding periperhal
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA | LL_APB2_GRP1_PERIPH_GPIOC);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

	// Enable SD card power
	LL_GPIO_SetPinMode(ENABLE_PIN_PORT, ENABLE_PIN_NUM, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(ENABLE_PIN_PORT, ENABLE_PIN_NUM, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinSpeed(ENABLE_PIN_PORT, ENABLE_PIN_NUM, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_ResetOutputPin(ENABLE_PIN_PORT, ENABLE_PIN_NUM);

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

bool SPIDriver::initCard()
{
	deselectCard();

	for(int i=0; i<10; i++)
		transmitByte(0xff);

	selectCard();

	//printf("Sending CMD0\n");
	static const uint8_t cmd[] = { 0x40 | 0x00 /* CMD0 */, 0x00, 0x00, 0x00, 0x00 /* ARG = 0 */, (0x4A << 1) | 1 /* CRC7 + end bit */ };
	transmit(cmd, sizeof(cmd));

	char ch = 0;
	do
	{
		ch = receiveByte();
		printf("Received byte %02x\n", ch);
	}
	while(ch != 0x01);

	return true;
}
