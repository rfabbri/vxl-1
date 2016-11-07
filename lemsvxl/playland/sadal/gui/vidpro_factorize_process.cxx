// This is playland/sadal/vidpro_factorize_process.cxx

//:
// \file

#include "vidpro_factorize_process.h"
#include <vidpro/vidpro_parameters.h>

// include storage classes needed
// such as ...
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage_sptr.h>
#include <vidpro/storage/vidpro_vtol_storage.h>
#include <vidpro/storage/vidpro_vtol_storage_sptr.h>
#include <vidpro/storage/vidpro_vsol3D_storage.h>
#include <vidpro/storage/vidpro_vsol3D_storage_sptr.h>

// other includes needed

#include <blem/playland/sadal/bfac/bfac_factorization.h>
#include <vul/vul_awk.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_point_2d.h>
//#include <vcl_stdlib.h>



//: Constructor
vidpro_factorize_process::vidpro_factorize_process()
{
  // Set up the parameters for this process
  if( !parameters()->add( "No of frames in reconstruction" , "-num_input_frames" , (int)10 )
          ||(
     !parameters()->add( "VRML Filename" , "-vrml_fname" , (vcl_string)"d://surfptsnew.txt" ) )
        )

  {
    vcl_cerr << "ERROR: Adding parameters in vidpro_factorize_process::vidpro_factorize_process()" << vcl_endl;
  }
}


//: Destructor
vidpro_factorize_process::~vidpro_factorize_process()
{
}

vidpro_process* vidpro_factorize_process::clone() const
{
    return new vidpro_factorize_process(*this);
}


//: Return the name of this process
vcl_string
vidpro_factorize_process::name()
{
  return "Reconstruct by Factorization";
}


//: Return the number of input frame for this process
int
vidpro_factorize_process::input_frames()
{
  // input from this frame
  int framenum;
  parameters()->get_value("-num_input_frames",framenum);
  if (framenum<2)
      framenum = 2;
  return framenum;
}


//: Return the number of output frames for this process
int
vidpro_factorize_process::output_frames()
{
int framenum;
parameters()->get_value("-num_input_frames",framenum);
 if (framenum<2)
      framenum = 2;

  return framenum;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > 
vidpro_factorize_process::get_input_type()
{
  // this process looks for an image and vsol2D storage class
  // at each input frame
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > 
vidpro_factorize_process::get_output_type()
{  
  // this process produces a vsol2D storage class
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol3D" );
 // to_return.push_back( "vsol2D" );
  return to_return;
}


//: Execute the process
bool
vidpro_factorize_process::execute()
{
int num_of_frames;
  vcl_string vrml_fname;
  parameters()->get_value("-num_input_frames", num_of_frames);
  parameters()->get_value("-vrml_fname", vrml_fname );
  vcl_vector<vcl_vector<vsol_point_2d_sptr > >  obs_matrix;
  int no_pts;
  if (num_of_frames>2)
      {
  // verify that the number of input frames is correct
  if ( input_data_.size() != num_of_frames ){
    vcl_cout << "In vidpro_factorize_process::execute() - not exactly one"
             << " input frames" << vcl_endl;
    return false;
  }
  clear_output();

  // get images from the storage classes
  vcl_vector<vidpro_vsol2D_storage_sptr> feat_pts(num_of_frames); 
  
  for (int i = 0; i<num_of_frames; i++)
      feat_pts[i].vertical_cast(input_data_[i][0]);
    
  


  
  vcl_vector<vsol_spatial_object_2d_sptr>::const_iterator feat;

  
  vcl_vector<vsol_spatial_object_2d_sptr> feat_sovec;
  vcl_vector<vsol_point_2d_sptr> feat_pointvec;
  

         feat_sovec = (feat_pts[num_of_frames-1])->all_data();
         no_pts = feat_sovec.size();
         feat_sovec.clear();
  for (int i = 0; i<num_of_frames; i++)
  {
  
     feat_sovec = (feat_pts[i])->all_data();  // get all points in ith frame
     assert(feat_sovec.size()>0);
     

     int j = 0;
     vcl_cout<<i<<"Num of points"<<feat_sovec.size()<<"\n";
     for (feat = feat_sovec.begin(); (j<no_pts); feat++)
         {
         feat_pointvec.push_back( feat->ptr()->cast_to_point()  );
         j++;
         }
     obs_matrix.push_back(feat_pointvec);
     feat_pointvec.clear();
     feat_sovec.clear();
  }
      }
  else
      {
      vcl_ifstream ext_input(vrml_fname.c_str());
      vul_awk readerx(ext_input);
      vul_awk readery(ext_input);
      ++readery;
      double xread;
      double yread;
      int endx, endy;
      vcl_vector <vsol_point_2d_sptr> pts_in_ith_fr;
      int xptr,yptr;
      do
       {
          xptr = 0;
          yptr = 0;
          endx = readerx.NF();
          endy = readery.NF();
          if (no_pts>0)
          {
               assert(no_pts==endx);
               assert(no_pts==endy);
          }
          no_pts = endx;
     

          while ((xptr<endx)&&(yptr<endy))
          {

            xread = atof(readerx[xptr]);
            yread = atof(readery[yptr]);
            xptr++;
            yptr++;
           
            pts_in_ith_fr.push_back(new vsol_point_2d(xread,yread) );
           }
          
          ++readerx;
          ++readerx;
          ++readery;
          ++readery;
          obs_matrix.push_back(pts_in_ith_fr);
          pts_in_ith_fr.clear();
          }
          while(readerx&&readery);
      }
  




  //=========================================
  // Do something here with all of this data
  //=========================================

 bfac_reconstructor *Reconstructor = new bfac_reconstructor(obs_matrix, vrml_fname+".vrml"  );
  int * feat_indices = Reconstructor->get_indices();
  vcl_vector <vsol_point_3d_sptr> output_3d= Reconstructor->get_reconst();
  assert(output_3d.size() > 0);
  vcl_vector<int> block_indices = Reconstructor->get_blocks();

  int count = 0;
   for (int m = 0; m<no_pts; m++)
       {vcl_cout<<m<<"   "<<feat_indices[m];
        if (block_indices[count]==m)
            {
            count++;
            vcl_cout<<"Block end";
            }
        
       }

   vcl_cout<<"End\n";
    // create the output storage class
  vcl_vector<vidpro_vsol2D_storage_sptr> output_vsol(num_of_frames);
  for ( int i = 0; i<num_of_frames; i++)
   output_vsol[i] = vidpro_vsol2D_storage_new();

  vidpro_vsol3D_storage_sptr output_vsol3d = vidpro_vsol3D_storage_new();


  for (int j = 0; j<num_of_frames; j++)
  {
      vcl_vector <vsol_point_2d_sptr> temp = obs_matrix[j];
          for (int k = 0;k<output_3d.size(); k++ )
          output_vsol3d->add_object((vsol_spatial_object_3d *)(output_3d[k].ptr()),"3D point" );
      assert(temp.size()==no_pts);
          char str[3];
      //int group
      for (int m = 0; m<block_indices.size();m++)
      {
            for (int i = 0; i<block_indices[m]; i++)
            {
            output_vsol[j]->add_object(temp[feat_indices[i]].ptr(),"KL feature  point - group"+vcl_string(itoa(m,str,10)));
            }
#if 0
      for (int i = cutoff; i<no_pts; i++)
      {  
    
        output_vsol[j]->add_object(temp[feat_indices[i]].ptr(),"KL feature  point-group 2");
      }

#endif
      }
   

  }

  

  for ( int i = 0; i<num_of_frames; i++)
  {
          output_data_[i].push_back(output_vsol3d);
//        output_data_[i].push_back(output_vsol[i]);
  }
  ///Prepare output
 
  return true;
}


//: Finish
bool
vidpro_factorize_process::finish()
{
  // I'm not really sure what this is for, ask Amir
  // - mleotta
  return true;
}


