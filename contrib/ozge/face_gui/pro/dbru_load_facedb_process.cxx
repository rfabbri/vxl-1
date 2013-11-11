// This is ozge/face_gui/pro/dbru_load_facedb_process.cxx

//:
// \file

#include "dbru_load_facedb_process.h"

#include <vcl_ctime.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_cstdio.h>

#include <bpro1/bpro1_process_sptr.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/vidpro1_process_manager.h>

//#include <dbru/pro/dbru_facedb_storage_sptr.h>
#include <ozge/face_gui/pro/dbru_facedb_storage.h>
#include <ozge/face_gui/dbru_facedb.h>

#include <bvis1/bvis1_manager.h>
#include <vidpro1/vidpro1_repository.h>

dbru_load_facedb_process::dbru_load_facedb_process() : bpro1_process(), facedb_storage_(0)
{  
  if (!parameters()->add( "Input facedb file 1 <filename...>" , 
    "-file" ,
    bpro1_filepath("","*.bin") 
    ) ||
    !parameters()->add( "number of rows for visualization: " , "-nrows", static_cast<unsigned>(25)) || 
    !parameters()->add( "number of cols for visualization: " , "-ncols", static_cast<unsigned>(50)) 
    )
    {
      vcl_cerr << "ERROR: Adding parameters in dbru_load_facedb_process::dbru_load_facedb_process()" << vcl_endl;
    }
}


//: Clone the process
bpro1_process*
dbru_load_facedb_process::clone() const
{
  return new dbru_load_facedb_process(*this);
}

bool dbru_load_facedb_process::execute()
{
  vcl_string facedb_file_name;
  bpro1_filepath input_path;
  parameters()->get_value( "-file" , input_path);
  facedb_file_name = input_path.path;
  unsigned n_rows, n_cols;
  parameters()->get_value( "-nrows", n_rows);
  parameters()->get_value( "-ncols", n_cols);
  
  vsl_b_ifstream obfile(facedb_file_name);
  facedb_storage_ = dbru_facedb_storage_new();
  facedb_storage_->b_read(obfile);
  obfile.close();
  facedb_storage_->set_frame(-2); // for global storage
  facedb_storage_->set_nrows(n_rows);
  facedb_storage_->set_ncols(n_cols);
  
  vcl_cout << "---------------------------------\nTHIS facedb has ";
  vcl_cout << facedb_storage_->get_facedb_size() << " objects\n";
  vcl_cout << "(Each row is an object, columns in a row are prototypes of that particular object)\n";
  vcl_cout << "---------------------------------\n";
  this->finish();
  return true;
}

//: Finish
bool
dbru_load_facedb_process::finish() 
{
  if(!facedb_storage_)
    return false;
  vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
   if(!res)
     return false;
   res->initialize_global();
   //more official way
   facedb_storage_->mark_global();
   //Old way of enforcing global
   if(!res->store_data(facedb_storage_))
    {
      vcl_cout << "In dbinfo_facedb_display_tool::display_facedb() "
               << "store to repository failed\n";
      return false;
    }
  return true;
}

