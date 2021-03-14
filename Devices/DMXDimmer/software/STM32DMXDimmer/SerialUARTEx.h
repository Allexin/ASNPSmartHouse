#ifndef _SERIALUARTEX_H
#define _SERIALUARTEX_H

#include "STM32System.h"
#include "stm32_gpio_af.h"

class UARTTools{
public:
  
  static void begin2Stop(SerialUART& uart, const uint32_t baud){
    if (uart.txBuffer == NULL) {
    static uint8_t tx[128];
    static uint8_t static_tx_used = 0;
  
    if (!static_tx_used) {
      uart.txBuffer = (uint8_t*)tx;
      static_tx_used = true;
    } else {
      uart.txBuffer = (uint8_t*)malloc(128);
    }
    }
  
    if (uart.rxBuffer == NULL) {
      static uint8_t rx[128];
      static uint8_t static_rx_used = 0;
  
      if (!static_rx_used) {
        uart.rxBuffer = (uint8_t*)rx;
        static_rx_used = true;
      } else {
        uart.rxBuffer = (uint8_t*)malloc(128);
      }
      }
    
      if (uart.handle == NULL) {
        static UART_HandleTypeDef h = {};
        static uint8_t static_handle_used = 0;
    
        if (!static_handle_used) {
          uart.handle = &h;
        static_handle_used = true;
        } else {
          uart.handle = (UART_HandleTypeDef*)malloc(sizeof(UART_HandleTypeDef));
        }
      }
    
      uart.handle->Instance = uart.instance;
      
      #ifdef USART1
      if (uart.handle->Instance == USART1) {
        __HAL_RCC_USART1_CLK_ENABLE();
        HAL_NVIC_SetPriority(USART1_IRQn, 0, 0); 
        HAL_NVIC_EnableIRQ(USART1_IRQn); 
      }
      #endif
      
      #ifdef USART2
      if (uart.handle->Instance == USART2) {
        __HAL_RCC_USART2_CLK_ENABLE();
        HAL_NVIC_SetPriority(USART2_IRQn, 0, 0); 
        HAL_NVIC_EnableIRQ(USART2_IRQn); 
      }
      #endif
      
      #ifdef USART3
      if (uart.handle->Instance == USART3) {
        __HAL_RCC_USART3_CLK_ENABLE();
        HAL_NVIC_SetPriority(USART3_IRQn, 0, 0); 
        HAL_NVIC_EnableIRQ(USART3_IRQn); 
      }
      #endif
      
      #ifdef UART4
      if (uart.handle->Instance == UART4) {
        __HAL_RCC_UART4_CLK_ENABLE();
        HAL_NVIC_SetPriority(UART4_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(UART4_IRQn);
      }
      #endif
      #ifdef USART4
      if (uart.handle->Instance == USART4) {
        __HAL_RCC_USART4_CLK_ENABLE();
        HAL_NVIC_SetPriority(USART4_IRQn, 0, 0); 
        HAL_NVIC_EnableIRQ(USART4_IRQn); 
      }
      #endif
      
      #ifdef UART5
      if (uart.handle->Instance == UART5) {
        __HAL_RCC_UART5_CLK_ENABLE();
        HAL_NVIC_SetPriority(UART5_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(UART5_IRQn);
      }
      #endif
      #ifdef USART5
      if (uart.handle->Instance == USART5) {
        __HAL_RCC_USART5_CLK_ENABLE();
        HAL_NVIC_SetPriority(USART5_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART5_IRQn);
      }
      #endif
    
      #ifdef USART6
      if (uart.handle->Instance == USART6) {
        __HAL_RCC_USART6_CLK_ENABLE();
        HAL_NVIC_SetPriority(USART6_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART6_IRQn);
      }
      #endif
    
      stm32AfUARTInit(uart.instance, uart.rxPort, uart.rxPin, uart.txPort, uart.txPin);
      
      uart.handle->Init.BaudRate = baud; 
      uart.handle->Init.WordLength = UART_WORDLENGTH_8B;
      uart.handle->Init.StopBits = UART_STOPBITS_2; 
      uart.handle->Init.Parity = UART_PARITY_NONE; 
      uart.handle->Init.Mode = UART_MODE_TX_RX; 
      uart.handle->Init.HwFlowCtl = UART_HWCONTROL_NONE; 
      uart.handle->Init.OverSampling = UART_OVERSAMPLING_16; 
      HAL_UART_Init(uart.handle);
    
      HAL_UART_Receive_IT(uart.handle, &uart.receive_buffer, 1);    
    }

    static void end(SerialUART& uart){
      uart.flush();

      #ifdef USART1
      if (uart.handle->Instance == USART1) {
        __HAL_RCC_USART1_FORCE_RESET();
        __HAL_RCC_USART1_RELEASE_RESET();
        __HAL_RCC_USART1_CLK_DISABLE();
      }
      #endif
      
      #ifdef USART2
      if (uart.handle->Instance == USART2) {
        __HAL_RCC_USART2_FORCE_RESET();
        __HAL_RCC_USART2_RELEASE_RESET();
        __HAL_RCC_USART2_CLK_DISABLE();
      }
      #endif
      
      #ifdef USART3
      if (uart.handle->Instance == USART3) {
        __HAL_RCC_USART3_FORCE_RESET();
        __HAL_RCC_USART3_RELEASE_RESET();
        __HAL_RCC_USART3_CLK_DISABLE();
      }
      #endif
      
      #ifdef UART4
      if (uart.handle->Instance == UART4) {
        __HAL_RCC_UART4_FORCE_RESET();
        __HAL_RCC_UART4_RELEASE_RESET();
        __HAL_RCC_UART4_CLK_DISABLE();
      }
      #endif
      #ifdef USART4
      if (uart.handle->Instance == USART4) {
        __HAL_RCC_USART4_FORCE_RESET();
        __HAL_RCC_USART4_RELEASE_RESET();
        __HAL_RCC_USART4_CLK_DISABLE();
      }
      #endif
      
      #ifdef UART5
      if (uart.handle->Instance == UART5) {
        __HAL_RCC_UART5_FORCE_RESET();
        __HAL_RCC_UART5_RELEASE_RESET();
        __HAL_RCC_UART5_CLK_DISABLE();
      }
      #endif
      #ifdef USART5
      if (uart.handle->Instance == USART5) {
        __HAL_RCC_USART5_FORCE_RESET();
        __HAL_RCC_USART5_RELEASE_RESET();
        __HAL_RCC_USART5_CLK_DISABLE();
      }
      #endif
    
      #ifdef USART6
      if (uart.handle->Instance == USART6) {
        __HAL_RCC_USART6_FORCE_RESET();
        __HAL_RCC_USART6_RELEASE_RESET();
        __HAL_RCC_USART6_CLK_DISABLE();
      }
      #endif
      HAL_UART_DeInit(uart.handle);
    }
    
};

#endif //_SERIALUARTEX_H
