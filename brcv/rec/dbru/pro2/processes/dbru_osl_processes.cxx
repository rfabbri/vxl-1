// This is lemsvxlsrc/brcv/rec/dbru/pro2/processes/dbru_osl_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief OSL related processes
//
// \author Ozge Can Ozcanli
// \date 03/23/09
//
// \verbatim
//  Modifications
//     none yet
// \endverbatim

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <brdb/brdb_value.h>
#include <dbru/dbru_osl.h>
#include <dbru/dbru_osl_sptr.h>
#include <dbru/dbru_object.h>
#include <bvgl/bvgl_changes.h>
#include <bvgl/bvgl_changes_sptr.h>
#include <bsol/bsol_algs.h>

//: Constructor
bool dbru_load_objects_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string"); //input objects file 
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbru_osl_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

//: Execute the process
bool dbru_load_objects_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 1) {
    vcl_cout << "dbru_load_objects_process: The input number should be 1" << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vcl_string objects_file = pro.get_input<vcl_string>(i++);

  dbru_osl_sptr osl = new dbru_osl();

  vcl_ifstream dbfp(objects_file.c_str());
  if (!dbfp) {
    vcl_cout << "Problems in opening db object list file: " << objects_file << "!\n";
    return false;
  }

  vcl_cout << "reading database objects...\n";
  
  char buffer[1000]; 
  dbfp.getline(buffer, 1000);  // comment 
  vcl_string dummy;
  dbfp >> dummy;   // <contour_segmentation   
  dbfp >> dummy; // object_cnt="23">
  unsigned int size;
  sscanf(dummy.c_str(), "object_cnt=\"%d\">", &size); 

  for (unsigned i = 0; i<size; i++) {
    vcl_cout << "reading database object: " << i << "...\n";
    dbru_object_sptr obj = new dbru_object();
    if (!obj->read_xml(dbfp)) { 
      vcl_cout << "problems in reading database object number: " << i << vcl_endl;
      return 0;
    }
    osl->add_object(obj);
  }

  vcl_cout << "loaded " << osl->size() << " objects\n";

  pro.set_output_val<dbru_osl_sptr>(0, osl);
  return true;
}

// Create the bvgl_changes object instance for the given frame of the given video

//: Constructor
bool dbru_create_change_object_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbru_osl_sptr"); //input objects file 
  input_types.push_back("unsigned"); // video id
  input_types.push_back("unsigned"); // frame id
  input_types.push_back("vcl_string"); // name of the output file
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

//: Execute the process
bool dbru_create_change_object_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 4) {
    vcl_cout << "dbru_load_objects_process: The input number should be 1" << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  dbru_osl_sptr osl = pro.get_input<dbru_osl_sptr>(i++);
  unsigned video_id = pro.get_input<unsigned>(i++);
  unsigned frame_id = pro.get_input<unsigned>(i++);
  vcl_string output_file = pro.get_input<vcl_string>(i++);

  bvgl_changes_sptr changes = new bvgl_changes();

  //: check if each object has a polygon in the given frame of the given video
  for (unsigned jj = 0; jj < osl->size(); jj++) {
    dbru_object_sptr obj = osl->get_object(jj);
    if (obj->video_id_ != video_id)
      continue;
    if (obj->start_frame() > (int)frame_id || obj->end_frame() < (int)frame_id)
      continue;
    vsol_polygon_2d_sptr poly = obj->get_polygon(frame_id-obj->start_frame());
    vgl_polygon<double> p = bsol_algs::vgl_from_poly(poly);
    bvgl_change_obj_sptr cobj = new bvgl_change_obj(p, obj->category());
    changes->add_obj(cobj);
  }

  //: now write the changes as a binary file
  vsl_b_ofstream os(output_file);
  changes->b_write(os);
  
  return true;
}

