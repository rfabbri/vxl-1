
// aerial_vehicle_segmentation.cpp : Defines the entry point for the DLL application.
#include <vehicleseg/exe/aerial_background_foreground_detection.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
//#include <dbbgm/pro/dbbgm_aerial_fg_uncertainity_detect_process.h>
//#include <dbbgm/pro/dbbgm_aerial_bg_model_process.h>
#include <dbdet/pro/dbdet_blob_finder_process.h>

#include <vil/vil_image_resource.h>
#include <bpro1/bpro1_parameters_sptr.h>
#include <bpro1/bpro1_parameters.h>
#include <bpro1/bpro1_process_sptr.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <dbsta/dbsta_gaussian_indep.h>
#include <dbsta/dbsta_distribution.h>
#include <dbsta/dbsta_gaussian_full.h>
#include <dbsta/dbsta_gaussian_sphere.h>
#include <dbsta/dbsta_detector_gaussian.h>
#include <dbsta/dbsta_detector_mixture.h>
#include <dbsta/dbsta_basic_functors.h>
#include <dbsta/dbsta_functor_sptr.h>
#include <dbsta/algo/dbsta_gaussian_stats.h>
#include <dbsta/algo/dbsta_adaptive_updater.h>
#include <dbbgm/dbbgm_update.h>
#include <dbbgm/dbbgm_detect.h>
#include <brip/brip_vil_float_ops.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_binary_erode.h>
#include <vil/algo/vil_binary_dilate.h>
#include <bil/algo/bil_blob_finder.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsl/vsl_binary_io.h>
#include <dbdet/tracer/dbdet_contour_tracer.h>
#include <vil/vil_save.h>
#include <vil/vil_transpose.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vgl/vgl_polygon.h>

unsigned int intializemodel( int ni,int nj)
{
    dbsta_mixture<float> mixture;
    mixture.set_data(dbsta_stats_data<float>(0));
    dbbgm_distribution_image<float> * model = new dbbgm_distribution_image<float>(ni,nj,mixture);
    return  reinterpret_cast<unsigned int>(model);

}

void savemodel(unsigned int handle,char *filename)
{
  vsl_add_to_binary_loader(dbbgm_distribution_image<float>());
  vsl_add_to_binary_loader(dbsta_gaussian_indep<float,1>());
  if(dbbgm_distribution_image<float> * model=reinterpret_cast<dbbgm_distribution_image<float> *> (handle))
    {
      vsl_b_ofstream ofile(filename);
      model->b_write(ofile);

      ofile.close();
      
    }
}

unsigned int loadmodel(char *filename)
{
  vsl_add_to_binary_loader(dbbgm_distribution_image<float>());
  vsl_add_to_binary_loader(dbsta_gaussian_indep<float,1>());
  vsl_add_to_binary_loader(dbsta_mixture<float>());
  dbbgm_distribution_image<float> * model= new dbbgm_distribution_image<float>();
  vsl_b_ifstream ifile(filename);
  vcl_cout<<"\n file opened succesfully";
  vcl_cout.flush();
  model->b_read(ifile);
  ifile.close();
  return  reinterpret_cast<unsigned int>(model);

}
void updatemodel(unsigned int handle,unsigned char *imgdata,unsigned int max_components,float init_var,float init_weight,int ni,int nj)
    {
    dbsta_gaussian_indep<float,1> init_gauss(vnl_vector_fixed<float,1>(0.0f),
                                             vnl_vector_fixed<float,1>(init_var) );
    init_gauss.set_data(dbsta_stats_data<float>(1));                                  
    dbsta_mg_window_updater<float> updater(init_gauss, max_components);
    
    //vcl_cout.flush();
    if(dbbgm_distribution_image<float> * model=reinterpret_cast<dbbgm_distribution_image<float> *> (handle))
      {
          vil_image_view<unsigned char> imgd(imgdata,ni,nj,1,1,ni,ni*nj);
          vil_image_view<float> img=brip_vil_float_ops::convert_to_float(imgd);
          update(*model,img,updater,0.5f);
      }
    }

unsigned int getpolygon(unsigned int handle,unsigned char* img_data,float sigmathresh,float minweightthresh,
               int min_no_of_pixels,int max_no_of_pixels,float erosionrad,float postdilationerosionrad,
               float dilationrad, int ni,int nj,float sigma, int rad_of_uncert)
{
  vcl_cout<<"\n get polygons ...";
  if(dbbgm_distribution_image<float> * model=reinterpret_cast<dbbgm_distribution_image<float> *> (handle))
    {
    
    vcl_cout<<"\n Size of the model is "<<model->ni()<<"\t"<<model->nj();
    dbsta_top_weight_detector<float> detector(new dbsta_g_mdist_detector<float>(sigmathresh), minweightthresh);
    vil_image_view<unsigned char> imgd(img_data,ni,nj,1,1,ni,ni*nj);

    vil_image_view<float> img=brip_vil_float_ops::convert_to_float(imgd);
    vil_image_view<bool> fg(img.ni(),img.nj(),1);
    vil_image_view<float> fgbyteimg(img.ni(),img.nj(),1);
    fg.fill(false);
    detect(*model,img,detector,vnl_vector_fixed<float,1>(0.5f).as_ref(),rad_of_uncert,fg);
    
    vil_image_view<bool> temp(fg.ni(),fg.nj(),1);
    vil_image_view<unsigned char> temp1(fg.ni(),fg.nj(),1);

    for (unsigned i=0;i<fg.ni();i++)
      for(unsigned j=0;j<fg.nj();j++)
          {
          if(!fg(i,j))
              temp(i,j)=true;

          else 
              temp(i,j)=false;
          }
      //vil_save(temp1,"temp.tiff");

    vcl_vector<vcl_vector<vsol_polygon_2d_sptr > > frame_polygons;

    
    vil_structuring_element se;
    
    se.set_to_disk(erosionrad);
    vil_binary_erode(temp,fg,se);
    se.set_to_disk(dilationrad);
    vil_binary_dilate(fg,temp,se);
    se.set_to_disk(postdilationerosionrad);
    vil_binary_erode(temp,fg,se);


    for (unsigned i=0;i<fg.ni();i++)
      for(unsigned j=0;j<fg.nj();j++)
          {
          if(fg(i,j))
              temp1(i,j)=255;

          else 
              temp1(i,j)=0;
          }
      vil_save(temp1,"temp.tiff");

    //invoke the tracer
    dbdet_contour_tracer ctracer;
    ctracer.set_sigma(sigma);
    ctracer.trace(fg);
    
  //get the interesting contours
  vcl_vector<vcl_vector<vsol_point_2d_sptr > > * allpts=new vcl_vector<vcl_vector<vsol_point_2d_sptr > >();
  for (unsigned i=0; i<ctracer.contours().size(); i++)
  {

    if (static_cast<int>(ctracer.contours()[i].size())<=max_no_of_pixels && static_cast<int>(ctracer.contours()[i].size())>=min_no_of_pixels)
    {
      allpts->push_back(ctracer.contours()[i]);//ctracer.largest_contour()
    }
  }


    //vil_image_view<unsigned char> outimage(temp.ni(),temp.nj());

    //outimage.fill(0);
    //vcl_vector<int> bi,bj;

    //bil_blob_finder finder(temp);
    //
    //vcl_vector<vsol_spatial_object_2d_sptr> polygons;
    //vcl_vector<vsol_polygon_2d_sptr> polys;
    //vcl_vector<vcl_vector<vsol_point_2d_sptr > > * allpts= new vcl_vector<vcl_vector<vsol_point_2d_sptr > >(); 
    //meanpts.clear();
    //while (finder.next_4con_region(bi,bj))
    //    {
    //    if(bi.size()> static_cast<unsigned>(min_no_of_pixels) && bi.size()<static_cast<unsigned>(max_no_of_pixels))
    //        {

    //        vcl_vector<vsol_point_2d_sptr> points; 

    //        double mux=0;
    //        double muy=0;

    //        for(unsigned i=0;i<bi.size();i++)
    //            {
    //            outimage(bi[i],bj[i])=255;
    //            points.push_back(new vsol_point_2d(bi[i],bj[i]));
    //            //pts.push_back(vgl_point_2d<double>(bi[i],bj[i]));
    //            mux+=bi[i];
    //            muy+=bj[i];
    //            }
    //        allpts->push_back(points);  
    //        mux/=bi.size();
    //        muy/=bj.size();
    //        meanpts.push_back(vgl_point_2d<double> (mux,muy));

    //        }
    //    }
    //int no_of_polys=meanpts.size();
    unsigned int polyhandle=reinterpret_cast<unsigned int>(allpts);
    vcl_ofstream ofile("temp.txt");
    ofile<<"\n No of contours are "<<allpts->size();
    ofile.close();
    vcl_cout.flush();
    return polyhandle;//no_of_polys;
  
    }


    return 0;
}
int num_of_poly(unsigned int handle)
{
 if(vcl_vector<vcl_vector<vsol_point_2d_sptr > > * polys=
     reinterpret_cast<vcl_vector<vcl_vector<vsol_point_2d_sptr > > *> (handle))
   return polys->size();
 else
   return -1;
}
int numvertices_i(int i,unsigned int polyhandle)
{
 if(vcl_vector<vcl_vector<vsol_point_2d_sptr > > * polys=
     reinterpret_cast<vcl_vector<vcl_vector<vsol_point_2d_sptr > > *> (polyhandle))
    {
      if(i<static_cast<int>(polys->size()))
        {
          return (*polys)[i].size();
        }
    }
 return -1;
}
void returnvertices(int i, double * x, double *y, unsigned int polyhandle)
{
  if(vcl_vector<vcl_vector<vsol_point_2d_sptr > > * polys=
     reinterpret_cast<vcl_vector<vcl_vector<vsol_point_2d_sptr > > *> (polyhandle))
    {
      if(i<static_cast<int>(polys->size()))
        {
          for(int j=0;j<static_cast<int>((*polys)[i].size());j++)
            {
              x[j]=(*polys)[i][j]->x();
              y[j]=(*polys)[i][j]->y();
            }
        }
      
    }
}
unsigned int getconvexhulls(unsigned int polyhandle)
    {
  vcl_vector<vcl_vector<vsol_point_2d_sptr > > * allpts=new vcl_vector<vcl_vector<vsol_point_2d_sptr > >();
    if(vcl_vector<vcl_vector<vsol_point_2d_sptr > > * polys=
        reinterpret_cast<vcl_vector<vcl_vector<vsol_point_2d_sptr > > *> (polyhandle))
        {
        for(int i=0;i<static_cast<int>(polys->size());i++)
            {
            vcl_vector<vgl_point_2d<double> > ps;
            for(int j=0;j<static_cast<int>((*polys)[i].size());j++)
                {
                vgl_point_2d<double> p((*polys)[i][j]->x(),(*polys)[i][j]->y());
                ps.push_back(p);
                }
            vcl_vector<vsol_point_2d_sptr> cps;
            vgl_convex_hull_2d<double> hullp(ps);
            vgl_polygon<double> psg=hullp.hull();
            for(int k=0;k<static_cast<int>(psg[0].size());k++)
                cps.push_back(new vsol_point_2d(psg[0][k].x(),psg[0][k].y()));
            allpts->push_back(cps);
            }

        return reinterpret_cast<unsigned int>(allpts);
        }
    }
int getnumpointsconvexhull(int i,  unsigned int polyhandle)
{
    
vcl_vector<vgl_point_2d<double> > ps;

if(vcl_vector<vcl_vector<vsol_point_2d_sptr > > * polys=
   reinterpret_cast<vcl_vector<vcl_vector<vsol_point_2d_sptr > > *> (polyhandle))
    {
    if(i<static_cast<int>(polys->size()))
        {
        for(int j=0;j<static_cast<int>((*polys)[i].size());j++)
            {
            vgl_point_2d<double> p((*polys)[i][j]->x(),(*polys)[i][j]->y());
            ps.push_back(p);
            }
        vgl_convex_hull_2d<double> hullp(ps);
        vgl_polygon<double> psg=hullp.hull();

        return psg[0].size();
        }
    }
}

void getconvexhullpoints(int i, double * x, double *y, unsigned int polyhandle)
{
    
vcl_vector<vgl_point_2d<double> > ps;
if(vcl_vector<vcl_vector<vsol_point_2d_sptr > > * polys=
   reinterpret_cast<vcl_vector<vcl_vector<vsol_point_2d_sptr > > *> (polyhandle))
    {
    if(i<static_cast<int>(polys->size()))
        {
        for(int j=0;j<static_cast<int>((*polys)[i].size());j++)
            {
            vgl_point_2d<double> p((*polys)[i][j]->x(),(*polys)[i][j]->y());
            ps.push_back(p);
            }
        vgl_convex_hull_2d<double> hullp(ps);
        vgl_polygon<double> psg=hullp.hull();

        

       
        for(int k=0;k<static_cast<int>(psg[0].size());k++)
        {
        x[k]=psg[0][k].x();
        y[k]=psg[0][k].y();
        }
        
        }
    }
}
void returnmean(int i, double *pt, unsigned int polyhandle)
{

       if(vcl_vector<vcl_vector<vsol_point_2d_sptr > > * polys=
          reinterpret_cast<vcl_vector<vcl_vector<vsol_point_2d_sptr > > *> (polyhandle))
         {
             if(i<static_cast<int>(polys->size()))
               {
                 vsol_polygon_2d p((*polys)[i]);
                 pt[0]=p.centroid()->x();
                 pt[1]=p.centroid()->y();
                 
               }

         }
}


