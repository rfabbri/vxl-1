// This is dbsk2d/dbsk2d_cassert.cxx

#include <cstdio>
#include <cstdlib>

void dbsk2d_assert_failure(char const *FILE, int LINE, char const *expr)
{
  std::fprintf(stderr, "%s:%d assertion failure \'%s\'\n",
              FILE, LINE, expr);
  std::fflush(stderr);
  std::abort();
}

