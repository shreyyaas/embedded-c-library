#ifndef PARITY_H
#define PARITY_H

#include <stdint.h>

// Returns 0 if even parity, 1 if odd parity
uint8_t compute_parity(uint8_t data);

// Returns 1 if data has even parity, 0 otherwise
uint8_t check_even_parity(uint8_t data);

// Returns 1 if data has odd parity, 0 otherwise
uint8_t check_odd_parity(uint8_t data);



#endif