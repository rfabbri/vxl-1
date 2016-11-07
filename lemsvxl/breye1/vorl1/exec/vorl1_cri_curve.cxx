// This is breye1/vorl1/exec/vorl1_fgdetect_con_finder_exec.cxx
//:
// \file
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstring.h>
#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_cstdio.h>

#include <vidpro1/process/vidpro1_save_con_process.h>
#include <vul/vul_file.h>
#include <vorl1/vorl1.h>
#include <dbbgm/pro/dbbgm_aerial_bg_model_process.h>
#include <dbbgm/pro/dbbgm_aerial_fg_uncertainity_detect_process.h>
//#include <dbvrl/pro/dbvrl_register_images_process.h>
#include <dbdet/pro/dbdet_blob_finder_process.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <bpro1/bpro1_parameters_sptr.h>
#include <bpro1/bpro1_process_sptr.h>
#include <dbbgm/pro/dbbgm_storage.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <vnl/vnl_matrix.h>
#include <vimt/vimt_transform_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vcl_fstream.h>
#include <vbl/vbl_bounding_box.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>

#include <vidpro1/process/vidpro1_load_video_and_con_process.h>
#include <vidpro1/process/vidpro1_load_con_process.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vidpro1/process/vidpro1_load_image_process.h>

//FIXME: this is very bad
// local global paths should never be used
//#include "C:\Lems\lemsvxlsrc\contrib\han\dbcri\pro\dbcri_process.h"
//#include "C:\Lems\lemsvxlsrc\contrib\han\dbcri_accu\pro\dbcri_accu_process.h"
//#include "C:\Lemsnew\lemsvxlsrc\contrib\han\dbcri_accu_edge\pro\dbcri_accu_edge_process.h"
#include "C:\Lems\lemsvxlsrc\contrib\han\dbcri_whole\pro\dbcri_whole_process.h"
#include <vidpro1/storage/vidpro1_vtol_storage.h>

vcl_vector<vnl_matrix<double> > 
read_homographies(vcl_string filename)
{
    vcl_ifstream ifile(filename.c_str(),vcl_ios::in);
    vcl_vector<vnl_matrix<double> > homographies;
    if(!ifile)
        return homographies;
    char buffer[100];
    while(ifile.getline(buffer,100))
    {
        vnl_matrix<double> p(3,3);
        ifile>>p;
        homographies.push_back(p);
        ifile.getline(buffer,100);
    }

    return homographies;
}

void write_transformed_polygons(vcl_string homgfile, vcl_vector<vcl_vector<vsol_polygon_2d_sptr> > polygons,vcl_ofstream &ofile, int ni,int nj )
{
    //vcl_ofstream ofile(filename.c_str());
    ofile<<"NFRAMES: "<<polygons.size()<<"\n";

    vbl_bounding_box<double,2> box;
    vcl_vector<vnl_matrix<double> > homgs=read_homographies(homgfile);
    for(unsigned i=0;i<homgs.size();i++)
    {
        vimt_transform_2d p;
        p.set_affine(homgs[i].extract(2,3));
        
        box.update(p(0,0).x(),p(0,0).y());
        box.update(p(0,nj).x(),p(0,nj).y());
        box.update(p(ni,0).x(),p(ni,0).y());
        box.update(p(ni,nj).x(),p(ni,nj).y());
    }

    
    int offset_i=(int)vcl_ceil(0-box.min()[0]);
    int offset_j=(int)vcl_ceil(0-box.min()[1]);
    // iterating over frames
    for(unsigned int i=0;i<polygons.size();i++)
    {   
        vimt_transform_2d p;
        p.set_affine(homgs[i].extract(2,3));

        vimt_transform_2d hmgxform=p.inverse();
        //: iterating over polygons in one frame
        ofile<<"NOBJECTS: "<<polygons[i].size()<<"\n";
        ofile<<"NPOLYS: "<<polygons[i].size()<<"\n";
        for(unsigned int j=0;j<polygons[i].size();j++)
        {
            ofile<<"NVERTS: "<<polygons[i][j]->size()<<"\n";
            ofile<<"X: "<<"\n";
            for(unsigned int k=0;k<polygons[i][j]->size();k++)
            {
                double x=hmgxform(polygons[i][j]->vertex(k)->x()-offset_i,polygons[i][j]->vertex(k)->y()-offset_j).x();
                if(x<0)
                    x=0;
                if(x>ni)
                    x=ni;
                ofile<<x<<" ";
            }
            ofile<<"\n";

            ofile<<"Y: "<<"\n";
            for(unsigned int k=0;k<polygons[i][j]->size();k++)
            {
                double y=hmgxform(polygons[i][j]->vertex(k)->x()-offset_i,polygons[i][j]->vertex(k)->y()-offset_j).y();
                if(y<0)
                    y=0;
                if(y>nj)
                    y=nj;
                ofile<<y<<" ";
            }
            ofile<<"\n";
        }   
    }


}


void 
write_polygons(vcl_string filenaming,vcl_vector<vcl_vector<vsol_polygon_2d_sptr> > polygons,vcl_ofstream &ofile,
               int ni,int nj,int s0, int se)
{



  vcl_string con_file_rootname="aa";


  vcl_cout<< polygons.size()<<vcl_endl;

  bool first = true;
  for (unsigned int i = 0 ; i < polygons.size() ; i++ )
  {
    vcl_ofstream outfp;



    //create a unique name for this contour based on the root filename
    char filename[200];
    //vcl_sprintf(filename, "%s_%2d.con", con_file_rootname.c_str(), i);
    if (i<10) vcl_sprintf(filename, "0000%1d.con", (int)s0);
    else vcl_sprintf(filename, "000%2d.con", (int)s0);

    s0++;

    vcl_string fn=filenaming+"//"+filename;
    //create the file
    //outfp.open(filename, vcl_ios_out);
    outfp.open(fn.c_str(), vcl_ios_out);

    if (!outfp){
      vcl_cout << " Error writing file  " << filename << vcl_endl;
      return;// false;
    }

    //2) start writing out the contour to the file
    outfp << "CONTOUR" << vcl_endl;

    outfp << "CLOSE" << vcl_endl;


    //ofile<<"NOBJECTS: "<<polygons[i].size()<<"\n";
    if (polygons[i].size()==0) continue;

    outfp<<polygons[i][0]->size()<<"\n";
    //ofile<<"NPOLYS: "<<polygons[i].size()<<"\n";
    for(unsigned j=0;j<polygons[i].size();j++)
    {
      //ofile<<"NVERTS: "<<polygons[i][j]->size()<<"\n";
      //ofile<<"X: "<<"\n";
      for(unsigned k=0;k<polygons[i][j]->size();k++)
      {outfp<<polygons[i][j]->vertex(k)->x()<<" ";
      outfp<<polygons[i][j]->vertex(k)->y()<<" ";}
      outfp<<"\n";


    }
    outfp.close();
  }





  return;// true;






//    vcl_ofstream ofile(filename.c_str());
    ofile<<"NFRAMES: "<<polygons.size()<<"\n";
    for(unsigned i=0;i<polygons.size();i++)
    {
        ofile<<"NOBJECTS: "<<polygons[i].size()<<"\n";
        ofile<<"NPOLYS: "<<polygons[i].size()<<"\n";
        for(unsigned j=0;j<polygons[i].size();j++)
        {
            ofile<<"NVERTS: "<<polygons[i][j]->size()<<"\n";
            ofile<<"X: "<<"\n";
            for(unsigned k=0;k<polygons[i][j]->size();k++)
                ofile<<polygons[i][j]->vertex(k)->x()<<" ";
            ofile<<"\n";
            ofile<<"Y: "<<"\n";
            for(unsigned k=0;k<polygons[i][j]->size();k++)
                ofile<<polygons[i][j]->vertex(k)->y()<<" ";
            ofile<<"\n";

 
        }
    }

    ofile.close();
}

int main(int argc, char** argv)
{
 /*
  //REG_STORAGE(dbbgm_distribution_image_storage);
  REG_STORAGE(vidpro1_vsol2D_storage);
  REG_STORAGE(vidpro1_image_storage);
  //REG_STORAGE(dbbgm_storage);
  REG_PROCESS(vidpro1_load_video_and_con_process);
  // Make each process and add it to the list of program args
  

 // bpro1_process_sptr regimg(new dbvrl_register_images_process()); 
 // regimg->set_input_names(vcl_vector<vcl_string>(1,"video"));
 // regimg->set_output_names(vcl_vector<vcl_string>(1,"RegImg"));
 // vorl1_manager::instance()->add_process_to_args(regimg); 

  //process->set_input_names(vcl_vector<vcl_string>(1,"video"));

  
  vcl_vector<vcl_string> input_names_con_video;
  //input_names_con_video.push_back("Load Video and Con");
  //input_names_con_video.push_back("video");
  vcl_vector<vcl_string> output_names_con_video;
  
  output_names_con_video.push_back("vsol2D");
  output_names_con_video.push_back("image");
  bpro1_process_sptr builder(new vidpro1_load_video_and_con_process());

  vcl_cout<<builder<<vcl_endl;

  //builder->set_parameters();
// builder->parameters()->set_value("-video_filename",video_path);;
//load_video_proc->parameters()->set_value("-video_filename",bpro1_filepath(video_file_()));
  //process_manager_.register_process(builder);
 // builder->set_input_names(input_names_con_video);
 
  //builder->set_input_names(input_names_con_video);
  builder->set_input_names(vcl_vector<vcl_string>(1,"video"));
  builder->set_output_names(output_names_con_video);
  //builder->set_output_names(output_names_con_video);



 
  //vorl1_manager::instance()->load_video();
  vorl1_manager::instance()->add_process_to_args(builder);

  vorl1_manager::instance()->parse_params(argc, argv); 
  vorl1_manager::instance()->add_process_to_queue(builder);
  vorl1_manager::instance()->load_video();
  
  //vorl1_manager::instance()->write_params();
  vidpro1_repository_sptr rep1 = vorl1_manager::instance()->repository();
//vorl1_manager::instance()->add_process_to_queue(builder);
  vorl1_manager::instance()->run_process_queue_on_current_frame();
  vorl1_manager::instance()->finish_process_queue();



//rep1->print_summary();
  

  // vorl1_manager::instance()->load_video();
  //vidpro1_repository_sptr rep = vorl1_manager::instance()->repository();
  vcl_set<vcl_string> rep_video_and_con;
  rep_video_and_con.insert("image");
  rep_video_and_con.insert("vsol2D");
 
  vcl_string output_d = vorl1_manager::instance()->get_output_dir();
  vcl_string binfile_video_and_con=output_d+"/bin.rep";
  vorl1_manager::instance()->save_repository(binfile_video_and_con, rep_video_and_con);


  exit(1);
*/


  /*
REG_STORAGE(vidpro1_image_storage);
  REG_STORAGE(vidpro1_vsol2D_storage);
  

  vcl_vector<vcl_string> output_names_con_video;
  
  output_names_con_video.push_back("image");
output_names_con_video.push_back("vsol2D");
  bpro1_process_sptr cons(new vidpro1_load_video_and_con_process());
  ////cons->set_output_names(vcl_vector<vcl_string>(1,"vsol2D0"));
  //cons->set_output_names(vcl_vector<vcl_string>(1,""));

cons->set_output_names(output_names_con_video);
  vorl1_manager::instance()->add_process_to_args(cons);


 //// bpro1_process_sptr vid(new vidpro1_load_video_process());
  //vid->set_input_names(vcl_vector<vcl_string>(1,"video"));
 /// vcl_vector<vcl_string> vid_out;
  //vid_out.push_back("vsol2D");
  //vid_out.push_back("image");
 ///// vid->set_output_names(vcl_vector<vcl_string>(1,"image"));
  //vid->set_output_names(vid_out);

 //// vorl1_manager::instance()->add_process_to_args(vid);
 // vorl1_manager::instance()->load_video();
  //%--------------------------------------------------------------------


  vorl1_manager::instance()->parse_params(argc, argv); 
 
//%--------------------------------------------------------------------
  ///vorl1_manager::instance()->first_frame();
  //vorl1_manager::instance()->clear_process_queue();


//////////////////////////////////////////////////////////////
  //vorl1_manager::instance()->parse_params(argc, argv); 
  vorl1_manager::instance()->add_process_to_queue(cons); 
 
// vorl1_manager::instance()->add_process_to_queue(cons);
vorl1_manager::instance()->run_process_queue_on_current_frame();
 vorl1_manager::instance()->finish_process_queue();
  ///////////////////////////////////////////////////////////////
 //vorl1_manager::instance()->rewind();
vorl1_manager::instance()->clear_process_queue();
 // vorl1_manager::instance()->load_video();

 //// vorl1_manager::instance()->add_process_to_queue(vid);
//vorl1_manager::instance()->run_process_queue_on_current_frame();
//  vorl1_manager::instance()->finish_process_queue();





 vcl_set<vcl_string> rep_video_and_con;
  
  //rep_video_and_con.insert("vsol2D");
 
 rep_video_and_con.insert("image");
 rep_video_and_con.insert("vsol2D");
 vcl_string output_d = vorl1_manager::instance()->get_output_dir();
 vcl_string binfile_video_and_con=output_d+"/bin.rep";
 vorl1_manager::instance()->save_repository(binfile_video_and_con, rep_video_and_con);





 exit(1);
 */

REG_STORAGE(vidpro1_image_storage);
REG_STORAGE(vidpro1_vsol2D_storage);
REG_STORAGE(vidpro1_vtol_storage);

bpro1_process_sptr cons(new vidpro1_load_con_process());
cons->set_output_names(vcl_vector<vcl_string>(1,"vsol2D"));
vorl1_manager::instance()->add_process_to_args(cons);
//%--------------------------------------------------------------------
bpro1_process_sptr ims(new dbcri_whole_process());

vcl_vector<vcl_string> accu_edge_input_files;
accu_edge_input_files.push_back("vsol2D");
accu_edge_input_files.push_back("video");
accu_edge_input_files.push_back("vtol");
ims->set_input_names(accu_edge_input_files);
vcl_vector<vcl_string> accu_edge_output_files;
//accu_edge_output_files.push_back("vsol");

accu_edge_output_files.push_back("vtol");
//ims->set_output_names(vcl_vector<vcl_string>(1,"image0"));
accu_edge_output_files.push_back("vsol");
accu_edge_output_files.push_back("vsol");
accu_edge_output_files.push_back("vsol");
accu_edge_output_files.push_back("vsol");
accu_edge_output_files.push_back("vsol");
accu_edge_output_files.push_back("vsol");
accu_edge_output_files.push_back("vsol");
accu_edge_output_files.push_back("vsol");
accu_edge_output_files.push_back("vsol");
accu_edge_output_files.push_back("vsol");
accu_edge_output_files.push_back("vsol");
ims->set_output_names(accu_edge_output_files);
vorl1_manager::instance()->add_process_to_args(ims);

//%--------------------------------------------------------------------
vorl1_manager::instance()->parse_params(argc, argv);

///
///
vcl_cout<<vul_file::get_cwd()<<vcl_endl;
vcl_cerr<<vul_file::get_cwd()<<vcl_endl;
///
vorl1_manager::instance()->load_video();
vorl1_manager::instance()->add_process_to_queue(cons); 
vorl1_manager::instance()->run_process_queue_on_current_frame();

vorl1_manager::instance()->finish_process_queue();







//vorl1_manager::instance()->rewind();
//vorl1_manager::instance()->end_frame();
vorl1_manager::instance()->first_frame();
vorl1_manager::instance()->clear_process_queue();
vorl1_manager::instance()->add_process_to_queue(ims);

// run the process queue on the whole video
do {
  float status = vorl1_manager::instance()->status()/2.0 + 0.5;
  vorl1_manager::instance()->write_status(status);
  vorl1_manager::instance()->run_process_queue_on_current_frame();
}  while(vorl1_manager::instance()->next_frame());
vorl1_manager::instance()->finish_process_queue();
vorl1_manager::instance()->write_status(1.0);



vcl_set<vcl_string> rep_video_and_con;
rep_video_and_con.insert("image0");
rep_video_and_con.insert("vsol2D0");
vcl_string output_d = vorl1_manager::instance()->get_output_dir();
vcl_string binfile_video_and_con=output_d+"/bin.rep";
vorl1_manager::instance()->save_repository(binfile_video_and_con, rep_video_and_con);




















exit(1);





vcl_vector<vcl_string> input_names_bg;
input_names_bg.push_back("video");
input_names_bg.push_back("dbbgm");

  bpro1_process_sptr bgbuilder(new dbbgm_aerial_bg_model_process()); //dbbgm_distribution_image
  bgbuilder->set_input_names(input_names_bg);
  //bgbuilder->set_input_names(vcl_vector<vcl_string>(1,"bgmodel"));
  //bgbuilder->set_output_names(vcl_vector<vcl_string>(1,"bgmodel"));
  vorl1_manager::instance()->add_process_to_args(bgbuilder);

  bpro1_process_sptr fgdetector(new dbbgm_aerial_fg_uncertainity_detect_process());

  vcl_vector<vcl_string> inputnames;
  inputnames.push_back("video");
  //inputnames.push_back("bgmodel");
  inputnames.push_back("dbbgm");

  fgdetector->set_input_names(inputnames);
  fgdetector->set_output_names(vcl_vector<vcl_string>(1,"ForegroundDetected"));
  vorl1_manager::instance()->add_process_to_args(fgdetector);

  bpro1_process_sptr  blobfinder(new dbdet_blob_finder_process());
  blobfinder->set_input_names(vcl_vector<vcl_string>(1,"ForegroundDetected"));
  blobfinder->set_output_names(vcl_vector<vcl_string>(1,"blobs"));
  vorl1_manager::instance()->add_process_to_args(blobfinder);

  vorl1_manager::instance()->parse_params(argc, argv); 
  vorl1_manager::instance()->load_video();

  // building background model
  //vorl1_manager::instance()->add_process_to_queue(regimg);
  vorl1_manager::instance()->add_process_to_queue(bgbuilder);

  vcl_cout<<"\n building background model";
  vcl_cout.flush();
 

  ///
//vcl_set<bpro1_storage_sptr> modified; 
  ///

//vorl1_manager::instance()->run_process_queue_on_current_frame();
//vorl1_manager::instance()->finish_process_queue();
//input_names_bg.clear();
//input_names_bg.push_back("video");
 // input_names_bg.push_back("bgmodel1");


  //input_names_bg.push_back(vorl1_manager::instance()->repository()->get_data_by_name("bgmodel"));
  
  //bpro1_process_sptr bgbuilder(new dbbgm_aerial_bg_model_process()); //dbbgm_distribution_image
  bgbuilder->set_input_names(input_names_bg);
  //bgbuilder->set_input_names(vcl_vector<vcl_string>(1,"bgmodel"));
  bgbuilder->set_output_names(vcl_vector<vcl_string>(1,"bgmodel"));
  vorl1_manager::instance()->add_process_to_args(bgbuilder);


  
   vidpro1_repository_sptr rep = vorl1_manager::instance()->repository();
  int s0=0,se=0;
  bool s0_flag=true;
  vorl1_manager::instance()->rewind();
  do {

    if (s0_flag) {
      s0=rep->current_frame();
      s0_flag=false;
    }
    se++;
    vcl_cout<<rep->current_frame()<<vcl_endl;
    
  } while(vorl1_manager::instance()->next_frame());

  se+=s0-1;
  vcl_cout<<se<<vcl_endl;

do {
//   vcl_cout<<"\n 1";
    float status = vorl1_manager::instance()->status()/2.0;
    vorl1_manager::instance()->write_status(status);
 //    vcl_cout<<"\n 2";
    vorl1_manager::instance()->run_process_queue_on_current_frame();

 //    vcl_cout<<"\n 3";
    }  while(
      vorl1_manager::instance()->next_frame()&&
     // vorl1_manager::instance()->next_frame()&&
      vorl1_manager::instance()->next_frame()&&
      vorl1_manager::instance()->next_frame()&&
      vorl1_manager::instance()->next_frame()&&
      vorl1_manager::instance()->next_frame()
      );


 vorl1_manager::instance()->finish_process_queue();
 
  vcl_cout<<"\n detecting polygons";
  vcl_cout.flush();
  // detecting foreground and finding polygons
  vorl1_manager::instance()->rewind();
  vorl1_manager::instance()->clear_process_queue();
  //vorl1_manager::instance()->add_process_to_queue(regimg);
  vorl1_manager::instance()->add_process_to_queue(fgdetector);
  vorl1_manager::instance()->add_process_to_queue(blobfinder);
  ////vidpro1_repository_sptr rep = vorl1_manager::instance()->repository();
  rep->print_summary();

  do {
    float status = vorl1_manager::instance()->status()/2.0 + 0.5;
    vorl1_manager::instance()->write_status(status);

//    vcl_cout<<"\n fg 4";
    vorl1_manager::instance()->run_process_queue_on_current_frame();
//vcl_cout<<"\n fg 5";
  }  while(
   // vorl1_manager::instance()->next_frame()&&
    vorl1_manager::instance()->next_frame());

//vcl_cout<<"\n fg 6";
  //vidpro1_repository_sptr rep = vorl1_manager::instance()->repository();
  //: getting ni , nj  of the images
  vidpro1_image_storage_sptr img_storage;
  bpro1_storage_sptr inputimg= rep->get_data_by_name_at("video",0);

  img_storage.vertical_cast(inputimg);
  int ni=img_storage->get_image()->ni();
  int nj=img_storage->get_image()->nj();
vcl_cout<< img_storage->frame();
  vorl1_manager::instance()->rewind();
  vorl1_manager::instance()->clear_process_queue();
  //:retreiveing the polygons
  vcl_vector<vcl_vector<vsol_polygon_2d_sptr> > all_polygons;
  do {
      
      vidpro1_vsol2D_storage_sptr polygons_storage;
      bpro1_storage_sptr result= rep->get_data_by_name("blobs");
      polygons_storage.vertical_cast(result);
      vcl_vector<vsol_spatial_object_2d_sptr> temp=polygons_storage->all_data();
      vcl_vector<vsol_polygon_2d_sptr>  temp_polygons;
      for(unsigned int i=0;i<temp.size();i++)
        temp_polygons.push_back(temp[i]->cast_to_region()->cast_to_polygon());
      vcl_cout<<temp_polygons.size()<<vcl_endl;
      all_polygons.push_back(temp_polygons);
  } while(
   // vorl1_manager::instance()->next_frame()&&
    vorl1_manager::instance()->next_frame());

  vcl_string fname;
  bpro1_filepath filepath;
  //blobfinder->parameters()->get_value("-polyfile",filepath);
  vcl_string dirname=vorl1_manager::instance()->get_output_dir();
  vcl_string inputfile=dirname+"//poly.txt";

   
  ///regimg->parameters()->get_value("-fhmg",filepath);
  
  vcl_ofstream ofile(inputfile.c_str());
  //ofile<<"FILEID: "<<vorl1_manager::instance()->get_video_fileid()<<"\n";

 // write_transformed_polygons(filepath.path,all_polygons,ofile,ni,nj);
  // write_polygons(filepath.path,all_polygons,ofile,ni,nj);






 /* int s0=0,se=0;
  bool s0_flag=true;
  vorl1_manager::instance()->rewind();
  do {

    if (s0_flag) {
      s0=rep->current_frame();
      s0_flag=false;
    }
    se++;
    vcl_cout<<rep->current_frame()<<vcl_endl;
    
  } while(vorl1_manager::instance()->next_frame());

  se+=s0-1;
  vcl_cout<<se<<vcl_endl;*/
   write_polygons(dirname,all_polygons,ofile,ni,nj,s0,se);
  
  ofile.close();

  // Save part of the repository
  vcl_set<vcl_string> repos_names;
  repos_names.insert("dbbgm");
  repos_names.insert("ForegroundDetected");
  repos_names.insert("blobs");
  //repos_names.insert("blobs");
  vcl_string output_dir = vorl1_manager::instance()->get_output_dir();
  vcl_string binfile=output_dir+"/bin.rep";
  vorl1_manager::instance()->save_repository(binfile, repos_names);

return 0; 
}

