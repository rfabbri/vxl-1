#ifndef dbdet_evaluation_process_h_
#define dbdet_evaluation_process_h_

//By Yuliang Guo Oct/25/2010
//This file contains the process to perform contour evaluation

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <math.h>
#include "dbdet/sel/dbdet_curve_fragment_graph.h"
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vcl_vector.h>
#include <vcl_set.h>

// This class is to use in edit distance process to generate feasible 
class frags_combination
{
public:
	//Default Constructor
	frags_combination(): frags(0),index(0){}

	//Constructor
        frags_combination(dbdet_edgel_chain* const &chain, int i)
	{
		start_point = chain->edgels.front();
		end_point = chain->edgels.back();
		frags.push_back(chain);
		index.push_back(i);
	}
	//Destructor
	~frags_combination(){}

	//insert a combination into this combination, !! end points are not assigned inserting
	void insert_frags(frags_combination* const &combo)
	{
		vcl_list<dbdet_edgel_chain*>::const_iterator f_it_0 = combo->frags.begin();
		for (; f_it_0 != combo->frags.end(); f_it_0++)
		{	
			frags.push_back(*f_it_0);
		}
		for (int i =0; i< (combo->index.size()); i++)
		{
			index.push_back(combo->index[i]);
		}
	}

	void insert_frags(vcl_list<dbdet_edgel_chain*> const &frags_0)
	{
		vcl_list<dbdet_edgel_chain*>::const_iterator f_it_0 = frags_0.begin();
		for (; f_it_0 != frags_0.end(); f_it_0++)
		{	
			frags.push_back(*f_it_0);
		}		
	}

        int num_edgels()
	{
		int size;
		vcl_list<dbdet_edgel_chain*>::const_iterator f_it_0 = frags.begin();
		for (; f_it_0 != frags.end(); f_it_0++)
		{	
			size += (*f_it_0)->edgels.size();
		}
	}

        //function to check if two combination are the same
	bool equals(frags_combination* const &combo)
	{
		if(frags.size()!=combo->frags.size())
			return false;
		// copy the index lists to sort and compare 
		dbdet_edgel_chain_list frags_0 = frags;
		dbdet_edgel_chain_list frags_1 = combo->frags;
		frags_0.sort();
		frags_1.sort();

		dbdet_edgel_chain_list_iter iter_0 = frags_0.begin();
		dbdet_edgel_chain_list_iter iter_1 = frags_1.begin();
		for(; iter_0 != frags_0.end() && iter_1 != frags_1.end(); iter_0++, iter_1++)
			if((*iter_0)!=(*iter_1))
				return false;
		return true;
	}

	friend vcl_ostream &operator << (vcl_ostream &out, const frags_combination* c1)
	{
		out << "[ ";
		for (int i = 0; i< (c1->index.size()); i++)
			out<< (c1->index[i]) <<" ";
		out <<"]";
		return out;
	}	
	//List to hold the address of fragments	
	dbdet_edgel_chain_list frags;
	//list to hold the index in 1 combo list
	vcl_vector<int> index;
	//The start_point and end_point of the combination
	dbdet_edgel* start_point;
	dbdet_edgel* end_point;
};

class dbdet_evaluation_process : public bpro1_process
{
public:

  dbdet_evaluation_process();
  virtual ~dbdet_evaluation_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

  bool call_in_gui;
  double TP_gt_l;
  double TP_cp_l;
  double gt_length;
  double cp_length;
  double total_edit_dist;

  bool bprune_cfg_0;
  bool bprune_cfg_1;

  //compute the localization distance between two edgels
  double compute_edgel_dist(dbdet_edgel* e1, dbdet_edgel* e2);
  //compute the orientation different between two edgels
  double compute_ori_diff(dbdet_edgel* e1, dbdet_edgel* e2);
  //compute the cost from contour c1 to c2
  // cost = sum of each closest distance between the edgels in c1 and edgels in c2/ number of edgels in c1
  double compute_contours_cost(dbdet_edgel_chain* const &c1, dbdet_edgel_chain* const &c2);

  void check_column(vnl_matrix<double> cost_gt2cp,vnl_matrix<double> cost_cp2gt,vcl_vector<unsigned> &column2check, vcl_set<unsigned> &row_checked,vcl_set<unsigned> &column_checked,vnl_matrix<unsigned> &grouped_gt_cp);
  void check_row(vnl_matrix<double> cost_gt2cp,vnl_matrix<double> cost_cp2gt,vcl_vector<unsigned> &row2check, vcl_set<unsigned> &row_checked,vcl_set<unsigned> &column_checked,vnl_matrix<unsigned> &grouped_gt_cp);

  bool find_in_vector(vcl_vector<unsigned> const &v, unsigned value);

  void get_parameters();

  bool is_contour_relative(dbdet_edgel_list const &c1, dbdet_edgel_list const &c2);
  
  void set_cost_thresh(int value){cost_thresh = value;}

  int get_cost_thresh(){return cost_thresh;}

  void set_edit_thresh(double value){edit_thresh = value;}

  double get_edit_thresh(){return edit_thresh;}

  void set_local_dist(double value){local_dist = value;}

  double get_local_dist(){return local_dist;}

  void set_len_thresh_0(double value){len_thresh_0 = value;}

  double get_len_thresh_0(){return len_thresh_0;}

  void set_len_thresh_1(double value){len_thresh_1 = value;}

  double get_len_thresh_1(){return len_thresh_1;}

  void set_str_thresh_0(double value){str_thresh_0 = value;}

  double get_str_thresh_0(){return str_thresh_0;}

  void set_str_thresh_1(double value){str_thresh_1 = value;}

  double get_str_thresh_1(){return str_thresh_1;}

  vcl_vector<vcl_list<dbdet_edgel_chain_list> > edit_distance_process(dbdet_edgel_chain_list &gt_frags, dbdet_edgel_chain_list &cp_frags);

  bool overlap_combos(frags_combination* const &combo_0,  frags_combination* const &combo_1);

  // this function will use combos and combo_1 to generate a list of combos, each one has one more contour than each one in combos
  vcl_list<frags_combination*> generate_combination(vcl_list<frags_combination*> const &combos,  vcl_list<frags_combination*> const &combos_1, vcl_list<frags_combination*> const &group_combos);
  
  // this function compute the transform cost between two fragments combinations
  double compute_transform_cost(frags_combination* combo_0, frags_combination* combo_1);

  // check if combo_1 is sub combination of combo_0 (contain c1 \\ c2 \\ ... cn within combo_0)
  bool is_sub_combo(frags_combination* combo_0, frags_combination* combo_1);

  // this function compute the minimum distance from a edgel to a fragments combination
  double min_dist(dbdet_edgel* const &e0, frags_combination* const &combo_0);

  double combo_length(frags_combination* combo_0);

protected:

  double cost_thresh;
  double edit_thresh;
  double local_dist;
  bool bgroup_only;
  unsigned len_thresh_0;
  unsigned len_thresh_1;
  double str_thresh_0;
  double str_thresh_1;

};


#endif
