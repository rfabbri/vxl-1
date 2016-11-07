#include "bfac_factorization.h"
#include <vsol/vsol_point_2d.h>
#include <vcl_cassert.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vnl/vnl_rational.h>

#include <vsol/vsol_point_3d.h>

#include <vcl_fstream.h>



bfac_reconstructor::bfac_reconstructor( vcl_vector<vcl_vector<vsol_point_2d_sptr > > input_feature_pts, const vcl_string vrml_f_name)
{
    vcl_vector<vsol_point_3d_sptr> world_points;
    unsigned int no_frames = input_feature_pts.size();
    assert(no_frames>0);
    vcl_vector<vcl_vector<vsol_point_2d_sptr> >::const_iterator frame_iter = input_feature_pts.begin();
    unsigned int no_pts = (input_feature_pts[0]).size();
 

   
       
    frame_iter = input_feature_pts.begin();
    
    assert(no_pts>0);
    unsigned int no_pts_tracked = no_pts;

    meas_matrix.set_size(2*no_frames, no_pts);
    vcl_cout<<"\n num of frames"<<no_frames<<"\n num of pts"<<no_pts<<"\n";
    
    
        
    for (unsigned int i = 0; (i<no_frames); i++)
    {
        assert(frame_iter!=input_feature_pts.end());
        double  centrx = 0.0;
        double  centry = 0.0;
        vcl_vector<vsol_point_2d_sptr>::const_iterator  point_iter = frame_iter->begin() ;
        
        
        for (unsigned int j = 0; j<no_pts_tracked; j++)
        {
            
             assert(point_iter!=frame_iter->end());
             centrx += ((input_feature_pts[i][j]->x())/(double)no_pts_tracked) ;
             centry += ((input_feature_pts[i][j]->y())/(double)no_pts_tracked) ;

  
            point_iter++;
        }
        point_iter = frame_iter->begin() ;
        for (unsigned int j = 0; j<no_pts_tracked; j++)
            {
            assert(point_iter!=frame_iter->end());
                
            meas_matrix[2*i][j] = (input_feature_pts[i][j]->x()) - centrx ;
            meas_matrix[(2*i)+1][j] = (input_feature_pts[i][j]->y() )  -centry ;
            point_iter++;
          //  vcl_cout<< "x:  "<<meas_matrix[2*i][j]<< "y:  "<<meas_matrix[(2*i)+1][j]<<"      ";
                }

        frame_iter++;


       }


        vnl_svd<double> svd_decomp(meas_matrix);
        vnl_diag_matrix<double> SingValMat = svd_decomp.W();
        vcl_cout<<"\nW="<<SingValMat<<"\n";
        unsigned int n = no_pts;
        vnl_matrix<double> Vnx3 = svd_decomp.V().extract(n,3);
       // for (unsigned int i = 3; i<n; i++)
           //     SingValMat.fill(0.0);
        vnl_matrix<double> sigma_sqrt(3,3);
        sigma_sqrt.fill(0.0);
        for (unsigned int m = 0; m<3; m++)

           
            sigma_sqrt[m][m] = vcl_sqrt(SingValMat[m]);
       vcl_cout<<sigma_sqrt<<"\n";

      //  vnl_matrix<double> world_pt_matrix = (Vnx3.transpose());
        vnl_matrix<double> world_pt_matrix = (sigma_sqrt)*(Vnx3.transpose());
    for (unsigned int i = 0; i<n ; i++) 
        world_points.push_back( new vsol_point_3d(world_pt_matrix[0][i] , world_pt_matrix[1][i] , world_pt_matrix[2][i]) );
    vcl_ofstream vrml_f(vrml_f_name.c_str());
    brct_algos::write_vrml_header(vrml_f);
    brct_algos::write_vrml_points(vrml_f, world_points);
 //   brct_algos::write_vrml_trailer(vrml_f);
    vrml_f.close();

    vnl_matrix<double> VxVtr(n,n);
    VxVtr= Vnx3*(Vnx3.transpose());
    vnl_matrix<double> VxVtr_copy(VxVtr);
    vcl_vector<vcl_pair<int,int > > permut;
    vnl_matrix<double> dummy(1,1);
    vnl_matrix<double> test(4,4);
    test[0][0] = 3.0;
    test[0][1] = 1.5;
    test[1][0] = 1.5;
    test[1][1] = 0.2;
    test[0][2] = 0.7;
    test[2][0] = 0.7;
    test[1][2] = 2.1;
    test[2][1] = 3.5;
    test[2][2] = 2.8;
    test[3][0] = 0.6;
    test[0][3] = 1.1;
    test[3][1] =4.2;
    test[1][3] = 4.2;
    test[3][2] = 1.3;
    test[2][3] = 1.3;
    test[3][3] = 1.9;

//Initializing id
    id = new int[n];
   for (int m=0;  ((unsigned)m)<n; m++)
       id[m] = m;










///Sort
    matrix_block_segment(VxVtr_copy, dummy, 0, permut,id);



     
   for (int m=0;  ((unsigned)m)<n; m++)
       vcl_cout<<"mth in block sq mat: "<<m<<" idth in orig mat id[m]: " <<  id[m]<<"\t";

//writing output to files
   // vcl_cout<<"result is \n"<<VxVtr;
   vcl_vector<vsol_point_3d_sptr> Q_matrix;
  
    for (unsigned int i = 0; i<n ; i++) 
        for (unsigned int j = 0; j<n ; j++) 
        Q_matrix.push_back( new vsol_point_3d(j , i, (VxVtr[i][j])*100.0) );
    vcl_ofstream vrml_fQ((vrml_f_name+"_Q_.vrml").c_str());
    brct_algos::write_vrml_header((vrml_fQ));
   
    brct_algos::write_vrml_points((vrml_fQ), Q_matrix);
    vrml_fQ.close();
    
     vcl_vector<vsol_point_3d_sptr> Q_matrixsort;
    for (unsigned int i = 0; i<n ; i++) 
        for (unsigned int j = 0; j<n ; j++) 
        Q_matrixsort.push_back( new vsol_point_3d(j, i, 100.0*VxVtr_copy[i][j]));

    vcl_ofstream vrml_fQsort((vrml_f_name+"_Q_sort.vrml").c_str());
    brct_algos::write_vrml_header((vrml_fQsort));
    brct_algos::write_vrml_points((vrml_fQsort), Q_matrixsort);
    vrml_fQsort.close();    
    detect_blocks(VxVtr_copy);



}


vnl_matrix<double> bfac_reconstructor::matrix_block_segment(vnl_matrix<double> &Qwhole, vnl_matrix<double> &Qk, int iteration, vcl_vector<vcl_pair<int,int> > &permutations, int * ident)
{

if (iteration==0)
        {
        vnl_matrix<double> Qkp(1,1,Qwhole[0][0]);
        
        return matrix_block_segment(Qwhole, Qkp, iteration+1, permutations,ident);
    
}
int N = Qwhole.rows();
int k = Qk.rows();
if (N==k+1)
    return Qwhole;
#if 0
if (N==k+1)
    {vnl_matrix<double> Qkp(k+1,k+1);
     Qkp.update(Qk);
     Qkp.set_row(k, Qwhole.get_row);
     Qkp.set_column(k, Qwhole.get_column(N-1));
     return Qkp;
    }
#endif
else
    {
  
    vnl_matrix<double> Qkp(k+1,k+1);
   
    
    
    double max_cost = 0.0;
    int  max_index = -1;
     Qkp.update(Qk);
     for (int i = (k); i< N; i++)
         {
         vnl_matrix<double>  sub_matrix = Qwhole.extract(k,1,0,i);
         
         double cost  = sub_matrix.frobenius_norm();
           if (cost>max_cost)
               { 
               max_index = i;
               max_cost = cost;
               }
         }
     permutations.push_back(vcl_pair<int,int> (k,max_index));
     vcl_cout<<"Swap "<<k<<"  with "<<max_index<<" (starting from zero) \n";
     int temp_index;
     temp_index = ident[max_index];
     ident[max_index] = ident[k];
     ident[k] = temp_index;
     vnl_vector<double> temp = Qwhole.get_column(max_index);
     Qwhole.set_column(max_index, Qwhole.get_column(k));
     Qwhole.set_column(k, temp);
     Qkp.set_column(k,temp);
     
     
     temp = Qwhole.get_row(max_index);
     Qwhole.set_row(max_index, Qwhole.get_row(k));
     Qwhole.set_row(k, temp);
     Qkp.set_row(k,temp);

     //vcl_cout<<"Qkp in  iteration  "<<iteration<<"\n"<<Qkp;
     
     return matrix_block_segment(Qwhole, Qkp, iteration+1, permutations, ident);
     }
    



     
     

    }
int *bfac_reconstructor::get_indices()
    {
    return id;
    }

void    
bfac_reconstructor::detect_blocks(vnl_matrix<double> &Qsorted)

    {
    int N = Qsorted.rows();
    bool found_possible_block = false;
    double lastblock_rank = 0.0;
    int last_block = 0;
    int possible_block = -1;
    double error = 0.1;
    blocks.clear();
    for (int i =0 ; i<N; i++)
    {
        double rank_approx = Qsorted.extract(i, i, last_block+1,last_block+1).frobenius_norm();
        if (rank_approx>3.0+error)
        {
            if (found_possible_block == false)
            {
                last_block = i;
                blocks.push_back(last_block);
                found_possible_block = false;
                possible_block = -1;
                
            }
            else 
                {
                last_block = possible_block;
                blocks.push_back(last_block);
                found_possible_block =false;
                possible_block = -1;
                }
        }
            else
                if (found_possible_block == false)
                {
                    if (fabs(rank_approx - round(rank_approx))<error)
                    {
                        found_possible_block = true;
                        lastblock_rank = rank_approx;
                        possible_block = i;

                        if (round(rank_approx)==3.0)
                        {
                        found_possible_block = false;
                        blocks.push_back(last_block);
                        last_block = i;
                        blocks.push_back(last_block);
                        possible_block = -1;
                        }
                    }
                }
                else
                    {
                    if (fabs(rank_approx - round(rank_approx))<error)
                        if (round(rank_approx)==3.0)
                        {
                            found_possible_block = false;
                            last_block = i;
                            possible_block = -1;
                            blocks.push_back(last_block);
                        }
                        else 
                            possible_block = i;
                     }
        }




    for (int i = 0; i<blocks.size(); i++)
        vcl_cout<<i<<"th block     at  "<< blocks[i]<<"th feature"<<"\n";
    

    }


 vcl_vector<int>  bfac_reconstructor::get_blocks( )
    {
      return blocks;
    }
 vcl_vector <vsol_point_3d_sptr> bfac_reconstructor::get_reconst()
 {
     vcl_cout<<"vcl_vector <vsol_point_3d_sptr> bfac_reconstructor::get_reconst() TODO\n";
     vcl_vector <vsol_point_3d_sptr> dummy;
     return dummy;
 }
