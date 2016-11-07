// This is brcv/rec/dbru/pro/dbru_load_osl_process.cxx

//:
// \file

#include "dbru_load_osl_process.h"

#include <vcl_ctime.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_cstdio.h>

#include <bpro1/bpro1_process_sptr.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/vidpro1_process_manager.h>

#include <dbru/pro/dbru_osl_storage_sptr.h>
#include <dbru/pro/dbru_osl_storage.h>
#include <dbru/dbru_object.h>
#include <dbru/dbru_osl.h>

#include <bvis1/bvis1_manager.h>
#include <vidpro1/vidpro1_repository.h>

dbru_load_osl_process::dbru_load_osl_process() : bpro1_process(), osl_storage_(0)
{  
  if (!parameters()->add( "Input OSL file 1 <filename...>" , 
    "-file" ,
    bpro1_filepath("","*.bin") 
    ) ||
    !parameters()->add( "OSL old version (created before August 2006)?: " , "-old", false) || 
    !parameters()->add( "number of rows for visualization: " , "-nrows", static_cast<unsigned>(25)) || 
    !parameters()->add( "number of cols for visualization: " , "-ncols", static_cast<unsigned>(50)) 
    )
    {
      vcl_cerr << "ERROR: Adding parameters in dbru_load_osl_process::dbru_load_osl_process()" << vcl_endl;
    }
}


//: Clone the process
bpro1_process*
dbru_load_osl_process::clone() const
{
  return new dbru_load_osl_process(*this);
}

bool dbru_load_osl_process::execute()
{
  vcl_string osl_file_name;
  bpro1_filepath input_path;
  parameters()->get_value( "-file" , input_path);
  osl_file_name = input_path.path;
  bool old_version;
  parameters()->get_value( "-old", old_version);
  unsigned n_rows, n_cols;
  parameters()->get_value( "-nrows", n_rows);
  parameters()->get_value( "-ncols", n_cols);
  
  vsl_b_ifstream obfile(osl_file_name);
  osl_storage_ = dbru_osl_storage_new();
  if (old_version) {
    unsigned size;
    vsl_b_read(obfile, size);
    for (unsigned i = 0; i<size; i++) {
      dbru_object_sptr obj = new dbru_object();
      obj->b_read(obfile);
      osl_storage_->osl()->add_object(obj);
    }
  } else {
    osl_storage_->b_read(obfile);
  }
  obfile.close();
  osl_storage_->set_frame(-2); // for global storage
  osl_storage_->set_nrows(n_rows);
  osl_storage_->set_ncols(n_cols);
  
  vcl_cout << "---------------------------------\nTHIS OSL has ";
  vcl_cout << osl_storage_->get_osl_size() << " objects\n";
  vcl_cout << "(Each row is an object, columns in a row are prototypes of that particular object)\n";
  vcl_cout << "---------------------------------\n";
  this->finish();
  return true;
}

//: Finish
bool
dbru_load_osl_process::finish() 
{
  if(!osl_storage_)
    return false;
  vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
   if(!res)
     return false;
   res->initialize_global();
   //more official way
   osl_storage_->mark_global();
   //Old way of enforcing global
   if(!res->store_data(osl_storage_))
    {
      vcl_cout << "In dbinfo_osl_display_tool::display_osl() "
               << "store to repository failed\n";
      return false;
    }
  return true;
}

