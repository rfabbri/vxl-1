// This is brcv/rec/dber/pro/dber_load_instances_process.cxx

//:
// \file

#include "dber_load_instances_process.h"

#include <vcl_ctime.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_cstdio.h>

#include <bpro1/bpro1_process_sptr.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/vidpro1_process_manager.h>

#include <dber/pro/dber_instance_storage_sptr.h>
#include <dber/pro/dber_instance_storage.h>

#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>

#include <bvis1/bvis1_manager.h>
#include <vidpro1/vidpro1_repository.h>

dber_load_instances_process::dber_load_instances_process() : bpro1_process(), instance_storage_(0)
{  
  if (!parameters()->add( "Input Video Instances file 1 <filename...>" , 
    "-file" ,
    bpro1_filepath("","*.dat") 
    ) ||
    !parameters()->add( "number of rows for visualization: " , "-nrows", static_cast<unsigned>(10)) || 
    !parameters()->add( "number of cols for visualization: " , "-ncols", static_cast<unsigned>(50)) 
    )
    {
      vcl_cerr << "ERROR: Adding parameters in dber_load_instances_process::dber_load_instances_process()" << vcl_endl;
    }
}


//: Clone the process
bpro1_process*
dber_load_instances_process::clone() const
{
  return new dber_load_instances_process(*this);
}

bool dber_load_instances_process::execute()
{
  vcl_string file_name;
  bpro1_filepath input_path;
  parameters()->get_value( "-file" , input_path);
  file_name = input_path.path;
  unsigned n_rows, n_cols;
  parameters()->get_value( "-nrows", n_rows);
  parameters()->get_value( "-ncols", n_cols);
  
  vsl_b_ifstream obfile(file_name);
  instance_storage_ = dber_instance_storage_new();

  vcl_vector< vcl_vector<dbru_multiple_instance_object_sptr> >& frames = instance_storage_->get_frames();
  unsigned numframes=0;
  vsl_b_read(obfile,numframes);
  for(unsigned i=0;i<numframes;i++)
  {
      vcl_vector<dbru_multiple_instance_object_sptr> temp;
      vsl_b_read(obfile,temp);
      vcl_cout << "frame: " << i << " size: " << temp.size() << vcl_endl;
      frames.push_back(temp);
  }

  obfile.close();
  instance_storage_->set_frame(-2); // for global storage
  instance_storage_->set_nrows(n_rows);
  instance_storage_->set_ncols(n_cols);
  
  vcl_cout << "---------------------------------\nTHIS VIDEO INSTANCE FILE has ";
  vcl_cout << instance_storage_->size() << " frames\n";
  vcl_cout << "(Each row is a frame, columns in a row are first views of instances of that particular object)\n";
  vcl_cout << "---------------------------------\n";
  this->finish();
  return true;
}

//: Finish
bool
dber_load_instances_process::finish() 
{
  if(!instance_storage_)
    return false;
  vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
   if(!res)
     return false;
   res->initialize_global();
   //more official way
   instance_storage_->mark_global();
   //Old way of enforcing global
   if(!res->store_data(instance_storage_))
    {
      vcl_cout << "In dber_load_instances_process::finish() "
               << "store to repository failed\n";
      return false;
    }
  return true;
}

