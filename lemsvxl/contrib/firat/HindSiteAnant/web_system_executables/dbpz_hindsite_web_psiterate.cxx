#include <iostream>
#include <string>
#include <params.h>
#include <PuzzleSolving.h>
#include <time.h>

std::string SEST_FNAME;
std::string PUSO_FNAME;
std::string OUT_FNAME;

void get_params(std::string input)
{
  // web system's algorithm-II parameters only
  LINEUP_DIST_THRESH = 15.0;
  OLAP_THRESH_LOW = /*10.0*/15.0;
  IDENTICAL_BONUS = 20;
  CULLING_RANGE = 100;
  NUM_TOP_STATES = 9;
  LOCAL_REG_ITER_PUZ = 25;
  CLOSED_JUNCTION_BONUS = /*25*/10;
  LENGTH_THRESH_HIGH = 50.0;
  DIST_THRESH_LOW = 3.0;
  DIST_THRESH_HIGH = 10.0;
  DIAG_THRESH_HIGH = 5.0;
  NUM_STATES_ITER = 12;
  NUM_ITERATIONS = 1;
  // common parameters to both algorithms
/*  SMOOTH_EDGE_BONUS = 10.0;
  LOCAL_REG_THRESH_HIGH = 10.0;
  LOCAL_REG_THRESH_LOW = 1.0;
  NUM_POINTS_AVERAGED = 15.0;
  MOVE_BACK = 25.0;
  DISTANCE_THRESH_1 = 10;
  DISTANCE_THRESH_2 = 15;
  DIAG_THRESH_LOW = 2.0;
  DIAG_COEF = -5.0;
  LENGTH_COEF = -1;
  DIST_COEF = 20;*/

  OUT_FNAME = "D:/MyDocs/projects/PuzzleSolving/web-system/tile-a-fragments/a18/output.xml";

  std::ofstream out("D:/MyDocs/projects/PuzzleSolving/web-system/tile-c-fragments/a18/iteration_params.txt");
  out << "LINEUP_DIST_THRESH: " << LINEUP_DIST_THRESH << std::endl;
  out << "OLAP_THRESH_LOW: " << OLAP_THRESH_LOW << std::endl;
  out << "IDENTICAL_BONUS: " << IDENTICAL_BONUS << std::endl;
  out << "CULLING_RANGE: " << CULLING_RANGE << std::endl;
  out << "NUM_TOP_STATES: " << NUM_TOP_STATES << std::endl;
  out << "LOCAL_REG_ITER_PUZ: " << LOCAL_REG_ITER_PUZ << std::endl;
  out << "CLOSED_JUNCTION_BONUS: " << CLOSED_JUNCTION_BONUS << std::endl;
  out << "LENGTH_THRESH_HIGH: " << LENGTH_THRESH_HIGH << std::endl;
  out << "DIST_THRESH_LOW: " << DIST_THRESH_LOW << std::endl;
  out << "DIST_THRESH_HIGH: " << DIST_THRESH_HIGH << std::endl;
  out << "DIAG_THRESH_HIGH: " << DIAG_THRESH_HIGH << std::endl;
  out << "NUM_STATES_ITER: " << NUM_STATES_ITER << std::endl;
  out << "NUM_ITERATIONS: " << NUM_ITERATIONS << std::endl;
  out.close();
}

int main(int argc, char *argv[])
{
  if(argc != 3)
  {
    std::cout << "Usage: " << argv[0] << "-x [input xml file]" << std::endl;
    exit(-1);
  }
  std::cout << argv[0] << std::endl;
  std::cout << argv[1] << std::endl;
  std::cout << argv[2] << std::endl;

  std::string input = argv[2];
  // get the parameters
  get_params(input);
  // read in the puzzle solving and search state files and create the objects
  PuzzleSolving *puzzle_solver = new PuzzleSolving();
  std::vector<searchState> states_in;
  std::vector<searchState> states_out;
  puzzle_solver->read_experiment_search_state_and_puzzle_solving_objects(input, states_in);
  // iterate
  clock_t start, end, diff;
  start = clock();
  states_out = puzzle_solver->search(states_in);
  end = clock();
  diff = end - start;
  std::cout << "Iteration running time: " << (double(end)-start) / CLOCKS_PER_SEC << std::endl;
/*  double mili = diff % 1000;
  int sec = (int)floor((diff/1000.0)) % 60;
  int mn = (int)floor((diff/60000.0)) % 60;
  int hr = (int)floor((diff/360000.0));
  std::cout << "Iteration running time of: " << hr << " hours, " << mn << " min, " 
    << sec << " sec, " << mili << " miliseconds." << std::endl;*/

  puzzle_solver->write_experiment_search_state_and_puzzle_solving_objects(OUT_FNAME, states_out);

  return 0;
}
