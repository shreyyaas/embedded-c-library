#ifndef UART_SIM_H
#define UART_SIM_H

#include <stdint.h>
#include "reg_sim.h"

// UART sim config
#define UART_BUFFER_SIZE 8

// UART state struct
typedef struct {
  PeripheralReg_t reg;
  uint8_t         buffer[UART_BUFFER_SIZE];
  uint8_t         buf_index;
  uint8_t         expected_crc;
} UARTSim_t;

// Function declarations
void    uart_init(UARTSim_t *uart, uint8_t expected_crc);
uint8_t uart_receive_byte(UARTSim_t *uart, uint8_t data_byte);
void    uart_print_state(UARTSim_t *uart);

#endif