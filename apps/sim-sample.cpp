#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "sim/scheduler.h"
#include "sim/logging.h"

namespace {
  void syntax(char* appname) {
    printf("%s [-h] [-f floors] [-e elevators] [-r requests] [-t maxticks]\n", appname);
  }

  void parse_config(int argc, char *argv[],
      sim::floor_t &floor_count,
      size_t &elevator_count,
      size_t &request_count,
      size_t &total_tick_max) {
    int opt = 0;
    while ((opt = getopt(argc, argv, "hf:e:r:t:")) != -1) {
      switch (opt) {
        case 'h':
          syntax(argv[0]);
          exit(1);
          break;
        case 'f':
          floor_count = atoi(optarg);
          break;
        case 'e':
          elevator_count = atoi(optarg);
          break;
        case 'r':
          request_count = atoi(optarg);
          break;
        case 't':
          total_tick_max = atoi(optarg);
          break;
      }
    }
    printf("\n");
    syntax(argv[0]);
    printf("Args: floors(-f)=%lu elevators(-e)=%lu requests(-r)=%lu maxticks(-t)=%lu\n\n",
        floor_count, elevator_count, request_count, total_tick_max);
  }
}

/**
 * Basic sample that generates some random requests and inputs them into a
 * scheduler.
 */
int main(int argc, char *argv[]) {
  sim::floor_t floor_count = 50;
  size_t elevator_count = 16;
  size_t request_count = 1000;
  size_t total_tick_max = 10000;
  parse_config(argc, argv, floor_count, elevator_count, request_count, total_tick_max);

  sim::verbose_enabled = true;
  sim::Scheduler scheduler(floor_count, elevator_count);

  size_t ticks_elapsed = 0;
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
