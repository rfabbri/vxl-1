// This is contrib/sadali/gui/vidpro1_edgemap_form_process.cxx
#define MIN(a,b)  ((a<b) ? a : b)
//:
// \file

#include "vidpro1_edgemap_form_process.h"
#include <bpro1/bpro1_parameters.h>

// include storage classes needed
// such as ...

#include <vidpro1/storage/vidpro1_vtol_storage.h>
#include <vidpro1/storage/vidpro1_vtol_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h> 
#include <vidpro1/storage/vidpro1_image_storage.h>

// other includes needed

#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_topology_object_sptr.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_resource_sptr.h>

#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>







//: Constructor
vidpro1_edgemap_form_process::vidpro1_edgemap_form_process()
{
  // Set up the parameters for this process

  if 
   (0)
          
          
  {
    vcl_cerr << "ERROR: Adding parameters in vidpro1_edgemap_form_process::vidpro1_edgemap_form_process()" << vcl_endl;
  }

}



//: Destructor
vidpro1_edgemap_form_process::~vidpro1_edgemap_form_process()
{
}


//: Return the name of this process
vcl_string
vidpro1_edgemap_form_process::name()
{
  return "Edge2Img";
}


bpro1_process *
vidpro1_edgemap_form_process::clone() const
{
    return new vidpro1_edgemap_form_process(*this);

};
//: Return the number of input frame for this process
int
vidpro1_edgemap_form_process::input_frames()
{
  // input from this frame 
        return 1;
}


//: Return the number of output frames for this process
int
vidpro1_edgemap_form_process::output_frames()
{
  // output to this frame only
return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > 
vidpro1_edgemap_form_process::get_input_type()
{
  // this process looks for  vsol2D storage classes
  // at each input frame
  
  
  vcl_vector< vcl_string > to_return;
  
  to_return.push_back( "vtol" );
  to_return.push_back( "vtol" );
  
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > 
vidpro1_edgemap_form_process::get_output_type()
{  
  // this process produces a vsol2D storage class
        
  vcl_vector<vcl_string > to_return;
 
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  
  return to_return;
}


//: Execute the process
bool
vidpro1_edgemap_form_process::execute()
{
  // verify that the number of input frames is correct
  if ( input_data_.size() != 1 ){
    vcl_cout << "In vidpro1_edgemap_form_process::execute() - not exactly two"
             << " input frames" << vcl_endl;
    return false;
  }
  clear_output();
 

  int    param1;
   parameters()->get_value( "-p1" , param1 );
  // get vsol2d from the storage classes
  vidpro1_vtol_storage_sptr left_img_edge; 
  left_img_edge.vertical_cast(input_data_[0][0]);
  vidpro1_vtol_storage_sptr right_img_edge; 
  right_img_edge.vertical_cast(input_data_[0][1]);
 
  vcl_vector < vsol_spatial_object_2d_sptr > origpts;
  int  maxx,maxy;
  vcl_set<vtol_topology_object_sptr>::const_iterator u=left_img_edge->begin();
  vcl_set<vtol_topology_object_sptr>::const_iterator n=right_img_edge->begin();

  maxx = 0;
  maxy = 0;
  int x0,y0,x1,y1;

  for ( ;u!=left_img_edge->end(); u++)
  {
      

      vtol_edge_2d_sptr edge = (*u)->cast_to_edge()->cast_to_edge_2d();
      vsol_curve_2d * curve =edge->curve().ptr();
      
      vdgl_digital_curve_sptr dc= curve->cast_to_vdgl_digital_curve();
      
      vdgl_interpolator_sptr itrp = dc->get_interpolator();
      vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

      unsigned int chain_size = ech->size();

 
      
      for (unsigned int i=0; i<chain_size;i++)
      {
          
          vdgl_edgel ed = (*ech)[i];
          x1 = ed.get_x(); 
          y1 = ed.get_y();
          
          if (maxx<x1)
              maxx = x1;

          if (maxy<y1)
              maxy = y1;
      }
  }

  vil_image_view<float> leftedgemap(maxx+1,maxy+1);
  leftedgemap.fill(0.0);


  maxx = 0;
  maxy = 0;
   for ( ;n!=right_img_edge->end(); n++)
  {
      

      vtol_edge_2d_sptr edge = (*n)->cast_to_edge()->cast_to_edge_2d();
      vsol_curve_2d * curve =edge->curve().ptr();
      
      vdgl_digital_curve_sptr dc= curve->cast_to_vdgl_digital_curve();
      
      vdgl_interpolator_sptr itrp = dc->get_interpolator();
      vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

      unsigned int chain_size = ech->size();

 
      
      for (unsigned int i=0; i<chain_size;i++)
      {
          
          vdgl_edgel ed = (*ech)[i];

          x1 = ed.get_x(); 
          y1 = ed.get_y();

          if (maxx<x1)
              maxx = x1;
          
          if (maxy<y1)
              maxy = y1;
      }
   }
  
   
   vil_image_view<float> rightedgemap(maxx+1,maxy+1);
   
   rightedgemap.fill(0.0);

   u=left_img_edge->begin();
   n=right_img_edge->begin();
   vdgl_edgel mem;
   for ( ;u!=left_img_edge->end(); u++)
  {
       vtol_edge_2d_sptr edge = (*u)->cast_to_edge()->cast_to_edge_2d();
       vsol_curve_2d * curve =edge->curve().ptr();
       vdgl_digital_curve_sptr dc= curve->cast_to_vdgl_digital_curve();
       
       vdgl_interpolator_sptr itrp = dc->get_interpolator();
       vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();
       

       unsigned int chain_size = ech->size();
       
      
      for (unsigned int i=1; i<chain_size;i++)
      {
          vdgl_edgel pred;
    
          
           pred= (*ech)[i-1];
          vdgl_edgel ed = (*ech)[i];
         

          x0 = pred.get_x();
          y0 = pred.get_y();
          x1 = ed.get_x(); 
          y1 = ed.get_y();


          float x, y;
          float dy = y1 - y0;
          float dx = x1 - x0;
  
          float m; 
          if (dx!=0.00)
              m= dy / dx;
          float value = 1.0;
          if (dx == 0)
          {
              if (y0<y1)
          for (y = y0; y < y1; y++) 
          {
              leftedgemap(x0,y)= value;
              
          }
          else
              for (y = y1; y < y0; y++) 
              {
                 leftedgemap(x0,y)= value;
            
              }
         }
          else
              if (fabs(m)<1.0)
                  {
                  y = y0;
                  if (x0<x1)
                      for (x = x0; x < x1; x++) 
                      {
                          leftedgemap((int)x,vcl_floor(y))= value;
                          y = y + m;
                      }
                  else
                      {
                        y=y1;
                        for (x = x1; x < x0; x++) 

                            {
                          leftedgemap((int)x,vcl_floor(y))= value;
                          y = y + m;
                            }
                      }
                  }
               else
                   {
                   x= x0;
                    m= 1.0/m;

                    if (y0<y1)
                         for (y = y0; y < y1; y++) 
                          {
                             leftedgemap(vcl_floor(x),(int)y)= value;
                              x = x + m;
                          }
                         else
                             {
                             x = x1;
                             for (y = y1; y < y0; y++) 
                                 {
                                  leftedgemap(vcl_floor(x),(int)y)= value;
                                  x = x + m;
                                  }
                             }
                     }
  }
   }
      
   for ( ;n!=right_img_edge->end(); n++)
  {
       vtol_edge_2d_sptr edge = (*n)->cast_to_edge()->cast_to_edge_2d();
       vsol_curve_2d * curve =edge->curve().ptr();
       vdgl_digital_curve_sptr dc= curve->cast_to_vdgl_digital_curve();
       
       vdgl_interpolator_sptr itrp = dc->get_interpolator();
       vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();
      

       unsigned int chain_size = ech->size();
       
      
      for (unsigned int i=1; i<chain_size;i++)
      {
           vdgl_edgel pred;

          
          pred = (*ech)[i-1];
          vdgl_edgel ed = (*ech)[i];
        

          x0 = pred.get_x();
          y0 = pred.get_y();
          x1 = ed.get_x(); 
          y1 = ed.get_y();

  
  float x, y;
  float dy = y1 - y0;
  float dx = x1 - x0;
 
  float m; 
  if (dx!=0.00)
      m= dy / dx;
  float value = 1.0;
  if (dx == 0)
  {
       if (y0<y1)
          for (y = y0; y < y1; y++) 
          {
              rightedgemap(x0,(int)y)= value;
              
          }
          else
              for (y = y1; y < y0; y++) 
              {
                 rightedgemap(x0,(int)y)= value;
            
              }
  }
  else 
  if (fabs(m)<1.0)
  {
      y = y0;
      if (x0<x1)
          for (x = x0; x < x1; x++) 
          {
             rightedgemap(x,vcl_floor(y))= value;
              y = y + m;
          }
          else
          {  y = y1;
              for (x = x1; x < x0; x++) 
              {
                  rightedgemap(x,vcl_floor(y))= value;
                  y = y + m;
              }
          }
    }
  else
  { x = x0;
    m = 1.0/m;
        if (y0<y1)
        {
            for (y = y0; y < y1; y++) 
            {
              rightedgemap(vcl_floor(x),y)= value;
              x = x + m;
            }
        }
          else
          {
              x= x1;
              for (y = y1; y < y0; y++) 
              {
                  rightedgemap(vcl_floor(x),y)= value;
                  x = x + m;
              }
          }

  }
   }
   }
  // create the output storage class
  vidpro1_image_storage_sptr output_img1 = vidpro1_image_storage_new();
  vidpro1_image_storage_sptr output_img2 = vidpro1_image_storage_new();
  vidpro1_image_storage_sptr output_vis1 = vidpro1_image_storage_new();
  vidpro1_image_storage_sptr output_vis2 = vidpro1_image_storage_new();

  vil_image_view<vxl_byte> greyvalimg1(leftedgemap.ni(),leftedgemap.nj());
  vil_image_view<vxl_byte> greyvalimg2(rightedgemap.ni(),rightedgemap.nj());
  
  vil_image_resource_sptr output_rsrc_img1 =  vil_new_image_resource_of_view(leftedgemap);
  vil_image_resource_sptr output_rsrc_img2 = vil_new_image_resource_of_view(rightedgemap);
  vil_image_resource_sptr output_rsrc_vis1 =  vil_new_image_resource_of_view(greyvalimg1);
  vil_image_resource_sptr output_rsrc_vis2 = vil_new_image_resource_of_view(greyvalimg2);


   for (int i = 0; i<leftedgemap.ni();i++)
      for (int j = 0; j<leftedgemap.nj();j++)
  {
      greyvalimg1(i,j) = vxl_byte(MIN(255,vcl_floor(255.0*leftedgemap(i,j))));
      
  }
      for (int i = 0; i<rightedgemap.ni();i++)
      for (int j = 0; j<rightedgemap.nj();j++)
  {
      greyvalimg2(i,j) = vxl_byte(MIN(255,vcl_floor(255.0*rightedgemap(i,j))));
      
  }


  output_img1->set_image(output_rsrc_img1);
  output_img2->set_image(output_rsrc_img2);
  output_vis1->set_image(output_rsrc_vis1);
  output_vis2->set_image(output_rsrc_vis2);
  output_data_[0].push_back(output_img1);
  output_data_[0].push_back(output_img2);
  output_data_[0].push_back(output_vis1);
  output_data_[0].push_back(output_vis2);
  
   
  
  return true;
}


//: Finish
bool
vidpro1_edgemap_form_process::finish()
{
  // I'm not really sure what this is for, ask Amir
  // - mleotta
  return true;
}
