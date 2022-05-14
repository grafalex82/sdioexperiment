###################
# CMSIS and HAL
###################


SET(HAL_SRC
	STM32F1/CMSIS_Inc/stm32f103xe.h
	STM32F1/CMSIS_Inc/stm32f1xx.h
	STM32F1/CMSIS_Inc/system_stm32f1xx.h
	STM32F1/CMSIS_Src/startup_stm32f103xe.s

#	STM32F1/HAL_Inc/stm32_assert_template.h
	STM32F1/HAL_Inc/stm32f1xx_hal.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_adc.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_adc_ex.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_can.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_can_ex.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_cec.h
	STM32F1/HAL_Inc/stm32f1xx_hal_conf.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_conf_template.h
	STM32F1/HAL_Inc/stm32f1xx_hal_cortex.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_crc.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_dac.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_dac_ex.h
	STM32F1/HAL_Inc/stm32f1xx_hal_def.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_dma.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_dma_ex.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_eth.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_flash.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_flash_ex.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_gpio.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_gpio_ex.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_hcd.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_i2c.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_i2s.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_irda.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_iwdg.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_mmc.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_nand.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_nor.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_pccard.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_pcd.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_pcd_ex.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_pwr.h
	STM32F1/HAL_Inc/stm32f1xx_hal_rcc.h
	STM32F1/HAL_Inc/stm32f1xx_hal_rcc_ex.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_rtc.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_rtc_ex.h
	STM32F1/HAL_Inc/stm32f1xx_hal_sd.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_smartcard.h
	STM32F1/HAL_Inc/stm32f1xx_hal_spi.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_sram.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_tim.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_tim_ex.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_uart.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_usart.h
#	STM32F1/HAL_Inc/stm32f1xx_hal_wwdg.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_adc.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_bus.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_cortex.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_crc.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_dac.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_dma.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_exti.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_fsmc.h
	STM32F1/HAL_Inc/stm32f1xx_ll_gpio.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_i2c.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_iwdg.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_pwr.h
	STM32F1/HAL_Inc/stm32f1xx_ll_rcc.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_rtc.h
	STM32F1/HAL_Inc/stm32f1xx_ll_sdmmc.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_spi.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_system.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_tim.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_usart.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_usb.h
	STM32F1/HAL_Inc/stm32f1xx_ll_utils.h
#	STM32F1/HAL_Inc/stm32f1xx_ll_wwdg.h


	STM32F1/HAL_Src/stm32f1xx_hal.c
#	STM32F1/HAL_Src/stm32f1xx_hal_adc.c
#	STM32F1/HAL_Src/stm32f1xx_hal_adc_ex.c
#	STM32F1/HAL_Src/stm32f1xx_hal_can.c
#	STM32F1/HAL_Src/stm32f1xx_hal_cec.c
	STM32F1/HAL_Src/stm32f1xx_hal_cortex.c
#	STM32F1/HAL_Src/stm32f1xx_hal_crc.c
#	STM32F1/HAL_Src/stm32f1xx_hal_dac.c
#	STM32F1/HAL_Src/stm32f1xx_hal_dac_ex.c
#	STM32F1/HAL_Src/stm32f1xx_hal_dma.c
#	STM32F1/HAL_Src/stm32f1xx_hal_eth.c
#	STM32F1/HAL_Src/stm32f1xx_hal_flash.c
#	STM32F1/HAL_Src/stm32f1xx_hal_flash_ex.c
	STM32F1/HAL_Src/stm32f1xx_hal_gpio.c
#	STM32F1/HAL_Src/stm32f1xx_hal_gpio_ex.c
#	STM32F1/HAL_Src/stm32f1xx_hal_hcd.c
#	STM32F1/HAL_Src/stm32f1xx_hal_i2c.c
#	STM32F1/HAL_Src/stm32f1xx_hal_i2s.c
#	STM32F1/HAL_Src/stm32f1xx_hal_irda.c
#	STM32F1/HAL_Src/stm32f1xx_hal_iwdg.c
#	STM32F1/HAL_Src/stm32f1xx_hal_mmc.c
#	STM32F1/HAL_Src/stm32f1xx_hal_msp_template.c
#	STM32F1/HAL_Src/stm32f1xx_hal_nand.c
#	STM32F1/HAL_Src/stm32f1xx_hal_nor.c
#	STM32F1/HAL_Src/stm32f1xx_hal_pccard.c
#	STM32F1/HAL_Src/stm32f1xx_hal_pcd.c
#	STM32F1/HAL_Src/stm32f1xx_hal_pcd_ex.c
#	STM32F1/HAL_Src/stm32f1xx_hal_pwr.c
	STM32F1/HAL_Src/stm32f1xx_hal_rcc.c
	STM32F1/HAL_Src/stm32f1xx_hal_rcc_ex.c
#	STM32F1/HAL_Src/stm32f1xx_hal_rtc.c
#	STM32F1/HAL_Src/stm32f1xx_hal_rtc_ex.c
	STM32F1/HAL_Src/stm32f1xx_hal_sd.c
#	STM32F1/HAL_Src/stm32f1xx_hal_smartcard.c
	STM32F1/HAL_Src/stm32f1xx_hal_spi.c
#	STM32F1/HAL_Src/stm32f1xx_hal_spi_ex.c
#	STM32F1/HAL_Src/stm32f1xx_hal_sram.c
#	STM32F1/HAL_Src/stm32f1xx_hal_tim.c
#	STM32F1/HAL_Src/stm32f1xx_hal_tim_ex.c
#	STM32F1/HAL_Src/stm32f1xx_hal_timebase_rtc_alarm_template.c
#	STM32F1/HAL_Src/stm32f1xx_hal_timebase_tim_template.c
#	STM32F1/HAL_Src/stm32f1xx_hal_uart.c
#	STM32F1/HAL_Src/stm32f1xx_hal_usart.c
#	STM32F1/HAL_Src/stm32f1xx_hal_wwdg.c
#	STM32F1/HAL_Src/stm32f1xx_ll_adc.c
#	STM32F1/HAL_Src/stm32f1xx_ll_crc.c
#	STM32F1/HAL_Src/stm32f1xx_ll_dac.c
#	STM32F1/HAL_Src/stm32f1xx_ll_dma.c
#	STM32F1/HAL_Src/stm32f1xx_ll_exti.c
#	STM32F1/HAL_Src/stm32f1xx_ll_fsmc.c
#	STM32F1/HAL_Src/stm32f1xx_ll_gpio.c
#	STM32F1/HAL_Src/stm32f1xx_ll_i2c.c
#	STM32F1/HAL_Src/stm32f1xx_ll_pwr.c
	STM32F1/HAL_Src/stm32f1xx_ll_rcc.c
#	STM32F1/HAL_Src/stm32f1xx_ll_rtc.c
	STM32F1/HAL_Src/stm32f1xx_ll_sdmmc.c
#	STM32F1/HAL_Src/stm32f1xx_ll_spi.c
#	STM32F1/HAL_Src/stm32f1xx_ll_tim.c
#	STM32F1/HAL_Src/stm32f1xx_ll_usart.c
#	STM32F1/HAL_Src/stm32f1xx_ll_usb.c
	STM32F1/HAL_Src/stm32f1xx_ll_utils.c
	STM32F1/HAL_Src/system_stm32f1xx.c
)

ADD_LIBRARY(HAL STATIC ${HAL_SRC})
