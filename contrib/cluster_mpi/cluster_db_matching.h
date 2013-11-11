#ifndef CLUSTER_DB_MATCHING_H_
#define CLUSTER_DB_MATCHING_H_

//---------------------------------------------------------------------
// This is contrib/cluster_mpi/cluster_db_matching.h
//:
// \file
// \brief Given a database list find symettric distance metric for this database
//        should be called with different methods to match pairs
//
//
// \author
//  O. C. Ozcanli - May 25, 2006
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

// Includes
#include <vcl_string.h>
#include <vcl_vector.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <dbskr/dbskr_tree_sptr.h>
#include <dbinfo/dbinfo_observation_sptr.h>

#include <vil/vil_image_resource_sptr.h>

// Definitions
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#define TRACE_ERROR     0
#define TRACE_INFO      1
#define TRACE_VERBOSE   2

#define NUM_MATRICES    2

// Exception class definition
class cluster_exception
{
public:
    // Error types
    typedef enum { Incomplete, MPIError, SystemError } cluster_error;
private:
    // Bioproc error type
    cluster_error m_error;
    // Exception message
    vcl_string m_message;
    // MPI specific error.
    int m_mpierror;
    // System specific error code.
    int m_syserror;
public:
    // Constructors
    // Default is thrown when a container tries to fetch the filter response on non-LeadProcessor
    cluster_exception() : m_error (Incomplete), m_message ("Response field is only complete on processor 0"), m_mpierror (0), m_syserror (0) {}
    // Customizable exception instance
    cluster_exception(cluster_error err, vcl_string msg, int mpierr, int syserr) : m_error (err), m_message (msg), m_mpierror (mpierr), m_syserror (syserr) {}

    // Property accessors
    const char* get_message() { return m_message.data (); }
    int get_error() { return m_error; }
    int get_MPI_error() { return m_mpierror; }
    int get_syserror() { return m_syserror; }
};

// Class definition
class  cluster_db_matching
{
public:
   
    // Ctor/dtor
    cluster_db_matching (vcl_string db_file, vcl_string resolution, vcl_string db_string, vcl_string image_dir, 
        int verbose = TRACE_ERROR                           // Errors only
    );
    ~cluster_db_matching ();
  
    // Execute the algorithm and create the filter repsonse
    void execute_shock (vcl_string cons_dir, vcl_string esfs_dir, bool save_out_imgs = false, vcl_string output_dir = "", double sampling_ds = 1.0f, double pruning_threshold = -1.0f, bool elastic_splice = false, bool normalize_cost = false, bool save_shgm = false, vcl_string shgms_dir = "");
    

    // Fetch the distance matrix - throws if not on LeadProcessor
    vcl_vector< vcl_vector<double> > &get_dist_matrix() {
      // Only on processor 0. Otherwise throw an exception
      if (m_MyRank == 0) 
        return *dist_matrix_;
      else
        throw cluster_exception();
    }

    vcl_vector< vcl_vector<double> > &get_info_matrix() {
      // Only on processor 0. Otherwise throw an exception
      if (m_MyRank == 0) 
        return *info_matrix_;
      else
        throw cluster_exception();
    }


private:
  
    //: stores the distance_matrix 
    vcl_vector< vcl_vector<double> > *dist_matrix_;
    vcl_vector< vcl_vector<double> > *info_matrix_;

    unsigned int db_size_, num_pairs_;
    vcl_vector<vcl_string> database_; 
    vcl_vector<vil_image_resource_sptr> database_images_;
    vcl_vector<vsol_polygon_2d_sptr> database_polygons_;
    vcl_vector<dbinfo_observation_sptr> database_obs_;

    // Verbosity level
    int m_verbose;

    // new addition
    //vcl_vector<vil_image_resource_sptr> images;

  // Configuration including number of processors, our rank, and computer name
    int m_TotalProcessors;
    int m_MyRank;
    char *m_ComputerName;

    // Send/receive buffers used to marshal filter responses to/from when gathering at LeadProcessor
    double *m_SendBuf;
    double *m_RecvBuf;

    // Definition of each processors partition for MPI gathering operation
    int *m_PointOffsets;
    int *m_PointCounts;

    dbskr_tree_sptr read_esf_from_file(vcl_string fname, double sampling_ds, double pruning_threshold, bool elastic_splice);
    vsol_polygon_2d_sptr read_con_from_file(vcl_string fname);

    // Whether this processor is the lead processor
    bool LeadProcessor () { return m_MyRank == 0; }

    // Number of pairs per processor and the number left over
    unsigned PairsPerProcessor (unsigned npts) { return (npts / m_TotalProcessors); }
    unsigned PairsLeftOver (unsigned npts) { return (npts % m_TotalProcessors); }

    // Return the start point and number of points for a given point count and processor rank
    unsigned GetStartPair (unsigned npts, unsigned rank) { return (rank * PairsPerProcessor (npts) + min (rank, PairsLeftOver (npts))); }
    unsigned GetNumberPairs (unsigned npts, unsigned rank) { return (PairsPerProcessor (npts) + (rank < PairsLeftOver (npts) ? 1 : 0)); }

    // Trace message for debug
    bool IfError ()   { return m_verbose >= TRACE_ERROR; }
    bool IfInfo ()    { return m_verbose >= TRACE_INFO; }
    bool IfVerbose () { return m_verbose >= TRACE_VERBOSE; }
    void trace (bool cond, char *fmt, ...);

    // Return the MPI data type for specified argument. MPI methods often
    // require data type specifier and these methods provide the glue.
    // This collection restricts the template class, but can be easily extended
    // (See MPI include files for target platform).
#ifdef MPI_CPP_BINDING
    MPI::Datatype GetMPIDataType (double arg) { return (MPI::DOUBLE); }
    MPI::Datatype GetMPIDataType (float arg) { return (MPI::FLOAT); }
    MPI::Datatype GetMPIDataType (unsigned int arg) { return (MPI::UNSIGNED); }
    MPI::Datatype GetMPIDataType (unsigned long arg) { return (MPI::UNSIGNED_LONG); }
    MPI::Datatype GetMPIDataType (unsigned short arg) { return (MPI::UNSIGNED_SHORT); }
#else
    MPI_Datatype GetMPIDataType (double arg) { return (MPI_DOUBLE); }
    MPI_Datatype GetMPIDataType (float arg) { return (MPI_FLOAT); }
    MPI_Datatype GetMPIDataType (unsigned int arg) { return (MPI_UNSIGNED); }
    MPI_Datatype GetMPIDataType (unsigned long arg) { return (MPI_UNSIGNED_LONG); }
    MPI_Datatype GetMPIDataType (unsigned short arg) { return (MPI_UNSIGNED_SHORT); }
#endif  // MPI_CPP_BINDING
};

#endif //CLUSTER_DB_MATCHING_H_
