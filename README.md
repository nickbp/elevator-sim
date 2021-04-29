# [THIS PROJECT HAS MOVED TO SOURCEHUT!](https://sr.ht/~nickbp/elevator-sim/)

I'm consolidating my personal projects in one place. As such the old GitHub repo is being archived, and work now continues on sourcehut.

# Elevator Sim

### How Things Work

This library implements a simulated control system for one or more elevators in a multi-story building. Here's a rough breakdown of how the algorithm works in practice:

The algorithm is synchronous. The caller needs to move the simulation along by repeatedly calling a `tick()` function, where a tick represents an arbitrary unit of time defined to be the cost of an elevator opening its doors at a floor, or an elevator moving to a new floor. If a simulation is configured with multiple elevators, they will work in parallel with each tick call, all synchronized to the `tick()` calls.

The Scheduler allocates requests to Elevators. Elevators each maintain local state on the requests that they're currently serving, and operate fairly autonomously once they've accepted a request, automatically moving to requested locations in their list. The Scheduler just checks in periodically to give the Elevators new tasks to be processed.

When the Scheduler is choosing the Elevator to receive a given request, priority is given to any Elevators which are either idle or already on a route that crosses the request floor, as well as heading in the same direction as the request. This is implemented as an "approval" to be performed by the Elevators, where they decide whether the request falls within their route.

For example, if an up-bound Elevator is currently located at floor 5, then an incoming up-bound request for pickup at floor 8 would be taken by that elevator. However, if the up-bound request was at floor 4, then that elevator at floor 5 would decline the request, since the elevator does not currently have floor 4 in its path. Similarly, if the request was for pickup at floor 8 but in the downward direction, then the elevator would decline the request for now since it's going in the opposite direction.

If multiple Elevators have approved a given request (ie they all have routes that fit), the Scheduler then needs to pick one. To keep things relatively less complicated in this first pass, the Scheduler just picks the Elevator with the fewest outstanding requests. This could likely be improved upon with a different weighting metric, like selecting the Elevator which would add the least amount of total wait time.

For example, the current fewest-requests selection method would be sub-optimal when an idle elevator on the opposite end of the building is selected over an elevator that's slightly more busy but just a couple floors away from the request. The idle elevator technically has no requests pending, but it will take significantly longer to honor up the request, proportional to the building height.

If all Elevators have all declined a request due to a lack of path overlap, then the Scheduler will temporarily hold the request in its own local queue until an Elevator has become available, either by going idle or by switching to a new path that's compatible with the request. The Scheduler will attempt to allocate these pending requests at the start of every tick by re-querying Elevators to approve the request.

Requests come in two halves: a source floor and a destination floor. The source floor, along with the up/down direction of the request, are what first get passed to an Elevator. Once the Elevator has arrived at the source floor, the Scheduler passes the destination floor(s). Multiple may be passed if several requests in the same direction have been accumulated at that floor (picture someone pressing a button repeatedly). Only the Scheduler has knowledge about the two halves of a request. From the Elevator's perspective, there's no difference between the source and the destination, since in practice a given floor could be both a source for one request and a destination for another at the same time. Elevators just deal in request queues to open their doors on certain floors, regardless of whether the people on those floors are entering, exiting, or both. Additionally, having the Scheduler 'resolve' the second half of the request only after the elevator arrives at the first half in this way emulates the real-world scenario of a user pressing a directional button in a hallway (on the source floor), then entering the destination floor only after they've entered the elevator.

In comparison to other algorithms, this scheduler is superficially similar to the [LOOK Algorithm](https://en.wikipedia.org/wiki/LOOK_algorithm). The main similarity is that both algorithms are focused on finding workers that are already en-route to them, where the workers change direction once there are no requests to be fulfilled in the current heading. Beyond this behavior, however, the Elevator Sim algorithm is a bit more complicated than LOOK, mainly due to the directional nature of Elevator requests ("open the door at floor 1, then at floor 4"), where LOOK is focused on scheduling individual sector reads ("read sector 482").

### File Layout

Here's an overview of things are laid out:

- **elevator-sim/**
  - CMakeLists.txt *# (here and throughout) Used by cmake to generate Makefiles.*
  - LICENCE *# GPL3*
  - README
  - **apps/** *# Front-end executables to library code in sim/*
    - sim-sample.cpp *# Basic executable which just runs random requests with verbose settings* enabled.
  - **bin/** *# Build output goes here. created manually in "INSTALLATION/BUILD" steps.*
  - **sim/** *# Main library code. Referenced by apps/ and tests/*
    - elevator.h/.cpp *# The Elevator class, described in "HOW THINGS WORK"*
    - logging.h/.cpp *# Very basic logging utility (wouldn't recommend for 'real' code)*
    - scheduler.h/.cpp *# The Scheduler class, described in "HOW THINGS WORK"*
    - types.h/.cpp *# Types which are shared by Elevator and Scheduler code*
  - **tests/** *# Unit tests for library code in sim/*
    - test-elevator.cpp *# Tests for the Elevator class*
    - test-scheduler.cpp *# Tests for the Scheduler class*

### Install/Build

1. Install build tools: **g++** suite, **cmake**, and optionally **gtest**.

   ```sh
   $ sudo yum install gcc-c++ cmake gtest-devel #Redhat/Fedora
   ```
   OR
   ```sh
   $ sudo apt-get install build-essentials cmake libgtest-dev #Debian/Ubuntu
   ```

    If you omit gtest, unit tests will be omitted from the built config when `cmake` is run in step 3.

2. Pull this repo from github, then go into the `elevator-sim` directory; the one that `README.md` is in:

   ```sh
   $ git clone https://github.com/nickbp/elevator-sim.git
   $ cd elevator-sim
   elevator-sim$
   ```

3. From the directory that `README.md` is in, init the build (generate Makefiles in bin/):

   ```sh
   elevator-sim$ mkdir bin; cd bin; cmake ..
   ```

4. From the `bin` directory you just created, start the build:
   ```sh
   bin$ make
   ```

5. Run the basic sample executable:

   ```sh
   bin$ ./apps/sim-sample # use default settings
   bin$ ./apps/sim-sample -h # help
   bin$ ./apps/sim-sample -f 10 -e 3 -r 40 # custom settings
   ```

6. Run unit tests:

   ```sh
   bin$ make test # (run all test suites)
   ```
   OR
   ```sh
   bin$ ./tests/test-<thing> # (run single test suite with testcase breakdown)
   ```
