#include <gtest/gtest.h>
#include "sim/scheduler.h"
#include "sim/logging.h"

namespace {
  /**
   * A scheduler which provides access to its internal state.
   */
  class TestScheduler : public sim::Scheduler {
   public:
    TestScheduler(sim::floor_t floors, size_t elevators)
      : sim::Scheduler(floors, elevators) { }

    std::vector<sim::Elevator> &peek_elevators() {
      return elevators;
    }

    std::vector<sim::RequestGroup> &peek_up_requests() {
      return pending_up_requests;
    }

    std::vector<sim::RequestGroup> &peek_down_requests() {
      return pending_down_requests;
    }
  };
}

TEST(Scheduler, request_source_too_big) {
  sim::Scheduler s(1, 1);
  EXPECT_FALSE(s.insert_request(1, 0));
}

TEST(Scheduler, request_dest_too_big) {
  sim::Scheduler s(1, 1);
  EXPECT_FALSE(s.insert_request(0, 1));
}

TEST(Scheduler, request_source_match_dest) {
  sim::Scheduler s(1, 1);
  EXPECT_FALSE(s.insert_request(0, 0));
}

TEST(Scheduler, single_elevator_request_chain) {
  sim::verbose_enabled = true;
  TestScheduler s(5, 1);

  EXPECT_EQ(1, s.peek_elevators().size());
  sim::Elevator &e = s.peek_elevators()[0];
  EXPECT_EQ(0, e.floor());

  EXPECT_TRUE(s.idle());
  s.tick();
  EXPECT_TRUE(s.idle());

  EXPECT_TRUE(s.insert_request(0, 1));
  // Scheduler has the request:
  EXPECT_FALSE(s.idle());
  // Elevator hasn't gotten any request yet:
  EXPECT_EQ(0, e.request_count());

  EXPECT_TRUE(s.insert_request(1, 2));
  EXPECT_TRUE(s.insert_request(2, 3));
  EXPECT_TRUE(s.insert_request(3, 4));

  EXPECT_FALSE(s.insert_request(4, 5));

  EXPECT_EQ(0, e.request_count());
  s.tick();// Elevator is handed all the requests, and consumes floor 0
  EXPECT_EQ(0, e.floor());
  EXPECT_EQ(sim::Direction::UP, e.direction());
  EXPECT_EQ(3, e.request_count());

  s.tick();// floor 1
  s.tick();// consume 1
  s.tick();// floor 2
  s.tick();// consume 2
  s.tick();// floor 3
  s.tick();// consume 3
  s.tick();// floor 4
  EXPECT_FALSE(s.idle());
  EXPECT_EQ(sim::Direction::UP, e.direction());
  s.tick();// consume 4
  EXPECT_TRUE(s.idle());
  EXPECT_EQ(sim::Direction::EITHER, e.direction());
}

TEST(Scheduler, dual_elevator_request_chain) {
  sim::verbose_enabled = true;
  TestScheduler s(5, 2);

  EXPECT_EQ(2, s.peek_elevators().size());
  sim::Elevator &e0 = s.peek_elevators()[0];
  sim::Elevator &e1 = s.peek_elevators()[0];
  EXPECT_EQ(0, e0.floor());
  EXPECT_EQ(0, e1.floor());

  EXPECT_TRUE(s.idle());
  s.tick(); // 1: idle
  EXPECT_TRUE(s.idle());

  EXPECT_TRUE(s.insert_request(0, 1));
  // Scheduler has the request:
  EXPECT_FALSE(s.idle());
  // Elevators haven't gotten requests yet:
  EXPECT_EQ(0, e0.request_count());
  EXPECT_EQ(0, e1.request_count());

  EXPECT_TRUE(s.insert_request(1, 2));
  EXPECT_TRUE(s.insert_request(2, 3));
  EXPECT_TRUE(s.insert_request(3, 4));

  EXPECT_FALSE(s.insert_request(4, 5));

  s.tick();// 2: e0 consumes floor 0, e1 moves to floor 1

  // e0 has requests for floor 1 (from consuming 0->1) and floor 2
  EXPECT_EQ(2, e0.request_count());
  EXPECT_EQ(0, e0.floor());
  EXPECT_EQ(sim::Direction::UP, e0.direction());
  // e1 has requests for floor 1 and floor 3
  EXPECT_EQ(2, e1.request_count());
  EXPECT_EQ(0, e1.floor());
  EXPECT_EQ(sim::Direction::UP, e1.direction());

  s.tick();// 3: e0 at floor 1, e1 consumes floor 1 (->floor 2 added to queue)
  s.tick();// 4: e0 consumes floor 1, e1 at floor 2
  EXPECT_EQ(1, e0.request_count());
  EXPECT_EQ(1, e1.request_count());
  EXPECT_TRUE(s.insert_request(1, 0));// Added to scheduler, noone takes it yet
  s.tick();// 5: e0 at floor 2, e1 consumes floor 2 (->floor 3 added to queue)
  EXPECT_EQ(1, e0.request_count());
  EXPECT_EQ(1, e1.request_count());
  s.tick();// 6: e0 consumes floor 2 and takes down request, e1 at floor 3
  s.tick();// 7: e0 at floor 1, e1 consumes at floor 3
  s.tick();// 8: e0 consumes floor 1 (floor 0 added), e1 moves to floor 4
  s.tick();// 9: e0 at floor 0, e1 consumes floor 4
  EXPECT_FALSE(s.idle());
  s.tick();// 10: e0 consumes floor 0, e1 is idle
  EXPECT_EQ(0, e1.request_count());
  EXPECT_TRUE(s.idle());
  s.tick();// 11: e0 and e1 are idle
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
