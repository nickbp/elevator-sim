#ifndef _sim_scheduler_h_
#define _sim_scheduler_h_

#include <vector>

#include "sim/elevator.h"

namespace sim {
  class RequestGroup;

  /**
   * The scheduler handles incoming requests and hands them out to Elevators.
   * The caller is responsible for inputting requests via insert_request() and
   * moving the simulation along with tick(). To end a simulation cleanly with
   * all requests fulfilled, call tick() until idle() returns true.
   */
  class Scheduler {
   public:
    /**
     * Creates a new scheduler which operates on the provided quantity of
     * floors and elevators. Verbose logging may be enabled to print internal
     * state on every tick.
     */
    Scheduler(floor_t floors, size_t elevators);
    virtual ~Scheduler();

    /**
     * Inserts a new elevator request. Returns true if the request was inserted,
     * or false if it was ignored. Requests may be ignored if they are invalid
     * or if an identical request has already been queued.
     */
    bool insert_request(floor_t source, floor_t dest);

    /**
     * Runs the simulation for a step, updating Elevator and request state in
     * the process. This must be called repeatedly to move the simulation along.
     */
    void tick();

    /**
     * Returns whether the scheduler is idle, which is when no requests remain
     * to be completed. This may be called to determine if the simulation has
     * serviced all inserted requests.
     */
    bool idle() const;

   protected:
    /**
     * The elevators which are being simulated. Visible for testing.
     */
    std::vector<Elevator> elevators;

    /**
     * Requests which are not yet assigned to an elevator, grouped by direction.
     * 'up' requests have destination greater than source, while 'down' requests
     * have destination less than source. Visible for testing.
     */
    std::vector<RequestGroup> pending_up_requests, pending_down_requests;

   private:
    void add_any_pickup_requests(std::vector<Elevator> &elevators,
        std::vector<RequestGroup> &request_groups, Direction direction);
    void add_dropoff_requests(Elevator &elevator, RequestGroup &request_group,
        Direction direction);
    void verbose(const char *format, ...) const;

    size_t tick_;
  };
}

#endif /* _sim_scheduler_h_ */
