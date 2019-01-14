//  This is under contrib\Manifold_extraction\vis

#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_complexify.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_box_2d.h>
#include <vector>
#include <vbl/vbl_array_3d.h>
#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include <vehicle_model.h>
#include <Lie_group_operations.h>
#include <vsol/vsol_rectangle_2d_sptr.h>

std::vector<vnl_matrix<double> >read_transformation_matrices(std::string trans_matrices_info,int num_matrices)
    {
    int i;
    std::vector<vnl_matrix<double> > transformation_matrices;
    std::ifstream ifst(trans_matrices_info.c_str());

    for (i = 0;i<num_matrices;i++)
        {
        vnl_matrix<double>TM(3,3,0.0);
        ifst >> TM;
        std::cout << TM << std::endl;
        transformation_matrices.push_back(TM);
        }
    return transformation_matrices;
    }

std::vector<vehicle_model> read_models(std::string vehicle_model_info,int num_models)
    {
    int i,j,k;
    double x,y;
    char ch;
    std::ifstream ifst(vehicle_model_info.c_str());
    std::vector<vehicle_model> model_vec;
    std::vector<vsol_rectangle_2d> box1,box2,box3;

    vsol_point_2d p0,p1,p2,p3;

    // vgl_point_2d<double> pt1box1,pt2box1,pt1box2,pt2box2,pt1box3,pt2box3;

    for (i =0;i<num_models;i++)
        {
        for (j =0;j<3;j++)
            {  
            for (k = 0;k<4;k++)
                {
                // ifst >> x >> ch >> y;
                ifst >> x >> y;
                if (k==0)
                    {
                    p0.set_x(x);
                    p0.set_y(y);
                    }
                if (k==1)
                    {
                    p1.set_x(x);
                    p1.set_y(y);
                    }
                if (k==2)
                    {
                    p2.set_x(x);
                    p2.set_y(y);
                    }
                if (k==3)
                    {
                    p3.set_x(x);
                    p3.set_y(y);
                    }
                }
            vsol_point_2d_sptr p0_sptr = new vsol_point_2d(p0);
            vsol_point_2d_sptr p1_sptr = new vsol_point_2d(p1);
            vsol_point_2d_sptr p2_sptr = new vsol_point_2d(p2);
            vsol_point_2d_sptr p3_sptr = new vsol_point_2d(p3);

            if (j == 0)
                {
                vsol_rectangle_2d_sptr box1_sptr = new vsol_rectangle_2d(p0_sptr,p1_sptr,p2_sptr,p3_sptr);
                box1.push_back(*box1_sptr);

                }
            if (j == 1)
                {
                vsol_rectangle_2d_sptr box2_sptr = new vsol_rectangle_2d(p0_sptr,p1_sptr,p2_sptr,p3_sptr);
                box2.push_back(*box2_sptr);
                }
            if (j == 2)
                {
                vsol_rectangle_2d_sptr box3_sptr = new vsol_rectangle_2d(p0_sptr,p1_sptr,p2_sptr,p3_sptr);
                box3.push_back(*box3_sptr);
                }


            }
        vehicle_model M(box1[i],box2[i],box3[i]);
        model_vec.push_back(M);
        }
    return model_vec;
    }



int main(int argc,char **argv)
    {
    std::string vehicle_model_info = argv[1];
    std::string non_class_models = argv[2];
    

    std::ofstream non_class_models_info(non_class_models.c_str());

    int i,num_models = 10;

    vnl_matrix<double>G1(3,3,0.0);
    vnl_matrix<double>G2(3,3,0.0);
    vnl_matrix<double>G3(3,3,0.0);
    vnl_matrix<double>g1(3,3,0.0);
    vnl_matrix<double>g2(3,3,0.0);
    vnl_matrix<double>g3(3,3,0.0);

    
    std::vector<vehicle_model> model_vec = read_models(vehicle_model_info,num_models);

    vehicle_model ref_model(model_vec[0].engine(),model_vec[0].body(),model_vec[0].rear());
    std::vector<vnl_matrix<double> >G1_vec,G2_vec,G3_vec;

    double tx,ty;

    for (i=0;i<num_models;i++)
        {
        get_transformation_matrices(model_vec[0],model_vec[i],G1,G2,G3);

        // get the translation values for G2 and subtract them from G1,G2 and G3
        // this is a kind of alignment to get rid of redundant parameters
       
        tx = G2.get(0,2);
        ty = G2.get(1,2);

        G1.put(0,2,G1.get(0,2)-tx);
        G1.put(1,2,G1.get(1,2)-ty);
        G2.put(0,2,G2.get(0,2)-tx);
        G2.put(1,2,G2.get(1,2)-ty);
        G3.put(0,2,G3.get(0,2)-tx);
        G3.put(1,2,G3.get(1,2)-ty);

       
        non_class_models_info << G1 << std::endl;
        non_class_models_info << G2 << std::endl;
        non_class_models_info << G3 << std::endl;

        }
    return 0;
    }
