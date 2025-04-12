#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)

void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT 2025 - MS1 - [1] Kernel Heap] kmalloc()
	// Write your code here, remove the panic and write your code
	kpanic_into_prompt("kmalloc() is not implemented yet...!!");

	//NOTE: Allocation is based on FIRST FIT strategy
	//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer
	return NULL;
}


void kfree(void* virtual_address)
{
	//TODO: [PROJECT 2025 - MS1 - [1] Kernel Heap] kfree()
	// Write your code here, remove the panic and write your code
	panic("kfree() is not implemented yet...!!");
	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details
	

}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT 2025 - MS1 - [1] Kernel Heap] kheap_virtual_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");

	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer
	uint32 va = KERNEL_HEAP_START;
	struct Frame_Info* ptr = NULL;
	uint32* pt_ptr = NULL;
	uint32 pa;
	while(va != KERNEL_HEAP_MAX){
		ptr = get_frame_info(ptr_page_directory, (void*)va, &pt_ptr);
		if(ptr != NULL){
			pa = to_physical_address(ptr);
			if((pa >> 12) == (physical_address >> 12)){
				va += (physical_address & 4059);
				return va;
			}
		}
		va += 4096;
	}
	
	
	return -1;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT 2025 - MS1 - [1] Kernel Heap] kheap_physical_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer
		
	uint32* page_table_ptr = NULL;
		
	get_page_table(ptr_page_directory, (void *)virtual_address, 0, &page_table_ptr);
	if(page_table_ptr == NULL){
		return -1;
	}
	uint32 pte_ptr = page_table_ptr[PTX(virtual_address)];
	uint32 frameNo = pte_ptr >> 12;
	

	return frameNo * 4096;
}

void *krealloc(void *virtual_address, uint32 new_size)
{
	panic("krealloc() is not required...!!");
	return NULL;

}