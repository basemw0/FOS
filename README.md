# FOS Kernel - Memory Management 
**OS 2025 Course Project**  
*Ain Shams University, Faculty of Computer and Ai Engineering*

A kernel implementation for virtual memory management, page replacement, and dynamic allocation.

## Project Overview
Implements core OS memory management components:
- Kernel/User heap allocators (Best Fit/First Fit/Worst Fit) *(kheap.c/uheap.c)*
- Page fault handler with Modified Clock replacement *(trap.c)*
- Page buffering and working set management *(memory_manager.c)*
- System calls for `malloc`/`free`

## Key Features
- **Virtual Memory**: Page file operations, working set tracking
- **Allocation Strategies**: Configurable kernel/user heap policies
- **Page Replacement**: Buffered modified/free lists, Modified Clock algorithm
- **System Integration**: Environment lifecycle management, user-kernel mode switching

## Authors
[Basem Walid](https://github.com/basemw0)

[Amir Tamer](https://github.com/amirtamer-27)

[Mohamed Wael Badra](https://github.com/MohamedWBadra)

[Mohamed Wael ElSayed](https://github.com/memoo221)

[Moaz Ahmed](https://github.com/Moaz715)

[Youssef Waleed](https://github.com/weloo11)

