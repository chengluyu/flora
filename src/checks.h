#ifndef FLORA_CHECKS_H
#define FLORA_CHECKS_H

#include <cstdlib>
#include <cstdio>

#include "flora.h"

NORETURN static void flora_fatal(const char *file, int line, const char *message) {
  std::printf("FATAL ERROR at line %d in file '%s': %s", line, file, message);
  std::abort();
}

#define UNREACHABLE() flora_fatal(__FILE__, __LINE__, "unreachable code")
#define UNIMPLEMENTED() flora_fatal(__FILE__, __LINE__, "unimplemented code")

#endif