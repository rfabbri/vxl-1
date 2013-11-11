#ifndef CLUSTER_DB_MATCHING_CXX_
#define CLUSTER_DB_MATCHING_CXX_

// Includes
#include <mpi.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <vcl_iostream.h>
#include <vul/vul_timer.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_sm_cor_sptr.h>
#include <dbskr/dbskr_sm_cor.h>

#include <dbru/dbru_rcor_sptr.h>
#include <dbru/dbru_rcor.h>
#include <dbru/algo/dbru_object_matcher.h>

#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_matcher.h>

#include "cluster_db_matching.h"

cluster_db_matching::cluster_db_matching(vcl_string db_file, 
                                         vcl_string resolution, 
                                         vcl_string db_string, 
                                         vcl_string image_dir, 
                                         int verbose) 
   : m_verbose (verbose)
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
    if (!MPI::Is_initialized ()) throw cluster_exception (cluster_exception::MPIError, "MPI must be initialized in main", 0, 0);
#else
    {
        int inited = 0;
        MPI_Initialized (&inited);
        if (!inited) throw cluster_exception (cluster_exception::MPIError, "MPI must be initialized in main", 0, 0);
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
  
  // each processor reads db file
  db_size_ = 0;
  vcl_ifstream fpd((db_file).c_str());
  if (!fpd.is_open()) {
    vcl_cout << "Unable to open database file!\n";
  } else {
    char buffer[1000];
    while (!fpd.eof()) {
      vcl_string temp;
      fpd.getline(buffer, 1000);
      temp = buffer;
      if (temp.size() > 1) {
        //vcl_cout << "temp: " << temp << " addition: " << db_string << vcl_endl;
        database_.push_back(temp+"_"+db_string);
      }
    }
    fpd.close();
    db_size_ = database_.size(); 
  }

  trace (IfInfo(), "db_size_: %d\n", db_size_);
#if 0
  vcl_cout << "printing database list: \n";
  for (unsigned int i = 0; i<db_size_; i++) {
    vcl_cout << database_[i] << "\n";
  }
#endif
  
  //: each processor loads the images
  for (unsigned i = 0; i<db_size_; i++) {
    vcl_string image_file = image_dir + database_[i] + ".png";
    vil_image_resource_sptr image_r = vil_load_image_resource( image_file.c_str() );
    if (!image_r) 
      vcl_cout << "image: " << image_file << " could not be loaded!\n";
    else 
      database_images_.push_back(image_r);
  }
  trace (IfInfo(), "loaded images\n");

  num_pairs_ = (db_size_*(db_size_-1))/2;  

  vcl_vector<double> tmp(db_size_, 10000.0f);
  dist_matrix_ = new vcl_vector< vcl_vector<double> > (db_size_, tmp);
  info_matrix_ = new vcl_vector< vcl_vector<double> > (db_size_, tmp);
}

// Destructor
cluster_db_matching::~cluster_db_matching()
{
    // Dispose of allocated resources - that is, we did the 'new'.
    delete dist_matrix_;
    //if (m_delete_sample_roster) delete m_sample_roster;
    delete m_SendBuf;
    delete m_RecvBuf;
    delete m_PointOffsets;
    delete m_PointCounts;
    delete m_ComputerName;
}

// Execute - execute the matching algorithm as currently configured.
void cluster_db_matching::execute_shock(vcl_string cons_dir, 
                                        vcl_string esfs_dir, 
                                        bool save_out_imgs, vcl_string output_dir, 
                                        double sampling_ds, double pruning_threshold,
                                        bool elastic_splice, bool normalize_cost, 
                                        bool save_shgm, vcl_string shgms_dir)
{
    trace (LeadProcessor () && IfInfo (), "number of pairs: %d", num_pairs_);
    
    // Allocate the send buffer for this processor large enough to hold its responses.
    unsigned myStartPoint = GetStartPair (num_pairs_, m_MyRank);
    unsigned myNumberPoints = GetNumberPairs (num_pairs_, m_MyRank);
    trace (LeadProcessor () && IfInfo (), "PointsPerProcessor: %d, PointsLeftOver %d", PairsPerProcessor (num_pairs_), PairsLeftOver (num_pairs_));
    trace (IfVerbose (), "myStartPoint %d, myNumberPoints %d", myStartPoint, myNumberPoints);
    delete m_SendBuf;
    m_SendBuf = new double [myNumberPoints * NUM_MATRICES];  // since we're creating 2 values for each pair

    if (database_images_.size() != db_size_) {
      vcl_cout << "images not loaded! Exiting!\n";
      return;
    }

    //: load cons
    for (unsigned i = 0; i<db_size_; i++) {
      vcl_string con_file = cons_dir + database_[i] + ".con";
      vsol_polygon_2d_sptr poly = read_con_from_file(con_file.c_str());
      if (!poly) 
        trace( IfInfo(), "contour: %s could not be loaded!\n", con_file.c_str());
      else 
        database_polygons_.push_back(poly);
    }

    if (database_polygons_.size() != db_size_) {
      trace( IfInfo(), "contours not loaded! Exiting!\n");
      return;
    }

    trace( IfInfo(), "loaded contours, preparing observations..\n");

    //: load observations
    for (unsigned i = 0; i<db_size_; i++) {
      dbinfo_observation_sptr obs = new dbinfo_observation(0, database_images_[i], database_polygons_[i], true, true, false);
      if (!obs) 
        trace( IfInfo(), "observation: %d could not be created\n", i);
      else
        database_obs_.push_back(obs);
    }

    if (database_obs_.size() != db_size_) {
      vcl_cout << "observations not created! Exiting!\n";
      return;
    }
    
    //: load esfs and create trees
    vcl_vector<dbskr_tree_sptr> database_trees;
    for (unsigned i = 0; i<db_size_; i++) {
      vcl_string esf_file = esfs_dir + database_[i] + ".esf";
      trace (LeadProcessor () && IfInfo (), "Loading: %s\n", esf_file.c_str());
      dbskr_tree_sptr tree = read_esf_from_file(esf_file.c_str(), sampling_ds, pruning_threshold, elastic_splice);
      if (!tree)
        trace( IfInfo(), "tree: %s could not be loaded\n", esf_file.c_str());
      else
        database_trees.push_back(tree);
    }

    if (database_trees.size() != db_size_) {
      trace( IfInfo(), "trees not loaded! Exiting!\n");
      return;
    }

    // Process the pairs 
    trace (LeadProcessor () && IfInfo (), "Loaded all the data!! Execute loop start");

    unsigned point = 0;
    unsigned myEndPoint = myStartPoint + myNumberPoints;
    for (unsigned i = 0; i < db_size_; i++)
    for (unsigned j = i+1; j < db_size_; j++) 
    { 
      if (point < myStartPoint || point >= myEndPoint) {
        point++; continue;
      }
      
      // do something for each point
      vul_timer t;
      dbskr_sm_cor_sptr sm_cor1, sm_cor2; double cost1, cost2, norm;
      vcl_string shgm_file_name1 = shgms_dir+database_[i]+"_"+database_[j]+".shgm";
      vcl_string shgm_file_name2 = shgms_dir+database_[j]+"_"+database_[i]+".shgm";
      vcl_ifstream shgmf1(shgm_file_name1.c_str());
      vcl_ifstream shgmf2(shgm_file_name2.c_str());

      if (normalize_cost) {
        norm = database_trees[i]->total_splice_cost()+database_trees[j]->total_splice_cost();
        //vcl_cout << " norm: " << norm << " ";
      } else
        norm = 1.0f;
      
      if (shgmf1.is_open()) {
        //vcl_cout << "reading " << shgm_file_name1 << vcl_endl;
        trace (IfInfo (), "read shgmf1\n");
        shgmf1.close();
        sm_cor1 = new dbskr_sm_cor(database_trees[i], database_trees[j]);
        cost1 = sm_cor1->read_and_construct_from_shgm(shgm_file_name1);
      } else {
        sm_cor1 = dbru_object_matcher::compute_shock_alignment(database_trees[i],database_trees[j],cost1,false);
        //: shgms contain non-normalized costs as a convention
        if (save_shgm) 
          sm_cor1->write_shgm(cost1, shgm_file_name1);
      }
      
      if (shgmf2.is_open()) {
        //vcl_cout << "reading " << shgm_file_name2 << vcl_endl;
        trace (IfInfo (), "read shgmf2\n");
        shgmf2.close();
        sm_cor2 = new dbskr_sm_cor(database_trees[j], database_trees[i]);
        cost2 = sm_cor2->read_and_construct_from_shgm(shgm_file_name2);
      } else {
        sm_cor2 = dbru_object_matcher::compute_shock_alignment(database_trees[j],database_trees[i],cost2,false);      
        //: shgms contain non-normalized costs as a convention
        if (save_shgm) 
          sm_cor2->write_shgm(cost2, shgm_file_name2);
      }

      cost1 /= norm;
      cost2 /= norm;
      if (cost1 < cost2) {
        (*dist_matrix_)[i][j] = cost1;
        (*dist_matrix_)[j][i] = (*dist_matrix_)[i][j];
      } else {
        (*dist_matrix_)[i][j] = cost2;
        (*dist_matrix_)[j][i] = (*dist_matrix_)[i][j]; 
      }
              
      dbru_rcor_sptr rcor1 = dbru_object_matcher::generate_rcor_shock_matching(database_obs_[i], database_obs_[j], sm_cor1, false);
      dbru_rcor_sptr rcor2 = dbru_object_matcher::generate_rcor_shock_matching(database_obs_[j], database_obs_[i], sm_cor2, false);
      float info1 = dbinfo_observation_matcher::minfo(database_obs_[i], database_obs_[j], rcor1->get_correspondences(), false);
      float info2 = dbinfo_observation_matcher::minfo(database_obs_[j], database_obs_[i], rcor2->get_correspondences(), false);

      if (info1 > info2) {
        (*info_matrix_)[i][j] = info1;
        (*info_matrix_)[j][i] = info1;
        if (save_out_imgs) {
          vil_image_resource_sptr out_image = rcor1->get_appearance2_on_pixels1();
          vil_save_image_resource(out_image, (output_dir+database_[i]+"_"+database_[j]+"-shock.png").c_str()); 
        }
      } else {
        (*info_matrix_)[i][j] = info2;
        (*info_matrix_)[j][i] = info2;
        if (save_out_imgs) {
          vil_image_resource_sptr out_image = rcor2->get_appearance2_on_pixels1();
          vil_save_image_resource(out_image, (output_dir+database_[j]+"_"+database_[i]+"-shock.png").c_str()); 
        }
      }
      trace (IfVerbose (), "i: %d, j: %d, point: %d, cost: %lf, info: %lf, matching time: %f secs\n", i, j, point, (*dist_matrix_)[i][j], (*info_matrix_)[i][j], t.real()/1000.0f);

      double val = point;  // to see if they're in order in the output
      
      // Marshal this point's filter response into the send buffer.
      m_SendBuf[(point - myStartPoint) * NUM_MATRICES + 0] = (*dist_matrix_)[i][j];  // a fixed value for now
      m_SendBuf[(point - myStartPoint) * NUM_MATRICES + 1] = (*info_matrix_)[i][j];  // a fixed value for now

      point++;
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
          (*dist_matrix_)[i][j] = m_RecvBuf[(point * NUM_MATRICES) + 0];
          (*dist_matrix_)[j][i] = (*dist_matrix_)[i][j];
          (*info_matrix_)[i][j] = m_RecvBuf[(point * NUM_MATRICES) + 1];
          (*info_matrix_)[j][i] = (*info_matrix_)[i][j];
          point++;
        }
    }
 }

// trace - Add trace message to stdout.
// TODO - move this logic to a stream to prefix all cout messages.
void cluster_db_matching:: trace (bool cond, char* fmt, ...)
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

vsol_polygon_2d_sptr cluster_db_matching::read_con_from_file(vcl_string fname) {
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

dbskr_tree_sptr cluster_db_matching::read_esf_from_file(vcl_string fname, double sampling_ds, double pruning_threshold, bool elastic_splice) {
  dbsk2d_xshock_graph_fileio loader;
  dbskr_tree_sptr tree = new dbskr_tree(sampling_ds);
  dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(fname);
  if (pruning_threshold <= 0)
    tree->acquire(sg, elastic_splice);    
  else
    tree->acquire_and_prune(sg, pruning_threshold, elastic_splice);

  return tree;
}

#endif // CLUSTER_DB_MATCHING_CXX_

