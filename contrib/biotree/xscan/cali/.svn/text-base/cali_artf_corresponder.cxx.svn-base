#include <cali/cali_artf_corresponder.h>
#include <cali/cali_calibrator.h>
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_fit_conics_params.h>
#include <sdet/sdet_fit_conics.h>
#include <sdet/sdet_detector.h>
#include <vsol/vsol_conic_2d_sptr.h>
#include <vsol/vsol_conic_2d.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>
#include <vul/vul_file.h>
#include <vnl/vnl_math.h>
#include <vsl/vsl_binary_io.h>
#include <cali/cali_conic_info.h>
#include <vcl_cstddef.h>
#include <vcl_string.h>
#include <vcl_iomanip.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_point_2d.h>


cali_artf_corresponder::cali_artf_corresponder()
 // : NUM_OF_CORRESP(3)
{
  this->fstream.open ("diff.txt", vcl_ofstream::out);
}

cali_artf_corresponder::cali_artf_corresponder(cali_param par):par_(par)
{
 
    this->fstream.open (par_.DIFF.c_str(), vcl_ofstream::out );
}

cali_artf_corresponder::~cali_artf_corresponder(void)
{
}

conic_vector_set cali_artf_corresponder::fit_conics(vil_image_resource_sptr img)
{
        if (!img||!img->ni()||!img->nj())
        {
                vcl_cout << "In segv_vil_segmentation_manager::vd_edges() - no image\n";
                conic_vector_set empty(0);
                return empty;
        }

        sdet_detector_params dp; 
        sdet_fit_conics_params fcp;

        dp.smooth = par_.GAUSSIAN_SIGMA;
        dp.noise_multiplier=par_.THRESHOLD;
        dp.aggressive_junction_closure=0;
        dp.automatic_threshold = 0;
        dp.junctionp = false;
        dp.borderp = false;

        fcp.min_fit_length_= par_.MIN_FIT_LENGTH;
        fcp.rms_distance_= par_.RMS_DISTANCE;
        fcp.aspect_ratio_ = par_.ASPECT_RATIO;

        sdet_detector det(dp);
        det.SetImage(img);
        det.DoContour();
        vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
        if (!edges)
        {
                vcl_cout << "No edges to fit conics\n";
                conic_vector_set empty(0);
                return empty;
        }
        sdet_fit_conics fl(fcp);
        fl.set_edges(*edges);
        fl.fit_conics();
        vcl_vector<vsol_conic_2d_sptr> conics = fl.get_conic_segs();

        vcl_cout << " cali_artf_corresponder::fit_conics(vil_image_resource_sptr img)\n";
        vcl_cout << "fit " << conics.size() << "conics\n";
        return combine_conics(conics);
}

double cali_artf_corresponder::ellipse_angle(vsol_conic_2d_sptr e) const{
        double theta =  e->ellipse_angular_position(e->p0()) - e->ellipse_angular_position(e->p1());
        if (theta < 0) theta = 2*vnl_math::pi + theta;
        return theta; 
}

conic_vector_set cali_artf_corresponder::combine_conics(vcl_vector<vsol_conic_2d_sptr> conics) const
{
        vcl_cout << "conic_vector_set cali_artf_corresponder::combine_conics(vcl_vector<vsol_conic_2d_sptr> conics)\n";

        conic_vector_set comb_conics;
        double cx,cy,phi,width,height;
        double cx2,cy2,phi2,width2,height2;
        bool comb;
        double theta;

        vcl_cout << conics.size() << vcl_endl;
        for(vcl_vector<vsol_conic_2d_sptr>::iterator it1=conics.begin(); 
                        it1 != conics.end(); ++it1) 
        {
                if ((*it1)->is_real_ellipse()){
                        (*it1)->ellipse_parameters(cx,cy,phi,width,height);
                        vcl_cout << "Center-- x=" << cx << "  y=" << cy << vcl_endl;
                        vcl_cout << "\twidth " << width << " " ;
                        vcl_cout << "height " << height << "\n" ;
                        vcl_cout << "\tarc length " <<(*it1)->length()  << "\n" ;
                        //eliminating small conics and conics with high ellipticity 
                        if ((vcl_fabs(width - height) < par_.WIDTH_HEIGHT_DIFF_THRESHOLD)
                                        && (width > par_.WIDTH_THRESHOLD)) 
                        {
                                theta = ellipse_angle(*it1);
                                /*
                                vsol_point_2d_sptr p0 = (*it1)->p0();
                                vsol_point_2d_sptr p1 = (*it1)->p1();
                                double angle0 = (*it1)->ellipse_angular_position(p0);
                               double angle1 = (*it1)->ellipse_angular_position(p1);
                                theta = angle0-angle1;
                                if (theta < 0) theta = 2*vnl_math::pi + theta;
                                */
                                vcl_cout << "\tellipse subtends " << theta*180./vnl_math::pi << "\n" ;
                                if (theta <= 3*vnl_math::pi/2) {
                                        vcl_cout << "\t\tangle <= 270 degrees"  << "\n" ;
                                        comb = false;
                                        // add an error margin to the center of the ellipse
                                        double center_x0 = cx - (width + height)/par_.ERROR_MARGIN_FACTOR;
                                        double center_x1 = cx + (width + height)/par_.ERROR_MARGIN_FACTOR;
                                        double center_y0 = cy - (width + height)/par_.ERROR_MARGIN_FACTOR;
                                        double center_y1 = cy + (width + height)/par_.ERROR_MARGIN_FACTOR;
                                        vcl_cout << "\t\tlooking for centers bounded by " << center_x0 << " " << center_y0;
                                        vcl_cout << "and " << center_x1 << " " << center_y1 << "\n";

                                        for(conic_vector_set::iterator it2=comb_conics.begin(); it2 != comb_conics.end(); ++it2) 
                                        { 
                                                int set_size = (*it2).size();
                                                if (set_size > 0) {
                                                        vsol_conic_2d_sptr ptr = (*it2)[0];
                                                        ptr->ellipse_parameters(cx2,cy2,phi2,width2,height2);
                                                        if ((cx2 >= center_x0) && (cx2 <= center_x1) && 
                                                                        (cy2 >= center_y0) && (cy2 <= center_y1)) {
                                                                // conics are close enough to say they are part of the same ellipse
                                                               vcl_cout  << "\t\t\tfound conic, " << cx2 << " " << cy2 << " within range\n"; 
                                                               double newtheta = ellipse_angle(ptr);
                                                               vcl_cout  << "\t\t\tnew conic subtends " << newtheta*180./vnl_math::pi << ",  old one " << theta*180./vnl_math::pi << "\n";
                                                               if(newtheta > theta){
                                                                       vcl_cout  << "\t\t\tnew conic wins, pushing front\n";
                                                                       (*it2).insert(it2->begin(),*it1);
                                                               }
                                                               else{
                                                                       vcl_cout  << "\t\t\tnew conic loses, pushing back\n";
                                                                       (*it2).push_back(*it1);
                                                               }
                                                               comb = true;
                                                               continue;
                                                        }
                                                }
                                        }
                                        if (!comb){   // add it to the list
                                                vcl_cout  << "\t\t\tno existing match found, adding to list\n";
                                                vcl_vector<vsol_conic_2d_sptr> v;
                                                v.push_back(*it1);
                                                comb_conics.push_back(v);
                                        }
                                }
                                else{
                                        vcl_cout << "\t\tangle > 270 degrees"  << "\n" ;
                                        vcl_cout << "\t\tdoing ... nothing???"  << "\n" ;
                                }
                        }
                        else{
                                vcl_cout << "width/height diff exceeds threshold " << par_.WIDTH_HEIGHT_DIFF_THRESHOLD << "\n";
                                vcl_cout << "or width exceeds threshold " << par_.WIDTH_THRESHOLD << "\n";
                        }
                }
        }

        conic_vector_set sorted = order_conics(comb_conics);
        //  conic_vector_set sorted_along_axis = axis_orientation_sort(sorted);

        vcl_cout << vcl_endl << "After SORTING" << vcl_endl;
        /*print(sorted_along_axis);
          return sorted_along_axis;*/
        print(sorted);
        return sorted;
}


// sort the conics based on their y values
 conic_vector_set
cali_artf_corresponder::order_conics(conic_vector_set &conics) const{
  conic_vector_set sorted;
  double cx, cy, phi, width, height;
  
  conic_vector_set::iterator min_ptr;
// conic_vector_set::iterator max_ptr;

  while (conics.size() > 0) {
    double min_y = 10000;
   //  double max_y = 0;
    for(conic_vector_set::iterator it=conics.begin(); 
      it != conics.end(); ++it) {
      vcl_vector<vsol_conic_2d_sptr> v = (*it);
      // only check the first in the list, they all have the (almost) same center
      v[0]->ellipse_parameters(cx,cy,phi,width,height); 
      if (cy < min_y) {
        min_y = cy;
        min_ptr = it;
      }
      } // for
    sorted.push_back(*min_ptr);
    conics.erase(min_ptr);
  }
  return sorted;
}

// sort the conics based on their y values in descending order
conic_vector_set 
cali_artf_corresponder::reverse_order_conics(conic_vector_set &conics){
  conic_vector_set sorted;
  double cx, cy, phi, width, height;
  
  conic_vector_set::iterator max_ptr;


  while (conics.size() > 0) {
    
     double max_y = -100;
    for(conic_vector_set::iterator it=conics.begin(); 
      it != conics.end(); ++it) {
      vcl_vector<vsol_conic_2d_sptr> v = (*it);
      // only check the first in the list, they all have the (almost) same center
      v[0]->ellipse_parameters(cx,cy,phi,width,height); 
      if (cy > max_y) {
        max_y = cy;
        max_ptr = it;
      }
      } // for
    sorted.push_back(*max_ptr);
    conics.erase(max_ptr);
  }
  return sorted;
}

//:takes a set of simulated artifact images and a list of binary files 
// where real artifact images' conic info is stored
vcl_vector<vcl_pair<conic_vector_set *, conic_vector_set *> > 
cali_artf_corresponder::gen_corresp(vcl_vector<vil_image_resource_sptr> img_vector, 
                                   vcl_string file_base, int interval)
{
    vcl_cout << "file_base is " << file_base << "\n";
  // find the conic info of the given simulated images
  for (unsigned int i=0; i<img_vector.size(); i++) {
    conic_vector_set conics = fit_conics(img_vector[i]);
    vcl_string file_name = gen_read_fname(file_base, i*interval);
    vcl_cout << "reading conics from " << file_name << "...\n";
    conic_vector_set saved_conics = read_conics_bin(file_name);
    vcl_cout << "SAVED CONICS------->" << vcl_endl;
    print(saved_conics);
    correspondences.push_back(vcl_pair<conic_vector_set *, conic_vector_set *>(&conics, &saved_conics));
  }
  return correspondences;
}


void cali_artf_corresponder::diff_corresp_set(vcl_vector<vcl_vector<vsol_conic_2d > > first, 
                vcl_vector<conic_vector_set> second, vnl_vector<double> &diff_vector)
{
  unsigned int size=0, v_size=0, k=0, i, j,count;
  double cx2,cy2,phi2,width2,height2;
  double cx1,cy1,phi1,width1,height1;

  if (first.size() < second.size()) 
      v_size = first.size();
    else 
      v_size = second.size();
    
  for (i=0; i<v_size; i++) {
    
     //   vcl_cout<<" first size "<<first[i].size()<<vcl_endl;
     //   vcl_cout<<" second size "<<second[i].size()<<vcl_endl;

    if (first[i].size() < second[i].size()) 
      size = first[i].size();
    else 
      size = second[i].size();

  
   if (size > par_.NUM_OF_CORRESP) size = par_.NUM_OF_CORRESP;
    for (j=0,count = 0; j<size; j++) {
      // only get the first one in the list, because the list is suppose to contain 
      //the same centered conics
 if (par_.SETMASK[j])
 {
      vsol_conic_2d_sptr conic_second_ptr = second[i][j][0];
      conic_second_ptr->ellipse_parameters(cx2,cy2,phi2,width2,height2);
      if (first[i][j].is_real_ellipse()) {
        first[i][j].ellipse_parameters(cx1,cy1,phi1,width1,height1);
        /*if ((abs(cy2 - cy1 )<140)& (abs(width1 - width2)<15))
        {*/
            count++;
        double diff = sqrt((cx2 - cx1)*(cx2 - cx1) + 
          (cy2 - cy1)*(cy2 - cy1));
        fstream << k << " (" << cx1 << "," << cy1 << ") (" << cx2 << "," << cy2 << ")=" << diff << "--(" << (cx1-cx2) << "," << (cy1 - cy2) << ")" "\n";
      
        diff_vector[k++] = diff; 
      //  vcl_cout<<" difference vector 1 "<< k <<" "<<diff<<vcl_endl;
     
        double rad_diff = sqrt((width2 - width1)*(width2 - width1) + 
          (height2 - height1)*(height2 - height1));
        //fstream << k << " (" <<  << "," << ellipse_radius << ") = " << rad_diff << "\n";
        fstream << k << " (" << width1 << "," << height1 << ") (" << width2 << "," << height2 << ")=" << rad_diff << "--(" << (width1-width2) << "," << (height1 - height2) << ")" "\n";
       
        diff_vector[k++] = rad_diff;
       //  vcl_cout<<" difference vector 2 "<< k <<" "<<rad_diff<<vcl_endl;
      //}
     }
      else {
        vcl_cout << "Not a real ellipse!" << vcl_endl;
        vcl_cout << first[i][j] << vcl_endl;
        // to ignore the correspondence, place 0 for the differences,
        // one for the center difference, one for the width, height difference
        diff_vector[k++] = 0;
        diff_vector[k++] = 0;
      }
 }
    }
    // if the number of correspondences smaller than the preset value, place 
    // zero for the difference values, one for the center difference, one for 
    // the width, height difference
    for (; count < par_.NUM_OF_CORRESP; count++) {
         fstream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" "\n";
        diff_vector[k++]=0.0;
         fstream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" "\n";
      diff_vector[k++]=0.0;
    }
    
  }
  

}
  void cali_artf_corresponder::robust_corresp_set(vcl_vector<vcl_vector<vsol_conic_2d> > first, 
                                             vcl_vector<conic_vector_set> second, 
                                             vnl_vector<double> &diff_vector)
{
   unsigned int t_size,i,j,size,k = 0,count,offset;
   double cx1,cy1,width1,height1,phi1,cx2,cy2,width2,height2,phi2;
       
   if (first.size()>second.size())
       t_size = second.size();
   else
       t_size = first.size();
  

    for (i = 0;i<t_size;i++)
    {
        if (first[i].size()>second[i].size())
            size = second[i].size();
        else
            size = first[i].size();

if (size>par_.NUM_OF_CORRESP)
size = par_.NUM_OF_CORRESP;


    for (j = 0,count = 0;j<size;j++)
    {
        if (first[i][j].is_real_ellipse())
    {
        first[i][j].ellipse_parameters(cx1,cy1,phi1,width1,height1);
        second[i][j][0]->ellipse_parameters(cx2,cy2,phi2,width2,height2);
offset = 0;
        while(((cy1 - cy2) > 200) & (j+offset<size))
        {
            vcl_cout<<" the correspondences where there is mismatch " << k <<vcl_endl;
            first[i][j+offset].ellipse_parameters(cx1,cy1,phi1,width1,height1);
            offset++;
        }

     double diff = sqrt((cx2 - cx1)*(cx2 - cx1) + 
          (cy2 - cy1)*(cy2 - cy1));
        fstream << k << " (" << cx1 << "," << cy1 << ") (" << cx2 << "," << cy2 << ")=" << diff << "--(" << (cx1-cx2) << "," << (cy1 - cy2) << ")" "\n";
      /* vcl_cout << k << " (" << cx1 << "," << cy1 << ") (" << cx2 << "," << cy2 << ")=" << diff << "--(" << (cx1-cx2) << "," << (cy1 - cy2) << ")" <<vcl_endl;*/
        diff_vector[k++] = diff; 
      
     
        double rad_diff = sqrt((width2 - width1)*(width2 - width1) + 
          (height2 - height1)*(height2 - height1));

        fstream << k << " (" << width1 << "," << height1 << ") (" << width2 << "," << height2 << ")=" << rad_diff << "--(" << (width1-width2) << "," << (height1 - height2) << ")" "\n";
    /* vcl_cout << k << " (" << width1 << "," << height1 << ") (" << width2 << "," << height2 << ")=" << rad_diff << "--(" << (width1-width2) << "," << (height1 - height2) << ")" <<vcl_endl;  */
        diff_vector[k++] = rad_diff;

    }
    else {
        vcl_cout << "Not a real ellipse!" << vcl_endl;
        vcl_cout << first[i][j] << vcl_endl;
        // to ignore the correspondence, place 0 for the differences,
        // one for the center difference, one for the width, height difference
        diff_vector[k++] = 0;
        diff_vector[k++] = 0;
      }
    count++;
    }

    for (;count<par_.NUM_OF_CORRESP;count++)
    {
        fstream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" "\n";
        diff_vector[k++] = 0;
        fstream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" "\n";
        diff_vector[k++] = 0;

}
    }

}
/**
 * assumes that synthetic_conics and found_conics are both sorted by y-value
 **/

void cali_artf_corresponder::verbose_error_summary(vcl_vector<vcl_vector<vsol_conic_2d> > first, 
                                             conic_vector_set second, 
                                             vnl_vector<double> &diff_vector) 
{
        unsigned int t_size,i,j,size,k = 0,count,offset,m;
        double cx1,cy1,width1,height1,phi1,cx2,cy2,width2,height2,phi2;

        //number of images.  these should never differ 
        if (first.size()>second.size())
                t_size = second.size();
        else
                t_size = first.size();

        vcl_cout << "first.size() " <<first.size() << " images\n";
        vcl_cout << "comparing " << t_size << " images\n";
        for (i = 0;i<t_size;i++)
        {
                if (i != 0)
                {
                        int first_ball;
                        vcl_cout << "first[" << i << "].size() " << first[i].size() << "\n";
                        vcl_cout << "second[" << i << "].size() " << second[i].size() << "\n";
                        //number of conics found in each image.  
                        //might differ depending on fitting
                        if (first[i].size()>second[i].size())
                                size = second[i].size();
                        else
                                size = first[i].size();

                        vcl_cout << "image  " << i << ", choosing smaller, using " << size << " conics\n";
                        if (size>par_.NUM_OF_CORRESP)
                                size = par_.NUM_OF_CORRESP;
                        m = 0;

                        //vcl_cout << "NUM_OF_CORRESP is " << par_.NUM_OF_CORRESP << "\n";
                        vcl_cout << "image " << i << ", using " << size << " conics\n";
                        for(int idx = 0; idx < first[i].size(); idx++){
                                        if (first[i][idx].is_real_ellipse()){
                                                first[i][idx].ellipse_parameters(cx1,cy1,phi1,width1,height1);
                                                vcl_cout << "\tfirst[" << i <<  "][" << idx << "]  center " << cx1 << " " << cy1 << "\n"; 
                                        }
                                        else{
                                                vcl_cout << "\tfirst[" << i <<  "][" << idx << "]  not a real ellipse\n"; 
                                        }
                        }
                        for(int idx = 0; idx < second[i].size(); idx++){
                                if(second[i][idx]->is_real_ellipse()){ 
                                        second[i][idx]->ellipse_parameters(cx1,cy1,phi1,width1,height1);
                                        vcl_cout << "\tsecond[" << i <<  "][" << idx << "]  center " << cx1 << " " << cy1 << "\n"; 
                                }
                                else{
                                        vcl_cout << "\tsecond[" << i <<  "][" << idx << "]  not a real ellipse\n"; 

                                }
                        }
                        for (j = 0,count = 0;j<size;j++)
                        {
                                //finds the first ball that is desired for
                                //correspondnece and has associated conics in
                                //both lists
                                while  (!(par_.SETMASK[j+m])&& (j+m<first[i].size())&&(j+m<second[i].size()))
                                {
                                        m++;
                                }

                                

                                //it is possible the previous loop terminated
                                //because it exceeded the list size, check here
                                if ((j+m<first[i].size()) && (j+m<second[i].size()))
                                {
                                        //we know we want to compare the (j+m)'th
                                        //ball, but is it a real ellipse
                                        if (first[i][j+m].is_real_ellipse())
                                        {
                                                ///get the ellipse parameters
                                                //from both lists. 
                                                //why always  second[i][j+m][0]??
                                                first[i][j+m].ellipse_parameters(cx1,cy1,phi1,width1,height1);
                                                second[i][j+m]->ellipse_parameters(cx2,cy2,phi2,width2,height2);
                                                vcl_cout << "\tfirst list " << j+m << " center " << cx1 << " " << cy1 << "\n"; 
                                                vcl_cout << "\tsecond list " << j+m << " center " << cx2 << " " << cy2 << "\n"; 
                                                first_ball = j+m;

                                                offset = 0;

                                                //search through the first conic
                                                //list until you hit the end or
                                                //find a ball within the Y range
                                                //of the first ball in the
                                                //second list
                                                while((fabs(cy1 - cy2) > par_.Y_COORD_DIFF_RANGE) && (j+m+offset<first[i].size()) )
                                                {

                                                        if (first[i][j+m+offset].is_real_ellipse() && par_.SETMASK[j+m+offset]){
                                                                first[i][j+m+offset].ellipse_parameters(cx1,cy1,phi1,width1,height1);
                                                                first_ball = j+m+offset;
                                                        }
                                                        vcl_cout << "\t\t mismatch...";
                                                        vcl_cout << " first list " << j+m+offset << " center " << cx1 << " " << cy1 << "\n"; 
                                                        offset++;
                                                }

                                                //if you failed to find a ball
                                                //within the Y range in the
                                                //previous loop...
                                                if (fabs(cy1 - cy2)>par_.Y_COORD_DIFF_RANGE)
                                                {
                                                        vcl_cout << "\t\t mismatch again...\n";
                                                        //go back to checking
                                                        //against the first ball
                                                        //in the first list
                                                        offset = 0;
                                                        first[i][j+m].ellipse_parameters(cx1,cy1,phi1,width1,height1);
                                                        first_ball = j+m;
                                                        vcl_cout << "\t\tnow on first list " << j+m+offset << " center " << cx1 << " " << cy1 << "\n"; 
                                                        //search through the
                                                        //second list until you
                                                        //hit the end or find a
                                                        //ball within the Y
                                                        //range of the first
                                                        //ball in the first list
                                                        while((fabs(cy1 - cy2) > static_cast<double>(par_.Y_COORD_DIFF_RANGE)) && (j+m+offset<second[i].size()) )
                                                        {
                                                                second[i][j+m+offset]->ellipse_parameters(cx2,cy2,phi2,width2,height2);

                                                                vcl_cout << "\t\t mismatch...";
                                                                vcl_cout << " second list " << j+m+offset << " center " << cx2 << " " << cy2 << "\n"; 
                                                                offset++;
                                                        }
                                                }

                                                //if you succeeded finding 2
                                                //conics that fall within the
                                                //defined Y and X ranges of one
                                                //another, then calculate the
                                                //center differences and the
                                                //radius differnces
                                                if ((fabs(cy1 - cy2)<par_.Y_COORD_DIFF_RANGE) && (fabs(cx1 - cx2)<par_.X_COORD_DIFF_RANGE))
                                                { 
                                                        vcl_cout << "\t match, centers " << cx1 << " " << cy1 << " and " << cx2 << " " << cy2 << "\n"; 
                                                        double diff = sqrt((cx2 - cx1)*(cx2 - cx1) + 
                                                                        (cy2 - cy1)*(cy2 - cy1));
                                                        fstream << k << " (" << cx1 << "," << cy1 << ") (" << cx2 << "," << cy2 << ")=" << diff << "--(" << (cx1-cx2) << "," << (cy1 - cy2) 
                                                        << ") img: " << i << "(" << cx1 << "," << cy1 << ") PT, ball " << first_ball+1 << "\n";
                                                        diff_vector[k++] = diff; 

                                                        double rad_diff = sqrt((width2 - width1)*(width2 - width1) + 
                                                                        (height2 - height1)*(height2 - height1));

                                                        fstream << k << " (" << width1 << "," << height1 << ") (" << width2 << "," << height2 << ")=" << rad_diff << "--(" << (width1-width2) << "," << (height1 - height2) << ") img: " << i << "(" << cx1 << "," << cy1 << ") RAD, ball " << first_ball +1<< "\n";
                                                        diff_vector[k++] = rad_diff; 
                                                        vcl_cout << "\t" << k << " differences; new diffs: dcenter = " << diff << " drad = " << rad_diff << "\n";
                                                }
                                                //otherwise, put zeroes
                                                else
                                                {
                                                        vcl_cout<<" assigning zeroes to ignore the correspondence as it is a mismatch " <<vcl_endl;
                                                        fstream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" 
                                                        << ") img: " << i << "(" << cx1 << "," << cy1 << ")\n";

                                                        diff_vector[k++] = 10;
                                                        fstream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" 
                                                        << ") img: " << i << "(" << cx1 << "," << cy1 << ")\n";
                                                        diff_vector[k++] = 10;
                                                }


                                        }
                                        //when the first ball of the first list
                                        //is not a real ellipse, add zeroes and go to the next ball
                                        else {
                                                vcl_cout << "Not a real ellipse!" << vcl_endl;
                                                vcl_cout << first[i][j] << vcl_endl;
                                                // to ignore the correspondence, place 0 for the differences,
                                                // one for the center difference, one for the width, height difference
                                                diff_vector[k++] = 0;
                                                diff_vector[k++] = 0;
                                        }
                                        count++;
                                }

                        }

                        for (;count<par_.NUM_OF_CORRESP;count++)
                        {

                                fstream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" "\n";
                                diff_vector[k++] = 0;
                                fstream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" "\n";
                                diff_vector[k++] = 0;
                                vcl_cout << "\t" << k << " differences; padding with zero for remaining correspondences\n";

                        }
                }
        }
}

/**
 * assumes that synthetic_conics and found_conics are both sorted by y-value
 **/
void cali_artf_corresponder::new_correspondence(const vcl_vector<vcl_vector<vsol_conic_2d> >& synthetic_conics, 
                                             const conic_vector_set& found_conics, 
                                             vnl_vector<double> &diff_vector) 
{ 
        unsigned int t_size,size,k = 0,count,offset,m;
        //number of images.  these should never differ 
        if(synthetic_conics.size() > found_conics.size()){
                    vcl_cerr << "WHOAH! Why is found_conics vector smaller than synthetic conic vector ???\n";
                    exit(1);
        }

        for(int img = 0; img < synthetic_conics.size(); img++){
                vcl_cout << "Image : " << img << "\n";

                if(synthetic_conics[img].size() < par_.BALL_NUMBER){
                            vcl_cerr << "In img : " << img << ", only found " << synthetic_conics[img].size() << " conics\n";
                }
                else if(synthetic_conics[img].size() > par_.BALL_NUMBER){
                            vcl_cerr << "In img : " << img << ", found too many:  " << synthetic_conics[img].size() << " conics\n";
                }

                int mask_count = 0;
                for(int maskidx =0; maskidx < par_.BALL_NUMBER; maskidx++){
                        mask_count += par_.SETMASK[maskidx];
                }
                if(mask_count != par_.NUM_OF_CORRESP){
                        vcl_cerr << "Mask indicates using " << mask_count << " balls, but par_.NUM_OF_CORRESP is " << par_.NUM_OF_CORRESP << "\n";
                        exit(1);
                }

                for(int idx = 0; idx < synthetic_conics[img].size(); idx++){
                        double cx1,cy1,width1,height1,phi1,cx2,cy2,width2,height2,phi2;
                        synthetic_conics[img][idx].ellipse_parameters(cx1,cy1,phi1,width1,height1);
                        vcl_cout << "\tsynth[" << img <<  "][" << idx << "]  center " << cx1 << " " << cy1 << "\n"; 
                }
                for(int idx = 0; idx < found_conics[img].size(); idx++){ 
                        double cx1,cy1,width1,height1,phi1,cx2,cy2,width2,height2,phi2;
                        found_conics[img][idx]->ellipse_parameters(cx1,cy1,phi1,width1,height1);
                        vcl_cout << "\tfound_conics[" << img <<  "][" << idx << "]  center " << cx1 << " " << cy1 << "\n"; 
                }


                int last_match = -1; 
                for(int conic_count = 0; conic_count < synthetic_conics[img].size(); conic_count++){
                        vcl_cout << "\tBall : " << conic_count << "\n";

                        double cx1,cy1,width1,height1,phi1,cx2,cy2,width2,height2,phi2;

                        double diff = 0;
                        double rad_diff = 0;

                        if(par_.SETMASK[conic_count]){
                                if(synthetic_conics[img][conic_count].is_real_ellipse()){
                                        synthetic_conics[img][conic_count].ellipse_parameters(cx1,cy1,phi1,width1,height1);
                                        vcl_cout << "\t\tSynthetic [" << conic_count << "] : " << cx1 << " " << cy1  << "\n";

                                        bool found=false;
                                        for(int offset = last_match+1; offset < found_conics[img].size(); offset++){
                                                found_conics[img][offset]->ellipse_parameters(cx2,cy2,phi2,width2,height2); 
                                                vcl_cout << "\t\tFound[" << offset << "] : " << cx2 << " " << cy2  << "\n";
                                                if((vcl_fabs(cy1 - cy2) < par_.Y_COORD_DIFF_RANGE)){
                                                        last_match = offset;
                                                        found = true; break;
                                                }
                                        }

                                        if(found){

                                                if(vcl_fabs(cx1-cx2) < par_.X_COORD_DIFF_RANGE){
                                                        vcl_cout << "\t\t match, centers " << cx1 << " " << cy1 << " and " << cx2 << " " << cy2 << "\n"; 
                                                        diff = sqrt((cx2 - cx1)*(cx2 - cx1) + (cy2 - cy1)*(cy2 - cy1));
                                                        rad_diff = sqrt((width2 - width1)*(width2 - width1) + (height2 - height1)*(height2 - height1));
                                                }
                                                else{
                                                        vcl_cout << "\t\t mismatch on X coords , centers " << cx1 << " " << cy1 << " and " << cx2 << " " << cy2 << "\n"; 
                                                        vcl_cout<<" assigning zeroes to ignore the correspondence as it is a mismatch " <<vcl_endl;
                                                }

                                        } 
                                        else{
                                                vcl_cout << "\t\t mismatch on Y coords, last centers " << cx1 << " " << cy1 << " and " << cx2 << " " << cy2 << "\n"; 
                                                vcl_cout<<" assigning zeroes to ignore the correspondence as it is a mismatch " <<vcl_endl;
                                        }

                                }
                                else{
                                        vcl_cout << "Image : " << img << ", synthetic conic " << conic_count << " not a real ellipse\n";
                                }
                        }
                        else{
                                vcl_cout << "Image : " << img << ",ball " << conic_count << " masked out\n";
                        }


                        fstream << diff << " --> (" << cx1 << "," << cy1 << ") vs (" << cx2 << "," << cy2 << ")"; 
                        fstream << " img: " << img << " PT, ball " << conic_count+1 << "\n";

                        fstream << rad_diff << " --> (" << width1 << "," << height1 << ") vs (" << width2 << "," << height2 << ")"; 
                        fstream << " img: " << img << " RAD, ball " << conic_count+1 << "\n";

                        diff_vector[k++] = diff; 
                        diff_vector[k++] = rad_diff; 
                } //loop over conics 
        } // loop over images

}

void cali_artf_corresponder::masked_corresp_set(vcl_vector<vcl_vector<vsol_conic_2d> > first, 
                vcl_vector<conic_vector_set> second, 
                vnl_vector<double> &diff_vector)


{

        unsigned int t_size,i,j,size,k = 0,count,offset,m;
        double cx1,cy1,width1,height1,phi1,cx2,cy2,width2,height2,phi2;

        //number of images.  these should never differ 
        if (first.size()>second.size())
                t_size = second.size();
        else
                t_size = first.size();


        for (i = 0;i<t_size;i++)
        {
                if (i != 0)
                {
                        //number of conics found in each image.  
                        //might differ depending on fitting
                        if (first[i].size()>second[i].size())
                                size = second[i].size();
                        else
                                size = first[i].size();

                        if (size>par_.NUM_OF_CORRESP)
                                size = par_.NUM_OF_CORRESP;
                        m = 0;


                        for (j = 0,count = 0;j<size;j++)
                        {

                                while  (!(par_.SETMASK[j+m])& (j+m<first[i].size())&(j+m<second[i].size()))
                                {
                                        m++;
                                }
                                if ((j+m<first[i].size()) & (j+m<second[i].size()))
                                {
                                        if (first[i][j+m].is_real_ellipse())
                                        {
                                                first[i][j+m].ellipse_parameters(cx1,cy1,phi1,width1,height1);
                                                second[i][j+m][0]->ellipse_parameters(cx2,cy2,phi2,width2,height2);

                                                offset = 0;

                                                /*while((abs(cy1 - cy2) > 20 & (j+m+offset<first[i].size()))|(abs(cx1 - cx2)>100 & (j+m+offset<first[i].size())) )*/
                                                //   while((abs(cy1 - cy2) > 20) & (j+m+offset<first[i].size()) )
                                                while((fabs(cy1 - cy2) > par_.Y_COORD_DIFF_RANGE) & (j+m+offset<first[i].size()) )
                                                {
                                                        /*  vcl_cout<<" the correspondences where there is mismatch " << k <<vcl_endl;*/
                                                        first[i][j+m+offset].ellipse_parameters(cx1,cy1,phi1,width1,height1);
                                                        offset++;
                                                }

                                                //  if (abs(cy1 - cy2)>20)
                                                if (fabs(cy1 - cy2)>par_.Y_COORD_DIFF_RANGE)
                                                {
                                                        offset = 0;
                                                        first[i][j+m].ellipse_parameters(cx1,cy1,phi1,width1,height1);
                                                        /* while((abs(cy1 - cy2) > 20 & (j+m+offset<second[i].size()))|(abs(cx1 - cx2)>100 & (j+m+offset<second[i].size())) )*/
                                                        //     while((abs(cy1 - cy2) > 20) & (j+m+offset<second[i].size()) )
                                                        while((fabs(cy1 - cy2) > static_cast<double>(par_.Y_COORD_DIFF_RANGE)) & (j+m+offset<second[i].size()) )
                                                        {
                                                                /*vcl_cout<<" the correspondences where there is mismatch " << k <<vcl_endl;*/
                                                                second[i][j+m+offset][0]->ellipse_parameters(cx2,cy2,phi2,width2,height2);
                                                                offset++;
                                                        }
                                                }

                                                //   if ((abs(cy1 - cy2)<40) & (abs(cx1 - cx2)<500))
                                                //   if (abs(cy1 - cy2)<20 )
                                                if ((fabs(cy1 - cy2)<par_.Y_COORD_DIFF_RANGE) & (fabs(cx1 - cx2)<par_.X_COORD_DIFF_RANGE))
                                                {


                                                        double diff = sqrt((cx2 - cx1)*(cx2 - cx1) + 
                                                                        (cy2 - cy1)*(cy2 - cy1));
                                                        //   f_stream << k << " (" << cx1 << "," << cy1 << ") (" << cx2 << "," << cy2 << ")=" << diff << "--(" << (cx1-cx2) << "," << (cy1 - cy2) << ")" "\n";
                                                        fstream << k << " (" << cx1 << "," << cy1 << ") (" << cx2 << "," << cy2 << ")=" << diff << "--(" << (cx1-cx2) << "," << (cy1 - cy2) << ")" "\n";
                                                        /* vcl_cout << k << " (" << cx1 << "," << cy1 << ") (" << cx2 << "," << cy2 << ")=" << diff << "--(" << (cx1-cx2) << "," << (cy1 - cy2) << ")" <<vcl_endl;*/
                                                        diff_vector[k++] = diff; 


                                                        double rad_diff = sqrt((width2 - width1)*(width2 - width1) + 
                                                                        (height2 - height1)*(height2 - height1));

                                                        //  f_stream << k << " (" << width1 << "," << height1 << ") (" << width2 << "," << height2 << ")=" << rad_diff << "--(" << (width1-width2) << "," << (height1 - height2) << ")" "\n";

                                                        fstream << k << " (" << width1 << "," << height1 << ") (" << width2 << "," << height2 << ")=" << rad_diff << "--(" << (width1-width2) << "," << (height1 - height2) << ")" "\n";
                                                        /* vcl_cout << k << " (" << width1 << "," << height1 << ") (" << width2 << "," << height2 << ")=" << rad_diff << "--(" << (width1-width2) << "," << (height1 - height2) << ")" <<vcl_endl;  */
                                                        diff_vector[k++] = rad_diff;

                                                }
                                                else
                                                {
                                                        vcl_cout<<" assigning zeroes to ignore the correspondence as it is a mismatch " <<vcl_endl;
                                                        //    f_stream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" "\n";

                                                        fstream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" "\n";
                                                        diff_vector[k++] = 0;
                                                        //  f_stream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" "\n";

                                                        fstream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" "\n";
                                                        diff_vector[k++] = 0;

                                                }


                                        }
                                        else {
                                                vcl_cout << "Not a real ellipse!" << vcl_endl;
                                                vcl_cout << first[i][j] << vcl_endl;
                                                // to ignore the correspondence, place 0 for the differences,
                                                // one for the center difference, one for the width, height difference
                                                diff_vector[k++] = 0;
                                                diff_vector[k++] = 0;
                                        }
                                        count++;
                                }
                        }

                        for (;count<par_.NUM_OF_CORRESP;count++)
                        {
                                //  f_stream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" "\n";
                                fstream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" "\n";
                                diff_vector[k++] = 0;
                                //  f_stream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" "\n";
                                fstream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" "\n";
                                diff_vector[k++] = 0;

                        }
                }
        }

}


#if 0
void cali_artf_corresponder::manual_corresp_set(conic_vector_setfirst,vnl_vector<double> &diff_vector)
                                             
{
  double cx1,cy1,cx2,cy2,width1,height1,width2,height2,phi2;
  double diff,rad_diff; 
  unsigned int i,k = 0,j = 0,a = 0,size;
  
  vcl_ofstream out(par_.DIFF.c_str(),vcl_fstream::out | vcl_ofstream::app);


for (i = 0;i<first.size();i++)
{
    size = first[i].size();

if (size > par_.NUM_OF_CORRESP) size = par_.NUM_OF_CORRESP;

    for (j = 0;j<size;j++)
    {
        cx1 = par_.COORDS[a].first;
        cy1 = par_.COORDS[a].second;

        width1 = par_.HEIGHT_RADII[a].first;
        height1 = par_.HEIGHT_RADII[a].second;
         first[i][j + 3].ellipse_parameters(cx2,cy2,phi2,width2,height2);
        
        diff = sqrt((cx2 - cx1)*(cx2 - cx1) + 
          (cy2 - cy1)*(cy2 - cy1));
     out << k << " (" << cx1 << "," << cy1 << ") (" << cx2 << "," << cy2 << ")=" << diff << "--(" << (cx1-cx2) << "," << (cy1 - cy2) << ")" "\n";
 //     vcl_cout<< k << "  " <<diff <<vcl_endl;
//  vcl_cout << k << " (" << cx1 << "," << cy1 << ") (" << cx2 << "," << cy2 << ")=" << diff << "--(" << (cx1-cx2) << "," << (cy1 - cy2) << ")" "\n";
        diff_vector[k++] = diff;

        rad_diff = sqrt((width2 - width1)*(width2 - width1) + 
            (height2 - height1)*(height2 - height1));
     out << k << " (" << width1 << "," << height1 << ") (" << width2 << "," << height2 << ")=" << rad_diff << "--(" << (width1-width2) << "," << (height1 - height2) << ")" "\n";   
 //  vcl_cout<< k << "  " <<rad_diff <<vcl_endl;
// vcl_cout << k << " (" << width1 << "," << height1 << ") (" << width2 << "," << height2 << ")=" << rad_diff << "--(" << (width1-width2) << "," << (height1 - height2) << ")" "\n";   
        diff_vector[k++] = diff;
        a++;
    }

}

}

#endif


                                             
  void cali_artf_corresponder::manual_corresp_set(vcl_vector<vcl_vector<vsol_conic_2d > > first,vnl_vector<double> &diff_vector)
{
  double cx1,cy1,cx2,cy2,width1,height1,width2,height2,phi2;
  double diff,rad_diff; 
  unsigned int i,k = 0,j = 0,a = 0,size,count;
  
  vcl_ofstream out(par_.DIFF.c_str(),vcl_fstream::out | vcl_ofstream::app);


for (i = 0;i<first.size();i++)
{
    size = first[i].size();

if (size > par_.NUM_OF_CORRESP) size = par_.NUM_OF_CORRESP;
count = 0;
    for (j = 0;j<first[i].size();j++)
    {
        if ((j != 0) & (j != 1) &(j != 2)&(j!=3) &(j!=4))
    //  if ((j != 0) & (j!= 1))
        {
            count++;
        cx1 = par_.COORDS[a].first;
        cy1 = par_.COORDS[a].second;

        width1 = par_.HEIGHT_RADII[a].first;
        height1 = par_.HEIGHT_RADII[a].second;
         first[i][first[i].size() - j -1].ellipse_parameters(cx2,cy2,phi2,width2,height2);
       /* first[i][j].ellipse_parameters(cx2,cy2,phi2,width2,height2);*/

        diff = sqrt((cx2 - cx1)*(cx2 - cx1) + 
          (cy2 - cy1)*(cy2 - cy1));
     out << k << " (" << cx1 << "," << cy1 << ") (" << cx2 << "," << cy2 << ")=" << diff << "--(" << (cx1-cx2) << "," << (cy1 - cy2) << ")" "\n";
 //     vcl_cout<< k << "  " <<diff <<vcl_endl;
//  vcl_cout << k << " (" << cx1 << "," << cy1 << ") (" << cx2 << "," << cy2 << ")=" << diff << "--(" << (cx1-cx2) << "," << (cy1 - cy2) << ")" "\n";
        diff_vector[k++] = diff;

        rad_diff = sqrt((width2 - width1)*(width2 - width1) + 
            (height2 - height1)*(height2 - height1));
     out << k << " (" << width1 << "," << height1 << ") (" << width2 << "," << height2 << ")=" << rad_diff << "--(" << (width1-width2) << "," << (height1 - height2) << ")" "\n";   
 //  vcl_cout<< k << "  " <<rad_diff <<vcl_endl;
// vcl_cout << k << " (" << width1 << "," << height1 << ") (" << width2 << "," << height2 << ")=" << rad_diff << "--(" << (width1-width2) << "," << (height1 - height2) << ")" "\n";   
        diff_vector[k++] = diff;
        a++;
    }
        if (count==par_.NUM_OF_CORRESP)
            break;

}
}
}



//: saves a given conic sets list into 
void cali_artf_corresponder::save_conics_bin(conic_vector_set conics, vcl_string file_name){
  //vcl_string file_name = "test_binary_write.bin";
  
  cali_conic_info info(conics, 0, 10);
  vsl_b_ofstream stream(file_name.c_str());
  info.b_write(stream);
  stream.close();
}

conic_vector_set cali_artf_corresponder::read_conics_bin(vcl_string file_name){
  vsl_b_ifstream istream(file_name.c_str());
  cali_conic_info newinfo=cali_conic_info();
  newinfo.b_read(istream);
  return newinfo.get_conic_list();

}

vcl_ostream& cali_artf_corresponder::print(vcl_ostream&  s,  
  conic_vector_set const &list)
{
  unsigned int i,j;
  double cx, cy, phi, width, height;

  s << "size=" << list.size() << "\n";
  for(i=0; i < list.size(); i++) {
      vcl_vector<vsol_conic_2d_sptr> v = list[i];
      for(j=0; j <v.size(); j++) {
      v[j]->ellipse_parameters(cx,cy,phi,width,height);
      s  << i << "-" << j << " Center-- x=" << cx << "  y=" << cy << " width=" << width << " height=" << height << " p0=" << *(v[j]->p0()) << " p1=" << *(v[j]->p1()) << " length = " << v[j]->length();
    }
  }
  s << "\n";
  return s;
}
   

void cali_artf_corresponder::print(conic_vector_set &list){
  vcl_cout << list.size() << vcl_endl;
  int i =0;
  unsigned int j;
  double cx, cy, phi, width, height;

  for(conic_vector_set::iterator it3=list.begin(); 
    it3 != list.end(); ++it3) {
      vcl_vector<vsol_conic_2d_sptr> v = (*it3);
      for(j=0; j <v.size(); j++) {
        v[j]->ellipse_parameters(cx,cy,phi,width,height);
        vcl_cout << i << "-" << j << " Center-- x=" << cx << "  y=" << cy << " width=" << width << " height=" << height << 
        " p0=" << *(v[j]->p0()) << " p1=" << *(v[j]->p1()) << " length = " << v[j]->length() <<
 vcl_endl;
    }
    i++;
  }
}

//: generates a file name for reading the binary conic files
// it appends a number and .bin at the end of the file base
vcl_string cali_artf_corresponder::gen_read_fname(vcl_string file_base, int i)
{
        return gen_write_fname(file_base,i);
        /*
    char* num = new char[4];
    num[0] = num[1] = num[2] = num[3] = '0';
    sprintf (num, "%d", i);
    vcl_string num_str = num;
    vcl_cout<<"num" <<num<<vcl_endl;
    file_base.replace(file_base.size()-num_str.size(), num_str.size(), num);
    vcl_string outname = file_base;
    outname += ".bin";
    return outname;
    */
}

//: generates a file name for writing the binary info for the conics.
// fname is the name of the real scan image, 
// e.g. artifact0000.tif becomes bins/artifact0000.bin
//      artifact0010.tif becomes bins/artifact0010.bin
//      
vcl_string cali_artf_corresponder::gen_write_fname(vcl_string fname, int i)
{
        vcl_string dir = vul_file::dirname(fname);
        vcl_string ext = vul_file::extension(fname);
        vcl_string base = vul_file::basename(fname,ext.c_str());
        vcl_string num_str;
        vcl_stringstream ss;
        ss << std::setfill('0') << std::setw(4);
        ss << i; 
        ss >> num_str;

        int idx = base.length()-1;
        while(idx > 0 && base[idx] >= '0' && base[idx] <= '9'){
                idx--;
        }
        base.assign(base,0,idx+1);
#if defined(VCL_WIN32)
        return dir + "\\" base + num_str + ".bin";
#else
        return dir + "/" + base + num_str + ".bin";
#endif
}



conic_vector_set cali_artf_corresponder::axis_orientation_sort(conic_vector_set real_conics)
{
        unsigned int count = 0,j = 0,k;
        double  cx,cy,height,width,phi,angle,x,y,min_y,x1,y1;
        vgl_point_2d<double> p1(0,0),p2(0,0);
        vgl_line_segment_2d<double> l;
        conic_vector_set sorted_conics;


        // conic_vector_setreverse_sorted = reverse_order_conics(real_conics);
        vcl_vector<vgl_point_2d<double> >centers;

        for (conic_vector_set::iterator iter = real_conics.begin();iter != real_conics.end();iter++)
        {
                count++;
                vcl_vector<vsol_conic_2d_sptr> v = (*iter);
                v[0]->ellipse_parameters(cx,cy,phi,width,height);
                vgl_point_2d<double>point(cx,cy);
                centers.push_back(point);
                if (count == 5)
                        p1.set(cx,cy);
                if ((width>30)&(width<50))
                        p2.set(cx,cy);
        }

        l.set(p1,p2);
        angle =  l.slope_radians();
        angle = fabs(angle);
        angle = -(- vnl_math::pi/2 + angle);

        vcl_vector<double>y_coord;
        for (vcl_vector<vgl_point_2d<double> >::iterator it = centers.begin();it != centers.end();it++)
        {
                x =  it->x();
                y = it->y();
                x = x - p2.x();
                y = y - p2.y();
                x1 = x*vcl_cos(angle) - y *vcl_sin(angle);
                y1 = x*vcl_sin(angle) + y *vcl_cos(angle);

                /*r = vcl_sqrt((x-p1.x())*(x - p1.x()) + (y - p1.y())*(y-p1.y()));
                  y = y + r - r*vcl_cos(angle);*/
                y_coord.push_back(y1);
        }

        //while (y_coord.size() != 0)
        for (unsigned int a = 0;a<y_coord.size();a++)
        {
                min_y = 10000;
                count = 0;
                j = 0,k =0;
                vcl_vector<double> ::iterator min_ptr;
                for (vcl_vector<double> ::iterator it = y_coord.begin();it != y_coord.end();it++)
                {

                        if ((*it)<min_y)
                        {
                                min_y = *it;
                                count = j;
                                min_ptr = it;
                                // *it = 10000;
                        }
                        j++;
                }
                y_coord[count] = 10000;
                //  vcl_vector<vsol_conic_2d_sptr>temp;
                /* vcl_cout<<" testing the size in orientation_sort " <<real_conics[count].size() <<vcl_endl;
                   for (int k = 0;k <real_conics[count].size();k++)
                   {
                   temp.push_back(real_conics[count][k]);
                   }*/
                for (conic_vector_set::iterator ite = real_conics.begin();ite != real_conics.end();ite++)
                {
                        if (k == count)
                                sorted_conics.push_back(*ite);
                        k++;
                }


                //  sorted_conics.push_back(temp);
                // y_coord.erase(min_ptr);

        }



        return sorted_conics;
}

#if 0

void cali_artf_corresponder::diff_corresp_set(conic_vector_set first, 
                                             vcl_vector<conic_vector_set> second, 
                                             vnl_vector<double> &diff_vector) 
{
  unsigned int size=0, v_size=0, k=0, i, j,count;
  double cx2,cy2,phi2,width2,height2;
  double cx1,cy1,phi1,width1,height1;

  if (first.size() < second.size()) 
      v_size = first.size();
    else 
      v_size = second.size();
    
  for (i=0; i<v_size; i++) {
    
     //   vcl_cout<<" first size "<<first[i].size()<<vcl_endl;
     //   vcl_cout<<" second size "<<second[i].size()<<vcl_endl;

    if (first[i].size() < second[i].size()) 
      size = first[i].size();
    else 
      size = second[i].size();

  
   if (size > par_.NUM_OF_CORRESP) size = par_.NUM_OF_CORRESP;
    for (j=0,count = 0; j<size; j++) {
      // only get the first one in the list, because the list is suppose to contain 
      //the same centered conics
 if (par_.SETMASK[j])
 {
      vsol_conic_2d_sptr conic_second_ptr = second[i][j][0];
      conic_second_ptr->ellipse_parameters(cx2,cy2,phi2,width2,height2);
      if (first[i][j].is_real_ellipse()) {
        first[i][j].ellipse_parameters(cx1,cy1,phi1,width1,height1);
        /*if ((abs(cy2 - cy1 )<140)& (abs(width1 - width2)<15))
        {*/
            count++;
        double diff = sqrt((cx2 - cx1)*(cx2 - cx1) + 
          (cy2 - cy1)*(cy2 - cy1));
        fstream << k << " (" << cx1 << "," << cy1 << ") (" << cx2 << "," << cy2 << ")=" << diff << "--(" << (cx1-cx2) << "," << (cy1 - cy2) << ")" "\n";
      
        diff_vector[k++] = diff; 
      //  vcl_cout<<" difference vector 1 "<< k <<" "<<diff<<vcl_endl;
     
        double rad_diff = sqrt((width2 - width1)*(width2 - width1) + 
          (height2 - height1)*(height2 - height1));
        //fstream << k << " (" <<  << "," << ellipse_radius << ") = " << rad_diff << "\n";
        fstream << k << " (" << width1 << "," << height1 << ") (" << width2 << "," << height2 << ")=" << rad_diff << "--(" << (width1-width2) << "," << (height1 - height2) << ")" "\n";
       
        diff_vector[k++] = rad_diff;
       //  vcl_cout<<" difference vector 2 "<< k <<" "<<rad_diff<<vcl_endl;
      //}
     }
      else {
        vcl_cout << "Not a real ellipse!" << vcl_endl;
        vcl_cout << first[i][j] << vcl_endl;
        // to ignore the correspondence, place 0 for the differences,
        // one for the center difference, one for the width, height difference
        diff_vector[k++] = 0;
        diff_vector[k++] = 0;
      }
 }
    }
    // if the number of correspondences smaller than the preset value, place 
    // zero for the difference values, one for the center difference, one for 
    // the width, height difference
    for (; count < par_.NUM_OF_CORRESP; count++) {
         fstream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" "\n";
        diff_vector[k++]=0.0;
         fstream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" "\n";
      diff_vector[k++]=0.0;
    }
    
  }
  

}

#endif






                                             
                                             

