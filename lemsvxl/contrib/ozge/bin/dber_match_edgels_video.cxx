// compare pairs of observations

#include <vcl_ctime.h>
#include <vcl_cstdlib.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

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

#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>

#include <dber/dber_match.h>
#include <sdet/sdet_nonmax_suppression_params.h>
#include <sdet/sdet_nonmax_suppression.h>

//#include <dbcvr/dbcvr_cv_cor.h>

#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_object_matcher.h>
//#include <dbinfo/dbinfo_observation_matcher.h>

#include <georegister/dbrl_multiple_instance_object.h>
#include <georegister/dbrl_multiple_instance_object_sptr.h>

#include <vgl/vgl_distance.h>

vsol_polygon_2d_sptr read_con_from_file(vcl_string fname) {
  double x, y;
  char buffer[2000];
  int nPoints;

  vcl_vector<vsol_point_2d_sptr> inp;
  inp.clear();

  vcl_ifstream fp(fname.c_str());
  if (!fp) {
    vcl_cout<<" Unable to Open "<< fname <<vcl_endl;
    return 0;
  }
  //2)Read in file header.
  fp.getline(buffer,2000); //CONTOUR
  fp.getline(buffer,2000); //OPEN/CLOSE flag (not important, we assume close)
  fp >> nPoints;
#if 0
  vcl_cout << "Number of Points from Contour: " << nPoints << vcl_endl;
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

void read_file(const char *filename, vcl_vector< vcl_vector<dbrl_multiple_instance_object_sptr> >& frames)
{
 //   filename="D:\\Lockheed_Deliveries\\Nov_03_06\\280-203-303-0-20multi.dat";
    vsl_b_ifstream ifile(filename);
    unsigned numframes=0;
    vsl_b_read(ifile,numframes);
    for(unsigned i=0;i<numframes;i++)
    {
        vcl_vector<dbrl_multiple_instance_object_sptr> temp;
        vsl_b_read(ifile,temp);
        vcl_cout << "frame: " << i << " size: " << temp.size() << vcl_endl;
        frames.push_back(temp);
    }
}

void write_file(const char *filename, vcl_vector< vcl_vector<dbrl_multiple_instance_object_sptr> >& frames)
{
    vsl_b_ofstream ofile(filename);
    unsigned numframes=frames.size();
    vsl_b_write(ofile,numframes);
    for(unsigned i=0;i<numframes;i++)
      vsl_b_write(ofile,frames[i]);
}

int main(int argc, char *argv[]) {
  vcl_cout << "Comparing Edgel Sets of video to syntethic database edgel sets!\n";

  //: out file contains the wrong mathces if any
  vcl_string query_vfile, database_vfiles, out_file, output_dir, output_video_file;

  vcl_cout << "argc: " << argc << vcl_endl;
  if (argc != 11) {
    vcl_cout << "Usage: <program name> <selfradius> <sigma_square> <matching_threhsold> <query video file name> <database video files> <motion bin> <every_k> <out file> <output_video_file> <output_dir>\n";
    vcl_cout << "selfradius is the distance between edgels to be considered as self similar, typical value: 0.5\n";
    vcl_cout << "sigma square is used current-matching norm, which is used to find correspondences among edgels\n";
    vcl_cout << "matching threshold is used to eliminate low similarity edgel matchings, typical 0.2\n";
    vcl_cout << "take every_k frames from the database, if every_k = 1, uses all frames in the database\n";
    return -1;
  }

  double selfradius = atof(argv[1]);
  double sigma_square = atof(argv[2]);
  double matching_threshold = atof(argv[3]);
  query_vfile = vcl_string(argv[4]);
  database_vfiles = vcl_string(argv[5]);
  int motion_bin = atoi(argv[6]);
  unsigned every_k = unsigned(atoi(argv[7]));
  
  out_file = vcl_string(argv[8])+"_"+vcl_string(argv[1])+"_"+vcl_string(argv[2])+"_"+vcl_string(argv[5])+".out";
  output_video_file = argv[9];
  output_dir = argv[10];

  vcl_vector< vcl_vector<dbrl_multiple_instance_object_sptr> > q_orig, q_frames, db_frames, db_orig, classified_q_frames;
  
  read_file(query_vfile.c_str(), q_orig);
  vcl_cout << "number of query frames: " << q_orig.size() << vcl_endl;

  vcl_vector<vcl_string> database; 
  vcl_ifstream fpd((database_vfiles).c_str());
  if (!fpd.is_open()) {
    vcl_cout << "Unable to open database file!\n";
    return -1;
  }

  char buffer[1000];
  while (!fpd.eof()) {
    vcl_string temp;
    fpd.getline(buffer, 1000);
    temp = buffer;
    if (temp.size() > 1) {
      vcl_cout << "temp: " << temp << vcl_endl;
      database.push_back(temp);
    }
  }
  fpd.close();

  for (unsigned i = 0; i<database.size(); i++) {
    read_file(database[i].c_str(), db_orig);
  }
  vcl_cout << "number of database frames: " << db_orig.size() << vcl_endl;
  
  vcl_cout << "translating edgels....";
  dbru_label_sptr null_label = new dbru_label();
  // translate edgels to chipped image coordinates before matching
  for (unsigned k = 0; k<q_orig.size(); k++) {
    vcl_vector<dbrl_multiple_instance_object_sptr> qq;
    for (unsigned m = 0; m<q_orig[k].size(); m++) {
      if (!q_orig[k][m]->get_label())
        q_orig[k][m]->set_label(null_label);

      dbrl_multiple_instance_object_sptr o = new dbrl_multiple_instance_object(*q_orig[k][m]);
      
      vsol_box_2d_sptr ibox = o->get_img_box();
      double x_min = ibox->get_min_x();
      double y_min = ibox->get_min_y();
      vgl_vector_2d<double> trans(-x_min, -y_min);

      vcl_vector<vsol_line_2d_sptr> edgels = o->get_edges();
      for (unsigned i = 0; i<edgels.size(); i++) {
        vsol_line_2d_sptr ll = edgels[i];
        ll->p0()->add_vector(trans);
        ll->p1()->add_vector(trans);
      }

      //eliminate extra edgels within 1 pixel
      vcl_vector<bool> eliminate(edgels.size(), false);
      for (unsigned i = 0; i<edgels.size(); i++) {
        if (eliminate[i]) continue;
        vsol_point_2d_sptr mi = edgels[i]->middle();
        for (unsigned j = i+1; j<edgels.size(); j++) {
          if (eliminate[j]) continue;
          vsol_point_2d_sptr mj = edgels[j]->middle();
          if (vgl_distance(mi->get_p(), mj->get_p()) < selfradius)
            eliminate[j] = true;
        }
      }
      int cnt = 0; 
      vcl_vector<vsol_line_2d_sptr> newedgels;
      for (unsigned i = 0; i<edgels.size(); i++) {
        if (eliminate[i]) {
          cnt++;
          continue;
        }
        newedgels.push_back(edgels[i]);
      }
      vcl_cout << "eliminated: " << cnt << " out of " << edgels.size() << " due to self_similarity\n";
          
      vsol_polygon_2d_sptr poly = o->get_poly();
      for (unsigned i = 0; i<poly->size(); i++) {
        vsol_point_2d_sptr p = poly->vertex(i);
        p->add_vector(trans);
      }

      if (!o->get_label())
        o->set_label(null_label);

      dbrl_multiple_instance_object_sptr oo = new dbrl_multiple_instance_object(*o);
      oo->set_edges(newedgels);

      qq.push_back(oo);
    }
    q_frames.push_back(qq);
  }

  // translate edgels to chipped image coordinates before matching
  for (unsigned k = 0; k<db_orig.size(); k++) {
    vcl_vector<dbrl_multiple_instance_object_sptr> qq;
    for (unsigned m = 0; m<db_orig[k].size(); m++) {
      if (!db_orig[k][m]->get_label())
        db_orig[k][m]->set_label(null_label);

      dbrl_multiple_instance_object_sptr o = new dbrl_multiple_instance_object(*db_orig[k][m]);
      
      vsol_box_2d_sptr ibox = o->get_img_box();
      double x_min = ibox->get_min_x();
      double y_min = ibox->get_min_y();
      vgl_vector_2d<double> trans(-x_min, -y_min);

      vcl_vector<vsol_line_2d_sptr> edgels = o->get_edges();
      for (unsigned i = 0; i<edgels.size(); i++) {
        vsol_line_2d_sptr ll = edgels[i];
        ll->p0()->add_vector(trans);
        ll->p1()->add_vector(trans);
      }

      //eliminate extra edgels within 1 pixel
      vcl_vector<bool> eliminate(edgels.size(), false);
      for (unsigned i = 0; i<edgels.size(); i++) {
        if (eliminate[i]) continue;
        vsol_point_2d_sptr mi = edgels[i]->middle();
        for (unsigned j = i+1; j<edgels.size(); j++) {
          if (eliminate[j]) continue;
          vsol_point_2d_sptr mj = edgels[j]->middle();
          if (vgl_distance(mi->get_p(), mj->get_p()) < selfradius)
            eliminate[j] = true;
        }
      }
      int cnt = 0; 
      vcl_vector<vsol_line_2d_sptr> newedgels;
      for (unsigned i = 0; i<edgels.size(); i++) {
        if (eliminate[i]) {
          cnt++;
          continue;
        }
        newedgels.push_back(edgels[i]);
      }
      vcl_cout << "eliminated: " << cnt << " out of " << edgels.size() << " due to self_similarity\n";
          
      
      vsol_polygon_2d_sptr poly = o->get_poly();
      for (unsigned i = 0; i<poly->size(); i++) {
        vsol_point_2d_sptr p = poly->vertex(i);
        p->add_vector(trans);
      }

      if (!o->get_label())
        o->set_label(null_label);

      dbrl_multiple_instance_object_sptr oo = new dbrl_multiple_instance_object(*o);
      oo->set_edges(newedgels);

      qq.push_back(oo);
    }
    db_frames.push_back(qq);
  }
  vcl_cout << "done!!\n";

  //: recognize each instance in each frame
  vul_timer t;
  int cnt = 0, cnt_correct = 0;
  vcl_string null("null");
  for (unsigned k = 0; k<q_frames.size(); k++) {
    vcl_vector<dbrl_multiple_instance_object_sptr> classified_instances;
    
    for (unsigned m = 1; m<q_frames[k].size() ; m++) {
      dbrl_multiple_instance_object_sptr o = q_frames[k][m];
      
      dbru_label_sptr q_label = o->get_label();
      if (q_label->category() == null) continue;

      vcl_cout << "frame: " << k << " instance: " << m << " # of edgels: " << o->get_edges().size() << vcl_endl;
      vcl_vector<vsol_line_2d_sptr> edgels = o->get_edges();
      vsol_polygon_2d_sptr poly = o->get_poly();

      char frame_k[1000];
      char instance_m[1000];
      itoa(k, frame_k, 10);
      itoa(m, instance_m, 10);

      vcl_vector<dbinfo_observation_sptr> o_obs;
      for (unsigned i = 0; i<o->imgs_size(); i++) {
        vil_image_view<vxl_byte> img = o->get_image_i(i);
        vil_image_resource_sptr img_r = vil_new_image_resource_of_view(img);
        dbinfo_observation_sptr obs = new dbinfo_observation(0, img_r, poly, true, true, false);
        o_obs.push_back(obs);
      }

      dber_match matcher;
      //: radius is the sigma_square in the current_matching norm,
      //  edge correspondences are found using this norm
      matcher.set_radius(sigma_square);
      matcher.match_greedy(matching_threshold);
      matcher.set_lines1(edgels);
      matcher.set_poly1(poly);

      double max_info = 0;
      unsigned max_dk = 0;
      unsigned max_dm = 0;
      // run against all instances in the database
      int cnt_compared = 0;
      for (unsigned dk = 0; dk < db_frames.size(); dk += every_k)
        for (unsigned dm = 0; dm < db_frames[dk].size(); dm++) {
          t.mark();
          dbrl_multiple_instance_object_sptr db_o = db_frames[dk][dm];
          dbru_label_sptr db_label = db_o->get_label();
          if (db_label->category() == null) continue;
          if (db_label->motion_orientation_bin_ != q_label->motion_orientation_bin_) continue;

          vcl_cout << "\tdb frame: " << dk << " instance: " << dm << " # of edgels: " << db_o->get_edges().size() << "\n";
          
          vsol_polygon_2d_sptr db_poly = o->get_poly();
          matcher.set_lines2(db_o->get_edges());
          matcher.set_poly2(db_poly);
          matcher.match_greedy(matching_threshold);
          if (!matcher.find_tps(false)) {  // if true affine only
            continue;
          }

          char frame_dk[1000];
          char instance_dm[1000];
          itoa(dk, frame_dk, 10);
          itoa(dm, instance_dm, 10);

          vcl_vector<dbinfo_observation_sptr> db_o_obs;
          for (unsigned i = 0; i<db_o->imgs_size(); i++) {
            vil_image_view<vxl_byte> img = db_o->get_image_i(i);
            vil_image_resource_sptr img_r = vil_new_image_resource_of_view(img);
            dbinfo_observation_sptr obs = new dbinfo_observation(0, img_r, db_poly, true, true, false);
            db_o_obs.push_back(obs);
          }

          double mi = 0;
          for (unsigned i = 0; i < db_o_obs.size(); i++) 
            for (unsigned j = 0; j < o_obs.size(); j++) 
              mi += matcher.find_global_mi(o_obs[j], db_o_obs[i]);    
            
          vil_image_resource_sptr correspondence_im = matcher.get_correspondence_image();        
          vil_save_image_resource(correspondence_im, (output_dir+"q_"+frame_k+"_"+instance_m+"_db_"+frame_dk+"_"+instance_dm+".png").c_str()); 
          vcl_cout << "\t----------------- overall mutual info: " << mi << " time: " << t.real()/1000.0f << vcl_endl;
          
          if (mi > max_info) {
            max_info = mi; max_dk = dk; max_dm = dm;
          }

          cnt_compared++;
          matcher.clear_assignment();
          matcher.clear_lines2();
        }

      cnt++;
      dbrl_multiple_instance_object_sptr db_best_o = db_frames[max_dk][max_dm];
      if (db_best_o->get_label()->category() == q_label->category()) {
        cnt_correct++;
        vcl_cout << "------- compared to " << cnt_compared << " db items, classification CORRECT\n";
      }

      q_orig[k][m]->set_label(db_best_o->get_label());
      classified_instances.push_back(q_orig[k][m]);
      
      matcher.clear_lines1();
      
    }
    classified_q_frames.push_back(classified_instances);
  }

  vcl_ofstream of((out_file).c_str());
  of << "total query items: " << cnt << " number of corrects: " << cnt_correct << " recognition rate: " << (cnt_correct/cnt)*100 << vcl_endl; 
  of.close();

  vcl_cout << "total query items: " << cnt << " number of corrects: " << cnt_correct << " recognition rate: " << (cnt_correct/cnt)*100 << vcl_endl; 
  write_file(output_video_file.c_str(), classified_q_frames);
  return 0;
}



/*  vil_image_resource_sptr image_q, image_rdb, image_match;
        // first rigid alignment by maximizing mutual info
        //: ratio is 0.01, so try to alignm cog of database within 0.01*radius of query observation
        //  dr: 15 so try random rotations between -15 and +15 degrees
        //  ds: 0
        vcl_cout << "query diameter: " << o_obs[0]->geometry()->diameter() << vcl_endl;
        vcl_cout << "database diameter: " << database_obs[i]->geometry()->diameter() << vcl_endl;
        float forced_scale = o_obs[0]->geometry()->diameter()/database_obs[i]->geometry()->diameter();
        vgl_h_matrix_2d<float> best_trans;
        double mi = dbinfo_object_matcher::minfo_rigid_alignment_rand(o_obs[0], database_obs[i], best_trans,
                                                                      0, 15*vnl_math::pi/180.0f, 1, 0.1, 10,
                                                                      image_q, image_rdb, image_match, true, forced_scale, true, true);
                                                                      // use intensity, use gradient

        vil_save_image_resource(image_q, (output_dir+database[i]+"_frame_"+frame_k+"_instance_"+"_"+instance_m+"_view_0_image_q"+".png").c_str()); 
        vil_save_image_resource(image_rdb, (output_dir+database[i]+"_frame_"+frame_k+"_instance_"+"_"+instance_m+"_view_0_image_rdb"+".png").c_str()); 
        vil_save_image_resource(image_match, (output_dir+database[i]+"_frame_"+frame_k+"_instance_"+"_"+instance_m+"_view_0_image_match"+".png").c_str()); 
        */

/*
  int cat_id[] = {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}; 
  //vcl_vector<int> cat_id(cat_id_f);
  vcl_vector<vcl_string> cats;
  cats.push_back("car");
  cats.push_back("minivan");
  cats.push_back("pickup");
  vcl_vector<vcl_string> cat;
  for (unsigned i = 0; i<21; i++) {
    cat.push_back(cats[cat_id[i]]);
  }
  for (unsigned k = 0; k<frames_infos.size(); k++) {
    vcl_vector< vcl_vector<double> >& instances_infos = frames_infos[k];
    for (unsigned m = 0; m<instances_infos.size() ; m++) {
      //find best db item
      double max_mi = 0;
      unsigned max_i = 0;
      vcl_vector<double>& infos = instances_infos[m];
      for (unsigned i = 0; i<database.size(); i++)
        if (infos[i] > max_mi) {
          max_mi = infos[i];
          max_i = i;
        }
      dbrl_multiple_instance_object_sptr o = frames[k][m];
      dbru_label_sptr label = new dbru_label(cat[max_i], -1, -1, -1, -1);
      o->set_label(label);
    }
  }
  */
