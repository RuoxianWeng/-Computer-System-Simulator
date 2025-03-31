# Computer System Simulator

## Overview

This project is a computer system simulator implemented in C on a Linux environment. It mimics the behavior of a simple computer system consisting of a CPU and memory, utilizing interprocess communication (IPC) through fork() and pipe(). The CPU fetches and executes instructions, while the memory module handles read/write operations and enforces memory access rules.

## Features

- Interprocess Communication: Uses pipe() for data exchange between CPU and memory processes.

- Process Management: Utilizes fork() to create separate CPU and memory processes.

- Interrupt Handling: Supports a timer-based interrupt mechanism for transitioning between user mode and kernel mode.

- Memory Management: Implements a segmented memory structure with protected system regions.

- Instruction Execution: Supports various instructions such as load, store, arithmetic operations, jumps, stack operations, and system calls.

## Components
### `computerSystem.c`

- Initializes the system and sets up interprocess communication.
- Creates separate processes for CPU and memory using fork().
- Passes necessary command-line arguments to CPU and memory processes.

### `cpu.c`

- Fetches and executes instructions.
- Manages CPU registers (PC, SP, AC, X, Y).
- Handles memory read/write operations via pipes.
- Implements timer-based interrupts and system calls.

### `memory.c`

- Loads the program into memory from an input file.
- Responds to read and write requests from the CPU.
- Ensures memory protection between user and system space.
