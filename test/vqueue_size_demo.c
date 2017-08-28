#include <stdio.h>
#include <stdlib.h>
#include "vqueue.h"


int main(int argc, char *argv[])
{
  const int k_max_queue_size = 10;

  void * queue_buffer;
  vqueue_t queue;
  int i;
  void * retrieved;
  int data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  queue_buffer = malloc(vqueue_get_bytes_required(k_max_queue_size));
  queue = vqueue_create(queue_buffer, k_max_queue_size);

  for (i = 0; i < k_max_queue_size; ++i) {
    vqueue_push(queue, ((void *) (data + i)));
  }

  for (i = 0; i < k_max_queue_size; ++i) {
    if (! vqueue_pop(queue, &retrieved)) {
      printf("No data in queue!\n");
    }
    if (*((int *) retrieved) != i) {
      printf("Unexpected data in queue!\n");
    } else {
      printf("%d retrieved\n", *((int *) retrieved));
    }
  }

  free(queue_buffer);


  return 0;
}
