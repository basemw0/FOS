# FOS Kernel - Memory Management (Milestone 2)  
**OS 2025 Course Project**  
*Ain Shams University, Faculty of Computer and Information Sciences*

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A kernel implementation for virtual memory management, page replacement, and dynamic allocation.

## Project Overview
Implements core OS memory management components:
- Kernel/User heap allocators (Best Fit/First Fit/Worst Fit)
- Page fault handler with Modified Clock replacement
- Page buffering and working set management
- System calls for `malloc`/`free`

## Key Features
- **Virtual Memory**: Page file operations, working set tracking
- **Allocation Strategies**: Configurable kernel/user heap policies
- **Page Replacement**: Buffered modified/free lists, Modified Clock algorithm
- **System Integration**: Environment lifecycle management, user-kernel mode switching

## Build & Run
**Prerequisites**:  
- `gcc`, `nasm`, `make`, Bochs/QEMU

**Compile**:  
```bash
git clone https://github.com/yourusername/fos-kernel.git
cd fos-kernel
make all
