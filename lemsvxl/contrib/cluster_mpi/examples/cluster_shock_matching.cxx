// This is /contrib/biotree/proc/examples/bioproc_filtering_example_xml.cxx

//: 
// \file    cluster_shock_matching.cxx
// \brief   All the datafiles are read from a global directory by all the processors
//          Each processor executes some portion of the distance matrix depending on database size and the number of processors
//          All the output files are written to a global directory by all the processors for the portions that they create
//
// \author  Ozge Can Ozcanli
// \date    May 26, 2006

#include <mpi.h>
#include <vcl_ctime.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstring.h>

#include <cluster_mpi/cluster_db_matching.h>

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
      vcl_string database_list, images_dir, esfs_dir, cons_dir, out_file, output_dir, output_dir_shgm, resolution, addition;
      bool elastic_splice = false, normalize_cost = false, save_shgm = false, save_output_img = false;
      double sampling_ds = 1.0f, pruning_threshold = -1.0f;

      for (int i = 1; i < argc; i++) {
        vcl_string arg (argv[i]);
        if (arg == vcl_string ("-v")) verbose = atoi (argv[++i]);
        else if (arg == vcl_string ("-esf")) { esfs_dir = vcl_string(argv[++i]);}
        else if (arg == vcl_string ("-img")) { images_dir = vcl_string(argv[++i]);}
        else if (arg == vcl_string ("-con")) { cons_dir = vcl_string(argv[++i]);}
        else if (arg == vcl_string ("-db")) { database_list = vcl_string(argv[++i]);}
        else if (arg == vcl_string ("-outf")) { out_file = vcl_string(argv[++i]);}
        else if (arg == vcl_string ("-outd")) { output_dir = vcl_string(argv[++i]); save_output_img = true; }
        else if (arg == vcl_string ("-outshgm")) { output_dir_shgm = vcl_string(argv[++i]); save_shgm = true; }
        else if (arg == vcl_string ("-res")) { resolution = vcl_string(argv[++i]);}
        else if (arg == vcl_string ("-dbstring")) { addition = vcl_string(argv[++i]);}
        else if (arg == vcl_string ("-ds")) { sampling_ds = double(atof(argv[++i]));}
        else if (arg == vcl_string ("-elastic")) { elastic_splice = true;}
        else if (arg == vcl_string ("-normalize")) { normalize_cost = true;}
        else if (arg == vcl_string ("-prune")) { pruning_threshold = double(atof(argv[++i]));}
        else
        {
          vcl_cout << "Usage: " << argv[0] << "[-help] [-v {0|1|2}] [-esf esf directory] [-img image directory] [-con con directory] " << vcl_endl;
          vcl_cout << "[-outf out_file] [-outd directory for output images] [-outshgm directory for output shgm files (not saving if this argument does not exist)] " << vcl_endl;
          vcl_cout << "[-prune shock pruning before matching, default = -1.0 (no pruning)] [-ds shock curve sampling ds, default = 1.0] [-elastic use elastic splice cost]" << vcl_endl;
          vcl_cout << "[-normalize create the matrix with normalized costs] [-elastic use elastic splice cost] " << vcl_endl;
          vcl_cout << "[-res resolution (064)] [-dbstring additional db string to be added to end of each name] " << vcl_endl;
          throw -1;
        }
      }

      cluster_db_matching match(database_list, resolution, addition, images_dir, verbose);
      output_dir_shgm = output_dir_shgm + resolution + "-" + addition + "/";
      vcl_cout << "output_dir_shgm: " << output_dir_shgm << vcl_endl;
      output_dir = output_dir + resolution + "-" + addition + "/";
      match.execute_shock(cons_dir, esfs_dir, save_output_img, output_dir, sampling_ds, pruning_threshold, elastic_splice, normalize_cost, save_shgm, output_dir_shgm);

      //: write the output distance matrix to a file
      if (elastic_splice)
        out_file = out_file + "_elsp";
      if (normalize_cost)
        out_file = out_file + "_norm";

      out_file = out_file + "_" + resolution + "_";
      vcl_string dummy = addition;
      char *pch = vcl_strtok ((char *)dummy.c_str(),"-");
      while (pch != NULL)
      {
        out_file = out_file + pch + "_";
        pch = strtok (NULL, "-");
      }
      out_file = out_file+"shock.out";
      
      vcl_vector<vcl_vector<double> > & matrix = match.get_dist_matrix();
      vcl_ofstream of((out_file).c_str());
      of << "\n shock costs: \n" << matrix.size() << " " << matrix[0].size() << "\n";
      for (unsigned i = 0; i<matrix.size(); i++) {
        for (unsigned j = 0; j<matrix[i].size(); j++)
          of << matrix[i][j] << " ";
        of << "\n";
      }

      matrix = match.get_info_matrix();
      of << "\n infos: \n" << matrix.size() << " " << matrix[0].size() << "\n";
      for (unsigned i = 0; i<matrix.size(); i++) {
        for (unsigned j = 0; j<matrix[i].size(); j++)
          of << matrix[i][j] << " ";
        of << "\n";
      }

      of.close();
      
  } catch (int i) {
        // Error detected in this method
        returnStatus = i;
  } catch (cluster_exception& e) {
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
