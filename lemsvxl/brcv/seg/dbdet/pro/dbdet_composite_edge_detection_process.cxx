// This is brcv/seg/dbdet/pro/dbdet_composite_edge_detection_process.cxx

//:
// \file

#include "dbdet_composite_edge_detection_process.h"


#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>

#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/algo/dbdet_third_order_edge_det.h>

//: Constructor
dbdet_composite_edge_detection_process::dbdet_composite_edge_detection_process() : num_frames_(0)
{
  vcl_vector<vcl_string> gradient_operator_choices;
  gradient_operator_choices.push_back("Gaussian");       //0
  gradient_operator_choices.push_back("h0-operator");    //1
  gradient_operator_choices.push_back("h1-operator");    //2
  vcl_vector<vcl_string> parabola_fit_type;
  parabola_fit_type.push_back("3-point fit");      //0
  parabola_fit_type.push_back("9-point fit");      //1

  vcl_vector<vcl_string> edge_output_type;
  edge_output_type.push_back("Points");            //0
  edge_output_type.push_back("Line segments");     //1
  //edge_output_type.push_back("Other");           //2

  if( !parameters()->add( "Gradient Operator"   , "-grad_op" , gradient_operator_choices, 0) ||
      !parameters()->add( "Sigma (Gaussian)"    , "-sigma"   , 1.0 ) ||
      !parameters()->add( "Gradient Magnitude Threshold"   , "-thresh" , 5.0 ) ||
      !parameters()->add( "Interpolation factor [2^N], N= "  , "-int_factor" , 1 ) ||
      !parameters()->add( "Reduce edgel tokens "  , "-breduce" , false ) ||
      !parameters()->add( "Parabola Fit type"   , "-parabola_fit" , parabola_fit_type, 0) ||
      !parameters()->add( "Output type"   , "-out_type" , edge_output_type, 1) ||
      !parameters()->add( "Combine edges from R, G and B channels?", "-combine", false) ||
      !parameters()->add( "Take average of edgels on the same pixel?", "-average", false) ||
      !parameters()->add( "Split the edgels onto 3 frames?", "-split", false) ) 
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbdet_composite_edge_detection_process::~dbdet_composite_edge_detection_process()
{
}


//: Clone the process
bpro1_process*
dbdet_composite_edge_detection_process::clone() const
{
  return new dbdet_composite_edge_detection_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_composite_edge_detection_process::name()
{
  return "RGB Composite Edge Detector";
}


//: Return the number of input frame for this process
int
dbdet_composite_edge_detection_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_composite_edge_detection_process::output_frames()
{
  return num_frames_;
}

//: Call the parent function and reset num_frames_
void
dbdet_composite_edge_detection_process::clear_output(int resize)
{
  bpro1_process::clear_output(resize);  
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_composite_edge_detection_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_composite_edge_detection_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  //to_return.push_back( "vsol2D" );
  to_return.push_back( "edge_map" );
  return to_return;
}


//: Execute the process
bool
dbdet_composite_edge_detection_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbdet_composite_edge_detection_process::execute() - not exactly one"
             << " input images \n";
    return false;
  }
  clear_output();

  vcl_cout << "Third_order edge detection...";
  vcl_cout.flush();

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);
  vil_image_resource_sptr image_sptr = frame_image->get_image();
  vil_image_view<vxl_byte> image_view = image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj() );

  //get the parameters
  unsigned grad_op, parabola_fit, out_type;
  double sigma, thresh;
  int N;
  bool reduce_tokens;
  parameters()->get_value( "-grad_op", grad_op);
  parameters()->get_value( "-int_factor" , N );
  parameters()->get_value( "-breduce" , reduce_tokens );
  parameters()->get_value( "-sigma", sigma);
  parameters()->get_value( "-thresh", thresh);
  parameters()->get_value( "-parabola_fit", parabola_fit );
  parameters()->get_value( "-out_type", out_type );
  
  bool combine_channels;
  parameters()->get_value( "-combine", combine_channels);
  bool average_channels;
  parameters()->get_value( "-average", average_channels);
  bool split_channels;
  parameters()->get_value( "-split", split_channels);

  dbdet_edgemap_sptr edge_map;
  if (!combine_channels) {
    // perfrom third-order edge detection with these parameters
    edge_map = dbdet_detect_third_order_edges(image_view, sigma, thresh, N, parabola_fit, grad_op, reduce_tokens);

    // create the output storage class
    dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
    output_edgemap->set_edgemap(edge_map);
    if (output_data_.size() == 0) 
      output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,output_edgemap));
    else
      output_data_[0].push_back(output_edgemap);

  } else if (combine_channels && image_view.nplanes() == 3) {
    vcl_cout << "combining edgels from R, G and B channels!\n";
    vil_image_resource_sptr Rimg = vil_plane(image_sptr, 0);
    vil_image_resource_sptr Gimg = vil_plane(image_sptr, 1);
    vil_image_resource_sptr Bimg = vil_plane(image_sptr, 2);
    vil_image_view<vxl_byte> image_viewR = Rimg->get_view(0, Rimg->ni(), 0, Rimg->nj() );
    vil_image_view<vxl_byte> image_viewG = Gimg->get_view(0, Gimg->ni(), 0, Gimg->nj() );
    vil_image_view<vxl_byte> image_viewB = Bimg->get_view(0, Bimg->ni(), 0, Bimg->nj() );
    dbdet_edgemap_sptr edge_mapR = dbdet_detect_third_order_edges(image_viewR, sigma, thresh, N, parabola_fit, grad_op, reduce_tokens);
    dbdet_edgemap_sptr edge_mapG = dbdet_detect_third_order_edges(image_viewG, sigma, thresh, N, parabola_fit, grad_op, reduce_tokens);
    dbdet_edgemap_sptr edge_mapB = dbdet_detect_third_order_edges(image_viewB, sigma, thresh, N, parabola_fit, grad_op, reduce_tokens);

    if (edge_mapR->width() != edge_mapG->width() || edge_mapR->width() != edge_mapB->width() ||
        edge_mapR->height() != edge_mapG->height() || edge_mapR->height() != edge_mapB->height()) {
        vcl_cout << "Inconsistencies in edge map sizes of color channels!\n";
        return false;
    }

    edge_map = new dbdet_edgemap(edge_mapR->width(), edge_mapR->height());
    dbdet_edgemap_const_iter it = edge_mapR->edge_cells.begin();
    for (; it!=edge_mapR->edge_cells.end(); it++)
      for (unsigned j=0; j<(*it).size(); j++)
        edge_map->insert(new dbdet_edgel(*(*it)[j]));

    it = edge_mapG->edge_cells.begin();
    for (; it!=edge_mapG->edge_cells.end(); it++)
      for (unsigned j=0; j<(*it).size(); j++)
        edge_map->insert(new dbdet_edgel(*(*it)[j]));

    it = edge_mapB->edge_cells.begin();
    for (; it!=edge_mapB->edge_cells.end(); it++)
      for (unsigned j=0; j<(*it).size(); j++)
        edge_map->insert(new dbdet_edgel(*(*it)[j]));

    if (average_channels) {
      // quick hack by ozge, just take the average of all edgels falling on the same pixel
      dbdet_edgemap_iter itr = edge_map->edge_cells.begin();
      for (; itr != edge_map->edge_cells.end(); itr++) {
        if ((*itr).size() == 0) continue;
        dbdet_edgel *e = (*itr)[0];
        vgl_point_2d<double> pt = e->pt;
        double tangent = e->tangent;
        double strength = e->strength;
        for (unsigned j = 1; j < (*itr).size(); j++) {
          pt = midpoint(pt, (*itr)[j]->pt);
          tangent = (tangent + (*itr)[j]->tangent);
          strength = (strength + (*itr)[j]->strength);
        }
        tangent /= (*itr).size();
        strength /= (*itr).size();
        dbdet_edgel *ee = new dbdet_edgel(pt, tangent, strength, e->id);
        itr->clear();
        itr->push_back(ee);
      }
      dbdet_edgemap_sptr edge_temp = edge_map;
      edge_map = new dbdet_edgemap(edge_temp->width(), edge_temp->height());
      it = edge_temp->edge_cells.begin();
      for (; it!=edge_temp->edge_cells.end(); it++)
        for (unsigned j=0; j<(*it).size(); j++)
          edge_map->insert(new dbdet_edgel(*(*it)[j]));
    }
    if (!split_channels) {
      // create the output storage class
      dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
      output_edgemap->set_edgemap(edge_map);
      output_data_[0].push_back(output_edgemap);
      num_frames_ = 1;
    } else {
      // create the output storage class
      dbdet_edgemap_storage_sptr output_edgemapR = dbdet_edgemap_storage_new();
      output_edgemapR->set_edgemap(edge_mapR);
      dbdet_edgemap_storage_sptr output_edgemapG = dbdet_edgemap_storage_new();
      output_edgemapG->set_edgemap(edge_mapG);
      dbdet_edgemap_storage_sptr output_edgemapB = dbdet_edgemap_storage_new();
      output_edgemapB->set_edgemap(edge_mapB);
      
      //  output_data_[0].push_back(output_edgemapR);
      //  output_data_[1].push_back(output_edgemapG);
      //  output_data_[2].push_back(output_edgemapB);
        num_frames_ = 3;

      if (output_data_.size() == 0) {
        output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,output_edgemapR));
        output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,output_edgemapG));
        output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,output_edgemapB));
      } else if (output_data_.size() == 1) {
        output_data_[0].push_back(output_edgemapR);
        output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,output_edgemapG));
        output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,output_edgemapB));
      } else if (output_data_.size() == 2) {
        output_data_[0].push_back(output_edgemapR);
        output_data_[1].push_back(output_edgemapG);
        output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,output_edgemapB));
      } else if (output_data_.size() > 2) {
        output_data_[0].push_back(output_edgemapR);
        output_data_[1].push_back(output_edgemapG);
        output_data_[2].push_back(output_edgemapB);
      }
      
    }

  } else {
    vcl_cout << "the image does not have 3 channels, cannot combine edgels\n";
    return false;
  }

  vcl_cout << "done!" << vcl_endl;
  vcl_cout.flush();
/*
  // create the output storage class
  dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
  output_edgemap->set_edgemap(edge_map);
  output_data_[0].push_back(output_edgemap);
*/
  ////output the edgels
  //vcl_vector< vsol_spatial_object_2d_sptr > edgels;
  //for (unsigned i=0; i<edge_locations.size(); i++){
  //  if (out_type==1)//lines
  //  { 
  //    double edge_len = 0.5/scale;
  //    vsol_point_2d_sptr line_start = new vsol_point_2d(edge_locations[i].x() - edge_len*vcl_cos(edge_orientations[i]), 
  //                                                      edge_locations[i].y() - edge_len*vcl_sin(edge_orientations[i]));
  //    vsol_point_2d_sptr line_end = new vsol_point_2d(edge_locations[i].x() + edge_len*vcl_cos(edge_orientations[i]), 
  //                                                    edge_locations[i].y() + edge_len*vcl_sin(edge_orientations[i]));
  //    vsol_line_2d_sptr new_line = new vsol_line_2d(line_start, line_end);
  //    edgels.push_back(new_line->cast_to_spatial_object());  
  //  }
  //  else { //points
  //    vsol_point_2d_sptr new_p = new vsol_point_2d(edge_locations[i].x(), edge_locations[i].y());
  //    edgels.push_back(new_p->cast_to_spatial_object());
  //  }
  //}

  //// create the output storage class
  //vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  //output_vsol->add_objects(edgels, "edgels");
  //output_data_[0].push_back(output_vsol);

  return true;
}

bool
dbdet_composite_edge_detection_process::finish()
{
  return true;
}

