# Mano-Machine-Assembler: Language Specification

This documentation provides the syntax and structural rules for the custom assembly language designed for the Mano-Machine-Assembler. This assembler follows the Morris Mano ISA logic with a custom-built parser and syntax.

## 1. Core Syntax Rules
- **Line Termination:** Every instruction and declaration MUST end with a semicolon (`;`).
- **Case Sensitivity:** All keywords (`VAR`, `LABEL`) and Instruction Mnemonics (e.g., `LDA`, `ADD`) must be in **UPPERCASE**.
- **Whitespaces:** The parser is designed to ignore leading spaces and tabs, but instructions must be separated from operands by at least one space.

-------------------------------

## 2. Variables (VAR)
Variables are used to allocate memory and store initial values. They are processed in the Data Segment.
- **Syntax:** `VAR [variable_name] [value];`
- **Constraint:** Variable names should be 1-4 characters long for optimal memory alignment.

- **Example:**
VAR val1 15;
VAR temp 0;

-------------------------------

## 3. Labeling System (LABEL)
This assembler uses a unique "Triple-Token" format for labels to identify branch targets for loops and jumps.
- **Syntax:** `LABEL: [label_name] : [Instruction];`
- **Constraint:** The label name must be followed by a colon before the instruction begins.
  
- **Example:**
LABEL: START : LDA val1;
LABEL: LOOP : ADD temp;

-------------------------------

## 4. Instruction Set (ISA) Mapping
The assembler maps mnemonics to a 16-bit binary word. Memory Reference Instructions (MRI) automatically handle the Indirect (I) bit and address mapping.

### Memory Reference Instructions (MRI)
Requires an operand (Variable or Label name).
- `AND`, `ADD`, `LDA`, `STA`, `BUN`, `BSA`, `ISZ`

### Register-Reference Instructions (RRI)
Standalone instructions (No operand).
- `CLA`, `CLE`, `CMA`, `CME`, `CIR`, `CIL`, `INC`, `SPA`, `SNA`, `SZA`, `SZE`, `HLT`

### Input/Output Instructions (I/O)
Standalone instructions for hardware communication.
- `INP`, `OUT`, `SKI`, `SKO`, `ION`, `IOF`

-------------------------------

## 5. Memory Map & Processing
The assembler performs a **Two-Pass** process:
1. **Pass 1:** It scans for `VAR` and `LABEL` tags to build a Symbol Table, calculating their hex addresses.
2. **Pass 2:** It translates mnemonics into Opcode bits and replaces the names with the hex addresses from Pass 1.
3. **End Result:** Generates a Big-Endian binary file compatible with 16-bit hardware architecture.

-------------------------------

## 6. Full Program Example
```text
VAR a 5;
VAR b 10;
VAR res 0;

LABEL: CALC : LDA a;
  ADD b;
  STA res;
  HLT;
```

-------------------------------
