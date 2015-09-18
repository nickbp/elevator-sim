#ifndef _sim_types_h_
#define _sim_types_h_

#include <cstddef>
#include <set>

namespace sim {
  /**
   * A floor index in a building. The lowest level in the building is always 0,
   * regardless of any user-facing labels for floors.
   */
  typedef std::size_t floor_t;
  typedef std::set<floor_t> floor_set_t;

  /**
   * The direction that an elevator may move.
   */
  enum Direction {
    EITHER, // Pending request is at current location, or no pending requests.
    UP, // Pending requests are all upward bound
    DOWN // Pending requests are all downward bound
  };

  /**
   * An action that an elevator may take in a single tick of the simulation.
   */
  enum Action {
    IDLE, // No requests pending
    FLOOR_UP, // Approached an upward request
    FLOOR_DOWN, // Approached a downward request
    DOOR_OPEN // Opened door for request at current location
  };

  /**
   * Returns a fixed string representation of the provided Direction.
   */
  const char *string(Direction direction);

  /**
   * Returns a fixed string representation of the provided Action.
   */
  const char *string(Action action);
}

#endif /* _sim_types_h_ */
