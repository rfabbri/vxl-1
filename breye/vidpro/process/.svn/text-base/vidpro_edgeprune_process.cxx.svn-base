#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vidpro/process/vidpro_edgeprune_process.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_interpolator.h>

vidpro_edgeprune_process::vidpro_edgeprune_process() : bpro_process()
{
  if(!parameters()->add( "Prune edges?" ,                               "-prune" ,  true ) ||
     !parameters()->add( "by length? (otherwise by number of points)" , "-prunel" , true ) ||
     !parameters()->add( "Prune threshold" ,                            "-thres" ,  (int)40 ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Clone the process
bpro_process*
vidpro_edgeprune_process::clone() const
{
  return new vidpro_edgeprune_process(*this);
}


vcl_vector< vcl_string > vidpro_edgeprune_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;

  to_return.push_back( "vtol" );
  return to_return;
}

vcl_vector< vcl_string > vidpro_edgeprune_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;

  //is this input or output
  to_return.push_back( "vsol2D" );
  return to_return;
}

bool vidpro_edgeprune_process::execute()
{
  bool prune=false; //I = true O = false
  parameters()->get_value( "-prune" , prune );
  bool prunel=false; //I = true O = false
  parameters()->get_value( "-prunel" , prunel );
  int thres=0;
  parameters()->get_value( "-thres" , thres );

  // create the input storage class
  vidpro_vtol_storage_sptr input_vtol;
  input_vtol.vertical_cast(input_data_[0][0]);

  // new vvector to store the points
  vcl_vector< vsol_spatial_object_2d_sptr > contours;

  for ( vcl_set<vtol_topology_object_sptr>::const_iterator itr = input_vtol->begin();
        itr != input_vtol->end();  ++itr ) {

    //: Adding only curves from the vtol objects
    vtol_edge *edge = (*itr)->cast_to_edge();
    if (edge){
      vtol_edge_2d *edge_2d = edge->cast_to_edge_2d();
      if (edge_2d){
        vsol_curve_2d *curve_2d = edge_2d->curve().ptr();
        if (curve_2d) {
          vdgl_digital_curve_sptr dc = curve_2d->cast_to_vdgl_digital_curve();
          if (dc.ptr()){
            vdgl_edgel_chain_sptr chain = dc->get_interpolator()->get_edgel_chain();
                   
            if ((prune && 
                  ((!prunel && int(chain->size()) > thres) || 
                   (prunel && dc->get_interpolator()->get_length() > thres)
                  )
                 ) || !prune) {
              vcl_vector< vsol_point_2d_sptr > points; 
              for (unsigned int i = 0; i<chain->size(); i++) {
                vsol_point_2d_sptr newPt = new vsol_point_2d (chain->edgel(i).x(),chain->edgel(i).y());
                points.push_back(newPt);
              }
              vsol_polyline_2d_sptr newContour = new vsol_polyline_2d (points);
              contours.push_back(newContour->cast_to_spatial_object());
            }

          }
        } else vcl_cout << "failed curve_2d" << vcl_endl;
      } else vcl_cout << "failed edge_2d" << vcl_endl;
    } else vcl_cout << "failed edge" << vcl_endl;
        
  }

  vcl_cout << "Pruned " << input_vtol->size()-contours.size() << " edges out of " << input_vtol->size() << " edges\n"; 
  // create the output storage class
  vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
  output_vsol->add_objects(contours, "pruned");
  output_data_[0].push_back(output_vsol);

  return true;
}
