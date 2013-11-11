#ifndef BIOCLUSTER_VOLPART_FILTERING_PROC_H_
#define BIOCLUSTER_VOLPART_FILTERING_PROC_H_

// (c) 2005 Bio-Tree Systems, Inc.  All rights reserved.
// No part of this software may be used, reproduced or transmitted for
// any purpose, in any form or by any means, except as expressly
// permitted in writing by Bio-Tree Systems, Inc.

//////////////////////////////////////////////////////////////////////
//
// biocluster_volpart_filtering_proc.h
//  Definition for the bioproc_volpart_filtering_proc class.
//  Tony Anzelmo
//  10/12/2005
//
//  Modifications
//  Gamze Tunali
//  03/21/2006
//  modified based on the bioproc_filtering_proc changes
//
// This class, based on other bioproc filtering template classes supports
// volume partitioning for execution across a cluster for performance.
//
//////////////////////////////////////////////////////////////////////

// Includes
#include <biob/biob_grid_worldpt_roster.h>
#include <biob/biob_worldpt_roster.h>
#include <biob/biob_worldpt_roster_sptr.h>
#include <biob/biob_worldpt_field.h>
#include <xscan/xscan_scan.h>
#include <xmvg/xmvg_composite_filter_3d.h>
#include <imgr/imgr_image_view_3d_base_sptr.h>
#include <imgr/imgr_scan_images_resource.h>
#include <imgr/imgr_bounded_image_view_3d.h>
//#include <splr/splr_per_orbit_index_splat_collection.h>
#include <proc/bioproc_dummy_splat_resource.h>

// Definitions
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#define TRACE_ERROR     0
#define TRACE_INFO      1
#define TRACE_VERBOSE   2

// Exception class definition
class BioClusterException
{
public:
    // Error types
    typedef enum { Incomplete, MPIError, SystemError } BioClusterError;
private:
    // Bioproc error type
    BioClusterError m_error;
    // Exception message
    vcl_string m_message;
    // MPI specific error.
    int m_mpierror;
    // System specific error code.
    int m_syserror;
public:
    // Constructors
    // Default is thrown when a container tries to fetch the filter response on non-LeadProcessor
    BioClusterException() : m_error (Incomplete), m_message ("Response field is only complete on processor 0"), m_mpierror (0), m_syserror (0) {}
    // Customizable exception instance
    BioClusterException(BioClusterError err, vcl_string msg, int mpierr, int syserr) : m_error (err), m_message (msg), m_mpierror (mpierr), m_syserror (syserr) {}

    // Property accessors
    const char* GetMessage() { return m_message.data (); }
    int GetError() { return m_error; }
    int GetMPIError() { return m_mpierror; }
    int GetSysError() { return m_syserror; }
};

// Class definition
template< class T, class F>
class  biocluster_volpart_filtering_proc
{
public:
   biocluster_volpart_filtering_proc(xscan_scan const & scan,
                           vgl_box_3d<double> & box,
                           double resolution,
                           vgl_box_3d<double> &outer_box,
                           vcl_string bin_scan_file,
                           xmvg_composite_filter_3d<T, F> const& filter_3d,
                           biob_worldpt_roster_sptr sample_roster = 0,
                           int verbose = TRACE_ERROR);
    // Ctor/dtor
    biocluster_volpart_filtering_proc (
        imgr_scan_images_resource const & scan_image,       // Input image dataset
        vgl_box_3d<double> &box,                            // Bounding box definition
        double resolution,                                  // Resolution in the bounding box
        vgl_box_3d<double> &outer_box,
        vcl_string bin_scan_file,
        xmvg_composite_filter_3d<T, F> const& filter_3d,    // Filters to use
        biob_worldpt_roster_sptr sample_roster = 0,      // TODO - Is this used by any callers?
        int verbose = TRACE_ERROR                           // Errors only
    );
    ~biocluster_volpart_filtering_proc ();
  
    // Execute the algorithm and create the filter repsonse
    void execute (const vcl_vector<biob_worldpt_index> * which_samples = 0);

    // Fetch the filter response - throws if not on LeadProcessor
    /*vcl_vector<xmvg_filter_response<T> > const & responses () const
        {
            // Only on processor 0. Otherwise throw an exception.
            if (m_MyRank == 0) return m_response_field;
            else throw BioProcException ();
        }
    */
    // Fetch the filter response - throws if not on LeadProcessor
    biob_worldpt_field<xmvg_filter_response<T> > &worldpt_field() {
      // Only on processor 0. Otherwise throw an exception
      if (m_MyRank == 0) 
        return *m_field;
      else
        throw BioClusterException();
    }
    // Other "property" fetches.
    // TODO - Are these used anywhere?
    //const biob_worldpt_roster * sample_locations () const { return m_sample_roster; }
    //const biob_grid_worldpt_roster * voxelsptr () { return dynamic_cast <const biob_grid_worldpt_roster *>(m_sample_roster); }
    imgr_image_view_3d_base_sptr get_image_resource () { return m_view_3d;}
    bioproc_dummy_splat_resource<T,F>* splr () { return m_splr; }
    xmvg_composite_filter_3d<T, F> composite_filter() { return m_filter_3d; }

private:
   void check_sample_roster(const vgl_box_3d<double> & box, double resolution,
     biob_worldpt_roster_sptr sample_roster);
    // Image scan - the set of images making up the input datasaet
    xscan_scan m_scan;

    // Bounding box - region of interest in the reconstructed volume
    vgl_box_3d<double> m_box;

    //: the big region of interest
    vgl_box_3d<double> m_outer_box;
  
    //: stores the roster and the response values 
    biob_worldpt_field<xmvg_filter_response<T> > *m_field;

    // Composite filter to use - provided by container in constructor
    xmvg_composite_filter_3d<T, F> m_filter_3d;

    // Used to store all the 3D points
    // TODO - Is this used by any container?
    //const biob_worldpt_roster * m_sample_roster;
    //bool m_delete_sample_roster;    // False if specified in constructor

    // Verbosity level
    int m_verbose;

    // Splat resource
    bioproc_dummy_splat_resource<T, F>* m_splr;

    // Bounded 3D view
    imgr_image_view_3d_base_sptr m_view_3d;

    imgr_bounded_image_view_3d<unsigned short> v3d_;

    // Response field
    vcl_vector<xmvg_filter_response<T> > m_response_field;

    // new addition
        vcl_vector<vil_image_resource_sptr> images;
    // Configuration including number of processors, our rank, and computer name
    int m_TotalProcessors;
    int m_MyRank;
    char *m_ComputerName;

    // Send/receive buffers used to marshal filter responses to/from when gathering at LeadProcessor
    T *m_SendBuf;
    T *m_RecvBuf;

    // Definition of each processors partition for MPI gathering operation
    int *m_PointOffsets;
    int *m_PointCounts;

    // Whether this processor is the lead processor
    bool LeadProcessor () { return m_MyRank == 0; }

    // Number of points per processor and the number left over
    unsigned PointsPerProcessor (unsigned npts) { return (npts / m_TotalProcessors); }
    unsigned PointsLeftOver (unsigned npts) { return (npts % m_TotalProcessors); }

    // Return the start point and number of points for a given point count and processor rank
    unsigned GetStartPoint (unsigned npts, unsigned rank) { return (rank * PointsPerProcessor (npts) + min (rank, PointsLeftOver (npts))); }
    unsigned GetNumberPoints (unsigned npts, unsigned rank) { return (PointsPerProcessor (npts) + (rank < PointsLeftOver (npts) ? 1 : 0)); }

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

template <class T, class F>
void x_write(vcl_ostream& os, biocluster_volpart_filtering_proc<T,F> &proc);

#endif //BIOCLUSTER_VOLPART_FILTERING_PROC_H_
