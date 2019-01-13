//---------------------------------------------------------------------
// This is bbasd/bgld/algo/examples/example_generate_eulerspiral_lookup_table.cxx
//:
// \file
// \brief
//
// \author Based on original code by 
//  Nhon Trinh
// \date 2/15/2005
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <bgld/algo/bgld_eulerspiral.h>
#include <iostream>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/vnl_math.h>


int main( int , char*[] )
{
  std::cout << "EULER SPIRAL LOOK-UP TABLE GENERATOR" << std::endl << std::endl;
  std::cout << "This program creates a look-up table for speeding Euler spiral computation" << std::endl;
  std::cout << "It creates three tables npts x npts of k0, gamma, and length " <<
    "of a normalized Euler spiral, i.e. start = (0, 0) and end = (1, 0)" << std::endl;
  std::cout << "The two variables of the tables are start_angle and end_angles ranging in [0, 2pi)" << std::endl;
  
  int npts = 100;
  std::string es_file("bgld_eulerspiral_lookup_table.bvl");

  // create the table.
  std::vector< std::vector< double > > k0_table;
  std::vector< std::vector< double > > gamma_table;
  std::vector< std::vector< double > > len_table;

  double step = vnl_math::pi*2.0 / npts;
  double start_angle = 0;
  double end_angle = 0;
  
  vgl_point_2d< double > start(0, 0);
  vgl_point_2d< double > end(1, 0);
  double k0;
  double gamma;
  double len;
  std::cout << std::endl << " ----- Generate tables for k0, gamma, and length ------ " 
    << std::endl << std::endl;
  bgld_eulerspiral es;
  std::cout << "(i, Start angle) = " << std::endl;
  int num_failed_cases = 0;
  for (int i = 0; i < npts; i ++){
    
    start_angle = i * step; // + step * 0.5;
    std::cout << "\t( " << i << " , " << start_angle << " )";
    std::vector< double > k0_row;
    std::vector< double > gamma_row;
    std::vector< double > len_row;
    for (int j = 0; j< npts; j++){
      end_angle = j * step; // + step * 0.5;
      es.set_params(start, start_angle, end, end_angle);
      if (! es.compute_es_params(false, false )){
        std::cout << "j = " << j << " - Euler spiral computation failed" << std::endl;
        num_failed_cases ++;
        
      }
      k0 = es.k0();
      gamma = es.gamma();
      len = es.length();
      
      // add to the rows
      k0_row.push_back(k0);
      gamma_row.push_back(gamma);
      len_row.push_back(len);
    }
    k0_table.push_back(k0_row);
    gamma_table.push_back(gamma_row);
    len_table.push_back(len_row);
  }

  std::cout << std::endl << "Completed generating tables" << std::endl;
  std::cout << "Number of failed cases = " << num_failed_cases << std::endl;
  std::cout << "Saving all three tables to file " << es_file << " ... ";
  vsl_b_ofstream out_stream = vsl_b_ofstream(es_file);
  // write to file

  vsl_b_write(out_stream, npts);
  // k0
  for (int i = 0; i < npts; i++ ){
    vsl_b_write(out_stream, k0_table.at(i));
  }

  // gamma
  for (int i = 0; i < npts; i++ ){
    vsl_b_write(out_stream, gamma_table.at(i));
  }

  // len
  for (int i = 0; i < npts; i++ ){
    vsl_b_write(out_stream, len_table.at(i));
  }

  out_stream.close();
  std::cout << "completed." << std::endl;
  

  // verify data
  std::cout << "----------- Verify output file ---------" << std::endl << std::endl ;

  std::vector < std::vector< double > > k0_table_new;
  std::vector < std::vector< double > > gamma_table_new;
  std::vector < std::vector< double > > len_table_new;

  std::cout << "Reading file " << es_file << std::endl ;
  vsl_b_ifstream in_stream(es_file);
  if (!in_stream){
    std::cerr<<"Failed to open " << es_file << " for input." << std::endl;
  }
  std::cout << "Opened file successfully " << std::endl;
  
  // k0
  std::cout << "Reading k0 table ... ";
  int npts_new;
  vsl_b_read(in_stream, npts_new);
  for (int i = 0; i < npts_new; i++ ){
    std::vector< double > k0_row_new;
    vsl_b_read(in_stream, k0_row_new);
    k0_table_new.push_back(k0_row_new);
  }
  std::cout << "done. " << std::endl;

  // gamma
  std::cout << "Reading gamma table ... ";
  for (int i = 0; i < npts_new; i++ ){
    std::vector< double > gamma_row_new;
    vsl_b_read(in_stream, gamma_row_new);
    gamma_table_new.push_back(gamma_row_new);
  }
  std::cout << "done. " << std::endl;

  // len
  std::cout << "Reading len table ... ";
  for (int i = 0; i < npts_new; i++ ){
    std::vector< double > len_row_new;
    vsl_b_read(in_stream, len_row_new);
    len_table_new.push_back(len_row_new);
  }
  std::cout << " done." << std::endl;
  in_stream.close();

  std::cout << "Comparing results from tables against computed results " << std::endl;
  double tolerance = 1e-3;
  bool verified = true;
  if (npts_new == npts){
    for (int i =0; i < npts_new; i ++){
      for (int j = 0; j < npts_new; j ++){
        double error = std::fabs(k0_table.at(i).at(j) - k0_table.at(i).at(j)) + 
          std::fabs(gamma_table.at(i).at(j) - gamma_table.at(i).at(j)) + 
          std::fabs(len_table.at(i).at(j) - len_table.at(i).at(j));
        if (error > 3* tolerance){
          verified = false;
          std::cout << "Error at (i, j) = " << i << " ,  " << j << " )" << std::endl;
        } 
      }
    }
  }
  else{
    std::cerr << "npts_new != npts" << std::endl;
    verified = false;
  }

  if (verified)
    std::cout << "Verification completed successfully - no error " << std::endl;
  else
    std::cout << "Verification failed. " << std::endl;
  
  // Examples
  std::cout << std::endl <<  " ----- Example -----------" << std::endl;
  start_angle = 1.5;
  end_angle = 1.1;
  es.compute_es_params(start, start_angle, end, end_angle);
  es.print(std::cout);
  int start_index = (int) (start_angle/ step);
  int end_index = (int) (end_angle/ step);
  std::cout << "Table result " << std::endl;
  std::cout << " k0 = " << k0_table_new.at(start_index).at(end_index) << std::endl;
  std::cout << " gamma = " << gamma_table_new.at(start_index).at(end_index) << std::endl;
  std::cout << " len = " << len_table_new.at(start_index).at(end_index) << std::endl;
  
  return 0;
}

