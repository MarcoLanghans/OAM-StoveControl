#pragma once
#include "OpenKNXHardware.h"

#ifdef ARDUINO_ARCH_RP2040
    #ifdef StoveControl_RP2040
        // #define OPENKNX_RECOVERY_ON LOW
        #define PROG_LED_PIN D8
        #define PROG_LED_PIN_ACTIVE_ON HIGH
        //#define INFO1_LED_PIN D8
        //#define INFO1_LED_PIN_ACTIVE_ON HIGH
        #define PROG_BUTTON_PIN D3
        #define PROG_BUTTON_PIN_INTERRUPT_ON FALLING
        #define SAVE_INTERRUPT_PIN D1
        #define STOVE_UART_RX_PIN D10
        #define STOVE_UART_TX_PIN D9
        //#define KNX_UART_RX_PIN D6
        //#define KNX_UART_TX_PIN D7
    #endif

    #ifdef StoveControl_PiPico
        // #define OPENKNX_RECOVERY_ON LOW
        #define PROG_LED_PIN 3
        #define PROG_LED_PIN_ACTIVE_ON HIGH
        #define INFO1_LED_PIN 11
        #define INFO1_LED_PIN_ACTIVE_ON HIGH
        #define PROG_BUTTON_PIN 16
        #define PROG_BUTTON_PIN_INTERRUPT_ON FALLING
        #define SAVE_INTERRUPT_PIN 2
        #define STOVE_UART_RX_PIN 9
        #define STOVE_UART_TX_PIN 8
        #define KNX_UART_RX_PIN 1
        #define KNX_UART_TX_PIN 0
        #define I2C_SDA_PIN 20
        #define I2C_SCL_PIN 21
    #endif


#endif

#ifdef ARDUINO_ARCH_ESP32
    #ifdef StoveControl_ESP32
        #define PROG_LED_PIN 20
        #define PROG_LED_PIN_ACTIVE_ON HIGH
        #define PROG_BUTTON_PIN 21
        #define PROG_BUTTON_PIN_INTERRUPT_ON FALLING
        #define INFO_LED_PIN 5
        #define INFO_LED_PIN_ACTIVE_ON HIGH
        #define OPENKNX_NO_BOOT_PULSATING 1
        #define KNX_UART_RX_PIN 24
        #define KNX_UART_TX_PIN 25
        #define KNX_SERIAL Serial1
        #define STOVE_UART_RX_PIN 16
        #define STOVE_UART_TX_PIN 17
    #endif

    #ifdef StoveControl_XIAO_ESP32
        #define PROG_LED_PIN  D8  //D8
        #define PROG_LED_PIN_ACTIVE_ON HIGH
        #define PROG_BUTTON_PIN D3 //D3
        #define PROG_BUTTON_PIN_INTERRUPT_ON FALLING
        //#define INFO_LED_PIN 5
        //#define INFO_LED_PIN_ACTIVE_ON HIGH
        //#define SAVE_INTERRUPT_PIN D1
        //#define OPENKNX_NO_BOOT_PULSATING 1
        //#define KNX_NO_DEFAULT_UART
        //#define KNX_UART_RX_PIN RX
        //#define KNX_UART_TX_PIN TX
        //#define KNX_SERIAL Serial0
        #define STOVE_UART_RX_PIN D10
        #define STOVE_UART_TX_PIN D9
    #endif
    

#endif

