# Always build in release as it uses -Os
SET(CMAKE_BUILD_TYPE Release)

# All compiler related stuff is defined in the toolchain file
SET(STM32_CHIP "STM32F103RC")

# Load the toolchain file that uses vars above
SET(CMAKE_TOOLCHAIN_FILE cmake/gcc_stm32.cmake)

# Project definition
PROJECT(SDIOExperiment)

# Misc stuff
CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
ENABLE_LANGUAGE(ASM)

# Project wide definitions
ADD_DEFINITIONS(
	-DSTM32F1
	-DSTM32F103RC
	-DSTM32F103xE # That is correct, all chips (F103xC, F103xD, and F103xE) have code under this define
	-DF_CPU=72000000L
)

# Project wide include directories
INCLUDE_DIRECTORIES(

	# HAL and CMSIS
	${CMAKE_CURRENT_SOURCE_DIR}/HAL/CMSIS
	${CMAKE_CURRENT_SOURCE_DIR}/HAL/STM32F1/CMSIS_Inc
	${CMAKE_CURRENT_SOURCE_DIR}/HAL/STM32F1/HAL_Inc
)

ADD_SUBDIRECTORY(HAL)
ADD_SUBDIRECTORY(Src)

