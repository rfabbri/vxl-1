// This is breye1/vorl1/exec/vorl1_compute_homography_exec.cxx
//:
// \file

#include <vorl1/vorl1.h>
#include <dbbgm/pro/dbbgm_aerial_bg_model_process.h>
#include <dbbgm/pro/dbbgm_aerial_fg_uncertainity_detect_process.h>
#include <dbvrl/pro/dbvrl_register_images_process.h>
#include <dbdet/pro/dbdet_blob_finder_process.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <bpro1/bpro1_parameters_sptr.h>
#include <bpro1/bpro1_process_sptr.h>
#include <dbbgm/pro/dbbgm_storage.h>
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
int main(int argc, char** argv)
{
  REG_STORAGE(vidpro1_image_storage);
  REG_STORAGE(dbbgm_storage);
  REG_STORAGE(vidpro1_vsol2D_storage);
  
  // Make each process and add it to the list of program args


  bpro1_process_sptr regimg(new dbvrl_register_images_process()); 
  regimg->set_input_names(vcl_vector<vcl_string>(1,"video"));
  regimg->set_output_names(vcl_vector<vcl_string>(1,"RegImg"));
  vorl1_manager::instance()->add_process_to_args(regimg); 

  bpro1_process_sptr bgbuilder(new dbbgm_aerial_bg_model_process()); 
  bgbuilder->set_input_names(vcl_vector<vcl_string>(1,"RegImg"));
  bgbuilder->set_output_names(vcl_vector<vcl_string>(1,"bgmodel"));
  vorl1_manager::instance()->add_process_to_args(bgbuilder);

  bpro1_process_sptr fgdetector(new dbbgm_aerial_fg_uncertainity_detect_process());

  vcl_vector<vcl_string> inputnames;
  inputnames.push_back("RegImg");
  inputnames.push_back("bgmodel");

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
  vorl1_manager::instance()->add_process_to_queue(regimg);
  vorl1_manager::instance()->add_process_to_queue(bgbuilder);

  vcl_cout<<"\n building background model";
  vcl_cout.flush();

 do {
    float status = vorl1_manager::instance()->status()/2.0;
    vorl1_manager::instance()->write_status(status);
    vorl1_manager::instance()->run_process_queue_on_current_frame();
    }  while(vorl1_manager::instance()->next_frame());
 vorl1_manager::instance()->finish_process_queue();
 
  vcl_cout<<"\n detecting polygons";
  vcl_cout.flush();
  // detecting foreground and finding polygons
  vorl1_manager::instance()->rewind();
  vorl1_manager::instance()->clear_process_queue();
  //vorl1_manager::instance()->add_process_to_queue(regimg);
  vorl1_manager::instance()->add_process_to_queue(fgdetector);
  vorl1_manager::instance()->add_process_to_queue(blobfinder);
  vidpro1_repository_sptr rep = vorl1_manager::instance()->repository();
  rep->print_summary();

  do {
    float status = vorl1_manager::instance()->status()/2.0 + 0.5;
    vorl1_manager::instance()->write_status(status);
    vorl1_manager::instance()->run_process_queue_on_current_frame();

  }  while(vorl1_manager::instance()->next_frame());


  //vidpro1_repository_sptr rep = vorl1_manager::instance()->repository();
  //: getting ni , nj  of the images
  vidpro1_image_storage_sptr img_storage;
  bpro1_storage_sptr inputimg= rep->get_data_by_name_at("video",0);

  img_storage.vertical_cast(inputimg);
  int ni=img_storage->get_image()->ni();
  int nj=img_storage->get_image()->nj();

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
      all_polygons.push_back(temp_polygons);
  } while(vorl1_manager::instance()->next_frame());

  vcl_string fname;
  bpro1_filepath filepath;
  //blobfinder->parameters()->get_value("-polyfile",filepath);*/
  vcl_string dirname=vorl1_manager::instance()->get_output_dir();
  vcl_string inputfile=dirname+"//poly.txt";

   
  regimg->parameters()->get_value("-fhmg",filepath);
  
  vcl_ofstream ofile(inputfile.c_str());
  ofile<<"FILEID: "<<vorl1_manager::instance()->get_video_fileid()<<"\n";
  write_transformed_polygons(filepath.path,all_polygons,ofile,ni,nj);
  
  ofile.close();


return 0; 
}


