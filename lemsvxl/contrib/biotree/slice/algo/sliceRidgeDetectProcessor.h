#if !defined(SLICE_RIDGEDETECT_PROCESSOR_H_)
#define SLICE_RIDGEDETECT_PROCESSOR_H_ 

#include <vcl_vector.h>
#include <vcl_string.h>
#include <slice/sliceProcessor.h>
#include <vil3d/vil3d_image_view.h>
#include <vil/vil_image_view.h>

class sliceRidgeDetectProcessor  : public sliceProcessor<float>
{
  public:
    sliceRidgeDetectProcessor( const float& epsilon) : epsilon_(epsilon) {};
    virtual vcl_vector<float*> process(const vcl_vector< vcl_vector<float*> >& slice_sets, int w, int h, int slice);
    virtual const int nstreams() const { return 3;}
    virtual const int nslices() const{ return 5;}
    virtual const int noutputs() const{ return 3;}
    virtual const vcl_string name() const{ return vcl_string("sliceRidgeDetectProcessor");}
  protected:
    
    void hessian_decompose_oneslice(const vil_image_view<float>& Im_xx,
                                    const vil_image_view<float>& Im_yy,
                                    const vil_image_view<float>& Im_zz,
                                    const vil_image_view<float>& Im_xy,
                                    const vil_image_view<float>& Im_xz,
                                    const vil_image_view<float>& Im_yz,
                                     vil_image_view<float>& e1,
                                     vil_image_view<float>& e2,
                                     vil_image_view<float>& e3,
                                     vil_image_view<float>& l1,
                                     vil_image_view<float>& l2,
                                     vil_image_view<float>& l3);

    void pickLargerEigenvalues(const vil_image_view<float>& e1,
                               const vil_image_view<float>& e2,
                               const vil_image_view<float>& e3,
                               const vil_image_view<float>& l1,
                               const vil_image_view<float>& l2,
                               const vil_image_view<float>& l3,
                               vil_image_view<float>& largest_eigen_x,
                               vil_image_view<float>& largest_eigen_y,
                               vil_image_view<float>& largest_eigen_z,
                               vil_image_view<float>& largest_lambda,
                               vil_image_view<float>& second_largest_eigen_x,
                               vil_image_view<float>& second_largest_eigen_y,
                               vil_image_view<float>& second_largest_eigen_z,
                               vil_image_view<float>& second_largest_lambda,
                               vil_image_view<float>& third_eigenvector);

    void computeRho3d( 
                     const vil3d_image_view<float>& gradient_x,
                     const vil3d_image_view<float>& gradient_y,
                     const vil3d_image_view<float>& gradient_z, 
                     const vil_image_view<float>& eigen_x,
                     const vil_image_view<float>& eigen_y,
                     const vil_image_view<float>& eigen_z,
                     const vil_image_view<float>& lambda,
                     const int & z_index,
                     vil_image_view<int>& rho);

  private:
    float epsilon_;
};

#endif 
