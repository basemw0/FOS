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
		//Initializing once the KHEAp array values to -1 which means that this frame is free
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

			uint32 i=nextFit;//start searching from where we have ended the last allocation
			uint32 start=-1;//holds a possible  frame that will be the start of our allocation

			do{

						if(KHEAP_ARR[i]==-1 )//free frame
						{
							if(start==-1)start=i;//if we were not already holding a start
												//make this frame the start that we are going to count from
						if(start !=-1 && i+1-start==ROUNDUP(size,PAGE_SIZE)/PAGE_SIZE)//if there was a start and we have reached
																					 //the desired # of frames
											{

												struct Frame_Info* fptr=NULL;
												for(uint32 j=start;j<=i;j++)//allocating each frame
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
													fptr->va = (j*PAGE_SIZE)+KERNEL_HEAP_START;
												}
												KHEAP_ARR[start]=i+1-start;//setting the start frame in the array with the number of frames used after it
												nextFit=(i+1)%KHEAP_ARR_SIZE;//setting the nextfit ptr to the next frame after the last one we have allocated
												return (void*)((start*PAGE_SIZE)+KERNEL_HEAP_START);//return virtual address of the start frame
											}
						}
						else//if the frame is full
						{
							start=-1;//reset the start to find another free space
								i=(i+KHEAP_ARR[i])%KHEAP_ARR_SIZE;//jump to the next possible free frame
								continue;

						}
						i++;
						if(i==KHEAP_ARR_SIZE){//if the kheap has ended and we have not reached our start (nextfit)
							start=-1; i=0;//start from the beginning of the kheap and reset the start to find another free space
						}
			}while(i != nextFit);//continue until checking every frame possible


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
							fptr->va = KERNEL_HEAP_START + (page_index * PAGE_SIZE);
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
				fptr->va = j*PAGE_SIZE + KERNEL_HEAP_START;
			}
			//Update the start index with the number of pages this process was allocated
			KHEAP_ARR[worstStart] = process_size;
			//return address
			return (void*)(KERNEL_HEAP_START + (worstStart * PAGE_SIZE));
	}else{
		return NULL;
	}

	} else if(isKHeapPlacementStrategyBESTFIT()){
		//BESFT FIT STRATEGY
		//process_sise -> input size of process rounded to nearest page
		//i-> iterator over the heap
		//start-> points to a possible start for allocation when iterating
		//VA-> our final Start
		//pageCounter-> counts pages that are free
		//BestFit-> stores the size of the current best fit of the process
	 int process_size , i , start , VA ,pageCounter , BestFit;
	 process_size = ROUNDUP(size,PAGE_SIZE)/PAGE_SIZE;
	 i = 0;
	 start = -1;
	 VA = -1; // initialize to -1 , since we still dont know if there is space for the allocation
	 pageCounter = 0;
	 BestFit = KHEAP_ARR_SIZE+1; // initialize to a num greater than the heap size to allow comparision
	 while(i<KHEAP_ARR_SIZE){
		//If we dont have a start currently and the page we are on is free set the start to that page number
		 if(KHEAP_ARR[i] == -1&& start == -1) start = i;
		 // if we hit a block that is allocated check if we had a start we will compare to see how many pages were free up to that point , if it is suitable for our process and better than the best fit we update Bestfit and VA.
		 else if (KHEAP_ARR[i] != -1 ){
			 if(start!= -1){
			 if(pageCounter >= process_size && pageCounter < BestFit){
					 BestFit = pageCounter;
					 VA = start;

			 }}
			 //Jump the allocated block
			 i+=KHEAP_ARR[i];
			 start = -1;
			 pageCounter = 0 ;
			 continue;

		 }
		 //incrementing i and page counter to count how many pages the free block contains
		 i++;
		 pageCounter++;

	 }
	 //This condition is specifically for allocations at the end of the kernel heap since we wont be able to check the condition in the loop because i will be = to the KH size
	 if(start!=-1){
				 if(pageCounter >= process_size && pageCounter < BestFit){
						 BestFit = pageCounter;
						 VA = start;

				 }
			 }
	//If our VA was set to something other than -1 that means we found a space to allocate the block so we start allocating.
	 if(VA!=-1){
		 struct Frame_Info* fptr=NULL;
		 for(uint32 j=VA;j<VA + process_size;j++)
		 {
			 allocate_frame(&fptr);
			 if(fptr==NULL)
			 {
				 cprintf("\n no frame found for page, memory is FULL\n");
				 return NULL;
			 }

			 int res=map_frame(ptr_page_directory,fptr,(void*)((j*PAGE_SIZE) + KERNEL_HEAP_START), PERM_WRITEABLE);
			 if(res!=0)
			 {
				 cprintf("\n no frame found for page table, memory is FULL\n");
				 return NULL;
			 }
			 fptr->va = (j*PAGE_SIZE) + KERNEL_HEAP_START; //this line here is to make the lookup process in the physical to vitrual function O(1)
		 }
		 //Update our array and return the VA
		 KHEAP_ARR[VA]=process_size;
		 return (void*)((VA*PAGE_SIZE)+KERNEL_HEAP_START);
	 }
	 return NULL;
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
	            unmap_frame(ptr_page_directory, (void*)address);
           }
       }

}


unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//This is our previous O(n) solution
	// physical_address =ROUNDDOWN(physical_address,PAGE_SIZE);
    // for (int i = 0; i < KHEAP_ARR_SIZE; i++) {
    //     if (KHEAP_ARR[i] == -1) {
    //         continue;
    //     }

    //     int block_size = KHEAP_ARR[i];
    //     for (int j = 0; j < block_size; j++) {
    //         uint32 va = KERNEL_HEAP_START + (i + j) * PAGE_SIZE;

    //         struct Frame_Info* ptr = NULL;
    //         uint32* pt_ptr = NULL;
    //         ptr = get_frame_info(ptr_page_directory, (void*)va, &pt_ptr);

    //         if (ptr != NULL) {
    //             uint32 pa = to_physical_address(ptr);
    //             if (pa == physical_address) {
    //                 return va + (physical_address & (PAGE_SIZE - 1));
    //             }
    //         }
    //     }

    //     i += block_size - 1;
    // }

    // return 0;

	//Here we use the property va that we set in the frame_info struct while allocating to find the va of a the given physical address, turn it into a frame number , get the page number(va) , then add back the offset
	struct Frame_Info* ptr = NULL;
	ptr = to_frame_info(physical_address);
	uint32 va = ptr->va;
	if(va < KERNEL_HEAP_START) return 0;
	return va  + physical_address%PAGE_SIZE;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{

	//We use the get_page_table function to get a pointer on the PT that has the allocated from for this virtual address
	uint32* page_table_ptr = NULL;

	get_page_table(ptr_page_directory, (void *)virtual_address, &page_table_ptr);
	if(page_table_ptr == NULL){
		return -1;
	}
	//we get the entry from the page table using PTX which returns the middle 10 bits of the VA that have the page number in the PT
	uint32 pte_ptr = page_table_ptr[PTX(virtual_address)];
	//Shift it 12 bits to the right to get the frame number alone without the control bits
	uint32 frameNo = pte_ptr >> 12;
	//Return the physical address after adding back the offset
	return frameNo * PAGE_SIZE + (virtual_address%PAGE_SIZE);
}

void *krealloc(void *virtual_address, uint32 new_size)
{
	panic("krealloc() is not required...!!");
	return NULL;

}
