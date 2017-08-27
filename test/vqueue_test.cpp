#include <vector>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <gtest/gtest.h>

typedef std::chrono::steady_clock Clock;

extern "C" {
  #include "vqueue.h"
}

/*Struct for items pushed to the queue with high resolution timestamp*/
typedef struct {
  int data;
  Clock::time_point timestamp;
} QItem;


class vqueueTest : public ::testing::Test {
 protected:
  const int k_max_queue_size = 11;
  static const int threaded_push_count = 512;
  static const int consumer_thread_count = 2;
  static const int producer_thread_count = 4;
  void * queue_buffer;
  vqueue_t queue;
  int data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  void * retrieved;
  std::vector< std::shared_ptr<std::thread> > consumers;
  std::vector< std::shared_ptr<std::thread> > producers;

  virtual void SetUp() {
    queue_buffer = malloc(vqueue_get_bytes_required(k_max_queue_size));
    queue = vqueue_create(queue_buffer, k_max_queue_size);
  }

  virtual void TearDown() {
    free(queue_buffer);
  }

  /*Task for producer threads*/
  static void push_data(vqueue_t * queue) {
    QItem * cur_item;
    for (int i = 0; i < threaded_push_count; ++i) {
      cur_item = (QItem *) malloc(sizeof(QItem *));
      cur_item->data = i;
      cur_item->timestamp = Clock::now();
      vqueue_push(*queue, ((void *) cur_item));
    }
  }

  /*Task for consumer threads*/
  static void read_data(vqueue_t * queue) {
    void * retrieved;
    QItem * cur_item;
    int consumed_count = 0;
    auto prev_timestamp = Clock::time_point(std::chrono::nanoseconds::min());
    int prev_data = -1;
    int error_count = 0;
    while (
        consumed_count <
        threaded_push_count * producer_thread_count / consumer_thread_count
        ) {
      if (vqueue_pop(*queue, &retrieved)) {
        ++consumed_count;
        cur_item = (QItem *) retrieved;
        if (cur_item->timestamp < prev_timestamp) {
          ++error_count;
        }
        prev_timestamp = cur_item->timestamp;
        prev_data = cur_item->data;
      }
    }
    EXPECT_GT(consumed_count / 100, error_count);
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
  for (int i = 0; i < producer_thread_count; ++i) {
    producers.push_back(std::make_shared<std::thread>(push_data, &queue));
  }
  std::cout << "Producers created\n";
  for (int i = 0; i < consumer_thread_count; ++i) {
    consumers.push_back(std::make_shared<std::thread>(read_data, &queue));
  }
  /*read_data(&queue);*/
  std::cout << "Consumers created\n";
  for (auto cur_thread : producers) {
    cur_thread->detach();
  }
  for (auto cur_thread : consumers) {
    cur_thread->join();
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
