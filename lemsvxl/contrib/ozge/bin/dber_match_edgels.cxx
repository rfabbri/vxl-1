// compare pairs of observations

#include <ctime>
#include <algorithm>
#include <iostream>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_box_2d.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_convolve_2d.h>
#include <vil/vil_new.h>
#include <vnl/vnl_math.h>

#include <vul/vul_timer.h>
#include <bsol/bsol_algs.h>

#include <dber/dber_match.h>
#include <sdet/sdet_nonmax_suppression_params.h>
#include <sdet/sdet_nonmax_suppression.h>

//#include <dbcvr/dbcvr_cv_cor.h>

#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_observation.h>
//#include <dbinfo/dbinfo_observation_matcher.h>

vsol_polygon_2d_sptr read_con_from_file(std::string fname) {
  double x, y;
  char buffer[2000];
  int nPoints;

  std::vector<vsol_point_2d_sptr> inp;
  inp.clear();

  std::ifstream fp(fname.c_str());
  if (!fp) {
    std::cout<<" Unable to Open "<< fname <<std::endl;
    return 0;
  }
  //2)Read in file header.
  fp.getline(buffer,2000); //CONTOUR
  fp.getline(buffer,2000); //OPEN/CLOSE flag (not important, we assume close)
  fp >> nPoints;
#if 0
  std::cout << "Number of Points from Contour: " << nPoints << std::endl;
#endif     
  for (int i=0;i<nPoints;i++) {
    fp >> x >> y;
    vsol_point_2d_sptr vs = new vsol_point_2d(x, y);
    inp.push_back(vs);
  }
  fp.close();
  vsol_polygon_2d_sptr poly = new vsol_polygon_2d(inp);
  return poly;
}

int main(int argc, char *argv[]) {
  std::cout << "Matching Edgel Sets!\n";

  //: out file contains the wrong mathces if any
  std::string database_list, images_dir, cons_dir, out_file, output_dir;

  std::cout << "argc: " << argc << std::endl;
  if (argc != 10) {
    std::cout << "Usage: <program name> <database_list> <images_dir> <cons_dir> <size> <additional_string> <gradient_mag_threshold> <sigma_square> <matching_threhsold> <outfile name> <output_dir>\n";
    std::cout << "<size> is 032, 064, 128 or 256, it is to be added to create output directory names when necessary\n";
    std::cout << "<additional string> is to be added to end of each name in database list, e.g. 055-135-068-030\n";
    std::cout << "sigma square is used current-matching norm, which is used to find correspondences among edgels\n";
    std::cout << "matching threshold is used to eliminate low similarity edgel matchings, typical 0.2\n";
    return -1;
  }

  database_list = argv[1];
  images_dir = argv[2];
  cons_dir = argv[3];
  int size = atoi(argv[4]);
  std::string addition = argv[5];
  std::cout << "additional string read: " << addition << "\n";

  double gradient_mag_threshold = atof(argv[6]);
  //double sigma_ratio = atof(argv[7]);
  double sigma_square = atof(argv[7]);
  double matching_threshold = atof(argv[8]);
  out_file = std::string(argv[9])+"_"+std::string(argv[4])+"_";

  output_dir = ".\\" + std::string(argv[4]) + "-" + addition + "\\";

  std::vector<std::string> database; 
  std::ifstream fpd((database_list).c_str());
  if (!fpd.is_open()) {
    std::cout << "Unable to open database file!\n";
    return -1;
  }

  char buffer[1000];
  while (!fpd.eof()) {
    std::string temp;
    fpd.getline(buffer, 1000);
    temp = buffer;
    if (temp.size() > 1) {
      std::cout << "temp: " << temp << " addition: " << addition << std::endl;
      database.push_back(temp+"_"+addition);
    }
  }
  fpd.close();
  
  unsigned int D = database.size(); 
  std::cout << " D: " << D << "\n";
#if 1
  std::cout << "printing database list: \n";
  for (unsigned int i = 0; i<D; i++) {
    std::cout << database[i] << "\n";
  }
#endif

  std::string dummy = addition;
  char *pch = std::strtok ((char *)dummy.c_str(),"-");
  while (pch != NULL)
  {
    printf ("%s\n",pch);
    out_file = out_file + pch + "_";
    pch = strtok (NULL, "-");
  }
  out_file = out_file+"edgel_"+argv[6]+"_"+argv[7]+"_"+argv[8]+".out";
  std::cout << "out_file: " << out_file << std::endl;
  std::string dump_file = "dump_results_" + out_file;
  std::cout << "dump_file: " << dump_file << std::endl;

  //: load images
  std::vector<vil_image_resource_sptr> database_images;
  for (unsigned int i = 0; i<D; i++) {
    std::string image_file = images_dir + database[i] + ".png";
    vil_image_resource_sptr image_r = vil_load_image_resource( image_file.c_str() );
    database_images.push_back(image_r);
  }
  std::cout << "loaded images\n";

  //: load cons
  std::vector<vsol_polygon_2d_sptr> database_polygons;
  for (unsigned int i = 0; i<D; i++) {
    std::string con_file = cons_dir + database[i] + ".con";
    vsol_polygon_2d_sptr poly = read_con_from_file(con_file.c_str());
    database_polygons.push_back(poly);
  }
  //: prepare the observations
  std::vector<dbinfo_observation_sptr> database_obs;
  for (unsigned i = 0; i<D; i++) {
    dbinfo_observation_sptr obs = new dbinfo_observation(0, database_images[i], database_polygons[i], true, true, false);
    database_obs.push_back(obs);
  }

  //: prepare the edgels
  std::vector< std::vector<vsol_line_2d_sptr> > database_edgels;
  std::vector< vsol_box_2d_sptr> database_boxes;
  for (unsigned int i = 0; i<D; i++) {
    vil_image_resource_sptr image_r = database_images[i];
    //convert to grayscale
    vil_image_view<vxl_byte> image_view = image_r->get_view(0, image_r->ni(), 0, image_r->nj() );
    vil_image_view<vxl_byte> greyscale_view;
    if(image_view.nplanes() == 3){
      vil_convert_planes_to_grey(image_view, greyscale_view );
    }
    else if (image_view.nplanes() == 1){
      greyscale_view = image_view;
    }

    //compute image gradients before performing nonmax suppression
    vil_image_view<double> grad_x, grad_y;
    // create Gaussian kernel
    vil_image_view<double> gauss_x, gauss_y;
    // kernel sizes should be chosen according to the Gaussian sigma
    double sigma = 1.0f;
    int khs = (int)std::ceil(3*sigma); //kernel half size
    int ks = 2*khs+1; //kernel full size
    gauss_x.set_size(ks,ks);
    gauss_y.set_size(ks,ks);
    double ssq = sigma*sigma;
    for(int y = -khs; y <= khs; y++){
      for(int x = -khs; x <= khs; x++){
        double c = std::exp(-(x*x+y*y)/(2*ssq))/(vnl_math::pi*ssq);
        gauss_x(x+khs,y+khs) = -c*x;
        gauss_y(x+khs,y+khs) = -c*y;
      }
    }

    // create a padded image first (because 2d conv reduces the size)
    vil_image_resource_sptr padded_res = vil_new_image_resource(greyscale_view.ni()+gauss_x.ni()-1, 
                                                                  greyscale_view.nj()+gauss_x.nj()-1, 
                                                                  1, VIL_PIXEL_FORMAT_BYTE);
    vil_image_view<vxl_byte> padded_img = padded_res->get_view();
    padded_img.fill(0);
    padded_res->put_view(padded_img,0,0);
    padded_res->put_view(greyscale_view,khs,khs);

    //: convolve the image with the kernels to get gradients in x and y directions
    vil_convolve_2d(padded_img, grad_x, gauss_x, double());
    vil_convolve_2d(padded_img, grad_y, gauss_y, double());

    //Now call the nms code to get the subpixel edge geometry
    //init the params class
    static sdet_nonmax_suppression_params nsp;
    nsp.thresh_= gradient_mag_threshold;
    nsp.pfit_type_ = 0;

    sdet_nonmax_suppression ns(nsp, grad_x, grad_y);
    ns.apply();
  
    //get the edgels
    std::vector< vsol_line_2d_sptr > edgels;
    //mask the edgels using the polygon
    vsol_polygon_2d_sptr poly = database_polygons[i];
    vgl_polygon<double> polyg = bsol_algs::vgl_from_poly(poly);
    int cnt = 0, cnt_elim = 0;
    for (unsigned k=0; k<ns.get_lines().size(); k++) {
      vsol_line_2d_sptr line = ns.get_lines()[k];
      cnt++;
      if (polyg.contains(line->middle()->get_p()))
        edgels.push_back(line);
      else
        cnt_elim++;
    }
    std::cout << "for image: " << i << " " << cnt << " edgels found, " << cnt_elim << " are eliminated\n";
    database_edgels.push_back(edgels);
    vsol_box_2d_sptr box = dber_match::get_box(edgels);
    database_boxes.push_back(box);
  }

  std::cout << "loaded contours and prepared masked edgels..\n";
  vbl_array_2d<double> matching_costs(D, D, 100000);
  
  unsigned int i_start = 0, j_start = 1;
  std::ifstream if3;

  if3.open(dump_file.c_str());
  if (if3) {
    int dummy; bool tag = false;
    if3 >> dummy;  // read first i
    for (unsigned int i = 0; i<D; i++) {
      i_start = i;
      for (unsigned int j = i+1; j < D; j++) {
        j_start = j;
        if (if3.eof()) { tag = true; break; }  
        if3 >> dummy;   // read j
        if3 >> matching_costs[i][j];
        matching_costs[j][i] = matching_costs[i][j];
        if3 >> dummy;  // try reading i again, if not then it'll break 
      }
      if (tag) break;
    }
    if3.close();
  }
  
  std::cout << "i_start: " << i_start << " j_start: " << j_start << std::endl;
  std::ofstream of3;

  of3.open(dump_file.c_str());
  
  for (unsigned int i = 0; i<=i_start; i++)
    for (unsigned int j = i+1; j < (i == i_start?j_start:D); j++) {
      of3 << i << " " << j << " " << matching_costs[i][j] << "\n";
    }
  of3.close();

  for (unsigned int i = i_start; i<D; i++) {
    unsigned int j = (i == i_start ? j_start : i+1);
    for ( ; j<D; j++) {
      vul_timer t;
      t.mark();
      
      dber_match matcher;
      matcher.set_lines1(database_edgels[i]);
      matcher.set_lines2(database_edgels[j]);
      //std::vector<vsol_line_2d_sptr> & l1 = database_edgels[i];
      //std::vector<vsol_line_2d_sptr> & l2 = database_edgels[j];
      
      //: radius is the sigma_square in the current_matching norm,
      //  edge correspondences are found using this norm
      matcher.set_radius(sigma_square);
      matcher.set_width_radius((database_boxes[i]->width() + database_boxes[i]->height())/4.0f);
  
      double scale_factor = database_boxes[i]->width()/database_boxes[j]->width();
      matcher.set_scale_factor(scale_factor);
      
      if (i == 6 && j == 7)
        std::cout << "now!\n";
      matcher.match_greedy(matching_threshold);
                                                                          // if true: poor affine, otherwise uses TPS
      //double mi = matcher.find_global_mi(database_obs[i], database_obs[j], false);
      

      double mi = matcher.find_global_mi(database_obs[i], database_obs[j], true);
      std::cout << "overall mutual information: " << mi << std::endl;
      //if (mi != 0) {
      vil_image_resource_sptr correspondence_im = matcher.get_correspondence_image();
      vil_save_image_resource(correspondence_im, (output_dir+database[i]+"_"+database[j]+".png").c_str()); 
      //}
      
      /*
      dber_match::scale_lines(l2, scale_factor);
      
      vsol_point_2d_sptr gc1 = dber_match::center_of_gravity(l1);
      vsol_point_2d_sptr gc2 = dber_match::center_of_gravity(l2);
      dber_match::translate_lines(l2, gc1->x()-gc2->x(), gc1->y()-gc2->y());

      double sigma = (database_boxes[i]->width() + database_boxes[i]->height())*sigma_ratio;
      double cost1 = dber_match::measure_support(l1, l2, sigma*sigma);
      */

      matching_costs[i][j] = mi;
      matching_costs[j][i] = mi;
      
      std::cout << "i: " << i << " j: " << j << " mi " << mi << " time: "<< t.real()/1000.0f << "\n";
      
      of3.open(dump_file.c_str(), std::ofstream::app);
      of3 << i << " " << j << " " << matching_costs[i][j] << "\n";
      of3.close();
   }
  }

  std::ofstream of((out_file).c_str());
  of << "\n edgel norms: \n" << D << " " << D << "\n";
  for (unsigned i = 0; i<D; i++) {
    for (unsigned j = 0; j<D; j++)
      of << matching_costs[i][j] << " ";
    of << "\n";
  }

  of.close();

  return 0;
}
