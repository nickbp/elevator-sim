#include <gtest/gtest.h>
#include "sim/elevator.h"

TEST(Elevator, idle) {
  sim::Elevator e;
  EXPECT_EQ(0, e.request_count());
  EXPECT_EQ(sim::Direction::EITHER, e.direction());
  EXPECT_EQ(0, e.floor());
  EXPECT_EQ(sim::Action::IDLE, e.tick());
}

TEST(Elevator, single_request) {
  sim::Elevator e;
  EXPECT_TRUE(e.insert_request(1, sim::Direction::UP));

  // Start at floor 0 with upward trajectory
  EXPECT_EQ(1, e.request_count());
  EXPECT_EQ(0, e.floor());
  EXPECT_EQ(sim::Direction::UP, e.direction());

  // Move up to floor 1
  EXPECT_EQ(sim::Action::FLOOR_UP, e.tick());
  EXPECT_EQ(1, e.request_count());
  EXPECT_EQ(1, e.floor());
  EXPECT_EQ(sim::Direction::UP, e.direction());

  // Arrived at floor, open door and clear queue.
  EXPECT_EQ(sim::Action::DOOR_OPEN, e.tick());
  EXPECT_EQ(0, e.request_count());
  EXPECT_EQ(1, e.floor());
  EXPECT_EQ(sim::Direction::EITHER, e.direction());

  // Queue empty, nothing to do
  EXPECT_EQ(sim::Action::IDLE, e.tick());
}

TEST(Elevator, idle_same_floor_request) {
  sim::Elevator e(1);
  EXPECT_TRUE(e.insert_request(1, sim::Direction::UP));

  // Start at floor 1 with matching trajectory
  EXPECT_EQ(1, e.request_count());
  EXPECT_EQ(1, e.floor());
  EXPECT_EQ(sim::Direction::UP, e.direction());

  // Open door and clear queue.
  EXPECT_EQ(sim::Action::DOOR_OPEN, e.tick());
  EXPECT_EQ(0, e.request_count());
  EXPECT_EQ(1, e.floor());
  EXPECT_EQ(sim::Direction::EITHER, e.direction());

  // Queue empty, nothing to do
  EXPECT_EQ(sim::Action::IDLE, e.tick());
}

TEST(Elevator, upward_same_floor_request) {
  sim::Elevator e(2);
  // Start with upward trajectory
  EXPECT_TRUE(e.insert_request(3, sim::Direction::UP));
  EXPECT_EQ(sim::Direction::UP, e.direction());
  // Reject downward floor
  EXPECT_FALSE(e.insert_request(1, sim::Direction::DOWN));
  EXPECT_EQ(sim::Direction::UP, e.direction());
  // Accept current floor in same direction
  EXPECT_TRUE(e.insert_request(2, sim::Direction::UP));
  EXPECT_FALSE(e.insert_request(2, sim::Direction::DOWN));
  // Reject too-low floor
  EXPECT_FALSE(e.insert_request(1, sim::Direction::UP));

  // Consume at current floor
  EXPECT_EQ(2, e.request_count());
  EXPECT_EQ(2, e.floor());
  EXPECT_EQ(sim::Direction::UP, e.direction());

  EXPECT_EQ(sim::Action::DOOR_OPEN, e.tick());

  // Consume at next floor up
  EXPECT_EQ(2, e.floor());
  EXPECT_EQ(sim::Direction::UP, e.direction());
  EXPECT_EQ(sim::Action::FLOOR_UP, e.tick());
  EXPECT_EQ(1, e.request_count());
  EXPECT_EQ(3, e.floor());
  EXPECT_EQ(sim::Direction::UP, e.direction());
  EXPECT_EQ(sim::Action::DOOR_OPEN, e.tick());
  EXPECT_EQ(0, e.request_count());
  EXPECT_EQ(3, e.floor());
  EXPECT_EQ(sim::Direction::EITHER, e.direction());
  EXPECT_EQ(sim::Action::IDLE, e.tick());
}

TEST(Elevator, downward_same_floor_request) {
  sim::Elevator e(2);
  // Start with downward trajectory
  EXPECT_TRUE(e.insert_request(1, sim::Direction::DOWN));
  // Reject upward floor
  EXPECT_FALSE(e.insert_request(3, sim::Direction::UP));
  EXPECT_EQ(sim::Direction::DOWN, e.direction());
  // Accept current floor in same direction
  EXPECT_TRUE(e.insert_request(2, sim::Direction::DOWN));
  EXPECT_FALSE(e.insert_request(2, sim::Direction::UP));

  // Consume at current floor
  EXPECT_EQ(2, e.request_count());
  EXPECT_EQ(2, e.floor());
  EXPECT_EQ(sim::Direction::DOWN, e.direction());

  EXPECT_EQ(sim::Action::DOOR_OPEN, e.tick());

  // Consume at next floor down
  EXPECT_EQ(2, e.floor());
  EXPECT_EQ(sim::Direction::DOWN, e.direction());
  EXPECT_EQ(sim::Action::FLOOR_DOWN, e.tick());
  EXPECT_EQ(1, e.request_count());
  EXPECT_EQ(1, e.floor());
  EXPECT_EQ(sim::Direction::DOWN, e.direction());
  EXPECT_EQ(sim::Action::DOOR_OPEN, e.tick());
  EXPECT_EQ(0, e.request_count());
  EXPECT_EQ(1, e.floor());
  EXPECT_EQ(sim::Direction::EITHER, e.direction());
  EXPECT_EQ(sim::Action::IDLE, e.tick());
}

TEST(Elevator, alternating_directions) {
  sim::Elevator e;

  // Request in upward direction
  EXPECT_EQ(sim::Direction::EITHER, e.direction());
  EXPECT_TRUE(e.insert_request(0, sim::Direction::UP));
  EXPECT_EQ(sim::Direction::UP, e.direction());
  EXPECT_TRUE(e.insert_request(4, sim::Direction::UP));
  EXPECT_EQ(sim::Direction::UP, e.direction());
  EXPECT_TRUE(e.insert_request(2, sim::Direction::UP));

  EXPECT_EQ(sim::Direction::UP, e.direction());
  EXPECT_EQ(sim::Action::DOOR_OPEN, e.tick()); // consume 0
  EXPECT_EQ(sim::Direction::UP, e.direction());
  EXPECT_EQ(sim::Action::FLOOR_UP, e.tick()); // at 1
  EXPECT_EQ(sim::Direction::UP, e.direction());
  EXPECT_EQ(sim::Action::FLOOR_UP, e.tick()); // at 2
  EXPECT_EQ(sim::Direction::UP, e.direction());
  EXPECT_EQ(sim::Action::DOOR_OPEN, e.tick()); // consume 2
  EXPECT_EQ(sim::Direction::UP, e.direction());
  EXPECT_EQ(sim::Action::FLOOR_UP, e.tick()); // at 3
  EXPECT_EQ(sim::Direction::UP, e.direction());
  EXPECT_EQ(sim::Action::FLOOR_UP, e.tick()); // at 4
  EXPECT_EQ(sim::Direction::UP, e.direction());
  EXPECT_EQ(sim::Action::DOOR_OPEN, e.tick()); // consume 4
  EXPECT_EQ(sim::Direction::EITHER, e.direction());

  // Request in downward direction
  EXPECT_TRUE(e.insert_request(2, sim::Direction::DOWN));
  EXPECT_EQ(sim::Direction::DOWN, e.direction());
  EXPECT_FALSE(e.insert_request(6, sim::Direction::DOWN));
  EXPECT_EQ(sim::Direction::DOWN, e.direction());
  EXPECT_TRUE(e.insert_request(4, sim::Direction::DOWN));

  EXPECT_EQ(sim::Direction::DOWN, e.direction());
  EXPECT_EQ(sim::Action::DOOR_OPEN, e.tick()); // consume 4
  EXPECT_EQ(sim::Direction::DOWN, e.direction());
  EXPECT_EQ(sim::Action::FLOOR_DOWN, e.tick()); // at 3
  EXPECT_EQ(sim::Direction::DOWN, e.direction());
  EXPECT_EQ(sim::Action::FLOOR_DOWN, e.tick()); // at 2
  EXPECT_EQ(sim::Direction::DOWN, e.direction());
  EXPECT_EQ(sim::Action::DOOR_OPEN, e.tick()); // consume 2
  EXPECT_EQ(sim::Direction::EITHER, e.direction());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
