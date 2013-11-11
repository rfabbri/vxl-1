/*
 * dbdet_pb_edge_detector_test.cxx
 *
 *  Created on: Feb 16, 2010
 *      Author: firat
 */

#include <dbdet/algo/dbdet_pb_edge_detector.h>
#include <dbdet/algo/dbdet_load_edg.h>
#include <dbdet/algo/dbdet_kovesi_edge_linker.h>
#include<dbdet/sel/dbdet_curve_fragment_graph.h>
#include<vil/vil_load.h>
#include<vil/vil_save.h>

int main()
{
    dbdet_pb_edge_detector pb("/tmp/pb_temp_files/");
    dbdet_kovesi_edge_linker kel("/tmp/kovesi_temp_files");
    vil_image_view<vxl_byte> apple_img = vil_load("/home/firat/lemsvxl/src/contrib/firat/dbdet_pb_edge_detector/data/apple.jpg");
    vil_image_view<vxl_byte> horse_img = vil_load("/home/firat/lemsvxl/src/contrib/firat/dbdet_pb_edge_detector/data/horse.jpg");

    vcl_string edg_file;
    vcl_string png_file;
    dbdet_edgemap_sptr edgemap_ptr;

    /*for(int i = 0; i < 2; i++)
    {*/
        //vcl_cout << "PASS " << i+1 << vcl_endl;
        edgemap_ptr = pb.detect_bg(horse_img);
        edg_file = "/home/firat/lemsvxl/src/contrib/firat/dbdet_pb_edge_detector/data/horse_bg.edg";
        png_file = "/home/firat/lemsvxl/src/contrib/firat/dbdet_pb_edge_detector/data/horse_bg.png";
        dbdet_save_edg(edg_file, edgemap_ptr);
        dbdet_edgemap_sptr out_EM;
        dbdet_curve_fragment_graph out_CFG;
        kel.link_and_prune_edges(edgemap_ptr, 15, 4, out_EM, out_CFG);
        vcl_cout << "Number of edges = " << out_EM->num_edgels() << vcl_endl;
        vcl_cout << "Size of CFG = " << out_CFG.frags.size() << vcl_endl;
        //vil_save(out, png_file.c_str());

        /*edgemap_ptr = pb.detect_cg(apple_img);
        edg_file = "/home/firat/lemsvxl/src/contrib/firat/dbdet_pb_edge_detector/data/apple_cg.edg";
        png_file = "/home/firat/lemsvxl/src/contrib/firat/dbdet_pb_edge_detector/data/apple_cg.png";
        dbdet_save_edg(edg_file, edgemap_ptr);
        //vil_save(out, png_file.c_str());

        edgemap_ptr = pb.detect_gm(horse_img);
        edg_file = "/home/firat/lemsvxl/src/contrib/firat/dbdet_pb_edge_detector/data/horse_gm.edg";
        png_file = "/home/firat/lemsvxl/src/contrib/firat/dbdet_pb_edge_detector/data/horse_gm.png";
        dbdet_save_edg(edg_file, edgemap_ptr);
        //vil_save(out, png_file.c_str());

        //edgemap_ptr = pb.detect_gm2(horse_img);
        //edg_file = "/home/firat/lemsvxl/src/contrib/firat/dbdet_pb_edge_detector/data/horse_gm2.edg";
        //png_file = "/home/firat/lemsvxl/src/contrib/firat/dbdet_pb_edge_detector/data/horse_gm2.png";
        //dbdet_save_edg(edg_file, edgemap_ptr);
        //vil_save(out, png_file.c_str());

        edgemap_ptr = pb.detect_tg(horse_img);
        edg_file = "/home/firat/lemsvxl/src/contrib/firat/dbdet_pb_edge_detector/data/horse_tg.edg";
        png_file = "/home/firat/lemsvxl/src/contrib/firat/dbdet_pb_edge_detector/data/horse_tg.png";
        dbdet_save_edg(edg_file, edgemap_ptr);
        //vil_save(out, png_file.c_str());

        edgemap_ptr = pb.detect_cgtg(apple_img);
        edg_file = "/home/firat/lemsvxl/src/contrib/firat/dbdet_pb_edge_detector/data/apple_cgtg.edg";
        png_file = "/home/firat/lemsvxl/src/contrib/firat/dbdet_pb_edge_detector/data/apple_cgtg.png";
        dbdet_save_edg(edg_file, edgemap_ptr);
        //vil_save(out, png_file.c_str());

        edgemap_ptr = pb.detect_bgtg(horse_img);
        edg_file = "/home/firat/lemsvxl/src/contrib/firat/dbdet_pb_edge_detector/data/horse_bgtg.edg";
        png_file = "/home/firat/lemsvxl/src/contrib/firat/dbdet_pb_edge_detector/data/horse_bgtg.png";
        dbdet_save_edg(edg_file, edgemap_ptr);
        //vil_save(out, png_file.c_str());*/
    //}
}
