#include "Arduino.h"
#include "bit_utils.h"
#include "parity.h"
#include "reg_sim.h"
#include "uart_sim.h"

// ---- Parity Implementation ----

uint8_t compute_parity(uint8_t data) {
  uint8_t parity = 0;
  while (data) {
    parity ^= (data & 1);
    data >>= 1;
  }
  return parity;
}

uint8_t check_even_parity(uint8_t data) {
  return compute_parity(data) == 0;
}

uint8_t check_odd_parity(uint8_t data) {
  return compute_parity(data) == 1;
}

void run_parity_tests() {
  uint8_t test_cases[] = {0b00000000, 0b00000001,
                           0b10110010, 0b11111111,
                           0b01010101, 0b10101010};
  uint8_t num_tests = sizeof(test_cases) / sizeof(test_cases[0]);

  Serial.println("=== Parity Checker ===");
  Serial.println("Data       | Parity | Even? | Odd?");
  Serial.println("-----------|--------|-------|-----");

  for (uint8_t i = 0; i < num_tests; i++) {
    uint8_t d = test_cases[i];
    Serial.print("0b");
    for (int b = 7; b >= 0; b--) {
      Serial.print(BIT_READ(d, b));
    }
    Serial.print(" | ");
    Serial.print(compute_parity(d));
    Serial.print("      | ");
    Serial.print(check_even_parity(d));
    Serial.print("     | ");
    Serial.println(check_odd_parity(d));
  }
}

// ---- CRC-8 Implementation ----

uint8_t compute_crc8(uint8_t *data, uint8_t len) {
  uint8_t crc = 0x00;
  for (uint8_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t bit = 0; bit < 8; bit++) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x07;
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

void run_crc_tests() {
  Serial.println("\n=== CRC-8 Calculator ===");
  uint8_t msg1[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F};
  uint8_t msg2[] = {0xDE, 0xAD, 0xBE, 0xEF};
  uint8_t msg3[] = {0x01, 0x02, 0x03, 0x04};
  Serial.print("CRC8('Hello'):         0x");
  Serial.println(compute_crc8(msg1, 5), HEX);
  Serial.print("CRC8(0xDEADBEEF):      0x");
  Serial.println(compute_crc8(msg2, 4), HEX);
  Serial.print("CRC8(0x01020304):      0x");
  Serial.println(compute_crc8(msg3, 4), HEX);
}

// ---- Register Simulator Implementation ----

void reg_init(PeripheralReg_t *reg) {
  reg->STATUS = 0x00;
  reg->CTRL   = 0x00;
  reg->DATA   = 0x00;
}

void reg_set_flag(PeripheralReg_t *reg, uint8_t bit) {
  BIT_SET(reg->STATUS, bit);
}

void reg_clear_flag(PeripheralReg_t *reg, uint8_t bit) {
  BIT_CLEAR(reg->STATUS, bit);
}

uint8_t reg_read_flag(PeripheralReg_t *reg, uint8_t bit) {
  return BIT_READ(reg->STATUS, bit);
}

void reg_print_status(PeripheralReg_t *reg) {
  Serial.println("\n--- STATUS REGISTER ---");
  Serial.print("Raw value: 0x");
  Serial.println(reg->STATUS, HEX);
  Serial.print("ERR: "); Serial.println(reg_read_flag(reg, REG_BIT_ERR));
  Serial.print("OVF: "); Serial.println(reg_read_flag(reg, REG_BIT_OVF));
  Serial.print("RDY: "); Serial.println(reg_read_flag(reg, REG_BIT_RDY));
  Serial.print("INT: "); Serial.println(reg_read_flag(reg, REG_BIT_INT));
  Serial.print("TXE: "); Serial.println(reg_read_flag(reg, REG_BIT_TXE));
  Serial.print("RXF: "); Serial.println(reg_read_flag(reg, REG_BIT_RXF));
  Serial.println("-----------------------");
}

void run_reg_tests() {
  PeripheralReg_t myReg;
  reg_init(&myReg);

  Serial.println("\n=== Register Simulator ===");
  Serial.println("\n[1] Initial state:");
  reg_print_status(&myReg);

  Serial.println("\n[2] Device powers up — set RDY:");
  reg_set_flag(&myReg, REG_BIT_RDY);
  reg_print_status(&myReg);

  Serial.println("\n[3] Data received — set RXF:");
  reg_set_flag(&myReg, REG_BIT_RXF);
  reg_print_status(&myReg);

  Serial.println("\n[4] Overflow occurs — set OVF + ERR:");
  reg_set_flag(&myReg, REG_BIT_OVF);
  reg_set_flag(&myReg, REG_BIT_ERR);
  reg_print_status(&myReg);

  Serial.println("\n[5] Error handled — clear OVF + ERR:");
  reg_clear_flag(&myReg, REG_BIT_OVF);
  reg_clear_flag(&myReg, REG_BIT_ERR);
  reg_print_status(&myReg);
}

// ---- UART Simulator Implementation ----

void uart_init(UARTSim_t *uart, uint8_t expected_crc) {
  reg_init(&uart->reg);
  uart->buf_index    = 0;
  uart->expected_crc = expected_crc;
  for (uint8_t i = 0; i < UART_BUFFER_SIZE; i++) {
    uart->buffer[i] = 0x00;
  }
  reg_set_flag(&uart->reg, REG_BIT_RDY);
}

uint8_t uart_receive_byte(UARTSim_t *uart, uint8_t data_byte) {
  if (uart->buf_index >= UART_BUFFER_SIZE) {
    reg_set_flag(&uart->reg, REG_BIT_OVF);
    reg_set_flag(&uart->reg, REG_BIT_ERR);
    return 0;
  }
  if (!check_even_parity(data_byte)) {
    reg_set_flag(&uart->reg, REG_BIT_ERR);
    Serial.print("  [PARITY FAIL] byte: 0x");
    Serial.println(data_byte, HEX);
    return 0;
  }
  uart->buffer[uart->buf_index++] = data_byte;
  reg_set_flag(&uart->reg, REG_BIT_RXF);
  if (uart->buf_index == UART_BUFFER_SIZE) {
    uint8_t crc = compute_crc8(uart->buffer, UART_BUFFER_SIZE);
    if (crc != uart->expected_crc) {
      reg_set_flag(&uart->reg, REG_BIT_ERR);
      Serial.print("  [CRC FAIL] got: 0x");
      Serial.print(crc, HEX);
      Serial.print(" expected: 0x");
      Serial.println(uart->expected_crc, HEX);
      return 0;
    }
    Serial.println("  [CRC OK] Message verified");
  }
  return 1;
}

void uart_print_state(UARTSim_t *uart) {
  Serial.println("\n--- UART STATE ---");
  Serial.print("Buffer index: ");
  Serial.println(uart->buf_index);
  Serial.print("Buffer: ");
  for (uint8_t i = 0; i < uart->buf_index; i++) {
    Serial.print("0x");
    Serial.print(uart->buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  reg_print_status(&uart->reg);
}

void run_uart_sim() {
  Serial.println("\n=== UART Receiver Simulation ===");

  uint8_t valid_msg[] = {0x48, 0x14, 0x33, 0x0F,
                          0xAA, 0x96, 0x69, 0xC3};
  uint8_t expected = compute_crc8(valid_msg, 8);

  Serial.print("Expected CRC for valid message: 0x");
  Serial.println(expected, HEX);

  UARTSim_t uart;

  // Scenario 1: valid message
  Serial.println("\n[Scenario 1] Sending valid message:");
  uart_init(&uart, expected);
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print("  Receiving byte: 0x");
    Serial.println(valid_msg[i], HEX);
    uart_receive_byte(&uart, valid_msg[i]);
  }
  uart_print_state(&uart);

  // Scenario 2: parity error (0x01 has odd parity)
  Serial.println("\n[Scenario 2] Injecting parity error:");
  uart_init(&uart, expected);
  uint8_t scenario2[] = {0x48, 0x01, 0x33, 0x0F,
                          0xAA, 0x96, 0x69, 0xC3};
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print("  Receiving byte: 0x");
    Serial.println(scenario2[i], HEX);
    uart_receive_byte(&uart, scenario2[i]);
  }
  uart_print_state(&uart);

  // Scenario 3: CRC mismatch
  Serial.println("\n[Scenario 3] CRC mismatch:");
  uart_init(&uart, 0xAB);
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print("  Receiving byte: 0x");
    Serial.println(valid_msg[i], HEX);
    uart_receive_byte(&uart, valid_msg[i]);
  }
  uart_print_state(&uart);
}

// ---- Main ----

void setup() {
  Serial.begin(9600);
  delay(1000);
  run_parity_tests();
  run_crc_tests();
  run_reg_tests();
  run_uart_sim();
}

void loop() {}