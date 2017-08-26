#include <gtest/gtest.h>
extern "C" {
  #include "vqueue.h"
}

class vqueueTest : public ::testing::Test {
 protected:
  const int k_max_queue_size = 11;
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
    EXPECT_TRUE(vqueue_pop(queue, &retrieved));
    EXPECT_EQ(i, *((int *) retrieved));
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
