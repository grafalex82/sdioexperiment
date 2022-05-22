#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_usart.h>
#include <stm32f1xx_ll_rcc.h>

#include "UartUtils.h"

namespace
{
    bool verboseLevel = false;
}

// Pin constants
static GPIO_TypeDef * const		TX_PIN_PORT		= GPIOA;
static const uint32_t			TX_PIN_NUM		= LL_GPIO_PIN_9;
static GPIO_TypeDef * const		RX_PIN_PORT		= GPIOA;
static const uint32_t			RX_PIN_NUM		= LL_GPIO_PIN_10;

void initUART()
{
    // Enable clocking of corresponding periperhal
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA | LL_APB2_GRP1_PERIPH_USART1);

    // Init pins in alternate function mode
    LL_GPIO_SetPinMode(TX_PIN_PORT, TX_PIN_NUM, LL_GPIO_MODE_ALTERNATE);	// TX pin
    LL_GPIO_SetPinSpeed(TX_PIN_PORT, TX_PIN_NUM, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(TX_PIN_PORT, TX_PIN_NUM, LL_GPIO_OUTPUT_PUSHPULL);

    LL_GPIO_SetPinMode(RX_PIN_PORT, RX_PIN_NUM, LL_GPIO_MODE_INPUT);		// RX pin

    // Prepare for initialization
    LL_USART_Disable(USART1);

    // Init
    LL_RCC_ClocksTypeDef clocks;
    LL_RCC_GetSystemClocksFreq(&clocks);
    LL_USART_SetBaudRate(USART1, clocks.PCLK2_Frequency, 115200);
    LL_USART_SetDataWidth(USART1, LL_USART_DATAWIDTH_8B);
    LL_USART_SetStopBitsLength(USART1, LL_USART_STOPBITS_1);
    LL_USART_SetParity(USART1, LL_USART_PARITY_NONE);
    LL_USART_SetTransferDirection(USART1, LL_USART_DIRECTION_TX_RX);
    LL_USART_SetHWFlowCtrl(USART1, LL_USART_HWCONTROL_NONE);

    // Finally enable the peripheral
    LL_USART_Enable(USART1);
}

extern "C"
{
    void uartSendOneChar(char ch)
    {
        // Wait until previous byte is transmitted
        while(LL_USART_IsActiveFlag_TXE(USART1) == 0)
            ;

        // Send the byte to output shift register
        LL_USART_TransmitData8(USART1, ch);
    }

    char uartWaitForChar()
    {
        while(LL_USART_IsActiveFlag_RXNE(USART1) == 0)
            ;

        int ch = LL_USART_ReceiveData8(USART1);
        return ch;
    }

    int _write(int fd, char* ptr, int len) // Used inside printf()
    {
        (void)fd;

        int i = 0;
        while (ptr[i] && (i < len))
        {
            uartSendOneChar(ptr[i]);
            i++;
        }
        return len;
    }
}

void readLine(char * buf, size_t size)
{
    char * ptr = buf;

    while(ptr + 1  < buf + size)
    {
        char ch = uartWaitForChar();
        if(ch == '\r' || ch == '\n')
            break;

        *ptr = ch;
        ptr++;
    }

    *ptr = '\0';
    return;
}

void setVerboseLevel(bool verbose)
{
    verboseLevel = verbose;
}

bool getVerboseLevel()
{
    return verboseLevel;
}
