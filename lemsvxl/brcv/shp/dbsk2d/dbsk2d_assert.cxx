// This is dbsk2d/dbsk2d_cassert.cxx

#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>

void dbsk2d_assert_failure(char const *FILE, int LINE, char const *expr)
{
  vcl_fprintf(stderr, "%s:%d assertion failure \'%s\'\n",
              FILE, LINE, expr);
  vcl_fflush(stderr);
  vcl_abort();
}

