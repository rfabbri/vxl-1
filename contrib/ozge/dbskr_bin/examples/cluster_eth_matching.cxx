// This is /contrib/biotree/proc/examples/bioproc_filtering_example_xml.cxx

//: 
// \file    cluster_eth_matching.cxx
// \brief   
//          Each processor executes some portion of the distance matrix depending on database size and the number of processors
//          All the output files are written to a global directory by all the processors for the portions that they create
//
// \author  Ozge Can Ozcanli
// \date    April 24, 2007

#include <mpi.h>
#include <vcl_ctime.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstring.h>

#include <dbskr/bin/cluster_matching.h>

int VERSION = 2;

int main(int argc, char** argv)
{
  // Initialize defaults
  int returnStatus = 0;
  int verbose = TRACE_VERBOSE;
  
  try
    {
      // Initialize MPI interface
      #ifdef MPI_CPP_BINDING
        MPI::Init (argc, argv);
      #else
        MPI_Init (&argc, &argv);
      #endif  // MPI_CPP_BINDING

      // Parse arguments
      // TODO - from the configuration file
      vcl_string view_str, patch_dir, match_dir, sampling_ds_str;

      bool eth_matching = false; 
      bool match_with_circular_completions = false;

      for (int i = 1; i < argc; i++) {
        vcl_string arg (argv[i]);
        if (arg == vcl_string ("-v")) verbose = atoi (argv[++i]);
        else if (arg == vcl_string ("-eth")) { eth_matching = true;}
        else if (arg == vcl_string ("-view")) { view_str = vcl_string(argv[++i]);}
        else if (arg == vcl_string ("-p")) { patch_dir = vcl_string(argv[++i]);}
        else if (arg == vcl_string ("-m")) { match_dir = vcl_string(argv[++i]);}
        else if (arg == vcl_string ("-ds")) { sampling_ds_str = vcl_string(argv[++i]);}
        else if (arg == vcl_string ("-circular")) { match_with_circular_completions = true; }
        else
        {
          vcl_cout << "Usage: " << argv[0] << "[-help] [-v {0|1|2}] [-eth match eth database] [-view view from eth database] [-p patch directory] [-m output match directory]" << vcl_endl;
          vcl_cout << " [-ds shock curve sampling ds, default = 1.0] " << vcl_endl;
          vcl_cout << "[-circular, match with circular completions at the leaf scurves, no completions if this argument is not there]" << vcl_endl;
          throw -1;
        }
      }

      if (eth_matching) {
        dbskr_cluster_matching match(view_str, patch_dir, match_dir, sampling_ds_str, verbose);
        match.execute_shock(match_with_circular_completions);
      } else {
        vcl_cout << "No other database matching method known, give -eth argument to match eth database\n";
      }
      
  } catch (int i) {
        // Error detected in this method
        returnStatus = i;
  } catch (dbskr_cluster_exception& e) {
        // Error thrown by bioproc class
        if (e.get_error() != e.Incomplete)
        {
            if (verbose >= TRACE_ERROR) 
              vcl_cout << "BioProcException: " //<< e.GetMessage()
              << " err: " << e.get_error() << " mpi: " 
              << e.get_MPI_error() << " sys: " 
              << e.get_syserror() << vcl_endl;
            returnStatus = e.get_error();
        }
  }
  
  // Terminate MPI
  #ifdef MPI_CPP_BINDING
    MPI::Finalize ();
  #else
    MPI_Finalize ();
  #endif  // MPI_CPP_BINDING
    return (returnStatus);
}
