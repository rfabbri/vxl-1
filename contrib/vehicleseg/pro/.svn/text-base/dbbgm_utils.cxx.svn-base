#include "dbbgm_utils.h"

void  mask_nonoverlapping_region(vil_image_view<float> im,vgl_point_2d<double> p,
                                        vgl_vector_2d<double> u,vgl_vector_2d<double> v)
{
    int ni=im.ni();
    int nj=im.nj();

    for(int i=0;i<ni;i++)
        for(int j=0;j<nj;j++)
        {
            vgl_point_2d<double> p1=p+i*u+j*v;
            if(!(p1.x()<ni && p1.x()>=0 && p1.y()<nj && p1.y()>=0))
                im(i,j)=0.0;
        }
}
