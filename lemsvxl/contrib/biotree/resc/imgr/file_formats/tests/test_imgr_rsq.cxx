//: 
// \file     test_imgr_rsq.cxx
// \brief    testing for rsq file filter
// \author   Michael Morgenstern
// \date     2005-28-08


#include <vcl_iostream.h>
#include <vcl_string.h>
#include <testlib/testlib_test.h>
#include <vil/vil_stream_fstream.h>
#if VXL_WIN32 
#include <vil/vil_stream_fstream_64.h>
#endif
#include <imgr/file_formats/imgr_rsq_header.h>
#include <imgr/file_formats/imgr_rsq.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_quaternion.h>
#include <vxl_config.h>
#include <xscan/xscan_uniform_orbit.h>

static void test_imgr_rsq(int argc, char* argv[] )
{

vil_stream* is = new vil_stream_fstream(argv[1], "r");

imgr_rsq_header rsq_header(is);

    TEST_EQUAL("check_ : "         , strcmp(rsq_header.check_,"CTDATA-HEADER_V1") ,1);
    TEST_EQUAL("data_type_ : "     , rsq_header.data_type_     ,1);
    TEST_EQUAL("nr_of_bytes_ : "   , rsq_header.nr_of_bytes_   ,335360);
    TEST_EQUAL("nr_of_blocks_ : "  , rsq_header.nr_of_blocks_  ,0);
    TEST_EQUAL("patient_index_ : " , rsq_header.patient_index_ ,641);
    TEST_EQUAL("scanner_id_ : "    , rsq_header.scanner_id_    ,4136);
    TEST_EQUAL("creation_date_[0] : " , rsq_header.creation_date_[0], -348659845);
    TEST_EQUAL("creation_date_[1] : " , rsq_header.creation_date_[1], 10752599);
    
    vcl_cout << "---" << vcl_endl;
    
    TEST_EQUAL("dimx_p_ : "    , rsq_header.dimx_p_    ,1024);
    TEST_EQUAL("dimy_p_ : "    , rsq_header.dimy_p_    ,3);
    TEST_EQUAL("dimz_p_ : "    , rsq_header.dimz_p_    ,54);
    TEST_EQUAL("dimx_um_ : "   , rsq_header.dimx_um_   ,20871);
    TEST_EQUAL("dimy_mdeg_ : " , rsq_header.dimy_mdeg_ ,360000);
    TEST_EQUAL("dimz_um_ : "   , rsq_header.dimz_um_   ,540);
    
    vcl_cout << "---" << vcl_endl;
    
    TEST_EQUAL("slice_thickness_um_ : " , rsq_header.slice_thickness_um_ ,10);
    TEST_EQUAL("slice_increment_um_ : " , rsq_header.slice_increment_um_ ,10);
    TEST_EQUAL("slice_1_pos_um_ : "     , rsq_header.slice_1_pos_um_     ,64967);
    
    vcl_cout << "---" << vcl_endl;
    
    TEST_EQUAL("scanner_type_ : "   , rsq_header.scanner_type_   ,10);
    TEST_EQUAL("min_data_value_ : " , rsq_header.min_data_value_ ,4772);
    TEST_EQUAL("max_data_value_ : " , rsq_header.max_data_value_ ,9608);
    
    vcl_cout << "---raw area type structure members start here---" << vcl_endl;
    
    TEST_EQUAL("nr_of_det_x_ : "                  , rsq_header.u_.area_.nr_of_det_x_                 ,1024);
    TEST_EQUAL("nr_of_det_y_ : "                  , rsq_header.u_.area_.nr_of_det_y_                 ,56);
    TEST_EQUAL("detector_length_x_um_ : "         , rsq_header.u_.area_.detector_length_x_um_        ,24576);
    TEST_EQUAL("detector_length_y_um_ : "         , rsq_header.u_.area_.detector_length_y_um_        ,1344);
    TEST_EQUAL("detector_center_x_p_ : "          , rsq_header.u_.area_.detector_center_x_p_         ,187);
    TEST_EQUAL("detector_center_y_p_ : "          , rsq_header.u_.area_.detector_center_y_p_         ,240);
    TEST_EQUAL("detector_distance_um_ : "         , rsq_header.u_.area_.detector_distance_um_        ,126138);
    TEST_EQUAL("rotation_center_distance_um_ : "  , rsq_header.u_.area_.rotation_center_distance_um_ ,54557);
    TEST_EQUAL("detector_angle_x_mdeg_ : "        , rsq_header.u_.area_.detector_angle_x_mdeg_       ,0);
    TEST_EQUAL("detector_angle_y_mdeg_ : "        , rsq_header.u_.area_.detector_angle_y_mdeg_       ,0);
    TEST_EQUAL("detector_angle_z_mdeg_ : "        , rsq_header.u_.area_.detector_angle_z_mdeg_       ,-2);
    TEST_EQUAL("slice_angle_increment_mdeg_ : "   , rsq_header.u_.area_.slice_angle_increment_mdeg_  ,0);

    vcl_cout << "---" << vcl_endl;

    TEST_EQUAL("i0_pixel_l_x_ : "   , rsq_header.u_.area_.i0_pixel_l_x_   ,0);
    TEST_EQUAL("i0_pixel_r_x_ : "   , rsq_header.u_.area_.i0_pixel_r_x_   ,0);
    TEST_EQUAL("i0_pixel_u_y_ : "   , rsq_header.u_.area_.i0_pixel_u_y_   ,0);
    TEST_EQUAL("i0_pixel_d_y_ : "   , rsq_header.u_.area_.i0_pixel_d_y_   ,0);
    TEST_EQUAL("dark_pixel_l_x_ : " , rsq_header.u_.area_.dark_pixel_l_x_ ,0);
    TEST_EQUAL("dark_pixel_r_x_ : " , rsq_header.u_.area_.dark_pixel_r_x_ ,0);
    TEST_EQUAL("dark_pixel_u_y_ : " , rsq_header.u_.area_.dark_pixel_u_y_ ,0);
    TEST_EQUAL("dark_pixel_d_y_ : " , rsq_header.u_.area_.dark_pixel_d_y_ ,0);
    TEST_EQUAL("data_pixel_l_x_ : " , rsq_header.u_.area_.data_pixel_l_x_ ,0);
    TEST_EQUAL("data_pixel_r_x_ : " , rsq_header.u_.area_.data_pixel_r_x_ ,1023);
    TEST_EQUAL("data_pixel_u_y_ : " , rsq_header.u_.area_.data_pixel_u_y_ ,0);
    TEST_EQUAL("data_pixel_d_y_ : " , rsq_header.u_.area_.data_pixel_d_y_ ,0);
    TEST_EQUAL("i0_index_ : "       , rsq_header.u_.area_.i0_index_       ,1);
    TEST_EQUAL("dark_index_ : "     , rsq_header.u_.area_.dark_index_     ,0);
    TEST_EQUAL("data_index_ : "     , rsq_header.u_.area_.data_index_     ,2);
    
    vcl_cout << "---" << vcl_endl;
    
    TEST_EQUAL("index_measurement_ : "   , rsq_header.u_.area_.index_measurement_   ,977);
    TEST_EQUAL("integration_time_us_ : " , rsq_header.u_.area_.integration_time_us_ ,300000);
    TEST_EQUAL("name_ : "                , strcmp(rsq_header.u_.area_.name_,"G8-Filament"),1);
    TEST_EQUAL("energy_ : "              , rsq_header.u_.area_.energy_              ,70000);
    TEST_EQUAL("intensity_ : "           , rsq_header.u_.area_.intensity_           ,90);
    TEST_EQUAL("scanning_mode_ : "       , rsq_header.u_.area_.scanning_mode_       ,4);
    
    vcl_cout << "---" << vcl_endl;
    
    TEST_EQUAL("no_frames_per_stack_ : "      , rsq_header.u_.area_.no_frames_per_stack_     ,2000);
    TEST_EQUAL("no_I0_per_stack_ : "          , rsq_header.u_.area_.no_I0_per_stack_          ,0);
    TEST_EQUAL("no_dark_per_stack_ : "        , rsq_header.u_.area_.no_dark_per_stack_        ,0);
    TEST_EQUAL("no_pro_per_stack_ : "         , rsq_header.u_.area_.no_pro_per_stack_         ,2000);
    TEST_EQUAL("z_pos_1stframe_center_um_ : " , rsq_header.u_.area_.z_pos_1stframe_center_um_ ,749287);
    TEST_EQUAL("z_inc_per_frame_nm_ : "       , rsq_header.u_.area_.z_inc_per_frame_nm_       ,0);
    TEST_EQUAL("z_inc_per_stack_um_ : "       , rsq_header.u_.area_.z_inc_per_stack_um_       ,470);
    
    vcl_cout << "---" << vcl_endl;
    
    TEST_EQUAL("vt_ : "        , rsq_header.u_.area_.vt_        ,60);
    TEST_EQUAL("ht_ : "        , rsq_header.u_.area_.ht_        ,1024);
    TEST_EQUAL("vo_ : "        , rsq_header.u_.area_.vo_        ,2);
    TEST_EQUAL("ho_ : "        , rsq_header.u_.area_.ho_        ,0);
    TEST_EQUAL("flip_code_ : " , rsq_header.u_.area_.flip_code_ ,3);
    TEST_EQUAL("vbinu_ : "     , rsq_header.u_.area_.vbinu_     ,1024);
    TEST_EQUAL("vbind_ : "     , rsq_header.u_.area_.vbind_     ,1024);
    TEST_EQUAL("hbinu_ : "     , rsq_header.u_.area_.hbinu_     ,1024);
    TEST_EQUAL("hbind_ : "     , rsq_header.u_.area_.hbind_     ,1024);
    
    vcl_cout << "---raw area type structure members end here---" << vcl_endl;

for (int f=0;f<36;f++)
    TEST_EQUAL("fill_ : "                , rsq_header.fill_[f]             ,0);
    TEST_EQUAL("reco_flags_ : "          , rsq_header.reco_flags_          ,0);
    TEST_EQUAL("reference_line_ : "      , rsq_header.reference_line_      ,0);
    TEST_EQUAL("site_ : "                , rsq_header.site_                ,4);
    TEST_EQUAL("def_reco_angle_mdeg_ : " , rsq_header.def_reco_angle_mdeg_ ,0);
    TEST_EQUAL("def_reco_size_ : "       , rsq_header.def_reco_size_       ,0);
    TEST_EQUAL("dummy1_ : "              , rsq_header.dummy1_              ,0);
    TEST_EQUAL("dummy2_ : "              , rsq_header.dummy2_              ,0);
    TEST_EQUAL("dummy3_ : "              , rsq_header.dummy3_              ,0);
    TEST_EQUAL("dummy4_ : "              , rsq_header.dummy4_              ,0);
    TEST_EQUAL("hext_offset_ : "         , rsq_header.hext_offset_         ,0);
    TEST_EQUAL("data_offset_ : "         , rsq_header.data_offset_         ,6);

    imgr_rsq rsq(is);
    
    vcl_cout << "---scan parameters begin here---" << vcl_endl;
    
    xscan_scan scan = rsq.get_scan();
    xscan_orbit_base_sptr orbitbase = scan.orbit();
    xscan_orbit_base *ptr_orbit = orbitbase.ptr();
    assert (ptr_orbit->class_id()==orbitbase->class_id());
    xscan_uniform_orbit *orbitt = static_cast<xscan_uniform_orbit*>(ptr_orbit);
    
    
    double matrix[3][3] = {{5255.75, 0, 1042.7}, {0, 5255.75, 80}, {0, 0, 1}};
    double rotch[4] = {120, 115, 99, 97};
    double tch[3] = {0,0,0};
    double r0ch[4] = {-0.707106781078849, 1.23413412837372e-005, -1.23413412837372e-005, 0.707106781078849};
    double t0ch[3] = {-0.00190439852714881, 0, 54.556999966762};


    TEST_EQUAL("total number of views : "   , scan.n_views()                   ,1);

    for(int i=0;i<3;i++) for(int j=0;j<3;j++)
         TEST_NEAR("matrix: ", scan.kk().get_matrix()[i][j], matrix[i][j],  1e-6);
    TEST("kind of orbit : ", scan.orbit()->class_id()=="xscan_uniform_orbit", true);

// the rest of the code is built assuming that the orbit is xscan_uniform_orbit.  It the test file changes you must reprogram.    
    for(int i=0;i<4;i++)
         TEST_NEAR("rotation_quaternion ",orbitt->class_id()[i], rotch[i],1e-6);
    for(int i=0;i<3;i++)
         TEST_NEAR("pitch_is ",orbitt->t()[i], tch[i],1e-6);
    for(int i=0;i<4;i++)
         TEST_NEAR("rotation_quaternion_to_the_first_camera ",orbitt->r0()[i], r0ch[i],1e-6);
    for(int i=0;i<3;i++)
         TEST_NEAR("translation_to_the_first_camera ",orbitt->t0()[i], t0ch[i],1e-6);


}

TESTMAIN_ARGS(test_imgr_rsq);
