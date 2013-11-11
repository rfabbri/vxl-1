// This is /lemsvxl/brcv/seg/dbdet/algo/dbdet_kovesi_edge_linker.cxx

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 1, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#include "dbdet_kovesi_edge_linker.h"
#include "dbdet_kovesi_edge_linker_embedded_files.h"
#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/algo/dbdet_cem_file_io.h>

#include <dbil/algo/dbil_octave.h>

#include <dbul/dbul_random.h>

#include<vcl_vector.h>

#include<vpl/vpl.h>

#define DBDET_CONVERT_EDGEMAP_TO_STH_OCTAVE_ARRAY(sth)\
        vcl_vector<int> sizes(2);\
        sizes[0] = in_EM->height(); sizes[1] = in_EM->width();\
        dbul_octave_double_array sth_array = dbul_octave_get_empty_double_array(sizes);\
        vcl_vector<dbdet_edgel*> edgels = in_EM->edgels;\
        for(int i = 0; i < edgels.size(); i++)\
        {\
            dbdet_edgel* e = edgels[i];\
            const vgl_point_2d<int>& gpt = (e->gpt);\
            sth_array(gpt.y(), gpt.x()) = e->sth;\
        }\
        return sth_array;

dbul_octave_double_array dbdet_convert_edgemap_to_orientation_octave_array(dbdet_edgemap_sptr& in_EM)
{
    DBDET_CONVERT_EDGEMAP_TO_STH_OCTAVE_ARRAY(tangent)
}

dbul_octave_double_array dbdet_convert_edgemap_to_strength_octave_array(dbdet_edgemap_sptr& in_EM)
{
    DBDET_CONVERT_EDGEMAP_TO_STH_OCTAVE_ARRAY(strength)
}

dbdet_kovesi_edge_linker::dbdet_kovesi_edge_linker(const vcl_string& temp_dir)
{
    temp_dir_ = temp_dir;
    dbdet_extract_embedded_kovesi_files(temp_dir_.c_str());
    temp_cem_file_ = temp_dir + "/";
    temp_cem_file_ += dbul_get_random_alphanumeric_string(10) + "_temp.cem";
}

dbdet_kovesi_edge_linker::~dbdet_kovesi_edge_linker()
{
    dbdet_delete_extracted_kovesi_files(temp_dir_.c_str());
}

void dbdet_kovesi_edge_linker::link_and_prune_edges(dbdet_edgemap_sptr& in_EM, int edge_threshold, int link_edge_length_threshold, dbdet_edgemap_sptr& out_EM, dbdet_curve_fragment_graph& out_CFG)
{
    dbul_octave_value edgemap_octave_value = dbdet_convert_edgemap_to_strength_octave_array(in_EM);
    dbul_octave_value orient_octave_value = dbdet_convert_edgemap_to_orientation_octave_array(in_EM);

    dbul_octave_argument_list inargs;
    dbul_octave_argument_list outargs;

    inargs(0) = edgemap_octave_value;
    inargs(1) = orient_octave_value;
    inargs(2) = edge_threshold;
    inargs(3) = link_edge_length_threshold;
    inargs(4) = temp_cem_file_.c_str();

    dbul_octave.run(temp_dir_,"kovesi_main", inargs, outargs);

    vcl_ifstream cems(temp_cem_file_.c_str());
    out_EM = dbdet_load_cem_v1(cems, out_CFG, in_EM->width(), in_EM->height(), false);
    vpl_unlink(temp_cem_file_.c_str());
}
