#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
//Array name is KHEAP_ARR
int nextFit = 0;
void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT 2025 - MS1 - [1] Kernel Heap] kmalloc()
	// Write your code here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");

	//NOTE: Allocation is based on FIRST FIT strategy
	//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer
	uint32 i=nextFit;
	uint32 start=-1;

	do{
		if(start !=-1 && i-start==ROUND_UP(size,PAGE_SIZE)/PAGE_SIZE)
				{

					struct Frame_Info* fptr=NULL;
					for(uint32 j=start;j<i;j=(j+1)%KHEAP_ARR_SIZE)
					{
						allocate_frame(&fptr);
						if(fptr==NULL)
						{ cprintf("\n no frame found for page, memory is FULL\n");
							return NULL;
						}
						int res=map_frame(ptr_page_directory,fptr,(j*PAGE_SIZE)+KERNEL_HEAP_START);
						if(res!=0)
						{ 	cprintf("\n no frame found for page table, memory is FULL\n");
							return NULL;
						}
					}
					KHEAP_ARR[start]=i-start;
					nextFit=i;
					return (start*PAGE_SIZE)+KERNEL_HEAP_START;
				}
				else if(KHEAP_ARR[i]==-1 && start==-1) start=i;
				else if(KHEAP_ARR[i]!=-1)
				{
					i=(i+KHEAP_ARR[i])%KHEAP_ARR_SIZE;
					start=-1;
					continue;
				}
				i=(i+1);
				if(i==KHEAP_ARR_SIZE) {start=-1; i=0;}
	}while(i != nextFit);


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
				va += (physical_address & 4095);
				return va;
			}
		}
		va += PAGE_SIZE;
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


	return frameNo * PAGE_SIZE;
}

void *krealloc(void *virtual_address, uint32 new_size)
{
	panic("krealloc() is not required...!!");
	return NULL;

}
