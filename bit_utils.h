#ifndef BIT_UTILS_H
#define BIT_UTILS_H

#include <stdint.h>

// Core bit manipulation macros
#define BIT_SET(reg, bit)    ((reg) |=  (1U << (bit)))
#define BIT_CLEAR(reg, bit)  ((reg) &= ~(1U << (bit)))
#define BIT_TOGGLE(reg, bit) ((reg) ^=  (1U << (bit)))
#define BIT_READ(reg, bit)   (((reg) >> (bit)) & 1U)

// Extract a multi-bit field from a register
#define BIT_FIELD_READ(reg, mask, pos) (((reg) & (mask)) >> (pos))

#endif