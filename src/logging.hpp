#ifndef _logging_hpp_INCLUDED
#define _logging_hpp_INCLUDED

/*------------------------------------------------------------------------*/
#ifdef LOGGING
/*------------------------------------------------------------------------*/

/* ------added by cl------ */
#include <mutex>
#include <ctime>
#include <fstream>
#include <cstdarg>
#include <sstream>
#include <unistd.h> // For getpid()
#include <iostream>
/* ------ end ------ */

#include <vector>

namespace CaDiCaL {

// For debugging purposes and to help understanding what the solver is doing
// there is a logging facility which is compiled in by './configure -l'.  It
// still has to be enabled at run-time though (again using the '-l' option
// in the stand-alone solver).  It produces quite a bit of information.

using namespace std;

struct Clause;
struct Internal;

struct Logger {
/* ------added by cl------ */
  static mutex mtx;

  static void initialize(const char* base_file_path);
  static void log_to_file(Internal* internal, const char* fmt, ...);
  static std::string capture_log_prefix(Internal* internal);
  static std::string log_file_path;
  static std::string generate_unique_file_name(const char* base_file_path);

  // static void log_to_file (Internal *, const char *file_path, const char *fmt, ...)
  //     CADICAL_ATTRIBUTE_FORMAT (3, 4);
  // static std::string capture_log_prefix(Internal* internal);
  // static std::string generate_unique_file_name(const char* base_file_path);
/* ------ end ------ */
  static void print_log_prefix (Internal *);

  // Simple logging of a C-style format string.
  //
  static void log (Internal *, const char *fmt, ...)
      CADICAL_ATTRIBUTE_FORMAT (2, 3);

  // Prints the format string (with its argument) and then the clause.  The
  // clause can also be a zero pointer and then is interpreted as a decision
  // (current decision level > 0) or unit clause (zero decision level) and
  // printed accordingly.
  //
  static void log (Internal *, const Clause *, const char *fmt, ...)
      CADICAL_ATTRIBUTE_FORMAT (3, 4);

  // Same as before, except that this is meant for the global 'clause' stack
  // used for new clauses (and not for reasons).
  //
  static void log (Internal *, const vector<int> &, const char *fmt, ...)
      CADICAL_ATTRIBUTE_FORMAT (3, 4);

  // Another variant, to avoid copying (without logging).
  //
  static void log (Internal *, const vector<int>::const_iterator &begin,
                   const vector<int>::const_iterator &end, const char *fmt,
                   ...) CADICAL_ATTRIBUTE_FORMAT (4, 5);

  // used for logging LRAT proof chains
  //
  static void log (Internal *, const vector<uint64_t> &, const char *fmt,
                   ...) CADICAL_ATTRIBUTE_FORMAT (3, 4);

  static void log_empty_line (Internal *);
};

} // namespace CaDiCaL

/*------------------------------------------------------------------------*/

// Make sure that 'logging' code is really not included (second case of the
// '#ifdef') if logging code is not included.
/*
#define LOG(...) \
  do { \
    if (!internal->opts.log) \
      break; \
    Logger::log (internal, __VA_ARGS__); \
  } while (0)
*/
#define LOG(...) \
  do { \
    if (1) \
      break; \
    Logger::log (internal, __VA_ARGS__); \
  } while (0) 
/* ------added by cl------ */
#define LOG_TO_FILE(base_file_path, ...) \
   do { \
        if (Logger::log_file_path.empty()) { \
            Logger::initialize(base_file_path); \
        } \
        Logger::log_to_file(internal, __VA_ARGS__); \
    } while (0)
//使用方法：LOG("/path/to/log.txt", "This is a log message");
/* ------ end ------ */

/*------------------------------------------------------------------------*/
#else // end of 'then' part of 'ifdef LOGGING'
/*------------------------------------------------------------------------*/

#define LOG(...) \
  do { \
  } while (0)
#define LOG_TO_FILE(base_file_path, ...) \
    do { \
    } while (0)
/*------------------------------------------------------------------------*/
#endif // end of 'else' part of 'ifdef LOGGING'
/*------------------------------------------------------------------------*/
#endif
