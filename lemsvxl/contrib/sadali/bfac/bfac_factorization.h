#include <vnl/algo/vnl_svd.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vector>
#include <utility>
#include <string>
#include <vnl/vnl_matrix.h>
#include <bmvl/brct/brct_algos.h>


class bfac_reconstructor
{
public:
        bfac_reconstructor();
        bfac_reconstructor( std::vector<std::vector<vsol_point_2d_sptr > >  input_feature_pts, 
            const std::string vrml_f_name);
        ~bfac_reconstructor();
    vnl_matrix<double> 
        matrix_block_segment(vnl_matrix<double> &Qwhole, vnl_matrix<double> &Qk, int iteration,
        std::vector<std::pair<int,int> > &permutations, int *ident);
        int *get_indices();

        void  detect_blocks(vnl_matrix<double> &Qsorted);

         std::vector<int> get_blocks();
         std::vector <vsol_point_3d_sptr> get_reconst();
        


private:
        std::vector<std::vector < vsol_point_2d_sptr > > feature_pts;
        vnl_matrix <double> meas_matrix;
        int *id;
        std::vector<int> blocks;
        
}
;
