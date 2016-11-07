// This is pro/dbdet_blob_finder_process.cxx

//:
// \file
#include "dbdet_blob_finder_process.h"
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <bil/algo/bil_blob_finder.h>
#include<bpro1/bpro1_parameters.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_binary_closing.h>
#include <vil/algo/vil_binary_opening.h>
#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_binary_erode.h>
#include <vil/algo/vil_histogram.h>
#include <vil/vil_save.h>
#include<brip/brip_vil_float_ops.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_binary_closing.h>
#include <vil/algo/vil_binary_opening.h>
#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_binary_erode.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vul/vul_timer.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vul/vul_sprintf.h>
#include <vil/vil_pixel_format.h>
#include <dbdet/tracer/dbdet_contour_tracer.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vgl/vgl_polygon.h>
//: Constructor
dbdet_blob_finder_process::dbdet_blob_finder_process(void): bpro1_process()
{

    if(!parameters()->add(  "No of Maximum  Pixels in a region " , "-maxregion" , (int)200)||
        !parameters()->add( "No of Minimum  Pixels in a region " , "-minregion" , (int)75)||
        !parameters()->add( "Use Pixel-based tracer " , "-ispixeltracer" , (bool)true)||
        !parameters()->add( "Convex Hull (on) or polygons (off) " , "-isconvexhull" , (bool)true)||
        !parameters()->add( "Radius for Erosion (" ,          "-rerode" ,  (float)2.0) ||
        !parameters()->add( "Radius for Erosion(Post dilation)" ,          "-secerode" ,  (float)2.0) ||
        !parameters()->add( "Radius for Dilation" ,          "-rdilate" ,  (float)7.0)||
        !parameters()->add( "Output for Polygons filename..." ,          "-polyfile" ,  bpro1_filepath("","*.*"))||
        !parameters()->add( "Use Subpixel-based tracer " , "-issubpixeltracer" , (bool)false)||
        !parameters()->add( "Smoothing Sigma" , "-_sigma"     , 1.0f ) ||
        !parameters()->add( "# of times to smooth"   , "-n_smooth" , 1 )||
        !parameters()->add( "threshold"   , "-thresh" , 20 )||
                !parameters()->add( "Video id"   , "-vidid" , (int)333)
        )

    {
        vcl_cerr << "ERROR: Adding parameters in dbdet_blob_finder_process()" << vcl_endl;
    }

}

//: Destructor
dbdet_blob_finder_process::~dbdet_blob_finder_process()
{

}


//: Return the name of this process
vcl_string
dbdet_blob_finder_process::name()
{
    return "Blob Finder";
}


//: Return the number of input frame for this process
int
dbdet_blob_finder_process::input_frames()
{
    return 1;
}


//: Return the number of output frames for this process
int
dbdet_blob_finder_process::output_frames()
{
    return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_blob_finder_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );

    return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_blob_finder_process::get_output_type()
{  
    vcl_vector<vcl_string > to_return;
    to_return.push_back( "vsol2D" );
    to_return.push_back( "image" );
    return to_return;
}


//: Execute the process
bool
dbdet_blob_finder_process::execute()
{
    if ( input_data_.size() != 1 ){
        vcl_cout << "In dbdet_blob_finder_process::execute() - "
            << "not exactly one input images \n";
        return false;
    }


    if(input_data_[0].size()<1)
        return false;
    clear_output();

    static int maxregion;
    static int minregion;
    static float eroderadius;
    static float dilateradius;
    static float seconderoderadius;
    //static float aspectratio=0.1f;

    bpro1_filepath filename;
    parameters()->get_value("-polyfile",filename);
    polyfilename=filename.path;
    parameters()->get_value( "-maxregion" , maxregion );
    parameters()->get_value( "-minregion" , minregion );
    parameters()->get_value( "-rerode" , eroderadius );
    parameters()->get_value( "-rdilate" , dilateradius );
    parameters()->get_value( "-secerode" , seconderoderadius );
    

    vidpro1_image_storage_sptr image;
    image.vertical_cast(input_data_[0][0]);

    vil_image_resource_sptr image_sptr = image->get_image();

    vil_image_view<unsigned char> inputimage,grey_img;
    inputimage = image_sptr->get_view();

    // Convert to greyscale images if needed
    if( inputimage.nplanes() == 3 ) {
        vil_convert_planes_to_grey( inputimage , grey_img );
    }
    else if ( inputimage.nplanes() == 1 ) {
        grey_img = inputimage;
    } else {
        vcl_cerr << "Returning false. inputimage.nplanes(): " << inputimage.nplanes() << vcl_endl;
        return false;
    }

    vil_image_view<bool> boolimage;
    if(grey_img.pixel_format()==VIL_PIXEL_FORMAT_BOOL)
        boolimage=grey_img;
    if(grey_img.pixel_format()==VIL_PIXEL_FORMAT_BYTE)

    boolimage=brip_vil_float_ops::convert_to_bool(grey_img);


    vil_image_view<bool> temp;
    vil_structuring_element se;


    se.set_to_disk(eroderadius);
    vil_binary_erode(boolimage,temp,se);
    //vil_binary_erode(temp,boolimage,se);
    se.set_to_disk(dilateradius);
    vil_binary_dilate(temp,boolimage,se);
    se.set_to_disk(seconderoderadius);
    vil_binary_erode(boolimage,temp,se);
    
    bool ispixeltracer=false;
    bool isconvexhull=false;
    float sigma=1.0;
    int n_smooth=3;
    int threshold=100;
    parameters()->get_value( "-ispixeltracer" , ispixeltracer );
    parameters()->get_value( "-isconvexhull" , isconvexhull );
    parameters()->get_value( "-_sigma" , sigma );
    parameters()->get_value( "-n_smooth" , n_smooth );
    parameters()->get_value( "-thresh" , threshold );
    vcl_vector<vsol_spatial_object_2d_sptr> polygons;

        vcl_vector<vsol_polygon_2d_sptr> polys;
    if(ispixeltracer)
        {
        vil_image_view<unsigned char> outimage(boolimage.ni(),boolimage.nj());
        outimage.fill(0);
        vcl_vector<int> bi,bj;
        bil_blob_finder finder(temp);
        

        vcl_cout<<"\nProcessing Frame No: "<<input_data_[0][0]->frame();

        while (finder.next_4con_region(bi,bj))
            {
            if(bi.size()> static_cast<unsigned>(minregion) && bi.size()<static_cast<unsigned>(maxregion))
                {

                vcl_vector<vsol_point_2d_sptr> points; 
                vcl_vector<vgl_point_2d<double> > pts; 
                for(unsigned i=0;i<bi.size();i++)
                    {
                    outimage(bi[i],bj[i])=255;
                    points.push_back(new vsol_point_2d(bi[i],bj[i]));
                    pts.push_back(vgl_point_2d<double>(bi[i],bj[i]));
                    }
                if(isconvexhull)
                    {
                    vgl_convex_hull_2d<double> hullp(pts);
                    vgl_polygon<double> psg=hullp.hull();
                    vcl_vector<vsol_point_2d_sptr> contour;
                    for(int k=0;k<static_cast<int>(psg[0].size());k++)
                        {
                        vsol_point_2d_sptr p =new vsol_point_2d(psg[0][k].x(),psg[0][k].y());
                        contour.push_back(p);
                        }
                        vsol_polygon_2d_sptr newContour = new vsol_polygon_2d (contour);
                                                //ctracer.largest_contour()
                                                polys.push_back(newContour);
                        polygons.push_back(newContour->cast_to_spatial_object());
                        }
                else
                    {
                vsol_polygon_2d_sptr polygon=new vsol_polygon_2d(points);
                polygons.push_back(polygon->cast_to_spatial_object() );
                polys.push_back(polygon );
                    }
                }
            }

        frame_polygons_.push_back(polys);

        }
    else{

        //invoke the tracer
        dbdet_contour_tracer ctracer;
        ctracer.set_sigma(sigma);
        ctracer.trace(temp);

        //get the interesting contours
        vcl_vector< vsol_spatial_object_2d_sptr > contours;
        for (unsigned i=0; i<ctracer.contours().size(); i++)
            {
            if (ctracer.contours()[i].size()>=static_cast<unsigned>(minregion) && ctracer.contours()[i].size()<=static_cast<unsigned>(maxregion))
                {
                //: making a convex hull of contour

                if(isconvexhull)
                    {
                vcl_vector<vgl_point_2d<double> > ps;
                for(unsigned j=0;j<ctracer.contours()[i].size();j++)
                    {
                        vgl_point_2d<double> p(ctracer.contours()[i][j]->x(),ctracer.contours()[i][j]->y());
                        ps.push_back(p);
                    }

                vgl_convex_hull_2d<double> hullp(ps);
                vgl_polygon<double> psg=hullp.hull();
                vcl_vector<vsol_point_2d_sptr> contour;
                for(int k=0;k<static_cast<int>(psg[0].size());k++)
                    {
                    vsol_point_2d_sptr p =new vsol_point_2d(psg[0][k].x(),psg[0][k].y());
                    contour.push_back(p);
                    }
                    vsol_polygon_2d_sptr newContour = new vsol_polygon_2d (contour);//ctracer.largest_contour()
                                        polys.push_back(newContour );
                    polygons.push_back(newContour->cast_to_spatial_object());
                    }
                else
                    {
                    vsol_polygon_2d_sptr newContour = new vsol_polygon_2d (ctracer.contours()[i]);//ctracer.largest_contour()
                    polys.push_back(newContour );
                                        polygons.push_back(newContour->cast_to_spatial_object());
                    }
                }
            }
        frame_polygons_.push_back(polys);
        }
    

    vidpro1_vsol2D_storage_sptr output_storage2 = vidpro1_vsol2D_storage_new();
    output_storage2->add_objects(polygons);
    output_data_[0].push_back(output_storage2);

    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(temp));
    output_data_[0].push_back(output_storage);

        
    return true;
}
//: Clone the process
bpro1_process*
dbdet_blob_finder_process::clone() const
{
    return new dbdet_blob_finder_process(*this);
}

bool
dbdet_blob_finder_process::finish()
{
    write_polygons(polyfilename);
    return true;
}

void 
dbdet_blob_finder_process::write_polygons(vcl_string filename)
{

        int vidid=333;
        parameters()->get_value( "-vidid" , vidid );
        vcl_ofstream ofile(filename.c_str());
        ofile<<"VIDEOID: "<<vidid<<"\n";
        ofile<<"NFRAMES: "<<frame_polygons_.size()<<"\n";
        for(unsigned i=0;i<frame_polygons_.size();i++)
        {
                ofile<<"NOBJECTS: "<<frame_polygons_[i].size()<<"\n";
                ofile<<"NPOLYS: "<<frame_polygons_[i].size()<<"\n";
                for(unsigned j=0;j<frame_polygons_[i].size();j++)
                {
                        ofile<<"NVERTS: "<<frame_polygons_[i][j]->size()<<"\n";
                        ofile<<"X: "<<"\n";
                        for(unsigned k=0;k<frame_polygons_[i][j]->size();k++)
                                ofile<<frame_polygons_[i][j]->vertex(k)->x()<<" ";
                        ofile<<"\n";
                        ofile<<"Y: "<<"\n";
                        for(unsigned k=0;k<frame_polygons_[i][j]->size();k++)
                                ofile<<frame_polygons_[i][j]->vertex(k)->y()<<" ";
                        ofile<<"\n";


                }
        }

        ofile.close();
}

vcl_vector< vcl_vector < vsol_polygon_2d_sptr > > 
dbdet_blob_finder_process::getpolygons()
{
  return frame_polygons_;
}

double
dbdet_blob_finder_process::compute_aspect_ratio(vcl_vector<vgl_point_2d<double> > pts)
{
    double x=0.0;
    double y=0.0;
    for(unsigned i=0;i<pts.size();i++)
    {
        x+=pts[i].x();
        y+=pts[i].y();
    }

    x/=pts.size();
    y/=pts.size();

    double a11=0;
    double a12=0;
    double a21=0;
    double a22=0;

    for(unsigned i=0;i<pts.size();i++)
    {
        a11+=(pts[i].x()-x)*(pts[i].x()-x);
        a12+=(pts[i].x()-x)*(pts[i].y()-y);
        a21+=(pts[i].y()-y)*(pts[i].x()-x);
        a22+=(pts[i].y()-y)*(pts[i].y()-y);
    }

    vnl_matrix<double> a(2,2);
    a[0][0]=a11; a[0][1]=a12;
    a[1][0]=a21; a[1][1]=a22;


    vnl_svd<double> svd(a);
    vnl_vector<double> temp=svd.nullvector();

    double t=vcl_fabs(temp[0]/temp[1]);

    if(t>1)
        return 1/t;
    else
        return t;

}

