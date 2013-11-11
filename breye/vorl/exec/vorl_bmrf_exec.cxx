// This is breye/vorl/exec/vorl_bmrf_exec.cxx
//:
// \file

#include <vorl/vorl.h>
#include <bpro/bpro_parameters.h>
#include <vidpro/vidpro_repository.h>
#include <vidpro/process/vidpro_VD_edge_process.h>
#include <dbmrf/pro/dbmrf_network_builder_process.h>
#include <dbmrf/pro/dbmrf_curve_3d_builder_process.h>
#include <dbmrf/pro/dbmrf_curve_projector_process.h>

#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_vtol_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <dbmrf/pro/dbmrf_bmrf_storage.h>
#include <dbmrf/pro/dbmrf_curvel_3d_storage.h>

#include <bmrf/bmrf_curve_3d.h>
#include <bmrf/bmrf_curvel_3d.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/algo/vnl_qr.h>
#include <vnl/vnl_det.h>

#include <vcl_cmath.h>
#include <vcl_algorithm.h>


//: decompose the camera into internal and external params
bool 
decompose_camera( const vnl_double_3x4& camera,
                        vnl_double_3x3& internal,
                        vnl_double_3x3& rotation,
                        vnl_double_3&   translation )
{
  // camera = [H t]
  //
  vnl_double_3x3 PermH;

  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      PermH(i,j) = camera(2-j,2-i);

  vnl_qr<double> qr(PermH);

  vnl_double_3x3 Q = qr.Q();
  vnl_double_3x3 R = qr.R();

  // Ensure all diagonal components of C are positive.
  // Must insert a det(+1) or det(-1) mx between.
  int r0pos = R(0,0) > 0 ? 1 : 0;
  int r1pos = R(1,1) > 0 ? 1 : 0;
  int r2pos = R(2,2) > 0 ? 1 : 0;
  typedef double d3[3];
  d3 diags[] = {   // 1 2 3
    { -1, -1, -1}, // - - -
    {  1, -1, -1}, // + - -
    { -1,  1, -1}, // - + -
    {  1,  1, -1}, // + + -
    { -1, -1,  1}, // - - +
    {  1, -1,  1}, // + - +
    { -1,  1,  1}, // - + +
    {  1,  1,  1}, // + + +
  };
  int d = r0pos * 4 + r1pos * 2 + r2pos;
  double* diag = &diags[d][0];
 
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) {
      internal(j,i) = diag[i] * R(2-i,2-j);
      rotation(j,i) = diag[j] * Q(2-i,2-j);
    }
  vcl_cout << internal << vcl_endl;
  // Compute t' = inv(C) t
  vnl_double_3 t;
  for (int i = 0; i < 3; ++i)
    t[i] = camera(i,3);

  t[2] /= internal(2,2);
  t[1] = (t[1] - internal(1,2)*t[2])/internal(1,1);
  t[0] = (t[0] - internal(0,1)*t[1] - internal(0,2)*t[2])/internal(0,0);

  translation = t;

  // Recompose the matrix and compare
  vnl_double_3x4 Po;
  Po.update(rotation);
  Po.set_column(3,translation);
  if (((internal * Po - camera).fro_norm() > 1e-4) ||
      (internal(0,0) < 0) ||
      (internal(1,1) < 0) ||
      (internal(2,2) < 0)) {
    return false;
  }

  // Scale the internal calibration matrix such that the bottom right is 1
  internal /= internal(2,2);

  return true;
}


//: Write the header info to a VRML file
void write_vrml_header(vcl_ofstream& str)
{
  str << "#VRML V2.0 utf8\n\n";
}

//: Write the cameras to the VRML file
void write_vrml_cameras( vcl_ofstream& str,
                         const vcl_vector<vnl_double_3x4>& cameras )
{
  for(unsigned int i=0; i<cameras.size(); ++i){
    vnl_double_3x3 K, R;
    vnl_double_3 t;
    decompose_camera( cameras[i], K, R, t);
    vnl_double_3 ctr = -R.transpose() * t;
    
    R.scale_row(1,-1);
    R.scale_row(2,-1);
    if(vnl_det(R) < 0)
      R.scale_row(1,-1);
    vnl_quaternion<double> ornt(R);
    double fov = vcl_max(vcl_atan(K[1][2]/K[1][1]), vcl_atan(K[0][2]/K[0][0]));
    str << "Viewpoint {\n"
        << "  position    "<< ctr[0] << ' ' << ctr[1] << ' ' << ctr[2] << '\n'
        << "  orientation "<< ornt.axis() << ' '<< ornt.angle() << '\n'
        << "  fieldOfView "<< fov << '\n'
        << "  description \"Camera" << i << "\"\n"
        << "}\n";
  }
}


//: Write a curve to the VRML file
void write_vrml_bbox( vcl_ofstream& str,
                      const vnl_double_4x4& bbox_xform )
{
  str << "DEF BoundingBox Shape {\n"
      << "  geometry \n"
      << "  IndexedLineSet {\n"
      << "    coord \n"
      << "    Coordinate {\n"
      << "      point [ ";
  for (int i=0; i<8; ++i){
    vnl_double_4 p((i/4)%2, (i/2)%2, i%2, 1);
    vnl_double_4 pt = bbox_xform * p;
    str << pt[0]/pt[3] << " " << pt[1]/pt[3] << " " << pt[2]/pt[3] << ", ";
  }
  str << " ]\n"
      << "    }\n"
      << "    colorPerVertex FALSE\n"
      << "    color Color {\n"
      << "      color [ 0 0 1, 0 0 1, 0 0 1, 0 0 1 ]\n"
      << "    }\n"
      << "    coordIndex [ 0, 1, 3, 2, 0, 4, 5, 7, 6, 4, -1\n"
      << "                 1, 5, -1, 2, 6, -1, 3, 7, -1 ]\n"
      << "  }\n"
      << "} \n";

}


//: Write a curve to the VRML file
void write_vrml_curve( vcl_ofstream& str,
                       const bmrf_curve_3d_sptr& curve )
{
  str << "Shape {\n"
      << "  appearance NULL\n"
      << "    geometry IndexedLineSet {\n"
      << "      color NULL\n"
      << "      coord Coordinate{\n"
      << "       point[\n";
  for ( bmrf_curve_3d::const_iterator itr = curve->begin();
        itr != curve->end();  ++itr )
  {
    str << (*itr)->x() << ' ' << (*itr)->y() << ' ' << (*itr)->z() << '\n';
  }
  str << "   ]\n"
      << "  }\n";

  str << "   coordIndex [\n"; 
  for (unsigned int i=0; i<curve->size(); ++i)
    str << i << ", ";
  str << " -1\n";
  str << "  ]\n";
  str << " }\n"
      << "}\n";
}


//: Write a set of curves to the VRML file
void write_vrml_curve_set( vcl_ofstream& str,
                           const vcl_set<bmrf_curve_3d_sptr>& curves )
{
  for ( vcl_set<bmrf_curve_3d_sptr>::const_iterator itr = curves.begin();
        itr != curves.end();  ++itr )
  {
    write_vrml_curve(str, *itr);
  }
}


// The Main Function
int main(int argc, char** argv)
{
  // Register storage types and processes
  REG_STORAGE(vidpro_image_storage);
  REG_STORAGE(vidpro_vtol_storage);
  REG_STORAGE(vidpro_vsol2D_storage);
  REG_STORAGE(dbmrf_bmrf_storage);
  REG_STORAGE(dbmrf_curvel_3d_storage);

  //==========================================================

  // Make each process and add them to the list of program args
  bpro_process_sptr edge_detector(new vidpro_VD_edge_process());
  edge_detector->set_input_names(vcl_vector<vcl_string>(1,"video"));
  edge_detector->set_output_names(vcl_vector<vcl_string>(1,"edges"));
  vorl_manager::instance()->add_process_to_args(edge_detector);

  bpro_process_sptr network_builder(new dbmrf_network_builder_process());
  vcl_vector<vcl_string> network_input_names;
  network_input_names.push_back("video");
  network_input_names.push_back("edges");
  network_builder->set_input_names(network_input_names);
  network_builder->set_output_names(vcl_vector<vcl_string>(1,"network"));
  vorl_manager::instance()->add_process_to_args(network_builder);

  bpro_process_sptr curve_reconstructor(new dbmrf_curve_3d_builder_process());
  curve_reconstructor->set_input_names(vcl_vector<vcl_string>(1,"network"));
  curve_reconstructor->set_output_names(vcl_vector<vcl_string>(1,"curves3D"));
  vorl_manager::instance()->add_process_to_args(curve_reconstructor);

  bpro_process_sptr curve_projector(new dbmrf_curve_projector_process());
  curve_projector->set_input_names(vcl_vector<vcl_string>(1,"curves3D"));
  curve_projector->set_output_names(vcl_vector<vcl_string>(1,"curves2D"));
  vorl_manager::instance()->add_process_to_args(curve_projector);

  //===========================================================

  // Parse the arguments and load the video
  vorl_manager::instance()->parse_params(argc, argv);  
  vorl_manager::instance()->load_video();
  
  float epu=0,epv=0;
  network_builder->parameters()->get_value("-nbeu", epu);
  network_builder->parameters()->get_value("-nbev", epv);
  if(epu == 0.0f && epv == 0.0f){
    bpro_filepath camera_path;
    curve_reconstructor->parameters()->get_value("-camera", camera_path);
    vcl_ifstream fp(camera_path.path.c_str());
    if(fp.is_open()){
      vnl_double_3x4 C;
      fp >> C;
      fp.close();
      vnl_double_3 nep = C.get_column(0);   
      network_builder->parameters()->set_value("-nbeu", float(nep[0]/nep[2]));
      network_builder->parameters()->set_value("-nbev", float(nep[1]/nep[2]));
    }
  }

  //===========================================================

  // Set up the process queue to be run on all frames
  vorl_manager::instance()->add_process_to_queue(edge_detector);
  vorl_manager::instance()->add_process_to_queue(network_builder);
  vorl_manager::instance()->add_process_to_queue(curve_reconstructor);

  // run the process queue on the whole video
  do {
    float status = vorl_manager::instance()->status()/2.0;
    vorl_manager::instance()->write_status(status);
    vorl_manager::instance()->run_process_queue_on_current_frame();
  }  while(vorl_manager::instance()->next_frame());
  vorl_manager::instance()->finish_process_queue();

  
  //===========================================================

  // Rewind to the first frame processed and set a new queue
  vorl_manager::instance()->rewind();
  vorl_manager::instance()->clear_process_queue();
  vorl_manager::instance()->add_process_to_queue(curve_projector);

  // run the process queue on the whole video
  do {
    float status = vorl_manager::instance()->status()/2.0 + 0.5;
    vorl_manager::instance()->write_status(status);
    vorl_manager::instance()->run_process_queue_on_current_frame();
  }  while(vorl_manager::instance()->next_frame());
  vorl_manager::instance()->finish_process_queue();
  vorl_manager::instance()->write_status(1.0);

  //===========================================================

  vidpro_repository_sptr rep = vorl_manager::instance()->repository();
  bpro_storage_sptr result = rep->get_data_by_name("curves3D");
  dbmrf_curvel_3d_storage_sptr curvel_storage;
  curvel_storage.vertical_cast(result);
 
  vcl_set< bmrf_curve_3d_sptr > curves;
  curvel_storage->get_curvel_3d( curves );
  vnl_double_4x4 bbox_xform = curvel_storage->bb_xform();
  vcl_vector<vnl_double_3x4> cameras;

  // Write out the bounding box file
  vorl_manager::instance()->rewind();
  vcl_string out_dir = vorl_manager::instance()->get_output_dir();
  vcl_ofstream bbout(vcl_string(out_dir+"/bbox_cam.txt").c_str());
  bbout << "Bounding Box Transform\n" << bbox_xform << '\n';
  do {
    result = rep->get_data_by_name("curves3D");
    curvel_storage.vertical_cast(result);
    vnl_double_3x4 camera = curvel_storage->camera();
    bbout << "Camera " << rep->current_frame() << '\n' << camera << '\n';
    cameras.push_back(camera);
  } while(vorl_manager::instance()->next_frame());
  bbout.close();
  
  // Create VRML output
  vcl_string vrml_file = vorl_manager::instance()->get_output_vrml_dir() + "/model.wrl";
  vcl_ofstream vrml(vrml_file.c_str());
  write_vrml_header(vrml);
  write_vrml_cameras(vrml, cameras);
  write_vrml_bbox(vrml, bbox_xform);
  write_vrml_curve_set(vrml,curves);
  vrml.close();

  // Save part of the repository
  vcl_set<vcl_string> names;
  names.insert("network");
  names.insert("curves3D");
  names.insert("curves2D");
  vcl_string output_dir = vorl_manager::instance()->get_output_dir();
  vcl_string binfile=output_dir+"/bin.rep";
  vorl_manager::instance()->save_repository(binfile, names);

  return 0; 
}




