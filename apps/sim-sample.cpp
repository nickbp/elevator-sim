#include <stdio.h>
#include <stdlib.h>

#include "sim/scheduler.h"
#include "sim/logging.h"

/**
 * Basic sample that generates some random requests and inputs them into a
 * scheduler.
 */
int main() {
  sim::verbose_enabled = true;

  sim::floor_t floor_count = 50;
  size_t elevator_count = 16;
  size_t request_count = 1000;

  size_t ticks_elapsed = 0;
  size_t total_tick_max = 10000;

  sim::Scheduler scheduler(floor_count, elevator_count);

  // Input random requests, incrementing steps as we add them.
  for (; ticks_elapsed < request_count; ++ticks_elapsed) {
    sim::floor_t source = rand() % floor_count;
    sim::floor_t dest;
    do {
      // Avoid having dest == source. The scheduler will reject these.
      dest = rand() % floor_count;
    } while (source == dest);
    scheduler.insert_request(source, dest);
    scheduler.tick();
  }
  for (; ticks_elapsed < total_tick_max; ++ticks_elapsed) {
    if (scheduler.idle()) {
      break;
    }
    scheduler.tick();
  }
  if (scheduler.idle()) {
    printf("\nSimulation completed successfully in %lu ticks: "
        "%lu elevators on %lu floors with %lu requests.\n\n",
        ticks_elapsed, elevator_count, floor_count, request_count);
  } else {
    fprintf(stderr, "\nWarning!: Scheduler still busy after %lu ticks!\n\n",
        total_tick_max);
  }
}
