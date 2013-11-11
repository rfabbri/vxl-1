// This is brcv/seg/dbdet/pro/dbdet_edge_tracer_process.cxx

//:
// \file

#include "dbdet_edge_tracer_process.h"


#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage_sptr.h>

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>

//: Constructor
dbdet_edge_tracer_process::dbdet_edge_tracer_process()
{
  if( !parameters()->add( "Edge pixel Value"   , "-edge_val" , 0) ||
      !parameters()->add( "Smooth Contour"  , "-bsmooth" , true ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

//: Destructor
dbdet_edge_tracer_process::~dbdet_edge_tracer_process()
{
}


//: Clone the process
bpro_process*
dbdet_edge_tracer_process::clone() const
{
  return new dbdet_edge_tracer_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_edge_tracer_process::name()
{
  return "Edge Tracer";
}


//: Return the number of input frame for this process
int
dbdet_edge_tracer_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_edge_tracer_process::output_frames()
{
  return 1;
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_edge_tracer_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_edge_tracer_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Execute the process
bool
dbdet_edge_tracer_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbdet_edge_tracer_process::execute() - not exactly one"
             << " input images \n";
    return false;
  }
  clear_output();

  vcl_cout << "Tracing ...";
  vcl_cout.flush();

  // get image from the storage class
  vidpro_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);
  vil_image_resource_sptr image_sptr = frame_image->get_image();
  vil_image_view<vxl_byte> image_view = image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj() );

  //get the parameters
  unsigned edge_val;
  bool bsmooth;
  parameters()->get_value( "-edge_val", edge_val);
  parameters()->get_value( "-bsmooth" , bsmooth );
  
  // perform edge tracing with the given parameters
  vcl_vector< vsol_spatial_object_2d_sptr > image_curves;
  trace_edges(image_view, edge_val, image_curves);

  //smooth the traced curves if desired
  if (bsmooth){
  }

  // create the output storage class
  vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
  output_vsol->add_objects(image_curves, "traced contours");
  output_data_[0].push_back(output_vsol);

  vcl_cout << "done!" << vcl_endl;
  vcl_cout.flush();

  return true;
}

bool
dbdet_edge_tracer_process::finish()
{
  return true;
}


void dbdet_edge_tracer_process::trace_edges(vil_image_view<vxl_byte> & image, 
                                            unsigned edge_val, 
                                            vcl_vector<vsol_spatial_object_2d_sptr> & contours)
{
  //set up various buffers to assist in edge tracing

  //first trace chains from end points
  trace_contours_from_end_points();

  //next trace all the chains starting at junctions
  //trace_contours_from_junctions();

  //finally trace all the chains that are closed
  //trace_closed_contours();

}

bool dbdet_edge_tracer_process::Is_Inside_Image (int row, int col)
{
  return ((row >= 0) && (row < nrows_) &&
          (col >= 0) && (col < ncols_));
}

bool dbdet_edge_tracer_process::Is_8_Neighbor (int x1, int y1, int x2, int y2)
{
  //* Two points are 8-connected neighbors
  //* IF delta_x <= 1 AND delta_y <= 1

  int delta_x = vcl_abs (x2 - x1);
  int delta_y = vcl_abs (y2 - y1);

  return ((delta_x <= 1) && (delta_y <= 1));
}

bool dbdet_edge_tracer_process::Is_4_Neighbor (int x1, int y1, int x2, int y2)
{
  //* Two points are 4-connected neighbors
  //* IF delta_x + delta_y = 1

  int delta_x = vcl_abs (x2 - x1);
  int delta_y = vcl_abs (y2 - y1);

  return ((delta_x+delta_y) == 1);
}

//: returns the number of 8 connected neighbors to the current edgel.
//   (when it's on the border, it ignores the out of bounds pixels)
int dbdet_edge_tracer_process::Num_of_8_Neighbor (int row, int col)
{
  int num_of_neighbors = 0;
  
  // if the point is on the boundary of the image return 0
  if (row ==0 || row==(nrows_-1) || col==0 || col==(ncols_-1))
    return 0;

  // go over all its 8 neighbors
  for (int ii = -1; ii < 2; ii++)
    for (int jj = -1; jj < 2; jj++)
      if (unlinked_(row+ii, col+jj))
        num_of_neighbors++;

  return num_of_neighbors;
}

// Get a list of neighboring unlinked edgels 
int dbdet_edge_tracer_process::Find_8_Neighbors (int row, int col, int *neighbor_row, int *neighbor_col)
{
  int neighbor_ct = 0;
  int d_row[8] = {0, -1, -1, -1, 0, 1, 1, 1};
  int d_col[8] = {1, 1, 0, -1, -1, -1, 0, 1};

  int neigh_row, neigh_col;
  
  //go over the 8-neighbors of this edgel and determine 
  //if there are any unlinked edgels available
  for (int neigh_lp = 0; neigh_lp < 8; neigh_lp++)
  {
    neigh_row = row + d_row[neigh_lp];
    neigh_col = col + d_col[neigh_lp];

    //make sure the neighboring edgels are inside the image
    if (Is_Inside_Image (neigh_row, neigh_col))
    {
      if (unlinked_(neigh_row, neigh_col)){
        neighbor_row[neighbor_ct] = neigh_row;
        neighbor_col[neighbor_ct] = neigh_col;

        neighbor_ct++;
      }
    }
  }

  return neighbor_ct;
}

//: Grow the chain recursively from the current edgel (assume 8-connectivity)
//  Assumes that the current pixel has already been added to the current chain
//  but hasn't been marked off as linked yet.
int dbdet_edge_tracer_process::Grow_Seed_Edge(int x, int y, dbdet_edgel_chain* cur_chain)
{
  int neighbor_row[8], neighbor_col[8];
  
  //find the number of neighbors that are connected to this edgel
  int num_of_neigh = Find_8_Neighbors (y, x, neighbor_row, neighbor_col);

  if (num_of_neigh == 1)
  {
    // Mark the current edge as linked
    unlinked_(y, x) = false;

    //update the cur_edgel pointer to the neighboring edgel
    x = neighbor_col[0]; y = neighbor_row[0];

    //add the neighbor to the chain
    cur_chain->push_back(edgemap_->edge_cells(y, x).front());

    //grow the chain from this neighbor
    return Grow_Seed_Edge (x, y, cur_chain);
  } 

  //special treatment necessary if there are more than one neighboring edgels to the current one
  if (num_of_neigh == 2)
  {
    // if they are not 4-neighbors, we've reached a junction point, so terminate the chain
    if (!Is_4_Neighbor (neighbor_col[0], neighbor_row[0], neighbor_col[1], neighbor_row[1]))
      return 0;

    // If they are 4-neighbors of each other, add both of them to the chain

    // compute the displacements of the neighboring pixels from the current pixel
    // to figure out which one is closer
    int y_dist1 = neighbor_row[0] - y;
    int x_dist1 = neighbor_col[0] - x;

    int y_dist2 = neighbor_row[1] - y;
    int x_dist2 = neighbor_col[1] - x;

    if ((x_dist1 * y_dist1) == 0) 
    {
      // Mark the current edge as linked
      unlinked_(y, x) = false;

      //update the cur_edgel pointer to the closer neighbor
      x = neighbor_col[0]; y = neighbor_row[0];

      //add the closer neighbor to the chain
      cur_chain->push_back(edgemap_->edge_cells(y, x).front());

      // Mark the closer edgel as linked
      unlinked_(y, x) = false;

      //update the current edgel pointer to the farther edgel
      x = neighbor_col[1]; y = neighbor_row[1];

      //now add the farther edgel to the chain
      cur_chain->push_back(edgemap_->edge_cells(y, x).front());
    }
    else
    {
      // Mark the current edge as linked
      unlinked_(y, x) = false;

      //update the cur_edgel pointer to the closer neighbor
      x = neighbor_col[1]; y = neighbor_row[1];

      //add the closer neighbor to the chain      
      cur_chain->push_back(edgemap_->edge_cells(y, x).front());

      // Mark the closer edgel as linked
      unlinked_(y, x) = false;

      //update the current edgel pointer to the farther edgel
      x = neighbor_col[0]; y = neighbor_row[0];

      //now add the farther edgel to the chain
      cur_chain->push_back(edgemap_->edge_cells(y, x).front());
    }

    //now grow from the updated current edgel
    return Grow_Seed_Edge (x, y, cur_chain);
  }

  // the current edgel is at a junction, terminate the chain
  return 0;
}

void dbdet_edge_tracer_process::trace_contours_from_end_points()
{
  dbdet_edgel_chain *temp_chain; 
  int neighbor_row[8], neighbor_col[8];

  // Loop over all pixels in the image to see if chains can be started from them
  for (int i = 1; i < (nrows_-1); i++){
    for (int j = 1; j < (ncols_-1); j++){

      if (!unlinked_(i,j)) //either linked or no edgel at this pixel
        continue;

      // Get the # of 8-neighbors of this edgel 
      int num_of_neigh = Find_8_Neighbors(i, j, neighbor_row, neighbor_col);

      // if this pixel contains an unlinked edgel and it has a single unlinked neighbor
      // or two unlinked neighbors that are themseleves neighbors of each other, 
      // this edgel is a good candidate to start tracing a chain from.

      if (num_of_neigh == 1 || num_of_neigh ==2 )
      {
        if (num_of_neigh == 2){
          // Make sure it is an 8-connect link (if the 2 neighbors are themselves 4-connected)
          if (!Is_4_Neighbor(neighbor_col[0], neighbor_row[0], neighbor_col[1], neighbor_row[1]))
            continue;
        }

        //start a new chain
        temp_chain = new dbdet_edgel_chain();
        temp_chain->push_back(edgemap_->edge_cells(i,j).front());

        Grow_Seed_Edge (j, i, temp_chain);

        //save this chain on the list of chains
        edgel_chains_.push_back(temp_chain);
      }
    } 
  }
}

