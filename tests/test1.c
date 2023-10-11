#include "debug.h"
#include "helpers.h" 
#include "icsmm.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>

#define VALUE1_VALUE 320
#define VALUE2_VALUE 0xDEADBEEFF00D

void press_to_cont() {
    printf("Press Enter to Continue");
    while (getchar() != '\n')
      ;
    printf("\n");
}

void null_check(void* ptr, long size) {
    if (ptr == NULL) {
      error(
          "Failed to allocate %lu byte(s) for an integer using ics_malloc.\n",
          size);
      error("%s\n", "Aborting...");
      assert(false);
    } else {
      success("ics_malloc returned a non-null address: %p\n", (void *)(ptr));
    }
}

void payload_check(void* ptr) {
    if ((unsigned long)(ptr) % 16 != 0) {
      warn("Returned payload address is not divisble by a quadword. %p %% 16 "
           "= %lu\n",
           (void *)(ptr), (unsigned long)(ptr) % 16);
    }
}

int main(int argc, char *argv[]) {

  int i = 0;

  // Initialize the custom allocator
  ics_mem_init();

  // Tell the user about the fields
  info("Initialized heap\n");
  press_to_cont();

  printf("=== MYTEST1: Allocation test ===\n");
  ics_freelist_print();

  void *ptr0 = ics_malloc(70);
  void *ptr1 = ics_malloc(44);
  void *ptr2 = ics_malloc(26);
  void *ptr3 = ics_malloc(81);

  ics_freelist_print();

  ics_free(ptr0);
  ics_freelist_print();
  press_to_cont();

  ics_free(ptr1);
  ics_freelist_print();
  press_to_cont();

  ics_free(ptr2);
  ics_freelist_print();
  press_to_cont();

  ics_free(ptr3);
  ics_freelist_print();
  press_to_cont();
  

  // ics_malloc(48);
  // ics_freelist_print();
  // void *ptr1 = ics_malloc(4);
  // ics_freelist_print();
  // void *ptr2 = ics_malloc(60);
  // ics_freelist_print();
  // void *ptr3 = ics_malloc(144);
  // ics_freelist_print();
  // ics_malloc(32);
  // ics_freelist_print();
  // press_to_cont();
  // ics_free(ptr3);
  // ics_freelist_print();
  // press_to_cont();
  // ics_free(ptr2);
  // ics_freelist_print();
  // press_to_cont();
  // ics_free(ptr1);
  // ics_freelist_print();
  // press_to_cont();

  // void *ptr0 = ics_malloc(436);
  // ics_freelist_print();
  // ics_payload_print((void*)ptr0);
  // press_to_cont();

  // void *ptr1 = ics_malloc(490);
  // ics_freelist_print();
  // ics_payload_print((void*)ptr1);
  // press_to_cont();

  // void *ptr2 = ics_malloc(311);
  // ics_freelist_print();
  // ics_payload_print((void*)ptr2);
  // press_to_cont();

  // void *ptr3 = ics_malloc(232);
  // ics_freelist_print();
  // ics_payload_print((void*)ptr3);
  // press_to_cont();

  // void *ptr4 = ics_malloc(249);
  // ics_freelist_print();
  // ics_payload_print((void*)ptr4);
  // press_to_cont();

  // void *ptr5 = ics_malloc(386);
  // ics_freelist_print();
  // ics_payload_print((void*)ptr5);
  // press_to_cont();

  // void *ptr6 = ics_malloc(144);
  // ics_freelist_print();
  // ics_payload_print((void*)ptr6);
  // press_to_cont();

  // void *ptr7 = ics_malloc(184);
  // ics_freelist_print();
  // ics_payload_print((void*)ptr7);
  // press_to_cont();

  // void *ptr8 = ics_malloc(162);
  // ics_freelist_print();
  // ics_payload_print((void*)ptr8);
  // press_to_cont();

  // void *ptr9 = ics_malloc(113);
  // ics_freelist_print();
  // ics_payload_print((void*)ptr9);
  // press_to_cont();

  // void *ptr10 = ics_malloc(157);
  // ics_freelist_print();
  // ics_payload_print((void*)ptr10);
  // press_to_cont();

  // void *ptr11 = ics_malloc(358);
  // ics_freelist_print();
  // ics_payload_print((void*)ptr11);
  // press_to_cont();

  // ics_freelist_print_compact(0);
  // ics_free(ptr9);
  // ics_freelist_print();
  // press_to_cont();

  // ics_free(ptr4);
  // ics_freelist_print();
  // press_to_cont();

  // ics_free(ptr10);
  // ics_freelist_print();
  // press_to_cont();

  // ics_free(ptr6);
  // ics_freelist_print();
  // press_to_cont();

  // ics_free(ptr11);
  // ics_freelist_print();
  // press_to_cont();

  // ics_free(ptr8);
  // ics_freelist_print();
  // press_to_cont();

  ics_freelist_print_compact(0);

  // void* a = ics_malloc(4064);
  // ics_freelist_print();
  // ics_payload_print((void*)a);
  // press_to_cont();


  // ics_free(a);
  // ics_freelist_print();
  // ics_payload_print((void*)a);
  // press_to_cont();


  // void* b = ics_malloc(2032);
  // ics_freelist_print();
  // ics_payload_print((void*)b);
  // press_to_cont();


  // void* c = ics_malloc(1008);
  // ics_freelist_print();
  // ics_payload_print((void*)c);
  // press_to_cont();


  // void* d = ics_malloc(992);
  // ics_freelist_print();
  // ics_payload_print((void*)d);
  // press_to_cont();


  // ics_free(d);
  // ics_freelist_print();
  // ics_payload_print((void*)d);
  // press_to_cont();


  // ics_free(c);
  // ics_freelist_print();
  // ics_payload_print((void*)c);
  // press_to_cont();


  // ics_free(b);
  // ics_freelist_print();
  // ics_payload_print((void*)b);

  press_to_cont();

  // // Test #1: Allocate an integer
  // void *i1 = ics_malloc(48);
  // ics_freelist_print();
  // ics_payload_print((void*)i1);

  // void *ptr = ics_malloc(32);
  // ics_freelist_print();
  // ics_payload_print((void*)ptr);

  // void *i2 = ics_malloc(60);
  // ics_freelist_print();
  // ics_payload_print((void*)i2);

  // void *i3 = ics_malloc(144);
  // ics_freelist_print();
  // ics_payload_print((void*)i3);

  // void *i4 = ics_malloc(32);
  // ics_freelist_print();
  // ics_payload_print((void*)i4);

  // void *i5 = ics_malloc(3664);
  // ics_freelist_print();
  // ics_payload_print((void*)i5);

  // i = ics_free(ptr);
  // ics_freelist_print();
  // ics_payload_print((void*)ptr);

  // void *i6 = ics_malloc(16);
  // ics_freelist_print();
  // ics_payload_print((void*)i6);

  press_to_cont();

  //  printf("=== MYTEST2: Allocation test ===\n");
  // ics_freelist_print();
  // // Test #1: Allocate an integer
  // char *myVal2 = ics_malloc(1);
  // ics_freelist_print();
  // null_check(myVal2, 1);
  // payload_check(myVal2);
  // ics_payload_print((void*)myVal2);
  // press_to_cont();

  // int i = 0;

  // printf("case 1\n");
  // char *tmp1 = malloc(sizeof(char));
  // i = ics_free(tmp1);
  // ics_freelist_print();
  // printf("%d\n", i);
  // press_to_cont();

  // printf("case 2\n");
  // i = ics_free(myVal1 + 32);
  // ics_freelist_print();
  // printf("%d\n", i);
  // press_to_cont();

  // printf("case 3\n");
  // i = ics_free(myVal1 + 32);
  // ics_freelist_print();
  // printf("%d\n", i);
  // press_to_cont();

  // printf("case 4\n");
  // i = ics_free(tmp1);
  // ics_freelist_print();
  // printf("%d\n", i);
  // press_to_cont();

  // printf("case 5\n");
  // i = ics_free(tmp1);
  // ics_freelist_print();
  // printf("%d\n", i);
  // press_to_cont();

  // printf("case 6\n");
  // i = ics_free(tmp1);
  // ics_freelist_print();
  // printf("%d\n", i);
  // press_to_cont();

  // printf("freeing the block\n");
  // i = ics_free(myVal1);
  // ics_freelist_print();
  // printf("%d\n", i);
  // press_to_cont();

//   printf("=== MYTEST2: Allocation test ===\n");
//  ics_freelist_print();
//   // Test #1: Allocate an integer
//   char *myVal2 = ics_malloc(sizeof(int));
//   ics_freelist_print();
//   null_check(myVal2, sizeof(int));
//   payload_check(myVal2);
//   ics_payload_print((void*)myVal2);
//   press_to_cont();

  // printf("=== MYTEST3: Allocation test ===\n");
  // ics_freelist_print();
  // // Test #1: Allocate an integer
  // char *myVal3 = ics_malloc(5000);
  // ics_freelist_print();
  // null_check(myVal3, 5000);
  // payload_check(myVal3);
  // ics_payload_print((void*)myVal3);
  // press_to_cont();

  // printf("=== MYTEST4: Allocation test ===\n");
  // ics_freelist_print();
  // // Test #1: Allocate an integer
  // char *myVal4 = ics_malloc(5000);
  // ics_freelist_print();
  // null_check(myVal4, 5000);
  // payload_check(myVal4);
  // ics_payload_print((void*)myVal4);
  // press_to_cont();

  // printf("=== MYTEST5: Allocation test ===\n");
  // ics_freelist_print();
  // // Test #1: Allocate an integer
  // char *myVal5 = ics_malloc(5000);
  // ics_freelist_print();
  // null_check(myVal5, 5000);
  // payload_check(myVal5);
  // ics_payload_print((void*)myVal5);
  // press_to_cont();


  // // Print out title for first test
  // printf("=== Test1: Allocation test ===\n");
  // // Test #1: Allocate an integer
  // int *value1 = ics_malloc(sizeof(int));
  // ics_freelist_print();
  // null_check(value1, sizeof(int));
  // payload_check(value1);
  // ics_payload_print((void*)value1);
  // press_to_cont();

  // // Now assign a value
  // printf("=== Test2: Assignment test ===\n");
  // info("Attempting to assign value1 = %d\n", VALUE1_VALUE);
  // // Assign the value
  // *value1 = VALUE1_VALUE;
  // // Now check its value
  // CHECK_PRIM_CONTENTS(value1, VALUE1_VALUE, "%d", "value1");
  // ics_payload_print((void*)value1);
  // press_to_cont();

  // printf("=== Test3: Allocate a second variable ===\n");
  // info("Attempting to assign value2 = %ld\n", VALUE2_VALUE);
  // long *value2 = ics_malloc(sizeof(long));
  // ics_freelist_print();
  // null_check(value2, sizeof(long));
  // payload_check(value2);
  // // Assign a value
  // *value2 = VALUE2_VALUE;
  // // Check value
  // CHECK_PRIM_CONTENTS(value2, VALUE2_VALUE, "%ld", "value2");
  // ics_payload_print((void*)value2);
  // press_to_cont();

  // printf("=== Test4: does value1 still equal %d ===\n", VALUE1_VALUE);
  // CHECK_PRIM_CONTENTS(value1, VALUE1_VALUE, "%d", "value1");
  // ics_payload_print((void*)value1);
  // press_to_cont();

  // // Free a variable
  // printf("=== Test5: Free a block and snapshot ===\n");
  // info("%s\n", "Freeing value1...");
  // ics_free(value1);
  // ics_freelist_print();
  // press_to_cont();

  // // Allocate a large chunk of memory and then free it
  // printf("=== Test6: 8192 byte allocation ===\n");
  // void *memory = ics_malloc(8192);
  // ics_freelist_print();
  // press_to_cont();
  // ics_free(memory);
  // ics_freelist_print();
  // press_to_cont();

  ics_mem_fini();

  return EXIT_SUCCESS;
}
