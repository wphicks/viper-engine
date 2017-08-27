#include <vector>
#include <iostream>
#include <memory>
#include <thread>
#include <gtest/gtest.h>
extern "C" {
  #include "vqueue.h"
}

typedef struct {
  int data;
  int thread_id;
} QItem;

typedef struct {
  int id;
  vqueue_t * queue;
} WorkerSpec;


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
  static void push_data(WorkerSpec * spec) {
    vqueue_t * queue = spec->queue;
    int tid = spec->id;
    QItem * cur_item;
    for (int i = 0; i < threaded_push_count; ++i) {
      cur_item = (QItem *) malloc(sizeof(QItem *));
      cur_item->data = i;
      cur_item->thread_id = tid;
      vqueue_push(*queue, ((void *) cur_item));
    }
    free(spec);
  }

  /*Task for consumer threads*/
  static void read_data(WorkerSpec * spec) {
    vqueue_t * queue = spec->queue;
    void * retrieved;
    QItem * cur_item;
    int consumed_count = 0;
    std::vector<int> prev_data(producer_thread_count, -1);
    while (
        consumed_count <
        threaded_push_count * producer_thread_count / consumer_thread_count
        ) {
      if (vqueue_pop(*queue, &retrieved)) {
        ++consumed_count;
        cur_item = (QItem *) retrieved;
        // Basic check of ordered insertion and retrieval
        EXPECT_LT(prev_data[cur_item->thread_id], cur_item->data);
        prev_data[cur_item->thread_id] = cur_item->data;
      }
    }
    free(spec);
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

/* The most important aspect of this test is not the (relatively
 * straightforward) check of insertion/retrieval error but whether or not it
 * hangs.
 */
TEST_F(vqueueTest, ThreadedPushPop) {
  WorkerSpec * cur_spec;
  for (int i = 0; i < producer_thread_count; ++i) {
    cur_spec = (WorkerSpec *) malloc(sizeof(WorkerSpec));
    cur_spec->id = i;
    cur_spec->queue = &queue;
    producers.push_back(std::make_shared<std::thread>(push_data, cur_spec));
  }
  for (int i = 0; i < consumer_thread_count; ++i) {
    cur_spec = (WorkerSpec *) malloc(sizeof(WorkerSpec));
    cur_spec->id = i + producer_thread_count;
    cur_spec->queue = &queue;
    consumers.push_back(std::make_shared<std::thread>(read_data, cur_spec));
  }
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
