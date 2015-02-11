// This is brcv/shp/dbskfg/algo/dbskfg_compute_sift.cxx

//:
// \file
#include <dbskfg/algo/dbskfg_compute_sift.h>
#include <dbsk2d/dbsk2d_utils.h>
#include <vil/vil_bilin_interp.h>
#include <vcl_cstring.h>
#include <vl/mathop.h>

dbskfg_compute_sift::dbskfg_compute_sift
(        
    vgl_point_2d<double> model_pt,
    double model_radius,
    double model_theta,
    double model_scale_ratio,
    VlSiftFilt* model_sift_filter,
    vgl_point_2d<double> query_pt,
    double query_radius,
    double query_theta,
    double query_scale_ratio,
    VlSiftFilt* query_sift_filter,
    vl_sift_pix* model_red_grad_data,
    vl_sift_pix* query_red_grad_data,
    vl_sift_pix* model_green_grad_data,
    vl_sift_pix* query_green_grad_data,
    vl_sift_pix* model_blue_grad_data,
    vl_sift_pix* query_blue_grad_data
    ):model_pt_(model_pt),
    model_radius_(model_radius),
    model_theta_(model_theta),
    model_scale_ratio_(model_scale_ratio),
    model_sift_filter_(model_sift_filter),
    query_pt_(query_pt),
    query_radius_(query_radius),
    query_theta_(query_theta),
    query_scale_ratio_(query_scale_ratio),
    query_sift_filter_(query_sift_filter),
    distance_(0.0)
{
    model_red_grad_mod_.set_to_memory(&model_red_grad_data[0],
                                      model_sift_filter_->width,
                                      model_sift_filter_->height,
                                      1,
                                      2,
                                      2.0*model_sift_filter_->width,
                                      0);

    model_red_grad_angle_.set_to_memory(&model_red_grad_data[1],
                                        model_sift_filter_->width,
                                        model_sift_filter_->height,
                                        1,
                                        2,
                                        2.0*model_sift_filter_->width,
                                        0);

    model_green_grad_mod_.set_to_memory(&model_green_grad_data[0],
                                        model_sift_filter_->width,
                                        model_sift_filter_->height,
                                        1,
                                        2,
                                        2.0*model_sift_filter_->width,
                                        0);

    model_green_grad_angle_.set_to_memory(&model_green_grad_data[1],
                                          model_sift_filter_->width,
                                          model_sift_filter_->height,
                                          1,
                                          2,
                                          2.0*model_sift_filter_->width,
                                          0);

    model_blue_grad_mod_.set_to_memory(&model_blue_grad_data[0],
                                       model_sift_filter_->width,
                                       model_sift_filter_->height,
                                       1,
                                       2,
                                       2.0*model_sift_filter_->width,
                                       0);

    model_blue_grad_angle_.set_to_memory(&model_blue_grad_data[1],
                                         model_sift_filter_->width,
                                         model_sift_filter_->height,
                                         1,
                                         2,
                                         2.0*model_sift_filter_->width,
                                         0);


    query_red_grad_mod_.set_to_memory(&query_red_grad_data[0],
                                      query_sift_filter_->width,
                                      query_sift_filter_->height,
                                      1,
                                      2,
                                      2.0*query_sift_filter_->width,
                                      0);

    query_red_grad_angle_.set_to_memory(&query_red_grad_data[1],
                                        query_sift_filter_->width,
                                        query_sift_filter_->height,
                                        1,
                                        2,
                                        2.0*query_sift_filter_->width,
                                        0);

    query_green_grad_mod_.set_to_memory(&query_green_grad_data[0],
                                        query_sift_filter_->width,
                                        query_sift_filter_->height,
                                        1,
                                        2,
                                        2.0*query_sift_filter_->width,
                                        0);

    query_green_grad_angle_.set_to_memory(&query_green_grad_data[1],
                                          query_sift_filter_->width,
                                          query_sift_filter_->height,
                                          1,
                                          2,
                                          2.0*query_sift_filter_->width,
                                          0);

    query_blue_grad_mod_.set_to_memory(&query_blue_grad_data[0],
                                       query_sift_filter_->width,
                                       query_sift_filter_->height,
                                       1,
                                       2,
                                       2.0*query_sift_filter_->width,
                                       0);

    query_blue_grad_angle_.set_to_memory(&query_blue_grad_data[1],
                                         query_sift_filter_->width,
                                         query_sift_filter_->height,
                                         1,
                                         2,
                                         2.0*query_sift_filter_->width,
                                         0);

    vnl_vector<vl_sift_pix> model_vector(384,0.0);
    vnl_vector<vl_sift_pix> query_vector(384,0.0);


    compute_descriptors(model_pt_,
                        model_radius_/2.0,
                        model_theta_,
                        model_sift_filter_,
                        model_scale_ratio_,
                        model_red_grad_mod_,
                        model_red_grad_angle_,
                        model_green_grad_mod_,
                        model_green_grad_angle_,
                        model_blue_grad_mod_,
                        model_blue_grad_angle_,
                        model_vector);

    compute_descriptors(query_pt_,
                        query_radius_/2.0,
                        query_theta_,
                        query_sift_filter_,
                        query_scale_ratio_,
                        query_red_grad_mod_,
                        query_red_grad_angle_,
                        query_green_grad_mod_,
                        query_green_grad_angle_,
                        query_blue_grad_mod_,
                        query_blue_grad_angle_,
                        query_vector);
    
    model_vector.normalize();
    query_vector.normalize();
    
    vl_sift_pix result_final[1];


    VlFloatVectorComparisonFunction Chi2_distance =    
      vl_get_vector_comparison_function_f (VlDistanceChi2) ;

    vl_eval_vector_comparison_on_all_pairs_f(result_final,
                                             model_vector.size(),
                                             model_vector.data_block(),
                                             1,
                                             query_vector.data_block(),
                                             1,
                                             Chi2_distance);

    distance_ = 0.5*result_final[0];

} 


void dbskfg_compute_sift::compute_descriptors(
    vgl_point_2d<double> keypoint,
    double radius,
    double angle0,
    VlSiftFilt* filter,
    double scale_ratio,
    vil_image_view<vl_sift_pix>& red_grad_mod,
    vil_image_view<vl_sift_pix>& red_grad_angle,
    vil_image_view<vl_sift_pix>& green_grad_mod,
    vil_image_view<vl_sift_pix>& green_grad_angle,
    vil_image_view<vl_sift_pix>& blue_grad_mod,
    vil_image_view<vl_sift_pix>& blue_grad_angle,
    vnl_vector<vl_sift_pix>& output)
{

    int angle_bins=8;
    int spatial_bins=4;

    double const magnif = filter->magnif ;

    int          w      = filter->width ;
    int          h      = filter->height ;
    int const    xo     = 2 ;         /* x-stride */
    int const    yo     = 2 * w ;     /* y-stride */

    int          xi     = (int) (keypoint.x() + 0.5) ;
    int          yi     = (int) (keypoint.y() + 0.5) ;

    double const st0    = sin (angle0) ;
    double const ct0    = cos (angle0) ;
    double const SBP    = magnif * radius + vnl_math::eps;
    int    const W      = vcl_floor
        (sqrt(2.0) * SBP * (spatial_bins + 1) / 2.0 + 0.5) ;

    int const binto = 1 ;                          /* bin angle0-stride */
    int const binyo = angle_bins * spatial_bins ;  /* bin y-stride */
    int const binxo = angle_bins ;                 /* bin x-stride */

    int bin, dxi, dyi ;

    vl_sift_pix       *red_dpt;
    vl_sift_pix       *green_dpt;
    vl_sift_pix       *blue_dpt;

    /* check bounds */
    if(xi    <  0               ||
       xi    >= w               ||
       yi    <  0               ||
       yi    >= h -    1        )
    {
        return ;
    }

    vl_sift_pix red_descr[angle_bins*spatial_bins*spatial_bins];
    vl_sift_pix green_descr[angle_bins*spatial_bins*spatial_bins];
    vl_sift_pix blue_descr[angle_bins*spatial_bins*spatial_bins];

    /* clear descriptor */
    vcl_memset(red_descr, 0, 
               sizeof(vl_sift_pix) * angle_bins*spatial_bins*spatial_bins) ;
    vcl_memset(green_descr, 0, 
               sizeof(vl_sift_pix) * angle_bins*spatial_bins*spatial_bins) ;
    vcl_memset(blue_descr, 0, 
               sizeof(vl_sift_pix) * angle_bins*spatial_bins*spatial_bins) ;

    /* Center the scale space and the descriptor on the current keypoint.
     * Note that dpt is pointing to the bin of center (SBP/2,SBP/2,0).
     */
    red_dpt = red_descr + 
        (spatial_bins/2) * binyo + (spatial_bins/2) * binxo ;
    green_dpt = green_descr + 
        (spatial_bins/2) * binyo + (spatial_bins/2) * binxo ;
    blue_dpt = blue_descr + 
        (spatial_bins/2) * binyo + (spatial_bins/2) * binxo ;

    /*
     * Process pixels in the intersection of the image rectangle
     * (1,1)-(M-1,N-1) and the keypoint bounding box.
     */
    for(dyi =  vnl_math::max(- W,   - yi   ) ;
        dyi <= vnl_math::min(+ W, h - yi -1) ; ++ dyi)
    {
        
        for(dxi =  vnl_math::max(- W,   - xi   ) ;
            dxi <= vnl_math::min(+ W, w - xi -1) ; ++ dxi)
        {

            vgl_point_2d<double> interp_pt(dxi+xi,
                                           dyi+yi);

            // Process red channel
            vl_sift_pix red_mod     = vil_bilin_interp(red_grad_mod,
                                                       interp_pt.x(),
                                                       interp_pt.y());
            vl_sift_pix red_angle   = vil_bilin_interp(red_grad_angle,
                                                       interp_pt.x(),
                                                       interp_pt.y());
            vl_sift_pix red_theta   = angle0To2Pi (red_angle - angle0) ;

            // Process green channel
            vl_sift_pix green_mod     = vil_bilin_interp(green_grad_mod,
                                                         interp_pt.x(),
                                                         interp_pt.y());
            vl_sift_pix green_angle   = vil_bilin_interp(green_grad_angle,
                                                         interp_pt.x(),
                                                         interp_pt.y());
            vl_sift_pix green_theta   = angle0To2Pi (green_angle - angle0) ;


            // Process blue channel
            vl_sift_pix blue_mod     = vil_bilin_interp(blue_grad_mod,
                                                        interp_pt.x(),
                                                        interp_pt.y());
            vl_sift_pix blue_angle   = vil_bilin_interp(blue_grad_angle,
                                                        interp_pt.x(),
                                                        interp_pt.y());
            vl_sift_pix blue_theta   = angle0To2Pi (blue_angle - angle0) ;


            /* fractional displacement */
            vl_sift_pix dx = xi + dxi - keypoint.x();
            vl_sift_pix dy = yi + dyi - keypoint.y();

            /* get the displacement normalized w.r.t. the keypoint
               orientation and extension */
            vl_sift_pix nx = ( ct0 * dx + st0 * dy) / SBP ;
            vl_sift_pix ny = (-st0 * dx + ct0 * dy) / SBP ;
            vl_sift_pix nt_red   = angle_bins* red_theta   /(2 * vnl_math::pi);
            vl_sift_pix nt_green = angle_bins* green_theta /(2 * vnl_math::pi);
            vl_sift_pix nt_blue  = angle_bins* blue_theta  /(2 * vnl_math::pi);

            /* Do flat weighting of pixels ala dense sift */
            vl_sift_pix win=1.0;

            /* The sample will be distributed in 8 adjacent bins.
               We start from the ``lower-left'' bin. */
            int         binx = (int)vcl_floor (nx - 0.5) ;
            int         biny = (int)vcl_floor (ny - 0.5) ;
            int         bint_red   = (int)vcl_floor (nt_red) ;
            int         bint_green = (int)vcl_floor (nt_green) ;
            int         bint_blue  = (int)vcl_floor (nt_blue) ;
            vl_sift_pix rbinx = nx - (binx + 0.5) ;
            vl_sift_pix rbiny = ny - (biny + 0.5) ;
            vl_sift_pix rbint_red   = nt_red   - bint_red ;
            vl_sift_pix rbint_green = nt_green - bint_green ;
            vl_sift_pix rbint_blue  = nt_blue  - bint_blue ;
            int         dbinx ;
            int         dbiny ;
            int         dbint ;

            /* Distribute the current sample into the 8 adjacent bins*/
            for(dbinx = 0 ; dbinx < 2 ; ++dbinx)
            {
                for(dbiny = 0 ; dbiny < 2 ; ++dbiny)
                {
                    for(dbint = 0 ; dbint < 2 ; ++dbint)
                    {

                        if (binx + dbinx >= - (spatial_bins/2) &&
                            binx + dbinx <    (spatial_bins/2) &&
                            biny + dbiny >= - (spatial_bins/2) &&
                            biny + dbiny <    (spatial_bins/2) )
                        {
                            vl_sift_pix red_weight = win
                                * red_mod
                                * vcl_abs (1 - dbinx - rbinx)
                                * vcl_abs (1 - dbiny - rbiny)
                                * vcl_abs (1 - dbint - rbint_red) ;

                            vl_sift_pix green_weight = win
                                * green_mod
                                * vcl_abs (1 - dbinx - rbinx)
                                * vcl_abs (1 - dbiny - rbiny)
                                * vcl_abs (1 - dbint - rbint_green) ;

                            vl_sift_pix blue_weight = win
                                * blue_mod
                                * vcl_abs (1 - dbinx - rbinx)
                                * vcl_abs (1 - dbiny - rbiny)
                                * vcl_abs (1 - dbint - rbint_blue) ;


                            int red_offset=
                                ((bint_red+dbint)% angle_bins)*binto + 
                                (biny+dbiny)*binyo + 
                                (binx+dbinx)*binxo;

                            int green_offset=
                                ((bint_green+dbint)% angle_bins)*binto + 
                                (biny+dbiny)*binyo + 
                                (binx+dbinx)*binxo;


                            int blue_offset=
                                ((bint_blue+dbint)% angle_bins)*binto + 
                                (biny+dbiny)*binyo + 
                                (binx+dbinx)*binxo;


                            *(red_dpt+red_offset)     += red_weight;
                            *(green_dpt+green_offset) += green_weight;
                            *(blue_dpt+blue_offset)   += blue_weight;
                        }
                    }
                }
            }
        }
    }



    for ( unsigned int d=0; d < angle_bins*spatial_bins*spatial_bins ; ++d)
    {
        output.put(d,red_descr[d]);
        output.put(d+128,green_descr[d]);
        output.put(d+256,blue_descr[d]);
    }

}


