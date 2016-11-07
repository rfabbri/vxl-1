#include <testlib/testlib_test.h>

#include <vcl_cstring.h>
#include <vul/vul_get_timestamp.h>
#include <vnl/vnl_random.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_print.h>
#include <int_cedt3d/cedt3d.h>
#include <int_cedt3d/dbil3d_cedt.h>
#include <int_cedt3d/dt_by_3d_cedt.h>

void randomPoints_noncube(vil3d_image_view<double>& im, 
const int& dimx,
const int& dimy,
const int& dimz, 
const int& numSources, 
vnl_random& rand)
{
        im.set_size(dimx,dimy,dimz);
        im.fill(9);

        int n = 0;
        int i,j,k;
        while (n < numSources){
          i = (int)(2 + rand.drand32()*(dimx-4));
          j = (int)(2 + rand.drand32()*(dimy-4));
          k = (int)(2 + rand.drand32()*(dimz-4));
          if(im(i,j,k) < 1) continue; 
          else{ im(i,j,k) = 0; n++; }
        }
}


void randomPoints(vil3d_image_view<double>& im, const int& dim, const int& numSources, vnl_random& rand)
{
        im.set_size(dim,dim,dim);
        im.fill(9);

        int n = 0;
        int i,j,k;
        while (n < numSources){
          i = (int)(2 + rand.drand32()*(dim-4));
          j = (int)(2 + rand.drand32()*(dim-4));
          k = (int)(2 + rand.drand32()*(dim-4));
          if(im(i,j,k) < 1) continue; 
          else{ im(i,j,k) = 0; n++; }
        }
}


dist_sq_t minDist(const int& check_i, const int& check_j, const int& check_k, 
                  const vil3d_image_view<double>& im)
{

        if(im(check_i,check_j,check_k) == 0) return 0;

        double minDist = -1; 
        dist_sq_t dist = 0;

        for(int k=0; k<static_cast<int>(im.nk()); k++) {
        for(int j=0; j<static_cast<int>(im.nj()); j++) {
        for(int i=0; i<static_cast<int>(im.ni()); i++) {
                if(im(i,j,k) == 0){
                        dist = (check_i-i)*(check_i-i) +(check_j-j)*(check_j-j) +(check_k-k)*(check_k-k);
                        if(dist < minDist || minDist <0 ){ 
                                minDist = dist;
                        }
                }    
        } 
        }
        }
        return static_cast<unsigned>(minDist);
}


MAIN( test_dbil3d_cedt )
{
        START ("3D Contour-based DT");
        int secs, msecs;
        vul_get_timestamp(secs,msecs);
        vnl_random rand(secs + msecs);

        bool cube = 0;

        vil3d_image_view<double> dt_image;
if(cube){
        double some_number = rand.drand32(10,25);
        int dim = (int)(some_number); 
        int nsources =  (int)(rand.drand32(1,some_number));
        vcl_cout << "Dimension : " << dim << " Sources : " << nsources << vcl_endl;
        randomPoints(dt_image,dim,nsources, rand);
}
else{
        double some_number = rand.drand32(10,25);
        int dimx = (int)(some_number); 
        some_number = rand.drand32(10,25);
        int dimy = (int)(some_number); 
        some_number = rand.drand32(10,25);
        int dimz = (int)(some_number); 

        int nsources =  (int)(rand.drand32(1,some_number));
        vcl_cout << "Dimension : " << dimx << " " << dimy << " " << dimz 
        << " Sources : " << nsources << vcl_endl;
        randomPoints_noncube(dt_image,dimx,dimy,dimz,nsources, rand);
}


        vil3d_image_view<double> brute_dt_image(dt_image.ni(),dt_image.nj(),dt_image.nk());

         vcl_cout << "* computation of exact DT" << vcl_endl;
        /*-------------------
         * computation of exact DT
         */
        double diff;
        for(int k=0; k<static_cast<int>(brute_dt_image.nk()); k++) {
                for(int j=0; j<static_cast<int>(brute_dt_image.nj()); j++) {
                        for(int i=0; i<static_cast<int>(brute_dt_image.ni()); i++) {
                                brute_dt_image(i,j,k) = minDist(i,j,k,dt_image);
                        } 
                }
        }
        //----------------------------


//        vil3d_print_all(vcl_cout, brute_dt_image);

        vcl_cout << "dbil3d_cedt " << vcl_endl;
        vil3d_image_view<int> offsets;
        bool take_sqrt = false;
        bool get_offsets = true;
        dbil3d_cedt(dt_image, offsets, take_sqrt,get_offsets);

 //       vil3d_print_all(vcl_cout, dt_image);

         vcl_cout << "differences " << vcl_endl;
        double sumsq  = 0;
        double max = -1;

        for(int k=0; k<static_cast<int>(brute_dt_image.nk()); k++) {
                for(int j=0; j<static_cast<int>(brute_dt_image.nj()); j++) {
                        for(int i=0; i<static_cast<int>(brute_dt_image.ni()); i++) {
                                diff = vcl_fabs(brute_dt_image(i,j,k) - dt_image(i,j,k));
                                sumsq +=  diff;
                                if( diff > max || max == -1)
                                            max = diff;
                        } 
                }
        }

        TEST_NEAR("Max error ", max,0, 0.001);
        TEST_NEAR("Sum of squared differences ", sumsq,0, 0.001);
        SUMMARY();
}
