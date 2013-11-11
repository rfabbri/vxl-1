// This is /lemsvxl/contrib/firat/dbsks_xgraph_likelihood_calculator/dbsks_xgraph_likelihood_calculator_utils.cxx.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Feb 24, 2011
//

#include "dbsks_xgraph_likelihood_calculator_utils.h"

#include <vcl_utility.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include<dbsks/dbsks_xgraph_ccm_model.h>
#include<dbsks/algo/dbsks_load.h>
#include<dbdet/algo/dbdet_resize_edgemap.h>
#include<dbsksp/dbsksp_xshock_graph.h>
#include <vsol/vsol_box_2d.h>


bool dbsks_xgraph_likelihood_calculator_utils::
load_edgemap_pyramid(vcl_vector<vcl_string >& list_edgemap_base_name,
        vcl_vector<unsigned >& list_edgemap_width,
        vcl_vector<double >& list_edgemap_scale,
        const vcl_string& edgemap_folder,
        const vcl_string& edgemap_ext,
        const vcl_string& object_id,
        double image_width)
{
    //>> Load all edgemaps in the pyramid ........................................
    vcl_cout << "\n>> Loading all edgemap images in the pyramid ... ";

    // regular expression to iterate thru edgemap files
    vcl_string edgemap_regexp = edgemap_folder + "/" + object_id +  "/" + object_id + "*" + edgemap_ext;

    // clean up any existing data
    list_edgemap_base_name.clear();
    list_edgemap_width.clear();
    list_edgemap_scale.clear();




    // sort the edgemaps by their width, decreasing order
    vcl_map<int, edgemap_level_info> map_width2info;
    for (vul_file_iterator fn = edgemap_regexp; fn; ++fn)
    {
        vil_image_resource_sptr img = vil_load_image_resource(fn());
        if (img)
        {
            edgemap_level_info info;
            info.width = int(img->ni());
            info.scale = img->ni() / image_width;
            // base name: computed by taking away the "edgemap_ext" part of file name
            // note that "vul_file::strip_extension(...) will not work because
            // "edgemap_ext" may containt a dot ".", which will confuse the
            // vul_file::strip_extension(...) function
            vcl_string fname = vul_file::strip_directory(fn());
            info.base_name = fname.substr(0, fname.size()-edgemap_ext.size());
            map_width2info.insert(vcl_make_pair(-info.width, info));
        }
    }

    // put the info back in the form we're familiar with
    for (vcl_map<int, edgemap_level_info>::iterator iter = map_width2info.begin();
            iter != map_width2info.end(); ++iter)
    {
        edgemap_level_info info = iter->second;
        list_edgemap_width.push_back(info.width);
        list_edgemap_scale.push_back(info.scale);
        list_edgemap_base_name.push_back(info.base_name);
    }
    return true;
}

//------------------------------------------------------------------------------
//: Load edgemap corresponding to a target xgraph size to that the actual
// xgraph size is about the same as the base xgraph size
dbsks_xshock_ccm_likelihood* dbsks_xgraph_likelihood_calculator_utils::
get_ccm_like_keeping_graph_size_fixed(dbsksp_xshock_graph_sptr& input_xgraph,
        double target_xgraph_size,
        double& actual_pyramid_scale,
        dbdet_edgemap_sptr& actual_edgemap,
        dbsksp_xshock_graph_sptr& actual_xgraph,
        dbsks_xgraph_ccm_model_sptr& xgraph_ccm,
        vcl_vector<vcl_string >& list_edgemap_base_name,
        vcl_vector<double >& list_edgemap_scale,
        const vcl_string& edgemap_folder,
        const vcl_string& edgemap_ext,
        const vcl_string& edgeorient_ext,
        const vcl_string& object_id,
        double edgemap_log2_scale_ratio,
        dbsks_biarc_sampler* biarc_sampler_ptr,
        vcl_map<int, dbsks_xshock_ccm_likelihood*>& ccm_like_db,
        int precision,
        double cut_off_target_pyramid_scale)
{
    // Compute image scale (compared to base image) such that we can maintain the same graph/image ratio
    // while keeping the graph scale at the base level
    double target_pyramid_scale = xgraph_ccm->base_xgraph_size() / target_xgraph_size; //

    // Set the actual scale to be the target scale
    // note that this may result in enlarging the edgemap when the target scale > 1
    actual_pyramid_scale = target_pyramid_scale;

    int map_index = precision*actual_pyramid_scale;
    vcl_cout << "MAP INDEX: " << map_index << vcl_endl;
    if(map_index > cut_off_target_pyramid_scale*precision)
    {
        return 0;
    }
    vcl_map<int, dbsks_xshock_ccm_likelihood*>::iterator elem_it = ccm_like_db.find(map_index);
    if(elem_it == ccm_like_db.end())
    {

        unsigned actual_edgemap_level;
        double actual_edgemap_scale;
        {
            // scale of the "edgemap" which is different from the "image" scale by "edgemap_log2_scale_ratio"
            double scale_ratio = vcl_pow(2.0, edgemap_log2_scale_ratio);
            double target_edgemap_scale = target_pyramid_scale * scale_ratio;

            // find the edgemap closest to this scale
            vnl_vector<double > scale_diff(list_edgemap_scale.size(), vnl_numeric_traits<double >::maxval);
            for (unsigned k =0; k < scale_diff.size(); ++k)
            {
                scale_diff[k] = vnl_math_abs(vcl_log(list_edgemap_scale[k] / target_edgemap_scale));
            }
            actual_edgemap_level = scale_diff.arg_min();
            actual_edgemap_scale = list_edgemap_scale[actual_edgemap_level];
            if (actual_edgemap_scale == vnl_numeric_traits<double >::maxval)
            {
                return 0;
            }
        }

        // base-name for edge-related files
        vcl_string base_name = list_edgemap_base_name[actual_edgemap_level];

        // edgemap file
        vcl_string edgemap_fname = base_name + edgemap_ext;
        vcl_string edgemap_file = edgemap_folder + "/" + object_id + "/" + edgemap_fname;

        // Name of edge orientation file
        vcl_string edgeorient_fname = base_name + edgeorient_ext;
        vcl_string edgeorient_file = edgemap_folder + "/" + object_id + "/" + edgeorient_fname;

        // Load the edgel map/////////////////////////////////////////////////////////
        actual_edgemap = dbsks_load_subpix_edgemap(edgemap_file, edgeorient_file, 15.0f, 255.0f);

        // re-scale the edgemap to the level of image in the pyramid
        dbdet_resize_edgemap(actual_edgemap, actual_pyramid_scale / actual_edgemap_scale);
        //////////////////////////////////////////////////////////////////////////////

        //> Adjust the size of the model xgraph

        // Because the edgemap pyramid is discrete, the actual loaded edgemap does
        // not neccessarily have the same scale as the target scale. Need to make
        // adjustment on the size of the xgraph:

        // Scale the xgraph up (or down) so that:
        //   size of adjusted xgraph                target_xgraph_size
        // ------------------------------- = ---------------------------
        //  size of actual pyramid image            base image (=1)

        //////////////////////////////////////////////////////////////////////////////
        actual_xgraph = input_xgraph;
        double adjusted_xgraph_size = target_xgraph_size * actual_pyramid_scale;
        double cur_xgraph_size = vcl_sqrt(actual_xgraph->area());
        actual_xgraph->scale_up(0, 0, adjusted_xgraph_size / cur_xgraph_size);
        /*vsol_box_2d_sptr bbox_sptr = actual_xgraph->bounding_box();
        double min_x = bbox_sptr->get_min_x()-5;
        if (min_x >= actual_edgemap->width()-1)
        {
            return 0;
        }
        if (min_x < 0)
        {
            min_x = 0;
        }
        double max_x = bbox_sptr->get_max_x()+5;
        if (max_x <= 0)
        {
            return 0;
        }
        if (max_x > actual_edgemap->width()-1)
        {
            max_x = actual_edgemap->width()-1;
        }
        double min_y = bbox_sptr->get_min_y()-5;
        if (min_y >= actual_edgemap->height()-1)
        {
            return 0;
        }
        if (min_y < 0)
        {
            min_y = 0;
        }
        double max_y = bbox_sptr->get_max_y()+5;
        if (max_y <= 0)
        {
            return 0;
        }
        if (max_y > actual_edgemap->height()-1)
        {
            max_y = actual_edgemap->height()-1;
        }*/
        
        
        
        //////////////////////////////////////////////////////////////////////////////

        dbsks_xshock_ccm_likelihood* ccm_like = new dbsks_xshock_ccm_likelihood;
        ccm_like->set_biarc_sampler(biarc_sampler_ptr);
        ccm_like->set_ccm_model(xgraph_ccm);
        ccm_like->set_edgemap(actual_edgemap);
        vgl_box_2d<int > window(0, actual_edgemap->width()-1, 0, actual_edgemap->height()-1);
        //vgl_box_2d<int > window(min_x, max_x, min_y, max_y);
        if(ccm_like->compute_internal_data(window))
        {
            ccm_like_db[map_index] = ccm_like;
            return ccm_like;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return elem_it->second;
    }
}
