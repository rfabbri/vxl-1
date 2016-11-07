// This is brcv/seg/dbdet/algo/dbdet_pb_edge_detector.cxx

//:
// \file
// \brief
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Feb 16, 2010

#include <dbil/algo/dbil_octave.h>
#include <vcl_ctime.h>
#include <vcl_iostream.h>

#include "dbdet_pb_edge_detector.h"
#include "dbdet_pb_edge_detector_embedded_files.h"


dbdet_edgemap_sptr dbdet_convert_pb_outputs_to_edgemap(dbul_octave_double_array& pb_array, dbul_octave_double_array& theta_array)
{
    dim_vector dv = pb_array.dims();
    int height = dv(0);
    int width = dv(1);
    dbdet_edgemap_sptr edgemap_ptr = new dbdet_edgemap(width, height);
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            double strength = pb_array(y, x);
            if(strength > 0)
            {
                vgl_point_2d<double> loc(x, y);
                vgl_point_2d<int> loc2(x, y);
                dbdet_edgel* new_edgel = new dbdet_edgel(loc, theta_array(y, x), strength, 0, 0);
                new_edgel->gpt = loc2;
                edgemap_ptr->insert(new_edgel, x, y);
            }
        }
    }
    return edgemap_ptr;
}

dbdet_pb_edge_detector::dbdet_pb_edge_detector(const vcl_string& temp_dir)
{
    temp_dir_ = temp_dir;
    dbdet_extract_embedded_pb_files(temp_dir_.c_str());
}

dbdet_pb_edge_detector::~dbdet_pb_edge_detector()
{
    dbdet_delete_extracted_pb_files(temp_dir_.c_str());
}

dbdet_edgemap_sptr dbdet_pb_edge_detector::detect_bg(vil_image_view<vxl_byte>& image, double radius, int norient)
{
    dbul_octave_argument_list conversion_inargs;
    dbul_octave_argument_list inargs;
    dbul_octave_argument_list conversion_outargs;
    dbul_octave_argument_list outargs;

    dbul_octave_value octave_image = dbil_convert_image_view_to_octave_value<vxl_byte>(image);
    conversion_inargs(0) = octave_image;
    dbul_octave.run("","im2double", conversion_inargs, conversion_outargs);

    inargs(0) = dbul_octave_value_to_octave_double_array(conversion_outargs(0));
    inargs(1) = radius;
    inargs(2) = norient;

    dbul_octave.run(temp_dir_,"pbBG", inargs, outargs);

    dbul_octave_double_array pb_array = dbul_octave_value_to_octave_double_array(outargs(0));
    dbul_octave_double_array theta_array = dbul_octave_value_to_octave_double_array(outargs(1));

    return dbdet_convert_pb_outputs_to_edgemap(pb_array, theta_array);
}

dbdet_edgemap_sptr dbdet_pb_edge_detector::detect_cg(vil_image_view<vxl_byte>& image, double radius, int norient)
{
    dbul_octave_argument_list conversion_inargs;
    dbul_octave_argument_list inargs;
    dbul_octave_argument_list conversion_outargs;
    dbul_octave_argument_list outargs;

    dbul_octave_value octave_image = dbil_convert_image_view_to_octave_value<vxl_byte>(image);
    conversion_inargs(0) = octave_image;
    dbul_octave.run("","im2double", conversion_inargs, conversion_outargs);

    inargs(0) = dbul_octave_value_to_octave_double_array(conversion_outargs(0));
    inargs(1) = radius;
    inargs(2) = norient;

    dbul_octave.run(temp_dir_,"pbCG", inargs, outargs);

    dbul_octave_double_array pb_array = dbul_octave_value_to_octave_double_array(outargs(0));
    dbul_octave_double_array theta_array = dbul_octave_value_to_octave_double_array(outargs(1));

    return dbdet_convert_pb_outputs_to_edgemap(pb_array, theta_array);
}

dbdet_edgemap_sptr dbdet_pb_edge_detector::detect_gm(vil_image_view<vxl_byte>& image, double sigma)
{
    dbul_octave_argument_list conversion_inargs;
    dbul_octave_argument_list inargs;
    dbul_octave_argument_list conversion_outargs;
    dbul_octave_argument_list outargs;

    dbul_octave_value octave_image = dbil_convert_image_view_to_octave_value<vxl_byte>(image);
    conversion_inargs(0) = octave_image;
    dbul_octave.run("","im2double", conversion_inargs, conversion_outargs);

    inargs(0) = dbul_octave_value_to_octave_double_array(conversion_outargs(0));
    inargs(1) = sigma;

    dbul_octave.run(temp_dir_,"pbGM", inargs, outargs);

    dbul_octave_double_array pb_array = dbul_octave_value_to_octave_double_array(outargs(0));
    dbul_octave_double_array theta_array = dbul_octave_value_to_octave_double_array(outargs(1));

    return dbdet_convert_pb_outputs_to_edgemap(pb_array, theta_array);
}

dbdet_edgemap_sptr dbdet_pb_edge_detector::detect_gm2(vil_image_view<vxl_byte>& image, double sigma)
{
    dbul_octave_argument_list conversion_inargs;
    dbul_octave_argument_list inargs;
    dbul_octave_argument_list conversion_outargs;
    dbul_octave_argument_list outargs;

    dbul_octave_value octave_image = dbil_convert_image_view_to_octave_value<vxl_byte>(image);
    conversion_inargs(0) = octave_image;
    dbul_octave.run("","im2double", conversion_inargs, conversion_outargs);

    inargs(0) = dbul_octave_value_to_octave_double_array(conversion_outargs(0));
    inargs(1) = sigma;

    dbul_octave.run(temp_dir_,"pbGM2", inargs, outargs);

    dbul_octave_double_array pb_array = dbul_octave_value_to_octave_double_array(outargs(0));
    dbul_octave_double_array theta_array = dbul_octave_value_to_octave_double_array(outargs(1));

    return dbdet_convert_pb_outputs_to_edgemap(pb_array, theta_array);
}

dbdet_edgemap_sptr dbdet_pb_edge_detector::detect_tg(vil_image_view<vxl_byte>& image, double radius, int norient)
{
    dbul_octave_argument_list conversion_inargs;
    dbul_octave_argument_list inargs;
    dbul_octave_argument_list conversion_outargs;
    dbul_octave_argument_list outargs;

    dbul_octave_value octave_image = dbil_convert_image_view_to_octave_value<vxl_byte>(image);
    conversion_inargs(0) = octave_image;
    dbul_octave.run("","im2double", conversion_inargs, conversion_outargs);

    inargs(0) = dbul_octave_value_to_octave_double_array(conversion_outargs(0));
    inargs(1) = radius;
    inargs(2) = norient;

    dbul_octave.run(temp_dir_,"pbTG", inargs, outargs);

    dbul_octave_double_array pb_array = dbul_octave_value_to_octave_double_array(outargs(0));
    dbul_octave_double_array theta_array = dbul_octave_value_to_octave_double_array(outargs(1));

    return dbdet_convert_pb_outputs_to_edgemap(pb_array, theta_array);
}

dbdet_edgemap_sptr dbdet_pb_edge_detector::detect_bgtg(vil_image_view<vxl_byte>& image, const vcl_string& pres, double radius[], int norient)
{
    dbul_octave_argument_list conversion_inargs;
    dbul_octave_argument_list inargs;
    dbul_octave_argument_list conversion_outargs;
    dbul_octave_argument_list outargs;

    dbul_octave_value octave_image = dbil_convert_image_view_to_octave_value<vxl_byte>(image);
    conversion_inargs(0) = octave_image;
    dbul_octave.run("","im2double", conversion_inargs, conversion_outargs);

    inargs(0) = dbul_octave_value_to_octave_double_array(conversion_outargs(0));
    inargs(1) = pres.c_str();
    vcl_vector<int> sizes(2);
    sizes[0] = 1; sizes[1] = 2;
    dbul_octave_double_array radius_array = dbul_octave_get_empty_double_array(sizes);
    radius_array(0,0) = radius[0]; radius_array(0,1) = radius[1];
    inargs(2) = radius_array;
    inargs(3) = norient;

    dbul_octave.run(temp_dir_,"pbBGTG", inargs, outargs);

    dbul_octave_double_array pb_array = dbul_octave_value_to_octave_double_array(outargs(0));
    dbul_octave_double_array theta_array = dbul_octave_value_to_octave_double_array(outargs(1));

    return dbdet_convert_pb_outputs_to_edgemap(pb_array, theta_array);
}

dbdet_edgemap_sptr dbdet_pb_edge_detector::detect_cgtg(vil_image_view<vxl_byte>& image, double radius[], int norient)
{
    dbul_octave_argument_list conversion_inargs;
    dbul_octave_argument_list inargs;
    dbul_octave_argument_list conversion_outargs;
    dbul_octave_argument_list outargs;

    dbul_octave_value octave_image = dbil_convert_image_view_to_octave_value<vxl_byte>(image);
    conversion_inargs(0) = octave_image;
    dbul_octave.run("","im2double", conversion_inargs, conversion_outargs);

    inargs(0) = dbul_octave_value_to_octave_double_array(conversion_outargs(0));
    vcl_vector<int> sizes(2);
    sizes[0] = 1; sizes[1] = 4;
    dbul_octave_double_array radius_array = dbul_octave_get_empty_double_array(sizes);
    radius_array(0,0) = radius[0]; radius_array(0,1) = radius[1]; radius_array(0,2) = radius[2]; radius_array(0,3) = radius[3];
    inargs(1) = radius_array;
    inargs(2) = norient;

    dbul_octave.run(temp_dir_,"pbCGTG", inargs, outargs);

    dbul_octave_double_array pb_array = dbul_octave_value_to_octave_double_array(outargs(0));
    dbul_octave_double_array theta_array = dbul_octave_value_to_octave_double_array(outargs(1));

    return dbdet_convert_pb_outputs_to_edgemap(pb_array, theta_array);
}

dbdet_edgemap_sptr dbdet_pb_edge_detector::detect_bg(vil_image_view<vxl_byte>& image)
{
    return detect_bg(image, 0.01, 8);
}

dbdet_edgemap_sptr dbdet_pb_edge_detector::detect_cg(vil_image_view<vxl_byte>& image)
{
    return detect_cg(image, 0.02, 8);
}

dbdet_edgemap_sptr dbdet_pb_edge_detector::detect_gm(vil_image_view<vxl_byte>& image)
{
    return detect_gm(image, 2);
}

dbdet_edgemap_sptr dbdet_pb_edge_detector::detect_gm2(vil_image_view<vxl_byte>& image)
{
    return detect_gm2(image, 2);
}

dbdet_edgemap_sptr dbdet_pb_edge_detector::detect_tg(vil_image_view<vxl_byte>& image)
{
    return detect_tg(image, 0.02, 8);
}

dbdet_edgemap_sptr dbdet_pb_edge_detector::detect_bgtg(vil_image_view<vxl_byte>& image)
{
    double radius[] = {0.01, 0.02};
    return detect_bgtg(image, "gray", radius, 8);
}

dbdet_edgemap_sptr dbdet_pb_edge_detector::detect_cgtg(vil_image_view<vxl_byte>& image)
{
    double radius[] = {0.01, 0.02, 0.02, 0.02};
    return detect_cgtg(image, radius, 8);
}
