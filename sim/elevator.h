#ifndef _sim_elevator_h_
#define _sim_elevator_h_

#include "sim/types.h"

namespace sim {

  /**
   * An elevator stores local state on the elevator's current location and prior
   * location, as well as the elevator's claimed requests. The elevator updates
   * its state in 'ticks', fulfilling the next request in the queue.
   */
  class Elevator {
   public:
    Elevator(floor_t starting_floor = 0);
    virtual ~Elevator() { }

    /**
     * Returns the current location of this elevator.
     */
    floor_t floor() const;

    /**
     * Returns the action produced by this elevator in the last tick. This may
     * be used to see which direction this elevator is moving.
     */
    Direction direction() const;

    /**
     * Returns whether this Elevator would accept the provided request, ie
     * whether the requested start point and direction is within the Elevator's
     * current path. The floor isn't actually added to the queue until
     * insert_floor() is called. The requested floor may match the Elevator's
     * current floor.
     */
    bool approve_request(floor_t floor, Direction req_direction);

    /**
     * Inserts the provided request into this elevator's queue, or returns false
     * if it's rejected. See also approve_floor().
     */
    bool insert_request(floor_t floor, Direction req_direction);

    /**
     * Runs one tick in the simulation for this elevator. Each tick equates to
     * an Action being performed, whether that's opening the doors, moving
     * up/down a floor, or idling.
     *
     * The action is done according to the next-nearest entry at the front of
     * the in the request queues, which may be updated via the
     * 'mutable_*_requests()' methods.
     */
    Action tick();

    /**
     * Returns the number of requests currently being serviced by this
     * elevator.
     */
    size_t request_count() const;

   private:
    /**
     * The current position of this elevator.
     */
    floor_t floor_;

    /**
     * The list of floors which have requests to enter this elevator.
     * (Button pressed to either enter or exit)
     */
    floor_set_t floor_requests_;

    /**
     * The direction of requests that are currently being served.
     */
    Direction accept_direction;
  };
}

#endif /* _sim_elevator_h_ */
