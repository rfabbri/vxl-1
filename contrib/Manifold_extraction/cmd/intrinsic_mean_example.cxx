//  This is under contrib\Manifold_extraction\cmd

//  Brief:  An example to find out the intrinsic mean of a given set of Lie group elements
//(currently handles isotropic scaling,rotation and translation) 

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

// reading the transformation matrices from an input text file
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

int main(int argc,char **argv)
    {

    vcl_string trans_matrices_info = argv[1];
    vcl_string debug_info_file = argv[2];

    //usage: trans_matrices_info are the transformation matrices of three box models
    //the intrinsic mean is written down in debug_info_file 

    int i,num_matrices = 3;

    vcl_ofstream debug_info(debug_info_file.c_str());

    // read the transformation matrices from a text file
    vcl_vector<vnl_matrix<double> >test_TM = read_transformation_matrices(trans_matrices_info,num_matrices);

    // print out the Lie algebra conversion of the transformation matrices 
    // and also convert the algebra elements back to group elements 
    // and print them out for sanity check

    for (vcl_vector<vnl_matrix<double> >::iterator it = test_TM.begin();it != test_TM.end();it++)
        {
        vnl_matrix<double>g = get_Lie_algebra(*it);
        vnl_matrix<double>G = get_Lie_group(g);

        debug_info << "algebra: " << vcl_endl;
        debug_info << g << vcl_endl;
        debug_info <<  "group: " << vcl_endl;
        debug_info <<  G << vcl_endl;

        }

    vnl_matrix<double> test_IM = calculate_intrinsic_mean(test_TM);

    //print out the intrinsic mean 

    debug_info << "test_IM: "<<vcl_endl;
    debug_info << test_IM << vcl_endl;


    return 0;
    }

