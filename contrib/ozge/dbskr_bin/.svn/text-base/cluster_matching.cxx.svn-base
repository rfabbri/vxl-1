#ifndef DBSKR_CLUSTER_DB_MATCHING_CXX_
#define DBSKR_CLUSTER_DB_MATCHING_CXX_

// Includes
#include <mpi.h>
#include <stdarg.h>
#include <stdio.h>

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/dbskr_utilities.h>
#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbskr/dbskr_tree_sptr.h>
#include <dbskr/dbskr_tree.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_plane.h>
#include <brip/brip_vil_float_ops.h>

#include <vul/vul_timer.h>
#include <vul/vul_file.h>
#include <dbskr/bin/cluster_matching.h>

dbskr_cluster_matching::dbskr_cluster_matching (vcl_string view_str, 
                                                vcl_string patch_dir, 
                                                vcl_string match_dir,
                                                vcl_string sampling_ds_str,
                                                int verbose) 
   : m_verbose (verbose), patch_dir_(patch_dir), match_dir_(match_dir), sampling_ds_str_(sampling_ds_str)
{
  // Initialize pointers
    m_SendBuf = 0;
    m_RecvBuf = 0;
    m_PointOffsets = 0;
    m_PointCounts = 0;
    m_ComputerName = 0;

    // Verify that the MPI interface has been initialized
    // TODO - Performing MPI_Init/MPI_Finalize in the ctor/dtor requires access to argc/argv.
#ifdef MPI_CPP_BINDING
    if (!MPI::Is_initialized ()) throw dbskr_cluster_exception (dbskr_cluster_exception::MPIError, "MPI must be initialized in main", 0, 0);
#else
    {
        int inited = 0;
        MPI_Initialized (&inited);
        if (!inited) throw dbskr_cluster_exception (dbskr_cluster_exception::MPIError, "MPI must be initialized in main", 0, 0);
    }
#endif  // MPI_CPP_BINDING

    // Fetch the total number of processors and our rank - a value from 0 to m_TotalProcessors - 1
    m_ComputerName = new char [MPI_MAX_PROCESSOR_NAME];
    int nameSize;
#ifdef MPI_CPP_BINDING
    m_TotalProcessors = MPI::COMM_WORLD.Get_size ();
    m_MyRank = MPI::COMM_WORLD.Get_rank ();
    MPI::Get_processor_name (m_ComputerName, nameSize);
#else
    MPI_Comm_size (MPI_COMM_WORLD, &m_TotalProcessors);
    MPI_Comm_rank (MPI_COMM_WORLD, &m_MyRank);
    MPI_Get_processor_name (m_ComputerName, &nameSize);
#endif  // MPI_CPP_BINDING
    trace (IfInfo (), "m_TotalProcessors: %d, m_MyRank: %d, m_ComputerName: %s", m_TotalProcessors, m_MyRank, m_ComputerName);
  
  // each processor prepares db files
  vcl_vector<vcl_string> cats, cat_par_strs;
  cats.push_back("horse");  cat_par_strs.push_back("_6_15_3_2_0.3_0.8");
  cats.push_back("dog");    cat_par_strs.push_back("_6_15_3_2_0.3_0.8");
  cats.push_back("cow");    cat_par_strs.push_back("_6_15_3_2_0.3_0.8");
  cats.push_back("car");    cat_par_strs.push_back("_6_15_3_2_0.3_0.8");

  cats.push_back("pear");   cat_par_strs.push_back("_2_5_1_1_0.3_0.8");
  cats.push_back("cup");    cat_par_strs.push_back("_2_5_1_1_0.3_0.8");
  cats.push_back("tomato"); cat_par_strs.push_back("_2_5_1_1_0.3_0.8");
  cats.push_back("apple");  cat_par_strs.push_back("_2_5_1_1_0.3_0.8");
  
  sorting_method_str_ = "_color_";

  for (unsigned i = 0; i < cats.size(); i++) {
    for (unsigned j = 1; j < 11; j++) {
      vcl_ostringstream oss;
      oss << j;

      vcl_string ins_name = cats[i]+oss.str()+"-"+view_str;
      vcl_string out_dir = match_dir+cats[i]+"/";
      vcl_string ins_dir = out_dir+ins_name+"/";
      vul_file::make_directory_path(ins_dir);
      
      database_ins_names_.push_back(ins_name);
      database_cat_names_.push_back(cats[i]);
      cat_par_ins_strs_.push_back(cat_par_strs[i]);
      trace (IfInfo(), "ins_name: %s\n", ins_name.c_str());
    }
  }

  db_size_ = database_ins_names_.size();

  trace (IfInfo(), "db_size_: %d\n", db_size_);
  num_pairs_ = (db_size_*(db_size_-1))/2;  
}

// Destructor
dbskr_cluster_matching::~dbskr_cluster_matching()
{
    // Dispose of allocated resources - that is, we did the 'new'.
    database_ins_names_.clear(); 
    database_cat_names_.clear(); 
    cat_par_ins_strs_.clear();
    //if (m_delete_sample_roster) delete m_sample_roster;
    delete m_SendBuf;
    delete m_RecvBuf;
    delete m_PointOffsets;
    delete m_PointCounts;
    delete m_ComputerName;
}

// Execute - execute the matching algorithm as currently configured.
void dbskr_cluster_matching::execute_shock (bool match_with_circular_completions)
{
    trace (LeadProcessor () && IfInfo (), "number of pairs: %d", num_pairs_);
    
    // Allocate the send buffer for this processor large enough to hold its responses.
    unsigned myStartPoint = GetStartPair (num_pairs_, m_MyRank);
    unsigned myNumberPoints = GetNumberPairs (num_pairs_, m_MyRank);
    trace (LeadProcessor () && IfInfo (), "PointsPerProcessor: %d, PointsLeftOver %d", PairsPerProcessor (num_pairs_), PairsLeftOver (num_pairs_));
    trace (IfVerbose (), "myStartPoint %d, myNumberPoints %d", myStartPoint, myNumberPoints);
    delete m_SendBuf;
    m_SendBuf = new double [myNumberPoints * NUM_MATRICES];  // send a dummy double

    trace( IfInfo(), "loaded contours, preparing observations..\n");
    
    double scurve_sampling_ds = atof(sampling_ds_str_.c_str());
    float shock_pruning_threshold = 0.8f;
    bool elastic_splice_cost = true;

    unsigned point = 0;
    unsigned myEndPoint = myStartPoint + myNumberPoints;
    dbskr_shock_patch_storage_sptr st1, st2;
    for (unsigned i = 0; i < db_size_; i++) {
      st1 = 0;  // clears scurves and shock graphs
      vcl_string ins_name = database_ins_names_[i];
      vcl_string ins_dir = match_dir_+database_cat_names_[i]+"/"+ins_name+"/";

    for (unsigned j = i+1; j < db_size_; j++) 
    { 
      st2 = 0;  // clears scurves and shock graphs
      vcl_string ins_name2 = database_ins_names_[point];

      vcl_string output_name = ins_dir+ins_name+"-"+ins_name2+"-sc-"+sampling_ds_str_+"-normal-edit.bin";
      vcl_string output_name2 = match_dir_+database_cat_names_[j]+"/"+ins_name2+"/"+ins_name2+"-"+ins_name+"-sc-"+sampling_ds_str_+"-normal-edit.bin";

      if (point < myStartPoint || point >= myEndPoint) {
        point++; continue;
      }

      if (vul_file::exists(output_name) || vul_file::exists(output_name2)) 
        continue;

      if (!st1) {
        //: load str1
        vcl_string st_file1 = patch_dir_+database_cat_names_[i]+"/"+ins_name+"-patches"+sorting_method_str_+cat_par_ins_strs_[i]+"/"+ins_name+"_patch_storage.bin";
        st1 = load_str_from_file(st_file1);
      }

      if (!st2) {
        //: load str2  
        vcl_string st_file2 = patch_dir_+database_cat_names_[j]+"/"+ins_name2+"-patches"+sorting_method_str_+cat_par_ins_strs_[j]+"/"+ins_name2+"_patch_storage.bin";        
        st2 = load_str_from_file(st_file2);
      }
      
      trace( IfInfo(), "loaded storages, matching..\n");
      vul_timer t;
      t.mark();
/*
      // do something for each point
      dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
      patch_cor_map_type& map = match->get_map();

      vcl_vector<dbskr_shock_patch_sptr>& pv1 = st1->get_patches();
      for (unsigned iii = 0; iii < pv1.size(); iii++) {
        pv1[iii]->shock_pruning_threshold_ = shock_pruning_threshold;
        pv1[iii]->scurve_sample_ds_ = scurve_sample_ds;
        pv1[iii]->elastic_splice_cost_ = elastic_splice_cost;
      }
    
      match->shock_pruning_threshold_ = shock_pruning_threshold;
      match->scurve_sample_ds_ = scurve_sample_ds;
      match->elastic_splice_cost_ = elastic_splice_cost;

      for (unsigned iii = 0; iii < pv2.size(); iii++) {
        pv2[iii]->shock_pruning_threshold_ = shock_pruning_threshold;
        pv2[iii]->scurve_sample_ds_ = scurve_sample_ds;
        pv2[iii]->elastic_splice_cost_ = elastic_splice_cost;
      }

      vcl_vector<dbskr_shock_patch_sptr>& pv2 = st2->get_patches();
      
      
      

      for (unsigned iii = 0; iii < pv1.size(); iii++) {  
        vcl_cout << iii << " out of " << pv1.size() << ": ";
        find_patch_correspondences(pv1[iii], pv2, map, match_with_circular_completions);
      }

      vcl_cout << "\t\t\twriting bin file total time: " << ((t.real()/1000.0f)/60.0f) << " mins.\n";
      vsl_b_ofstream bfs(output_name.c_str());
      match->b_write(bfs);
      bfs.close();

      match->clear();
*/
      trace (IfVerbose (), "i: %d, j: %d, point: %d, matching time: %f mins\n", i, j, point, ((t.real()/1000.0f)/60.0f));
      double val = point;  // to see if they're in order in the output
        
      // Marshal this point's filter response into the send buffer.
      m_SendBuf[(point - myStartPoint)] = val;  // a fixed value for now
      point++;

    }

    }

    trace (LeadProcessor () && IfInfo (), "Execute loop end");
    
    // If we're the lead processor...
    if (LeadProcessor ())
    {
        // Allocate the receive buffer to hold the merged responses and the gather map.
        delete m_RecvBuf;
        m_RecvBuf = new double [num_pairs_ * NUM_MATRICES];
        m_PointOffsets = new int [m_TotalProcessors];
        m_PointCounts = new int [m_TotalProcessors];
        
        // Build the gather map - the offset/count for each processor
        for (int rank = 0; rank < m_TotalProcessors; rank++)
        {
            // Compute the offset/count for this processor.
            m_PointOffsets[rank] = GetStartPair (num_pairs_, rank) * NUM_MATRICES;
            m_PointCounts[rank] = GetNumberPairs (num_pairs_, rank) * NUM_MATRICES;
            trace (IfVerbose (), "Process: %d, offset: %d, count: %d", rank, m_PointOffsets[rank], m_PointCounts[rank]);
        }
    }
    
    // Gather results - This method is such that LeadProcessor receives and others send.
    trace (LeadProcessor () && IfInfo (), "Gather start");
#ifdef MPI_CPP_BINDING
    MPI::COMM_WORLD.Gatherv (m_SendBuf, myNumberPoints * NUM_MATRICES , GetMPIDataType ((double)0), 
                             m_RecvBuf, m_PointCounts, m_PointOffsets, 
                             GetMPIDataType ((double)0), 0);
#else
    MPI_Gatherv (m_SendBuf, myNumberPoints * NUM_MATRICES, GetMPIDataType ((double)0), 
                 m_RecvBuf, m_PointCounts, m_PointOffsets, 
                 GetMPIDataType ((double)0), 0, MPI_COMM_WORLD);
#endif  // MPI_CPP_BINDING
    trace (LeadProcessor () && IfInfo (), "Gather end");

    // Lead processor marshals back to response field
    if (LeadProcessor ())
    {
      unsigned point = 0;
      for (unsigned int i = 0; i<db_size_; i++)
        for (unsigned int j = i+1; j<db_size_; j++) 
        {
          double val = m_RecvBuf[(point * NUM_MATRICES)];
          if (val != point)
            vcl_cout << "ERROR: in message passing for point: " << point << " mes received: " << val << " expecting: " << point << vcl_endl;
          point++;
        }
    }
 }

// trace - Add trace message to stdout.
// TODO - move this logic to a stream to prefix all cout messages.
void dbskr_cluster_matching:: trace (bool cond, char* fmt, ...)
{
    // Only if enabled
    if (cond)
    {
        // Prefix message with date/time/rank
        char tbuf[BUFSIZ];
        memset (tbuf, 0, sizeof (tbuf));
        time_t now;
        time (&now);
        struct tm *ptm = localtime (&now);
        strftime (tbuf, sizeof (tbuf), "%m/%d.%H:%M:%S", ptm);
        char prefix[BUFSIZ];
        sprintf (prefix, "%s.%03d: ", tbuf, m_MyRank);
        
        // Format callers message.
        char msg[BUFSIZ];
        va_list vaMarker;
        va_start (vaMarker, fmt);
        vsprintf (msg, fmt, vaMarker);
        
        // Display
        vcl_cout << prefix << msg << vcl_endl;
    }
}

dbskr_shock_patch_storage_sptr dbskr_cluster_matching::load_str_from_file(vcl_string fname) {
  dbskr_shock_patch_storage_sptr st1 = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs(fname.c_str());
  st1->b_read(ifs);
  ifs.close();

  vcl_string str_name_end = "patch_storage.bin";

  //: read the esfs as well
  vcl_cout << st1->size() << " patches in str, reading shocks..\n";
  //: load esfs for each patch
  for (unsigned iii = 0; iii < st1->size(); iii++) {
    dbskr_shock_patch_sptr sp = st1->get_patch(iii);
    vcl_string patch_esf_name = fname.substr(0, fname.length()-str_name_end.size());
    vcl_ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_xshock_graph_fileio file_io;
    dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph(patch_esf_name);
    sp->set_shock_graph(sg);
  }
}

#endif // DBSKR_CLUSTER_DB_MATCHING_CXX_

