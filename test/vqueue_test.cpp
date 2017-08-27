#include <stdio>
#include <chrono>
#include <gtest/gtest.h>

typedef std::chrono::high_resolution_clock Clock;

extern "C" {
  #include "vqueue.h"
}

/*Struct for items pushed to the queue with high resolution timestamp*/
typedef struct {
  int data;
  std::chrono::time_point timestamp;
} QItem;

/*Task for producer threads*/
void push_data(vqueue_t * queue) {
  QItem * cur_item;
  for (int i = 0; i < PUSHCOUNT; ++i) {
    cur_item = malloc(sizeof(QItem));
    cur_item->data = i;
    cur_item->timestamp = Clock::now();
    vqueue_push(*queue, ((void *) cur_item));
  }
}

/*Task for consumer threads*/
void read_data(vqueue_t * queue) {
  void * retrieved;
  QItem * cur_item;
  int consumed_count = 0;
  auto prev_timestamp = std::chrono::time_point::min();
  int prev_data = -1;
  while (consumed_count < PUSHCOUNT * PRODUCERTHREADS / CONSUMERTHREADS) {
    if (vqueue_pop(*queue, &retrieved)) {
      ++consumed_count;
      cur_item = (QItem *) retrieved;
      if (cur_item->timestamp < prev_timestamp) {
        std::cout << std::chrono::duration_cat<std::chrono::nanoseconds>(
            cur_item->timestamp - prev_timestamp
        );
      }
      prev_timestamp = cur_item->timestamp;
      prev_data = cur_item->data;
    }
  }
}

class vqueueTest : public ::testing::Test {
 protected:
  const int k_max_queue_size = 11;
  const int threaded_push_count = 512;
  const int consumer_thread_count = 1;
  const int producer_thread_count = 2; // Make this int multiple of consumers
  void * queue_buffer;
  vqueue_t queue;
  int data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  void * retrieved;
  int previous;
  int next;

  virtual void SetUp() {
    queue_buffer = malloc(vqueue_get_bytes_required(k_max_queue_size));
    queue = vqueue_create(queue_buffer, k_max_queue_size);
    previous = 0;
  }

  virtual void TearDown() {
    free(queue_buffer);
  }
};

TEST_F(vqueueTest, PushPop) {
  for (int i = 0; i < 10; ++i) {
    vqueue_push(queue, ((void *) (data + i)));
  }
  for (int i = 0; i < 10; ++i) {
    ASSERT_TRUE(vqueue_pop(queue, &retrieved));
    EXPECT_EQ(i, *((int *) retrieved));
  }
}

TEST_F(vqueueTest, ThreadedPushPop) {
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
