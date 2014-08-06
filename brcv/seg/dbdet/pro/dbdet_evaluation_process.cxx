// This Process only output the contour information without edgemap
#include "dbdet_evaluation_process.h"

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>

#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>
#include <dbdet/algo/dbdet_sel_sptr.h>
#include <dbdet/algo/dbdet_sel.h>
#include <dbdet/algo/dbdet_postprocess_contours.h>
#include <dbdet/sel/dbdet_curve_model.h>

#include <vcl_string.h>
#include <vul/vul_timer.h>
#include <vil/vil_image_resource.h>

static double contour_length(dbdet_edgel_chain* const &c);
static bool is_longer (const dbdet_edgel_chain *c1, const dbdet_edgel_chain *c2){ // whether contour 1 is longer
	if (c1->edgels.size()>c2->edgels.size()){
		return true;
	}
	return false;
	
}

static double pi = vnl_math::pi;
static vcl_list<frags_combination*> gt_combos_from_grouping;
static vcl_list<frags_combination*> cp_combos_from_grouping;
//: Constructor
dbdet_evaluation_process::dbdet_evaluation_process()
{
  if (!parameters()->add( "Cost Treshhold for Grouping" , "-cost_thresh" , 2.0 ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
  if (!parameters()->add( "Cost Treshhold for Edit Distance" , "-edit_thresh" , 0.9 ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
  if (!parameters()->add( "Localization distance" , "-local_dist" , 2.0 ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
  if (!parameters()->add( "Only Grouping" , "-bgroup_only" , false ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
  if (!parameters()->add( "Prune Contours in cem 0" , "-bprune_cfg_0" , false ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
  if (!parameters()->add( "  Length threshold for cem 0" , "-len_thresh_0", (unsigned) 3 ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;    
  }
  if (!parameters()->add( "  Strength Threshold cem 0" , "-strength_thresh_0", (double) 10 ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;    
  }

  if (!parameters()->add( "Prune Contours in cem 1" , "-bprune_cfg_1" , false ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
  if (!parameters()->add( "  Length threshold for cem 1" , "-len_thresh_1", (unsigned) 3 ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;    
  }
  if (!parameters()->add( "  Strength Threshold cem 1" , "-strength_thresh_1", (double) 10 ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;    
  }

  call_in_gui = 1;

}

void
dbdet_evaluation_process::get_parameters()
{
  //parameters
  parameters()->get_value( "-cost_thresh", cost_thresh);
  parameters()->get_value( "-edit_thresh", edit_thresh);
  parameters()->get_value( "-local_dist", local_dist);
  parameters()->get_value( "-bgroup_only", bgroup_only);
  parameters()->get_value( "-bprune_cfg_0", bprune_cfg_0);
  parameters()->get_value( "-bprune_cfg_1", bprune_cfg_1);
  parameters()->get_value( "-len_thresh_0", len_thresh_0);
  parameters()->get_value( "-len_thresh_1", len_thresh_1);
  parameters()->get_value( "-strength_thresh_0", str_thresh_0);
  parameters()->get_value( "-strength_thresh_1", str_thresh_1);
}

//: Destructor
dbdet_evaluation_process::~dbdet_evaluation_process()
{
}

//: Clone the process
bpro1_process*
dbdet_evaluation_process::clone() const
{
  return new dbdet_evaluation_process(*this);
}

//: Return the name of this process
vcl_string
dbdet_evaluation_process::name()
{
  return "Contour Evaluation";
}

//: Return the number of input frame for this process
int
dbdet_evaluation_process::input_frames()
{
  return 2;
}


//: Return the number of output frames for this process
int
dbdet_evaluation_process::output_frames()
{
  return 2;
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_evaluation_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "sel" );
  to_return.push_back( "sel" );
  return to_return;
}



//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_evaluation_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  //output the sel storage class
  to_return.push_back( "sel" );
  to_return.push_back( "sel" );

  return to_return;
}


//: Execute the process
bool dbdet_evaluation_process::execute()
{
  //get the parameters
  if(call_in_gui)
  	get_parameters();
  TP_gt_l = 0;
  TP_cp_l = 0;
  gt_length = 0;
  cp_length = 0;
  total_edit_dist = 0;
  //get the input storage class
  
  dbdet_sel_storage_sptr input_sel_0, input_sel_1;
  // first input the GT contours
  input_sel_0.vertical_cast(input_data_[0][0]);
  dbdet_edgemap_sptr EM_0 = input_sel_0->EM();
  dbdet_curve_fragment_graph& CFG_0 = input_sel_0->CFG();
  // second input the CP contours
  input_sel_1.vertical_cast(input_data_[0][1]);
  dbdet_edgemap_sptr EM_1 = input_sel_1->EM();
  dbdet_curve_fragment_graph& CFG_1 = input_sel_1->CFG();

  // create the sel storage class
  dbdet_sel_storage_sptr output_sel_2 = dbdet_sel_storage_new();
  dbdet_sel_storage_sptr output_sel_3 = dbdet_sel_storage_new();

  //referance the prune frags
  dbdet_edgel_chain_list& gt_prune_frags = output_sel_2->prune_frags();
  dbdet_edgel_chain_list& cp_prune_frags = output_sel_3->prune_frags();

  //get pointers to the data structures in it
  //output_sel_2->CM() = input_sel_0->CM();
  //output_sel_3->CM() = input_sel_1->CM();
  //output_sel_2->ELG() = input_sel_0->ELG();
  //output_sel_3->ELG() = input_sel_1->ELG();
  output_sel_2->set_EM(EM_0);
  output_sel_3->set_EM(EM_1);
  dbdet_curve_fragment_graph& CFG_2 = output_sel_2->CFG();
  dbdet_curve_fragment_graph& CFG_3 = output_sel_3->CFG();

  //start the timer
  vul_timer t;

  // prune contour maps using certain matrix
  if(bprune_cfg_0)
  {
	prune_contours_by_length(CFG_0, len_thresh_0);
	prune_contours_by_strength(CFG_0, str_thresh_0);
  }
  if(bprune_cfg_1)
  {
	prune_contours_by_length(CFG_1, len_thresh_1);
	prune_contours_by_strength(CFG_1, str_thresh_1);
  }  
  unsigned row = CFG_0.frags.size(),col = CFG_1.frags.size();

  vcl_cout<<" total GT contours: "<<row<<", total CC contours: "<<col<<vcl_endl;
  
  //creat the cost matix : rows are GT contours, cols are IM contours
  //Each GT contour to Each IM contour has one cost value
  vnl_matrix<double> cost_mat(row,col);
  vcl_list<dbdet_edgel_chain*>::const_iterator f_it_0 = CFG_0.frags.begin(), f_it_1;

  unsigned i=0,j;
  for(;f_it_0!=CFG_0.frags.end();f_it_0++,i++)
  { 	
    j=0;
    f_it_1 = CFG_1.frags.begin();
    for(;f_it_1!=CFG_1.frags.end();f_it_1++,j++)
    {
      cost_mat[i][j]=compute_contours_cost(*f_it_0,*f_it_1);// compute cost 0->1
      double cost_rev = compute_contours_cost(*f_it_1,*f_it_0);//compute cost 1->0
      if(cost_rev<cost_mat[i][j])
        cost_mat[i][j] = cost_rev;   // take the smaller cost for grouping relative contours     
    }
  }
  //vcl_cout<<cost_mat<<vcl_endl;

  //Checking by rows of the cost matrix to extract the related GT contours
  //if some the cost values in a row is smaller than the threshold, put the corrisponding GT contour into the relative GT contour list

  for(i=0, f_it_0 = CFG_0.frags.begin();(i<row)&&(f_it_0!=CFG_0.frags.end());i++,f_it_0++)
  { 
    double min;
    vnl_vector<double> M_row=cost_mat.get_row(i);
    min=M_row.min_value(); 
    gt_length += contour_length(*f_it_0);
    dbdet_edgel_chain* one_contour = new dbdet_edgel_chain(**f_it_0);
    if (min<1000)
       CFG_2.frags.push_back(one_contour); // the relative contours from GT
    else
       gt_prune_frags.push_back(one_contour);
  }


  //Checking by colume of the cost matrix to extract the related IM contours
  //if some the cost values in a col is smaller than the threshold, put the corrisponding IM contour into the relative IM contour list

  for(j=0, f_it_1 = CFG_1.frags.begin();(j<col)&&(f_it_1!=CFG_1.frags.end());j++,f_it_1++)
  { 
    double min;
    vnl_vector<double> M_col=cost_mat.get_column(j);
    min=M_col.min_value();
    cp_length += contour_length(*f_it_1);
    dbdet_edgel_chain* one_contour = new dbdet_edgel_chain(**f_it_1);
    if (min<1000)
       CFG_3.frags.push_back(one_contour);// the relative contours from Computed Contours
    else
       cp_prune_frags.push_back(one_contour);
  } 

  //sort contour lists in length, so that we prefer to find longer matched contours first
  CFG_2.frags.sort(is_longer);
  CFG_3.frags.sort(is_longer);
  vcl_cout << "Finish sorting contour list in length" << vcl_endl;

  double relative_refine_time = t.real() / 1000.0;
  vcl_cout << "Time taken to get relative GT and CC contours: " << relative_refine_time << " sec" << vcl_endl;
  t.mark();

  //creat the matrix holding cost(GT-CC)
  row = CFG_2.frags.size(),col = CFG_3.frags.size();

  vcl_cout<<" relative GT contours: "<<row<<", relative CC contours: "<<col<<vcl_endl; 

  vnl_matrix<double> cost_gt2cp(row,col); // cost matrix contains cost (GT-CC)
  f_it_0 = CFG_2.frags.begin();
  i=0;
  for(;f_it_0!=CFG_2.frags.end();f_it_0++,i++)
  { 
    j=0;
    f_it_1 = CFG_3.frags.begin();
    for(;f_it_1!=CFG_3.frags.end();f_it_1++,j++)
    {
      cost_gt2cp[i][j]=compute_contours_cost(*f_it_0,*f_it_1);// compute cost 0->1   
    }
  }

  //creat the matrix holding cost(CC-GT)
  vnl_matrix<double> cost_cp2gt(row,col); // cost matrix contains cost (CC-GT)
  f_it_0 = CFG_2.frags.begin();
  i=0;
  for(;f_it_0!=CFG_2.frags.end();f_it_0++,i++)
  { 
    j=0;
    f_it_1 = CFG_3.frags.begin();
    for(;f_it_1!=CFG_3.frags.end();f_it_1++,j++)
    {
      cost_cp2gt[i][j]=compute_contours_cost(*f_it_1,*f_it_0);// compute cost 1->0   
    }
  }

  unsigned sum_match_gt_w=0, sum_match_cc_w=0;
  vcl_list<dbdet_edgel_chain_list>& gt_group = output_sel_2->c_groups();
  vcl_list<dbdet_edgel_chain_list>& cp_group = output_sel_3->c_groups();// build two lists to hold frags, frags from gt_group[i]and cp_group[i] are in the same group

  vcl_set<unsigned> row_checked, column_checked;// two vectors to hold the id of rows and columns which are already checked
    vnl_matrix<unsigned> grouped_gt_cp(2,(row>=col)?row:col),sum_grouped_gt_cp(2,(row>=col)?row:col,0);//make a 2 X (max(row,col)) matrix to hold the id of 
                                                                                                       //grouped gt frags and cc frags
  //first row for gt, second row for cc, grouped id is 1,others are 0, ex.
  //      0 0 0 0 1 0 0 0 0 0 1 0
  //      1 0 0 1 0 0 1 1 0 0 1 1
 
  for(j=0;j<col;j++) // start by checked columns in cost(gt-cc) matrix 
  {
    if(column_checked.find(j)!=column_checked.end())
        continue;
    grouped_gt_cp.fill(0);
    vcl_vector<unsigned> column2check;
    grouped_gt_cp[1][j]=1;
    column_checked.insert(j);
    column2check.push_back(j);

    vnl_vector<double> col_v = cost_gt2cp.get_column(j);
    if(col_v.min_value()==1000)
    {
      column_checked.erase(j);
      grouped_gt_cp[1][j]=0;
      continue;
    }
    check_column(cost_gt2cp,cost_cp2gt,column2check,row_checked,column_checked,grouped_gt_cp);

    if(grouped_gt_cp.get_row(0).max_value()==0 || grouped_gt_cp.get_row(1).max_value()==0)
      continue;

    f_it_0 = CFG_2.frags.begin();
    dbdet_edgel_chain_list gt_frags;
    for(unsigned k=0;k<grouped_gt_cp.cols()&&f_it_0!=CFG_2.frags.end();k++,f_it_0++)
    {
      if(grouped_gt_cp[0][k]!=0)
      {
	gt_frags.push_back(*f_it_0);
      }
    }


    f_it_1 = CFG_3.frags.begin();
    dbdet_edgel_chain_list cp_frags;
    for(unsigned k=0;k<grouped_gt_cp.cols()&&f_it_1!=CFG_3.frags.end();k++,f_it_1++)
    {
      if(grouped_gt_cp[1][k]!=0)  // for grouped CC
      {
	cp_frags.push_back(*f_it_1);
      }
    }

    if(!bgroup_only)
    {
    vcl_vector<vcl_list<dbdet_edgel_chain_list> > gt_and_cp_groups = edit_distance_process(gt_frags, cp_frags);
    
    vcl_list<dbdet_edgel_chain_list> insert_gt_group = gt_and_cp_groups[0];
    vcl_list<dbdet_edgel_chain_list> insert_cp_group = gt_and_cp_groups[1];	
    // push the groups to show in GUI
    gt_group.insert(gt_group.end(),insert_gt_group.begin(), insert_gt_group.end());
    cp_group.insert(cp_group.end(),insert_cp_group.begin(), insert_cp_group.end());
    //vcl_cout<<"edit distance match done! "<<vcl_endl;
    }
    else
    {
	    gt_group.push_back(gt_frags);
	    cp_group.push_back(cp_frags);
    }
  }

  for(i=0;i<row;i++) // start by checked rows in cost(cc-gt) matrix 
  {
    if(row_checked.find(i)!=row_checked.end())
        continue;
    grouped_gt_cp.fill(0);
    vcl_vector<unsigned> row2check;
    grouped_gt_cp[0][i]=1;
    row_checked.insert(i);
    row2check.push_back(i);

    vnl_vector<double> row_v = cost_cp2gt.get_row(i);
    if(row_v.min_value()==1000)
    {
      row_checked.erase(i);
      grouped_gt_cp[0][i]=0;
      continue;
    }
    check_row(cost_gt2cp,cost_cp2gt,row2check,row_checked,column_checked,grouped_gt_cp);

    if(grouped_gt_cp.get_row(0).max_value()==0 || grouped_gt_cp.get_row(1).max_value()==0)
      continue;

    f_it_0 = CFG_2.frags.begin();
    dbdet_edgel_chain_list gt_frags;
    for(unsigned k=0;k<grouped_gt_cp.cols()&&f_it_0!=CFG_2.frags.end();k++,f_it_0++)
    {
      if(grouped_gt_cp[0][k]!=0)
      {
	gt_frags.push_back(*f_it_0);
      }
    }


    f_it_1 = CFG_3.frags.begin();
    dbdet_edgel_chain_list cp_frags;
    for(unsigned k=0;k<grouped_gt_cp.cols()&&f_it_1!=CFG_3.frags.end();k++,f_it_1++)
    {
      if(grouped_gt_cp[1][k]!=0)  // for grouped CC
      {
	cp_frags.push_back(*f_it_1);
      }
    }

    if(!bgroup_only)
    {
	    vcl_vector<vcl_list<dbdet_edgel_chain_list> > gt_and_cp_groups = edit_distance_process(gt_frags, cp_frags);
	    
	    vcl_list<dbdet_edgel_chain_list> insert_gt_group = gt_and_cp_groups[0];
	    vcl_list<dbdet_edgel_chain_list> insert_cp_group = gt_and_cp_groups[1];	
	    // push the groups to show in GUI
	    gt_group.insert(gt_group.end(),insert_gt_group.begin(), insert_gt_group.end());
	    cp_group.insert(cp_group.end(),insert_cp_group.begin(), insert_cp_group.end());
	    //vcl_cout<<"edit distance match done! "<<vcl_endl;
    }
    else
    {
	    gt_group.push_back(gt_frags);
	    cp_group.push_back(cp_frags);
    }
  }

  double grouping_time = t.real() / 1000.0;
  vcl_cout << "Time taken to goupe GT and CC contours: " << grouping_time << " sec" << vcl_endl;
  t.mark();

  vcl_cout<<"Evaluation Done"<<vcl_endl;


  //set up color for prune contours
  output_sel_2->set_prune_color(0.5f, 0.0f, 0.0f);
  output_sel_3->set_prune_color(0.0f, 0.5f, 0.0f);

  //build a color matrix
  int group_num = gt_group.size();
  vnl_matrix<double> color_mat(2*group_num,3), color_mat_2(2*group_num,3);
  for(unsigned i=0;i<color_mat.rows();i++)
  {
     do
     {
	     color_mat[i][0]=(rand()%256)/255.0; color_mat_2[i][0] = color_mat[i][0]*0.6;
	     color_mat[i][1]=(rand()%256)/255.0; color_mat_2[i][1] = color_mat[i][1]*0.6;
	     color_mat[i][2]=(rand()%256)/255.0; color_mat_2[i][2] = color_mat[i][2]*0.6;
     }
     while((color_mat[i][0] > 0.5 && color_mat[i][1] < 0.02 && color_mat[i][2] < 0.02)||(color_mat[i][0] < 0.02 && color_mat[i][1] > 0.5 && color_mat[i][2] < 0.02));
  }

  output_sel_2->set_color_mat(color_mat);
  output_sel_3->set_color_mat(color_mat_2);
 
  //referance all the groups fragments
  dbdet_edgel_chain_list& all_gt_group_frags = output_sel_2->all_groups_frags();
  dbdet_edgel_chain_list& all_cp_group_frags = output_sel_3->all_groups_frags();

  //save all the prune gt contours and compute matched contours' length
  f_it_0 = CFG_2.frags.begin();
  for(;f_it_0!=CFG_2.frags.end();f_it_0++)
  { 	
     if(vcl_find(all_gt_group_frags.begin(), all_gt_group_frags.end(), *f_it_0)==all_gt_group_frags.end())
	gt_prune_frags.push_back(*f_it_0);
     else
	TP_gt_l += contour_length(*f_it_0);
  }

  //save all the pruned cp contours
  f_it_1 = CFG_3.frags.begin();

  for(;f_it_1!=CFG_3.frags.end();f_it_1++)
  { 	
     if(vcl_find(all_cp_group_frags.begin(), all_cp_group_frags.end(), *f_it_1)==all_cp_group_frags.end())
	cp_prune_frags.push_back(*f_it_1);
     else
	TP_cp_l += contour_length(*f_it_1);
  }

//  vcl_cout<<"prune_gt: "<<gt_prune_frags.size()<<vcl_endl;
//  vcl_cout<<"prune_cp: "<<cp_prune_frags.size()<<vcl_endl;

//  vcl_ofstream myfile;
//  myfile.open ("/home/guoy/Desktop/num_of_groups.txt");

  vcl_list<dbdet_edgel_chain_list>::iterator iter_0 = gt_group.begin();
  vcl_list<dbdet_edgel_chain_list>::iterator iter_1 = cp_group.begin();
  for(; iter_0!= gt_group.end(); iter_0++, iter_1++)
  {
	frags_combination* combo_0 = new frags_combination();
     	frags_combination* combo_1 = new frags_combination();
        combo_0->insert_frags(*iter_0);
	combo_1->insert_frags(*iter_1);

	//vcl_cout<<(*iter_0).size() << " " << (*iter_1).size() <<vcl_endl;
        //myfile << (*iter_0).size() << " " << (*iter_1).size() <<vcl_endl;

        total_edit_dist += compute_transform_cost(combo_0, combo_1)*(combo_length(combo_0)+combo_length(combo_1));
        delete combo_0;
	delete combo_1;
  }


//  myfile.close();

  total_edit_dist += (gt_length - TP_gt_l + cp_length - TP_cp_l);
  total_edit_dist /= cp_length;

  double recall = TP_gt_l/gt_length;
  double precision = TP_cp_l/cp_length;
  vcl_cout << "TP GP length: "<< TP_gt_l << " Total GT length: "<< gt_length << " Recall: "<< recall <<vcl_endl;
  vcl_cout << "TP CP length: "<< TP_cp_l << " Total CP length: "<< cp_length << " Precision: "<< precision <<vcl_endl;
  vcl_cout << "F measure: "<< 2*recall*precision/(recall+precision) << vcl_endl;
  vcl_cout << "Total Edit Distance "<< total_edit_dist << vcl_endl; 
  // output the sel storage class
  output_data_[0].push_back(output_sel_2);
  output_data_[0].push_back(output_sel_3);
  return true;
}

bool
dbdet_evaluation_process::finish()
{
  return true;
}

// compute contour length
static double contour_length(dbdet_edgel_chain* const &c)
{
  vcl_deque<dbdet_edgel* > chain = c->edgels;
  double total_ds = 0.0;
  for (unsigned i=1; i<chain.size(); i++)
  {
    dbdet_edgel* eA = chain[i];
    dbdet_edgel* eP = chain[i-1];

    //compute ds
    double ds = vgl_distance(eA->pt, eP->pt);
    total_ds += ds;
  }
  return total_ds;
}

//check columns to get frags for group
void
dbdet_evaluation_process::check_column(vnl_matrix<double> cost_gt2cp,vnl_matrix<double> cost_cp2gt,vcl_vector<unsigned>& column2check, vcl_set<unsigned>& row_checked,vcl_set<unsigned>& column_checked,vnl_matrix<unsigned>& grouped_gt_cp)
{
    vcl_vector<unsigned>::iterator vit=column2check.begin();
	for(;vit!=column2check.end();vit++)
	{
	  vnl_vector<double> col_v=cost_gt2cp.get_column(*vit);
	  if(col_v.min_value()==1000)
            continue;
	  vcl_vector<unsigned> row2check;

          double temp_min=1000;
          for(unsigned i=0;i<col_v.size();i++)
	    {
	      if(row_checked.find(i)!=row_checked.end())
	        continue;
	      if(col_v[i]!=1000)  // find the group frag 
	      {
                vnl_vector<double> row_i = cost_gt2cp.get_row(i);
                
		if(row_i.min_value()!=col_v[i] && col_v[i] < cost_thresh)// if this short frag matching other long frag better than cc(j), leave it for the future
			continue;

	        grouped_gt_cp[0][i]=1;//remember its id in grouped_gt_cp
                if(col_v[i]<temp_min)
 		{
		  temp_min=col_v[i];
		  row2check.insert(row2check.begin(),i);// push its id into the first place row2check
		}
                else
		{
	          row2check.push_back(i);// push its id into row2check
		}
	        row_checked.insert(i);// push its id into row_checked
	      }
	    }
	  if(!row2check.empty())
	    check_row(cost_gt2cp,cost_cp2gt,row2check,row_checked,column_checked,grouped_gt_cp);
	}
	column2check.clear();
}

//check rows to get frags for group
void 
dbdet_evaluation_process::check_row(vnl_matrix<double> cost_gt2cp,vnl_matrix<double> cost_cp2gt,vcl_vector<unsigned>& row2check, vcl_set<unsigned>& row_checked,vcl_set<unsigned>& column_checked,vnl_matrix<unsigned>& grouped_gt_cp)
{
    vcl_vector<unsigned>::iterator vit=row2check.begin();
	for(;vit!=row2check.end();vit++)
	{
	  vnl_vector<double> row_v=cost_cp2gt.get_row(*vit);
	  if(row_v.min_value()==1000)
            continue;
	  vcl_vector<unsigned> column2check;

          double temp_min=1000;
          for(unsigned j=0;j<row_v.size();j++)
	  {
	    if(column_checked.find(j)!=column_checked.end())
	      continue;
	    if(row_v[j]!=1000)  // find the group frag 
	    {
	      vnl_vector<double> col_j = cost_cp2gt.get_column(j);
	      if(col_j.min_value()!=row_v[j] && row_v[j]< cost_thresh) // if this short frag matching other long frag better than cc(j), leave it for the future
		 continue;
             
	      grouped_gt_cp[1][j]=1;//remember its id in grouped_gt_cp
              if(row_v[j]<temp_min)
 	      {
		temp_min=row_v[j];
		column2check.insert(column2check.begin(),j);// push its id into the first place row2check
	      }
	      else
	      {
	        column2check.push_back(j);// push its id into column2check
	      }
	      column_checked.insert(j);// push its id into column_checked
	    }
	  }
	  if(!column2check.empty())
	    check_column(cost_gt2cp,cost_cp2gt,column2check,row_checked,column_checked,grouped_gt_cp);
	}
	row2check.clear();
}

//compute the distance between two edgels
double 
dbdet_evaluation_process::compute_edgel_dist(dbdet_edgel* e1, dbdet_edgel* e2)
{
  return vgl_distance(e1->pt, e2->pt);
}

double 
dbdet_evaluation_process::compute_ori_diff(dbdet_edgel* e1, dbdet_edgel* e2)
{
	double dir_1 = e1->tangent;
	double dir_2 = e2->tangent;

	if(dir_1 >= pi)
		dir_1 -= pi;
	if(dir_2 >= pi)
		dir_2 -= pi;

	double d_theta = vcl_abs(dir_2-dir_1);
	if(d_theta >= pi*0.5)
		d_theta = pi - d_theta;
	return d_theta;
}
//compute the cost from contour c1 to c2
// We apply the oriented chamfer Matching here to computer the cost (c1->c2) = 1/N1 * sum ((1-w)*dist(i,min_j))+w*d_dir(i, min_j)) 
double 
dbdet_evaluation_process::compute_contours_cost(dbdet_edgel_chain* const &c1, dbdet_edgel_chain* const &c2)
{
  //double w = 0.5;
  int length_1 = c1->edgels.size(), length_2 = c2->edgels.size();
  double sum=0.0, cost=0;

  int sum_overlap = 0;
  for(int i=0; i< length_1; i++)
  {
  	double min_value = 1000;
	int min_j = 0;
	for(int j=0; j<length_2; j++)
	{
		double d = compute_edgel_dist(c1->edgels[i], c2->edgels[j]);
		if(d<min_value)
		{
			min_value=d;
			min_j =j;
		}
	}
	
	double d_theta = compute_ori_diff(c1->edgels[i], c2->edgels[min_j]);

	/*if(c1->edgels[i]->pt.x() == 56.5861 && c1->edgels[i]->pt.y() == 26.428 && c2->edgels[min_j]->pt.x() == 56.75 && c2->edgels[min_j]->pt.y() == 25.25)
		vcl_cout<<"!!!!!!!!!!!!! "<< d_theta <<vcl_endl;*/

	if(min_value < cost_thresh && d_theta < (pi*0.25))
	  sum_overlap ++;
	sum += (min_value + d_theta);
  }

  cost = sum/double(length_1);

  //if(cost > cost_thresh)
  if(double(sum_overlap)/double(length_1) >= 0.7 || sum_overlap >=20) // if less than half of the c1 overlap c2, take the cost as 1000
	return cost;
  return 1000;
}

//this function is to apply edit distance method in matching contour combination between GT group and Cp group. Consideration all the combinations of contours within each group.
vcl_vector<vcl_list<dbdet_edgel_chain_list> > 
dbdet_evaluation_process::edit_distance_process(dbdet_edgel_chain_list &gt_frags, dbdet_edgel_chain_list &cp_frags)
{
	//vcl_cout << "GT frags: "<< gt_frags.size()<<vcl_endl;
	//vcl_cout << "CP frags: "<< cp_frags.size()<<vcl_endl;
	//vcl_cout << "combinations of GT contours" << vcl_endl;
	vcl_list<frags_combination*> gt_all_combinations;
	vcl_list<frags_combination*> gt_1;
	vcl_list<dbdet_edgel_chain*>::const_iterator f_it_0 = gt_frags.begin(); int i = 0;
	//always construct a full contours combination
	frags_combination* full_gt_combo = new frags_combination();
        // build up "1" contour list
	for(;f_it_0!=gt_frags.end();f_it_0++, i++)
	{
		dbdet_edgel_chain* c1 = *f_it_0;
		frags_combination* combo = new frags_combination(c1, i);
		gt_1.push_back(combo);
		full_gt_combo->insert_frags(combo);
		//vcl_cout<< i << vcl_endl;
	}

	gt_all_combinations.insert(gt_all_combinations.end(), gt_1.begin(), gt_1.end());
	vcl_list<frags_combination*> prev_c = gt_1;
	vcl_list<frags_combination*> next_c = generate_combination(prev_c, gt_1, gt_combos_from_grouping);
	while (next_c.size() > 0)
	{
		gt_all_combinations.insert(gt_all_combinations.end(), next_c.begin(), next_c.end());
		prev_c = next_c;
		next_c = generate_combination(prev_c, gt_1, gt_combos_from_grouping);
	}


	//vcl_cout << "combinations of CP contours" << vcl_endl;
	vcl_list<frags_combination*> cp_all_combinations;
	vcl_list<frags_combination*> cp_1;
	f_it_0 = cp_frags.begin(); i = 0;
	//always build up a full cp combo 
	frags_combination* full_cp_combo = new frags_combination();
        // build up "1" contour list
	for(;f_it_0!=cp_frags.end();f_it_0++, i++)
	{
		dbdet_edgel_chain* c1 = *f_it_0;
		frags_combination* combo = new frags_combination(c1, i);
		cp_1.push_back(combo);
		full_cp_combo->insert_frags(combo);
		//vcl_cout<< i << vcl_endl;
	}
	cp_all_combinations.insert(cp_all_combinations.end(), cp_1.begin(), cp_1.end());
	prev_c = cp_1;
	next_c= generate_combination(prev_c, cp_1, cp_combos_from_grouping);
	while (next_c.size() > 0)
	{
		cp_all_combinations.insert(cp_all_combinations.end(), next_c.begin(), next_c.end());
		prev_c = next_c;
		next_c = generate_combination(prev_c, cp_1, cp_combos_from_grouping);
	}

	//Only if is is one gt vs mutiple cp, or mutiple gt vs one gt, we insert the full combos
	// ******************  this is importance to recuce the risk that overlook a lot of necessuary penalty *****************************8
	if(cp_1.size() ==1)
		gt_all_combinations.push_back(full_gt_combo);
	if(gt_1.size() ==1)
		cp_all_combinations.push_back(full_cp_combo);

	//return updated matching contours combinations, preparing to show in GUI
	vcl_list<dbdet_edgel_chain_list> gt_group;
	vcl_list<dbdet_edgel_chain_list> cp_group;

	// compute deform cost for between combinations in GT and CP group, find the combinations match with lowest cost and delete their sub combinations 
	int gt_size = gt_all_combinations.size();
	int cp_size = cp_all_combinations.size();

	//vcl_cout << "GT combinations: "<< gt_size<<vcl_endl;
	//vcl_cout << "CP combinations: "<< cp_size<<vcl_endl;
	// do this process iteratively, util only gt or cp combinations left, take the remaining as contours to purge
	while(gt_size > 0 && cp_size >0)
	{
		// find the matching combinations with lowest cost
		vcl_list<frags_combination*>::const_iterator c_it_0 = gt_all_combinations.begin(); double min_cost = 1000;
		frags_combination* min_gt = gt_all_combinations.front();
		frags_combination* min_cp = cp_all_combinations.front();
		for(; c_it_0!= gt_all_combinations.end(); c_it_0++)
		{
			vcl_list<frags_combination*>::const_iterator c_it_1 = cp_all_combinations.begin();
			for(; c_it_1!= cp_all_combinations.end(); c_it_1++)
			{
				double trans_cost = compute_transform_cost((*c_it_0), (*c_it_1));
				if(trans_cost < min_cost)
				{
					min_cost = trans_cost;
					min_gt = *c_it_0;
					min_cp = *c_it_1;
				}
			}
		}

		if(min_cost >= edit_thresh)// when the left gt and cp combinations can not be matched, don't continue
		{
			break;
		}
		//vcl_cout << "min_cost: " << min_cost<<vcl_endl;
		//vcl_cout << "with gt:" << min_gt << " cp:" << min_cp << vcl_endl;
		gt_group.push_back(min_gt->frags);
		cp_group.push_back(min_cp->frags);

		//delete sub combinations
		for(c_it_0 = gt_all_combinations.begin(); c_it_0 != gt_all_combinations.end(); c_it_0++)
		{
			frags_combination* combo_1 = *c_it_0;
			if(is_sub_combo(min_gt, combo_1))
			{
				gt_all_combinations.remove(combo_1);
				c_it_0--;
			}	
		}

		for(c_it_0 = cp_all_combinations.begin(); c_it_0 != cp_all_combinations.end(); c_it_0++)
		{
			frags_combination* combo_1 = *c_it_0;
			if(is_sub_combo(min_cp, combo_1))
			{
				cp_all_combinations.remove(combo_1);
				c_it_0--;
			}	
		}

		gt_size = gt_all_combinations.size();
		cp_size = cp_all_combinations.size();
	}
	vcl_vector<vcl_list<dbdet_edgel_chain_list> > results;
	results.push_back(gt_group);
	results.push_back(cp_group);
	return results;
}

// check if combo_1 is sub combination of combo_0 (contain c1 \\ c2 \\ ... cn within combo_0)
bool 
dbdet_evaluation_process::is_sub_combo(frags_combination* combo_0, frags_combination* combo_1)
{
	vcl_list<dbdet_edgel_chain*>::const_iterator f_it_0 = combo_0->frags.begin();
	for (; f_it_0 != combo_0->frags.end(); f_it_0++)
	{
		if(vcl_find(combo_1->frags.begin(), combo_1->frags.end(), *(f_it_0)) != combo_1->frags.end())
			return true;
	}
	return false;
}

double
dbdet_evaluation_process::combo_length(frags_combination* combo_0)
{
	double combo_length_0 = 0;
	// compute combo_0 total length
	vcl_list<dbdet_edgel_chain*>::const_iterator f_it_0 = combo_0->frags.begin();
	for (; f_it_0 != combo_0->frags.end(); f_it_0++)
	{	
		dbdet_edgel_list n_chain = (*f_it_0)->edgels;
		for (unsigned i=1; i<n_chain.size(); i++)
		{      		
			double dist = compute_edgel_dist(n_chain[i-1], n_chain[i]);
			combo_length_0 += dist;
		}
	}
	return combo_length_0;
}

double
dbdet_evaluation_process::compute_transform_cost(frags_combination* combo_0, frags_combination* combo_1)
{
	dbdet_edgel_list all_edgels_0, all_edgels_1;
	double combo_length_0 = 0, combo_length_1 = 0;
// compute combo_0 total length
	vcl_list<dbdet_edgel_chain*>::const_iterator f_it_0 = combo_0->frags.begin();
	for (; f_it_0 != combo_0->frags.end(); f_it_0++)
	{	
		dbdet_edgel_list n_chain = (*f_it_0)->edgels;
		all_edgels_0.push_back(n_chain[0]);
		for (unsigned i=1; i<n_chain.size(); i++)
		{      		
			all_edgels_0.push_back(n_chain[i]);
			double dist = compute_edgel_dist(n_chain[i-1], n_chain[i]);
			combo_length_0 += dist;
		}
	}
// compute combo_1 total length
	vcl_list<dbdet_edgel_chain*>::const_iterator f_it_1 = combo_1->frags.begin();
	for (; f_it_1 != combo_1->frags.end(); f_it_1++)
	{	
		dbdet_edgel_list n_chain = (*f_it_1)->edgels;
		all_edgels_1.push_back(n_chain[0]);
		for (unsigned i=1; i<n_chain.size(); i++)
		{
      			all_edgels_1.push_back(n_chain[i]);
			double dist = compute_edgel_dist(n_chain[i-1], n_chain[i]);
			combo_length_1 += dist;			
		}
	}

	vcl_vector<double> v_0(all_edgels_0.size(), 1000), v_1(all_edgels_1.size(), 1000);
	
// build up two vector to record whether each eagel have matched edgel. if not, mark 1000, if yes, mark the min dist. 	

	vcl_vector<int> used_v_1(all_edgels_1.size(),0), used_v_0(all_edgels_0.size(),0);

	for(unsigned i=0;i<all_edgels_0.size();i++)
	{
		double min_dist = 1000;
		int min_j=0;
		for(unsigned j=0;j<all_edgels_1.size();j++){
			double d = compute_edgel_dist(all_edgels_0[i], all_edgels_1[j]);
			//double ori_d = compute_ori_diff(all_edgels_0[i], all_edgels_1[j]);
			if(d<=min_dist)// && used_v_1[j]==0)
			{
				min_dist = d;
				min_j = j;
			} 
		}
		if(min_dist<local_dist)
		{
			v_0[i] = vcl_pow(min_dist/local_dist, 4) + 0.6*compute_ori_diff(all_edgels_0[i], all_edgels_1[min_j]);
			//used_v_1[min_j] = 1;
			//used_v_0[i] = 1;
		}
	}

	for(unsigned i=0;i<all_edgels_1.size();i++)
	{
		double min_dist = 1000;
		//if(used_v_1[i]==1)
		//{
		//	v_1[i]=0;// input the matched edgels in v_1 cost as 0
		//	continue;
		//}
		int min_j = 0;
		for(unsigned j=0;j<all_edgels_0.size();j++){
			double d = compute_edgel_dist(all_edgels_1[i], all_edgels_0[j]);
			if(d<=min_dist)// && used_v_0[j] ==0)
			{
				min_dist = d;
				min_j = j;
			}
		}
		if(min_dist<local_dist)
		{
			v_1[i]= vcl_pow(min_dist/local_dist,4) + 0.6*compute_ori_diff(all_edgels_1[i], all_edgels_0[min_j]);
		//	used_v_0[min_j] = 1;
		//	used_v_1[i] = 1;			
		}
	}
// compute the tranformation cost as the length of unmatched [1000, 1000, 1000 ...] segments, plus the localization cost 
	double deform_length_0 = 0, deform_length_1 = 0, local_cost_0 = 0, local_cost_1 = 0;
	for (int i = 0; i < (v_0.size()-1); i++)
	{
		double dist = compute_edgel_dist(all_edgels_0[i], all_edgels_0[i+1]);
		if(v_0[i]==1000 && v_0[i+1]==1000)		
			deform_length_0 += dist;
		if(v_0[i]!=1000 && v_0[i+1]!=1000)
			local_cost_0 += (v_0[i]+v_0[i+1])*dist/2;
	}


	for (int i = 0; i < (v_1.size()-1); i++)
	{
		double dist = compute_edgel_dist(all_edgels_1[i], all_edgels_1[i+1]);
		if(v_1[i]==1000 && v_1[i+1]==1000)
			deform_length_1 += dist;
		if(v_1[i]!=1000 && v_1[i+1]!=1000)
			local_cost_1 += (v_1[i]+v_1[i+1])*dist/2;
	}

	// for the case a large portion is to prune, just return a super large cost
        //if((deform_length_0/combo_length_0)>0.7 || (deform_length_1/combo_length_1) >0.7)
	//	return 1000;
	return ((deform_length_0 + deform_length_1) + (local_cost_0 + local_cost_1)/2 + 2*(combo_0->frags.size() + combo_1->frags.size()-2))/(combo_length_0 + combo_length_1);
}

// function to check is the second combo is overlopping with part (over 5 edgels) of first combo
bool
dbdet_evaluation_process::overlap_combos(frags_combination* const &combo_0,  frags_combination* const &combo_1)
{
	dbdet_edgel_list all_edgels_0;
	dbdet_edgel_list all_edgels_1;
	vcl_list<dbdet_edgel_chain*>::const_iterator f_it_0 = combo_0->frags.begin();
	for (; f_it_0 != combo_0->frags.end(); f_it_0++)
	{
		dbdet_edgel_list n_chain = (*f_it_0)->edgels;
		for (unsigned i=0; i<n_chain.size(); i++)
  			all_edgels_0.push_back(n_chain[i]);
	}
	// here just assume the combo_1 only have one fragment, because here is the only case we use.
	all_edgels_1 = combo_1->frags.front()->edgels;

	if(all_edgels_1.size() < 5)
		return false;
	int overlap=0;
	for(unsigned i=0;i<all_edgels_1.size();i++){
		for(unsigned j=0;j<all_edgels_0.size();j++){
			double d = compute_edgel_dist(all_edgels_1[i], all_edgels_0[j]);
			if(d<= 2*local_dist && compute_ori_diff(all_edgels_1[i], all_edgels_0[j])<0.16*pi)
			{
				overlap++;
				break;
			}
		}
		if(overlap >= 12 || double(overlap)/double(all_edgels_1.size())>0.6 )// also consider very short curves 
			return true;
	}	
	return false;
}

vcl_list<frags_combination*> 
dbdet_evaluation_process::generate_combination(vcl_list<frags_combination*> const &combos,  vcl_list<frags_combination*> const &combos_1, vcl_list<frags_combination*> const &group_combos)
{
	//vcl_cout<<"combos_0 size "<<combos.size()<<vcl_endl;
	//vcl_cout<<"combos_1 size "<<combos_1.size()<<vcl_endl;
	bool is_feasible = 1; // this parameter is to ensure computation feasible, but reduce performance in some cases
	if(combos_1.size()>30 || combos.size()>50)
		is_feasible = 0;
	vcl_list<frags_combination*> new_combo_list;
	vcl_list<frags_combination*>::const_iterator c_it_0 = combos.begin();

	for(;c_it_0 != combos.end();c_it_0++)
	{
		frags_combination* combo_0 = *c_it_0;
		vcl_list<frags_combination*>::const_iterator c_it_1 = combos_1.begin();
		for(;c_it_1 != combos_1.end();c_it_1++)
		{
			
			frags_combination* combo_1 = *c_it_1;
			// only consider combo_1 is not included in combo_0, and combo_1 is not overlapping with part of combo_0
			if(vcl_find(combo_0->frags.begin(), combo_0->frags.end(), combo_1->frags.front())==combo_0->frags.end()  && !overlap_combos(combo_0, combo_1) )
			{
				frags_combination* new_combo = new frags_combination();
				// find the cloest two end points of the two input combos, and assign the other two end points as new_combo's end points 
				double min_dist = 1000;

				dbdet_edgel* merge_point_0;
				dbdet_edgel* merge_point_1;

				double dist_0 = compute_edgel_dist(combo_0->start_point,combo_1->start_point);
				if(dist_0<min_dist)
				{
					new_combo->start_point = combo_0->end_point;
					new_combo->end_point = combo_1->end_point;
					min_dist = dist_0;
					merge_point_0 = combo_0->start_point;
					merge_point_1 = combo_1->start_point;
				}
				double dist_1 = compute_edgel_dist(combo_0->start_point,combo_1->end_point);
				if(dist_1<min_dist)
				{
					new_combo->start_point = combo_0->end_point;
					new_combo->end_point = combo_1->start_point;
					min_dist = dist_1;
					merge_point_0 = combo_0->start_point;
					merge_point_1 = combo_1->end_point;
				}
				double dist_2 = compute_edgel_dist(combo_0->end_point,combo_1->start_point);
				if(dist_2<min_dist)
				{
					new_combo->start_point = combo_0->start_point;
					new_combo->end_point = combo_1->end_point;
					min_dist = dist_2;
					merge_point_0 = combo_0->end_point;
					merge_point_1 = combo_1->start_point;
				}
				double dist_3 = compute_edgel_dist(combo_0->end_point,combo_1->end_point);
				if(dist_3<min_dist)
				{
					new_combo->start_point = combo_0->start_point;
					new_combo->end_point = combo_1->start_point;
					min_dist = dist_3;
					merge_point_0 = combo_0->end_point;
					merge_point_1 = combo_1->end_point;			
				}

				new_combo->insert_frags(combo_0);
				new_combo->insert_frags(combo_1);
				// check if this combination already exist both in new_combo_list
				bool is_exist = false;
				vcl_list<frags_combination*>::const_iterator c_it_2 = new_combo_list.begin();
				for(; c_it_2 != new_combo_list.end(); c_it_2++)
				{
					if ((*c_it_2)->equals(new_combo))
					{
						is_exist = true;
						delete new_combo;
						break;
					}
				}
				if(is_exist)
					continue;
				// check if new combo can form a closed circular contour
				if((dist_1<local_dist && dist_2<local_dist) || (dist_0<local_dist && dist_3<local_dist))
				{
					new_combo_list.push_back(new_combo);
					continue;
				}
					

				//compute the direction of combo_0 and combo_1 next merge_point_0 and merge_point_1
				double d_x_0 =0, d_y_0 = 0, d_x_1 = 0, d_y_1 =0;

				double d_x_merge = merge_point_1->pt.x() - merge_point_0->pt.x();
				double d_y_merge = merge_point_1->pt.y() - merge_point_0->pt.y();

				dbdet_edgel_list all_edgels_0;
				vcl_list<dbdet_edgel_chain*>::const_iterator f_it_0 = combo_0->frags.begin();
				for (; f_it_0 != combo_0->frags.end(); f_it_0++)
				{	
					dbdet_edgel_list n_chain = (*f_it_0)->edgels;
					if(merge_point_0 == n_chain.front())
					{
						if(n_chain.size()>=5)
						{
							d_x_0 = merge_point_0->pt.x() - n_chain[4]->pt.x();
							d_y_0 = merge_point_0->pt.y() - n_chain[4]->pt.y();
						}
						else
						{
							d_x_0 = merge_point_0->pt.x() - n_chain.back()->pt.x();
							d_y_0 = merge_point_0->pt.y() - n_chain.back()->pt.y();
						}
					}
					if(merge_point_0 == n_chain.back())
					{
						if(n_chain.size()>=5)
						{
							int n = n_chain.size()-5;
							d_x_0 = merge_point_0->pt.x() - n_chain[n]->pt.x();
							d_y_0 = merge_point_0->pt.y() - n_chain[n]->pt.y();
						}
						else
						{
							d_x_0 = merge_point_0->pt.x() - n_chain.front()->pt.x();
							d_y_0 = merge_point_0->pt.y() - n_chain.front()->pt.y();
						}						
					}
					for (unsigned i=0; i<n_chain.size(); i++)
					{
			      			all_edgels_0.push_back(n_chain[i]);
					}
				}
				dbdet_edgel_list all_edgels_1;
				all_edgels_1 = combo_1->frags.front()->edgels;
				if(merge_point_1 == all_edgels_1.front())
				{
					if(all_edgels_1.size()>=5)
					{
						d_x_1 = merge_point_1->pt.x() - all_edgels_1[4]->pt.x();
						d_y_1 = merge_point_1->pt.y() - all_edgels_1[4]->pt.y();
					}
					else
					{
						d_x_1 = merge_point_1->pt.x() - all_edgels_1.back()->pt.x();
						d_y_1 = merge_point_1->pt.y() - all_edgels_1.back()->pt.y();
					}
				}
				if(merge_point_1 == all_edgels_1.back())
				{
					if(all_edgels_1.size()>=5)
					{
						int n = all_edgels_1.size()-5;
						d_x_1 = all_edgels_1[n]->pt.x() - merge_point_1->pt.x();
						d_y_1 = all_edgels_1[n]->pt.y() - merge_point_1->pt.y();
					}
					else
					{
						d_x_1 = all_edgels_1.back()->pt.x() - merge_point_1->pt.x();
						d_y_1 = all_edgels_1.back()->pt.y() - merge_point_1->pt.y();
					}						
				}
				
				// check if the new combination is feasible, if not, delete
				// end points must be close enough
				if(min_dist < 2*local_dist)
				{
						new_combo_list.push_back(new_combo);
						/*for(int k=0; k<new_combo->index.size(); k++)
							vcl_cout << new_combo->index[k] << " ";
						vcl_cout << vcl_endl;*/
				}
				// if the cloeset end points further then local_dist, check if there is other contours lie between them, if not, approve the combination
				else if(min_dist >= 2*local_dist && min_dist < 10 && is_feasible)
				{
					bool is_approved = 1;
					if((d_x_0*d_x_merge + d_y_0*d_y_merge)/vcl_sqrt(d_x_0*d_x_0 + d_y_0*d_y_0)/vcl_sqrt(d_x_merge*d_x_merge + d_y_merge*d_y_merge) < 0.866 && (d_x_merge*d_x_1 + d_y_merge*d_y_1)/vcl_sqrt(d_x_merge*d_x_merge + d_y_merge*d_y_merge)/vcl_sqrt(d_x_1*d_x_1 + d_y_1*d_y_1) < 0.866)
						is_approved = 0;
					if((d_x_0*d_x_1 + d_y_0*d_y_1)/vcl_sqrt(d_x_0*d_x_0 + d_y_0*d_y_0)/vcl_sqrt(d_x_1*d_x_1 + d_y_1*d_y_1) > 0.866)
						is_approved = 1;

					vcl_list<frags_combination*>::const_iterator c_it_2 = combos_1.begin();
					for(;c_it_2 != combos_1.end() && is_approved!=0;c_it_2++)
					{
						frags_combination* combo_2 = *c_it_2;
						// for the situation combo_2 not equals to combo_1, and is not included in combo_0
						if ((combo_2==combo_1) || 
						vcl_find((combo_0->frags).begin(),(combo_0->frags).end(),(combo_2->frags).front())!=(combo_0->frags).end())
						{
							continue;
						}
						else
						{
							// if there is other contour lying between the connecting points 
							int sum_inside_edges = 0;
							dbdet_edgel_list chain = combo_2->frags.front()->edgels;
							for (int n = 0; n<chain.size(); n++)
							{
								double dist_0 = compute_edgel_dist(chain[n], merge_point_0);
								double dist_1 = compute_edgel_dist(chain[n], merge_point_1);
								if(dist_0<min_dist && dist_1<min_dist)
									sum_inside_edges ++;
								if( double(sum_inside_edges)/double(chain.size())>0.6 || sum_inside_edges > 12) // also consider very short contours
								{
									is_approved = 0;
								 	break;
								}
							}													
						}
					}
					if(is_approved)
					{
						new_combo_list.push_back(new_combo);	
						/*for(int k=0; k<new_combo->index.size(); k++)
							vcl_cout << new_combo->index[k] << " ";
						vcl_cout << "insert" << vcl_endl;*/
					}
					else
					{
						delete new_combo;
					}
				}
				else
					delete new_combo;
			}
		}
	
	}
	return new_combo_list;	
}

