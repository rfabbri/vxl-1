// This is brcv/shp/dbsk2d/tests/test_dbsk2d_exceptions.cxx

#include <dbsk2d/dbsk2d_assert.h>
#include <dbsk2d/dbsk2d_exception.h>
#include <testlib/testlib_test.h>


void test_exceptions()
{
  vcl_cout << "****************************\n"
           << "  Testing dbsk2d exceptions \n"
           << "****************************\n";

#if VCL_HAS_EXCEPTIONS
  {
    bool caught_error = false;
    try
    {
      dbsk2d_exception_error(dbsk2d_exception_abort("This is just a test"));
      //the following line should never be executed
      vcl_cout << "Execution continues after exception" << vcl_endl;
    }
    catch (const dbsk2d_exception_abort &e)
    {
      vcl_cout << e.what() << vcl_endl;
      caught_error = true;

      //break if in debug mode
      dbsk2d_assert(false); //this assertion does not seem to be effective
    }
    TEST("Caught Exception 1", caught_error, true);
  }
  {
    bool caught_error = false;
    try
    {
      throw dbsk2d_exception_abort("This is just a test");
    }
    catch (const dbsk2d_exception_abort &e)
    {
      vcl_cout << e.what() << vcl_endl;
      caught_error = true;
    }
    TEST("Caught Exception 2", caught_error, true);
  }
#else
  // No exceptions - all we can do is check that dbsk2d_exception_warning doesn't abort.
  dbsk2d_exception_warning(dbsk2d_exception_abort("This is just a test"));
#endif

}

MAIN( test_dbsk2d_exceptions )
{
  START( "dbsk2d_exceptions" );
  test_exceptions();
  SUMMARY();
}
