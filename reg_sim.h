#ifndef REG_SIM_H
#define REG_SIM_H

#include <stdint.h>

// Bit positions for each flag in the status register
#define REG_BIT_RXF  2   // RX Full
#define REG_BIT_TXE  3   // TX Empty
#define REG_BIT_INT  4   // Interrupt pending
#define REG_BIT_RDY  5   // Device ready
#define REG_BIT_OVF  6   // Overflow error
#define REG_BIT_ERR  7   // General error

// Status register struct
typedef struct {
  uint8_t STATUS;   // main status register
  uint8_t CTRL;     // control register
  uint8_t DATA;     // data register
} PeripheralReg_t;

// Function declarations
void     reg_init(PeripheralReg_t *reg);
void     reg_set_flag(PeripheralReg_t *reg, uint8_t bit);
void     reg_clear_flag(PeripheralReg_t *reg, uint8_t bit);
uint8_t  reg_read_flag(PeripheralReg_t *reg, uint8_t bit);
void     reg_print_status(PeripheralReg_t *reg);

#endif