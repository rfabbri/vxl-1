#include <vcl_iostream.h>
#include <vcl_string.h>
#include <bxml/bxml_read.h>
#include <params.h>
#include <bfrag2D.h>
#include <bfrag_curve.h>
#include <vul/vul_file_iterator.h>
#include <PuzzleSolving.h>

vcl_vector<vcl_string> FRAG_PATHS;
vcl_vector<int> FRAG_IDS;
vcl_string OUT_FNAME;
vcl_string FRAG_PAIRS_OUT_FNAME;
vcl_string COMMON_PARAMS_OUT_FNAME;

vcl_vector<bfrag2D *> the_frags;

void get_params(vcl_string input)
{
  // open input file
  vcl_ifstream in(input.c_str());
  // create xml reader
  dbxml2_stream_read reader;
  dbxml2_data_sptr data;
  unsigned int depth;
  // read parameters
  while(data = reader.next_element(in, depth))
  {
    dbxml2_element* elem = static_cast<dbxml2_element*>(data.ptr());
    vcl_string att_name;
    if(elem->name() == "data")
    {
      unsigned id_no = 0;
      while(1)
      {
        char id_name[10000];
        char path_name[10000];
        sprintf(id_name, "id%d", id_no);
        sprintf(path_name, "path%d", id_no);
        id_no++;
        vcl_string no;
        vcl_string path;
        elem->get_attribute(id_name, no);
        if(no != "")
        {
          FRAG_IDS.push_back(atoi(no.c_str()));
          elem->get_attribute(path_name, path);
          FRAG_PATHS.push_back(path);
        }
        else
          break;
      }
    }
    else if(elem->name() == "corner_finding_params")
    {
      elem->get_attribute("dist_step", DIST_STEP);
      elem->get_attribute("min_tan_turn", MIN_TAN_TURN);
      elem->get_attribute("vicinity", VICINITY);
    }
    else if(elem->name() == "sampling_params")
    {
      elem->get_attribute("fine_resample_ds", FINE_RESAMPLE_DS);
      elem->get_attribute("coarse_resample_ds", COARSE_RESAMPLE_DS);
    }
    else if(elem->name() == "smoothing_params")
    {
      elem->get_attribute("num_times", NUM_TIMES);
      elem->get_attribute("psi", PSI);
    }
    else if(elem->name() == "matching_params")
    {
      elem->get_attribute("angle_sim_thresh", ANGLE_SIM_THRESH);
      elem->get_attribute("diag_coef", DIAG_COEF);
      elem->get_attribute("diag_thresh_low", DIAG_THRESH_LOW);
      elem->get_attribute("dist_coef", DIST_COEF);
      elem->get_attribute("distance_thresh_1", DISTANCE_THRESH_1);
      elem->get_attribute("distance_thresh_2", DISTANCE_THRESH_2);
      elem->get_attribute("edge_coef", EDGE_COEF);
      elem->get_attribute("fine_sample_skip", FINE_SAMPLE_SKIP);
      elem->get_attribute("lam", LAM);
      elem->get_attribute("length_coef", LENGTH_COEF);
      elem->get_attribute("length_thresh_low", LENGTH_THRESH_LOW);
      elem->get_attribute("local_reg_iter_pair", LOCAL_REG_ITER_PAIR);
      elem->get_attribute("local_reg_thresh_high", LOCAL_REG_THRESH_HIGH);
      elem->get_attribute("local_reg_thresh_low", LOCAL_REG_THRESH_LOW);
      elem->get_attribute("max_fine_cost", MAX_FINE_COST);
      elem->get_attribute("min_d", MIN_D);
      elem->get_attribute("min_map_size_crse", MIN_MAP_SIZE_CRSE);
      elem->get_attribute("min_map_size_fine", MIN_MAP_SIZE_FINE);
      elem->get_attribute("move_back", MOVE_BACK);
      elem->get_attribute("num_coarse_out", NUM_COARSE_OUT);
      elem->get_attribute("num_fine_out", NUM_FINE_OUT);
      elem->get_attribute("num_points_averaged", NUM_POINTS_AVERAGED);
      elem->get_attribute("olap_thresh_high", OLAP_THRESH_HIGH);
      elem->get_attribute("smooth_edge_bonus", SMOOTH_EDGE_BONUS);
      elem->get_attribute("th_a", TH_A);
      elem->get_attribute("th_d", TH_D);
      elem->get_attribute("th_l", TH_L);
      elem->get_attribute("th_la", TH_LA);
      elem->get_attribute("transition_steepness", TRANSITION_STEEPNESS);
    }
    else if(elem->name() == "output")
    {
      elem->get_attribute("filename", OUT_FNAME);
      elem->get_attribute("fragment_pairs", FRAG_PAIRS_OUT_FNAME);
    }
  }
}

void get_images_and_contours()
{
  for(unsigned i=0; i<FRAG_PATHS.size(); i++)
  {
    vcl_string folder = FRAG_PATHS[i];
    folder.append("/");
    bfrag2D *newfrag = new bfrag2D;
    newfrag->frag_id_ = FRAG_IDS[i];
    // brackets are for putting variables out of scope
    {
      vcl_string ftype = folder;
      ftype.append("*_front.jpg");
      vul_file_iterator f(ftype);
      if(f.filename() != NULL) // there is top image
      {
        vcl_string fname = folder;
        fname.append(f.filename());
        newfrag->loadImage(fname, 1);
      }
    }

    {
      vcl_string ftype = folder;
      ftype.append("*_back.jpg");
      vul_file_iterator f(ftype);
      if(f.filename() != NULL) // there is back image
      {
        vcl_string fname = folder;
       fname.append(f.filename());
        newfrag->loadImage(fname, 0);
      }
    }

    {
      vcl_string ftype = folder;
      ftype.append("*_front.con");
      vul_file_iterator f(ftype);
      if(f.filename() != NULL) // there is front contour
      {
        vcl_string fname = folder;
        fname.append(f.filename());
        newfrag->loadContour(fname, 1);
      }
    }

    {
      vcl_string ftype = folder;
      ftype.append("*_back.con");
      vul_file_iterator f(ftype);
      if(f.filename() != NULL) // there is front contour
      {
        vcl_string fname = folder;
        fname.append(f.filename());
        newfrag->loadContour(fname, 0);
      }
    }

    the_frags.push_back(newfrag);
  }
}

int main(int argc, char *argv[])
{
  if(argc != 3)
  {
    vcl_cout << "Usage: " << argv[0] << "-x [input xml file]" << vcl_endl;
    exit(-1);
  }
  vcl_cout << argv[0] << vcl_endl;
  vcl_cout << argv[1] << vcl_endl;
  vcl_cout << argv[2] << vcl_endl;

  vcl_string input = argv[2];
  // get the parameters
  get_params(input);
  // read the images and contours into bfrag2D objects
  get_images_and_contours();
  // create and initialize puzzle solver
  vcl_vector<bfrag_curve> top_contours;
  for( unsigned i = 0; i < the_frags.size(); i++ )
  {
    bfrag_curve newCurve = the_frags[i]->getTopContourAsCurve();    
    top_contours.push_back( newCurve );
  }

  PuzzleSolving *puzzle_solver = new PuzzleSolving();
  vcl_vector<searchState> states;
  // set the contours
  puzzle_solver->setContours(top_contours);
  // preprocess contours
  puzzle_solver->preProcessCurves();
  // execute pairwise matching
  searchState pair_match_result;
  pair_match_result = puzzle_solver->pairMatch();
  // sort the matches according to the cost
  pair_match_result.sortPairwiseMatches();
  // write search state to an xml file
  states.push_back(pair_match_result);
  assert(OUT_FNAME.c_str() != NULL);
  puzzle_solver->write_experiment_search_state_and_puzzle_solving_objects(OUT_FNAME, states);
//  pair_match_result.write_frag_pairs_in_xml(FRAG_PAIRS_OUT_FNAME);

  return 0;
}
