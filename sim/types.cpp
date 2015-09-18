#include "sim/types.h"

const char *sim::string(Direction direction) {
  switch (direction) {
    case EITHER: return "Either";
    case UP: return "Up";
    case DOWN: return "Down";
  }
  return "?";
}

const char *sim::string(Action action) {
  switch (action) {
    case IDLE: return "Idle";
    case FLOOR_UP: return "Floor Up";
    case FLOOR_DOWN: return "Floor Down";
    case DOOR_OPEN: return "Door Open";
  }
  return "?";
}
