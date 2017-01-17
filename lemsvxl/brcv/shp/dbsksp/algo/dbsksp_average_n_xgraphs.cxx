// This is shp/dbsksp/algo/dbsksp_average_n_xgraphs.cxx


//:
// \file
// 



#include "dbsksp_average_n_xgraphs.h"
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsksp/algo/dbsksp_weighted_average_two_xgraphs.h>
#include <dbsksp/algo/dbsksp_average_two_xgraphs.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_numeric_traits.h>
#include <vul/vul_timer.h>
#include <vul/vul_sprintf.h>
#include <vul/vul_file.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstdio.h>
#include <vnl/vnl_random.h>



//==============================================================================
// dbsksp_average_n_xgraphs
//==============================================================================


//------------------------------------------------------------------------------
//: Constructor
dbsksp_average_n_xgraphs::
dbsksp_average_n_xgraphs()
{
  this->parent_xgraphs_.clear();

  this->scurve_sample_ds_ = 3.0f;

  this->scurve_matching_R_ = 6.0f;

  this->relative_error_tol_ = 0.01f;

  // Ouput----------------------------------------------------------------------

  //: Average xgraph
  this->average_xgraph_ =0;
  this->distance_parent_to_avg_.clear();
  this->deform_cost_parent_to_avg_.clear();
}


//------------------------------------------------------------------------------
//: Initialize distances
void dbsksp_average_n_xgraphs::
init()
{
  if (!vul_file::is_directory(vul_file::dirname(this->base_name_)))
  {
    this->base_name_ = "";
  }
  
  // make sure the list of object names is properly set up
  if (this->parent_names_.size() != this->parent_xgraphs_.size())
  {
    this->parent_names_.clear();
    for (unsigned i =0; i < this->parent_xgraphs_.size(); ++i)
    {
      vcl_string object_name = vul_sprintf("object%d", i+1);
      this->parent_names_.push_back(object_name);
    }
  }
}



//------------------------------------------------------------------------------
//: Set list of parent xgraphs
void dbsksp_average_n_xgraphs::
set_parent_xgraphs(const vcl_vector<dbsksp_xshock_graph_sptr >& xgraphs,
                   const vcl_vector<vcl_string >& names)
{
  this->parent_xgraphs_ = xgraphs;
  this->parent_names_   = names;
}



//------------------------------------------------------------------------------
//: Compute distance between two xgraphs - keeping the intermediate work
double dbsksp_average_n_xgraphs::
compute_edit_distance(const dbsksp_xshock_graph_sptr& xgraph1, 
                      const dbsksp_xshock_graph_sptr& xgraph2,
                      dbsksp_edit_distance& work)
{
  dbsksp_edit_distance edit_distance;
  edit_distance.set(xgraph1, xgraph2, this->scurve_matching_R(), this->scurve_sample_ds());
  edit_distance.save_path(true);
  edit_distance.edit();
  work = edit_distance;
  return edit_distance.final_cost();
}





//------------------------------------------------------------------------------
//: Compute weighted average of two xgraphs
bool dbsksp_average_n_xgraphs::
compute_average_xgraph(const dbsksp_xshock_graph_sptr& xgraph1,
    const dbsksp_xshock_graph_sptr xgraph2,
    double weight1,
    double weight2,
    const vcl_string& name1,
    const vcl_string& name2,
    dbsksp_xshock_graph_sptr& average_xgraph,
    double& relative_error)
{
  vcl_string pair_base_name = this->base_name_ + "+" + name1 + "+" + name2;
  dbsksp_weighted_average_two_xgraphs engine(xgraph1, xgraph2, weight1, weight2, 
    this->scurve_matching_R(), this->scurve_sample_ds(), 0.5,
    this->relative_error_tol_, pair_base_name);
  bool success = engine.compute();

  if (success)
  {
    average_xgraph = engine.average_xgraph();
    relative_error = engine.relative_error();
    return true;
  }
  else
  {
    average_xgraph = 0;
    relative_error = vnl_numeric_traits<double >::maxval;
    return false;
  }
}















//------------------------------------------------------------------------------
//: Print debug info to a file
void dbsksp_average_n_xgraphs::
print_debug_info(const vcl_string& str) const
{
  if (this->debugging())
  {
    vcl_string fname = this->base_name_ + "-debug.txt";
    vcl_ofstream ofs(fname.c_str(), vcl_ofstream::app);
    ofs << str;
    ofs.close();
  }
  return;
}















//==============================================================================
// dbsksp_average_n_xgraphs_iterative_merge
//==============================================================================


//------------------------------------------------------------------------------
//: Execute the averaging function
// Assumption: each shock branch has 2^n intervals
bool dbsksp_average_n_xgraphs_iterative_merge::
compute()
{
  //0) Initialize internal variables
  this->init();

  vcl_cout 
    << "\nCompute average of N xgraphs"
    << "\n  base name for saving intermediate files= " << this->base_name_ << "\n";

  // save the original xgraphs
  if (this->debugging())
  {
    for (unsigned i =0; i < this->parent_xgraphs_.size(); ++i)
    {
      vcl_string fname = this->base_name_ + vul_sprintf("-parent%d.xml", i+1); 
      x_write(fname, this->parent_xgraph(i));
    }
  }

  // nothing to compute when there is no shapes
  if (this->parent_xgraphs_.empty())
    return false;

  // Iterative merge, increase weight every time
  vcl_vector<dbsksp_xshock_graph_sptr > active_xgraphs = this->parent_xgraphs_;
  vcl_vector<double > active_weights (active_xgraphs.size(), 1.0);
  vcl_vector<vcl_string > active_names = this->parent_names_;

  while (active_xgraphs.size() > 1)
  {
    dbsksp_xshock_graph_sptr xgraph1 = active_xgraphs.back();
    active_xgraphs.pop_back();

    double w1 = active_weights.back();
    active_weights.pop_back();

    vcl_string name1 = active_names.back();
    active_names.pop_back();

    // find the xgraph that is closest to xgraph1
    int min_idx = -1;
    double min_distance = vnl_numeric_traits<double >::maxval;
    for (unsigned i =0; i < active_xgraphs.size(); ++i)
    {
      dbsksp_edit_distance work;
      double d = this->compute_edit_distance(xgraph1, active_xgraphs[i], work);
      if (d < min_distance)
      {
        min_idx = i;
        min_distance = d;
      }
    }
    
    // merge xgraph1 and its closest xgraph, with proper weights
    dbsksp_xshock_graph_sptr xgraph2 = active_xgraphs[min_idx];
    double w2 = active_weights[min_idx];
    vcl_string name2 = active_names[min_idx];

    // some annoucement
    vcl_cout << "\nCompute weighted average of:"
      << "\n  Object1: " << name1
      << "\n  Object2: " << name2 << "\n";


    //// put the xgraph with more weight in front
    //if (w2 > w1)
    //{
    //  vcl_swap(w1, w2);
    //  vcl_swap(xgraph1, xgraph2);
    //  vcl_swap(name1, name2);
    //}
    

    dbsksp_xshock_graph_sptr average_xgraph = 0;
    // try both sides and pick the best average
    {
      dbsksp_xshock_graph_sptr temp_avg[2];
      double temp_error[2];
      this->compute_average_xgraph(xgraph1, xgraph2, w1, w2, name1, name2,
      temp_avg[0], temp_error[0]);

      this->compute_average_xgraph(xgraph2, xgraph1, w2, w1, name2, name1,
      temp_avg[1], temp_error[1]);

      average_xgraph = (temp_error[0] <= temp_error[1]) ? temp_avg[0] : temp_avg[1];
    }

    if (!average_xgraph)
    {
      vcl_cout << "\nAveraging two xgraphs failed.\n";
      return false;
    }

    // replace xgraph2 with the average xgraph, change the weight
    active_xgraphs[min_idx] = average_xgraph;
    active_weights[min_idx] = w1 + w2;
    active_names[min_idx]   = name1 + "+" + name2;
  }

  this->average_xgraph_ = active_xgraphs.front();

  if (!this->average_xgraph_)
    return false;

  // Compute distance from parent shapes to the average shape
  this->distance_parent_to_avg_.clear();
  for (unsigned i =0; i < this->parent_xgraphs_.size(); ++i)
  {
    dbsksp_edit_distance work;
    double d = this->compute_edit_distance(this->parent_xgraphs_[i], this->average_xgraph_, work);
    this->distance_parent_to_avg_.push_back(d);

    vcl_vector<pathtable_key > correspondence;
    double deform_cost = work.get_deform_cost(correspondence);
    this->deform_cost_parent_to_avg_.push_back(deform_cost);
  }

  return true;
}







//==============================================================================
// dbsksp_average_n_xgraphs_by_curve_shortening
//==============================================================================


//: Compute average
bool dbsksp_average_n_xgraphs_by_curve_shortening::
compute()
{
  //0) Initialize internal variables
  this->init();

  vcl_cout 
    << "\nCompute average of N xgraphs by mid-point curve shortening"
    << "\n  base name for saving intermediate files= " << this->base_name_ << "\n";

  // save the original xgraphs
  if (this->debugging())
  {
    for (unsigned i =0; i < this->parent_xgraphs_.size(); ++i)
    {
      vcl_string fname = this->base_name_ + vul_sprintf("-parent%d.xml", i+1); 
      x_write(fname, this->parent_xgraph(i));
    }
  }

  // nothing to compute when there is no shapes
  if (this->parent_xgraphs_.empty())
    return false;

  unsigned num_parents = this->parent_xgraphs_.size();

  // Deep copy the parent xgraphs
  vcl_vector<dbsksp_xshock_graph_sptr > cur_xgraphs(num_parents, 0);
  vcl_vector<vcl_string > cur_names(num_parents, "");
  for (unsigned i =0; i < num_parents; ++i)
  {
    cur_xgraphs[i] = new dbsksp_xshock_graph(*(this->parent_xgraphs_[i]));
    cur_names[i]   = vul_sprintf("a0_%d", i+1);
  }


  vcl_vector<double > trace_curve_length;
  int max_num_iters = 20;
  double curve_length_tol = 3 * num_parents; // we're happy if the shapes are on average distance 3 apart
 
  for (int k = 0; k < max_num_iters; ++k)
  {
    // save the "a" sequence
    if (this->debugging())
    {
      for (unsigned i =0; i < num_parents; ++i)
      {
        vcl_string fname = this->base_name_ + "-" + cur_names[i] + ".xml"; 
        x_write(fname, cur_xgraphs[i]);
      }
    }


    // Compute total curve length and compare with stopping criteria
    double sum_length =0;
    for (unsigned i =0; i < num_parents; ++i)
    {
      unsigned ip1 = (i+1) % num_parents;
      dbsksp_edit_distance work;
      double d = this->compute_edit_distance(cur_xgraphs[i], cur_xgraphs[ip1], work);
      sum_length += d;
    }
    trace_curve_length.push_back(sum_length);

    vcl_cout << "\nCurrent curve_length = " << sum_length << "\n";
    this->print_debug_info(vul_sprintf("curve_length=%f\n", sum_length));
    
    // check stopping criteria
    if (sum_length < curve_length_tol)
    {
      break;
    }


    // random permutation of the cur_xgraph sequence
    vcl_vector<unsigned > p = this->random_permutation(num_parents);

    // print out the permuation
    vcl_cout << "\nPermutation sequence = ";
    for (unsigned i =0; i < num_parents; ++i)
    {
      vcl_cout << " " << p[i];
    }
    vcl_cout << "\n";


    vcl_vector<dbsksp_xshock_graph_sptr > p_xgraphs(num_parents, 0);
    vcl_vector<vcl_string > p_names(num_parents, "");
    
    for (unsigned i =0; i < num_parents; ++i)
    {
      p_xgraphs[i] = cur_xgraphs[i];  
      p_names[i] = vul_sprintf("b%d_%d", k, i+1);
    }


    // save the "b" sequence
    if (this->debugging())
    {
      for (unsigned i =0; i < num_parents; ++i)
      {
        vcl_string fname = this->base_name_ + "-" + p_names[i] + ".xml"; 
        x_write(fname, p_xgraphs[i]);
      }
    }

    // compute average of p_xgraphs and assigned back to cur_xgraphs
    cur_xgraphs.clear();
    cur_names.clear();
    for (unsigned i =0; i < num_parents; ++i)
    {
      unsigned ip1 = (i+1) % num_parents;

      dbsksp_xshock_graph_sptr xgraph1 = p_xgraphs[i];
      dbsksp_xshock_graph_sptr xgraph2 = p_xgraphs[ip1];

      vcl_string name1 = p_names[i];
      vcl_string name2 = p_names[ip1];
    
      // some annoucement
      vcl_cout << "\nCompute weighted average of:"
        << "\n  Object1: " << name1
        << "\n  Object2: " << name2 << "\n";

      double w1 = 1;
      double w2 = 1;

      dbsksp_xshock_graph_sptr average_xgraph = 0;
      // try both sides and pick the best average
      {
        dbsksp_xshock_graph_sptr temp_avg[2];
        double temp_error[2];
        this->compute_average_xgraph(xgraph1, xgraph2, w1, w2, name1, name2,
          temp_avg[0], temp_error[0]);

        this->compute_average_xgraph(xgraph2, xgraph1, w2, w1, name2, name1,
          temp_avg[1], temp_error[1]);

        average_xgraph = (temp_error[0] <= temp_error[1]) ? temp_avg[0] : temp_avg[1];
      }

      if (!average_xgraph)
      {
        vcl_cout << "\nAveraging two xgraphs failed.\n";
        return false;
      }

      cur_xgraphs.push_back(average_xgraph);
      cur_names.push_back(vul_sprintf("a%d_%d", k+1, i+1));
    }

    vcl_cout << "Number of current xgrapsh = " << cur_xgraphs.size() << "\n";
  }

  this->average_xgraph_ = cur_xgraphs.front();

  if (!this->average_xgraph_)
    return false;

  // Compute distance from parent shapes to the average shape
  this->distance_parent_to_avg_.clear();
  for (unsigned i =0; i < this->parent_xgraphs_.size(); ++i)
  {
    dbsksp_edit_distance work;
    double d = this->compute_edit_distance(this->parent_xgraphs_[i], this->average_xgraph_, work);
    this->distance_parent_to_avg_.push_back(d);

    vcl_vector<pathtable_key > correspondence;
    double deform_cost = work.get_deform_cost(correspondence);
    this->deform_cost_parent_to_avg_.push_back(deform_cost);
  }

  return true;
}








//------------------------------------------------------------------------------
//: Generate a random permutation of the squence (0, 1, ..., n-1)
vcl_vector<unsigned > dbsksp_average_n_xgraphs_by_curve_shortening::
random_permutation(unsigned n)
{
  // standard sequence
  vcl_vector<unsigned > a(n, 0);
  for (unsigned i =0; i < n; ++i)
  {
    a[i] = i;
  }

  vnl_random rand_engine;
  // Use Knuth-shuffle to create a random sequence
  for (unsigned i =0; i < n; ++i)
  {
    // generate a random number between i and (n-1)
    int q = rand_engine.lrand32(i, n-1);
    vcl_swap(a[i], a[q]);
  }
  return a;
}















