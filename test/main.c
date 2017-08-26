#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include "vqueue.h"

const int PUSHCOUNT = 512;
const int PRODUCERTHREADS = 2;
const int CONSUMERTHREADS = 1;

typedef struct {
  int data;
  long timestamp;
} QItem;

void push_data(void * queue) {
  QItem * cur_item;
  struct timeval tv;
  for (int i = 0; i < PUSHCOUNT; ++i) {
    cur_item = malloc(sizeof(QItem));
    cur_item->data = i;
    gettimeofday(&tv, NULL);
    cur_item->timestamp = tv.tv_sec * 1000000 + tv.tv_usec;
    vqueue_push(*((vqueue_t *) queue), ((void *) cur_item));
  }
}

void read_data(void * queue) {
  void * retrieved;
  QItem * cur_item;
  int consumed_count=0;
  long prev_timestamp=0;
  int prev_data=-1;
  while (consumed_count < PUSHCOUNT * PRODUCERTHREADS / CONSUMERTHREADS) {
    if(vqueue_pop(*((vqueue_t *) queue), &retrieved)){
      ++consumed_count;
      cur_item = (QItem *) retrieved;
      if (cur_item->timestamp < prev_timestamp) {
        printf(
            "Prev Time: %ld Time: %ld Prev Data: %d Data: %d\n",
            prev_timestamp, cur_item->timestamp, prev_data, cur_item->data);
      }
      prev_timestamp = cur_item->timestamp;
      prev_data = cur_item->data;
    }
  }
}

int main(int argc, char *argv[])
{
  const int k_max_queue_size = PUSHCOUNT * PRODUCERTHREADS + 1;
  void * queue_buffer;
  vqueue_t queue;
  pthread_t thread_id;
  pthread_t consumer_ids[CONSUMERTHREADS];
  int i;

  queue_buffer = malloc(vqueue_get_bytes_required(k_max_queue_size));
  queue = vqueue_create(queue_buffer, k_max_queue_size);

  for (i = 0; i < PRODUCERTHREADS; ++i) {
    pthread_create(&thread_id, NULL, push_data, (void *) &queue);
    pthread_detach(thread_id);
  }
  for (i = 0; i < CONSUMERTHREADS; ++i) {
    pthread_create(&thread_id, NULL, read_data, (void *) &queue);
    consumer_ids[i] = thread_id;
  }
  for (i = 0; i < CONSUMERTHREADS; ++i) {
    pthread_join(consumer_ids[i], NULL);
  }

  return 0;
}
