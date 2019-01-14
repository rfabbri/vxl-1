//:
// \file
// \brief 
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/25/07
//
//

#include "example_processor.h"
#include <iostream>

//: this method is run on each processor after lead processor broadcasts its command
//  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
//bool example_processor::parse_command_line(int argc, char *argv[])
bool example_processor::parse_command_line(std::vector<std::string>& argv)
{
  if (!argv.size()) {
    std::cout << "argv size is 0! Exit!\n";
    return false;
  } else
    std::cout << " argv size: " << argv.size() << std::endl;
  param_file_ = argv[0];
  return true; // nothing to parse
}

//: this method is run on each processor
bool example_processor::parse(const char* param_file)
{
  b_ = 1.0f;
  return true;
}

//: this method is run on each processor
bool example_processor::initialize(std::vector<int>& t)
{
  for (unsigned i = 0; i < 4; i++) {
    t.push_back(i + 1);
  }

  std::cout << "initialized " << t.size() << " integers as follows\n";
  for (unsigned i = 0; i < t.size(); i++)
    std::cout << t[i] << " ";
  std::cout << std::endl;

  return true;
}

//: this method is run in a distributed mode on each processor on the cluster
bool example_processor::process(int t1, float& f)
{
  f = float(t1 + b_);
  return true;
}

bool example_processor::finalize(std::vector<float>& results)
{
  float sum = 0.0f;
  for (unsigned i = 0; i < results.size(); i++) {
    sum += results[i];
  }
  std::cout << "sum of the results: " << sum << " from " << results.size() << " sized vector\n";
  std::cout << "sum of the results should be 14\n";
  return true;
}

#ifdef MPI_CPP_BINDING
MPI::Datatype example_processor::create_datatype_for_R()
{
  return MPI::FLOAT;
}
#else
MPI_Datatype example_processor::create_datatype_for_R()
{
  return MPI_FLOAT;
}
#endif

