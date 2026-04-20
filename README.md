# Mano-Machine-Assembler
Built a custom 16-bit ISA with 25 micro-instruction map to binary, defining my own syntax. The ISA adheres to Morris Mano Single Accumulator computer, the assembler genrate the code compatible with Mano Machine(⚠️Done slight optimisation).

# Mano-Machine-Assembler 🖥️

A robust 2-pass assembler written in C, designed to translate custom assembly syntax into 16-bit binary machine code for the **Morris Mano Single Accumulator Computer**.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Project Goals
The bridge between high-level logic and hardware is often invisible. This project was built to demystify that gap by creating a tool that handles:
* **Label Resolution:** Mapping human-readable branch targets to memory addresses.
* **Instruction Encoding:** Packing opcodes, indirect bits, and addresses into 16-bit words.
* **Memory Management:** Separating data (VAR) and instruction segments within a custom ISA.

---

## Quick Start

### 1. Clone the repository
```bash
git clone https://github.com/AnshuNegi2006/Mano-Machine-Assembler.git
cd Mano-Machine-Assembler
```

### 2. Compilation
Use any standard C compiler (GCC recommended):
```bash
gcc Assembler.c -o Assembler
```

###3. Run the Assembler
Feed your source file (ending in ;) into the assembler to generate a binary output:
```bash
./Assembler input_file.sys
```
###🛠 Features
***Custom Syntax:*** Supports a unique LABEL: NAME : and VAR structure for easy readability.

***Strict Memory Safety:*** Implements protection against buffer overflows and handles NULL pointer exceptions during tokenization.

***Big-Endian Output:*** Utilizes htons() to ensure the generated binary follows hardware-standard byte ordering.

***Full ISA Support:*** Complete mapping for all 25 Morris Mano micro-instructions (MRI, RRI, and I/O)


***For a deep dive into the grammar, memory mapping, and full instruction set, please refer to the Documentation.md file.***
