#include "icsmm.h"
#include "debug.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
 
/*
 * The allocator MUST store the head of its free list in this variable. 
 * Doing so will make it accessible via the extern keyword.
 * This will allow ics_freelist_print to access the value from a different file.
 */
 ics_header *epilogue = NULL;
 ics_footer *prologue = NULL;
 bool start = 1;
ics_free_header *freelist_head = NULL;
void **heapStart = NULL;
int totalPages = 1;
size_t sizeHolder;

/*
 * This is your implementation of malloc. It acquires uninitialized memory from  
 * ics_inc_brk() that is 16-byte aligned, as needed.
 *
 * @param size The number of bytes requested to be allocated.
 *
 * @return If successful, the pointer to a valid region of memory of at least the
 * requested size is returned. Otherwise, NULL is returned and errno is set to 
 * ENOMEM - representing failure to allocate space for the request.
 * 
 * If size is 0, then NULL is returned and errno is set to EINVAL - representing
 * an invalid request.
 */
void *ics_malloc(size_t size) { 

    if(size == 0){
        errno = EINVAL;
        return NULL;
    }

    void *brk = NULL;

    if(start){

        sizeHolder = size;

        while(sizeHolder > PAGE_SIZE - 16){
            sizeHolder = sizeHolder - PAGE_SIZE;
            totalPages++;
        }

        if(totalPages > 6){
            errno = ENOMEM;
            return NULL;
        }

        brk = ics_inc_brk(totalPages);

        if(brk < 0){
            return NULL;
        }

        heapStart = &brk;
        void *hStart = (*heapStart);

        ics_footer *pro = hStart;
        prologue = pro;
        pro->block_size = 0;
        pro->fid = PROLOGUE_MAGIC;

        ics_header *head = hStart + 8;
        head->block_size = (totalPages * PAGE_SIZE) - 16;
        head->padding_amount = 0;
        head->hid = HEADER_MAGIC;


        ics_footer *foot = hStart + ((totalPages * PAGE_SIZE) - 16);
        foot->block_size = (totalPages * PAGE_SIZE) - 16;
        foot->fid = FOOTER_MAGIC;

        ics_header *ep = hStart + ((totalPages * PAGE_SIZE) - 8);
        epilogue = ep;
        ep->block_size = 0;
        ep->padding_amount = 0;
        ep->hid = EPILOGUE_MAGIC;

        freelist_head = (ics_free_header *)head;
        freelist_head->header = *head;
        freelist_head->next = NULL;
        freelist_head->prev = NULL;

        start = 0;
    }

    ics_free_header *tempFreeHeader = freelist_head;
    bool splitBlock = 0;
    int padding = 0;

    while(tempFreeHeader != NULL){

        if(size % 16 > 0){
            padding = 16 - (size % 16);
        }

        else{
            padding = 0;
        }

        if(tempFreeHeader->header.block_size > size + padding){
            
            void *currPos = (void *)tempFreeHeader;
            int oldBlockSize = tempFreeHeader->header.block_size;
            int i = 0;

            i = oldBlockSize - size - padding - 16;

            if(i > 16){

                ics_header *newHeader = currPos;
                newHeader->hid = HEADER_MAGIC;
                newHeader->block_size = size + padding + 16 + 1;
                newHeader->padding_amount = padding;
                currPos = currPos + 8;

                currPos = currPos + padding + size;

                ics_footer *tempFooter = currPos;
                tempFooter->fid = FOOTER_MAGIC;
                tempFooter->block_size = size + padding + 16 + 1;
                currPos = currPos + 8;

                ics_header *tempHeader = currPos;
                tempHeader->block_size = oldBlockSize - newHeader->block_size + 1;
                tempHeader->hid = HEADER_MAGIC;
                tempHeader->padding_amount = 0;

                currPos = (void *)newHeader;
                currPos = currPos + oldBlockSize - 8;

                ics_footer *chgFooter = currPos;
                chgFooter->fid = FOOTER_MAGIC;
                chgFooter->block_size = oldBlockSize - newHeader->block_size + 1;



                if(tempFreeHeader->prev != NULL && tempFreeHeader->next != NULL){
                    tempFreeHeader->prev->next = tempFreeHeader->next;
                    tempFreeHeader->next->prev = tempFreeHeader->prev;
                    tempFreeHeader->next = NULL;
                    tempFreeHeader->prev = NULL;

                    ics_free_header *oldHead = freelist_head;
                    ics_free_header *newBlock = (ics_free_header *)tempHeader;
                    newBlock->header = *tempHeader;
                    newBlock->next = NULL;
                    newBlock->prev = NULL;

                    freelist_head = newBlock;
                    freelist_head->header = newBlock->header;
                    freelist_head->prev = NULL;
                    freelist_head->next = oldHead;
                    oldHead->prev = newBlock;
                    
                }

                else if(tempFreeHeader->prev != NULL && tempFreeHeader->next == NULL){
                    tempFreeHeader->prev->next = NULL;
                    tempFreeHeader->prev = NULL;

                    ics_free_header *oldHead = freelist_head;
                    ics_free_header *newBlock = (ics_free_header *)tempHeader;
                    newBlock->header = *tempHeader;
                    newBlock->next = NULL;
                    newBlock->prev = NULL;

                    freelist_head = newBlock;
                    freelist_head->header = newBlock->header;
                    freelist_head->prev = NULL;
                    freelist_head->next = oldHead;
                    oldHead->prev = newBlock;
                }

                else if(tempFreeHeader->prev == NULL && tempFreeHeader->next != NULL){

                    ics_free_header *newBlock = (ics_free_header *)tempHeader;
                    newBlock->header = *tempHeader;
                    newBlock->next = NULL;
                    newBlock->prev = NULL;

                    newBlock->next = tempFreeHeader->next;
                    tempFreeHeader->next->prev = newBlock;
                    tempFreeHeader->next = NULL;

                    freelist_head = newBlock;
                    freelist_head->header = newBlock->header;
                    freelist_head->next = newBlock->next;
                    freelist_head->prev = newBlock->prev;
                }

                else{
                    ics_free_header *newBlock = (ics_free_header *)tempHeader;
                    newBlock->header = *tempHeader;

                    freelist_head = newBlock;
                    freelist_head->header = newBlock->header;
                    freelist_head->next = NULL;
                    freelist_head->prev = NULL;
                }

                char *rtnPayload = ((char *)newHeader) + 8;

                return rtnPayload;
            }

            else{
                
                // tempFreeHeader->header.hid = HEADER_MAGIC;
                // tempFreeHeader->header.block_size = oldBlockSize + 1;
                // tempFreeHeader->header.padding_amount = oldBlockSize - size - 16;
            
                ics_header *newHeader = currPos;
                newHeader->hid = HEADER_MAGIC;
                newHeader->block_size = oldBlockSize + 1;
                newHeader->padding_amount = oldBlockSize - size - 16;

                currPos = currPos + oldBlockSize - 8;

                ics_footer *tempFooter = currPos;
                tempFooter->fid = FOOTER_MAGIC;
                tempFooter->block_size = oldBlockSize + 1;



                if(tempFreeHeader->next != NULL && tempFreeHeader->prev != NULL){
                    tempFreeHeader->prev->next = tempFreeHeader->next;
                    tempFreeHeader->next->prev = tempFreeHeader->prev;
                    tempFreeHeader->next = NULL;
                    tempFreeHeader->prev = NULL;

                }

                else if(tempFreeHeader->next == NULL && tempFreeHeader->prev != NULL){
                    tempFreeHeader->prev->next = NULL;
                    tempFreeHeader->prev = NULL;

                }

                else if(tempFreeHeader->next != NULL && tempFreeHeader->prev == NULL){

                    freelist_head = tempFreeHeader->next;
                    freelist_head->next = tempFreeHeader->next->next;
                    freelist_head->prev = NULL;
                    tempFreeHeader->next = NULL;

                }

                else{
                    freelist_head->next = NULL;
                    freelist_head->prev = NULL;
                    freelist_head = NULL;
                }


                char *rtnPayload = ((char *)newHeader) + 8;

                return rtnPayload;
            }
            
            
        }

        tempFreeHeader = tempFreeHeader->next;

    }

    if(totalPages <= 6){
        int newPages = 1;
        totalPages++;
        sizeHolder = size;
        
        while(sizeHolder > PAGE_SIZE - 16){
            sizeHolder = sizeHolder - PAGE_SIZE;
            newPages++;
            totalPages++;
        }

        brk = ics_inc_brk(newPages);

        if(brk != (void*)-1){
            if(brk < 0){
                return NULL;
            }

            void *hStart = brk;

            ics_header *head = hStart - 8;
            head->block_size = (newPages * PAGE_SIZE);
            head->padding_amount = 0;
            head->hid = HEADER_MAGIC;

            ics_footer *foot = hStart + ((newPages * PAGE_SIZE) - 16);
            foot->block_size = (newPages * PAGE_SIZE);
            foot->fid = FOOTER_MAGIC;

            ics_header *ep = hStart + ((newPages * PAGE_SIZE) - 8);
            epilogue = ep;
            ep->block_size = 0;
            ep->padding_amount = 0;
            ep->hid = EPILOGUE_MAGIC;
            hStart = (*heapStart);


            ics_free_header *tempFreeListHeader = freelist_head;
            freelist_head = (ics_free_header *)head;
            freelist_head->header = *head;
            freelist_head->next = tempFreeListHeader;
            freelist_head->prev = NULL;
        }
        
        else{
            errno = ENOMEM;
            return NULL;
        }

        if(freelist_head != NULL){
            return ics_malloc(size);
        }
    }

    errno = ENOMEM;
    return NULL;
}

/*
 * Marks a dynamically allocated block as no longer in use and coalesces with 
 * adjacent free blocks (as specified by Homework Document). 
 * Adds the block to the appropriate bucket according to the block placement policy.
 *
 * @param ptr Address of dynamically allocated memory returned by the function
 * ics_malloc.
 * 
 * @return 0 upon success, -1 if error and set errno accordingly.
 */
int ics_free(void *ptr) { 

    ics_header *allocHead = ptr - 8;

    if((void *)allocHead <= (void *)prologue){
        errno = EINVAL;
        return -1;
    }
    
    int allocBlockSize = ((ics_footer *)allocHead)->block_size;
    ics_footer *allocFooter = ptr + (allocBlockSize - 16 - 1);

    if ((void *)allocFooter >= (void*)epilogue){
        errno = EINVAL;
        return -1;
    }

    if(allocHead->block_size % 2 != 1){
        errno = EINVAL;
        return -1;
    }

    else if(allocFooter->block_size % 2 != 1){
        errno = EINVAL;
        return -1;
    }

    else if(allocHead->hid != HEADER_MAGIC){
        errno = EINVAL;
        return -1;
    }

    else if(allocFooter->fid != FOOTER_MAGIC){
        errno = EINVAL;
        return -1;
    }

    else if(allocHead->block_size != allocFooter->block_size){
        errno = EINVAL;
        return -1;
    }

    ics_header *nextBlockHead = ((void*)allocFooter) + 8;

    if ((void*)nextBlockHead <= (void*)epilogue){
        // next is epilogue or
        // allocated block
        if(nextBlockHead->block_size % 2 == 1 || nextBlockHead->hid == EPILOGUE_MAGIC){

            allocHead->block_size = allocBlockSize - 1;
            allocFooter->block_size = allocBlockSize - 1;

            ics_free_header *oldFreeHeader = freelist_head;

            if (oldFreeHeader != NULL){

                oldFreeHeader->header = freelist_head->header;
                oldFreeHeader->next = freelist_head->next;
                oldFreeHeader->prev = NULL;

            }


            freelist_head = (ics_free_header *)allocHead;
            freelist_head->header = *allocHead;
            freelist_head->next = oldFreeHeader;
            freelist_head->prev = NULL;
            
            if (oldFreeHeader != NULL){
                oldFreeHeader->prev = freelist_head;
            }  

            return 0;
        }

        // free block needs coalescing
        else{

            ics_footer *nextBlockFooter = ((void*)nextBlockHead) + (nextBlockHead->block_size - 8);

            ics_free_header *mergeFreeHeader = ((void *)allocFooter) + 8;
            ics_free_header *tempNextFreeHead = NULL;
            //ics_free_header *tempPrevFreeHead = NULL;
            
            if(mergeFreeHeader->next != NULL){
                mergeFreeHeader->next->prev = mergeFreeHeader->prev;
            }

            if(mergeFreeHeader->prev != NULL){
                mergeFreeHeader->prev->next = mergeFreeHeader->next;
                tempNextFreeHead = freelist_head;
                tempNextFreeHead->next = freelist_head->next;
                tempNextFreeHead->header = freelist_head->header;
            }

            else{
                tempNextFreeHead = mergeFreeHeader->next;
            }
            
            mergeFreeHeader->next = NULL;
            mergeFreeHeader->prev = NULL;
            mergeFreeHeader = NULL;


            int nextBlockSize = nextBlockHead->block_size;
            
            nextBlockHead = NULL;
            allocFooter = NULL;

            allocHead->block_size = (allocBlockSize - 1) + nextBlockSize;
            nextBlockFooter->block_size = (allocBlockSize - 1) + nextBlockSize;

            // ics_free_header *oldFreeHeader = tempNextFreeHead;
            // if (oldFreeHeader != NULL){

            //     oldFreeHeader->header = tempNextFreeHead->header;
            //     oldFreeHeader->next = tempNextFreeHead->next;
            //     oldFreeHeader->prev = NULL;

            // }
            

            freelist_head = (ics_free_header *)allocHead;
            freelist_head->header = *allocHead;
            freelist_head->next = tempNextFreeHead;
            freelist_head->prev = NULL;

            if (tempNextFreeHead != NULL){
                tempNextFreeHead->prev = freelist_head;
            }  

            return 0;

        }
    }

    else{

        allocHead->block_size = allocBlockSize - 1;
        allocFooter->block_size = allocBlockSize - 1;

        ics_free_header *oldFreeHeader = freelist_head;

        if (oldFreeHeader != NULL){

            oldFreeHeader->header = freelist_head->header;
            oldFreeHeader->next = freelist_head->next;
            oldFreeHeader->prev = NULL;

        }


        freelist_head = (ics_free_header *)allocHead;
        freelist_head->header = *allocHead;
        freelist_head->next = oldFreeHeader;
        freelist_head->prev = NULL;
        
        if (oldFreeHeader != NULL){
            oldFreeHeader->prev = freelist_head;
        }  

        return 0;

    }

    

    return -1;
}