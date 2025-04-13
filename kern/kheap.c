#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>
//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
int KHEAP_ARR[KHEAP_ARR_SIZE];

/*
 KHEAP_ARR Values
 Value = -1 -> Free space
 Value > 0 -> block_size(IN PAGES) taken by this page number
 */

int nextFit = 0;
bool flag = 0;
void* kmalloc(unsigned int size)
{
	if(flag == 0){
			for(int i =0;i<KHEAP_ARR_SIZE;i++){
				KHEAP_ARR[i]=-1;
				}
			flag=1;
			}
		if(size == 0)
			return NULL;
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

						if(KHEAP_ARR[i]==-1 )
						{
							if(start==-1)start=i;
						if(start !=-1 && i+1-start==ROUNDUP(size,PAGE_SIZE)/PAGE_SIZE)
											{

												struct Frame_Info* fptr=NULL;
												for(uint32 j=start;j<=i;j++)
												{
													allocate_frame(&fptr);
													if(fptr==NULL)
													{ cprintf("\n no frame found for page, memory is FULL\n");
														return NULL;
													}
													int res=map_frame(ptr_page_directory,fptr,(void*)((j*PAGE_SIZE)+KERNEL_HEAP_START), PERM_WRITEABLE);
													if(res!=0)
													{ 	cprintf("\n no frame found for page table, memory is FULL\n");
														return NULL;
													}
												}
												KHEAP_ARR[start]=i+1-start;
												nextFit=(i+1)%KHEAP_ARR_SIZE;
												return (void*)((start*PAGE_SIZE)+KERNEL_HEAP_START);
											}
						}
						else
						{
							start=-1;
								i=(i+KHEAP_ARR[i])%KHEAP_ARR_SIZE;
								continue;

						}
						i++;
						if(i==KHEAP_ARR_SIZE){
							start=-1; i=0;
						}
			}while(i != nextFit);


			return NULL;
}

void kfree(void* virtual_address)
{
	if ((uint32)virtual_address < KERNEL_HEAP_START || (uint32)virtual_address >= KERNEL_HEAP_MAX)
	        return;

	uint32 va = (uint32)virtual_address;
	uint32 page_number = (va - KERNEL_HEAP_START)/PAGE_SIZE;

	if (KHEAP_ARR[page_number] == -1)
		        return;

	uint32 block_size = KHEAP_ARR[page_number];
	uint32 end_address = va + (block_size * PAGE_SIZE);
	uint32 * ptr_page = NULL;
	for(int i =0;i<block_size;i++){
		KHEAP_ARR[page_number+i] = -1;
	}
	for (uint32 address = va; address < end_address ;address += PAGE_SIZE ) {
	        struct Frame_Info* frame_info = get_frame_info(ptr_page_directory, (void*)address, &ptr_page);
	        if (frame_info != NULL)
           {
               free_frame(frame_info);
           }
           unmap_frame(ptr_page_directory, (void*)address);
       }

}


unsigned int kheap_virtual_address(unsigned int physical_address)
{

	uint32 va = KERNEL_HEAP_START;
	struct Frame_Info* ptr = NULL;
	uint32* pt_ptr = NULL;
	uint32 pa;
	while(va < KERNEL_HEAP_MAX){
		ptr = get_frame_info(ptr_page_directory, (void*)va, &pt_ptr);
		if(ptr != NULL){
			pa = to_physical_address(ptr);
			if((pa >> 12) == (physical_address >> 12)){
				return va + (physical_address & (PAGE_SIZE - 1));
			}

		}
		va += PAGE_SIZE;
	}
	return 0;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{


	uint32* page_table_ptr = NULL;

	get_page_table(ptr_page_directory, (void *)virtual_address, &page_table_ptr);
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
