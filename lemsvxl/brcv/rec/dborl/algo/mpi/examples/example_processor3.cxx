//:
// \file
// \brief
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/30/07

//
//

#include "example_processor3.h"
#include <vcl_iostream.h>

//: this method is run on each processor after lead processor broadcasts its command
//  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
//bool example_processor3::parse_command_line(int argc, char *argv[])
bool example_processor3::parse_command_line(vcl_vector<vcl_string>& argv)
{
  param_file_ = "dummy";
  return true; // nothing to parse
}

//: this method is run on each processor
bool example_processor3::parse(const char* param_file)
{
  var_ = 1.0f;
  b_size_ = EX_CLASS3_B_SIZE;
  c_size_ = EX_CLASS3_C_SIZE;
  return true;
}

//: this method is run on each processor
bool example_processor3::initialize(vcl_vector<int>& t)
{
  for (unsigned i = 0; i < 4; i++) {
    t.push_back(int(i + var_));
  }

  vcl_cout << "initialized " << t.size() << " integers as follows\n";
  for (unsigned i = 0; i < t.size(); i++)
    vcl_cout << t[i] << " ";
  vcl_cout << vcl_endl;

  return true;
}

//: this method is run in a distributed mode on each processor on the cluster
bool example_processor3::process(int t1, ex_class3& f)
{
  f.a = t1;
  for (unsigned i = 0; i < b_size_; i++)
    f.b[i] = '#';
  f.b[0] = 'a' + t1%3;
  for (unsigned i = 0; i < c_size_; i++)
    f.c[i] = float(t1 + var_ + i);
  return true;
}

//: this method is run on the lead processor once after results are collected from each processor
bool example_processor3::finalize(vcl_vector<ex_class3>& results)
{
  float sum = 0.0f;
  for (unsigned i = 0; i < results.size(); i++) {
    for (unsigned j = 0; j < c_size_; j++)
      sum += results[i].c[j];
  }
  vcl_cout << "sum of the results: " << sum << " from " << results.size() << " sized vector, string is: ";
  for (unsigned i = 0; i < results.size(); i++) {
    vcl_cout << results[i].b;
  }
  vcl_cout << "\n";
  vcl_cout << "sum of the results should be 110, and the string should be something like:\n";
  vcl_cout << "b##...##c##...##a##...##b##...## with many #'s\n";
  return true;
}
#ifdef MPI_CPP_BINDING
MPI::Datatype example_processor3::create_datatype_for_R()
{
  MPI::Datatype ex_class3_type;
  MPI::Datatype type[4] = {MPI::INT, MPI::CHAR, MPI::FLOAT, MPI::UB};  // MPI_UB should be added at the end for each type to be safe
  int blocklen[4] = {1, b_size_, c_size_, 1};  // number of items from each type
  MPI::Aint disp[4];

  // compute byte displacements of each component, create a dummy instance array
  ex_class3 dummy[2];
  //MPI_Address( dummy, disp);
  //MPI_Address( dummy[0].b, disp + 1);
  //MPI_Address( dummy[0].c, disp + 2);
  //MPI_Address( dummy+1, disp + 3);
  disp[0] = MPI::Aint(&dummy);
  disp[1] = MPI::Aint(&dummy[0].b);
  disp[2] = MPI::Aint(&dummy[0].c);
  disp[3] = MPI::Aint(&dummy[1]);
  int base = disp[0];
  for (int i = 0; i < 4; i++) disp[i] -= base;  // get rid of initial address, we only need displacements
  ex_class3_type = MPI::Datatype::Create_struct ( 4, blocklen, disp, type);
  ex_class3_type.Commit();
  return ex_class3_type;
}
#else
MPI_Datatype example_processor3::create_datatype_for_R()
{
  MPI_Datatype ex_class3_type;
  MPI_Datatype type[4] = {MPI_INT, MPI_CHAR, MPI_FLOAT, MPI_UB};  // MPI_UB should be added at the end for each type to be safe
  int blocklen[4] = {1, b_size_, c_size_, 1};  // number of items from each type
  MPI_Aint disp[4];

  // compute byte displacements of each component, create a dummy instance array
  ex_class3 dummy[2];
  //MPI_Address( dummy, disp);
  //MPI_Address( dummy[0].b, disp + 1);
  //MPI_Address( dummy[0].c, disp + 2);
  //MPI_Address( dummy+1, disp + 3);
  disp[0] = MPI_Aint(&dummy);
  disp[1] = MPI_Aint(&dummy[0].b);
  disp[2] = MPI_Aint(&dummy[0].c);
  disp[3] = MPI_Aint(&dummy[1]);
  int base = disp[0];
  for (int i = 0; i < 4; i++) disp[i] -= base;  // get rid of initial address, we only need displacements
  MPI_Type_struct ( 4, blocklen, disp, type, &ex_class3_type);
  MPI_Type_commit ( &ex_class3_type );
  return ex_class3_type;
}
#endif

