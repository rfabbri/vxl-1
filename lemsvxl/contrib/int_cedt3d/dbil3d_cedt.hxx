#ifndef dbil3d_cedt_txx_
#define dbil3d_cedt_txx_

#include <vcl_iostream.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_new.h>
#include <vil3d/vil3d_convert.h>

#include "dbil3d_cedt.h"
#include "dt_by_3d_cedt.h"
#include "cedt3d.h"

template <class T>
void dbil3d_cedt(vil3d_image_view<T>& image, vil3d_image_view<int>& offsets, bool take_sqrt, bool get_offsets) 
{
        if(!image.is_contiguous()){
                vcl_cerr << "dbil3d_cedt requires contiguous image" << vcl_endl;
                return;
        }
        if(!image.nplanes() == 1){
                vcl_cerr << "dbil3d_cedt requires single plane images" << vcl_endl;
                return;
        }


        unsigned ni = image.ni();
        unsigned nj = image.nj();
        unsigned nk = image.nk();
        Dimension    dim;
        dim.iXdim = ni+4;
        dim.iYdim = nj+4;
        dim.iZdim = nk+4;
        dim.iSliceSize  = dim.iXdim * dim.iYdim ;
        dim.iWSpaceSize = dim.iZdim*dim.iSliceSize;


        //create an image with padded boundaries (assumed by dt_by_3d_cedt)
        vil3d_image_resource_sptr imResource = vil3d_new_image_resource(dim.iXdim,
                                                                 dim.iYdim,
                                                                 dim.iZdim,
                                                                 1,
                                                                 VIL_PIXEL_FORMAT_UINT_32);

        //fill the boundaries with 9 (a generic non-source value)
        { 
        vil3d_image_view<vxl_uint_32> view =    imResource->get_view(0,dim.iXdim,
                                                                     0,dim.iYdim,
                                                                     0,dim.iZdim);
        view.fill(9);
        imResource->put_view(view,0,0,0);
        }

        {
        //cast the input data into the format required by dt_by_3d_cedt
        vil3d_image_view<vxl_uint_32>     subview = imResource->get_view(2,ni,2,nj,2,nk);
        vil3d_convert_cast(image,subview);
        imResource->put_view(subview,2,2,2);
        }

        vil3d_image_view<vxl_uint_32> view = imResource->get_view(0,dim.iXdim,0,dim.iYdim,0,dim.iZdim);
        int iFlagV = 0;
        double dSourceRange = 0;

        offset_t *fdx,*fdy,*fdz;

        //call 3d DT code with origin ptr of padded volume 
        DTby3dCEDT(view.origin_ptr(), &dim, iFlagV, dSourceRange,fdx,fdy,fdz);

        //cast the result back for return value
        imResource->put_view(view,0,0,0);
        vil3d_image_view<vxl_uint_32> subview = imResource->get_view(2,ni,2,nj,2,nk);
        vil3d_convert_cast(subview,image);

        if(take_sqrt){
                for(unsigned k = 0; k < nk; ++k){
                        for(unsigned j = 0; j < nj; ++j){
                                for(unsigned i = 0; i < ni; ++i){
                                        image(i,j,k) = (T)vcl_sqrt((float)image(i,j,k));
                                }
                        }
                }

        }

        if(get_offsets){
                /*
                 * populate planes of the offset image
                 * (taking padding into account)
                 */
                offsets.set_size(ni,nj,nk,3);
                /*offset_t* xslice = fdx;
                offset_t* yslice = fdy;
                offset_t* zslice = fdz;*/

                for(unsigned k = 0; k < nk; ++k){
                        for(unsigned j = 0; j < nj; ++j){
                                for(unsigned i = 0; i < ni; ++i){
                                        unsigned idx = (k+2)*dim.iSliceSize + (j+2)*dim.iXdim + (i+2);
                                        offsets(i,j,k,0) = fdx[idx];
                                        offsets(i,j,k,1) = fdy[idx];
                                        offsets(i,j,k,2) = fdz[idx];
                                }
                        }
                }
        }

        delete [] fdx;
        delete [] fdy;
        delete [] fdz;
}
#define EPSILON 0.001
template <class T>
void dbil3d_signed_cedt(vil3d_image_view<T>& image, vil3d_image_view<int>& offsets, bool take_sqrt, bool get_offsets) 
{
        vil3d_image_view<T> tmp;

        //compute the "outside" distance transform w/ original image
        vcl_cerr << "\toutside DT " << vcl_endl;    
        dbil3d_cedt(image,offsets,take_sqrt, get_offsets);

        //create a complement image by swapping source and background values
        tmp.set_size(image.ni(),image.nj(),image.nk(),1);
        for(unsigned k = 0; k < image.nk(); ++k){
        for(unsigned j = 0; j < image.nj(); ++j){
        for(unsigned i = 0; i < image.ni(); ++i){
            if(image(i,j,k) > -EPSILON && image(i,j,k) < EPSILON)
                    tmp(i,j,k) = 9;
            else
                    tmp(i,j,k) = 0;    
        }
        }
        }

        vcl_cerr << "\tinside DT " << vcl_endl;    
        //compute the "inside" distance transform w/ complement image
        vil3d_image_view<int> tmp_offsets;
        dbil3d_cedt(tmp,tmp_offsets,take_sqrt,get_offsets);

        //stitch outside & inside DTs together to create a signed DT
        for(unsigned k = 0; k < image.nk(); ++k){
        for(unsigned j = 0; j < image.nj(); ++j){
        for(unsigned i = 0; i < image.ni(); ++i){
                    if(image(i,j,k) < EPSILON){
                        image(i,j,k) =(T)(0.5-tmp(i,j,k));
                    }
                    else{ image(i,j,k) = (T)(image(i,j,k)- 0.5);    }
        }    
        }
        }


        if(get_offsets){
                //transfer offset values from inverse offset image
                //for those voxels that are "inside" (dt value <=0)
                for(unsigned k = 0; k < image.nk(); ++k){
                for(unsigned j = 0; j < image.nj(); ++j){
                for(unsigned i = 0; i < image.ni(); ++i){
                    if(image(i,j,k) < EPSILON){
                        for(int p = 0; p < static_cast<int>(offsets.nplanes()); ++p)
                                    offsets(i,j,k,p) = tmp_offsets(i,j,k,p);
                    }
                }
                }
                }    
        }
}


#undef DBIL3D_CEDT_INSTANTIATE
#define DBIL3D_CEDT_INSTANTIATE(T) \
template void dbil3d_cedt(vil3d_image_view<T>& image, vil3d_image_view<int>& offsets, bool take_sqrt, bool get_offsets);\
template void dbil3d_signed_cedt(vil3d_image_view<T>& image, vil3d_image_view<int>& offsets,bool take_sqrt, bool get_offsets);

#endif // dbil3d_cedt_txx_
