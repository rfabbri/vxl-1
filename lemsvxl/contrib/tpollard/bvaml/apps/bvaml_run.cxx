#include <vcl_string.h>
#include "../bvaml_params.h"
#include "../bvaml_log_writer.h"
#include "../bvaml_world.h"


//-------------------------------------------
int main( int argc, char* argv[] )
{
  if( argc < 2 ) {
    vcl_cout<<"Usage : bgmv_run param_file1 param_file2 ...\n";
    return -1;
  }

  bvaml_log_writer masterwriter( true, "NONE" );

  bvaml_params params;
  for( int i = 1; i < argc; i++ )
    params.read_params( vcl_string(argv[i]) ); 
  params.print_summary();

  bvaml_world world( &params );
  world.process_job();

  return 0;
}

