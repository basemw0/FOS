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
bool initializeFlag = 0;
int callCount =0;

void* kmalloc(unsigned int size){
		//Initializing the KHEAP_ARR with -1 once
		if(initializeFlag == 0){
			for(int i = 0; i < KHEAP_ARR_SIZE; i++){
				KHEAP_ARR[i] = -1;
			}
			initializeFlag = 1;
		}

		if(size == 0)
			return NULL;

		if(callCount < 4 || isKHeapPlacementStrategyNEXTFIT()){
			callCount++;

/* NextFit (Our Main) */
		//i --> current index in the KHEAP_ARR
		//start --> hold the start index of a free block
		//count --> counts how many consecutive pages have been found
		//limit --> counter to limit loops to one cycle and prevent infinite looping

		uint32 num_pages = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
		uint32 i = nextFit;
		uint32 start = -1;
		uint32 count = 0;

		uint32 limit = 0;
		while(limit < KHEAP_ARR_SIZE) {
			//Found empty place
			if(KHEAP_ARR[i] == -1) {
				if(start == -1)
					start = i;
				count++;
				if(count == num_pages) {
					for(uint32 j = 0; j < num_pages; j++) {
						uint32 page_index = (start + j) % KHEAP_ARR_SIZE;
						struct Frame_Info* fptr = NULL;

						allocate_frame(&fptr);
						if(fptr == NULL) {
							//No frame found because Memory is full
							return NULL;
						}
						uint32 va = KERNEL_HEAP_START + (page_index * PAGE_SIZE);
						int res = map_frame(ptr_page_directory, fptr, (void*)va, PERM_WRITEABLE);
						if(res != 0) {
							return NULL;
						}
					}
					//Update the start index with the number of pages this process was allocated
					KHEAP_ARR[start] = num_pages;
					//Update nextFit
					nextFit = (start + num_pages) % KHEAP_ARR_SIZE;
					//return address
					return (void*)(KERNEL_HEAP_START + (start * PAGE_SIZE));
				}
			}
			else {
				// if you hit an allocated block just skip it
				if(KHEAP_ARR[i] > 0) {
					i = (i + KHEAP_ARR[i]) % KHEAP_ARR_SIZE;
				}
				else {
					i = (i + 1) % KHEAP_ARR_SIZE;
				}
				start = -1;
				count = 0;
				limit++;
				continue;
			}

			// Move to next slot
			i = (i + 1) % KHEAP_ARR_SIZE;
			limit++;
		}

		return NULL;

		}else if(isKHeapPlacementStrategyFIRSTFIT()){

/* FirstFit*/

			if(size == 0)
				return NULL;

			//i --> current index in the KHEAP_ARR
			//start --> hold the start index of a free block
			//count --> counts how many consecutive pages have been found
			uint32 num_pages = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
			uint32 i = 0;
			uint32 start = -1;
			uint32 count = 0;


			while(i < KHEAP_ARR_SIZE) {
				//Found empty place
				if(KHEAP_ARR[i] == -1) {
					if(start == -1)
						start = i;
					count++;
					if(count == num_pages) {
						for(uint32 j = 0; j < num_pages; j++) {
							uint32 page_index = (start + j) % KHEAP_ARR_SIZE;
							struct Frame_Info* fptr = NULL;

							allocate_frame(&fptr);
							if(fptr == NULL) {
								//No frame found because Memory is full
								return NULL;
							}
							uint32 va = KERNEL_HEAP_START + (page_index * PAGE_SIZE);
							int res = map_frame(ptr_page_directory, fptr, (void*)va, PERM_WRITEABLE);
							if(res != 0) {
								return NULL;
							}
						}
						//Update the start index with the number of pages this process was allocated
						KHEAP_ARR[start] = num_pages;
						//return address
						return (void*)(KERNEL_HEAP_START + (start * PAGE_SIZE));
					}
				}
				else {
					// if you hit an allocated block just skip it
					if(KHEAP_ARR[i] > 0) {
						i = (i + KHEAP_ARR[i]) % KHEAP_ARR_SIZE;
					}
					else {
						i = (i + 1) % KHEAP_ARR_SIZE;
					}
					start = -1;
					count = 0;
					continue;
				}

				// Move to next slot
				i = i + 1;
			}

			return NULL;
		}else if(isKHeapPlacementStrategyWORSTFIT()){

/* WORSTFIT / BESTFIT (change the lines with the corresponding comments to get BestFit) */

	int process_size = ROUNDUP(size,PAGE_SIZE)/PAGE_SIZE;
	int i =0;
	int worst =-1;       //KHEAP_ARR_SIZE+1;
	int worstStart =-1;
	int start =-1;
	int count = 0;
	while(i < KHEAP_ARR_SIZE){
		if(KHEAP_ARR[i]==-1){
			if (start == -1)
				start = i;

			while(i <KHEAP_ARR_SIZE && KHEAP_ARR[i]== -1){
				count++;
				i++;
			}
		//if(count >= process_size && count < worst) {
			if(count>worst){
			worst = count;
			worstStart = start;
			}
		}
		else if(KHEAP_ARR[i]>0){

			i  += KHEAP_ARR[i] ;
			start =-1;
			count =0;
			continue;
			}
		else{
				i ++;
			}
		}

	if(worst >= process_size){
		for(uint32 j = 0; j < process_size; j++) {
			uint32 page_index = worstStart + j;
				struct Frame_Info* fptr = NULL;

				allocate_frame(&fptr);
				if(fptr == NULL) {
					//No frame found because Memory is full
					return NULL;
				}
				uint32 va = KERNEL_HEAP_START + (page_index * PAGE_SIZE);
				int res = map_frame(ptr_page_directory, fptr, (void*)va, PERM_WRITEABLE);
				if(res != 0) {
					return NULL;
				}
			}
			//Update the start index with the number of pages this process was allocated
			KHEAP_ARR[worstStart] = process_size;
			//return address
			return (void*)(KERNEL_HEAP_START + (worstStart * PAGE_SIZE));
	}else{
		return NULL;
	}

	} else if(isKHeapPlacementStrategyBESTFIT()){
		int process_size = ROUNDUP(size,PAGE_SIZE)/PAGE_SIZE;
			int i =0;
			int best = KHEAP_ARR_SIZE+1;
			int bestStart =-1;
			int start =-1;
			int count = 0;
			while(i < KHEAP_ARR_SIZE){
				if(KHEAP_ARR[i]==-1){
					if (start == -1)
						start = i;

					while(i <KHEAP_ARR_SIZE && KHEAP_ARR[i]== -1){
						count++;
						i++;
					}
				if(count >= process_size && count < best) {
					best = count;
					bestStart = start;
					}
				}
				else if(KHEAP_ARR[i]>0){

					i  += KHEAP_ARR[i] ;
					start =-1;
					count =0;
					continue;
					}
				else{
						i ++;
					}
				}

			if(best >= process_size){
				for(uint32 j = 0; j < process_size; j++) {
					uint32 page_index = bestStart + j;
						struct Frame_Info* fptr = NULL;

						allocate_frame(&fptr);
						if(fptr == NULL) {
							//No frame found because Memory is full
							return NULL;
						}
						uint32 va = KERNEL_HEAP_START + (page_index * PAGE_SIZE);
						int res = map_frame(ptr_page_directory, fptr, (void*)va, PERM_WRITEABLE);
						if(res != 0) {
							return NULL;
						}
					}
					//Update the start index with the number of pages this process was allocated
					KHEAP_ARR[bestStart] = process_size;
					//return address
					return (void*)(KERNEL_HEAP_START + (bestStart * PAGE_SIZE));
			}else{
				return NULL;
			}
	}
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
	physical_address =ROUNDDOWN(physical_address,PAGE_SIZE);
    for (int i = 0; i < KHEAP_ARR_SIZE; i++) {
        if (KHEAP_ARR[i] == -1) {
            continue;
        }

        int block_size = KHEAP_ARR[i];
        for (int j = 0; j < block_size; j++) {
            uint32 va = KERNEL_HEAP_START + (i + j) * PAGE_SIZE;

            struct Frame_Info* ptr = NULL;
            uint32* pt_ptr = NULL;
            ptr = get_frame_info(ptr_page_directory, (void*)va, &pt_ptr);

            if (ptr != NULL) {
                uint32 pa = to_physical_address(ptr);
                if (pa == physical_address) {
                    return va + (physical_address & (PAGE_SIZE - 1));
                }
            }
        }

        i += block_size - 1;
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
