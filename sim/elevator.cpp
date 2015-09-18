#include "sim/elevator.h"
#include "sim/logging.h"

sim::Elevator::Elevator(floor_t starting_floor/*=0*/)
  : floor_(starting_floor) { }

sim::floor_t sim::Elevator::floor() const {
  return floor_;
}

sim::Direction sim::Elevator::direction() const {
  // If queue is empty, elevator is idle.
  if (floor_requests_.empty()) {
    return Direction::EITHER;
  }
  return accept_direction;
}

// 22/down approved, we were going either from 18
// we'd want to go up to 22, to service a down request
bool sim::Elevator::approve_request(floor_t req_floor, Direction req_direction) {
  Direction cur_direction = direction();
  switch (cur_direction) {
    case Direction::UP:
      if (req_floor < floor_ || req_direction != Direction::UP) {
        // Elevator is going up, but the request is below the elevator's current
        // location. Denied.
        debug("    Floor %lu/%s denied: we're going UP from floor %lu.",
            req_floor, string(req_direction), floor_);
        return false;
      }
      break;
    case Direction::DOWN:
      if (req_floor > floor_ || req_direction != Direction::DOWN) {
        // Elevator is going down, but the request is above the elevator's
        // current location. Denied.
        debug("    Floor %lu/%s denied: we're going DOWN from floor %lu.",
            req_floor, string(req_direction), floor_);
        return false;
      }
      break;
    case Direction::EITHER:
      // No problem!
      break;
  }

  debug("    Floor %lu/%s approved: we were going %s from %lu.",
      req_floor, string(req_direction), string(cur_direction), floor_);
  return true;
}

bool sim::Elevator::insert_request(floor_t floor, Direction req_direction) {
  if (!approve_request(floor, req_direction)) {
    return false;
  }
  debug("    Floor %lu inserted.", floor);
  floor_requests_.insert(floor);
  accept_direction = req_direction;
  return true;
}

sim::Action sim::Elevator::tick() {
  if (floor_requests_.empty()) {
    // Nothing in request queue, do nothing.
    debug("    Elevator queue empty at floor %lu.", floor_);
    return Action::IDLE;
  }

  // Get the next floor to be served from the queue.
  floor_t nearest_request;
  switch (direction()) {
    case Direction::UP:
      // Moving to lowest floor in queue
      nearest_request = *floor_requests_.begin();
      break;
    case Direction::DOWN:
      // Moving to highest floor in queue
      nearest_request = *floor_requests_.rbegin();
      break;
    case Direction::EITHER:
      // Should only be one request
      nearest_request = *floor_requests_.begin();
      break;
  }

  // Perform work depending on where the next floor is located, relative to the
  // elevator's current position.
  if (floor_ < nearest_request) {
    // Move up a floor
    ++floor_;
    debug("    Moved up to floor %lu towards floor %lu (%lu in queue).",
        floor_, nearest_request, floor_requests_.size());
    return Action::FLOOR_UP;
  } else if (floor_ > nearest_request) {
    // Move down a floor
    --floor_;
    debug("    Moved down to floor %lu towards floor %lu (%lu in queue).",
        floor_, nearest_request, floor_requests_.size());
    return Action::FLOOR_DOWN;
  } else {
    // Currently at a requested floor. Open doors and complete the request by
    // removing it from the set.
    floor_requests_.erase(floor_);
    debug("    Arrived at floor %lu (%lu still in queue).",
        floor_, floor_requests_.size());
    return Action::DOOR_OPEN;
  }
}

size_t sim::Elevator::request_count() const {
  return floor_requests_.size();
}
