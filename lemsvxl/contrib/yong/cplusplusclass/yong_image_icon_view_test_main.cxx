#include <iostream>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <string.h>
#include <cstring>
#include <bitset>
#include <cassert>
#include <math.h>
#include <cplusplusclass/yong_image_icon_view.h>
#include <vsl/vsl_vector_io.h>


#if 0

int main(int argc, char **argv)
{
    std::string bvl_name;
    std::string jpg_name;
    std::stringstream SS;

    std::vector<yong_image_icon_view_sptr> icon_list;

    icon_list.push_back(yong_image_icon_view_new("icon_images\\big_image_1.jpg"));
    icon_list.push_back(yong_image_icon_view_new("icon_images\\big_image_2.jpg"));
    icon_list.push_back(yong_image_icon_view_new("icon_images\\big_image_3.jpg"));
    icon_list.push_back(yong_image_icon_view_new("icon_images\\big_image_4.jpg"));
    icon_list.push_back(yong_image_icon_view_new("icon_images\\big_image_5.jpg"));
    icon_list.push_back(yong_image_icon_view_new("icon_images\\big_image_6.jpg"));
    icon_list.push_back(yong_image_icon_view_new("icon_images\\big_image_7.jpg"));
    icon_list.push_back(yong_image_icon_view_new("icon_images\\big_image_8.jpg"));
    icon_list.push_back(yong_image_icon_view_new("icon_images\\big_image_9.jpg"));

    // write into binary IO separately
    for(unsigned i=0; i<icon_list.size(); i++)
    {
        SS.clear();
        bvl_name.clear();
        SS << "icon_images\\icon_" << i << ".bvl";
        SS >> bvl_name;

        vsl_b_ofstream obfs(bvl_name.c_str());
        if (!obfs)
        {
            std::cerr<<"Failed to open " << bvl_name.c_str() << std::endl;
            return -1;
        }

        icon_list[i]->b_write(obfs);

        obfs.close();
    }

    icon_list_1.push_back(*yong_image_icon_view_new("icon_images\\big_image_8.jpg"));
    icon_list_1.push_back(*yong_image_icon_view_new("icon_images\\big_image_9.jpg"));
    // write the whole vector;
    vsl_b_ofstream obfs_vector("icon_images\\icon_vector.bvl");
    if (!obfs_vector)
    {
        std::cerr<<"Failed to open icon_images\\icon_vector.bvl" << std::endl;
        return -1;
    }

    vsl_b_write(obfs_vector, icon_list_1);
    obfs_vector.close();


    // clear the icon_list
    icon_list.clear();
    for(unsigned i=0; i<9; i++)
    {
        SS.clear();
        jpg_name.clear();
        SS << "icon_images\\icon_" << i << ".jpg";
        SS >> jpg_name;
        icon_list.push_back(yong_image_icon_view_new(jpg_name.c_str()));
    }

    // read binary IO separately
    for(unsigned i=0; i<9; i++)
    {
        SS.clear();
        bvl_name.clear();
        SS << "icon_images\\icon_" << i << ".bvl";
        SS >> bvl_name;

        vsl_b_ifstream ibfs(bvl_name.c_str());
        if (!ibfs)
        {
            std::cerr<<"Failed to open " << bvl_name.c_str() << std::endl;
            return -1;
        }

        icon_list[i]->b_read(ibfs);

        ibfs.close();
    }

    for(unsigned i=0; i<icon_list.size(); i++)
    {
        icon_list[i]->Output_Icon_Image();
    }


    // clear the icon_list
    icon_list.clear();
    for(unsigned i=0; i<9; i++)
    {
        SS.clear();
        jpg_name.clear();
        SS << "icon_images\\icon_from_vector_" << i << ".jpg";
        SS >> jpg_name;
        icon_list.push_back(yong_image_icon_view_new(jpg_name.c_str()));
    }

    // read the whole binary IO vector
    vsl_b_ifstream ibfs_vector("icon_images\\icon_vector.bvl");
    if (!ibfs_vector)
    {
        std::cerr<<"Failed to open icon_images\\icon_vector.bvl " << std::endl;
        return -1;
    }

    vsl_b_read(ibfs_vector, icon_list);
    ibfs_vector.close();


    for(unsigned i=0; i<icon_list.size(); i++)
    {
        icon_list[i]->Output_Icon_Image();
    }


    return 1;
}

#endif

#if 0

int main(int argc, char **argv)
{
    std::string bvl_name;
    std::string jpg_name;
    std::stringstream SS;

    std::vector<yong_image_icon_view> icon_list;

    icon_list.push_back(*yong_image_icon_view_new("icon_images\\big_image_1.jpg"));
    icon_list.push_back(*yong_image_icon_view_new("icon_images\\big_image_2.jpg"));
    icon_list.push_back(*yong_image_icon_view_new("icon_images\\big_image_3.jpg"));
    icon_list.push_back(*yong_image_icon_view_new("icon_images\\big_image_4.jpg"));
    icon_list.push_back(*yong_image_icon_view_new("icon_images\\big_image_5.jpg"));
    icon_list.push_back(*yong_image_icon_view_new("icon_images\\big_image_6.jpg"));
    icon_list.push_back(*yong_image_icon_view_new("icon_images\\big_image_7.jpg"));
    icon_list.push_back(*yong_image_icon_view_new("icon_images\\big_image_8.jpg"));
    icon_list.push_back(*yong_image_icon_view_new("icon_images\\big_image_9.jpg"));

    // write into binary IO separately
    for(unsigned i=0; i<icon_list.size(); i++)
    {
        SS.clear();
        bvl_name.clear();
        SS << "icon_images\\icon_" << i << ".bvl";
        SS >> bvl_name;

        vsl_b_ofstream obfs(bvl_name.c_str());
        if (!obfs)
        {
            std::cerr<<"Failed to open " << bvl_name.c_str() << std::endl;
            return -1;
        }

        icon_list[i].b_write(obfs);

        obfs.close();
    }

    // write the whole vector;
    vsl_b_ofstream obfs_vector("icon_images\\icon_vector.bvl");
    if (!obfs_vector)
    {
        std::cerr<<"Failed to open icon_images\\icon_vector.bvl" << std::endl;
        return -1;
    }

    vsl_b_write(obfs_vector, icon_list);
    obfs_vector.close();


    // clear the icon_list
    icon_list.clear();
    for(unsigned i=0; i<9; i++)
    {
        SS.clear();
        jpg_name.clear();
        SS << "icon_images\\icon_" << i << ".jpg";
        SS >> jpg_name;
        icon_list.push_back(*yong_image_icon_view_new(jpg_name.c_str()));
    }

    // read binary IO separately
    for(unsigned i=0; i<9; i++)
    {
        SS.clear();
        bvl_name.clear();
        SS << "icon_images\\icon_" << i << ".bvl";
        SS >> bvl_name;

        vsl_b_ifstream ibfs(bvl_name.c_str());
        if (!ibfs)
        {
            std::cerr<<"Failed to open " << bvl_name.c_str() << std::endl;
            return -1;
        }

        icon_list[i].b_read(ibfs);

        ibfs.close();
    }

    for(unsigned i=0; i<icon_list.size(); i++)
    {
        icon_list[i].Output_Icon_Image();
    }


    // clear the icon_list
    icon_list.clear();
    for(unsigned i=0; i<9; i++)
    {
        SS.clear();
        jpg_name.clear();
        SS << "icon_images\\icon_from_vector_" << i << ".jpg";
        SS >> jpg_name;
        icon_list.push_back(*yong_image_icon_view_new(jpg_name.c_str()));
    }

    // read the whole binary IO vector
    vsl_b_ifstream ibfs_vector("icon_images\\icon_vector.bvl");
    if (!ibfs_vector)
    {
        std::cerr<<"Failed to open icon_images\\icon_vector.bvl " << std::endl;
        return -1;
    }


    vsl_b_read(ibfs_vector, icon_list);
    ibfs_vector.close();


    for(unsigned i=0; i<icon_list.size(); i++)
    {
        icon_list[i].Output_Icon_Image();
    }


    return 1;
}

#endif
