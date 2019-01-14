// This is /lemsvxl/contrib/firat/dbsks_xgraph_likelihood_calculator/dbsks_xgraph_likelihood_calculator.cxx.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Feb 21, 2011
//

#include "dbsks_xgraph_likelihood_calculator_utils.h"

#include<iostream>
#include<fstream>
#include<sstream>
#include<cstdlib>
#include<dbsks/algo/dbsks_load.h>
#include<dbsks/dbsks_xgraph_ccm_model.h>
#include<dbsks/dbsks_xgraph_ccm_model_sptr.h>
#include<dbsks/dbsks_xshock_ccm_likelihood.h>
#include<dbsks/dbsks_biarc_sampler.h>
#include<dbdet/edge/dbdet_edgemap_sptr.h>
#include<dbdet/edge/dbdet_edgemap.h>
#include <vsol/vsol_box_2d.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include<dbsksp/dbsksp_xshock_edge_sptr.h>
#include<dbsksp/dbsksp_xshock_edge.h>
#include<dbsksp/dbsksp_xshock_graph.h>

// args[1] : ccm_model_file
// args[2] : ccm_param_file
// args[3] : image name
// args[4] : edgemap folder
// args[5] : hypothesis file
// args[6] : likelihood file

int main(int argn, char* args[])
{
    //std::string ccm_model_file = "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/bottles/bottles-ccm-model-rv4-11082010.xml";
    std::string ccm_model_file = args[1];
    //std::string ccm_parameter_file = "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/ccm_params_orient45_dist4.xml";
    std::string ccm_parameter_file = args[2];
    //std::string edgemap_file = "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_pb_edges_pyramid-clean_using_kovesi-I_15-len_4/bottles_dry2/bottles_dry2_00_pb_edges.png";
    std::string image_name = args[3];
    //std::string edgeorient_file = "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_pb_edges_pyramid-clean_using_kovesi-I_15-len_4/bottles_dry2/bottles_dry2_00_pb_orient.txt";
    std::string edgemap_folder = args[4];
    std::string edgemap_ext = args[5];
    std::string edgeorient_ext = args[6];
    double edgemap_log2_scale_ratio = std::atof(args[7]);
    int image_width = std::atoi(args[8]);
    int precision = std::atoi(args[9]);
    double cut_off_target_pyramid_scale = std::atof(args[10]);
    std::string hypothesis_file = args[11];
    std::string likelihood_file = args[12];

    //Load pyramid edgemap
    std::cout << "Load pyramid edgemap..." << std::endl;
    std::vector<std::string > list_edgemap_base_name;
    std::vector<unsigned > list_edgemap_width;
    std::vector<double > list_edgemap_scale;


    if(dbsks_xgraph_likelihood_calculator_utils::load_edgemap_pyramid(list_edgemap_base_name,
            list_edgemap_width,
            list_edgemap_scale,
            edgemap_folder,
            edgemap_ext,
            image_name,
            image_width))
    {
        std::cout << "[OK]" << std::endl;
    }
    else
    {
        std::cout << "[FAILED]" << std::endl;
        return 1;
    }

    //Load ccm model + build biarc sampler
    std::cout << "Load ccm model..." << std::endl;
    dbsks_xgraph_ccm_model_sptr xgraph_ccm;
    dbsks_load_xgraph_ccm_model(ccm_model_file, ccm_parameter_file , xgraph_ccm);

    std::cout << "Build biarc sampler..." <<std::endl;
    dbsks_biarc_sampler biarc_sampler;
    xgraph_ccm->build_biarc_sampler(biarc_sampler);
    biarc_sampler.compute_cache_sample_points();
    biarc_sampler.compute_cache_nkdiff();



    vgl_point_2d<double> pt(0,0);
    std::string prev_xgraph_file = "nofile";
    std::string line;
    std::ifstream ifs(hypothesis_file.c_str());
    std::ofstream ofs(likelihood_file.c_str());
    dbsksp_xshock_graph_sptr orig_xgraph;
    std::map<int, dbsks_xshock_ccm_likelihood*> ccm_like_db;

    if(ifs.is_open())
    {
        while(ifs.good())
        {
            getline(ifs, line);
            if(line.length() == 0)
            {
                break;
            }
            std::stringstream ss(std::stringstream::in | std::stringstream::out);
            std::string xg_file;
            double tx, ty, angle, scale;
            ss << line;
            ss >> xg_file;
            ss >> tx;
            ss >> ty;
            ss >> angle;
            ss >> scale;
            if(xg_file != prev_xgraph_file)
            {
                prev_xgraph_file = xg_file;
                std::cout << "Reading " << xg_file << " ..." << std::endl;
                x_read(xg_file, orig_xgraph);
            }
            dbsksp_xshock_graph_sptr xgraph = new dbsksp_xshock_graph(*orig_xgraph);
            //std::cout << "Parameters: " << tx << " " << ty << " " << angle << " " << scale << std::endl;
            xgraph->similarity_transform2(pt, tx, ty, angle, scale);

            double actual_pyramid_scale;
            dbdet_edgemap_sptr actual_edgemap;
            dbsksp_xshock_graph_sptr actual_xgraph;

            dbsks_xshock_ccm_likelihood* ccm_like =
                    dbsks_xgraph_likelihood_calculator_utils::get_ccm_like_keeping_graph_size_fixed(xgraph,
                    std::sqrt(xgraph->area()),
                    actual_pyramid_scale,
                    actual_edgemap,
                    actual_xgraph,
                    xgraph_ccm,
                    list_edgemap_base_name,
                    list_edgemap_scale,
                    edgemap_folder,
                    edgemap_ext,
                    edgeorient_ext,
                    image_name,
                    edgemap_log2_scale_ratio,
                    &biarc_sampler,
                    ccm_like_db,
                    precision,
                    cut_off_target_pyramid_scale);
            if(ccm_like)
            {
                std::list<dbsksp_xshock_edge_sptr>::iterator it = xgraph->edges_begin();
                double total_likelihood = 0;
                for(; it != xgraph->edges_end(); it++)
                {
                    dbsksp_xshock_edge_sptr edge = *it;
                    double frag_likelihood = ccm_like->loglike(edge->id(), *edge->fragment());
                    total_likelihood += frag_likelihood;
                    //std::cout << "The cost of frag " << edge->id() << " is " << frag_likelihood << std::endl;
                }
                //std::cout << "Total likelihood = " << total_likelihood << std::endl;
                ofs << total_likelihood << std::endl;
            }
            else
            {
                ofs << -100000 << std::endl;
            }
        }
    }
    ifs.close();
    ofs.close();
}
