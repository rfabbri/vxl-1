// This is brcv/seg/dbdet/pro/dbdet_subsample_edges.cxx

//:
// \file

#include "dbdet_subsample_edges.h"

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <dbdet/algo/dbdet_generic_linker.h>
#include <dbdet/algo/dbdet_postprocess_contours.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vul/vul_timer.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>

#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/algo/vgl_fit_lines_2d.h>

//: Constructor
dbdet_subsample_edges::dbdet_subsample_edges()
{
  //subsampling algorithms
  vcl_vector<vcl_string> subsampling_algo_choices;
  subsampling_algo_choices.push_back("Choose every Nth edge");                   //0
  subsampling_algo_choices.push_back("Fit lines to Contours");                   //1

  if ( 
      //parameters
      !parameters()->add( "SCF formation Algorithm: "   , "-subsampling_algo" , subsampling_algo_choices, 1) ||
      
      !parameters()->add( "SCF length" , "-SCF_len" , (unsigned) 4 ) ||
      !parameters()->add( "Line Fit Tolerance" , "-linefit_rms" , (double) 0.05 ) ||

      !parameters()->add( "Do not include short fragments" , "-bremove_short_frags" , false ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbdet_subsample_edges::~dbdet_subsample_edges()
{
}


//: Clone the process
bpro1_process*
dbdet_subsample_edges::clone() const
{
  return new dbdet_subsample_edges(*this);
}


//: Return the name of this process
vcl_string
dbdet_subsample_edges::name()
{
  return "SubSample Edges";
}


//: Return the number of input frame for this process
int
dbdet_subsample_edges::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_subsample_edges::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_subsample_edges::get_input_type()
{
  vcl_vector< vcl_string > to_return;
    to_return.push_back( "sel" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_subsample_edges::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "edge_map" );

  return to_return;
}


//: Execute the process
bool dbdet_subsample_edges::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbdet_subsample_edges::execute() - not exactly one input \n";
    return false;
  }
  clear_output();

  //get the parameters
  get_parameters();

  //get the input storage class
  dbdet_sel_storage_sptr input_sel;
  input_sel.vertical_cast(input_data_[0][0]);

  dbdet_edgemap_sptr EM = input_sel->EM();
  dbdet_curve_fragment_graph &CFG = input_sel->CFG();

  //create a new edgemap to hold the subsampled edges
  dbdet_edgemap_sptr EM_new = new dbdet_edgemap(EM->width(), EM->height());
  
  //---------------------------------------------------------------------------------
  //Sub sample edges from the original edgemap using the linked contours from the CFG

  if (subsampling_algo==0)
  {
    // Criteria for subsampling:
    //  (a) pick 1/(SCF_len+1) edges from each contour
    //  (b) if the length of the contour fragment < SCF_len, pick the central edgel

    dbdet_edgel_chain_list_iter f_it = CFG.frags.begin();
    for (; f_it != CFG.frags.end(); f_it++)
    {
      dbdet_edgel_chain* chain = (*f_it);
      
      if (chain->edgels.size() < SCF_len){
        if (bremove_short_frags) continue;

        unsigned mid_ind = chain->edgels.size()/2;

        EM_new->insert(new dbdet_edgel(*chain->edgels[mid_ind]));
        continue;
      }

      //for the rest subsample the chain
      for (unsigned j=SCF_len/2; j<chain->edgels.size()-SCF_len/2; j+=(SCF_len+1)){
        EM_new->insert(new dbdet_edgel(*chain->edgels[j]));
      }
    }
  }
  else if (subsampling_algo==1) //fit lines first
  {
    dbdet_edgel_chain_list_iter f_it = CFG.frags.begin();
    for (; f_it != CFG.frags.end(); f_it++)
    {
      dbdet_edgel_chain* chain = (*f_it);

      if (chain->edgels.size()<SCF_len) //weed out short contours
        continue;

      vgl_fit_lines_2d<double> fitter; 
      fitter.set_min_fit_length(SCF_len);
      fitter.set_rms_error_tol(linefit_rms);

      for (unsigned j=0; j<chain->edgels.size(); j++)
        fitter.add_point(chain->edgels[j]->pt);
      
      fitter.fit();
      vcl_vector<vgl_line_segment_2d<double> >& segs = fitter.get_line_segs();

      //convert the line segments into edgels
      for (unsigned int i=0; i<segs.size(); i++){

        double len = vgl_distance(segs[i].point1(),segs[i].point2()); 
        //vcl_cout << len << vcl_endl;

        if (len<SCF_len/2)
          continue;

        if (len>(2*SCF_len-1)){ //put multiple edgels
          double tangent = dbdet_angle0To2Pi(segs[i].slope_radians());
          double N = vcl_floor((len+1.0)/SCF_len);
          for (double j=0; j<N; j++){
            //determine the center of the segment first
            vgl_point_2d<double> pt = segs[i].point1() + (2*j+1)/(2*N)*(segs[i].point2()-segs[i].point1());
            EM_new->insert(new dbdet_edgel(pt, tangent));
          }
        }
        else { //put a single edgel
          double tangent = dbdet_angle0To2Pi(segs[i].slope_radians());
          vgl_point_2d<double> pt((segs[i].point1().x()+segs[i].point2().x())/2,
                                  (segs[i].point1().y()+segs[i].point2().y())/2);

          EM_new->insert(new dbdet_edgel(pt, tangent));
        }
      }
    }
  }
  //---------------------------------------------------------------------------------

  // create the edgemap storage class
  dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();;
  output_edgemap->set_edgemap(EM_new);
  output_data_[0].push_back(output_edgemap);

  return true;
}

bool
dbdet_subsample_edges::finish()
{
  return true;
}

void
dbdet_subsample_edges::get_parameters()
{
  //parameters
  parameters()->get_value( "-subsampling_algo", subsampling_algo);

  parameters()->get_value( "-SCF_len", SCF_len);
  parameters()->get_value( "-linefit_rms", linefit_rms);

  parameters()->get_value( "-bremove_short_frags", bremove_short_frags);
  
}

