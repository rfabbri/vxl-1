
//   an example to find out the generator of an optimal 1-D sub manifold which explains the variance observed in a 
//   given set of Lie group elements.

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

    int i,num_matrices = 3;

    vcl_ofstream debug_info(debug_info_file.c_str());

    // read the transformation matrices from a text file
    //  vcl_vector<vnl_matrix<double> >test_TM = read_transformation_matrices(trans_matrices_info,num_matrices);

    // define a generator and pick points along that generator

    vcl_vector<vnl_matrix<double> >test_TM;
    vnl_matrix<double>A(3,3,0.0);
    A.put(0,0,1);
    A.put(1,1,2);
    //A.put(0,2,5);
    

    for (i = 0;i<10;i++)
        {
        vnl_matrix<double>G = get_Lie_group(A*0.1*i);
        debug_info << "group element: " << vcl_endl;
        debug_info << G << vcl_endl;

         debug_info << "algebra element: " << vcl_endl;
        debug_info << get_Lie_algebra(G) << vcl_endl;

        test_TM.push_back(G);
        }

    vnl_vector<double> params;

    vnl_matrix<double> sub_manifold_generator = get_geodesic(test_TM,params);

    //print out the generator

    debug_info << "Generator: "<<vcl_endl;
    debug_info << sub_manifold_generator << vcl_endl;

    debug_info << "group element: "<<vcl_endl;
    debug_info << get_Lie_group(sub_manifold_generator) << vcl_endl;

    for (int i = 0;i<params.size();i++)
        debug_info << params[i] << vcl_endl;

    return 0;
    }
