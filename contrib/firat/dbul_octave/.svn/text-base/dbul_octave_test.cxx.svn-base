/*
 * dbul_octave_test.cxx
 *
 *  Created on: Feb 11, 2010
 *      Author: firat
 */

#include<dbul/dbul_octave.h>
#include<vcl_cstdio.h>
#include<bxml/bxml_read.h>

int main()
{
    dbul_octave_value pos_tree, neg_tree;
    bxml_document pos_doc = bxml_read("/vision/projects/kimia/shockshape/symseg/results/weizmann_horse/weizmann-pos_ccm_cost_3rd_order_edges.xml");
    bxml_data_sptr pos_root_xml = pos_doc.root_element();
    bool stat1 = dbul_octave_convert_xml_to_octave_value(pos_root_xml, pos_tree);

    bxml_document neg_doc = bxml_read("/vision/projects/kimia/shockshape/symseg/results/weizmann_horse/weizmann-neg_ccm_cost_3rd_order_edges.xml");
    bxml_data_sptr neg_root_xml = neg_doc.root_element();
    bool stat2 = dbul_octave_convert_xml_to_octave_value(neg_root_xml, neg_tree);
    if(stat1 && stat2)
    {
        dbul_octave_argument_list inargs;
        dbul_octave_argument_list outargs;
        inargs(0) = "/vision/projects/kimia/shockshape/symseg/results/weizmann_horse/weizmann-pos_ccm_cost_3rd_order_edges.xml";
        inargs(1) = "/vision/projects/kimia/shockshape/symseg/results/weizmann_horse/weizmann-neg_ccm_cost_3rd_order_edges.xml";
        inargs(2) = pos_tree;
        inargs(3) = neg_tree;
        inargs(4) = "";
        inargs(5) = "octave.xml";
        dbul_octave.run("/vision/scratch/firat/ccm-octave","compute_ccm_stat_dist_v2", inargs, outargs);
    }
    return 0;
}

