# Build rules for SDIO Experiment target.
# App specific compiler/linker settings are also defined here

SET(SOURCE_FILES
	SDIOExperiment.cpp
	BoardInit.cpp
	BoardInit.h
	SPIDriver.cpp
	SPIDriver.h
	SDCard.cpp
	SDCard.h
	SDIODriver.cpp
	SDIODriver.h
        UartUtils.cpp
        UartUtils.h
        IDriver.h
)

INCLUDE_DIRECTORIES(
	.
)

ADD_EXECUTABLE(SDIOExperiment ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(SDIOExperiment
	HAL
)

STM32_GENERATE_MAP_FILE(SDIOExperiment)
STM32_SET_TARGET_PROPERTIES(SDIOExperiment)
STM32_PRINT_SIZE_OF_TARGETS(SDIOExperiment)

# Additional handy targets
STM32_ADD_HEX_BIN_TARGETS(SDIOExperiment)
STM32_ADD_DUMP_TARGET(SDIOExperiment)
