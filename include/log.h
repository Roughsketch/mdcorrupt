/*
  This is an extremely hacked together class that provides extremely basic debug logging.

  Not recommended for anything serious. This is an abomination.

#ifndef _CORRUPTER_LOG_H
#define _CORRUPTER_LOG_H

#include <iostream>
#include <string>
#include <sstream>

namespace
{
  namespace debug
  {
    typedef std::ostream& (*iomanip) (std::ostream&);

    class Log {
    public:
      Log() : log(false), newline(true) {};
      Log(bool log) : log(log), out(&std::cout), identifier(""), newline(true) {};
      Log(bool log, std::ostream* out) : log(log), out(out), identifier(""), newline(true) {};
      Log(bool log, std::ostream* out, std::string id) : log(log), out(out), identifier(id), newline(true) {};

      inline bool enabled() { return log; }
      void disable() { log = false; }
      void enable() { log = true; }
      std::vector<std::string> rdbuf() { return output; }
      void wrbuf(std::string x) { output.push_back(x); }
      void clear() { output.clear(); }
      void setline(bool nl) { newline = nl; }
      bool isnewline() { return newline; }
      std::string id() { return identifier; }

      std::ostream& get() { return *out; }

    private:
      bool log;
      bool newline;
      std::string identifier;
      std::ostream* out;
      std::vector<std::string> output;
    };

    template <class T> Log& operator<< (Log& log, const T& x)
    {
      if (log.enabled())
      {
        std::ostringstream oss;
        oss << x;
        log.wrbuf(oss.str());
      }
      return log;
    }

    Log& operator<< (Log& log, iomanip manipulator)
    {
      if (log.enabled())
      {
        log.get() << log.id();
        for (auto& o : log.rdbuf())
        {
          log.get() << o;
        }
        log.get() << manipulator;
        log.clear();
        log.setline(true);
      }
      return log;
    }

    extern Log cout(true, &std::cout, "[DEBUG]: ");
    extern Log cerr(true, &std::cerr, "[ERROR]: ");

  }
}

#endif

*/