//  This is under contrib\Manifold_extraction\vis
// Brief: An example for vehicle classification based on Lie distances


#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_complexify.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_box_2d.h>
#include <vcl_vector.h>
#include <vbl/vbl_array_3d.h>
#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vehicle_model.h>
#include <Lie_group_operations.h>
#include <vsol/vsol_rectangle_2d_sptr.h>

vcl_vector<vnl_matrix<double> >read_transformation_matrices(vcl_string trans_matrices_info,int num_matrices)
    {
    int i;
    vcl_vector<vnl_matrix<double> > transformation_matrices;
    vcl_ifstream ifst(trans_matrices_info.c_str());

    for (i = 0;i<num_matrices;i++)
        {
        vnl_matrix<double>TM(3,3,0.0);
        ifst >> TM;
        vcl_cout << TM << vcl_endl;
        transformation_matrices.push_back(TM);
        }
    return transformation_matrices;
    }

vcl_vector<vehicle_model> read_models(vcl_string vehicle_model_info,int num_models)
    {
    int i,j,k;
    double x,y;
    char ch;
    vcl_ifstream ifst(vehicle_model_info.c_str());
    vcl_vector<vehicle_model> model_vec;
    vcl_vector<vsol_rectangle_2d> box1,box2,box3;

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



int perform_pca(vnl_matrix<double>alg)
    {
    unsigned int i,num_rows = alg.rows(),num_elements = alg.columns();
    vnl_matrix<double>modified_alg(num_rows,num_elements);
    vnl_vector<double>alg_row;

    vcl_vector<double> mean_vec(num_rows);

    vnl_matrix<double>covariance_matrix(num_rows,num_rows,0.0);

    for (i = 0;i<num_rows;i++)
        {
        alg_row = alg.get_row(i);
        mean_vec[i] = alg_row.mean();
        alg_row = alg_row - mean_vec[i];
        alg_row.normalize();
        modified_alg.set_row(i,alg_row);
        }
    covariance_matrix = modified_alg*modified_alg.transpose();
    vnl_svd<double>svd(covariance_matrix);

    vnl_matrix<double> u =  svd.U();

    vcl_cout << u << vcl_endl;
    return svd.rank();
    }


//function to put3x3  Lie algebra elements into a column vector of another matrix
void set_values(vnl_matrix<double> &alg,vnl_matrix<double>g,int i)
    {
    alg.put(0,i,g.get(0,0));
    alg.put(1,i,g.get(0,1));
    alg.put(2,i,g.get(0,2));
    alg.put(3,i,g.get(1,0));
    alg.put(4,i,g.get(1,1));
    alg.put(5,i,g.get(1,2));
    alg.put(6,i,g.get(2,0));
    alg.put(7,i,g.get(2,1));
    alg.put(8,i,g.get(2,2));
    }


int main(int argc,char **argv)
    {
    vcl_string vehicle_model_info = argv[1];
    vcl_string debug_info_file = argv[2];
    vcl_string intrinsic_mean_file = argv[3];

    int i,num_models = 7,r,c,num_rows = 3,num_cols = 3;

    vnl_matrix<double>G1(3,3,0.0);
    vnl_matrix<double>G2(3,3,0.0);
    vnl_matrix<double>G3(3,3,0.0);
    vnl_matrix<double>g1(3,3,0.0);
    vnl_matrix<double>g2(3,3,0.0);
    vnl_matrix<double>g3(3,3,0.0);

    // vnl_matrix<double>alg(9,3*num_models,0.0);

    vcl_ofstream debug_info(debug_info_file.c_str());
    vcl_ofstream intrinsic_mean_info(intrinsic_mean_file.c_str());

    vcl_vector<vehicle_model> model_vec = read_models(vehicle_model_info,num_models);

    vehicle_model ref_model(model_vec[0].engine(),model_vec[0].body(),model_vec[0].rear());
    vcl_vector<vnl_matrix<double> >G1_vec,G2_vec,G3_vec;

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

        g1 = get_Lie_algebra(G1);
        g2 = get_Lie_algebra(G2);
        g3 = get_Lie_algebra(G3);

        G1_vec.push_back(G1);
        G2_vec.push_back(G2);
        G3_vec.push_back(G3);

        /*debug_info <<" "<< i << "th model Group elements" << vcl_endl;
        debug_info << "G1: "<<vcl_endl;
        debug_info << G1 << vcl_endl;
        debug_info << "G2: "<<vcl_endl;
        debug_info << G2 << vcl_endl;
        debug_info << "G3: "<<vcl_endl;
        debug_info << G3 << vcl_endl;*/


        debug_info << G1 << vcl_endl;

        debug_info << G2 << vcl_endl;

        debug_info << G3 << vcl_endl;

        }


    double     M1_x2 = ref_model.engine().get_max_x();
    double   M1_x4 = ref_model.body().get_max_x();

    vnl_matrix<double> IM_G1,IM_G2,IM_G3;

    calculate_coupled_intrinsic_mean(G1_vec,G2_vec,G3_vec,M1_x2,M1_x4,IM_G1,IM_G2,IM_G3);


    debug_info << "IM_G1: "<<vcl_endl;
    debug_info << IM_G1 << vcl_endl;
    debug_info << "IM_G2: "<<vcl_endl;
    debug_info << IM_G2 << vcl_endl;
    debug_info << "IM_G3: "<<vcl_endl;
    debug_info << IM_G3 << vcl_endl;

    vcl_cout << "IM_G1: "<<vcl_endl;
    vcl_cout << IM_G1 << vcl_endl;
    vcl_cout << "IM_G2: "<<vcl_endl;
    vcl_cout << IM_G2 << vcl_endl;
    vcl_cout << "IM_G3: "<<vcl_endl;
    vcl_cout << IM_G3 << vcl_endl;

    vnl_vector<double> params;

    debug_info << get_variance(G1_vec,IM_G1) << vcl_endl;
    debug_info << get_variance(G2_vec,IM_G2) << vcl_endl;
    debug_info << get_variance(G3_vec,IM_G3) << vcl_endl;

    vnl_matrix<double> scale_values(6,num_models);

    for (i=0;i<num_models;i++)
        {
        vnl_matrix<double> G1_proj = vnl_matrix_inverse<double>(IM_G1)*G1_vec[i];
        vnl_matrix<double> G2_proj = vnl_matrix_inverse<double>(IM_G2)*G2_vec[i];
        vnl_matrix<double> G3_proj = vnl_matrix_inverse<double>(IM_G3)*G3_vec[i];

        tx = M1_x2*(G2_proj.get(0,0) - G1_proj.get(0,0));
        G1_proj.put(0,2,tx);
        tx = M1_x4*(G2_proj.get(0,0) - G3_proj.get(0,0));
        G3_proj.put(0,2,tx);

        G1_vec[i] = G1_proj;
        G2_vec[i] = G2_proj;
        G3_vec[i] = G3_proj;

        debug_info << G1_vec[i] << vcl_endl;
        debug_info << G2_vec[i] << vcl_endl;
        debug_info << G3_vec[i] << vcl_endl;
        }


    /* for (i=0;i<num_models;i++)
    {
    intrinsic_mean_info << G1_vec[i] << vcl_endl;
    intrinsic_mean_info << G2_vec[i] << vcl_endl;
    intrinsic_mean_info << G3_vec[i] << vcl_endl;
    }*/

    //perform pca on the 6D space formed by the scale values for the three boxes


    for (i = 0;i<num_models;i++)
        {
        double sx1 = G1_vec[i].get(0,0);
        double sy1 = G1_vec[i].get(1,1);
        double sx2 = G2_vec[i].get(0,0);
        double sy2 = G2_vec[i].get(1,1);
        double sx3 = G3_vec[i].get(0,0);
        double sy3 = G3_vec[i].get(1,1);

        scale_values.put(0,i,sx1);
        scale_values.put(1,i,sy1);
        scale_values.put(2,i,sx2);
        scale_values.put(3,i,sy2);
        scale_values.put(4,i,sx3);
        scale_values.put(5,i,sy3);

        // debug_info << sx1 <<" " << sy1<<" " << sx2 <<" " <<  sy2 <<" " <<sx3 <<" " <<sy3 << vcl_endl;
        }

    // perform_pca(scale_values);

    vnl_vector<double> initial_params,params_freeze;

    params_freeze.set_size(num_models+81);
    initial_params.set_size(num_models+81);
    initial_params.fill(0.0);

    for (i=0;i<num_models;i++)
        {
        params_freeze[i] = 0;
        }

    for (i=num_models;i<num_models +81;i++)
        {
        params_freeze[i] = 0;
        }

    //initialize all the parameters corresponding to scalar values 

    i = 0;
    initial_params[i++] = -0.16;
    initial_params[i++] = -0.2;
    initial_params[i++] = 0.29;
    initial_params[i++] = -0.52;
    initial_params[i++] = 0.09;
    initial_params[i++] = 0.52;
    initial_params[i++] = -0.03;

    //initialize all the parameters corresponding to generator

   i = num_models;
   initial_params[i] = 0.12329;
   i = i+10;
   initial_params[i] = -0.7498;
   i = i+20;
   initial_params[i] = -0.26442;
   i = i+10;
   initial_params[i] = -0.34324;
   i = i+20;
   initial_params[i] = -0.26485;
   i = i+10;
   initial_params[i] = -0.40587;

    // vnl_matrix<double>princp_geodesic = get_geodesic(G1_vec,initial_params,params_freeze,params);   

    vcl_vector<vnl_matrix<double> >G_vec;

    for (i = 0;i<G1_vec.size();i++)
        {
        vnl_matrix<double>G(9,9,0.0);

        for (r=0;r<3;r++)
            {
            for (c=0;c<3;c++)
                {
                G.put(r,c,G1_vec[i].get(r,c));
                G.put(r+num_rows,c+num_cols,G2_vec[i].get(r,c));
                G.put(r+2*num_rows,c+2*+num_cols,G3_vec[i].get(r,c));
                }
            }
        G.put(0,2,0.0);
        G.put(1,2,0.0);
        G.put(3,5,0.0);
        G.put(4,5,0.0);
        G.put(6,8,0.0);
        G.put(7,8,0.0);

        G_vec.push_back(G);
        }


    vnl_matrix<double>princp_geodesic = get_geodesic_9x9(G_vec,initial_params,params_freeze,params);   

    debug_info << "princp_geodesic:  "<<vcl_endl;
    debug_info <<  princp_geodesic  <<vcl_endl;

    debug_info << "scalars associated with projections: " << vcl_endl;
    vcl_cout  << params << vcl_endl;
    debug_info << params << vcl_endl;

    return 0;
    }

