#include <vnl/algo/vnl_svd.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vcl_string.h>
#include <vnl/vnl_matrix.h>
#include <bmvl/brct/brct_algos.h>


class bfac_reconstructor
{
public:
        bfac_reconstructor();
        bfac_reconstructor( vcl_vector<vcl_vector<vsol_point_2d_sptr > >  input_feature_pts, 
            const vcl_string vrml_f_name);
        ~bfac_reconstructor();
    vnl_matrix<double> 
        matrix_block_segment(vnl_matrix<double> &Qwhole, vnl_matrix<double> &Qk, int iteration,
        vcl_vector<vcl_pair<int,int> > &permutations, int *ident);
        int *get_indices();

        void  detect_blocks(vnl_matrix<double> &Qsorted);

         vcl_vector<int> get_blocks();
         vcl_vector <vsol_point_3d_sptr> get_reconst();
        


private:
        vcl_vector<vcl_vector < vsol_point_2d_sptr > > feature_pts;
        vnl_matrix <double> meas_matrix;
        int *id;
        vcl_vector<int> blocks;
        
}
;
