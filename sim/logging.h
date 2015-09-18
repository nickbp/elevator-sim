#ifndef _sim_logging_h_
#define _sim_logging_h_

namespace sim {
  extern bool verbose_enabled;

  void debug(const char* format, ...);
}

#endif /* _sim_logging_h_ */
