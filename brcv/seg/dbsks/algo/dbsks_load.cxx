// This is dbsks/algo/dbsks_load.cxx

//:
// \file

#include "dbsks_load.h"




#include <dbsks/xio/dbsks_xio_xgraph_ccm_model.h>
#include <dbsks/dbsks_xgraph_ccm_model.h>

#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsks/xio/dbsks_xio_xgraph_geom_model.h>

#include <dbsol/dbsol_file_io.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polyline_2d.h>

#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_file_matrix.h>



//
//
//------------------------------------------------------------------------------
//: load the image
bool dbsks_load_image_resource(const vcl_string& image_file, vil_image_resource_sptr& image_resource)
{
  vcl_cout << "\n> Loading image: " << image_file << " ... ";
  image_resource = vil_load_image_resource(image_file.c_str());
  
  if (!image_resource) 
  {
    vcl_cout << "[ Failed ]\n";
    return false;
  }
  else 
  {
    vcl_cout << "[ OK ]\n";
  }
  return true;
}


//------------------------------------------------------------------------------
//: Load edgemap
bool dbsks_load_edgemap(const vcl_string& edgemap_file, vil_image_view<float >& edgemap)
{
  //>>> load the edge map
  vcl_cout << "\n> Loading edgemap: " << edgemap_file << " ... ";
  edgemap = *vil_convert_cast(float(), vil_load(edgemap_file.c_str()));
  if (!edgemap) 
  {
    vcl_cout << "[ Failed ]\n";
    return false;
  }
  else 
  {
    vcl_cout << "[ OK ]\n";
  }
  return true;
}



//------------------------------------------------------------------------------
//: Load edge angle
bool dbsks_load_edge_angle(const vcl_string& edgeorient_file, vil_image_view<float >& edge_angle)
{
  // Load edge orientation file
  vcl_cout << "\n> Loading edge orientation file: " << edgeorient_file << " ... ";
  vnl_file_matrix<double > theta(edgeorient_file.c_str());
  bool loaded_edgeorient = !theta.empty();
  if (!loaded_edgeorient)
  {
    vcl_cout << "[ Failed ]\n";
    return false;
  }
  else
  {
    vcl_cout << "[ OK ]\n";
  }

  // create edge angle image view from a matrix
  edge_angle.set_size(theta.cols(), theta.rows());
  for (unsigned i =0; i < edge_angle.ni(); ++i)
  {
    for (unsigned j =0; j < edge_angle.nj(); ++j)
    {
      edge_angle(i, j) = float(theta(j, i));
    }
  }
  return true;
}


//------------------------------------------------------------------------------
//: Load an sub-pixel edgemap from 2 files: a binary image of edge location
// and a text file of edge orientation at each location
dbdet_edgemap_sptr dbsks_load_subpix_edgemap(const vcl_string edgemap_file, 
                                             const vcl_string edgeorient_file,
                                             float lower_threshold,
                                             float max_edge_value)
{
  // Load edge orientation file
  vcl_cout << "\n> Loading sub-pixel edgemap from 2 files: \n"
    << "    edge location: " << edgemap_file << "\n"
    << "    edge orientation: " << edgeorient_file << "\n";

  vil_image_view<float > edgemap_view;
  if (!dbsks_load_edgemap(edgemap_file, edgemap_view))
    return 0;
  vil_image_view<float > edge_angle_view;
  if (!dbsks_load_edge_angle(edgeorient_file, edge_angle_view))
    return 0;


  if (edgemap_view.ni() != edge_angle_view.ni() || 
    edgemap_view.nj() != edge_angle_view.nj())
  {
    vcl_cout << "  \nERROR: dimension mismatched.... [ Failed ]\n";
    return 0;
  }

  // construct the edgemap with the same size as the image
  dbdet_edgemap_sptr edgemap = new dbdet_edgemap(edgemap_view.ni(), edgemap_view.nj());
  int ni = int(edgemap_view.ni());
  int nj = int(edgemap_view.nj());

  for (int j =0; j < nj; ++j)
  {
    for (int i =0; i < ni; ++i)  
    {
      if (edgemap_view(i, j) < lower_threshold)
        continue;

      double x = i;
      double y = j;
      double dir = edge_angle_view(i, j);
      double conf = edgemap_view(i, j) / max_edge_value;

      dbdet_edgel* e = new dbdet_edgel(vgl_point_2d<double>(x,y), dir, conf);
      e->id = edgemap->edgels.size();
      edgemap->insert(e);      
    }
  }
  vcl_cout << "    \nSub-pixel edgemap constructed.... [ OK ]\n";
  
  return edgemap;
}





//------------------------------------------------------------------------------
//: Load linked-edge list
bool dbsks_load_polyline_list(const vcl_string& cemv_file, 
                              vcl_vector<vsol_polyline_2d_sptr >& polyline_list)
{
  //>> Load the cemv file containing linked contour info
  vcl_cout << "\n>>Loading linked-edge contour file: " << cemv_file << "...";
  vcl_vector<vsol_spatial_object_2d_sptr > vsol_list;
  if (!dbsol_load_cem(vsol_list, cemv_file))
  {
    vcl_cout << "[ Failed ]\n";
    return false;
  }
  else
  {
    vcl_cout << "[ OK ]\n";
  }


  // filter to take only the polylines
  polyline_list.clear();
  polyline_list.reserve(vsol_list.size());
  for (unsigned i =0; i < vsol_list.size(); ++i)
  {
    vsol_spatial_object_2d_sptr obj = vsol_list[i];
    if (obj->cast_to_curve() && obj->cast_to_curve()->cast_to_polyline())
    {
      polyline_list.push_back(obj->cast_to_curve()->cast_to_polyline());
    }
  }
  return true;
}


//------------------------------------------------------------------------------
//: Load the shock graph
bool dbsks_load_xgraph(const vcl_string& xgraph_file, dbsksp_xshock_graph_sptr& xgraph)
{
  vcl_cout << "\n> Loading xshock_graph XML file: " << xgraph_file << "...";
  xgraph = 0;
  if (!x_read(xgraph_file, xgraph) )
  {
    vcl_cout << "[ Failed ]\n";
    return false;
  }
  else
  {
    vcl_cout << "[ OK ]\n";
  }
  return true;
}


//------------------------------------------------------------------------------
//: Load the xgraph geometric model
bool dbsks_load_xgraph_geom_model(const vcl_string& xgraph_geom_file,
                                  const vcl_string& xgraph_geom_param_file,
                                  dbsks_xgraph_geom_model_sptr& xgraph_geom)
{
    vcl_cout << "\n> Loading xgraph geometric model file (.xml):"
    << xgraph_geom_file << vcl_endl << " and geom parameter file (.xml):" << xgraph_geom_param_file << "...";
  xgraph_geom = 0;
  if (!x_read(xgraph_geom_file, xgraph_geom_param_file, xgraph_geom))
  {
    vcl_cout << "[ Failed ]\n";
    return false;
  }
  else
  {
    vcl_cout << "[ OK ]\n";
  }
  return true;
}


//------------------------------------------------------------------------------
//: Load the xgraph CCM model
bool dbsks_load_xgraph_ccm_model(const vcl_string& xgraph_ccm_file,
                                 const vcl_string& xgraph_ccm_param_file,
                                 dbsks_xgraph_ccm_model_sptr& xgraph_ccm)
{
  vcl_cout << "> Loading xgraph CCM (Contour Chamfer Matching) model file (.xml):" 
    << xgraph_ccm_file << vcl_endl << " and CCM parameter file (.xml):" << xgraph_ccm_param_file << "...";
  xgraph_ccm = 0;

  if (!x_read(xgraph_ccm_file, xgraph_ccm_param_file, xgraph_ccm))
  {
    vcl_cout << "[ Failed ]\n";
    return false;
  }
  else
  {
    vcl_cout << "[ OK ]\n";
  }
  return true;
}

