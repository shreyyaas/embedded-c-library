# Bare-Metal Embedded C Library
### Arduino Uno R4 WiFi (Renesas RA4M1 — ARM Cortex-M4)

A from-scratch embedded C library implementing register-level hardware control,
error detection algorithms, and a UART-style data reception pipeline — built
without Arduino abstraction layers.

---

## Motivation

Most Arduino projects use high-level functions like `digitalWrite()` that hide
what's actually happening at the hardware level. This project bypasses those
abstractions entirely, directly manipulating the Renesas RA4M1's memory-mapped
registers in C — the same approach used in production embedded firmware.

---

## Project Structure

├── bit_utils.h # Core bit manipulation macros
├── parity.h # Even/odd parity checker
├── reg_sim.h # Peripheral register simulator
├── uart_sim.h # UART-style receiver pipeline
└── embedded_bit_library.ino # Main: test runners and implementations


---

## Modules

### 1. Bit Manipulation Library (`bit_utils.h`)
Zero-overhead macros for register-level bit operations:

| Macro | Operation |
|---|---|
| `BIT_SET(reg, bit)` | Set a specific bit HIGH |
| `BIT_CLEAR(reg, bit)` | Clear a specific bit LOW |
| `BIT_TOGGLE(reg, bit)` | Toggle a specific bit |
| `BIT_READ(reg, bit)` | Read a specific bit |

Used to control the onboard LED directly via `R_PORT1->PDR` and `R_PORT1->PODR`
registers, bypassing `digitalWrite()`.

---

### 2. Parity Checker (`parity.h`)
Implements even/odd parity detection using an XOR-chain algorithm — the
software equivalent of a hardware parity tree.

- `compute_parity(byte)` — returns 0 for even, 1 for odd
- `check_even_parity(byte)` — boolean even parity check
- `check_odd_parity(byte)` — boolean odd parity check

---

### 3. Peripheral Register Simulator (`reg_sim.h`)
Models a microcontroller status register with named bit flags:

Bit: 7 6 5 4 3 2 1 0
ERR OVF RDY INT TXE RXF -- --


Simulates a real peripheral lifecycle:
- Device power-up → RDY set
- Data received → RXF set
- Buffer overflow → OVF + ERR set
- ISR error handling → flags cleared

---

### 4. UART Receiver Pipeline (`uart_sim.h`)
A complete data reception pipeline combining all three modules:

Incoming byte
↓
Parity check → FAIL? → set ERR flag, reject byte
↓ PASS
Buffer accepted → set RXF flag
↓
Buffer full? → CRC-8 verify → FAIL? → set ERR flag
↓ PASS
[CRC OK] — message verified


**Three scenarios tested:**
- ✅ Valid 8-byte message — clean receive, CRC verified
- ❌ Parity error injection — corrupted byte rejected, ERR flag set
- ❌ CRC mismatch — full message received but checksum fails, ERR flag set

---

## Hardware

| Component | Detail |
|---|---|
| Board | Arduino Uno R4 WiFi |
| MCU | Renesas RA4M1 |
| Architecture | ARM Cortex-M4 |
| Register access | Direct via `R_PORT1->PDR`, `R_PORT1->PODR` |

---

## Key Concepts Demonstrated

- Memory-mapped I/O and direct register manipulation
- Bitwise operations at the hardware abstraction layer
- XOR-based parity detection (equivalent to hardware parity trees)
- CRC-8 polynomial division for error detection
- C structs and pointers for peripheral modelling
- Embedded firmware design patterns (init, set, clear, read, verify)

---

## Relevance to Digital Design

The error detection algorithms implemented here — parity and CRC — are
the same mechanisms used in digital logic verification. The register
simulator directly models the status register behaviour inspected by
DFT tools during scan testing, bridging hardware verification concepts
with embedded firmware implementation.

---

## Author
Shreyas S A  
Electronics and Communication Engineering, PES University  
github.com/shreyyaas
