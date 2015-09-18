#include "sim/scheduler.h"
#include "sim/elevator.h"
#include "sim/logging.h"

#include <cassert>

namespace sim {
  /**
   * Utility class: a group of requests which all go in the same direction and
   * which all start at a given source/pickup floor.
   */
  class RequestGroup {
   public:
    RequestGroup() : accepted(false) { }

    // Set of destination/dropoff floors which will be passed to the elevator
    // when it arrives at the source floor.
    floor_set_t dests;

    // Whether the source/pickup has been accepted by an elevator.
    bool accepted;
  };
}

sim::Scheduler::Scheduler(floor_t floors, size_t elevators)
  : elevators(elevators, Elevator()),
    pending_up_requests(floors, RequestGroup()),
    pending_down_requests(floors, RequestGroup()),
    tick_(1) {
  assert(floors > 0);
  assert(elevators > 0);
}

sim::Scheduler::~Scheduler() {
}

bool sim::Scheduler::insert_request(floor_t source, floor_t dest) {
  if (source >= pending_up_requests.size()
      || dest >= pending_up_requests.size()) {
    // Invalid input: floor value exceeds building
    // (request lists have same size)
    return false;
  }

  // Save the request, to be passed to an elevator within tick().
  if (source > dest) {
    // Destination is below source. Down request.
    return pending_down_requests[source].dests.insert(dest).second;
  } else if (source < dest) {
    // Destination is above source. Up request.
    return pending_up_requests[source].dests.insert(dest).second;
  } else {
    /* Invalid input: source equals destination. We could also treat this as
     * valid, where the elevator just arrives and performs a single door
     * operation, but let's keep things relatively simple for now. */
    return false;
  }
}

void sim::Scheduler::tick() {
  debug("--- Start of tick %lu", tick_);

  // Phase 1: Pass requests to any Elevator which will accept them.
  debug("Upward pickups:");
  add_any_pickup_requests(elevators, pending_up_requests, Direction::UP);
  debug("Downward pickups:");
  add_any_pickup_requests(elevators, pending_down_requests, Direction::DOWN);

  for (size_t i = 0; i < elevators.size(); ++i) {
    debug("Elevator %lu:", i);
    Elevator &elevator = elevators[i];

    // Phase 2: Run elevator ticks.
    debug("  Pre-tick: floor[%lu] direction[%s]",
        elevator.floor(), string(elevator.direction()));
    Action action = elevator.tick();
    debug("  Post-tick: floor[%lu] direction[%s] action[%s]",
        elevator.floor(), string(elevator.direction()), string(action));

    if (action != Action::DOOR_OPEN) {
      // No additional work; Phase 3 not applicable.
      continue;
    }

    debug("  Add dropoff requests for direction %s",
        string(elevator.direction()));
    /* Phase 3: For any elevators that performed a DOOR_OPEN action to serve an
     * entry request, populate them with any matching exit request(s) in the
     * direction of the elevator's movement.
     *
     * This simulates users entering the elevator and selecting their
     * destination floors within the elevator. */
    floor_t cur_floor = elevator.floor();
    switch (elevator.direction()) {
      case Direction::UP:
        add_dropoff_requests(
          elevator, pending_up_requests[cur_floor], Direction::UP);
        break;
      case Direction::DOWN:
        add_dropoff_requests(
          elevator, pending_down_requests[cur_floor], Direction::DOWN);
        break;
      case Direction::EITHER:
        // Arbitrarily pick the direction with the most floor requests
        if (pending_up_requests[cur_floor].dests.size()
            >= pending_down_requests[cur_floor].dests.size()) {
          add_dropoff_requests(
            elevator, pending_up_requests[cur_floor], Direction::UP);
        } else {
          add_dropoff_requests(
            elevator, pending_down_requests[cur_floor], Direction::DOWN);
        }
        break;
    }
  }

  debug("--- End of tick %lu", tick_);
  ++tick_;
}

bool sim::Scheduler::idle() const {
  // Check local request queues
  for (const RequestGroup &group : pending_up_requests) {
    if (!group.dests.empty()) {
      return false;
    }
  }
  for (const RequestGroup &group : pending_down_requests) {
    if (!group.dests.empty()) {
      return false;
    }
  }
  // Check elevators for idle status
  for (const Elevator &elevator : elevators) {
    if (elevator.request_count() != 0) {
      return false;
    }
  }
  return true;
}

void sim::Scheduler::add_any_pickup_requests(std::vector<Elevator> &elevators,
    std::vector<RequestGroup> &request_groups, Direction direction) {
  for (floor_t pickup_floor = 0;
       pickup_floor < request_groups.size(); ++pickup_floor) {
    RequestGroup &pickup_group = request_groups[pickup_floor];
    if (pickup_group.dests.empty()) {
      // This pickup group is empty.
      continue;
    }

    if (pickup_group.accepted) {
      debug("  Pickup at %lu already accepted", pickup_floor);
      // This pickup location is already accepted by an elevator.
      continue;
    }

    /* Find the 'best' elevator to take this pickup request, among the elevators
     * who are willing to take it. For now, we arbitrarily define 'best' as 'has
     * fewest pending requests', but other criteria could be used as well. */
    int best_index = -1;
    for (size_t i = 0; i < elevators.size(); ++i) {
      Elevator &elevator = elevators[i];
      if (elevator.approve_request(pickup_floor, direction)) {
        size_t request_count = elevator.request_count();
        debug("  Pickup by elevator %lu (requests=%lu) at floor %lu approved",
            i, request_count, pickup_floor);
        // This elevator will accept the request, but is it better than our
        // other options?
        if (best_index < 0
            || request_count < elevators[best_index].request_count()) {
          best_index = i;
        }
      } else {
        debug("  Pickup by elevator %lu at floor %lu declined", i, pickup_floor);
      }
    }
    if (best_index >= 0) {
      debug("  -> Pickup inserted into elevator %lu", best_index);
      // Insert the request into the best elevator according to our criteria,
      // then mark the RequestGroup as being accepted.
      elevators[best_index].insert_request(pickup_floor, direction);
      pickup_group.accepted = true;
    }
  }
}

void sim::Scheduler::add_dropoff_requests(Elevator &elevator,
    RequestGroup &request_group, Direction direction) {
  // Pass all floors to the elevator.

  debug("  -> %lu dropoff requests", request_group.dests.size());
  for (floor_t floor : request_group.dests) {
    bool inserted = elevator.insert_request(floor, direction);
    // The elevator should really approve this request to drop off passengers.
    // It already approved the same direction for the pickup!
    assert(inserted);
  }
  // Reset the group, clearing the requests and unsetting the accept bit.
  request_group.dests.clear();
  request_group.accepted = false;
}
