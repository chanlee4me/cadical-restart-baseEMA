#ifdef LOGGING

#include "internal.hpp"

namespace CaDiCaL {
/* ------added by cl------ */
std::mutex Logger::mtx;
std::string Logger::log_file_path;
std::string Logger::capture_log_prefix(Internal* internal) {
    std::stringstream ss;
    std::streambuf* old_buf = std::cout.rdbuf(ss.rdbuf()); // Redirect std::cout to stringstream
    internal->print_prefix();
    std::cout.rdbuf(old_buf); // Restore original buffer
    ss << "decision level " << internal->level << " ";
    return ss.str();
}
std::string Logger::generate_unique_file_name(const char* base_file_path) {
    std::stringstream ss;
    std::time_t t = std::time(nullptr);
    pid_t pid = getpid();
    ss << base_file_path << "_" << t << "_" << pid << ".txt";
    return ss.str();
}
void Logger::initialize(const char* base_file_path) {
    std::lock_guard<std::mutex> lock(mtx);
    log_file_path = generate_unique_file_name(base_file_path);
}
void Logger::log_to_file(Internal* internal, const char* fmt, ...) {
    std::lock_guard<std::mutex> lock(mtx);

    if (log_file_path.empty()) {
        std::cerr << "Error: Logger not initialized. Call initialize() first." << std::endl;
        return;
    }

    std::ofstream log_file;
    log_file.open(log_file_path, std::ios_base::app); // append instead of overwrite
    if (!log_file) {
        std::cerr << "Error: Unable to open log file: " << log_file_path << std::endl;
        return;
    }

    // Capture the log prefix
    std::string log_prefix = capture_log_prefix(internal);
    
    // Format the log message
    va_list ap;
    va_start(ap, fmt);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);
    
    // Write the log message to the file
    log_file << log_prefix << buffer << '\n';
    log_file.close();
}
/* ------ end ------ */

void Logger::print_log_prefix (Internal *internal) {
  internal->print_prefix ();
  tout.magenta ();
  fputs ("LOG ", stdout);
  tout.magenta (true);
  printf ("%d ", internal->level);
  tout.normal ();
}

void Logger::log_empty_line (Internal *internal) {
  internal->print_prefix ();
  tout.magenta ();
  const int len = internal->prefix.size (), max = 78 - len;
  for (int i = 0; i < max; i++)
    fputc ('-', stdout);
  fputc ('\n', stdout);
  tout.normal ();
  fflush (stdout);
}

void Logger::log (Internal *internal, const char *fmt, ...) {
  print_log_prefix (internal);
  tout.magenta ();
  va_list ap;
  va_start (ap, fmt);
  vprintf (fmt, ap);
  va_end (ap);
  fputc ('\n', stdout);
  tout.normal ();
  fflush (stdout);
}

// It is hard to factor out the common part between the two clause loggers,
// since they are also used in slightly different contexts.  Our attempt to
// do so were not more readable than the current version.  See the header
// for an explanation of the difference between the following two functions.

void Logger::log (Internal *internal, const Clause *c, const char *fmt,
                  ...) {
  print_log_prefix (internal);
  tout.magenta ();
  va_list ap;
  va_start (ap, fmt);
  vprintf (fmt, ap);
  va_end (ap);
  if (c) {
    if (c->redundant)
      printf (" glue %d redundant", c->glue);
    else
      printf (" irredundant");
    printf (" size %d clause[%" PRId64 "]", c->size, c->id);
    if (c->moved)
      printf (" ... (moved)");
    else {
      if (internal->opts.logsort) {
        vector<int> s;
        for (const auto &lit : *c)
          s.push_back (lit);
        sort (s.begin (), s.end (), clause_lit_less_than ());
        for (const auto &lit : s)
          printf (" %d", lit);
      } else {
        for (const auto &lit : *c)
          printf (" %d", lit);
      }
    }
  } else if (internal->level)
    printf (" decision");
  else
    printf (" unit");
  fputc ('\n', stdout);
  tout.normal ();
  fflush (stdout);
}

// Same as above, but for the global clause 'c' (which is not a reason).

void Logger::log (Internal *internal, const vector<int> &c, const char *fmt,
                  ...) {
  print_log_prefix (internal);
  tout.magenta ();
  va_list ap;
  va_start (ap, fmt);
  vprintf (fmt, ap);
  va_end (ap);
  if (internal->opts.logsort) {
    vector<int> s;
    for (const auto &lit : c)
      s.push_back (lit);
    sort (s.begin (), s.end (), clause_lit_less_than ());
    for (const auto &lit : s)
      printf (" %d", lit);
  } else {
    for (const auto &lit : c)
      printf (" %d", lit);
  }
  fputc ('\n', stdout);
  tout.normal ();
  fflush (stdout);
}

// Now for 'restore_clause' to avoid copying (without logging).

void Logger::log (Internal *internal,
                  const vector<int>::const_iterator &begin,
                  const vector<int>::const_iterator &end, const char *fmt,
                  ...) {
  print_log_prefix (internal);
  tout.magenta ();
  va_list ap;
  va_start (ap, fmt);
  vprintf (fmt, ap);
  va_end (ap);
  if (internal->opts.logsort) {
    vector<int> s;
    for (auto p = begin; p != end; p++)
      s.push_back (*p);
    sort (s.begin (), s.end (), clause_lit_less_than ());
    for (const auto &lit : s)
      printf (" %d", lit);
  } else {
    for (auto p = begin; p != end; p++)
      printf (" %d", *p);
  }
  fputc ('\n', stdout);
  tout.normal ();
  fflush (stdout);
}

// for LRAT proof chains

void Logger::log (Internal *internal, const vector<uint64_t> &c,
                  const char *fmt, ...) {
  print_log_prefix (internal);
  tout.magenta ();
  va_list ap;
  va_start (ap, fmt);
  vprintf (fmt, ap);
  va_end (ap);
  for (const auto &id : c)
    printf (" %" PRIu64, id);
  fputc ('\n', stdout);
  tout.normal ();
  fflush (stdout);
}

} // namespace CaDiCaL

#endif
