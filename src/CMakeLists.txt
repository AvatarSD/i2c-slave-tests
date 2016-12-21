
set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER /usr/bin/avr-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/avr-g++)
set(CMAKE_FIND_ROOT_PATH /usr/lib/avr)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_VERBOSE_MAKEFILE ON)


project("i2cnettest")


SET(flags "-mmcu=attiny85 -D__AVR_ATtiny85__ -DF_CPU=1000000UL \
	-Os -Wall -Wl,--gc-sections -fdata-sections -ffunction-sections -flto \
	-fno-threadsafe-statics -fshort-enums -mcall-prologues -Wl,--relax -fno-exceptions")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flags}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flags}")


include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/net/I2C
	${CMAKE_CURRENT_SOURCE_DIR}/net/server
	${CMAKE_CURRENT_SOURCE_DIR}/net/polymorphmemory
	${CMAKE_CURRENT_SOURCE_DIR}/net/cells)

file(GLOB_RECURSE EXE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
add_executable(${PROJECT_NAME} ${EXE_SRC})


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND
	nm --print-size -t d -C --size-sort ${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND
	avr-size -A -t ${PROJECT_NAME})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND
	avr-objcopy -j .text -j .data -O ihex ${PROJECT_NAME} ${PROJECT_NAME}.hex)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND
	avr-objcopy -j .eeprom --change-section-lma .eeprom=0 -O ihex ${PROJECT_NAME} ${PROJECT_NAME}_eeprom.hex)

#program to board
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND
#	avrdude -v -e -p t85 -c dragon_dw -Uflash:w:${PROJECT_NAME}.hex:i -Ueeprom:w:${PROJECT_NAME}_eeprom.hex)
