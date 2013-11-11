#ifndef BIOCLUSTER_VOLPART_FILTERING_PROC_TXX_
#define BIOCLUSTER_VOLPART_FILTERING_PROC_TXX_

// (c) 2005 Bio-Tree Systems, Inc.  All rights reserved.
// No part of this software may be used, reproduced or transmitted for
// any purpose, in any form or by any means, except as expressly
// permitted in writing by Bio-Tree Systems, Inc.

//////////////////////////////////////////////////////////////////////
//
// biocluster_volpart_filtering_proc.txx
//  Implementation for the biocluster_volpart_filtering_proc class.
//  Tony Anzelmo
//  10/12/2005
//  
//  Modifications
//  04/01/2006 Gamze Tunali
//    Modified it based on the changes of introducing the outer box, inner 
//    box ideas, and also introcing the biob_worldpt_field structure to hold 
//    the roster and the response values together
//
// This class, based on other bioproc filtering template classes supports
// volume partitioning for execution across a cluster for performance.
//
//////////////////////////////////////////////////////////////////////

// Includes
#include <mpi.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "biocluster_volpart_filtering_proc.h"
#include <imgr/imgr_scan_resource_sptr.h>
#include <imgr/imgr_scan_resource_io.h>
#include <xmvg/algo/xmvg_dot_product_2d.h>
#include <xmvg/algo/xmvg_ramp_compensation.h>
#include <vsl/vsl_basic_xml_element.h>

template<class T, class F> 
void biocluster_volpart_filtering_proc<T,F>::check_sample_roster(
     const vgl_box_3d<double> & box, double resolution,
     biob_worldpt_roster_sptr sample_roster){
  if (!sample_roster){
    biob_worldpt_roster_sptr roster = new biob_grid_worldpt_roster(box, resolution);
    m_field = new biob_worldpt_field<xmvg_filter_response<T> > (roster);
   }
  else {
    m_field = new biob_worldpt_field<xmvg_filter_response<T> > (sample_roster);
  }
  m_response_field.resize (m_field->roster()->num_points ());
}

template<class T, class F> 
biocluster_volpart_filtering_proc<T,F>::biocluster_volpart_filtering_proc(
    xscan_scan const& scan, 
    vgl_box_3d<double> &box,   double resolution,
    vgl_box_3d<double> &outer_box,
    vcl_string bin_scan_file,
    xmvg_composite_filter_3d<T, F> const & filter_3d,
    biob_worldpt_roster_sptr sample_roster,
    int verbose) 
   : m_scan (scan), m_box (box), m_outer_box(outer_box), m_filter_3d (filter_3d), m_verbose (verbose)
{
  // Initialize pointers
    m_splr = 0;
    m_SendBuf = 0;
    m_RecvBuf = 0;
    m_PointOffsets = 0;
    m_PointCounts = 0;
    m_ComputerName = 0;

    // Verify that the MPI interface has been initialized
    // TODO - Performing MPI_Init/MPI_Finalize in the ctor/dtor requires access to argc/argv.
#ifdef MPI_CPP_BINDING
    if (!MPI::Is_initialized ()) throw BioClusterException (BioClusterException::MPIError, "MPI must be initialized in main", 0, 0);
#else
    {
        int inited = 0;
        MPI_Initialized (&inited);
        if (!inited) throw BioClusterException (BioClusterException::MPIError, "MPI must be initialized in main", 0, 0);
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
  check_sample_roster(box, resolution, sample_roster);
  
  unsigned num_pts = m_field->roster()->num_points();

  // set up splat resource
  //set_up_splr();
  // Allocate a splat resource.
  m_splr = new bioproc_dummy_splat_resource<T, F> (m_scan, m_filter_3d);
    
  // if there is a binary file name for imgr_bounded_image_view, read it directly
  // from the binary file
  if (bin_scan_file.size() > 0) {
    vsl_b_ifstream is(bin_scan_file.data());
    v3d_.b_read(is);
  } else {
    // set up image resource
    
    imgr_scan_resource_sptr resc = imgr_scan_resource_io::read_resource(m_scan);

    if(!resc)
      return;

    m_view_3d = resc->get_bounded_view(m_outer_box);
    v3d_ = m_view_3d;
  }
}

// Constructor
template<class T, class F>
biocluster_volpart_filtering_proc<T, F>:: biocluster_volpart_filtering_proc
(
    imgr_scan_images_resource const & scan_images,
    vgl_box_3d<double> &box,
    double resolution,
    vgl_box_3d<double> &outer_box,
    vcl_string bin_scan_file,
    xmvg_composite_filter_3d<T, F> const& filter_3d,
    biob_worldpt_roster_sptr sample_roster,
    int verbose
) : m_scan (scan_images.get_scan ()), m_box (box), m_outer_box(outer_box), m_filter_3d (filter_3d), m_verbose (verbose)
{
    // Initialize pointers
    m_splr = 0;
    m_SendBuf = 0;
    m_RecvBuf = 0;
    m_PointOffsets = 0;
    m_PointCounts = 0;
    m_ComputerName = 0;

    // Verify that the MPI interface has been initialized
    // TODO - Performing MPI_Init/MPI_Finalize in the ctor/dtor requires access to argc/argv.
#ifdef MPI_CPP_BINDING
    if (!MPI::Is_initialized ()) throw BioClusterException (BioClusterException::MPIError, "MPI must be initialized in main", 0, 0);
#else
    {
        int inited = 0;
        MPI_Initialized (&inited);
        if (!inited) throw BioClusterException (BioClusterException::MPIError, "MPI must be initialized in main", 0, 0);
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

    // Allocate sample roster if not specified
    check_sample_roster(box, resolution, sample_roster);
    
    // Allocate a splat resource.
    m_splr = new bioproc_dummy_splat_resource<T, F> (m_scan, m_filter_3d); 

    // Get bounded views - this caches box projections for all views in the input dataset.
    trace (LeadProcessor () && IfInfo (), "Get bounded view start");
    
    if (bin_scan_file.size() > 0) {
      vsl_b_ifstream is(bin_scan_file.data());
      v3d_.b_read(is);
    } else {
      // get bounded view
      m_view_3d = scan_images.get_bounded_view(m_outer_box);
      v3d_ = m_view_3d;
    }
    trace (LeadProcessor () && IfInfo (), "Get bounded view end");
}

// Destructor
template<class T, class F>
biocluster_volpart_filtering_proc<T,F>:: ~biocluster_volpart_filtering_proc ()
{
    // Dispose of allocated resources - that is, we did the 'new'.
    delete m_splr;
    //if (m_delete_sample_roster) delete m_sample_roster;
    delete m_SendBuf;
    delete m_RecvBuf;
    delete m_PointOffsets;
    delete m_PointCounts;
    delete m_ComputerName;
}

// Execute - execute filtering algorithm as currently configured.
template<class T, class F>
void biocluster_volpart_filtering_proc<T, F> :: execute(const vcl_vector<biob_worldpt_index> * which_samples)
{
    // Get the problem size - number of points/view/filters
    unsigned npts = which_samples ? which_samples->size () : m_field->roster()->num_points();
    unsigned nviews = m_scan.scan_size ();
    unsigned num_atomic_filters = m_filter_3d.size();
    trace (LeadProcessor () && IfInfo (), "npts: %d, nviews: %d, num_atomic_filters: %d", npts, nviews, num_atomic_filters);
    
    // Allocate the send buffer for this processor large enough to hold its responses.
    unsigned myStartPoint = GetStartPoint (npts, m_MyRank);
    unsigned myNumberPoints = GetNumberPoints (npts, m_MyRank);
    trace (LeadProcessor () && IfInfo (), "PointsPerProcessor: %d, PointsLeftOver %d", PointsPerProcessor (npts), PointsLeftOver (npts));
    trace (IfVerbose (), "myStartPoint %d, myNumberPoints %d", myStartPoint, myNumberPoints);
    delete m_SendBuf;
    m_SendBuf = new T [myNumberPoints * num_atomic_filters];        

    // For my points in the volume...
    trace (LeadProcessor () && IfInfo (), "Execute loop start");
    for (unsigned point = myStartPoint; point < (myStartPoint + myNumberPoints); point++)
    {
        // Declare response for this point
        xmvg_filter_response<T> response (num_atomic_filters, T(0));
        
        // For every view in the input dataset...
        for (unsigned view = 0; view < nviews; view++)
        {
            // Get view after cast to desired type
            dbil_bounded_image_view<unsigned short> dv = v3d_.view_2d (view);
            
            // Get the 2d filter response
            biob_worldpt_index pti = which_samples ? ((*which_samples)[point]) : biob_worldpt_index (point);
            xmvg_composite_filter_2d<T> uncomp_filter = m_splr->splat (view, m_field->roster()->point (pti));
            xmvg_composite_filter_2d<T> filter;
            xmvg_ramp_compensation<T>(uncomp_filter, filter);
            filter.eliminate_residue_effect ();
            
            // Apply the 2d filter to the image and accummulate this point's filter response
            response += xmvg_dot_product_2d (filter, dv);
        }

        // Marshal this point's filter response into the send buffer.
        for (unsigned filter = 0; filter < num_atomic_filters; filter++) m_SendBuf[((point - myStartPoint) * num_atomic_filters) + filter] = (T)response[filter];
        trace (IfVerbose (), "Point %d completed", point);
    }
    trace (LeadProcessor () && IfInfo (), "Execute loop end");
    
    // If we're the lead processor...
    if (LeadProcessor ())
    {
        // Allocate the receive buffer to hold the merged responses and the gather map.
        delete m_RecvBuf;
        m_RecvBuf = new T [npts * num_atomic_filters];
        m_PointOffsets = new int [m_TotalProcessors];
        m_PointCounts = new int [m_TotalProcessors];
        
        // Build the gather map - the offset/count for each processor
        for (int rank = 0; rank < m_TotalProcessors; rank++)
        {
            // Compute the offset/count for this processor.
            m_PointOffsets[rank] = GetStartPoint (npts, rank) * num_atomic_filters;
            m_PointCounts[rank] = GetNumberPoints (npts, rank) * num_atomic_filters;
            trace (IfVerbose (), "Process: %d, offset: %d, count: %d", rank, m_PointOffsets[rank], m_PointCounts[rank]);
        }
    }
    
    // Gather results - This method is such that LeadProcessor receives and others send.
    trace (LeadProcessor () && IfInfo (), "Gather start");
#ifdef MPI_CPP_BINDING
    MPI::COMM_WORLD.Gatherv (m_SendBuf, myNumberPoints * num_atomic_filters, GetMPIDataType ((T)0), m_RecvBuf, m_PointCounts, m_PointOffsets, GetMPIDataType ((T)0), 0);
#else
    MPI_Gatherv (m_SendBuf, myNumberPoints * num_atomic_filters, GetMPIDataType ((T)0), m_RecvBuf, m_PointCounts, m_PointOffsets, GetMPIDataType ((T)0), 0, MPI_COMM_WORLD);
#endif  // MPI_CPP_BINDING
    trace (LeadProcessor () && IfInfo (), "Gather end");

    // Lead processor marshals back to response field
    if (LeadProcessor ())
    {
        for (unsigned point = 0; point < npts; point++)
        {
            xmvg_filter_response<T> resp (num_atomic_filters);
            for (unsigned filter = 0; filter < num_atomic_filters; filter++) resp[filter] = (T)m_RecvBuf[(point * num_atomic_filters) + filter];
            m_response_field[point] = resp;
        }
        m_field->set_values(m_response_field);
    }
}

// trace - Add trace message to stdout.
// TODO - move this logic to a stream to prefix all cout messages.
template<class T, class F>
void biocluster_volpart_filtering_proc<T, F> :: trace (bool cond, char* fmt, ...)
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

template<class T, class F>
void x_write(vcl_ostream& os, biocluster_volpart_filtering_proc<T,F> &proc)
{
  vsl_basic_xml_element element("bioproc_filtering_proc");
  element.x_write_open(os);
  
  //: composite filter 3d
  xmvg_composite_filter_3d<T, F> f = proc.composite_filter();
  x_write(os, f);
  
  // write the roster and values
  x_write(os, proc.worldpt_field());
  element.x_write_close(os);
}

// Code for easy instantiation.
#undef BIOCLUSTER_VOLPART_FILTERING_PROC_INSTANTIATE
#define BIOCLUSTER_VOLPART_FILTERING_PROC_INSTANTIATE(T, F) \
template class biocluster_volpart_filtering_proc<T, F>;\
template void x_write(vcl_ostream&, biocluster_volpart_filtering_proc<T, F> &)

#endif // BIOCLUSTER_VOLPART_FILTERING_PROC_TXX_

