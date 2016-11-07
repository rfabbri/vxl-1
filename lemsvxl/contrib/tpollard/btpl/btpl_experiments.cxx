#include <vcl_iostream.h>

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_cstdio.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_distance.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/algo/vnl_svd.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_resample_bicub.h>
#include <vil/vil_resample_bilin.h>
#include <vil/algo/vil_sobel_1x3.h>
#include <vul/vul_awk.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
//#include <bgeo/bgeo_lvcs.h>
//#include <btpl/btpl_satellite_tools/btpl_satellite_tools.h>
#include "baml/baml_mog.h"
#include "baml/baml_prob_img.h"

// Assorted experimental apps, to be moved when mature.
void recover_normalization();
void joint_change();
void exp_change_render();
void add_haze();
void contrast_exp();
void prob_img_test();
void shadow_thresh();
void intensity_ratios();
void inspect_baghdad();
void syn_brdf_exp1();
void plasticville_exp1();
void beach_exp1();
void beach_exp2();
void change_render();
void registration_render();
void overhead_cam();
void phong_exp();
void edge_hist();
void phong_exp2();
void rocgen();
void rocgen2();
void draw_black();

int main( int argc, char* argv[] )
{  
  draw_black();
  return 1;
};


// Helper apps.
void load_view_files(
  vcl_string img_dir,
  vcl_string camera_file,
  vcl_string lighting_file,
  vcl_vector< vcl_string >* imgs,
  vcl_vector< vpgl_proj_camera<double> >* cams,
  vcl_vector< vgl_vector_3d<double> >* lights );

void write_x3d_insp_file(
  const vcl_vector< vcl_vector<double> >& obs,
  const vcl_vector< vgl_vector_3d<double> >& lights,
  vcl_string file );


//------------------------------------------------------
void draw_black()
{
  vil_image_view<vxl_byte> img = vil_load("D:/images_multiview/baghdad/parkway/06NOV11081521-P1BS-005630692010_01_P001_parkway.png");
  vil_image_view<float> changes = vil_load("D:/images_multiview/baghdad/parkway_warped/test_aff/06nov_change_aff.tiff");
  vcl_string save_img("D:/out.png");
  float scale = 1.0;
  float thresh = .001;

  for( int i = 0; i < img.ni(); i++ ){
    for( int j = 0; j < img.nj(); j++ ){
      img(i,j,0) = img(i,j,1) = img(i,j,2) = 128+(int)floor(128*img(i,j)/(float)256);
      int ci = (int)floor(scale*i), cj = (int)floor(scale*j);
      if( ci >= changes.ni() || cj >= changes.nj() ) continue;
      if( changes(ci,cj) < thresh ) img(i,j,0) = img(i,j,1) = img(i,j,2) = 0;
    }
  }
  vil_save( img, save_img.c_str() );

};


//-----------------------------------------------------
void rocgen()
{
  vcl_string test_dir("D:/images_multiview/baghdad/hiafa_warped/test_aff"); 
  vcl_string truth_dir("D:/images_multiview/baghdad/hiafa_truth");
  vcl_ofstream roc_ofs("D:/roc.txt");
  float scale = 1.0;

  for( float thresh = .0001; thresh < 5.0; thresh*=1.5 ){
    vcl_cerr << thresh << '\n';

    int true_positives = 0, false_positives = 0;//,  true_negatives = 0, false_negatives = 0;
    int total_changes = 0, total_nonchanges = 0;

    vul_file_iterator truth_iter = (truth_dir+"\\*").c_str();
    for( vul_file_iterator test_iter= (test_dir+"\\*").c_str(); test_iter; ++test_iter ){
      vcl_string test_file( test_iter() );
      vcl_string truth_file( truth_iter() );
      vcl_string test_ext = vul_file::extension( test_file );
      if( test_ext != ".tiff" ){++truth_iter; continue;}
      
      vil_image_view<float> test_img = vil_load( test_file.c_str() );
      vil_image_view<vxl_byte> truth_img = vil_load( truth_file.c_str() );

      for( int i = 0; i < truth_img.ni(); i++ ){
        for( int j = 0; j < truth_img.nj(); j++ ){
          bool true_change = false;
                    int fucked = truth_img(i,j);
          if( truth_img(i,j) == 255 ) true_change = true;
          else if( truth_img(i,j) != 0 ) continue;
          bool detected_change = false;
          int testi = (int)floor(i*scale), testj = (int)floor(j*scale);
          if( testi >= test_img.ni() || testj >= test_img.nj() ) continue;
          if( test_img(testi,testj) < thresh ) detected_change = true;
          if( true_change && detected_change ) true_positives++;
          //if( true_change && !detected_change ) true_negatives++;
          //if( !true_change && !detected_change ) false_negatives++;
          if( !true_change && detected_change ) false_positives++;
          if( true_change ) total_changes++;
          if( !true_change ) total_nonchanges++;
        }
      }

      ++truth_iter;
    }

    roc_ofs << false_positives/(float)(total_nonchanges) << '\t' 
      << true_positives/(float)(total_changes) << '\n'; 
  }

};


//------------------------------------------------------------
void phong_exp2()
{
  int rad = 100;
  vcl_string imgname("D:/phong_pic.png");
  vcl_ofstream samplefile("D:/samples.txt");
  float a = 127, b=128;
  int alpha = 8;
  float rx=0, ry=.05, rz=1.0;
  float vphi = 60.0*(3.1417/2.0)/90.0;
  int vtheta_incs = 500;

  float n = vcl_sqrt( rx*rx+ry*ry+rz*rz );
  rx/=n; ry/=n; rz/=n;
  vil_image_view<vxl_byte> img( 1+2*rad, 1+2*rad );

  for( int dx = -rad; dx <=rad ; dx++ ){
    for( int dy = -rad; dy <=rad ; dy++ ){
      float x = dx/(float)rad, y = dy/(float)rad;
      int cx = dx+rad, cy = dy+rad;

      if( x*x+y*y > 1 ){
        img(cx,cy) = 0;
        continue;
      }

      float z = (vcl_sqrt(1-(x*x+y*y)));
      float rv = x*rx+y*ry+z*rz;
      if( rv < 0 )
        img(cx,cy)=a;
      else{
        rv = vcl_pow( rv, alpha );
        img(cx,cy)=a+b*rv;
      }

    }
  }


  for( int i = 0; i < vtheta_incs; i++ ){
    float vtheta = (2*3.1417)*i/(float)vtheta_incs;
    float vx = vcl_cos(vphi)*vcl_cos(vtheta);
    float vy = vcl_cos(vphi)*vcl_sin(vtheta); 
    float vz = vcl_sin(vphi);
    int cx = (int)vcl_floor(rad + vx*rad);
    int cy = (int)vcl_floor(rad + vy*rad);
    img(cx,cy)=0.0;

    float rv = vx*rx+vy*ry+vz*rz;
    if( rv < 0 ) rv = 0;
    rv = vcl_pow( rv, alpha );
    samplefile << (a+b*rv) << '\n';
  }
  vil_save( img, imgname.c_str() );

};


//-----------------------------------------------------------
void edge_hist()
{
  vil_image_view<vxl_byte> I = vil_load("D:/images_multiview/CapitolSite/video_grey/frame_00000.png");
  vgl_point_2d<int> p1(1071,170+2);
  vgl_point_2d<int> p2(1060,163+2);
  double dist_thresh = 1.0;
  vcl_ofstream ofs( "D:/hist.txt" );
  
  vgl_line_2d<double> l( vgl_point_2d<double>(p1.x(), p1.y()), vgl_point_2d<double>(p2.x(), p2.y()) );
  int min_x = p1.x(), max_x = p2.x();
  if( min_x > max_x ){ min_x = p2.x(); max_x = p1.x(); }
  int min_y = p1.y(), max_y = p2.y();
  if( min_y > max_y ){ min_y = p2.y(); max_y = p1.y(); }

  vnl_vector<int> h(20);
  int h_counter = 0;
  for( int x = min_x; x <= max_x; x++ ){
    for( int y = min_y; y <= max_y; y++ ){
      vgl_point_2d<double> p(x,y);
      if( vgl_distance( l, p ) > dist_thresh ) continue;
      int h_bin = (int)floor( 20*I(x,y)/256.0 );
      h(h_bin)++;
      h_counter++;
    }
  }

  for( int i = 0; i < 20; i++ )
    ofs << h(i)/(double)h_counter << '\n';

};


//------------------------------------------------------------
void phong_exp()
{
  float iad = .2;
  float is_c = .1;
  int alpha = 64;

  // Calculate is
  float is = is_c/(3.1417/4);
  for( int i = 4; i <= alpha; i+=2 )
    is*=(i/(float)(i-1));

  // Set up the MOG
  baml_mog mog( 3 );
  float mog_data[9]; mog.set_component(0,mog_data);

  // Set up the histogram
  int H[20];
  for( int i = 0; i < 20; i++ ) H[i] = 0;

  // Train on samples
  int num_training_samples = 10000;
  for( int i = 0; i < num_training_samples; i++ ){
    float sample = iad + is*vcl_pow( vcl_cos( 3.1416*(rand()/(float)RAND_MAX)/2.0 ), alpha );
    if( sample > 1.0 ) sample = .9999;
    H[(int)floor(sample*20)]++;
    mog.update( sample, 1.0, vnl_vector<float>() );
  }

  // Print the learned mixture and histogram
  for( int i = 0; i < 20; i++ )
    vcl_cerr << H[i] << ' ';
  vcl_cerr << '\n';
  for( int i = 0; i < 9; i++ )
    vcl_cerr << mog.get_data()[i] << ' ';

  vcl_ofstream ofs( "D:/a.txt" );
  for( float x = 0; x <= 1.01; x += .01 ){
    ofs << x << '\t' << mog.prob(x,vnl_vector<float>()) << '\n';
  }
  
  // Generate new distribution samples and change samples
  vnl_vector<float> dist_samples(10000);
  vnl_vector<float> change_samples(10000);
  for( int i = 0; i < dist_samples.size(); i++ ){
    float sample = iad + is*vcl_pow( vcl_cos( 3.1416*(rand()/(float)RAND_MAX)/2.0 ), alpha );
    if( sample > 1.0 ) sample = .9999;
    dist_samples[i] = sample;
  }
  for( int i = 0; i < change_samples.size(); i++ ){
    change_samples[i] = rand()/(float)RAND_MAX;
  }

  // Compute ROC
  vcl_string roc_file( "D:\\roc.txt");
  vcl_ofstream rocstream( roc_file.c_str() );
  for( float thresh = .1; thresh < 100.0; thresh *= 1.5 ){

    int false_positives = 0;
    for( int i = 0; i < dist_samples.size(); i++ )
      if( mog.prob( dist_samples[i], vnl_vector<float>() ) < thresh ) 
        false_positives++;
    
    int true_positives = 0;
    for( int i = 0; i < change_samples.size(); i++ )
      if( mog.prob( change_samples[i], vnl_vector<float>() ) < thresh ) 
        true_positives++;
    
    rocstream << false_positives/(float)dist_samples.size() << '\t' << true_positives/(float)change_samples.size() << '\n';
  }
};

//-----------------------------------------------------------
void recover_normalization()
{
  vcl_string out_file("D:/norm_params.txt");
  vcl_ofstream ofs(out_file.c_str());
  vcl_string orig_dir("D:/images_multiview/baghdad/hiafa");
  vcl_string norm_dir("D:/results/baghdad/hiafa_mmog1/norm");
  int num_samples = 100;

  vul_file_iterator of = (orig_dir+"\\*").c_str();
  for( vul_file_iterator nf= (norm_dir+"\\*").c_str(); nf; ++nf ){
    vcl_string orig_file( of() );
    vcl_string norm_file( nf() );
    vcl_string orig_ext = vul_file::extension( orig_file );
    if( orig_ext != ".png" ){++of; continue;}
    vcl_string norm_ext = vul_file::extension( norm_file );
    if( norm_ext != ".png" ){++of; continue;}
    vil_image_view<vxl_byte> orig_img = vil_load( orig_file.c_str() );
    vil_image_view<vxl_byte> norm_img = vil_load( norm_file.c_str() );

    vnl_matrix<float> A(num_samples,2);
    vnl_vector<float> B(num_samples);
    for( int i = 0; i < num_samples; i++ ){
      int sx = (int)floor( rand()*500/(double)RAND_MAX );
      int sy = (int)floor( rand()*500/(double)RAND_MAX );
      float o1 = orig_img(sx,sy)/255.0;
      float n1 = norm_img(sx,sy)/255.0;
      A(i,0) = o1; A(i,1) = 1.0;
      B(i) = n1;
    }

    vnl_svd<float> A_svd( A );
    vnl_vector<float> X = A_svd.solve( B );

    vnl_vector<float> R = (A*X)-B;
    float R_sum = 0.0;
    for( int i = 0; i < num_samples; i++ )
      R_sum += fabs( R(i) );
    R_sum /= (float)num_samples;
 
    ofs << X << ' ' << orig_file << '\n';
    ++of;
  }
  
};


//--------------------------------------------------------------------
void joint_change()
{
  vil_image_view<vxl_byte> img1 = vil_load("D:/results/baghdad/micd/embassy/07JAN25/base.png");
  vil_image_view<vxl_byte> img2 = vil_load("D:/results/baghdad/micd/embassy/07JAN25/registered2.png");
  int num_bins = 32;

  double num_samples = img1.ni()*img1.nj();

  // Get the joint histogram
  vnl_matrix<double> p12(num_bins, num_bins, 0.0 );
  vnl_vector<double> p1(num_bins,0.0), p2(num_bins,0.0);
  for( int i = 0; i < img1.ni(); i++ ){
    for( int j = 0; j < img1.nj(); j++ ){
      int b1 = (int)floor( num_bins*img1(i,j)/256.0 );
      int b2 = (int)floor( num_bins*img2(i,j)/256.0 );
      p12(b1,b2) += 1.0;
      p1(b1) += 1.0;
      p2(b2) += 1.0;
    }
  }

  // Get the e1 errors.
  vil_image_view<vxl_byte> img1e( img1.ni(), img1.nj() );
  for( int i = 0; i < img1.ni(); i++ ){
    for( int j = 0; j < img1.nj(); j++ ){
      int b1 = (int)floor( num_bins*img1(i,j)/256.0 );
      int b2 = (int)floor( num_bins*img2(i,j)/256.0 );
      img1e(i,j) = (int)floor(30*255*p12(b1,b2)/(double)num_samples); continue;
    }
  }

  vil_save( img1e, "D:/img.png" );
};


//---------------------------------------------------------------------
void exp_change_render()
{
  vil_image_view<vxl_byte> img1 = vil_load( "D:/images_multiview/baghdad/hiafa/07JAN25080151-P1BS-005630695010_01_P001_hiafa.png" );
  //vil_image_view<vxl_byte> change = vil_load( "D:/results/baghdad/indp_roc/hiafa_jan25/detected_changes/07JAN25.jpg" );
  vil_image_view<vxl_byte> change = vil_load( "D:/results/baghdad/hiafa_mmog1/07JAN25080151-P1BS-005630695010_01_P001_hiafa_prob_map5.png" );
  float prob_thresh = (float)0.2;
  float img_scale = (float).5;
  int min_color = 128;

  vil_image_view<vxl_byte> img2( img1.ni(), img1.nj(), 1 );
  for( int i = 0; i < img1.ni(); i++ ){
    for( int j = 0; j < img1.nj(); j++ ){
      float prob = change( (int)floor(i*(img_scale-.001)), (int)floor(j*(img_scale-.001)) )/255.0;
      img2(i,j) = (int)floor( min_color+img1(i,j)*(255.0-min_color)/256.0 );
      if( prob < prob_thresh ) img2(i,j) = 0;
    }
  }
  vil_save( img2, "D:/img.png" );
};


//--------------------------------------------------------------------
void add_haze()
{
  /*vcl_ofstream haze_file( "D:/images_multiview/baghdad/normseq_true_haze.txt" );
  vcl_string original_img_dir( "D:/images_multiview/baghdad/normseq_original" );
  vcl_string haze_img_dir( "D:/images_multiview/baghdad/normseq_haze" );
  vcl_string original_cam_file( "D:/images_multiview/baghdad/normseq_original_cameras.txt" );
  vcl_ofstream haze_cam_file( "D:/images_multiview/baghdad/normseq_haze_cameras.txt" );
*/
  vcl_ofstream haze_file( "D:/images_multiview/plasticville/sunseq/050_true_haze.txt" );
  vcl_string original_img_dir( "D:/images_multiview/plasticville/sunseq/050" );
  vcl_string haze_img_dir( "D:/images_multiview/plasticville/sunseq/050_haze" );
  vcl_string original_cam_file( "D:/images_multiview/plasticville/sunseq/050_cameras.txt" );
  //vcl_ofstream haze_cam_file( "D:/images_multiview/plasticville/sunseq/_haze_cameras.txt" );

  vcl_vector< vcl_string > imgs;
  vcl_vector< vpgl_proj_camera<double> > cams;
  load_view_files( original_img_dir, original_cam_file, "NONE", &imgs, &cams, NULL );

  for( int img = 0; img < imgs.size(); img++ ){

    vil_image_view<vxl_byte> original_img_color = vil_load( imgs[img].c_str() );
    vil_image_view<float> original_img(  original_img_color.ni(), original_img_color.nj() );
    vil_resample_bilin( original_img_color, original_img, 
      original_img_color.ni(), original_img_color.nj() );

    vcl_string namebase = vul_file::strip_extension( vul_file::strip_directory( imgs[img] ) );
    for( int h = 0; h < 1; h++ ){

      // Pick haze parameters
      float haze_color = 150;
      float haze_amount = 0.35;
      haze_file << 1.0/(1.0-haze_amount) << ' ' << -haze_amount*haze_color/(1-haze_amount) << '\n';

      // Get the new name.
      vcl_stringstream haze_img_name;
      haze_img_name << haze_img_dir; haze_img_name << '/' << namebase << '_' << h << ".png";

      // Haze the image.
      vil_image_view<vxl_byte> haze_img( original_img.ni(), original_img.nj(), 1 );
      for( int i = 0; i < original_img.ni(); i++ )
        for( int j = 0; j < original_img.nj(); j++ ){
          float original_pixel = 0;
          for( int k = 0; k < original_img.nplanes(); k++ ) 
            original_pixel += (original_img(i,j,k)/(float)original_img.nplanes());
          int haze_pixel = (int)floor( original_pixel*(1.0-haze_amount) + haze_amount*haze_color );
          if( haze_pixel < 0 ) haze_pixel = 0;
          if( haze_pixel > 255 ) haze_pixel = 255;
          haze_img(i,j) = haze_pixel;
        }
      vil_save( haze_img, haze_img_name.str().c_str() );

      //haze_cam_file << haze_img_name.str() << '\n' << cams[img].get_matrix() << '\n';
    }
  }
};


//---------------------------------------------------------------------
void contrast_exp()
{
  int num_bins = 20;
  vcl_ofstream ofs( "D:/0.txt" );
  vcl_string img_dir( "D:/images_multiview/baghdad/hiafa_ref" );
  for( vul_file_iterator f = (img_dir+"\\*").c_str(); f; ++f ){

    if( vul_file::extension( f() ) != ".png" ) continue;
    vil_image_view<vxl_byte> img = vil_load( f() );

    vnl_vector<double> int_bins(num_bins,0.0);
    for( int i = 0; i < img.ni(); i++ ){
      for( int j = 0; j < img.nj(); j++ ){
        int this_bin = (int)floor(num_bins*img(i,j)/256.0);
        int_bins(this_bin)+= 1.0;
      }
    }
    for( int i = 0; i < num_bins; i++ )
      ofs << int_bins(i)/(img.ni()*img.nj()) << '\t';

    double mean = 0.0;
    for( int i = 0; i < num_bins; i++ )
      mean += i*int_bins(i)/(img.ni()*img.nj());
    double var = 0.0;
    for( int i = 0; i < num_bins; i++ )
      var += i*i*int_bins(i)/(img.ni()*img.nj());
    var -= mean*mean;

    ofs << var << '\n';
  }
};


//--------------------------------------------------------------------
void prob_img_test()
{
  vcl_string pimg_file( "D:/33" );
  vcl_string draw_file( "D:/33.png" );
  baml_prob_img pimg;
  pimg.read( pimg_file );
  vcl_string img_file( "D:/images_multiview/prov_c/seq1/vlcsnap-33.png" );
  vil_image_view<vxl_byte> img_unscaled = vil_load( img_file.c_str() );
  vil_image_view<float> img;
  vil_resample_bicub( img_unscaled, img, 
    (int)floor( img_unscaled.ni()*.5 ), 
    (int)floor( img_unscaled.nj()*.5 ) );

  vcl_string excel_file( "D:/0.txt" );
  int ix = 512, iy = 273;
  vcl_cerr << img(ix,iy)/255.0 << '\n';
  pimg.write_pixel( ix, iy, excel_file );
  pimg.draw_best( draw_file );

  //float a = 1.0;
  float b = 0.0;
  for( float a = .5; a < 1.5; a+=.1 ){
    //for( float b = -40; b <= 40; b+= 20 ){
      vil_image_view<float> nimg( img.ni(), img.nj() );
      for( int i = 0; i < nimg.ni(); i++ ){
        for( int j = 0; j < nimg.nj(); j++ ){
          int new_pixel = (int)(b+a*img(i,j));
          if( new_pixel < 0 ) new_pixel = 0;
          if( new_pixel > 255 ) new_pixel = 255;
          nimg(i,j) = new_pixel;
        }
      }
      //vil_save( nimg, "D:/m.png" );
      vcl_cerr << pimg.prob( nimg ) << ' ';
    }
    vcl_cerr << '\n';
  //}

};


//--------------------------------------------------------------------
void shadow_thresh()
{
  vcl_string input_img = "D:/images_multiview/baghdad/hiafa_ref/02AUG18074303-P1BS-005630620010_01_P001_hiafa.png";
  vcl_string output_img = "D:/results/baghdad/shadow_thresh.png";
  double t1 = .2, t2 = .3;

  vil_image_view<vxl_byte> img = vil_load( input_img.c_str() );
  vil_image_view<vxl_byte> prob( img.ni(), img.nj(),3 );
  for( int i = 0; i < img.ni(); i++ ){
    for( int j = 0; j < img.nj(); j++ ){
      double p = img(i,j)/255.0;
      if( p < t1 ){ prob(i,j,0) = 255; prob(i,j,1) = prob(i,j,2) = 0; }
      else if( p > t2 ){ prob(i,j,0) = prob(i,j,1) = prob(i,j,2) = img(i,j); }
      else{
        prob(i,j,0) = (int)floor(((t2-p)/(t2-t1))*255);
        prob(i,j,1) = prob(i,j,2) = 0;
      }
    }
  }
  vil_save( prob, output_img.c_str() );
};


//--------------------------------------------------------------------
void intensity_ratios()
{
  vcl_string img1_file = "D:/images_multiview/plasticville/statseq/DSCN1135.JPG";
  vcl_string img2_file = "D:/images_multiview/plasticville/statseq/DSCN1160.JPG";
  vcl_string ratio_file = "D:/results/ratio.png";
  int max_ratio = 10;

  vil_image_view<vxl_byte> img1 = vil_load( img1_file.c_str() );
  vil_image_view<vxl_byte> img2 = vil_load( img2_file.c_str() );
  vil_image_view<vxl_byte> ratio_img( img1.ni(), img1.nj(), 3 );

  for( int i = 0; i < img1.ni(); i++ ){
    for( int j = 0; j < img1.nj(); j++ ){
      double pixel_ratio;
      double ratio_top = ( img1(i,j,0)+img1(i,j,1)+img1(i,j,2) );
      double ratio_bot = ( img2(i,j,0)+img2(i,j,1)+img2(i,j,2) );
      bool ratio_flip = false; 
      if( ratio_top < ratio_bot ){ ratio_flip = true; pixel_ratio = ratio_bot/ratio_top; }
      else pixel_ratio = ratio_top/ratio_bot;
      if( pixel_ratio > max_ratio ) pixel_ratio = max_ratio;
      int ratio_color = (int)floor( 255*pixel_ratio/max_ratio );
      ratio_img(i,j,0) = ratio_img(i,j,1) = ratio_img(i,j,2) = 0;
      if( ratio_flip ) ratio_img(i,j,0) = ratio_color;
      else ratio_img(i,j,2) = ratio_color;
    }
  }
  vil_save( ratio_img, ratio_file.c_str() );
};
/*
//------------------------------------------------------------------
void inspect_baghdad()
{
  vcl_string img_dir = "D:/images_multiview/baghdad/hiafa";
  vcl_string cam_file = "D:/images_multiview/baghdad/hiafa_cameras.txt";
  vcl_string lighting_file = "D:/images_multiview/baghdad/hiafa_lights.txt";
  vcl_string x3d_file = "D:/results/baghdad/insp.x3d";
  vcl_string ray_check_img = "D:/results/baghdad/ray_check.png";
  vgl_point_2d<double> inspect_pixel( 839, 687 );
  double bot_elv = 0, top_elv = 200;
  int num_insp = 50;

  // Set up an occluding polygon.
  vcl_vector< vgl_point_3d<double> > occluding_polygon;
  occluding_polygon.push_back( vgl_point_3d<double>( 44.38218061, 33.33570869, 92 ) );
  occluding_polygon.push_back( vgl_point_3d<double>( 44.38239647, 33.33557447, 92 ) );
  occluding_polygon.push_back( vgl_point_3d<double>( 44.38258841, 33.3357829, 92 ) );
  occluding_polygon.push_back( vgl_point_3d<double>( 44.38237255, 33.33591711, 92 ) );
  btpl_satellite_tools baghdad;
  baghdad.set_scene( "hiafa" );
  for( int i = 0; i < occluding_polygon.size(); i++ )
    occluding_polygon[i] = baghdad.transform_wp( occluding_polygon[i] );
  baghdad.scene_bounds();
  
  vcl_vector< vcl_string > imgs;
  vcl_vector< vpgl_proj_camera<double> > cams;
  vcl_vector< vgl_vector_3d<double> > lights;
  load_view_files( img_dir, cam_file, lighting_file, &imgs, &cams, &lights );

  // Get voxels along the ray.
  vgl_homg_line_3d_2_points<double> inspect_ray = cams[0].backproject( 
    vgl_homg_point_2d<double>( inspect_pixel.x(), inspect_pixel.y() ) );
  vgl_homg_point_3d<double> p1 = inspect_ray.point_finite(); p1.rescale_w(1.0);
  vgl_homg_point_3d<double> p2 = inspect_ray.point_infinite();
  double ttop = (top_elv-p1.z())/p2.z(), tbot = (bot_elv-p1.z())/p2.z();
  vgl_point_3d<double> inspect_top( p1.x()+ttop*p2.x(), p1.y()+ttop*p2.y(), top_elv );
  vgl_point_3d<double> inspect_bot( p1.x()+tbot*p2.x(), p1.y()+tbot*p2.y(), bot_elv );
  vgl_vector_3d<double> inspect_dir = inspect_bot-inspect_top;

  // Get observations of each voxel on the ray.
  vcl_vector< vcl_vector<double> > obs;
  for( unsigned img = 0; img < imgs.size(); img++ ){
    vcl_cerr << imgs[img] << '\n';
    vil_image_view<vxl_byte> img_view = vil_load( imgs[img].c_str() );
    vil_image_view<vxl_byte> ray_check( img_view.ni(), img_view.nj(), 3 );
    for( int i = 0; i < img_view.ni(); i++ )
      for( int j = 0; j < img_view.nj(); j++ )
        ray_check(i,j,0) = ray_check(i,j,1) = ray_check(i,j,2) = img_view(i,j);

    // Get the occluding polygon in this image.
    vcl_vector< vgl_point_2d<double> > img_polygon_pts;
    for( int i = 0; i < occluding_polygon.size(); i++ ){
      vgl_homg_point_2d<double> new_ip = cams[img].project( 
        vgl_homg_point_3d<double>( occluding_polygon[i] ) );
      img_polygon_pts.push_back( vgl_point_2d<double>( 
        new_ip.x()/new_ip.w(), new_ip.y()/new_ip.w() ) );
    }
    vgl_polygon<double> img_polygon( img_polygon_pts );

    vcl_vector<double> img_obs;
    for( unsigned n = 0; n < num_insp; n++ ){
      vgl_homg_point_3d<double> v( inspect_bot + (n/(double)num_insp)*inspect_dir );
      vgl_homg_point_2d<double> ph = cams[img].project( v );
      vgl_point_2d<int> p( (int)floor(ph.x()/ph.w()), (int)floor(ph.y()/ph.w()) );
      if( p.x() < 0 || p.y() < 0 || p.x() >= img_view.ni() || p.y() >= img_view.nj() ||
        img_polygon.contains( p.x(), p.y() ) ){
        img_obs.push_back( 0.0 ); continue;
      }
      else
        img_obs.push_back( (double)img_view( (unsigned)p.x(), (unsigned)p.y() )/255.0 );
      if( n == 0 ){
        ray_check( (unsigned)p.x(), (unsigned)p.y(), 1 ) = 255;
        ray_check( (unsigned)p.x(), (unsigned)p.y(), 0 ) = 
          ray_check( (unsigned)p.x(), (unsigned)p.y(), 2 ) = 0;
      }
      else {
        ray_check( (unsigned)p.x(), (unsigned)p.y(), 0 ) = 255;
        ray_check( (unsigned)p.x(), (unsigned)p.y(), 1 ) = 
        ray_check( (unsigned)p.x(), (unsigned)p.y(), 2 ) = 0;
      }
    }
    obs.push_back( img_obs );
    vil_save( ray_check, ray_check_img.c_str() );
  }

  write_x3d_insp_file( obs, lights, x3d_file );
};

*/

/*
 * Synthetic brdf experiments
 */

void syn_brdf_exp1()
{ /*
  int num_training_points = 100;
  float prob_thresh = 1.5;

  vil_image_view<vxl_byte> img1 = vil_load( "D:\\images\\brdf2.png" );
  vil_image_view<vxl_byte> img2 = vil_load( "D:\\images\\brdf2.png" );
  vcl_string output_file( "D:\\results\\baml_exp.png" );
  vil_image_view<vxl_byte> prob_img( img1.ni(), img1.nj(), 3 );
  int img_size = 200;

  // Train on some sample points
  baml_rbf rbf;
  vcl_vector< vnl_vector<int> > training_points, unused_points;
  vcl_vector<float> training_vals;
  for( int i = 0; i < num_training_points; i++ ){
    vnl_vector<int> training_point(2); 
    training_point(0) = (int)floor(img_size*rand()/(float)RAND_MAX); 
    training_point(1) = (int)floor(img_size*rand()/(float)RAND_MAX);
    float training_val = img1( training_point(0), training_point(1) )/255.0;
    vnl_vector<float> scaled_training_point(2); 
    scaled_training_point(0) = training_point(0)/(float)img_size;
    scaled_training_point(1) = training_point(1)/(float)img_size;

    bool used_point = rbf.update( training_val, scaled_training_point );
    if( !used_point )
      unused_points.push_back( training_point );
    training_points.push_back( training_point );
    training_vals.push_back( training_val );
  }

  // Get the probability of every other point in the image.
  for( int i = 0; i < img1.ni(); i++ ){
    for( int j = 0; j < img1.nj(); j++ ){
      vnl_vector<float> this_light(2);
      this_light(0) = i/(float)img_size; this_light(1) = j/(float)img_size;
      float prob = rbf.prob( img2(i,j)/255.0, this_light );
      prob = 255*prob/8.0; if( prob > 255 ) prob = 255;
      //float prob = 255*rbf.best_color( this_light );
      prob_img(i,j,0) = prob_img(i,j,1) = prob_img(i,j,2) = (int)floor(prob);
    }
  }

  for( int i = 0; i < training_points.size(); i++ ){
    prob_img(training_points[i](0),training_points[i](1),0) = 255;
    prob_img(training_points[i](0),training_points[i](1),1) =
      prob_img(training_points[i](0),training_points[i](1),2) = 0;
  }
  for( int i = 0; i < unused_points.size(); i++ ){
    prob_img(unused_points[i](0),unused_points[i](1),1) = 255;
    prob_img(unused_points[i](0),unused_points[i](1),0) =
      prob_img(unused_points[i](0),unused_points[i](1),2) = 0;
  }
  vil_save( prob_img, output_file.c_str() ); */
};


/*
 * Lighted plasticville sequence experiments
 */
void plasticville_exp1()
{
  vgl_point_2d<int> inspect_pixel( 655, 655 );
  int num_inc = 50;
  vcl_string img_dir( "D:\\images_multiview\\plasticville\\lightseq" );
  vcl_string light_file( "D:\\images_multiview\\plasticville\\lightseq_lights.txt" );
  vcl_string camera_file( "D:\\images_multiview\\plasticville\\lightseq_cameras.txt" );
  vcl_string insp_x3d( "D:\\results\\pville_inspect.x3d" );
  vcl_string insp_img( "D:\\results\\pville_inspect.png" );

  // Load images.
  vcl_vector< vcl_string > imgs;
  for( vul_file_iterator f= (img_dir+"\\*").c_str(); f; ++f ){
    vcl_string current_file( f() );
    if( current_file[current_file.size()-1] == 'G' )
      imgs.push_back( current_file );
  }
  int num_imgs = imgs.size();

  // Load lights.
  vcl_vector< vgl_point_2d<double> > lights;
  vcl_ifstream light_stream( light_file.c_str() );
  vul_awk light_awk( light_stream );
  while( light_awk ){
    if( light_awk.NF() != 2 ){ ++light_awk; continue; }
    lights.push_back( vgl_point_2d<double>( atof(light_awk[0])/360.0, atof(light_awk[1]) ) );
    ++light_awk;
  }
  int num_lights = lights.size();

  // Load cameras.
  vcl_vector< vpgl_proj_camera<double> > cams;
  cams.clear();
  vcl_ifstream camera_stream( camera_file.c_str() );
  char line_buffer[256];
  if( !(camera_stream.good()) )
    return;
  while( camera_stream.eof() == 0 ){
    char nc = camera_stream.peek();
    if( nc == '-' || nc == '0' || nc == '1' || nc == '2' || nc == '3' || nc == '4' || 
        nc == '5' || nc == '6' || nc == '7' || nc == '8' || nc == '9' ){
      vnl_matrix_fixed<double,3,4> new_camera_matrix;
      camera_stream >> new_camera_matrix;
      cams.push_back( vpgl_proj_camera<double>( new_camera_matrix)  );
    }
    else
      camera_stream.getline(line_buffer,256);
  }
  int num_cameras = cams.size();

  // Get the voxels to inspect.
  vgl_homg_line_3d_2_points<double> inspect_ray = cams[0].backproject( 
    vgl_homg_point_2d<double>( inspect_pixel.x(), inspect_pixel.y() ) );
  vgl_homg_point_3d<double> p1 = inspect_ray.point_finite(); p1.rescale_w(1.0);
  vgl_homg_point_3d<double> p2 = inspect_ray.point_infinite();
  double ttop = (20-p1.z())/p2.z(), tbot = (0-p1.z())/p2.z();
  vgl_homg_point_3d<double> inspect_top( p1.x()+ttop*p2.x(), p1.y()+ttop*p2.y(), 20 );
  vgl_homg_point_3d<double> inspect_bot( p1.x()+tbot*p2.x(), p1.y()+tbot*p2.y(), 0 );
  vgl_vector_3d<double> inspect_inc = inspect_bot-inspect_top;
  inspect_inc /=(num_inc-1.0);

  // Write the x3d file.
  vcl_ofstream ofs( insp_x3d.c_str() );
  ofs << "<X3D version='3.0' profile='Immersive'>\n\n"
    << "<Scene>\n"
    << "<Background skyColor='0 0 1'/>\n\n";

  for( int img = 0; img < num_imgs; img++ ){
    vil_image_view<vxl_byte> this_img = vil_load( imgs[img].c_str() );
    vgl_homg_point_3d<double> inspect_point = inspect_top;
    for( int inc = 0; inc < num_inc; inc++ ){
      vgl_homg_point_2d<double> img_point = cams[img].project( inspect_point );
      vgl_point_2d<int> ip( (int)floor( img_point.x()/img_point.w() ), 
        (int)floor( img_point.y()/img_point.w() ) );
      float this_color = ( this_img(ip.x(),ip.y(),0) + this_img(ip.x(),ip.y(),1) +
        this_img(ip.x(),ip.y(),2) )/(3.0*255);

      ofs << "<Transform translation='" << 1.5*inc+lights[img].x() << ' ' 
          << lights[img].y() << ' ' << this_color/2.0 << "'>\n" 
        << " <Shape>\n"
        << "  <Appearance>\n"
        << "   <Material diffuseColor='1 0 0'/>\n"
        << "  </Appearance>\n"
        << "  <Box size='.05 .05 " << this_color << "'/>"
        << " </Shape>\n"
        << "</Transform>\n\n";
      inspect_point += inspect_inc;
    }
  }
      
  ofs << "</Scene>\n"
    << "</X3D>";

  // Record all observations of these voxels.
  vil_image_view<vxl_byte> I( num_inc, num_imgs, 3 );
  for( int img = 0; img < num_imgs; img++ ){
    vil_image_view<vxl_byte> this_img = vil_load( imgs[img].c_str() );
    vgl_homg_point_3d<double> inspect_point = inspect_top;
    for( int inc = 0; inc < num_inc; inc++ ){
      vgl_homg_point_2d<double> img_point = cams[img].project( inspect_point );
      vgl_point_2d<int> ip( (int)floor( img_point.x()/img_point.w() ), 
        (int)floor( img_point.y()/img_point.w() ) );
      int grey_val = 0;
      for( int c = 0; c < 3; c++ )
        grey_val += this_img( ip.x(), ip.y(), c );
        //I( inc, img, c ) = this_img( ip.x(), ip.y(), c );
      for( int c = 0; c < 3; c++ )
        I( inc, img, c ) = (int)floor(grey_val/3.0);
      inspect_point += inspect_inc;
    }
  }
  vil_save( I, insp_img.c_str() );

};


/*
 * Time Square experiment
 */
vgl_point_2d<int> centered_pixel_coord(
  const vgl_point_2d<int>& p,
  float time )
{
  vgl_vector_2d<int> offset( 0, 0 );//-14 );
  if( time >= 10+54.0/60 && time <= 15+11.0/60 )
    return p+offset;
  return p;
};


/*
 * Beach experiments
 */
void beach_exp1()
{  
  vcl_string img_dir = "C:\\beach_6_12";
  vcl_string out_file = "C:\\out2.txt";

  vgl_point_2d<int> sample1( 27, 127 );
  vcl_vector<float> sample1_obs;

  for( vul_file_iterator f = (img_dir+"\\*").c_str(); f; ++f ){

    // Get the time from the current file.
    vcl_string current_file( f() );
    if( current_file[current_file.size()-1] != 'g' ) continue;
    int last_slash = 0;
    for( int i = 0; i < current_file.size(); i++ )
      if( current_file[i] == '\\' ) last_slash = i;
    vcl_stringstream hour_string, minute_string;
    hour_string << current_file[last_slash+1] << current_file[last_slash+2];
    minute_string << current_file[last_slash+3] << current_file[last_slash+4];
    float time = atoi( hour_string.str().c_str() ) +
      atoi( minute_string.str().c_str() )/60.0;

    // Record the sample pixels.
    vil_image_view<vxl_byte> this_img = vil_load( current_file.c_str() );
    vgl_point_2d<int> cp = centered_pixel_coord( sample1, time );
    float nbhd_sum = 0;
    for( int i = -2; i <= 2; i++ )
      for( int j = -2; j <= 2; j++ )
        nbhd_sum += this_img( cp.x()+i, cp.y()+j, 0 ) +
        this_img( cp.x()+i, cp.y()+j, 1 ) + this_img( cp.x()+i, cp.y()+j, 2 );
    sample1_obs.push_back( nbhd_sum/75.0 );
  }


  // Print observations to file.
  vcl_ofstream ofs( out_file.c_str() );
  for( int i =0 ; i < sample1_obs.size(); i++ ){
    ofs << sample1_obs[i] << '\n';
  }

};

void beach_exp2()
{ /*
  vcl_string residual_img_name( "c:\\res.png" );
  vcl_string img_dir = "E:\\images\\beach_6_12";
  float percent_training = .2;

  // Get the light directions for the outside data set.
  vcl_vector<float> alt, azim;
  vcl_vector< vgl_vector_3d<float> > light_dirs;
  alt.push_back( 48.5 ); azim.push_back( 104.2 );
  alt.push_back( 49.4 ); azim.push_back(        105.3);
  alt.push_back( 50.3 ); azim.push_back(        106.4);
  alt.push_back( 51.1 ); azim.push_back(        107.6);
  alt.push_back( 52.0 ); azim.push_back(        108.8);
  alt.push_back( 52.9 ); azim.push_back(        110.1);
  alt.push_back( 53.7 ); azim.push_back(        111.4);
  alt.push_back( 54.6 ); azim.push_back(        112.7);
  alt.push_back( 55.4 ); azim.push_back(        114.1);
  alt.push_back( 56.2 ); azim.push_back(        115.5);
  alt.push_back( 57.0 ); azim.push_back(        116.9);
  alt.push_back( 57.9 ); azim.push_back(        118.5);
  alt.push_back( 58.7 ); azim.push_back(        120.0);
  alt.push_back( 59.4 ); azim.push_back(        121.7);
  alt.push_back( 60.2 ); azim.push_back(        123.4);
  alt.push_back( 61.0 ); azim.push_back(        125.1);
  alt.push_back( 61.7 ); azim.push_back(       127.0);
  alt.push_back( 62.4 ); azim.push_back(        128.9);
  alt.push_back( 63.1 ); azim.push_back(       130.9);
  alt.push_back( 63.8 ); azim.push_back(        132.9);
  alt.push_back( 64.5 ); azim.push_back(        135.1);
  alt.push_back( 65.1 ); azim.push_back(        137.3);
  alt.push_back( 65.7 ); azim.push_back(        139.7);
  alt.push_back( 66.3 ); azim.push_back(        142.1);
  alt.push_back( 66.8 ); azim.push_back(        144.7);
  alt.push_back( 67.3 ); azim.push_back(        147.3);
  alt.push_back( 67.8 ); azim.push_back(        150.1);
  alt.push_back( 68.2 ); azim.push_back(       152.9);
  alt.push_back( 68.6 ); azim.push_back(       155.8);
  alt.push_back( 69.0 ); azim.push_back(        158.9);
  alt.push_back( 69.3 ); azim.push_back(        162.0);
  alt.push_back( 69.5 ); azim.push_back(        165.1);
  alt.push_back( 69.8 ); azim.push_back(        168.4);
  alt.push_back( 69.9 ); azim.push_back(        171.7);
  alt.push_back( 70.0 ); azim.push_back(        175.0);
  alt.push_back( 70.1 ); azim.push_back(        178.4);
  alt.push_back( 70.1 ); azim.push_back(        181.8);
  alt.push_back( 70.0 ); azim.push_back(        185.1);
  alt.push_back( 69.9 ); azim.push_back(        188.5);
  alt.push_back( 69.7 ); azim.push_back(       191.8);
  alt.push_back( 69.5 ); azim.push_back(       195.0);
  alt.push_back( 69.3 ); azim.push_back(       198.2);
  alt.push_back( 69.0 ); azim.push_back(       201.3);
  alt.push_back( 68.6 ); azim.push_back(       204.3);
  alt.push_back( 68.2 ); azim.push_back(       207.3);
  alt.push_back( 67.8 ); azim.push_back(       210.1);
  alt.push_back( 67.3 ); azim.push_back(       212.8);
  alt.push_back( 66.8 ); azim.push_back(       215.5);
  alt.push_back( 66.2 ); azim.push_back(       218.0);
  alt.push_back( 65.7 ); azim.push_back(       220.4);
  alt.push_back( 65.1 ); azim.push_back(       222.8);
  alt.push_back( 64.4 ); azim.push_back(       225.0);
  alt.push_back( 63.8 ); azim.push_back(       227.2);
  alt.push_back( 63.1 ); azim.push_back(       229.3);
  alt.push_back( 62.4 ); azim.push_back(       231.2);
  alt.push_back( 61.7 ); azim.push_back(       233.1);
  alt.push_back( 60.9 ); azim.push_back(       235.0);
  alt.push_back( 60.2 ); azim.push_back(       236.7);
  alt.push_back( 59.4 ); azim.push_back(       238.4);
  alt.push_back( 58.6 ); azim.push_back(       240.1);
  alt.push_back( 57.8 ); azim.push_back(       241.6);
  alt.push_back( 57.0 ); azim.push_back(       243.2);
  alt.push_back( 56.2 ); azim.push_back(       244.6);
  alt.push_back( 55.4 ); azim.push_back(       246.0);
  alt.push_back( 54.5 ); azim.push_back(       247.4);
  alt.push_back( 53.7 ); azim.push_back(       248.7);
  alt.push_back( 52.8 ); azim.push_back(       250.0);
  alt.push_back( 52.0 ); azim.push_back(       251.3);
  alt.push_back( 51.1 ); azim.push_back(       252.5);
  alt.push_back( 50.2 ); azim.push_back(       253.6);
  alt.push_back( 49.4 ); azim.push_back(       254.8);
  alt.push_back( 48.5 ); azim.push_back(       255.9);
  alt.push_back( 47.6 ); azim.push_back(       257.0);
  alt.push_back( 46.7 ); azim.push_back(      258.1);
  alt.push_back( 45.8 ); azim.push_back(       259.1);
  alt.push_back( 44.9 ); azim.push_back(       260.1);
  alt.push_back( 44.0 ); azim.push_back(       261.1);
  alt.push_back( 43.1 ); azim.push_back(       262.1);
  alt.push_back( 42.2 ); azim.push_back(       263.1);
  alt.push_back( 41.3 ); azim.push_back(       264.0);
  alt.push_back( 40.4 ); azim.push_back(       265.0);
  alt.push_back( 39.5 ); azim.push_back(       265.9);
  alt.push_back( 38.6 ); azim.push_back(       266.8);
  alt.push_back( 37.7 ); azim.push_back(       267.7);
  alt.push_back( 36.7 ); azim.push_back(       268.5);
  alt.push_back( 35.8 ); azim.push_back(       269.4);
  alt.push_back( 34.9 ); azim.push_back(       270.3);
  alt.push_back( 34.0 ); azim.push_back(       271.1);
  alt.push_back( 33.1 ); azim.push_back(       271.9);
  alt.push_back( 32.2 ); azim.push_back(       272.8);
  alt.push_back( 31.3 ); azim.push_back(       273.6);
  alt.push_back( 30.4 ); azim.push_back(       274.4);
  alt.push_back( 29.5 ); azim.push_back(       275.2);
  alt.push_back( 28.5 ); azim.push_back(       276.0);
  alt.push_back( 27.6 ); azim.push_back(       276.8);
  alt.push_back( 26.7 ); azim.push_back(       277.6);
  alt.push_back( 25.8 ); azim.push_back(       278.4);
  alt.push_back( 24.9 ); azim.push_back(       279.2);
  alt.push_back( 24.0 ); azim.push_back(       280.0);
  alt.push_back( 23.1 ); azim.push_back(       280.8);
  alt.push_back( 22.2 ); azim.push_back(       281.5);
  alt.push_back( 21.4 ); azim.push_back(       282.3);
  alt.push_back( 20.5 ); azim.push_back(      283.1);
  alt.push_back( 19.6 ); azim.push_back(       283.9);
  alt.push_back( 18.7 ); azim.push_back(       284.7);
  alt.push_back( 17.8 ); azim.push_back(       285.4);
  alt.push_back( 16.9 ); azim.push_back(       286.2);
  alt.push_back( 16.1 ); azim.push_back(       287.0);
  for( int i = 0; i < alt.size(); i++ ){
    alt[i] *= 3.1417/180; azim[i] *= 3.1417/180;
    light_dirs.push_back( vgl_vector_3d<float>(
      cos( alt[i] )*sin( azim[i] ), cos( alt[i] )*cos( azim[i] ), sin( alt[i] ) ) );
  }

  // Load all outside images.
  vcl_vector< vil_image_view<vxl_byte> > imgs;
  for( vul_file_iterator f = (img_dir+"\\*").c_str(); f; ++f ){
    vcl_string this_file = f();
    if( this_file[ this_file.size()-1 ] != 'g' ) continue;
    imgs.push_back( vil_load( this_file.c_str() ) );
  }
  int num_imgs = imgs.size();
  int ni = imgs[0].ni(), nj = imgs[0].nj();

  // Divide the images into training images and test images.
  vcl_vector<bool> training_images;
  for( int i = 0; i < imgs.size(); i++ ){
    if( rand()/(float)RAND_MAX < percent_training ) training_images.push_back( true );
    else training_images.push_back( false );
  }

  vil_image_view<vxl_byte> prob_img( ni, nj );
  for( int i = 0; i < ni; i++ ){
    for( int j = 0; j < nj; j++ ){
      
      // Train on some of the images.
      baml_rbf appearance_model;
      for( int img = 0; img < num_imgs; img++ ){
        if( !training_images[img] ) continue;
        //appearance_model.update( imgs[img](i,j)/256.0, light_dirs[img] );
      }
  
      // Get the probability for the rest of the images.
      float total_prob = 0;
      int total_prob_counter = 0;
      for( int img = 0; img < num_imgs; img++ ){
        if( training_images[img] ) continue;
        //total_prob += appearance_model.prob( imgs[img](i,j)/256.0, light_dirs[img] );
        total_prob_counter++;
      }
      prob_img(i,j) = (int)floor( 255*total_prob/total_prob_counter );
    }
  }
  vil_save( prob_img, residual_img_name.c_str() );
};

float estimate_surface_params(
  const vcl_vector<float>& alt,
  const vcl_vector<float>& az,
  const vcl_vector<float>& intensity,
  vnl_vector<float>& params,
  vcl_string save_file = "NONE" )
{
  int num_obs = intensity.size();
  vnl_matrix<float> A( num_obs, 4 );
  vnl_vector<float> b( num_obs );
  for( int i = 0; i < num_obs; i++ ){
    b(i) = intensity[i];
    vgl_vector_3d<float> light_dir(
      cos( alt[i] )*sin( az[i] ), cos( alt[i] )*cos( az[i] ), sin( alt[i] ) );
    float amb_coeff = fabs( alt[i]/90 );
    A(i,0) = light_dir.x(); A(i,1) = light_dir.y(); A(i,2) = light_dir.z();
    A(i,3) = amb_coeff;
  }
  vnl_svd<float> A_svd( A );
  params = A_svd.solve( b );

  vnl_vector<float> b2 = A*params;

  if( save_file != "NONE" ){
    vcl_ofstream ofs( save_file.c_str() );
    for( int i = 0; i < num_obs; i++ )
      ofs << alt[i] << '\t' << b(i) << '\t' << b2(i) << '\n';
  }

  float residual = 0.0;
  for( int i =0 ; i < num_obs; i++ )
    residual += fabs( b(i)-b2(i) );
  return residual/num_obs; */
};


/*
 * Code for change detection visualization
 */
void change_render(){

  float dark_factor = .5;
  vcl_string old_img_dir = "D:\\images_multiview\\prov_c\\seq1";
  vcl_string masks_dir = "D:\\results\\prov_c_large\\red";
  vcl_string output_dir = "D:\\results\\prov_c_large\\final";

  // Get the image names
  vcl_vector< vcl_string > old_imgs, masks;
  old_img_dir += "/*.*";
  for( vul_file_iterator fit = old_img_dir; fit; ++fit ){
    if( vul_file::is_directory(fit()) )
      continue;
    vcl_string image_name = fit();
    if( image_name.find( ".jpg" ) > 1000 &&
        image_name.find( ".png" ) > 1000 &&
        image_name.find( ".tif" ) > 1000 ){
      continue;
    }
    old_imgs.push_back( image_name );
  }
  masks_dir += "/*.*";
  for( vul_file_iterator fit = masks_dir; fit; ++fit ){
    if( vul_file::is_directory(fit()) )
      continue;
    vcl_string image_name = fit();
    if( image_name.find( ".jpg" ) > 1000 &&
        image_name.find( ".png" ) > 1000 &&
        image_name.find( ".tif" ) > 1000 ){
      continue;
    }
    masks.push_back( image_name );
  }

  // Process all the images.
  for( int img = 0; img < old_imgs.size(); img++ ){
    vcl_cerr << "Processing image: " << old_imgs[img] << '\n';
    vil_image_view<vxl_byte> new_img = vil_load( old_imgs[img].c_str() );
    vil_image_view<vxl_byte> mask_img = vil_load( masks[img].c_str() );
    
    for( int i = 0; i < new_img.ni(); i++ ){
      for( int j = 0; j < new_img.nj(); j++ ){
        if( mask_img(i,j,0) == 0 && mask_img(i,j,1) == 255 && mask_img(i,j,2) == 0 ){
          new_img(i,j,0) = (int)floor( dark_factor*new_img(i,j,0) );
          new_img(i,j,1) = (int)floor( dark_factor*new_img(i,j,1) );
          new_img(i,j,2) = (int)floor( dark_factor*new_img(i,j,2) );
        }
        else{
          new_img(i,j,0) = mask_img(i,j,0);
          new_img(i,j,1) = mask_img(i,j,1);
          new_img(i,j,2) = mask_img(i,j,2);
        }
      }
    }

    vcl_stringstream new_img_name;
    new_img_name << output_dir << "\\";
    if( img < 10 ) new_img_name << '0';
    new_img_name << img << ".png";
    vil_save( new_img, new_img_name.str().c_str() );
  }
}

void registration_render(){

  vcl_string registration_dir = "D:\\results\\prov_c_reg";
  vcl_string mask_file = "D:\\results\\prov_c_reg_mask.png";
  vcl_string background_file = "D:\\images_multiview\\prov_c\\seq2\\0919.png";
  vcl_string output_dir = "D:\\results\\prov_c_reg_final";
  float background_scale = .5;
  float dark_factor = .5;

  // Get the image names
  vcl_vector< vcl_string > reg_imgs;
  registration_dir += "/*.*";
  for( vul_file_iterator fit = registration_dir; fit; ++fit ){
    if( vul_file::is_directory(fit()) )
      continue;
    vcl_string image_name = fit();
    if( image_name.find( ".jpg" ) > 1000 &&
        image_name.find( ".png" ) > 1000 &&
        image_name.find( ".tif" ) > 1000 ){
      continue;
    }
    reg_imgs.push_back( image_name );
  }
  vil_image_view<vxl_byte> mask_img = vil_load( mask_file.c_str() );
  vil_image_view<float> background_img;
  vil_image_view<vxl_byte> background_img_unscaled = vil_load( background_file.c_str() );
  vil_resample_bicub( background_img_unscaled, background_img, 
    (int)floor( background_img_unscaled.ni()*background_scale ), 
    (int)floor( background_img_unscaled.nj()*background_scale ) );

  // Process all the images.
  for( int img = 0; img < reg_imgs.size(); img++ ){
    vcl_cerr << "Processing image: " << reg_imgs[img] << '\n';
    vil_image_view<vxl_byte> new_img = vil_load( reg_imgs[img].c_str() );
    
    for( int i = 0; i < new_img.ni(); i++ ){
      for( int j = 0; j < new_img.nj(); j++ ){
        if( mask_img(i,j,0) == 255 ){
          int pixel_color = (int)floor( .3333*dark_factor*
            ( background_img(i,j,0)+background_img(i,j,1)+background_img(i,j,2) ) );
          new_img(i,j,0) = pixel_color;
        }
      }
    }
    vcl_string namebase = vul_file::strip_extension(vul_file::basename( reg_imgs[img] ));
    vcl_stringstream new_img_name;
    new_img_name << output_dir << "\\" << namebase << "_reg.png";
    vil_save( new_img, new_img_name.str().c_str() );
  }
}


/*
 * Get an overhead camera for registration.
 */
void overhead_cam()
{
  vnl_matrix_fixed<double,3,4> P1m;
  P1m(0,0)=-3.10791; P1m(0,1)=2.56872; P1m(0,2)=-0.519425; P1m(0,3)=499.811;
  P1m(1,0)=1.50367; P1m(1,1)=1.34889; P1m(1,2)=-3.31751; P1m(1,3)=390.049;
  P1m(2,0)=-0.00113663; P1m(2,1)=-0.000896845; P1m(2,2)=-0.00111493; P1m(2,3)=1;
  vpgl_perspective_camera<double> P1;
  vpgl_perspective_decomposition( P1m, P1 );

  vgl_point_3d<double> c = P1.get_camera_center();
  c.set( 30, 20, 1.2*c.z() );
  P1.set_camera_center( c );
  P1.look_at( vgl_homg_point_3d<double>( 30, 20, 0 ),
    vgl_vector_3d<double>(0,1,0) );
  vcl_cerr << P1.get_matrix() << '\n';
  //vcl_ofstream ofs( "D:\\images_multiview\\prov_c\\overhead_cam.txt" );
  //ofs << "FRAME 0\n" << P1.get_matrix() << '\n';
  for( int i = 0; i < 4; i++ ){
    vgl_homg_point_3d<double> p;
    if( i == 0 ) p.set( 0,0,15 );
    if( i == 1 ) p.set( 60,0,15 );
    if( i == 2 ) p.set( 0,40,15 );
    if( i == 3 ) p.set( 60,40,15 );
    vgl_homg_point_2d<double> pi = P1.project( p );
    vcl_cerr << pi.x()/pi.w() << ' ' << pi.y()/pi.w() << '\n';
  }
}


//-----------------------------------------------------------
// HELPER FUNCTIONS:-----------------------------------------
//-----------------------------------------------------------

//-----------------------------------------------------------
void load_view_files(
  vcl_string img_dir,
  vcl_string camera_file,
  vcl_string lighting_file,
  vcl_vector< vcl_string >* imgs,
  vcl_vector< vpgl_proj_camera<double> >* cams,
  vcl_vector< vgl_vector_3d<double> >* lights )
{
  // Load images.
  if( imgs != NULL ){
    imgs->clear();
    for( vul_file_iterator f= (img_dir+"\\*").c_str(); f; ++f ){
      vcl_string current_file( f() );
      vcl_string ext = vul_file::extension( current_file );
      if( ext == ".tif" || ext == ".png" || ext == ".jpg" || ext == ".JPG" )
        imgs->push_back( current_file );
    }
  }

  // Load cameras.
  if( cams != NULL ){
    cams->clear();
    vcl_ifstream camera_stream( camera_file.c_str() );
    char line_buffer[256];
    if( !(camera_stream.good()) )
      vcl_cerr << "\n\nBAD CAMERA FILE\n\n";
    while( camera_stream.eof() == 0 ){
      char nc = camera_stream.peek();
      if( nc == '-' || nc == '0' || nc == '1' || nc == '2' || nc == '3' || nc == '4' || 
          nc == '5' || nc == '6' || nc == '7' || nc == '8' || nc == '9' ){
        vnl_matrix_fixed<double,3,4> new_camera_matrix;
        camera_stream >> new_camera_matrix;
        cams->push_back( vpgl_proj_camera<double>( new_camera_matrix ) );
      }
      else
        camera_stream.getline(line_buffer,256);
    }
  }

  // Load lights.
  if( lights != NULL ){
    lights->clear();
    vcl_ifstream light_stream( lighting_file.c_str() );
    vul_awk light_awk( light_stream );
    while( light_awk ){
      if( light_awk.NF() == 0 ){ ++light_awk; continue; }
      double lx = 0, ly = 0, lz = 0;
      if( light_awk.NF() > 0 ) lx = (double)atof(light_awk[0]);
      if( light_awk.NF() > 1 ) ly = (double)atof(light_awk[1]);
      if( light_awk.NF() > 2 ) lz = (double)atof(light_awk[2]);
      lights->push_back( vgl_vector_3d<double>( lx, ly, lz ) );
      ++light_awk;
    }
  }
  
};


//----------------------------------------------------------
void write_x3d_insp_file(
  const vcl_vector< vcl_vector<double> >& obs,
  const vcl_vector< vgl_vector_3d<double> >& lights,
  vcl_string file )
{
  vcl_ofstream ofs( file.c_str() );
  ofs << "<X3D version='3.0' profile='Immersive'>\n\n"
    << "<Scene>\n"
    << "<Background skyColor='0 0 1'/>\n\n";

  for( unsigned img = 0; img < obs.size(); img++ ){
    for( unsigned v = 0; v < obs[img].size(); v++ ){
      ofs << "<Transform translation='" << 2.5*v+lights[img].x() << ' ' 
          << lights[img].y() << ' ' << obs[img][v]/2.0 << "'>\n" 
        << " <Shape>\n"
        << "  <Appearance>\n";
      if( v == 0 )
        ofs << "   <Material diffuseColor='0 1 0'/>\n";
      else
        ofs << "   <Material diffuseColor='1 0 0'/>\n";
      ofs << "  </Appearance>\n"
        << "  <Box size='.05 .05 " << obs[img][v] << "'/>"
        << " </Shape>\n"
        << "</Transform>\n\n";
    }
  }
  ofs << "</Scene>\n"
    << "</X3D>";
};

