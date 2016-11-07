#include <dbsta2/dbsta2_mixture.h>
#include <dbsta2/dbsta2_gauss_f1.h>
#include <dbbgm/dbbgm_image_sptr.h>
#include <dbsta2/dbsta2_attributes.h>
#include <dbsta2/algo/dbsta2_adaptive_updater.h>
#include <vil/vil_image_view.h>
#include <brip/brip_vil_float_ops.h>
#include <dbbgm/dbbgm_update.h>
#include <Gregistration/exe/multiple_frame_polygon.h>
#include <sdet/sdet_nonmax_suppression_params.h>
#include <sdet/sdet_nonmax_suppression.h>
#include <vil/vil_plane.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_binary_erode.h>
#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_blob_finder.h>
#include <vil/vil_new.h>
#include <vgl/vgl_box_2d.h>

#include <dbsta2/dbsta2_gaussian_full.h>
#include <dbsta2/dbsta2_gaussian_sphere.h>
#include <dbsta2/dbsta2_detector_gaussian.h>
#include <dbsta2/dbsta2_detector_mixture.h>
#include <dbsta2/dbsta2_basic_functors.h>
#include <dbsta2/dbsta2_gauss_f1.h>
#include <dbsta2/dbsta2_attributes.h>
#include <dbsta2/dbsta2_mixture.h>
#include <dbsta2/dbsta2_detector_mixture.h>
#include <dbsta2/algo/dbsta2_adaptive_updater.h>
#include <dbbgm/dbbgm_detect.h>

#include <dbdet/tracer/dbdet_contour_tracer.h>

#include <dbinfo/dbinfo_region_geometry.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_track.h>
#include <dbinfo/dbinfo_multi_tracker.h>
#include <dbinfo/dbinfo_mi_track.h>
#include <dbinfo/dbinfo_object_matcher.h>
#include <dbinfo/pro/dbinfo_track_storage_sptr.h>
#include <dbinfo/pro/dbinfo_track_storage.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_hungarian_algorithm.h>

#include <georegister/dbrl_match_set_sptr.h>
#include <georegister/dbrl_feature_point_sptr.h>
#include <georegister/dbrl_feature_point.h>
#include <georegister/dbrl_feature_point_tangent.h>
#include <georegister/dbrl_thin_plate_spline_transformation.h>
#include <georegister/dbrl_cubic_transformation.h>
#include <georegister/dbrl_rpm_affine.h>
#include <georegister/dbrl_estimator_thin_plate_spline.h>
#include <georegister/dbrl_estimator_cubic_patch.h>
#include <georegister/dbrl_id_point_2d_sptr.h>
#include <georegister/dbrl_support.h>
#include <dbru/dbru_multiple_instance_object.h>

#include <vgl/vgl_polygon.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vsol/vsol_line_2d.h>
#include <bsol/bsol_algs.h>

typedef dbsta2_num_obs<dbsta2_gauss_f1> gauss_type;
typedef dbsta2_num_obs<dbsta2_mixture<gauss_type> > mix_gauss_type;
typedef dbsta2_mg_statistical_updater<dbsta2_gauss_f1> mgstat;

unsigned int intializemodel(int ni,int nj)
{

    mix_gauss_type mixture;
    dbbgm_image_of<mix_gauss_type> * model = new dbbgm_image_of<mix_gauss_type>(ni,nj,mixture);
    vcl_ofstream ofile("d:\\data\\log.txt");
    ofile<<"Model dims: "<<model->ni()<<","<<model->nj()<<"\n";
    ofile.close();
    return  reinterpret_cast<unsigned int>(model);
}

double get_var(unsigned int handle,int i,int j,int component_num)
{
    if(dbbgm_image_of<mix_gauss_type> *model = reinterpret_cast<dbbgm_image_of<mix_gauss_type>*>(handle))
    {
        if(i>=0 && i<model->ni() && j>=0 && j<model->nj())
        {
            mix_gauss_type dist=(*model)(i,j);
            return dist.distribution(component_num).var();
        }
        return -1;
    }

    return -1;
}
double get_mean(unsigned int handle,int i,int j,int component_num)
{
    if(dbbgm_image_of<mix_gauss_type> *model = reinterpret_cast<dbbgm_image_of<mix_gauss_type>*>(handle))
    {
        if(i>=0 && i<model->ni() && j>=0 && j<model->nj())
        {
            mix_gauss_type dist=(*model)(i,j);
            return dist.distribution(component_num).mean();
        }
        return -1;
    }

    return -1;
}
double get_weight(unsigned int handle,int i,int j,int component_num)
{
    if(dbbgm_image_of<mix_gauss_type> *model = reinterpret_cast<dbbgm_image_of<mix_gauss_type>*>(handle))
    {
        if(i>=0 && i<model->ni() && j>=0 && j<model->nj())
        {
            mix_gauss_type dist=(*model)(i,j);
            return dist.weight(component_num);
            
        }
        return -1;
    }

    return -1;
}
void updatemodel(unsigned int handle, unsigned char * input_img,unsigned int maxComp,
                 float initialstd,float intialweight,int ni,int nj)
{

    dbsta2_gauss_f1 init_gauss(0.0f,initialstd);
    dbsta2_mg_statistical_updater<dbsta2_gauss_f1> updater(init_gauss, maxComp);
    vcl_ofstream ofile("d:\\data\\log.txt");
    vcl_cout<<"\n Loading Model ";
    if(dbbgm_image_of<mix_gauss_type> *model = reinterpret_cast<dbbgm_image_of<mix_gauss_type>*>(handle))
    {
        vcl_cout<<"\n Model was successfully loaded ";
        vil_image_view<unsigned char> imgd(input_img,ni,nj,1,1,ni,ni*nj);
        vil_image_view<float> img=brip_vil_float_ops::convert_to_float(imgd);
        ofile<<"Model dims: "<<model->ni()<<","<<model->nj()<<"\n";
        ofile<<"Image dims: "<<img.ni()<<","<<img.nj()<<"\n";
        update(*model,img,updater);
    }
    ofile.close();

}

void savemodel(unsigned int handle,char *filename)
{

  if(dbbgm_image_of<mix_gauss_type> * model=reinterpret_cast<dbbgm_image_of<mix_gauss_type>*> (handle))
    {
      vsl_b_ofstream ofile(filename);
      model->b_write(ofile);
      ofile.close();
    }
}
unsigned int loadmodel(char *filename)
{
  vcl_ofstream ofile("d:/data/v1_data/loadmodel_log.txt");
  ofile<<filename<<"\n";
  dbbgm_image_of<mix_gauss_type> *model  = new dbbgm_image_of<mix_gauss_type>();
  vsl_b_ifstream ifile(filename);
  model->b_read(ifile);
  ofile<<"\n  Size of the model is "<<model->ni()<<"\t"<<model->nj();
  ifile.close();
  ofile.close();
  return  reinterpret_cast<unsigned int>(model);

}

void updateedgemodel(unsigned int handle, unsigned char * input_img,unsigned int maxComp,
                 float initialstd,float intialweight,float ethresh,int ni,int nj)
{
    vil_image_view<unsigned char> grey_img(input_img,ni,nj,1,1,ni,ni*nj);

    static sdet_nonmax_suppression_params nsp;
    nsp.thresh_= ethresh;
    nsp.pfit_type_ = 0;

    vil_image_view<double> grad_x, grad_y;
    vil_sobel_3x3 <unsigned char,double > (grey_img, grad_x, grad_y);

    sdet_nonmax_suppression ns(nsp, grad_x, grad_y);
    ns.apply();

    vil_image_view<float> dir_img(ni,nj,3);
    dir_img.fill(-100.0);

    vcl_vector<vsol_point_2d_sptr> edges=ns.get_points();
    vcl_vector<vgl_vector_2d<double> > dirs=ns.get_directions();
    for(unsigned i=0;i<edges.size();i++)
    {
        dir_img((unsigned int)vcl_floor(edges[i]->x()),(unsigned int)vcl_floor(edges[i]->y()),0)=edges[i]->x();
        dir_img((unsigned int)vcl_floor(edges[i]->x()),(unsigned int)vcl_floor(edges[i]->y()),1)=edges[i]->y();
        double dir=vcl_atan2(dirs[i].y(),dirs[i].x());
        dir_img((unsigned int)vcl_floor(edges[i]->x()),(unsigned int)vcl_floor(edges[i]->y()),2)=dir;
    }

    vil_image_view<float> directionplane=vil_plane(dir_img,2);


    dbsta2_gauss_f1 init_gauss(0.0f,initialstd);
    dbsta2_mg_statistical_updater<dbsta2_gauss_f1> updater(init_gauss, maxComp);
    if(dbbgm_image_of<mix_gauss_type> *model = reinterpret_cast<dbbgm_image_of<mix_gauss_type>*>(handle))
    {
        update(*model,directionplane,updater);
    }

}

unsigned int getpolygon(unsigned int handle,unsigned char* img_data,float sigmathresh,float minweightthresh,
                                       int min_no_of_pixels,int max_no_of_pixels,float erosionrad,
                                       float postdilationerosionrad,float dilationrad,int ni,int nj,float sigma,
                                       int rad_of_uncert)
{
  vcl_cout<<"\n get polygons ...";
  vcl_ofstream ofile("d:/data/v1_data/trk_log.txt");
  vil_image_view<unsigned char> grey_img(img_data,ni,nj,1,1,ni,ni*nj);
  vil_image_view<float> img=brip_vil_float_ops::convert_to_float(grey_img);
  typedef dbsta2_g_mdist_detector<dbsta2_gauss_f1> detector_type;
  typedef dbsta2_top_weight_detector<mix_gauss_type,detector_type> weight_detector_type;
  typedef dbsta2_mix_any_less_index_detector<mix_gauss_type,detector_type> all_weight_detector_type;

  if(dbbgm_image_of<mix_gauss_type> *model = reinterpret_cast<dbbgm_image_of<mix_gauss_type>*>(handle))
    {
    ofile<<"\n Size of the model is "<<model->ni()<<"\t"<<model->nj();

    dbsta2_g_mdist_detector<dbsta2_gauss_f1> det(sigmathresh);
    dbsta2_mix_any_less_index_detector<mix_gauss_type,detector_type> detector_top(det, minweightthresh);

    vil_image_view<bool> fg(ni,nj,1);
    vil_image_view<bool> temp(ni,nj,1);
    detect<mix_gauss_type,all_weight_detector_type>(*model,img,fg,detector_top,rad_of_uncert);

    ofile<<"\n fg detection succesful";
    ofile.flush();
    for (unsigned i=0;i<fg.ni();i++)
      for(unsigned j=0;j<fg.nj();j++)
          {
          if(!fg(i,j))
              temp(i,j)=true;
          else 
              temp(i,j)=false;
          }
    vcl_vector<vcl_vector<vsol_polygon_2d_sptr > > frame_polygons;
    vil_structuring_element se;
    
    se.set_to_disk(erosionrad);
    vil_binary_erode(temp,fg,se);
    se.set_to_disk(dilationrad);
    vil_binary_dilate(fg,temp,se);
    se.set_to_disk(postdilationerosionrad);
    vil_binary_erode(temp,fg,se);

    ofile<<"\n morphology done succesful";
    ofile.flush();
    //invoke the tracer
    dbdet_contour_tracer ctracer;
    ctracer.set_sigma(sigma);
    ctracer.trace(fg);
    
  //get the interesting contours
  vcl_vector<vsol_polygon_2d_sptr > * allpts=new vcl_vector<vsol_polygon_2d_sptr >();
  for (unsigned i=0; i<ctracer.contours().size(); i++)
  {
    if (static_cast<int>(ctracer.contours()[i].size())<=max_no_of_pixels && static_cast<int>(ctracer.contours()[i].size())>=min_no_of_pixels)
    {
      allpts->push_back(new vsol_polygon_2d(ctracer.contours()[i]));
    }
  }
  unsigned int polyhandle=reinterpret_cast<unsigned int>(allpts);
  ofile<<"\n No of contours are "<<allpts->size();
  ofile.flush();
  ofile.close();
  return polyhandle;//no_of_polys;

  }
  //ofile.close();
  return 0;
}

unsigned int initialize_tracker(int n_samples ,
                                float search_radius,
                                float sigma,
                                float scorethresh ,
                                unsigned int intensity_hist_bins,
                                unsigned int gradient_dir_hist_bins
                                )
{
  dbinfo_multi_tracker_params mtp(n_samples,search_radius,0.0,0.0,
                                  sigma,scorethresh,
                                  true,
                                  true, 
                                  false,0, 0, 0,
                                  intensity_hist_bins,
                                  gradient_dir_hist_bins,
                                  8,false,false,false,false,
                                  false,false);
 dbinfo_multi_tracker *tracker_=new dbinfo_multi_tracker(mtp);
 return reinterpret_cast<unsigned int>(tracker_);

}
vcl_vector<dbrl_id_point_2d_sptr> get_foreground_edges_from_observ(dbinfo_observation_sptr obs,dbbgm_image_of<mix_gauss_type> *model,
                                                                   float dist,float minweightthresh,int rad,float thresh, float dilatepoly);
void track_polygons(unsigned int trkhndno,unsigned char *img_data, int ni,int nj,unsigned int polyhandle, float expmotion, int frameno)
{
    dbinfo_multi_tracker *tracker_=reinterpret_cast<dbinfo_multi_tracker*>(trkhndno);
    vil_image_view<unsigned char> grey_img(img_data,ni,nj,1,1,ni,ni*nj);

    vcl_ofstream ofile("d:\\data\\trackinglog.txt");

    ofile<<"Size of image "<<grey_img.ni()<<" "<<grey_img.nj()<<"\n";
    vil_image_resource_sptr im=vil_new_image_resource_of_view(grey_img);
    //Set current frame image on the tracker
    tracker_->set_image(frameno, im );

    if(vcl_vector<vsol_polygon_2d_sptr > * polys=
        reinterpret_cast<vcl_vector<vsol_polygon_2d_sptr > *> (polyhandle))
    {

        ofile<<"Size of polys "<<polys->size()<<"\n";
        if(polys->size()<=0)
        {
            vcl_cout << "no detections in frame no. "<<frameno<<"\n";
            return ;
        }

        vcl_vector<dbinfo_observation_sptr> curr_obs;
        //: obtain the detected polygons
        for(vcl_vector< vsol_polygon_2d_sptr >::iterator dit =
            polys->begin(); dit != polys->end(); ++dit)
        {
            vsol_polygon_2d_sptr poly = *dit;
            assert(poly);
            vsol_box_2d_sptr b2d=poly->get_bounding_box();
            if(b2d->get_max_x()<tracker_->image()->ni()-10 && b2d->get_max_y()<tracker_->image()->nj()-10 &&
                b2d->get_min_x()>10 && b2d->get_min_y()>10 )
            {
                dbinfo_observation_sptr new_obs=new dbinfo_observation(frameno,tracker_->image(),poly,
                    tracker_->intensity_info_,
                    tracker_->gradient_info_,
                    tracker_->color_info_);
                //compute_observation_snippet(new_obs.ptr(),im);
                curr_obs.push_back(new_obs);
                

            }
        }

        //create the storage for display
        unsigned n_trks = tracker_->n_tracks();
        vcl_vector<dbinfo_mi_track_sptr> enabled_tracks;
        for(unsigned i = 0; i<n_trks; ++i)
        {
            dbinfo_mi_track_sptr tr = tracker_->track(i);
            assert(tr);
            if(!tr->is_disabled())
                enabled_tracks.push_back(tr);
        }

        vnl_matrix<double> cost_matrix(curr_obs.size(),enabled_tracks.size(),0.0);

        unsigned track_counter=0;
        for(unsigned i = 0; i<n_trks; ++i)
        {
            dbinfo_mi_track_sptr tr = tracker_->track(i);
            assert(tr);
            if(tr->is_disabled())
                continue;
            dbinfo_observation_sptr prev_obs=tr->seed(0);
            vcl_vector<dbinfo_observation_sptr>::iterator iter;
            float max_score=0.0;
            unsigned curr_obs_counter=0;
            for(iter=curr_obs.begin();iter!=curr_obs.end();iter++)
            {

                vil_image_resource_sptr img;
                dbinfo_region_geometry_sptr geo_db = (*iter)->geometry();
                dbinfo_region_geometry_sptr geo_q = prev_obs->geometry();
                vsol_point_2d_sptr cent_db = geo_db->centroid();
                vsol_point_2d_sptr cent_q  = geo_q->centroid();

                double dist=vcl_sqrt((cent_db->x()-cent_q->x())*(cent_db->x()-cent_q->x())+(cent_db->y()-cent_q->y())*(cent_db->y()-cent_q->y()));
                if(dist<expmotion)
                {
                    vgl_h_matrix_2d<float> h;
                    float score=dbinfo_object_matcher ::minfo_rigid_alignment_rand(*iter,prev_obs,h,
                        tracker_->search_radius_,
                        tracker_->angle_range_,
                        tracker_->scale_range_,
                        0.0,1.0,0.8,tracker_->n_samples_,img,
                        true,1.0,tracker_->intensity_info_,
                        tracker_->gradient_info_);
                    if(score<   tracker_->scorethresh_ )
                        cost_matrix(curr_obs_counter, track_counter)=  100;
                    else
                        cost_matrix(curr_obs_counter, track_counter)=  10-score;
                }
                else
                    cost_matrix(curr_obs_counter, track_counter)=  200;

                curr_obs_counter++;
            }
            track_counter++;
            tr->disable();
        }

        
        vcl_vector<unsigned> assignment=vnl_hungarian_algorithm(cost_matrix);
        for(unsigned i=0;i<assignment.size();i++)
        {
            if(assignment[i]!=unsigned(-1))
            {
                if(cost_matrix(i,assignment[i])<100)
                {
                    enabled_tracks[assignment[i]]->enable();
                    dbinfo_observation_sptr p_obs=enabled_tracks[assignment[i]]->seed(0);
                    vgl_h_matrix_2d<float> f;
                    vil_image_resource_sptr img;

                    float s=dbinfo_object_matcher ::minfo_rigid_alignment_rand(curr_obs[i],p_obs,f,
                        tracker_->search_radius_,
                        tracker_->angle_range_,
                        tracker_->scale_range_,
                        0.0,1.0,0.8,tracker_->n_samples_,img,
                        true,1.0,tracker_->intensity_info_,
                        tracker_->gradient_info_);
                    enabled_tracks[assignment[i]]->extend_track(curr_obs[i],f);
                    enabled_tracks[assignment[i]]->set_seeds(vcl_vector<dbinfo_observation_sptr>(1,curr_obs[i]));
                    curr_obs[i]=NULL;
                }
            }

        }

        vcl_vector<vsol_spatial_object_2d_sptr> tracks;
        for(unsigned i = 0; i<n_trks; ++i)
        {
            dbinfo_mi_track_sptr tr = tracker_->track(i);
            assert(tr);

            dbinfo_track_geometry_sptr trk=tr->track_geometry();
            if(tr->is_disabled())
                continue;
            if(tr->n_seeds()==0)
            {
                tr->disable();
                continue;
            }
        }


        for(unsigned i = 0; i<curr_obs.size(); ++i)
        {
            if(curr_obs[i])
            {
                dbinfo_mi_track_sptr new_track = new dbinfo_mi_track();
                new_track->init_track(curr_obs[i]);
                //compute_observation_snippet(curr_obs[i].ptr(),im);
                new_track->set_id(tracker_->n_tracks());
                new_track->set_seeds(vcl_vector<dbinfo_observation_sptr>(1,curr_obs[i]));
                tracker_->add_track(new_track);
            }
        }
    }
    ofile.close();
}

void save_tracks(unsigned int trkhandle, char * filename)
{
    dbinfo_multi_tracker *tracker_=reinterpret_cast<dbinfo_multi_tracker*>(trkhandle);

    // create the track storage class
    dbinfo_track_storage_sptr output_dbinfo = dbinfo_track_storage_new();
    vcl_vector<dbinfo_mi_track_sptr> mi_tracks = tracker_->tracks();

    vcl_vector<dbinfo_track_sptr> tracks;
    for(vcl_vector<dbinfo_mi_track_sptr>::iterator trit = mi_tracks.begin();
        trit != mi_tracks.end(); ++trit)
        tracks.push_back((dbinfo_track*)((dbinfo_mi_track*)(*trit).ptr()));

    //store the tracks
    output_dbinfo->set_tracks(tracks);
    vsl_b_ofstream bp_out(filename);
    output_dbinfo->b_write(bp_out);
    bp_out.close();


}

unsigned int load_tracks(char * filename)
{
  // read the tracks from the storage file  
  vsl_b_ifstream is(filename);
  dbinfo_track_storage_sptr track_storage_ = new dbinfo_track_storage();
  track_storage_->b_read(is);
  //get the geometry for each track
  vcl_vector<dbinfo_track_sptr> tracks = track_storage_->tracks();
  vcl_vector<dbinfo_mi_track_sptr> * mi_tracks=new vcl_vector<dbinfo_mi_track_sptr>();
  for(vcl_vector<dbinfo_track_sptr>::iterator trit = tracks.begin();
      trit != tracks.end(); ++trit)
      mi_tracks->push_back(((dbinfo_mi_track*)(*trit).ptr()));


  return reinterpret_cast<unsigned int>(mi_tracks);
}
void compute_clips(unsigned int trkhandle, unsigned int edgehandle,int frameno,unsigned char *img_data,int ni,int nj,
                   float bgdist,float bgminweightthresh,int bgrad,float bgthresh, float polydilate)
{
    vcl_vector<dbinfo_mi_track_sptr> * tracks=reinterpret_cast<vcl_vector<dbinfo_mi_track_sptr> *>(trkhandle);
    dbbgm_image_of<mix_gauss_type> *emodel = reinterpret_cast<dbbgm_image_of<mix_gauss_type>*>(edgehandle);

    vil_image_view<unsigned char> grey_img(img_data,ni,nj,1,1,ni,ni*nj);
    vil_image_resource_sptr im=vil_new_image_resource_of_view(grey_img);

    vcl_ofstream ofile("d:\\data\\clip_info.txt");
    for(vcl_vector<dbinfo_mi_track_sptr>::iterator trit = tracks->begin();
        trit != tracks->end(); trit++)
    {
        if(frameno>=(*trit)->start_frame() && frameno<=(*trit)->end_frame())
        {
            dbinfo_observation_sptr ref_obs=(*trit)->observ(frameno); 
            compute_observation_snippet(ref_obs.ptr(),im);
            vcl_vector<dbrl_id_point_2d_sptr> edgels= get_foreground_edges_from_observ(ref_obs,emodel,bgdist, bgminweightthresh,
                                                                               bgrad,bgthresh,polydilate);
            vcl_vector<vgl_point_2d<double> > ps;
            vcl_vector<double > ds;

            for(int l=0;l<edgels.size();l++)
            {
                ps.push_back(vgl_point_2d<double>(edgels[l]->x(),edgels[l]->y()));
                ds.push_back(edgels[l]->tangent());
            }

            ref_obs->set_edge_dirs(ds);
            ref_obs->set_edge_points(ps);

            ofile<<"Size is "<<ref_obs->obs_snippet()->ni()<<","<<ref_obs->obs_snippet()->nj()<<"\n";
            ofile<<"#edges "<<edgels.size()<<"\n";
        }
    }
    ofile.close();

}

vsol_polygon_2d_sptr dilate_poly(vsol_polygon_2d_sptr poly, double rad);

//vsol_polygon_2d_sptr compute_spatial_support(double sigma_d,double thresh,
//                                             vcl_vector< vcl_vector< dbrl_id_point_2d_sptr > > pts);
vcl_vector<dbrl_feature_sptr> get_grid_points(vcl_vector<dbrl_feature_sptr> f2,
                                                                                     double spacing)
{
    vcl_vector<vgl_point_2d<double> > pts;

    vgl_box_2d<double> box;

    for(unsigned i=0;i<f2.size();i++)
    {
        if(dbrl_feature_point * pt=dynamic_cast<dbrl_feature_point*>(f2[i].ptr()))
            box.add(vgl_point_2d<double>(pt->location()[0],pt->location()[1]));
        else if(dbrl_feature_point_tangent * pt=dynamic_cast<dbrl_feature_point_tangent*>(f2[i].ptr()))
            box.add(vgl_point_2d<double>(pt->location()[0],pt->location()[1]));

    }

    double xmin=vcl_floor(box.min_x());
    double ymin=vcl_floor(box.min_y());
    double xmax=vcl_ceil(box.max_x());
    double ymax=vcl_ceil(box.max_y());

    vcl_vector<dbrl_feature_sptr> fs;
    for(double x=xmin;x<=xmax;)
    {   
        for(double y=ymin;y<=ymax;)
        {
            fs.push_back(new dbrl_feature_point(x,y));
            y+=spacing;
        }
        x+=spacing;
    }
    return fs;

}








vcl_vector<vsol_line_2d_sptr>
compute_spatial_support(double sigma_d,double thresh,
                                                               vcl_vector< vcl_vector< dbrl_id_point_2d_sptr > > pts)
{
    unsigned totsize=0;

    if(pts.size()<=0)
        return vcl_vector<vsol_line_2d_sptr>();

    vcl_vector< dbrl_id_point_2d_sptr > allpts;
    for(unsigned i=0;i<pts.size();i++)
    {
        totsize+=pts[i].size();
        allpts.insert(allpts.end(),pts[i].begin(),pts[i].end());
    }
    double *weightst=new double[totsize];

    for(unsigned j=0;j<totsize;j++)
        weightst[j]=0.0;

    for(unsigned j=0,cnt=0;j<pts.size();j++) {
        for(unsigned l=0;l<pts[j].size();l++) {
        double xj=pts[j][l]->x();
        double yj=pts[j][l]->y();
        double tj=pts[j][l]->tangent();
        for(unsigned i=0;i<pts.size();i++){
            double st=0.0;
            if(i!=j)
            {
                for(unsigned k=0;k<pts[i].size();k++){
                    double xi=pts[i][k]->x();
                    double yi=pts[i][k]->y();
                    double ti=pts[i][k]->tangent();
                    st+=compute_support(xj,yj,tj,xi,yi,ti,sigma_d);//vcl_exp(-((xi-xj)*(xi-xj)+(yi-yj)*(yi-yj))/(sigma_d*sigma_d) -(tj-ti)*(tj-ti)/(sigma_a*sigma_a));
                }
                if(weightst[cnt]<st)
                    weightst[cnt]=st;
            }
        }
        cnt++;
        }
    }

    vcl_vector<vsol_spatial_object_2d_sptr> vsol_out;
    vcl_vector<vsol_line_2d_sptr> supportedges;
    for(unsigned j=0;j<totsize;j++)
    {
        if(weightst[j] >thresh)
        {
            vsol_point_2d_sptr p=new vsol_point_2d(allpts[j]->x(),allpts[j]->y());
            vsol_point_2d_sptr ps=new vsol_point_2d(p->x()+0.2*vcl_cos(allpts[j]->tangent()),p->y()+0.2*vcl_sin(allpts[j]->tangent()));
            vsol_point_2d_sptr pe=new vsol_point_2d(p->x()-0.2*vcl_cos(allpts[j]->tangent()),p->y()-0.2*vcl_sin(allpts[j]->tangent()));
            vsol_line_2d_sptr l=new vsol_line_2d(ps,pe);
            supportedges.push_back(l);
            //vsol_out.push_back(p->cast_to_spatial_object());
            //supportpoints.push_back(vgl_point_2d<double>(p->x(),p->y()));
            //vsol_out.push_back(l->cast_to_spatial_object());
        }  
    }


    return supportedges;

    //if(supportpoints.size()<=0)
    //    return new vsol_polygon_2d();
    //vgl_convex_hull_2d<double> hullp(supportpoints);
    //vgl_polygon<double> psg=hullp.hull();

    //if(static_cast<int>(psg[0].size())<3)
    //{
    //    return new vsol_polygon_2d();
    //}

    //vcl_vector<vsol_point_2d_sptr> newpts;
    //for(int k=0;k<static_cast<int>(psg[0].size());k++)
    //{
    //    newpts.push_back(new vsol_point_2d(psg[0][k].x(),psg[0][k].y()));

    //}

    //vidpro1_vsol2D_storage_sptr vsol_storage;
    //vsol_storage.vertical_cast(get_output(0)[0]);

    //vsol_storage->add_objects(vsol_out);
    //return new vsol_polygon_2d(newpts);

}

vsol_polygon_2d_sptr 
compute_convex_hull( vcl_vector<vsol_line_2d_sptr > edges)
{
    if(edges.size()<=0)
        return new vsol_polygon_2d();
    vcl_vector<vgl_point_2d<double> > points;
    for(unsigned i=0;i<edges.size();i++)
    {
        vsol_point_2d_sptr p= edges[i]->middle();
        points.push_back(vgl_point_2d<double>(p->x(),p->y()));

    }
    vgl_convex_hull_2d<double> hullp(points);
    vgl_polygon<double> psg=hullp.hull();

    if(static_cast<int>(psg[0].size())<3)
        return new vsol_polygon_2d();

    vcl_vector<vsol_point_2d_sptr> newpts;
    for(int k=0;k<static_cast<int>(psg[0].size());k++)
        newpts.push_back(new vsol_point_2d(psg[0][k].x(),psg[0][k].y()));
    return new vsol_polygon_2d(newpts);
}

unsigned int superimpose_edges(unsigned int trkhandle, unsigned char *img_data,float Tinit,
                               float Tfinal,float annealrate,float lambdainit, float mconvg, 
                               float moutlier, int ni,int nj, int frame_no,
                               int winsize, float std, float thresh)
                               //float bgdist,float bgminweightthresh,int bgrad,float bgthresh, float polydilate)
{
    vcl_vector<vsol_polygon_2d_sptr> * polygons=new vcl_vector<vsol_polygon_2d_sptr>();
    vcl_vector<dbru_multiple_instance_object* > * multiple_objects=new vcl_vector<dbru_multiple_instance_object* >();

    vil_image_view<unsigned char> grey_img(img_data,ni,nj,1,1,ni,ni*nj);

    //dbbgm_image_of<mix_gauss_type> *emodel = reinterpret_cast<dbbgm_image_of<mix_gauss_type>*>(emodelandle);
    double gridspace=0.2;
    vcl_ofstream ofile("d:\\data\\superlog.txt");
    if(vcl_vector<dbinfo_mi_track_sptr> * tracks_
        =reinterpret_cast<vcl_vector<dbinfo_mi_track_sptr> *>(trkhandle))
    {
        //vcl_vector<dbinfo_mi_track_sptr> tracks_;
        //tracks_=tracker_->tracks();
        ofile<<"\n Number of tracks are"<<tracks_->size()<<"\n";
        for(vcl_vector<dbinfo_mi_track_sptr>::iterator trit = tracks_->begin();
            trit != tracks_->end(); trit++)
        {
            ofile<<"tracknumber "<<(*trit)->id()<<"\n";  
            vcl_vector<vil_image_view<float> > superimgs;

            if(frame_no>=(*trit)->start_frame() && frame_no<=(*trit)->end_frame())
            {
                vcl_vector<vcl_vector<dbrl_id_point_2d_sptr> > xformed_all_pivot_pointedges;
                vcl_vector<dbrl_id_point_2d_sptr> all_pivot_pointedges;
                dbru_multiple_instance_object  * m_object= new dbru_multiple_instance_object();
                vcl_vector<vil_image_view<vxl_byte> > xformedimgs;

                dbinfo_observation_sptr ref_obs=(*trit)->observ(frame_no); //: observation in the current frame
                if(ref_obs.ptr())
                {
                    vsol_polygon_2d_sptr polytest=ref_obs->geometry()->poly(0);
                    ofile<<"No of points on polygon are "<<polytest->size()<<"\n";
                    //ofile<<"Size of Image "<<ref_obs->obs_snippet()->ni()<<" "<<ref_obs->obs_snippet()->nj()<<"\n";
                    vcl_vector<dbrl_id_point_2d_sptr>  pivot_pointedges;
                    vcl_vector<vgl_point_2d<double> > ps=ref_obs->get_edge_points();
                    vcl_vector<double> ds=ref_obs->get_edge_dirs();
                    for(int icnt=0;icnt<ps.size();icnt++)
                    {
                        pivot_pointedges.push_back(new dbrl_id_point_2d(ps[icnt].x(),ps[icnt].y(),ds[icnt],icnt));
                    }
                    //vcl_vector<dbrl_id_point_2d_sptr>  pivot_pointedges=get_foreground_edges_from_observ(ref_obs,emodel,bgdist,bgminweightthresh,bgrad,bgthresh,polydilate);
                    ofile<<"#Edges in current frame "<<pivot_pointedges.size()<<"\n";  
                    vcl_vector<dbrl_feature_sptr> f1;
                    vcl_vector<vsol_spatial_object_2d_sptr> f1pts;
                    double xrefc=0,yrefc=0;
                    for(int k=0;k<pivot_pointedges.size();k++)
                    {
                        xrefc+=pivot_pointedges[k]->x();
                        yrefc+=pivot_pointedges[k]->y();
                    }
                    xrefc/=pivot_pointedges.size();
                    yrefc/=pivot_pointedges.size();

                    ofile<<"CM : "<<xrefc <<" "<<yrefc<<"\n";
                    ofile.flush();
                    //: converting points into features
                    vcl_vector<dbrl_id_point_2d_sptr> orig_pivot_pointedges;
                    for(int k=0;k<pivot_pointedges.size();k++)
                    {
                        vnl_vector_fixed<double,2> pt((pivot_pointedges[k]->x()-xrefc),(pivot_pointedges[k]->y()-yrefc));
                        dbrl_feature_point_tangent * fpt=new dbrl_feature_point_tangent(pt,pivot_pointedges[k]->tangent());
                        fpt->setid(pivot_pointedges[k]->id());
                        f1.push_back(fpt);
                        vsol_point_2d_sptr p=new vsol_point_2d(pivot_pointedges[k]->x(),pivot_pointedges[k]->y());
                        vsol_point_2d_sptr p0=new vsol_point_2d(p->x()-0.2*vcl_sin(fpt->dir()),p->y()+0.2*vcl_cos(fpt->dir()));
                        vsol_point_2d_sptr p1=new vsol_point_2d(p->x()+0.2*vcl_sin(fpt->dir()),p->y()-0.2*vcl_cos(fpt->dir()));

                        vsol_line_2d_sptr l=new vsol_line_2d(p0,p1);
                        double tan = vcl_atan2 (p1->y() - p0->y(), p1->x() - p0->x());
                        orig_pivot_pointedges.push_back(new dbrl_id_point_2d(p->x(),p->y(),tan,k));

                    }
                    vcl_vector<vgl_point_2d<double> > orgpts;
                    for(unsigned i=0;i<pivot_pointedges.size();i++)
                        orgpts.push_back(vgl_point_2d<double>(pivot_pointedges[i]->x(),pivot_pointedges[i]->y()));

                    if(pivot_pointedges.size()>0)
                    {

                        for(int i=frame_no-winsize;i<=frame_no+winsize;i++)
                        {
                            ofile<<"Match frame no: "<<i<<"\n";
                            if(i>=(*trit)->start_frame() && i<=(*trit)->end_frame() && i!=frame_no)
                            {
                                dbinfo_observation_sptr temp_obs=(*trit)->observ(i);
                                if(temp_obs.ptr())
                                {
                                    if(temp_obs->obs_snippet().ptr())
                                    {
                                        vil_image_view<unsigned char> img_view=temp_obs->obs_snippet()->get_view();
                                        brip_roi_sptr exroi=temp_obs->ex_roi();
                                        vcl_vector<dbrl_id_point_2d_sptr>  curr_pointedges;
                                        vcl_vector<vgl_point_2d<double> > cps=temp_obs->get_edge_points();
                                        vcl_vector<double> cds=temp_obs->get_edge_dirs();
                                        for(int icnt=0;icnt<cps.size();icnt++)
                                        {
                                            curr_pointedges.push_back(new dbrl_id_point_2d(cps[icnt].x(),cps[icnt].y(),cds[icnt],icnt));
                                        }

                                        //vcl_vector<dbrl_id_point_2d_sptr> curr_pointedges=get_foreground_edges_from_observ(temp_obs,emodel,bgdist,bgminweightthresh,bgrad,bgthresh,polydilate);
                                        vcl_vector<dbrl_feature_sptr> f2;

                                        double xcurc=0,ycurc=0;
                                        for(int k=0;k<curr_pointedges.size();k++)
                                        {
                                            xcurc+=curr_pointedges[k]->x();
                                            ycurc+=curr_pointedges[k]->y();
                                        }
                                        xcurc/=curr_pointedges.size();
                                        ycurc/=curr_pointedges.size();
                                        ofile<<"CM of other obs: "<<xcurc<<" "<<ycurc<<"\n";
                                        vcl_vector<vsol_spatial_object_2d_sptr> f2pts;
                                        for(unsigned k=0;k<curr_pointedges.size();k++)
                                        {
                                            vnl_vector_fixed<double,2> pt((curr_pointedges[k]->x()-xcurc),(curr_pointedges[k]->y()-ycurc));
                                            dbrl_feature_point_tangent * fpt=new dbrl_feature_point_tangent(pt,curr_pointedges[k]->tangent());
                                            fpt->setid(curr_pointedges[k]->id());
                                            f2.push_back(fpt);
                                            vsol_point_2d_sptr p=new vsol_point_2d(curr_pointedges[k]->x()-xcurc,curr_pointedges[k]->y()-ycurc);
                                        }
                                        ofile<<"#Point Edges "<<curr_pointedges.size()<<"\n";
                                        ofile.flush();
                                        //: applying translation 
                                        lambdainit=vcl_max(f1.size(),f2.size());
                                        dbrl_rpm_affine_params affineparams(lambdainit,mconvg,Tinit,Tfinal,moutlier,annealrate);

                                        //: applying affine 
                                        dbrl_rpm_affine match_affinely(affineparams,f1,f2);
                                        dbrl_match_set_sptr match_set=match_affinely.rpm("Euclidean");

                                        ////: binarize the match matrix
                                        dbrl_correspondence M=match_set->get_correspondence();
                                        M.setinitialoutlier(affineparams.outlier());

                                        dbrl_estimator_point_affine* final_affine_est= new dbrl_estimator_point_affine();
                                        final_affine_est->set_lambda(0.0);
                                        vcl_vector<dbrl_feature_sptr> affinef1xform=f1;
                                        //match_set->normalize_point_set(M.M(),affinef1xform );
                                        dbrl_transformation_sptr final_affine_tform=final_affine_est->estimate(f1,f2,M);
                                        dbrl_affine_transformation * final_affine_form=dynamic_cast<dbrl_affine_transformation *> (final_affine_tform.ptr());
                                        final_affine_form->set_from_features(f2);
                                        final_affine_form->transform();
                                        vcl_vector<dbrl_feature_sptr> affinef2xformed=final_affine_form->get_to_features();

                                        final_affine_form->print_summary(ofile);
                                        ofile.flush();
                                        dbrl_estimator_cubic_patch* cubic_est= new dbrl_estimator_cubic_patch();

                                        vcl_vector<dbrl_feature_sptr> f1xform=match_set->get_feature_set1();
                                        //dbrl_transformation_sptr tform=cubic_est->estimate(f1xform,match_set->get_feature_set2(),m);
                                        dbrl_transformation_sptr tform=cubic_est->estimate(f1xform,affinef2xformed,M);
                                        dbrl_cubic_transformation * cubictform=dynamic_cast<dbrl_cubic_transformation *> (tform.ptr());
                                        cubictform->set_from_features(affinef2xformed);
                                        cubictform->transform();
                                        vcl_vector<dbrl_feature_sptr> f2xformed=cubictform->get_to_features();
                                        ofile<<"\n xforming grid points";
                                        ofile.flush();
                                        //: xform grid points
                                        vcl_vector<dbrl_feature_sptr> gridpoints=get_grid_points(f2,gridspace);
                                        ofile<<"\n grid points obbtained";
                                        ofile.flush();
                                        final_affine_form->set_from_features(gridpoints);
                                        final_affine_form->transform();
                                        vcl_vector<dbrl_feature_sptr> affinexfomredgridpoints=final_affine_form->get_to_features();
                                        cubictform->set_from_features(affinexfomredgridpoints);
                                        cubictform->transform();
                                        vcl_vector<dbrl_feature_sptr> xfomredgridpoints=cubictform->get_to_features();
                                        //vcl_vector<dbrl_feature_sptr> xfomredgridpoints=final_affine_form->get_to_features();

                                        vcl_vector<vsol_spatial_object_2d_sptr> gridpts;
                                        vcl_vector<vsol_spatial_object_2d_sptr> xgridpts;

                                        vnl_matrix<float> intensities(ni,nj);
                                        intensities.fill(0.0);
                                        vnl_matrix<int> freq(ni,nj);
                                        freq.fill(0);
                                        ofile<<"\n xformed grid points";
                                        ofile.flush();
                                        for(unsigned m=0;m<gridpoints.size();m++)
                                        {
                                            vsol_point_2d_sptr p1;
                                            if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(gridpoints[m].ptr()))
                                            {
                                                p1=new vsol_point_2d(pt->location()[0]+xcurc,pt->location()[1]+ycurc);
                                                //gridpts.push_back(p1->cast_to_spatial_object());
                                            }
                                            if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(xfomredgridpoints[m].ptr()))
                                            {
                                                vsol_point_2d_sptr p=new vsol_point_2d(pt->location()[0]+xrefc,pt->location()[1]+yrefc);
                                                int p1x=vcl_floor(p1->x())-exroi->cmin(0);
                                                int p1y=vcl_floor(p1->y())-exroi->rmin(0);
                                                if(vcl_floor(p->x())<intensities.rows() && vcl_floor(p->x())>=0 &&
                                                    vcl_floor(p->y())<intensities.cols() && vcl_floor(p->y())>=0 && 
                                                    p1x>=0 && p1x<img_view.ni() && p1y>=0 && p1y<img_view.nj())
                                                {
                                                    intensities(vcl_floor(p->x()),vcl_floor(p->y()))+=img_view(p1x,p1y);
                                                    freq(vcl_floor(p->x()),vcl_floor(p->y()))++;
                                                }
                                                else
                                                {
                                                    vcl_cout<<"\n Error";
                                                }
                                                //if(vcl_floor(2*(p->x()-globalcmin))<globalintensities.rows() && vcl_floor(2*(p->x()-globalcmin))>=0 &&
                                                //    vcl_floor(2*(p->y()-globalrmin))<globalintensities.cols() &&vcl_floor(2*(p->y()-globalrmin))>=0)
                                                //{
                                                //    globalintensities(vcl_floor(2*(p->x()-globalcmin)),vcl_floor(2*(p->y()-globalrmin)))+=img_view(vcl_floor(p1->x())-exroi->cmin(0),vcl_floor(p1->y())-exroi->rmin(0));
                                                //    globalfreq(vcl_floor(2*(p->x()-globalcmin)),vcl_floor(2*(p->y()-globalrmin)))++;
                                                //}
                                            }
                                        }

                                        vil_image_view<float> img_result(ni,nj);
                                        for(unsigned k=0;k<img_result.ni();k++)
                                            for(unsigned l=0;l<img_result.nj();l++)
                                            {
                                                if(freq(k,l)>0)
                                                {
                                                    float val=(float)intensities(k,l)/(float)freq(k,l);
                                                    img_result(k,l)=val;
                                                }
                                            }

                                            
                                            superimgs.push_back(img_result);

                                            vcl_vector<dbrl_id_point_2d_sptr>  f2xid;
                                            for(unsigned m=0;m<f2xformed.size();m++)
                                            {
                                                if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(f2xformed[m].ptr()))
                                                {
                                                    vsol_point_2d_sptr p=new vsol_point_2d(pt->location()[0]+xrefc,pt->location()[1]+yrefc);
                                                    f2pts.push_back(p->cast_to_spatial_object());
                                                    f2xid.push_back(new dbrl_id_point_2d(p->x(),p->y(),m));
                                                }
                                                else if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(f2xformed[m].ptr()))
                                                {
                                                    vsol_point_2d_sptr p=new vsol_point_2d(pt->location()[0]+xrefc,pt->location()[1]+yrefc);
                                                    vsol_point_2d_sptr p0=new vsol_point_2d(p->x()-0.2*vcl_sin(pt->dir()),p->y()+0.2*vcl_cos(pt->dir()));
                                                    vsol_point_2d_sptr p1=new vsol_point_2d(p->x()+0.2*vcl_sin(pt->dir()),p->y()-0.2*vcl_cos(pt->dir()));

                                                    vsol_line_2d_sptr l=new vsol_line_2d(p0,p1);
                                                    double tan = vcl_atan2 (p1->y() - p0->y(), p1->x() - p0->x());
                                                    f2xid.push_back(new dbrl_id_point_2d(p->x(),p->y(),tan,m));
                                                }
                                            }
                                            xformed_all_pivot_pointedges.push_back(f2xid);
                                    }
                                }
                            }
                            else if(i==frame_no)
                            {
                                xformed_all_pivot_pointedges.push_back(orig_pivot_pointedges);
                                vil_image_view<float> imgtemp=brip_vil_float_ops::convert_to_float(grey_img);
                                superimgs.push_back(imgtemp);
                            }

                        }
                        ofile<<"\n computing spatial support";
                        ofile.flush();
                        vcl_vector<vsol_line_2d_sptr> supportedges=compute_spatial_support(std,thresh,xformed_all_pivot_pointedges);
                        vsol_polygon_2d_sptr p=compute_convex_hull(supportedges);

                        if(p->size()>0)
                        {
                            vsol_box_2d_sptr boxp=p->get_bounding_box();
                            brip_roi_sptr roiptr=new brip_roi(ni,nj);

                            vsol_box_2d_sptr boxnew=new vsol_box_2d();
                            boxnew->add_point(boxp->get_min_x()-10,boxp->get_min_y()-10);
                            boxnew->add_point(boxp->get_max_x()+10,boxp->get_min_y()-10);
                            boxnew->add_point(boxp->get_min_x()-10,boxp->get_max_y()+10);
                            boxnew->add_point(boxp->get_max_x()+10,boxp->get_max_y()+10);

                            for(unsigned k=0;k<superimgs.size();k++)
                            {
                                vil_image_view<float> chip;
                                brip_vil_float_ops::chip(superimgs[k],boxnew,chip);
                                xformedimgs.push_back(brip_vil_float_ops::convert_to_byte(chip,0,255));
                                //vil_save(brip_vil_float_ops::convert_to_byte(superimgs[k],0,255),"d:/temp.tiff");
                            }

                            m_object->set_images(xformedimgs);
                            m_object->set_poly(p);
                            m_object->set_edges(supportedges);
                            m_object->set_bbox(boxp);
                            m_object->set_bigbox(boxnew);
                            multiple_objects->push_back(m_object);
                        }

                    }
                }
            }
        }
    }
    ofile<<"\n No of hulls are "<<polygons->size();
    ofile.close();
    return reinterpret_cast<unsigned int>(multiple_objects);
}
unsigned int size_of_multiple_instance_vector(unsigned int mobs)
{
  if(vcl_vector<dbru_multiple_instance_object*> * obs_=reinterpret_cast<vcl_vector<dbru_multiple_instance_object*> *>(mobs))
    {
        return obs_->size();
  }
  else return 0;
}

unsigned int return_polys_from_multiple_instance(unsigned int mobs)
{
    vcl_vector<vsol_polygon_2d_sptr> * polys =new vcl_vector<vsol_polygon_2d_sptr>();
    if(vcl_vector<dbru_multiple_instance_object*> * obs_=reinterpret_cast<vcl_vector<dbru_multiple_instance_object*> *>(mobs))
    {
        for(unsigned i=0;i< obs_->size();i++)
        {
            polys->push_back((*obs_)[i]->get_poly());
        }
        return reinterpret_cast<unsigned int>(polys);
    }
  else return 0;
}
unsigned int return_i_multiple_instance(unsigned int mobs, int index)
{
    if(vcl_vector<dbru_multiple_instance_object*> * obs_=reinterpret_cast<vcl_vector<dbru_multiple_instance_object*> *>(mobs))
    {
       if(index<obs_->size())
       {
        return reinterpret_cast<unsigned int>((*obs_)[index]);
       }
       return 0;
    }
    else
    {
        vcl_cout<<"\n  the pointer does not contain any multiple instances.";
        return 0;
    }
    
}
vcl_vector<dbrl_id_point_2d_sptr> get_foreground_edges_from_observ(dbinfo_observation_sptr obs,dbbgm_image_of<mix_gauss_type> *model,
                                                                   float dist,float minweightthresh,int rad,float thresh, float polydilate)
    {
    vcl_vector<dbrl_id_point_2d_sptr> fgedges;
    vcl_vector<dbrl_id_point_2d_sptr> pointedges;
    if(!obs.ptr())
        return fgedges;

    vil_image_resource_sptr clip_im=obs->obs_snippet();
    vsol_polygon_2d_sptr poly=obs->geometry()->poly(0);
    vsol_polygon_2d_sptr dilated_poly=dilate_poly(poly,  polydilate);

    vgl_polygon<double> vgp = bsol_algs::vgl_from_poly(dilated_poly);
   
    if(!clip_im.ptr())
        return fgedges;

    sdet_nonmax_suppression_params nonmax_params(thresh,0);
    vil_image_view<double> gradx,grady;
    vil_sobel_3x3<unsigned char,double>(brip_vil_float_ops::convert_to_byte(clip_im),gradx,grady);
    sdet_nonmax_suppression edet(nonmax_params,gradx,grady);
    edet.apply();

    vcl_vector<vsol_point_2d_sptr> points=edet.get_points();
    vcl_vector< vgl_vector_2d<double> >tangents=edet.get_directions();

    brip_roi_sptr exroi=obs->ex_roi();

    typedef dbsta2_num_obs<dbsta2_gauss_f1> gauss_type;
    typedef dbsta2_num_obs<dbsta2_mixture<gauss_type> > mix_gauss_type;
    typedef dbsta2_g_mdist_detector<dbsta2_gauss_f1> detector_type;
    typedef dbsta2_top_weight_detector<mix_gauss_type,detector_type> weight_detector_type;
    typedef dbsta2_mix_any_less_index_detector<mix_gauss_type,detector_type> all_weight_detector_type;

    vil_image_view<float> objimage(model->ni(),model->nj(),1);
    objimage.fill(-200);

    vil_image_view<bool> masked(model->ni(),model->nj(),1);
    masked.fill(false);


    for(unsigned i=0;i<points.size();i++)
    {
        double dir=vcl_atan2(tangents[i].y(),tangents[i].x());//*vnl_math::pi/180+180;
        objimage(exroi->cmin(0)+(unsigned int)vcl_floor(points[i]->x()),exroi->rmin(0)+(unsigned int)vcl_floor(points[i]->y()))=dir;
        masked(exroi->cmin(0)+(unsigned int)vcl_floor(points[i]->x()),exroi->rmin(0)+(unsigned int)vcl_floor(points[i]->y()))=true;
        dbrl_id_point_2d_sptr point=new dbrl_id_point_2d(exroi->cmin(0)+points[i]->x(),exroi->rmin(0)+points[i]->y(),dir,pointedges.size()+1);
        if(vgp.contains(point->x(),point->y()))
            pointedges.push_back(point);
    }


    dbsta2_g_mdist_detector<dbsta2_gauss_f1> det(dist);
    //dbsta2_top_weight_detector<mix_gauss_type,detector_type> detector_top(det, minweightthresh);
    dbsta2_mix_any_less_index_detector<mix_gauss_type,detector_type> detector_top(det, minweightthresh);
    vil_image_view<bool> fg(objimage.ni(),objimage.nj(),1);
    detect_masked<mix_gauss_type,all_weight_detector_type>(*model,objimage,fg,detector_top,rad,masked);

    //: retaining the edges which are foreground
    for(int i=0;i<pointedges.size();i++)
    {
        if(!fg(vcl_floor(pointedges[i]->x()),vcl_floor(pointedges[i]->y())))
            fgedges.push_back(pointedges[i]);
    }

    return fgedges;
}



//vsol_polygon_2d_sptr
//compute_spatial_support(double sigma_d,double thresh,
//                         vcl_vector< vcl_vector< dbrl_id_point_2d_sptr > > pts)
//{
//    unsigned totsize=0;
//
//    if(pts.size()<=0)
//        return new vsol_polygon_2d();
//
//    vcl_vector< dbrl_id_point_2d_sptr > allpts;
//    for(unsigned i=0;i<pts.size();i++)
//    {
//        totsize+=pts[i].size();
//        allpts.insert(allpts.end(),pts[i].begin(),pts[i].end());
//    }
//    double *weightst=new double[totsize];
//
//    for(unsigned j=0;j<totsize;j++)
//        weightst[j]=0.0;
//
//    for(unsigned j=0;j<totsize;j++) {
//        double xj=allpts[j]->x();
//        double yj=allpts[j]->y();
//        double tj=allpts[j]->tangent();
//        for(unsigned i=0;i<pts.size();i++){
//            double st=0.0;
//            for(unsigned k=0;k<pts[i].size();k++){
//               
//                    double xi=pts[i][k]->x();
//                    double yi=pts[i][k]->y();
//                    double ti=pts[i][k]->tangent();
//                    st+=compute_support(xj,yj,tj,xi,yi,ti,sigma_d);//vcl_exp(-((xi-xj)*(xi-xj)+(yi-yj)*(yi-yj))/(sigma_d*sigma_d) -(tj-ti)*(tj-ti)/(sigma_a*sigma_a));
//            }
//            if(weightst[j]<st)
//                weightst[j]=st;
//        }
//    }
//
//    vcl_vector<vsol_spatial_object_2d_sptr> vsol_out;
//    vcl_vector<vgl_point_2d<double> > supportpoints;
//    for(unsigned j=0;j<totsize;j++)
//    {
//        if(weightst[j] >thresh)
//        {
//            vsol_point_2d_sptr p=new vsol_point_2d(allpts[j]->x(),allpts[j]->y());
//            vsol_point_2d_sptr ps=new vsol_point_2d(p->x()+0.2*vcl_cos(allpts[j]->tangent()),p->y()+0.2*vcl_sin(allpts[j]->tangent()));
//            vsol_point_2d_sptr pe=new vsol_point_2d(p->x()-0.2*vcl_cos(allpts[j]->tangent()),p->y()-0.2*vcl_sin(allpts[j]->tangent()));
//            vsol_line_2d_sptr l=new vsol_line_2d(ps,pe);
//            //vsol_out.push_back(p->cast_to_spatial_object());
//            supportpoints.push_back(vgl_point_2d<double>(p->x(),p->y()));
//            //vsol_out.push_back(l->cast_to_spatial_object());
//        }  
//    }
//
//    if(supportpoints.size()<=0)
//        return new vsol_polygon_2d();
//    vgl_convex_hull_2d<double> hullp(supportpoints);
//    vgl_polygon<double> psg=hullp.hull();
//
//    if(static_cast<int>(psg[0].size())<3)
//    {
//        return new vsol_polygon_2d();
//    }
//
//    vcl_vector<vsol_point_2d_sptr> newpts;
//    for(int k=0;k<static_cast<int>(psg[0].size());k++)
//    {
//       newpts.push_back(new vsol_point_2d(psg[0][k].x(),psg[0][k].y()));
//       
//    }
//  return new vsol_polygon_2d(newpts);
//
//}
//
vsol_polygon_2d_sptr dilate_poly(vsol_polygon_2d_sptr poly, double rad)
{
   
    vcl_vector<vgl_point_2d<double> > pts;
    for(unsigned i=0;i<poly->size();i++)
        pts.push_back(vgl_point_2d<double>(poly->vertex(i)->x(),poly->vertex(i)->y()));
   
    vgl_convex_hull_2d<double> hullp(pts);
    vgl_polygon<double> psg=hullp.hull();

    if(static_cast<int>(psg[0].size())<3)
    {
        return new vsol_polygon_2d();
    }

    vcl_vector<vsol_point_2d_sptr> newpts;
    for(int k=0;k<static_cast<int>(psg[0].size())-1;k++)
    {
       vgl_point_2d<double> p0=psg[0][k];
       vgl_point_2d<double> p1=psg[0][k+1];
       vgl_point_2d<double> p=midpoint(p0,p1);

       vgl_vector_2d<double> v(p1.x()-p0.x(),p1.y()-p0.y());
       normalize(v);
       vgl_vector_2d<double> nv(v.y(),-v.x());
       vgl_point_2d<double> outp=p+rad*nv;
       newpts.push_back(new vsol_point_2d(outp.x(),outp.y()));

       
    }
    vsol_polygon_2d_sptr dilatedpoly=new vsol_polygon_2d(newpts);
    return dilatedpoly;
}

int num_of_poly(unsigned int handle)
{
 if(vcl_vector<vsol_polygon_2d_sptr > * polys=
     reinterpret_cast<vcl_vector<vsol_polygon_2d_sptr > *> (handle))
   return polys->size();
 else
   return -1;
}
int numvertices_i(int i,unsigned int polyhandle)
{
 if(vcl_vector<vsol_polygon_2d_sptr > * polys=
     reinterpret_cast<vcl_vector<vsol_polygon_2d_sptr > *> (polyhandle))
    {
      if(i<static_cast<int>(polys->size()))
        {
          return (*polys)[i]->size();
        }
    }
 return -1;
}
void returnvertices(int i, double * x, double *y, unsigned int polyhandle)
{
 if(vcl_vector<vsol_polygon_2d_sptr > * polys=
     reinterpret_cast<vcl_vector<vsol_polygon_2d_sptr > *> (polyhandle))
    {
      if(i<static_cast<int>(polys->size()))
        {
          for(int j=0;j<static_cast<int>((*polys)[i]->size());j++)
            {
                x[j]=(*polys)[i]->vertex(j)->x();
                y[j]=(*polys)[i]->vertex(j)->y();
             
            }
        }
      
    }
}





