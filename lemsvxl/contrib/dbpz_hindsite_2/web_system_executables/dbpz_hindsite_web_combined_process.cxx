#include <vcl_iostream.h>
#include <vcl_string.h>
#include <bxml/bxml_read.h>
#include <bfrag2D.h>
#include <bfrag_curve.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <PuzzleSolving.h>
#include <params.h>
#include <time.h>
#include <stdio.h>
#include "PuzzleSolving.h"
#include <bxml/bxml_document.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_read.h>

// input
vcl_vector<vcl_string> FRAG_PATHS;
vcl_vector<int> FRAG_IDS;
// output
vcl_string OUTPUT_FOLDER;
vcl_vector<vcl_string> FOLDERS;
vcl_string FRAG_PAIRS_OUT_FILENAME;
vcl_vector<vcl_string> FRAG_ASSEMBLY_FILENAMES;

vcl_vector<bfrag2D *> the_frags;
unsigned FINISHED_STEPS_SO_FAR;

// for profiling execution time
clock_t start, end;
clock_t start_pwm, end_pwm;
clock_t start_psi, end_psi;
double total_psi;

void get_params(vcl_string input)
{
	
  // open input file
  vcl_ifstream in(input.c_str());//Create an input file stream for para,s file
  // create xml reader
  bxml_stream_read reader;
  bxml_data_sptr data;
  unsigned int depth;
  //vcl_cout<<depth;
  // read parameters
 // depth = 0;
  while(data = reader.next_element(in, depth))
  {vcl_cout<<"boom"<<vcl_endl;
    bxml_element* elem = static_cast<bxml_element*>(data.ptr());
    vcl_string att_name;
    if(elem->name() == "data")
    {
      unsigned id_no = 0;
      while(1)
      {
        char id_name[10000];
        char path_name[10000];
        sprintf(id_name, "id%d", id_no);
        sprintf(path_name, "path%d", id_no);//Stores paths and ids in a buffer(sprintf)
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
    else if(elem->name() == "matching_params")
    {
//      elem->get_attribute("angle_sim_thresh", ANGLE_SIM_THRESH);
      elem->get_attribute("coarse_resample_ds", COARSE_RESAMPLE_DS);
      elem->get_attribute("dist_step", DIST_STEP);
      elem->get_attribute("edge_coef", EDGE_COEF);
      elem->get_attribute("fine_resample_ds", FINE_RESAMPLE_DS);
      elem->get_attribute("fine_sample_skip", FINE_SAMPLE_SKIP);
      elem->get_attribute("lam", LAM);
      elem->get_attribute("length_thresh_low", LENGTH_THRESH_LOW);
      elem->get_attribute("local_reg_iter_pair", LOCAL_REG_ITER_PAIR);
      elem->get_attribute("max_fine_cost", MAX_FINE_COST);
      elem->get_attribute("min_d", MIN_D);
      elem->get_attribute("min_map_size_crse", MIN_MAP_SIZE_CRSE);
      elem->get_attribute("min_map_size_fine", MIN_MAP_SIZE_FINE);
      elem->get_attribute("min_tan_turn", MIN_TAN_TURN);
      elem->get_attribute("num_coarse_out", NUM_COARSE_OUT);
      elem->get_attribute("num_fine_out", NUM_FINE_OUT);
      elem->get_attribute("num_times", NUM_TIMES);
      elem->get_attribute("olap_thresh_high", OLAP_THRESH_HIGH);
      elem->get_attribute("psi", PSI);
      elem->get_attribute("th_a", TH_A);
      elem->get_attribute("th_d", TH_D);
      elem->get_attribute("th_l", TH_L);
      elem->get_attribute("th_la", TH_LA);
//      elem->get_attribute("transition_steepness", TRANSITION_STEEPNESS);
      elem->get_attribute("vicinity", VICINITY);
    }
    else if(elem->name() == "puzzle_solving_params")//Check if these are ever used********
    {
      elem->get_attribute("closed_junction_bonus", CLOSED_JUNCTION_BONUS);
      elem->get_attribute("culling_range", CULLING_RANGE);
      elem->get_attribute("diag_thresh_high", DIAG_THRESH_HIGH);
      elem->get_attribute("dist_thresh_high", DIST_THRESH_HIGH);
      elem->get_attribute("dist_thresh_low", DIST_THRESH_LOW);
      elem->get_attribute("identical_bonus", IDENTICAL_BONUS);
      elem->get_attribute("length_thresh_high", LENGTH_THRESH_HIGH);
      elem->get_attribute("lineup_dist_thresh", LINEUP_DIST_THRESH);
      elem->get_attribute("local_reg_iter_puz", LOCAL_REG_ITER_PUZ);
      elem->get_attribute("number_of_iteration_steps", NUMBER_OF_ITERATION_STEPS);
      elem->get_attribute("num_states_iter", NUM_STATES_ITER);
      elem->get_attribute("num_top_states_saved", NUM_TOP_STATES_SAVED);
      elem->get_attribute("num_top_states_processed", NUM_TOP_STATES_PROCESSED);
      elem->get_attribute("olap_thresh_low", OLAP_THRESH_LOW);
      elem->get_attribute("top_matches_to_keep", TOP_MATCHES_TO_KEEP);

    }
    else if(elem->name() == "common_params")
    {
      elem->get_attribute("diag_coef", DIAG_COEF);
      elem->get_attribute("diag_thresh_low", DIAG_THRESH_LOW);
      elem->get_attribute("distance_thresh_1", DISTANCE_THRESH_1);
      elem->get_attribute("distance_thresh_2", DISTANCE_THRESH_2);
      elem->get_attribute("dist_coef", DIST_COEF);
      elem->get_attribute("length_coef", LENGTH_COEF);
      elem->get_attribute("local_reg_thresh_high", LOCAL_REG_THRESH_HIGH);
      elem->get_attribute("local_reg_thresh_low", LOCAL_REG_THRESH_LOW);
      elem->get_attribute("move_back", MOVE_BACK);
      elem->get_attribute("num_points_averaged", NUM_POINTS_AVERAGED);
      elem->get_attribute("smooth_edge_bonus", SMOOTH_EDGE_BONUS);
    }
    else if(elem->name() == "output")
    {
      elem->get_attribute("directory", OUTPUT_FOLDER);
      elem->get_attribute("fragment_pairs", FRAG_PAIRS_OUT_FILENAME);
      // get fragment assembly object filenames
      unsigned id_no = 0;
      while(1)
      {
        char id_name[10000];
        sprintf(id_name, "fragment_assembly%d", id_no);
        id_no++;
        vcl_string filename;
        elem->get_attribute(id_name, filename);
        if(filename != "")
          FRAG_ASSEMBLY_FILENAMES.push_back(filename);
        else
          break;
      }
    }
  }
}

// 1. returns -1 if this is a brand new job, i.e. no output folders had been created,
// pairwise matching and puzzle solving iteration had not been run yet
// 2. returns -2 if pairwise matching was run with the same parameters, but
// iteration parameters are different
unsigned get_status()
{
  vcl_cout<<"get status called"<<vcl_endl;
  vcl_string status_filename = OUTPUT_FOLDER;//sets output folder which it had read from params file
  status_filename.append("\\user\\status.txt");
  //vcl_cout<<status_filename+"crapass"<<vcl_endl;
  vcl_ifstream in(status_filename.c_str());
  if(in == NULL)
  {//this means if file not there already. Sometimes it maybe. 
	  //for instance running another job.
    vcl_string pwm_output = OUTPUT_FOLDER;
	vcl_cout << "entered in==null" << vcl_endl;
    pwm_output.append("\\user\\iter0\\output.xml");//if no output folder exists create iter0
    vcl_ifstream pwm_in(pwm_output.c_str());
    if(pwm_in == NULL)
	{vcl_cout<<"entered status = -1"<<vcl_endl;
	return -1;}
	else{
		vcl_cout<<"entered status = -2"<<vcl_endl;
		return -2;}
  }
  else
  {
	  vcl_cout<<"entered in!=null"<<vcl_endl;
    unsigned num_already_taken_steps;
    vcl_string dummy;
    in >> dummy;
    assert(dummy == "Number_of_Iterations_Finished:");
    in >> num_already_taken_steps;
    return num_already_taken_steps;
  }
  /*So Status -1 means no iter folders and no status file. i.e new job. status -2 
  means no output file but some iters done. Probably a continuation of an old job*/
}

void write_status()
{
  // write status file
	vcl_cout<<"crappy"<<vcl_endl;
  vcl_string status_filename = OUTPUT_FOLDER;
  status_filename.append("\\user\\status.txt");
  vcl_ofstream out(status_filename.c_str());
  out << "Number_of_Iterations_Finished: " << FINISHED_STEPS_SO_FAR << vcl_endl;
  out << "Number_of_Pieces_Added_to_the_Puzzle: " << FINISHED_STEPS_SO_FAR+1 << vcl_endl;
  out.close();
}

void get_images_and_contours()

{
  for(unsigned i=0; i<FRAG_PATHS.size(); i++)
  {
    vcl_string folder = FRAG_PATHS[i];
    folder.append("/");
    bfrag2D *newfrag = new bfrag2D;//new frag points to a new bfrag2d
    newfrag->frag_id_ = FRAG_IDS[i];
	//vcl_cout<<FRAG_IDS[i]<<vcl_endl;
    // brackets are for putting variables out of scope
    {
      vcl_string ftype = folder;
	 
      ftype.append("*_front.jpg");
	 // ftype.append("b01_front.jpg");
	  //vcl_cout <<ftype<<vcl_endl;
      vul_file_iterator f(ftype);
      if(f.filename() != NULL) // there is top image
      {//vcl_cout<<"main aa gya"<<vcl_endl;
        vcl_string fname = folder;
        fname.append(f.filename());
        newfrag->loadImage(fname, 1);//calling load image method from bfrag2d
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
      {//vcl_cout<<"main aa gyacon"<<vcl_endl;
        vcl_string fname = folder;
        fname.append(f.filename());
        newfrag->loadContour(fname, 1);//calling load contour method from bfrag2d
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
//stores all fragments as objects after getting there image and contour
void execute_pairwise_matching()
{
  // read the images and contours into bfrag2D objects
  get_images_and_contours();//3rd june Daily record for functioning
 
  // create and initialize puzzle solver
  vcl_vector<bfrag_curve> top_contours;
  vcl_vector<bfrag_curve> bot_contours;
  for( unsigned i = 0; i < the_frags.size(); i++ )
  {if(the_frags[i]->topContourCurveMade){
    bfrag_curve newCurve = the_frags[i]->getTopContourAsCurve(); //retunrs something called topcontourascurve   
    top_contours.push_back( newCurve );//collection of topcontours
  }
	if(the_frags[i]->botContourCurveMade){
	bfrag_curve newCurve = the_frags[i]->getBotContourAsCurve(); //returns botcontourascurve   
    bot_contours.push_back( newCurve );//collection of botcontours
	}
  }

  PuzzleSolving *puzzle_solver = new PuzzleSolving();//starts a new puzzlesolving
  vcl_vector<searchState> states;
  // set the contours
 // vcl_cout<<top_contours.size()<<vcl_endl;basically all contours(one each per fragment.)
  //vcl_cout<<"top_contours.size()"<<vcl_endl;
  puzzle_solver->setContours(top_contours);

  start_pwm = clock();
  // preprocess contours
  puzzle_solver->preProcessCurves();
  // execute pairwise matching
  searchState pair_match_result;
  pair_match_result = puzzle_solver->pairMatch();
  end_pwm = clock();

  // sort the actual global matches according to the cost

  vcl_sort(_matches.begin(), _matches.end(), pairwiseMatchSort2());
  // sort the match lists according to the cost
  pair_match_result.sortPairwiseMatches();
 
  // change the index in the match list
 
  for(unsigned i=0; i < pair_match_result.matches_ez_list_.size(); i++)
    pair_match_result.matches_ez_list_[i].first = i;
  // write search state to an xml file
  states.push_back(pair_match_result);
  assert(FOLDERS[0].c_str() != NULL);
  puzzle_solver->write_experiment_search_state_and_puzzle_solving_objects(FOLDERS[0], states, true);
  pair_match_result.write_frag_pairs_in_xml(FRAG_PAIRS_OUT_FILENAME);
 //vcl_cout<<"chutiyappa nahin huashayad"<<vcl_endl;
}

void execute_puzzle_solving_iteration(unsigned i)
{
	vcl_cout<<"entered puzzle solving iteration"<<i+1<<vcl_endl;
  // read in the puzzle solving and search state files and create the objects
  PuzzleSolving *puzzle_solver = new PuzzleSolving();
	//vcl_cout<<"finsihed making a new puzzle solving"<<vcl_endl;
  vcl_vector<searchState> states_in;
  vcl_vector<searchState> states_out;

  puzzle_solver->read_experiment_search_state_and_puzzle_solving_objects(FOLDERS[i], states_in, true);//read in data about prev state and where it dumped files.

  /*******TEMPORARY*******/
/*  vcl_vector<searchState> states;
  // sort the actual global matches according to the cost
  vcl_sort(_matches.begin(), _matches.end(), pairwiseMatchSort2());
  // sort the match lists according to the cost
  states_in[0].sortPairwiseMatches();
  // change the index in the match list
  for(unsigned k=0; k < states_in[0].matches_ez_list_.size(); k++)
    states_in[0].matches_ez_list_[k].first = k;
  // write search state to an xml file
  states.push_back(states_in[0]);
  assert(FOLDERS[0].c_str() != NULL);
  puzzle_solver->write_experiment_search_state_and_puzzle_solving_objects(FOLDERS[0], states, true);
  exit(-1);*/
  /*******TEMPORARY*******/

  // iterate
  start_psi = clock();
  states_out = puzzle_solver->search(states_in);//SEE SOMETHING U DIN KNOW WAS SIGNIFICANT!!!
  end_psi = clock();
  total_psi =+ ((double(end_psi) - start_psi) / CLOCKS_PER_SEC);

  if(i+1 < FOLDERS.size())
    puzzle_solver->write_experiment_search_state_and_puzzle_solving_objects(FOLDERS[i+1], states_out, true);
  if(i <= NUMBER_OF_ITERATION_STEPS)//CHANGED HERE BY ANANT!!!
  {vcl_stringstream dump_state_file_full_path;
	  //dump_state_file_full_path << FRAG_ASSEMBLY_FILENAMES[0]<< i << ".xml";
    //bxml_write(dump_state_file_full_path.c_str(), doc);
	  puzzle_solver->write_frag_assemblies_in_xml(dump_state_file_full_path.str(), states_out);}
}

int main(int argc, char *argv[])
{
  start = clock();
  if(argc != 3)
  {
    vcl_cout << "Usage: " << argv[0] << "-x [input xml file]" << vcl_endl;
    exit(-1);
  }
  vcl_cout << argv[0] << vcl_endl;
  vcl_cout << argv[1] << vcl_endl;
  vcl_cout << argv[2] << vcl_endl;

  vcl_string input = argv[2];
  // get the parameters. input is the string from the commandline argument which is param file name
  get_params(input);//pass params file to get_params
  assert(FRAG_IDS.size() != 0);//as long as more than one fragment present
  // get the status of this job
  int status = get_status();
  if(NUMBER_OF_ITERATION_STEPS >= FRAG_IDS.size())//BAsed on number of iteration steps u have told in params file.
	  //YOu can secify a max number of iterations if u dont want to cover a lot of pieces.
  {
    vcl_cout << "Specified number of iteration steps(" << NUMBER_OF_ITERATION_STEPS;
    vcl_cout << ") is too large for the given number of fragments(" << FRAG_IDS.size() << ")..." << vcl_endl;
    vcl_cout << "Setting it to the maximum possible value(" << FRAG_IDS.size()-1 << ")..." << vcl_endl;
    NUMBER_OF_ITERATION_STEPS = FRAG_IDS.size()-1;
  }
  if(status == -2)
  {
vcl_cout << "status == -2";
    FINISHED_STEPS_SO_FAR = 0;
    // create all missing(iteration steps) output folders
    // iter0 folder is already there

    for(unsigned i=1; i<FRAG_IDS.size(); i++)
    {

	//vcl_cout << "AARRGGG1: "  << vcl_endl;
      vcl_stringstream system_command;
      system_command << OUTPUT_FOLDER << "\\user\\iter" << i;
      vul_file::make_directory(system_command.str());
    }
    // set status to zero so that we enter puzzle solving iteration loop
    status = 0;
	
	
  }
  if(status == -1)
  {
    FINISHED_STEPS_SO_FAR = 0;
    // create all output folders
    // the number of folders to be created is "number of fragments - 1"
    for(unsigned i=0; i<FRAG_IDS.size(); i++)
    {
      vcl_stringstream system_command;
	  //vcl_cout << "\AARRGGG2: "  << vcl_endl;
      system_command << OUTPUT_FOLDER << "\\user\\iter" << i;//Creates all folders as it wants to be ready for thr future.
      vul_file::make_directory(system_command.str());
//vcl_cout << system_command.str()<< vcl_endl;
      FOLDERS.push_back(system_command.str());
    }
    // run pairwise matching
    execute_pairwise_matching();//Heart of function

    // run puzzle solving NUMBER_OF_ITERATION_STEPS times
    for(unsigned i=0; i < NUMBER_OF_ITERATION_STEPS; i++)
    {
      //vcl_cout << "Running Iteration No: " << i+1 << vcl_endl;
      execute_puzzle_solving_iteration(i)// problem definitely lies here everything else seems to be good though i doubt if folders ae being created but i guess that'll become clear when i debug this.
		  ;
	  //vcl_cout<<"guten tag"<<vcl_endl;
      FINISHED_STEPS_SO_FAR++;
      write_status();
    }
	 //vcl_cout<<"yyayayaya"<<vcl_endl;
  }
  else
  {
    FINISHED_STEPS_SO_FAR = status;
    if(FOLDERS.size() == 0)
    {
      for(unsigned i=0; i<FRAG_IDS.size(); i++)
      {
        vcl_stringstream system_command;
		//vcl_cout << "AARRGGG3: "  << vcl_endl;
        system_command << OUTPUT_FOLDER << "\\user\\iter" << i;
        //vcl_cout << system_command.str() << vcl_endl;
        FOLDERS.push_back(system_command.str());
      }
    }
    vcl_cout << "Pairwise matching already done and " << status << " iteration(s) already run..." << vcl_endl;
    vcl_cout << "Running " << NUMBER_OF_ITERATION_STEPS-status << " iteration(s)..." << vcl_endl;
    for(unsigned i=status; i < NUMBER_OF_ITERATION_STEPS; i++)
    {
      vcl_cout << "Running Iteration No: " << i+1 << vcl_endl;
      execute_puzzle_solving_iteration(i);
      FINISHED_STEPS_SO_FAR++;
      write_status();
    }
  }

  write_status();

  end = clock();

  vcl_cout << "Algorithm total execution time: " << (double(end)-start) / CLOCKS_PER_SEC << vcl_endl;
  vcl_cout << "Pairwise matching execution time: " << (double(end_pwm)-start_pwm) / CLOCKS_PER_SEC << vcl_endl;
  vcl_cout << "Puzzle solving iteration(s) execution time: " << total_psi << vcl_endl;

  return 0;
}
