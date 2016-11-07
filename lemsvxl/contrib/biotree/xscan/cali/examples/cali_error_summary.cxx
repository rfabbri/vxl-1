#include <xscan/xscan_dummy_scan.h>
#include <xscan/xscan_uniform_orbit.h> 
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_memory_image.h>
#include <vil/vil_new.h>
#include <cali/cali_simulated_scan_resource.h>
#include <cali/cali_vnl_least_squares_function.h>
#include <cali/cali_param.h>
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vcl_algorithm.h>
#include <vcl_cstdlib.h>
#include <imgr/file_formats/imgr_skyscan_log_header.h>
#include <imgr/file_formats/imgr_skyscan_log.h>

void newverbose_error_summary(vcl_ofstream& fstream,
                              cali_param par_,
                              vcl_vector<vcl_vector<vsol_conic_2d> > first, 
                              conic_vector_set second, 
                              vnl_vector<double> &diff_vector,
                              vcl_vector<vcl_vector<double> > &ball_diffs_center,
                              vcl_vector<vcl_vector<double> > &ball_diffs_rad) 
{
        ball_diffs_center.resize(par_.BALL_NUMBER); 
        ball_diffs_rad.resize(par_.BALL_NUMBER); 
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
                                                        ball_diffs_center[first_ball].push_back(diff);

                                                        double rad_diff = sqrt((width2 - width1)*(width2 - width1) + 
                                                                        (height2 - height1)*(height2 - height1));

                                                        fstream << k << " (" << width1 << "," << height1 << ") (" << width2 << "," << height2 << ")=" << rad_diff << "--(" << (width1-width2) << "," << (height1 - height2) << ") img: " << i << "(" << cx1 << "," << cy1 << ") RAD, ball " << first_ball +1<< "\n";
                                                        diff_vector[k++] = rad_diff; 
                                                        ball_diffs_rad[first_ball].push_back(rad_diff);
                                                        vcl_cout << "\t" << k << " differences; new diffs: dcenter = " << diff << " drad = " << rad_diff << "\n";
                                                }
                                                //otherwise, put zeroes
                                                else
                                                {
                                                        vcl_cout<<" assigning zeroes to ignore the correspondence as it is a mismatch " <<vcl_endl;
                                                        fstream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" 
                                                        << ") img: " << i << "(" << cx1 << "," << cy1 << ")\n";

                                                        diff_vector[k++] = 10;
                                                        ball_diffs_center[first_ball].push_back(0);
                                                        fstream << k << " (" << 0<< "," << 0 << ") (" << 0 << "," << 0 << ")=" << 0 << "--(" << 0 << "," << 0 << ")" 
                                                        << ") img: " << i << "(" << cx1 << "," << cy1 << ")\n";
                                                        diff_vector[k++] = 10;
                                                        ball_diffs_rad[first_ball].push_back(0);
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
int main(int argc, char* argv[]) {
  if(argc < 2){
    vcl_cerr << "usage " << argv[0] << " <parameter file>\n";
    exit(1);
  }
  vcl_string path = argv[1];
  cali_param par(path);
  vcl_string fname = par.LOGFILE;
  vcl_FILE *fp = vcl_fopen(fname.data(),"r");
  assert(fp != NULL);
  imgr_skyscan_log_header skyscan_log_header(fp);
  vcl_fclose(fp);

  imgr_skyscan_log skyscan_log(fname.data());
  xscan_scan scan;
  scan = skyscan_log.get_scan();
  vcl_cout << scan << vcl_endl;

  scan.set_scan_size(par.END - par.START + 1);

  int nviews = scan.scan_size();
  // change the scan
  vpgl_calibration_matrix<double> kk(scan.kk());
  vcl_cout << "scan SIZE---->" << nviews << vcl_endl;
  vcl_vector<vil_image_resource_sptr> resources(nviews);
  for (int i=0; i<nviews; i++) {
       resources[i] = vil_new_image_resource(skyscan_log_header.number_of_columns_,skyscan_log_header.number_of_rows_,1,  VIL_PIXEL_FORMAT_BYTE);
    }




  vnl_quaternion<double> artf_rot; 
  vgl_point_3d<double> artf_trans;
  vnl_quaternion<double> turn_table_rot;
  vnl_double_3 turn_table_trans;
  double x_scale;
  double y_scale;
  //unused variable double xy_scale;
  vgl_point_2d<double> princp_point;


  // read the x values from the text file
  vcl_string txt_file = par.CONVERGEDVALUES;
  vcl_cout << "reading in values from " <<  txt_file << "\n";
  vcl_ifstream fstream(txt_file.c_str(),vcl_ios::in);
  vnl_vector<double> x(par.SIZE_OF_X);

  double val;
  char val_string[256];
  int x_size=0;
  while (!fstream.eof()) {
    fstream.getline(val_string, 256);
    val = vcl_atof(val_string);
    x[x_size++] = val;
    vcl_cout << x_size << "=" << val << vcl_endl;
  }

  cali_vnl_least_squares_function::gen_scan_lsqr_fn_params(x, artf_rot,
      artf_trans,turn_table_rot,turn_table_trans,                     
      x_scale,y_scale,princp_point);

  kk.set_x_scale(x_scale);
  kk.set_y_scale(y_scale);
  kk.set_principal_point(princp_point);
  scan.set_calibration_matrix(kk);

  turn_table_rot.normalize();
  xscan_orbit_base_sptr orbit_base = (scan.orbit());
  xscan_uniform_orbit orbit = static_cast<const xscan_uniform_orbit&>(*orbit_base);
  xscan_orbit_base_sptr new_orbit = new xscan_uniform_orbit(orbit.rot(),
      orbit.t(),
      turn_table_rot,
      turn_table_trans);
  scan.set_orbit(new_orbit);


  vcl_vector<double>x_coord_tol;

  x_coord_tol.push_back(0);
  x_coord_tol.push_back(0);
  x_coord_tol.push_back(-0.0947263);
  x_coord_tol.push_back(0.0404068);
  x_coord_tol.push_back(0.050888);
  x_coord_tol.push_back(0.296285);
  x_coord_tol.push_back(0.286996);
  x_coord_tol.push_back(0.109258);
  x_coord_tol.push_back(0.125204);
  x_coord_tol.push_back(-0.103531);
  x_coord_tol.push_back(-0.0743285);
  x_coord_tol.push_back(0);
  x_coord_tol.push_back(0);

  vcl_vector<double>y_coord_tol;

  y_coord_tol.push_back(0);
  y_coord_tol.push_back(0);
  y_coord_tol.push_back(0.00245647);
  y_coord_tol.push_back(0.0188508);
  y_coord_tol.push_back(-0.183153);
  y_coord_tol.push_back(-0.376836);
  y_coord_tol.push_back(-0.182137);
  y_coord_tol.push_back(-0.164995);
  y_coord_tol.push_back(0.128703);
  y_coord_tol.push_back(0.243105);
  y_coord_tol.push_back(-0.00153744);
  y_coord_tol.push_back(0);
  y_coord_tol.push_back(0);

  vcl_vector<double>z_coord_tol;
  vcl_vector<double>rad_tol;

  for (int i = 0;i<13;i++)
  {
    z_coord_tol.push_back(0);
    rad_tol.push_back(0);
  }

  cali_artf_corresponder corr(par);
  vcl_vector<conic_vector_set> real_img_conics;
  vcl_string file_base = par.CONICS_BIN_FILE_BASE;
  vcl_cout << "file_base " << file_base << "\n";
  for (unsigned i=0; i<scan.n_views(); i+=par.INTERVAL) {
    vcl_string file_name = corr.gen_read_fname(file_base, i);
    vcl_cout << "reading conic file " << file_name << "\n";
    conic_vector_set saved_conics = corr.read_conics_bin(file_name);
    vcl_cout << "read " << saved_conics.size() << " conics\n";
    real_img_conics.push_back(saved_conics);
  }



  // cali_simulated_scan_resource scan_res = cali_simulated_scan_resource(scan,resources,par);
  cali_simulated_scan_resource scan_res = cali_simulated_scan_resource(scan,resources,par,x_coord_tol,y_coord_tol,z_coord_tol,rad_tol);

  vcl_cout << "Rotation---------------------------->" << vcl_endl;
  vcl_cout << artf_rot << vcl_endl;
  vcl_cout << "Translation------------------------->" << vcl_endl;
  vcl_cout << artf_trans << vcl_endl;

  vcl_vector<vcl_vector<vsol_conic_2d> > centers = scan_res.gen_ball_projections(artf_rot, 
      artf_trans,
      par.INTERVAL);

  vcl_vector<vcl_vector<vsol_conic_2d_sptr> > modified_real_conic_vector;
  modified_real_conic_vector.resize(real_img_conics.size());

  for(int i = 0; i < real_img_conics.size(); i++) {
    for(int j = 0; j < real_img_conics[i].size(); j++){
      modified_real_conic_vector[i].push_back(real_img_conics[i][j][0]);
    }
    vcl_sort(modified_real_conic_vector[i].begin(), 
        modified_real_conic_vector[i].end(), 
        cali_vnl_least_squares_function::y_coord_compare );

  }
  vnl_vector<double> fx(real_img_conics.size()*par.NUM_OF_CONICS*2,0);
  vcl_cerr << "set residual size to " << fx.size() << "\n";
  //fx is the vector of square-root of sum of squared differences between
  //center locations & width/heights.  quoted end error is the RMS of this
  //delta vector.  typically  of size near (num imgs)*(13 balls)*(2 measurements)

  //corr.verbose_error_summary(centers,modified_real_conic_vector,fx);
  //

  vcl_cerr << " par.INTERVAL = " << par.INTERVAL << "\n";
  vcl_cerr << "real_img_conics.size() =" << real_img_conics.size() << "\n";
  vcl_cerr << "centers.size() =" << centers.size() << "\n";
  vcl_ofstream out("diff.txt"); 
  vcl_vector<vcl_vector<double> > ball_diffs_center;
  vcl_vector<vcl_vector<double> > ball_diffs_rad;
 newverbose_error_summary(out,
                               par,
                              centers, 
                              modified_real_conic_vector, 
                              fx,
                              ball_diffs_center,
                              ball_diffs_rad);
 out.close();

  vcl_cout << "rms error of fx is " << fx.rms() << "\n";

  vcl_ofstream bdata("balldata.m"); 
  for(int i = 0; i < ball_diffs_center.size(); i++){
          vcl_cerr << "ball_diffs_center[" << i << "].size() = " << ball_diffs_center[i].size() << "\n";
          bdata << "ballc" << i+1 << " = ["; 
    for(int j = 0; j < ball_diffs_center[i].size(); j++){
    bdata << ball_diffs_center[i][j] << " ";
    }
    bdata << "];\n";
  }
  for(int i = 0; i < ball_diffs_rad.size(); i++){
    bdata << "ballr" << i+1 << " = ["; 
    for(int j = 0; j < ball_diffs_rad[i].size(); j++){
    bdata << ball_diffs_rad[i][j] << " ";
    }
    bdata << "];\n";
  }
  for(int i = 0; i < ball_diffs_center.size(); i++){
    bdata << "printf('mean(ballc" << i+1 << ") = \%f\\n',mean(ballc" << i+1 << "));\n";
  }
   for(int i = 0; i < ball_diffs_rad.size(); i++){
    bdata << "printf('mean(ballr" << i+1 << ") = \%f\\n',mean(ballr" << i+1 << "));\n";
  }
  for(int i = 0; i < ball_diffs_center.size(); i++){
    if(ball_diffs_center[i].size() > 0){
      bdata << "printf('" << i+1 << "\\n');\n";
      bdata << "plot(ballc" << i+1 << "); pause;\n";
  }
  }
   for(int i = 0; i < ball_diffs_rad.size(); i++){
    if(ball_diffs_rad[i].size() > 0){
      bdata << "printf('" << i+1 << "\\n');\n";
      bdata << "plot(ballr" << i+1 << "); pause;\n";
    }
  }
 
 
   bdata.close();


  vcl_cerr << "rms error of fx is " << fx.rms() << "\n";

  exit(0);
}

