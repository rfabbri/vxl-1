// This is brcv/seg/dbsks/bin/dbsks_cmdline_utils.cxx

//:
// \file

#include "dbsks_cmdline_utils.h"

//#include <dbsks/dbsks_compute_ocm_cost.h>
#include <dbsks/dbsks_biarc_sampler.h>
//#include <dbsks/dbsks_gray_ocm.h>

#include <dbsks/dbsks_xnode_geom.h>
#include <dbsks/dbsks_xgraph_geom_model.h>
#include <dbsks/dbsks_xnode_geom_sptr.h>
#include <dbsks/dbsks_hog.h>
#include <dbsks/dbsks_xfrag_bhog_model.h>
#include <dbsks/dbsks_xshock_utils.h>
#include <dbsks/dbsks_utils.h>
#include <bpro1/bpro1_parameters.h>

#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsks/xio/dbsks_xio_xgraph_geom_model.h>
#include <dbsks/xio/dbsks_xio_xshock_det.h>

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsksp/dbsksp_xshock_fragment_sptr.h>

#include <dbul/dbul_parse_simple_file.h>
#include <dbil/algo/dbil_gradient_color.h>
#include <dbsol/dbsol_file_io.h>

#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_polyline_2d.h>

#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vul/vul_sprintf.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>

#include <vnl/vnl_random.h>
#include <vnl/vnl_file_matrix.h>
#include <vnl/vnl_math.h>

#include <vcl_utility.h>
#include <vcl_cstdlib.h>

//: Convert an xshock_fragment and its corresponding graph's area into a 
// vnl_vector to save to a file
void dbsks_convert_to_vector(const dbsksp_xshock_fragment& xfrag, double graph_area, 
                             vnl_vector<double >& out);

//: Extract info from a vnl_vector
bool dbsks_convert_from_vector(const vnl_vector<double >& x, 
                               dbsksp_xshock_fragment& xfrag, double& graph_area);




// -----------------------------------------------------------------------------
//: Convert all postscript files (typically with extension .ps) in "input_folder"
// to .png format and save to "output_folder"
bool dbsks_batch_ps_to_jpeg(const vcl_string& input_folder, const vcl_string& output_folder, 
                        const vcl_string& ps_file_extension)
{
  // set of ps files
  vcl_string file_set = input_folder + "/*" + ps_file_extension;
  

  vcl_cout << "iterating over the files: " << file_set << vcl_endl;
  for (vul_file_iterator fi(file_set); fi; ++fi)
  {
    vcl_cout << fi() << "\n";
    if (!vul_file::exists(fi()))
      continue;
    
    // 1. convert from PS to ppm

    // find the object name
    vcl_string command1 = "C:\\cygwin\\bin\\pstopnm -portrait -xborder 0 -yborder 0 -ppm ";
    command1 = command1 + "\"" + vcl_string(fi()) + "\"" ;
    vcl_cout << "\tcommand: " << command1 << vcl_endl;
    vcl_system(command1.c_str());

    // 2. convert from ppm to png
    vcl_string pnm_file = "\"" + vul_file::strip_extension(fi()) + "001.ppm" + "\"";
    vcl_string jpeg_file = "\"" + output_folder + "\\" +
      vul_file::strip_extension(vul_file::strip_directory(fi())) + ".jpg" + "\"";
    vcl_string command2 = "C:\\cygwin\\bin\\pnmtojpeg ";
    command2 = command2 +  pnm_file + " > " + jpeg_file;
    vcl_cout << "\tcommand: " << command2 << vcl_endl;
    vcl_system(command2.c_str());

    // 3. Delete the ppm file
    vcl_cout << "\tDelete: " << pnm_file << vcl_endl;
    vul_file::delete_file_glob(pnm_file.c_str());
  }

  vcl_cout << " done!\n";
  return true;
}



// -----------------------------------------------------------------------------
//: Create a summary XML file from
bool dbsks_create_shapematch_summary_xml(const vcl_string& output_xml,
                                         const vcl_string& input_folder,
                                         const vcl_string& image_list,
                                         const vcl_string& shapematch_extension
                                         )
{
  // get the image list
  vcl_vector<vcl_string > image_names;
  dbul_parse_string_list(image_list, image_names);
  for (unsigned i =0; i < image_names.size(); ++i)
  {
    vcl_string image_name = image_names[i];
    vcl_string shapematch_file = input_folder + "/" + 
      vul_file::strip_extension(image_name) + shapematch_extension;

    // create an xml element from this shapematch file

  
  }
  
  return false;
}


// -----------------------------------------------------------------------------
//: Extract properties of an extrinsic fragment in a list of extrinsic shock graphs
bool dbsks_extract_xfrag_geom(const vcl_string& xshock_folder,
                              const vcl_string& xshock_list_file,
                              unsigned xedge_id,
                              const vcl_string& output_file)
{
  // parse input file
  vcl_vector<vcl_string > xml_filenames;
  dbul_parse_string_list(xshock_list_file, xml_filenames);

  // Process data
  // Go thru each shock graph and extract geometry of the fragment of interest
  vcl_vector<vnl_vector<double > > frag_geom_list;
  for (unsigned index =0; index < xml_filenames.size(); ++index)
  {
    // Load the shock graph 
    vcl_string xml_file = xshock_folder + "/" + xml_filenames[index];
    dbsksp_xshock_graph_sptr xg = 0;

    vcl_cout << "Loading xshock_graph XML file: " << xml_file << "...";
    if ( x_read(xml_file, xg) )
    {
      vcl_cout << "Succeeded.\n";
    }
    else
    {
      vcl_cout << "Failed.\n";
      continue;
    }

    // area of the graph
    double area = xg->area();

    // extract fragment properties
    dbsksp_xshock_edge_sptr xe = xg->edge_from_id(xedge_id);
    dbsksp_xshock_node_sptr xv0 = xe->source();
    dbsksp_xshock_node_sptr xv1 = xe->target();

    dbsksp_xshock_node_descriptor xdesc0 = *xv0->descriptor(xe);
    dbsksp_xshock_node_descriptor xdesc1 = xv1->descriptor(xe)->opposite_xnode();

    frag_geom_list.push_back(vnl_vector<double >());
    vnl_vector<double >& frag_geom = frag_geom_list.back();

    dbsks_convert_to_vector(dbsksp_xshock_fragment(xdesc0, xdesc1), area, frag_geom);
  }


  // Writing data to output file

  // open output file for writing
  vcl_ofstream os(output_file.c_str(), vcl_ios_out);
  if (!os)
  {
    vcl_cout << " ERROR: Couldn't open for writing file: " << output_file << ".\n";
    return false;
  }


  // write data
  for (unsigned k =0; k < frag_geom_list.size(); ++k)
  {
    vnl_vector<double >& x = frag_geom_list[k];
    os << x << "\n";
  }
  os.close();
  return false;
}


// -----------------------------------------------------------------------------
//: Convert an xshock_fragment and its corresponding graph's area into a 
// vnl_vector to save to a file
void dbsks_convert_to_vector(const dbsksp_xshock_fragment& xfrag, double graph_area, 
                             vnl_vector<double >& out)
{
  out.set_size(11);
  dbsksp_xshock_node_descriptor start = xfrag.start();
  dbsksp_xshock_node_descriptor end = xfrag.end();
  out[0] = start.pt().x();
  out[1] = start.pt().y();
  out[2] = start.psi_;
  out[3] = start.phi_;
  out[4] = start.radius_;
  out[5] = end.pt().x();
  out[6] = end.pt().y();
  out[7] = end.psi_;
  out[8] = end.phi_;
  out[9] = end.radius_;
  out[10] = graph_area;
}



// -----------------------------------------------------------------------------
//: Extract info from a vnl_vector
bool dbsks_convert_from_vector(const vnl_vector<double >& in, 
                               dbsksp_xshock_fragment& xfrag, double& graph_area)
{
  if (in.size() != 11) return false;
  dbsksp_xshock_node_descriptor start(in[0], in[1], in[2], in[3], in[4]);
  dbsksp_xshock_node_descriptor end(in[5], in[6], in[7], in[8], in[9]);
  graph_area = in[10];
  xfrag.set_start(start);
  xfrag.set_end(end);
  return true;
}










// -----------------------------------------------------------------------------
//: Extract positive examples of boundary histogram of gradient of extrinsic shock graphs
bool dbsks_extract_positive_xfrag_bhog(const vcl_string& xshock_folder,
                                    const vcl_string& image_folder,
                                    const vcl_string& xshock_list_file,
                                    unsigned xedge_id,
                                    const vcl_string& output_file)
{
  // random number generator
  vnl_random rand_engine(1010293);

  int num_segments_per_bnd_side = 4;
  double patch_width = 8; // width the arc patch
  int num_bins = 9;

  // place holder for the feature vectors
  vcl_vector<vnl_vector<double > > bhog_list;

  // parse input file
  vcl_vector<vcl_string > xml_filenames;
  dbul_parse_string_list(xshock_list_file, xml_filenames);

  // collect the fragments and their repsected scales
  vcl_vector<dbsksp_xshock_fragment > xfrag_list;
  vcl_vector<double > xfrag_scales;
  vcl_vector<vcl_string > image_file_list;
  for (unsigned i_file =0; i_file < xml_filenames.size(); ++i_file)
  {
    // Load the shock graph 
    vcl_string xml_file = xshock_folder + "/" + xml_filenames[i_file];
    dbsksp_xshock_graph_sptr xg = 0;
    if ( x_read(xml_file, xg) )
    {
      vcl_cout << "Loaded xshock_graph XML file: " << xml_file << ".\n";
    }
    else
    {
      vcl_cout << "Couldn't load xshock_graph XML file: " << xml_file << ".\n";
      continue;
    }

    // extract fragment
    dbsksp_xshock_edge_sptr xe = xg->edge_from_id(xedge_id);
    dbsksp_xshock_node_sptr xv0 = xe->source();
    dbsksp_xshock_node_sptr xv1 = xe->target();

    dbsksp_xshock_node_descriptor xdesc0 = *xv0->descriptor(xe);
    dbsksp_xshock_node_descriptor xdesc1 = xv1->descriptor(xe)->opposite_xnode();

    xfrag_list.push_back(dbsksp_xshock_fragment(xdesc0, xdesc1));
    xfrag_scales.push_back(vcl_sqrt(xg->area()));

    // determine the corresponding image file name
    vcl_string image_file = image_folder + "/" + 
      vul_file::strip_extension(xml_filenames[i_file]) + ".jpg";

    image_file_list.push_back(image_file);
  }


  // compute the average scale
  double sum_logscale = 0;
  for (unsigned i =0; i < xfrag_scales.size(); ++i)
  {
    sum_logscale += vcl_log(xfrag_scales[i]);
  }
  double avg_scale = vcl_exp(sum_logscale / xfrag_scales.size());
  

  // go thru each xfrag and collect boundary HOG
  for (unsigned i_frag =0; i_frag < xfrag_list.size(); ++i_frag)
  {
    dbsksp_xshock_fragment orig_xfrag = xfrag_list[i_frag];
    double xfrag_scale = xfrag_scales[i_frag];
    
    // load the image
    vcl_string image_file = image_file_list[i_frag]; 
    vil_image_view<double > image_view = *vil_convert_cast(double(), vil_load(image_file.c_str()));
    if (image_view.nplanes() != 3)
    {
      vcl_cout << "ERROR: expected a color image but didn't get it.\n";
      return false;
    }

    // Compute gradient of color image by taking strongest component of the three color planes
    vil_image_view<double > Gx, Gy;
    dbil_gradient_rgb_Linf(image_view, Gx, Gy);

    // randomly perturb the fragment within a small range to collect positive histogram
    int num_bhog_per_pos_xgraph = 1000;
    double delta_x = 4;
    double delta_y = 4;
    double delta_phi = vnl_math::pi / 24;
    double delta_psi = vnl_math::pi / 16 / 2;
    double delta_radius = 2;

    for (int k =0; k < num_bhog_per_pos_xgraph; ++k)
    {
      double dx0 = rand_engine.drand32(-delta_x, delta_x);
      double dx1 = rand_engine.drand32(-delta_x, delta_x);

      double dy0 = rand_engine.drand32(-delta_y, delta_y);
      double dy1 = rand_engine.drand32(-delta_y, delta_y);
      
      double dphi0 = rand_engine.drand32(-delta_phi, delta_phi);
      double dphi1 = rand_engine.drand32(-delta_phi, delta_phi);

      double dpsi0 = rand_engine.drand32(-delta_psi, delta_psi);
      double dpsi1 = rand_engine.drand32(-delta_psi, delta_psi);

      double dradius0 = rand_engine.drand32(-delta_radius, delta_radius);
      double dradius1 = rand_engine.drand32(-delta_radius, delta_radius);

      dbsksp_xshock_node_descriptor xdesc0 = orig_xfrag.start();
      dbsksp_xshock_node_descriptor xdesc1 = orig_xfrag.end();

      xdesc0.pt_ += vgl_vector_2d<double >(dx0, dy0);
      xdesc1.pt_ += vgl_vector_2d<double >(dx1, dy1);

      xdesc0.phi_ += dphi0;
      xdesc1.phi_ += dphi1;

      xdesc0.psi_ += dpsi0;
      xdesc1.psi_ += dpsi1;

      xdesc0.radius_ += dradius0;
      xdesc1.radius_ += dradius1;

      // collect histogram of gradient along the boundary
      dbsksp_xshock_fragment xfrag(xdesc0, xdesc1);

      // compute boundary histogram of orientation gradient for this fragment and image
      vnl_vector<double > bhog;
      if (!dbsks_hog_boundary(bhog, Gx, Gy, xfrag, num_segments_per_bnd_side, patch_width, num_bins))
      {
        vcl_cerr << "ERROR: failed to compute HOG of fragment boundary.\n";
        continue;
      }

      bhog_list.push_back(bhog);
    } // k - num_bhog
  } // i_frag


  // open output file for writing
  vcl_ofstream os(output_file.c_str(), vcl_ios_out);
  if (!os)
  {
    vcl_cout << " ERROR: Couldn't open for writing file: " << output_file << ".\n";
    return false;
  }


  // save data
  for (unsigned i =0; i < bhog_list.size(); ++i)
  {
    os << bhog_list[i] << "\n";
  }

  // close file
  os.close();

  return true;
}





//: Extract negative examples of boundary histogram of gradient of extrinsic shock graphs
bool dbsks_extract_negative_xfrag_bhog(const vcl_string& xshock_folder,
                              const vcl_string& xshock_list_file,
                              unsigned xedge_id,
                              const vcl_string& image_folder,
                              const vcl_string& image_list_file,
                              const vcl_string& output_file)
{
  int num_segments = 4;
  double patch_width = 8; // width the arc patch

  vnl_random rand_engine(9667566);

  // parse input file
  vcl_vector<vcl_string > xml_filenames;
  dbul_parse_string_list(xshock_list_file, xml_filenames);

  // Extract a list of fragment from list of shock files
  vcl_vector<dbsksp_xshock_fragment > xfrag_list;
  for (unsigned i_file =0; i_file < xml_filenames.size(); ++i_file)
  {
    // Load the shock graph 
    vcl_string xml_file = xshock_folder + "/" + xml_filenames[i_file];
    vcl_cout << "Loading xshock_graph XML file: " << xml_file << " ...";
    dbsksp_xshock_graph_sptr xg = 0;
    if ( x_read(xml_file, xg) ) 
    {
      vcl_cout << "Suceeded.\n";
    }
    else 
    {
      vcl_cout << "Failed.\n";
      continue;
    }

    // extract fragment properties
    dbsksp_xshock_edge_sptr xe = xg->edge_from_id(xedge_id);
    dbsksp_xshock_node_sptr xv0 = xe->source();
    dbsksp_xshock_node_sptr xv1 = xe->target();

    dbsksp_xshock_node_descriptor xdesc0 = *xv0->descriptor(xe);
    dbsksp_xshock_node_descriptor xdesc1 = xv1->descriptor(xe)->opposite_xnode();
    xfrag_list.push_back(dbsksp_xshock_fragment(xdesc0, xdesc1));
  }


    

  vcl_vector<vnl_vector<double > > bhog_list;

  // For each negative image, extract BHOG corresponding to all extracted xfragments
  // To handling different image sizes, the fragments are located at the center of image
  // parse input file
  vcl_vector<vcl_string > image_filenames;
  dbul_parse_string_list(image_list_file, image_filenames);
  for (unsigned i_file =0; i_file < image_filenames.size(); ++i_file)
  {
    // load the image
    vcl_string image_file = image_folder + "/" + 
      vul_file::strip_extension(image_filenames[i_file]) + ".jpg";

    vcl_cout << "Loading image: " << image_file << " ... ";
    vil_image_view<double > image_src = *vil_convert_cast(double(), vil_load(image_file.c_str()));

    if (!image_src)
    {
      vcl_cout << "Failed.\n";
    }
    else
    {
      vcl_cout << "Succeeded.\n";
    }

    // Compute gradient of color image by taking strongest component of the three color planes
    vil_image_view<double > Gx, Gy;
    dbil_gradient_rgb_Linf(image_src, Gx, Gy);

    // image center
    vgl_point_2d<double > image_center(image_src.ni() / 2, image_src.nj() / 2);

    // a place to draw bounding boxes of sampled fragments on
    vil_image_view<vxl_byte > frags_on_image(image_src.ni(), image_src.nj(), 3);
    if (image_src.nplanes() == 3)
    {
      for (unsigned i =0; i < image_src.ni(); ++i)
      {
        for (unsigned j =0; j < image_src.nj(); ++j)
        {
          for (unsigned p =0; p < 3; ++p)
          {
            frags_on_image(i, j, p) = static_cast<vxl_byte >(image_src(i, j, p));
          }
        }
      }
    }
    else
    {
      for (unsigned i =0; i < image_src.ni(); ++i)
      {
        for (unsigned j =0; j < image_src.nj(); ++j)
        {
          for (unsigned p =0; p < 3; ++p)
          {
            frags_on_image(i, j, p) = static_cast<vxl_byte >(image_src(i, j));
          }
        }
      }
    }
    



    // extract bhog for each fragment on this image
    for (unsigned i_xfrag =0; i_xfrag < xfrag_list.size(); ++i_xfrag)
    {
      dbsksp_xshock_fragment xfrag = xfrag_list[i_xfrag];

      // translate this fragment to align with the center of the image
      vgl_vector_2d<double > t0 = image_center - centre(xfrag.start().pt(), xfrag.end().pt());
      xfrag.start().pt_ += t0;
      xfrag.end().pt_ += t0; 

      // Translate this fragment around the image center and compute histogram
      // \TODO add randomness, rotation, and variation of phi, scale
      vgl_box_2d<double > bbox = xfrag.compute_bounding_box();

      // compute a list of locations to put the fragment at
      vcl_vector<vgl_vector_2d<double > > frag_loc_list;

      // use random translation
      {
        // do not sample when the image is too small compared to the fragment
        if (image_src.ni() <= 3*bbox.width()) continue;
        if (image_src.nj() <= 3*bbox.height()) continue;

        double max_tx = (image_src.ni() - 3*bbox.width()) / 2; 
        double min_tx = -max_tx;
        double max_ty = (image_src.nj() - 3*bbox.height()) / 2;
        double min_ty = -max_ty;

        
        for (unsigned k =0; k < 9; ++k)
        {
          double tx = rand_engine.drand32(min_tx, max_tx);
          double ty = rand_engine.drand32(min_ty, max_ty);
          frag_loc_list.push_back(vgl_vector_2d<double >(tx, ty));
        }
      }

      // iterate thru the list of location and collect BHOG
      for (unsigned k =0; k < frag_loc_list.size(); ++k)
      {
        vgl_vector_2d<double > t0 = frag_loc_list[k];
        dbsksp_xshock_fragment xfrag0 = xfrag;
        xfrag0.start().pt_ += t0;
        xfrag0.end().pt_ += t0;

        // compute boundary histogram of orientation gradient for this fragment and image
        vnl_vector<double > bhog;
        if (!dbsks_hog_boundary(bhog, Gx, Gy, xfrag0, 4, 8, 9))
        {
          vcl_cerr << "ERROR: failed to compute HOG of fragment boundary.\n";
          continue;
        }

        // record the feature vector
        bhog_list.push_back(bhog);

        //// write to output file
        //os << bhog << "\n";

        // draw a bounding box of this fragment on the image
        vgl_box_2d<double > bbox = xfrag0.compute_bounding_box();
        vgl_point_2d<int > top_left(vnl_math_rnd(bbox.min_x()), vnl_math_rnd(bbox.min_y()));
        vgl_point_2d<int > bot_right(vnl_math_rnd(bbox.max_x()), vnl_math_rnd(bbox.max_y()));
        
        // two horizontal lines
        for (int i = top_left.x(); i <= bot_right.x(); ++i)
        {
          int y0 = top_left.y();
          int y1 = bot_right.y();
          frags_on_image(i, y0, 0) = 255;
          frags_on_image(i, y0, 1) = 0;
          frags_on_image(i, y0, 2) = 0;

          frags_on_image(i, y1, 0) = 255;
          frags_on_image(i, y1, 1) = 0;
          frags_on_image(i, y1, 2) = 0;
        }

        // two vertical lines
        for (int j = top_left.y(); j <= bot_right.y(); ++j)
        {
          int x0 = top_left.x();
          int x1 = bot_right.x();
          frags_on_image(x0, j, 0) = 255;
          frags_on_image(x0, j, 1) = 0;
          frags_on_image(x0, j, 2) = 0;

          frags_on_image(x1, j, 0) = 255;
          frags_on_image(x1, j, 1) = 0;
          frags_on_image(x1, j, 2) = 0;
        }
      }
    } // i_frag
    
    // save the image
    vcl_string output_folder = vul_file::dirname(output_file);
    vcl_string out_image_file = output_folder + "/" + 
      vul_file::strip_extension(image_filenames[i_file]) + "+fragments.png";
    vil_save(frags_on_image, out_image_file.c_str());
  } // i_file


  // open output file for writing
  vcl_ofstream os(output_file.c_str(), vcl_ios_out);
  if (!os)
  {
    vcl_cout << " ERROR: Couldn't open for writing file: " << output_file << ".\n";
    return false;
  }

  for (unsigned k =0; k < bhog_list.size(); ++k)
  {
    os << bhog_list[k] << "\n";
  }

  os.close();
  return true;
}



// -----------------------------------------------------------------------------
//: Use a shape fragment's BHOG model and apply it to an image to detect the positive fragments
bool dbsks_detect_xfrag_using_bhog_model(const vcl_string& libsvm_xfrag_bhog_model_file,
                                         const vcl_string& xfrag_geom_file,
                                         const vcl_string& image_folder,
                                         const vcl_string& image_list_file,
                                         const vcl_string& output_file)
{
  // load svm model
  dbsks_xfrag_bhog_model bhog_model;
  if ( !bhog_model.load_from_file(libsvm_xfrag_bhog_model_file))
  {
    vcl_cout << "ERROR: couldn't load LIBSVM model file: " << libsvm_xfrag_bhog_model_file << "\n";
    return false;
  }
  
  // Load fragment geometry file
  vcl_vector<dbsksp_xshock_fragment > xfrag_list;
  vnl_file_matrix<double > geom_data(xfrag_geom_file.c_str());
  if (!geom_data)
  {
    vcl_cout << "ERROR: couldn't load xfrag_geom_file: " << xfrag_geom_file << "\n";
  }


  //vnl_random rand_engine(1010892);
  //vcl_vector<vnl_vector<double > > pos_xfrags;

  // Parse the image names and process each imag separately
  vcl_vector<vcl_string > image_filenames;
  dbul_parse_string_list(image_list_file, image_filenames);
  //for (unsigned i_file =0; i_file < image_filenames.size(); ++i_file)
  for (unsigned i_file =0; i_file < 1; ++i_file)
  {
    // load the image
    vcl_string image_file = image_folder + "/" + image_filenames[i_file];

    vcl_cout << "Loading image: " << image_file << " ... ";
    vil_image_view<double > image_src = *vil_convert_cast(double(), vil_load(image_file.c_str()));
    
    if (!image_src) vcl_cout << "Failed.\n";
    else vcl_cout << "Succeeded.\n";

    // Compute gradient of color image by taking strongest component of the three color planes
    vil_image_view<double > Gx, Gy;
    dbil_gradient_rgb_Linf(image_src, Gx, Gy);
    vgl_point_2d<double > image_center(image_src.ni() /2 , image_src.nj() / 2);

    // Now for each fragment, we sample the BHOG at various locations, scale,
    // orientations, etc, and use the xfrag_bhog_model to test whether it is a positive profile

    // All bhog for a fragment is saved in a long vector
    int num_bhog_per_pos_xfrag = 9;
    vcl_vector<vnl_vector<double > > bhog_list;
    vcl_vector<vnl_vector<double > > xfrag_list;

    //int count = 0;

    for (unsigned i_frag =0; i_frag < geom_data.rows(); ++i_frag)
    {
      vcl_cout << "i_frag = " << i_frag << vcl_endl;

      dbsksp_xshock_fragment orig_xfrag;
      double graph_area;
      dbsks_convert_from_vector(geom_data.get_row(i_frag), orig_xfrag, graph_area);

      // translate this fragment to the image center
      {
        vgl_point_2d<double > xfrag_center = centre(orig_xfrag.start().pt(), orig_xfrag.end().pt());
        orig_xfrag.translate(image_center - xfrag_center);
      }
      vgl_box_2d<double > orig_bbox = orig_xfrag.compute_bounding_box();

      if (image_src.ni() <= 3*orig_bbox.width()) continue;
      if (image_src.nj() <= 3*orig_bbox.height()) continue;

      double step_x = 8;
      double step_y = 8;
      double step_theta = vnl_math::pi / 18;

      int half_num_x = vnl_math_rnd((image_src.ni() - 3*orig_bbox.width()) / (2*step_x));
      int half_num_y = vnl_math_rnd((image_src.nj() - 3*orig_bbox.height()) / (2*step_y));

      // perturb this fragment to get variation
      double delta_x = 4;
      double delta_y = 4;
      double delta_phi = vnl_math::pi / 24;
      double delta_psi = vnl_math::pi / 16 / 2;
      double delta_radius = 2;

      int num_segments_per_bnd_side = 4;
      double patch_width = 8; // width the arc patch
      int num_bins = 9;

      for (int i_x = -half_num_x; i_x <= half_num_x; ++i_x)
      {
        for (int i_y = -half_num_y; i_y <= half_num_y; ++i_y)
        {
          // translate the fragment to this location
          vgl_vector_2d<double > v0(i_x * step_x, i_y * step_y);
          dbsksp_xshock_fragment xfrag_base = orig_xfrag;
          xfrag_base.translate(v0);

          

          for (int k =0; k < num_bhog_per_pos_xfrag; ++k)
          {
            // angle and center of rotation
            double dtheta = (k - 0.5*(num_bhog_per_pos_xfrag-1)) * step_theta;
            vgl_point_2d<double > chord_center = centre(xfrag_base.start_shock_pt(), xfrag_base.end_shock_pt());

            dbsksp_xshock_fragment xfrag = xfrag_base;
            xfrag.rotate(chord_center, dtheta);

            // compute boundary histogram of orientation gradient for this fragment and image
            bhog_list.push_back(vnl_vector<double >());
            if (!dbsks_hog_boundary(bhog_list.back(), Gx, Gy, xfrag, num_segments_per_bnd_side, patch_width, num_bins))
            {
              vcl_cerr << "ERROR: failed to compute HOG of fragment boundary.\n";
              bhog_list.pop_back();
              continue;
            }

            // save the fragment geometry too
            xfrag_list.push_back(vnl_vector<double > ());
            dbsks_convert_to_vector(xfrag, graph_area, xfrag_list.back());

            //// <<<< DEBUG ===============

            //++count;
            //if (count == 12790)
            //{
            //  vnl_vector<double > bhog;
            //  dbsks_hog_boundary(bhog, Gx, Gy, xfrag, num_segments_per_bnd_side, patch_width, num_bins);
            //  // classify this fragment
            //  double score;
            //  if (!bhog_model.compute_score(bhog, score))
            //    continue;
            //  
            //  if (score > 0.5)
            //  {
            //    pos_xfrags.push_back(vnl_vector<double > ());
            //    dbsks_convert_to_vector(xfrag, graph_area, pos_xfrags.back());
            //  }
            //}
            

            //vnl_vector<double > xx;
            //dbsks_convert_to_vector(xfrag, graph_area, xx);
            //if (vnl_math_abs(xx[0] - 80.49) < 0.1 && 
            //  vnl_math_abs(xx[1] - 92.11) < 0.1 && 
            //  vnl_math_abs(xx[5] - 55.51) < 0.1 && 

            //  vnl_math_abs(xx[6] - 103.88) < 0.1)
            //{
            //  vnl_vector<double > bhog;
            //  dbsks_hog_boundary(bhog, Gx, Gy, xfrag, num_segments_per_bnd_side, patch_width, num_bins);
            //  // classify this fragment
            //  double score;
            //  if (!bhog_model.compute_score(bhog, score))
            //    continue;
            //  
            //  if (score > 0.5)
            //  {
            //    pos_xfrags.push_back(vnl_vector<double > ());
            //    dbsks_convert_to_vector(xfrag, graph_area, pos_xfrags.back());
            //  }
            //}

            //// >>>> ================================

          } // k - num_bhog
        } // i_y
      } // i_x
    } // i_frag

    // Compute all the score at the same time
    vcl_vector<double > score_list;
    bhog_model.compute_score(bhog_list, score_list);
    
    // Print out results
    vcl_string output_basename = output_file + "+" +
      vul_file::strip_extension(image_filenames[i_file]);

    // print the bhog matrix to file
    vcl_string bhog_file = output_basename + ".bhog";
    dbsks_write_to_file(bhog_file, bhog_list);

    // print the fragment geometry to file
    vcl_string xfrag_file = output_basename + ".xfrag";
    dbsks_write_to_file(xfrag_file, xfrag_list);

    // print the prediction to a file
    vcl_string predict_file = output_basename + ".predict";
    dbsks_write_to_file(predict_file, score_list);
  } // i_file

  return true;
}





//// -----------------------------------------------------------------------------
////: Extract OCM cost
//bool dbsks_extract_xgraph_gray_ocm_cost(const vcl_string& image_list_file,
//                                   const vcl_string& xshock_folder,
//                                   const vcl_string& xshock_extension,
//                                   const vcl_string& image_folder,
//                                   const vcl_string& image_extension,
//                                   const vcl_string& edgemap_folder,
//                                   const vcl_string& edgemap_extension,
//                                   int root_vid, 
//                                   int major_child_eid,
//                                   float ocm_tol_near_zero,
//                                   const vcl_string& output_file)
//{
//  // parameters
//  float ocm_edge_threshold = 15;
//  float ocm_distance_threshold = 8;              
//  int nchannel = 18; 
//  float ocm_lambda = 0.3f;
//  float ocm_gamma = 0.4f;
//
//  bool option_compute_gray_ocm_cost = false;
//  bool option_compute_ccm_cost = true;
//
//
//  // >>> parse input file containing list of xshock graphs
//  vcl_vector<vcl_string > image_filenames;
//  if (!dbul_parse_string_list(image_list_file, image_filenames))
//  {
//    vcl_cout << "ERROR: couldn't load image list file:" << image_list_file << vcl_endl;
//    return false;
//  }
//
//  // Place holder for collected cost - each vector element is ocm cost of a xgraph
//  vcl_vector<vnl_vector<float > > list_ocm_cost;
//
//  for (unsigned index =0; index < image_filenames.size(); ++index)
//  {
//    vcl_string object_name = vul_file::strip_extension(image_filenames[index]);
//
//    //////////////////////////////////////////////////////////////////////////////
//    // Load the shock graph 
//    vcl_string xgraph_file = xshock_folder + "/" + object_name + xshock_extension;
//    dbsksp_xshock_graph_sptr xgraph = 0;
//
//    vcl_cout << "Loading xshock_graph XML file: " << xgraph_file << "...";
//    if ( x_read(xgraph_file, xgraph) )
//    {
//      vcl_cout << "Succeeded.\n";
//      xgraph->compute_vertex_depths(root_vid);
//    }
//    else
//    {
//      vcl_cout << "Failed.\n";
//      continue;
//    }
//
//    //////////////////////////////////////////////////////////////////////////////
//    // Load the image
//    vcl_string image_file = image_folder +"/"+ object_name + image_extension;
//    vcl_cout << "Loading image file: " << image_file << "...";
//
//    vil_image_resource_sptr image_resource = vil_load_image_resource(image_file.c_str());
//    if (image_resource)
//    {
//      vcl_cout << " Succeeded.\n";
//    }
//    else
//    {
//      vcl_cout << " Failed.\n";
//      continue;
//    }
//    vil_image_view<float > image_view = *vil_convert_cast(float(), image_resource->get_view());
//
//    //////////////////////////////////////////////////////////////////////////////
//    // Load the edgemap
//    vcl_string edgemap_file = edgemap_folder + "/" + object_name + edgemap_extension;
//    vcl_cout << "Loading edgemap file: " << image_file << "...";
//
//    vil_image_resource_sptr edgemap_resource = vil_load_image_resource(edgemap_file.c_str());
//    if (edgemap_resource)
//    {
//      vcl_cout << " Succeeded.\n";
//    }
//    else
//    {
//      vcl_cout << " Failed.\n";
//      continue;
//    }
//    vil_image_view<float > edgemap_view = *vil_convert_cast(float(), edgemap_resource->get_view());
//
//    //////////////////////////////////////////////////////////////////////////////
//    // Collect OCM cost for the shock graph
//    vcl_cout << "Prepare gray-OCM cost grid: ...";
//
//    dbsks_gray_ocm gray_ocm;
//    gray_ocm.set_image(image_view);
//    gray_ocm.set_edgemap(edgemap_view, ocm_edge_threshold, 255);
//    gray_ocm.set_lambda(ocm_lambda);
//    gray_ocm.set_ocm_params(ocm_distance_threshold, ocm_tol_near_zero, nchannel);
//    gray_ocm.compute();
//    vcl_cout << "Done." << vcl_endl;
//
//    //////////////////////////////////////////////////////////////////////////////
//    vcl_cout << "\nConstructing a biarc sampler ...";
//
//    // Set parameters of biarc sampler
//    dbsks_biarc_sampler_params bsp;
//    bsp.step_dx = 8;
//    bsp.num_dx = 29;
//    bsp.min_dx = -bsp.step_dx * (bsp.num_dx-1)/2;
//
//    bsp.step_dy = 8;
//    bsp.num_dy = 29;
//    bsp.min_dy = -bsp.step_dy * (bsp.num_dy-1)/2;
//
//    bsp.step_alpha0 = vnl_math::pi/17;
//    bsp.num_alpha0 = 9;
//    bsp.min_alpha0 = -bsp.step_alpha0*(bsp.num_alpha0-1)/2;
//
//    bsp.step_alpha2 = vnl_math::pi/17;
//    bsp.num_alpha2 = 9;
//    bsp.min_alpha2 = -bsp.step_alpha2*(bsp.num_alpha2-1)/2;
//
//    // Define a biarc sampler
//    dbsks_biarc_sampler biarc_sampler;
//
//    // compute coordinates of the grid points
//    biarc_sampler.set_grid(bsp);
//
//    // compute prototype sample points for all biarcs in the grid
//    double ds = 2;
//    int num_bins = 36;
//    biarc_sampler.set_sampling_params(num_bins, ds);
//    biarc_sampler.compute_cache_sample_points();
//    vcl_cout << "Done" << vcl_endl;
//
//
//    //////////////////////////////////////////////////////////////////////////////
//    vcl_cout << "\nCompute gray-OCM cost of each fragment of the xgraph...";
//
//    // create a record
//    list_ocm_cost.push_back(vnl_vector<float >());
//    vnl_vector<float >& ocm_cost = list_ocm_cost.back();
//    ocm_cost.set_size(xgraph->number_of_edges()*2);
//    vnl_vector<float >::iterator ocm_iter = ocm_cost.begin();
//
//    vcl_vector<int > x_vec, y_vec, angle_vec;
//    for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit !=
//      xgraph->edges_end(); ++eit)
//    {
//      dbsksp_xshock_edge_sptr xe = *eit;
//
//      unsigned eid = xe->id();
//
//      dbsksp_xshock_node_sptr xv_p, xv_c;
//      if (xe->source()->depth() < xe->target()->depth())
//      {
//        xv_p = xe->source();
//        xv_c = xe->target();
//      }
//      else
//      {
//        xv_p = xe->target();
//        xv_c = xe->source();
//      }
//      assert(xv_p->depth() < xv_c->depth());
//
//      dbsksp_xshock_node_descriptor start = *(xv_p->descriptor(xe));
//      dbsksp_xshock_node_descriptor end = xv_c->descriptor(xe)->opposite_xnode();
//
//      // Compute OCM cost
//      dbsks_compute_ocm_cost_biarc(&gray_ocm, &biarc_sampler, start.bnd_pt_left(), start.bnd_tangent_left(), 
//        end.bnd_pt_left(), end.bnd_tangent_left(), *ocm_iter);
//      ++ocm_iter;
//
//      dbsks_compute_ocm_cost_biarc(&gray_ocm, &biarc_sampler, start.bnd_pt_right(), start.bnd_tangent_right(), 
//        end.bnd_pt_right(), end.bnd_tangent_right(), *ocm_iter);
//      ++ocm_iter;
//    }
//  }
//
//  // Save the result to a file
//  vcl_string gray_ocm_output_file = output_file + "_gray_ocm.txt";
//  vcl_ofstream os(gray_ocm_output_file.c_str(), vcl_ios::out);
//  os << "#Gray-OCM cost\n"
//    << "#tol_near_zero " << ocm_tol_near_zero << "\n"
//    << "#edge_threshold " << ocm_edge_threshold << "\n"
//    << "#distance_threshold " << ocm_distance_threshold << "\n"
//    << "#ocm_lambda " << ocm_lambda << "\n";
//  for (unsigned i =0; i < list_ocm_cost.size(); ++i)
//  {
//    os << list_ocm_cost[i] << "\n";
//  }
//  os.close();
//
//
//  return true;
//
//
//}
//
//



















// -----------------------------------------------------------------------------
//: Save a list of features to a file
bool dbsks_write_to_file(const vcl_string& filename, const vcl_vector<vnl_vector<double > >& data)
{
  // open output file for writing
  vcl_ofstream os(filename.c_str(), vcl_ios_out);
  if (!os)
  {
    vcl_cout << " ERROR: Couldn't open for writing file: " << filename << "\n";
    return false;
  }

  for (unsigned k =0; k < data.size(); ++k)
  {
    os << data[k] << "\n";
  }

  os.close();
  return true;
}






//: Save a list of numbers to a file
bool dbsks_write_to_file(const vcl_string& filename, const vcl_vector<double >& data)
{
  // open output file for writing
  vcl_ofstream os(filename.c_str(), vcl_ios_out);
  if (!os)
  {
    vcl_cout << " ERROR: Couldn't open for writing file: " << filename << "\n";
    return false;
  }

  for (unsigned k =0; k < data.size(); ++k)
  {
    os << data[k] << "\n";
  }

  os.close();
  return true;
}






















// -----------------------------------------------------------------------------
//: Extend the front and rear legs of giraffe models
bool dbsks_extend_giraffe_legs()
{
  //>> input data

  // original folder for giraffe model
  vcl_string old_giraffe_xgraph_folder = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/giraffes-xgraph";
  
  // new folder for giraffe model
  vcl_string new_giraffe_xgraph_folder = "D:/vision/data/ETHZ-shape/giraffe-xgraph-extended-legs";

  // list of xshock files
  vcl_string xshock_list_file = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/list_giraffes_xml.txt";


  //>> Process

  // parse input file
  vcl_vector<vcl_string > xml_filenames;
  dbul_parse_string_list(xshock_list_file, xml_filenames);

  // Go thru each shock graph and add front and rear legs to it
  vcl_vector<vnl_vector<double > > frag_geom_list;
  for (unsigned index =0; index < xml_filenames.size(); ++index)
  {
    // Load the shock graph 
    vcl_string old_xml_file = old_giraffe_xgraph_folder + "/" + xml_filenames[index];
    dbsksp_xshock_graph_sptr xgraph = 0;

    vcl_cout << "Loading xshock_graph XML file: " << old_xml_file << "...";
    if ( x_read(old_xml_file, xgraph) )
    {
      vcl_cout << "Succeeded.\n";
    }
    else
    {
      vcl_cout << "Failed.\n";
      continue;
    }

    // extend the rear legs
    unsigned rear_eid1 = 30;
    dbsksp_xshock_edge_sptr rear_xe1 = xgraph->edge_from_id(rear_eid1);
    double rear_length = (rear_xe1->source()->pt()-rear_xe1->target()->pt()).length();

    unsigned rear_eid2 = 27;
    dbsksp_xshock_edge_sptr rear_xe2 = xgraph->edge_from_id(rear_eid2);

    xgraph->insert_xshock_node_at_terminal_edge(rear_xe2, rear_length);

    // extend the front legs
    unsigned front_eid1 = 15;
    dbsksp_xshock_edge_sptr front_xe1 = xgraph->edge_from_id(front_eid1);
    double front_length = (front_xe1->source()->pt()-front_xe1->target()->pt()).length();

    unsigned front_eid2 = 16;
    dbsksp_xshock_edge_sptr front_xe2 = xgraph->edge_from_id(front_eid2);

    xgraph->insert_xshock_node_at_terminal_edge(front_xe2, front_length);

    // finalize
    xgraph->update_all_degree_1_nodes();

    // save new xgraph
    vcl_string new_xml_file = new_giraffe_xgraph_folder + "/" + xml_filenames[index];
    x_write(new_xml_file, xgraph);
  }
  return true;
}

















// -----------------------------------------------------------------------------
//: Re-compute xgraph bbox and rewrite xml detection files
// The old detection code scaled the xgraph down but didn't recompute the bbox
// This yield wrong evaluation result. This script is to fix that
bool dbsks_recompute_detection_bbox()
{
  // input data
  vcl_string xml_header_file = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-experiments/xml_det_header.txt";
  vcl_string xml_footer_file = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-experiments/xml_det_footer.txt";

  //vcl_string orig_exp_folder = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-experiments";
  //vcl_string orig_exp_folder = "D:/vision/projects/symseg/xshock/vox-output/2009-feb-23-0222pm-dragon";
  vcl_string orig_exp_folder = "D:/vision/projects/symseg/xshock/xshock-experiments";
  
  vcl_string dest_exp_folder = "D:/vision/projects/symseg/xshock/xshock-experiments";
  //vcl_string exp_name = "xgraph-scale-80";
  //vcl_string exp_name = "exp_36-all_giraffes-v0_69-fixed-log0-I_15-len_10";
  vcl_string exp_name = "exp_40-all_images-v0_69-fixed-log0-I_15-len_10";

  //////////////////////////////////////////////////////////////////////////////

  //>> some announcement
  vcl_cout << "\n>> Original experiment folder = " << orig_exp_folder << vcl_endl;
  vcl_cout << "\n>> Destination experiment folder = " << dest_exp_folder << vcl_endl;
  vcl_cout << "\n>> Experiment name = " << exp_name << vcl_endl;
  vcl_cout << "\n-------------------------------------------------------------\n\n";

  //>> Create folder for output, if necessary
  vcl_string orig_exp_path = orig_exp_folder + "/" + exp_name;
  vcl_string dest_exp_path = dest_exp_folder + "/" + exp_name;

  if (!vul_file::is_directory(dest_exp_path))
  {
    vul_file::make_directory(dest_exp_path);
  }

  // process each file in original experiment folder
  
  vcl_vector<dbsks_xshock_det_record_sptr > all_xshock_dets;
  for (vul_file_iterator fn= (orig_exp_path + "/xml_det_record+*"); fn; ++fn) 
  {
    vcl_string xml_record_file = fn();
    vcl_cout << "\n>> Processing xml_record_file = " << xml_record_file << vcl_endl;

    // Add a header and footer to this xml file to make it complete. Output to a temp file
    vcl_string temp_xml_file = orig_exp_folder + "/" + "temp_det.xml";
    vcl_ofstream os(temp_xml_file.c_str());
    dbsks_append_text_file(os, xml_header_file);
    dbsks_append_text_file(os, xml_record_file);
    dbsks_append_text_file(os, xml_footer_file);
    os.close();

    // Load the temp file to get the list of detection
    vcl_vector<dbsks_xshock_det_record_sptr > xshock_det_list;
    x_read(temp_xml_file, xshock_det_list);
    vul_file::delete_file_glob(temp_xml_file.c_str());

    all_xshock_dets.insert(all_xshock_dets.end(), xshock_det_list.begin(), xshock_det_list.end());

    //// write the detection record back out
    //vcl_string output_xml_record_file = dest_exp_path + "/" +
    //  vul_file::strip_directory(xml_record_file);
  }

  vcl_cout << "\n>> Recomputing bounding boxes of xgraph detections ...\n";
  // Update the bounding boxes and scale in each detection record
  for (unsigned m =0; m < all_xshock_dets.size(); ++m)
  {
    vcl_cout << " " << m;
    dbsks_xshock_det_record_sptr det_record = all_xshock_dets[m];

    // filename of the xgraph xml
    vcl_string xgraph_filename = "";
    det_record->get_value("xgraph_xml", xgraph_filename);

    // full path to the xgraph file
    vcl_string xgraph_file = orig_exp_path + "/" + xgraph_filename;

    // Load the xgraph
    dbsksp_xshock_graph_sptr xgraph = 0;
    x_read(xgraph_file, xgraph);

    if (!xgraph)
    {
      vcl_cout << "\nERROR: couldn't load xgraph file " << xgraph_file << vcl_endl;
      continue;
    }

    double xgraph_scale = vcl_sqrt(xgraph->area());
    vsol_box_2d_sptr bbox = xgraph->bounding_box();
    
    det_record->set_value("xgraph_scale", xgraph_scale);
    det_record->set_value("bbox_xmin", bbox->get_min_x());
    det_record->set_value("bbox_ymin", bbox->get_min_y());
    det_record->set_value("bbox_xmax", bbox->get_max_x());
    det_record->set_value("bbox_ymax", bbox->get_max_y());  
  }
  vcl_cout << ".Done.\n";

  

  // output file
  vcl_string output_xml_file = dest_exp_path + "/" + "xml_det.xml";

  vcl_cout << "\n>> Saving final detection-record file to " << output_xml_file << " ... ";
  if (x_write(output_xml_file, all_xshock_dets))
  {
    vcl_cout << "Succeeded.\n";
  }
  else
  {
    vcl_cout << "Failed.\n";
  }
  
  return true;
}







