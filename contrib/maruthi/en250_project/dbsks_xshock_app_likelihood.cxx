// This is file seg/dbsks/dbsks_xshock_app_likelihood.cxx

//:
// \file


#include <en250_project/dbsks_xshock_app_likelihood.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_polygon.h>
#include <vnl/vnl_math.h>
#include <vcl_iostream.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_cstring.h>
#include <vnl/vnl_numeric_traits.h>

// ============================================================================
// dbsks_xshock_app_likelihood
// ============================================================================


dbsks_xshock_app_likelihood::
dbsks_xshock_app_likelihood(const vcl_string& queryImage)
    :dbsks_xshock_likelihood()
{

    // Start of by loading the shock model and ground truth image
    // Hard coded paths fix later
    bool status=false;

    // Load the model shock graph
    vcl_string filePath("/home/maruthi/Desktop/en250proj/cartilageModel.xml");   
    dbsksp_xshock_graph_sptr xg(0);
    x_read(filePath.c_str(), xg);

    // Set an arbitrary root node for the model
    dbsksp_xshock_node_sptr xv0 = *xg->vertices_begin();
    xg->compute_vertex_depths(2);

    // Load the training image
    vcl_string imagePath
        ("/home/maruthi/Desktop/en250proj/SP971_3T_noscrew_023.png");
    vil_image_view<vxl_byte> imageView=vil_load(imagePath.c_str());

    // Load the query image
    queryView_=vil_load(queryImage.c_str());

    // The next step is to create appearance model per fragment
    // Loop through all shock edges, form a vgl polygon, then scan
    // the polygon for (x,y) coordinates interior to the shape
    // Query the image with the (x,y) coordinates and that is the descriptor
    for(dbsksp_xshock_graph::edge_iterator itr = xg->edges_begin(); itr != 
            xg->edges_end();  ++itr)
    {
 
      
        vgl_polygon<double> polyFragment=convert_edge_to_poly(status,*itr);
      
        //Scan each poly line for pixels within interior
        vgl_polygon_scan_iterator<double> psi(polyFragment);
   
        for (psi.reset(); psi.next(); ) 
        {
            int y = psi.scany();
            for (int x = psi.startx(); x <= psi.endx(); ++x)
            {
                //Populate map entry with pixel values
                appearance_model_[(*itr)->id()].
                    push_back(imageView(x,y,0));
            }
          
        }

    }

}

// -----------------------------------------------------------------------------
//: Likelihood of a xshock fragment
double dbsks_xshock_app_likelihood::
loglike(unsigned edge_id, const dbsksp_xshock_fragment& xfrag)
{

    //For this edge_id find the appearanceModel
    vcl_vector<double> modelApp = appearance_model_[edge_id];

    // For this fragment conver to a poly and then
    // scan polygon to computer descriptor
    bool status = false;

    vgl_polygon<double> queryFragment=convert_frag_to_poly(status,xfrag);
    vcl_vector<double>  queryApp;
  
    if(status)
    {
        return -vnl_numeric_traits<double>::maxval;
    }

    //Scan each poly line for pixels within interior
    vgl_polygon_scan_iterator<double> psi(queryFragment);
   
    // Bool compute cost
    bool flag=true;

    for (psi.reset(); psi.next(); ) 
    {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x)
        {
         
            if ( x < queryView_.ni() &&  y < queryView_.nj())
            {
         
                //Populate apperance descriptor for query
                queryApp.push_back(queryView_(x,y,0));

            }
            else
            {

                flag=false;
                break;
            }
        }

        if (!flag)
        {


            break;
        }
      
    }

    double final_cost = -vnl_numeric_traits<double>::maxval;
    if ( flag)
    {
        final_cost=emd(modelApp,queryApp);
        
    }


    return final_cost;
}

// -----------------------------------------------------------------------------
//: Likelihood of an xshock fragment given the whole shape's boundary
double dbsks_xshock_app_likelihood::
f_whole_contour(unsigned edge_id, 
                const dbsksp_xshock_fragment& xfrag, 
                const vgl_polygon<double >& boundary)
{

    // wrapper around f cost
    double cost=loglike(edge_id,xfrag);

    return cost;

}


// -----------------------------------------------------------------------------
//: Likelihood of a xshock graph
double dbsks_xshock_app_likelihood::
loglike(const dbsksp_xshock_graph_sptr& xgraph, 
  const vcl_vector<unsigned >& ignored_edges,bool verbose)
{

    // Create a vector of apperance diagrams for each fragment
    vcl_vector<double> fragAppearance;

    // Bool compute cost
    bool flag=true;

    double sum_cost = 0;
    for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit !=
             xgraph->edges_end(); ++eit)
    {
      
        //For this edge_id find the appearanceModel
        dbsksp_xshock_edge_sptr xe = *eit;
        unsigned eid = xe->id();
        vcl_vector<double> modelApp = appearance_model_[eid];
      
        bool status=false;

        //Convert fragment to polygon
        vgl_polygon<double> polyFragment=convert_edge_to_poly(status,xe);

        if(status)
        {
            return -vnl_numeric_traits<double>::maxval;
        }

        //Scan each poly line for pixels within interior
        vgl_polygon_scan_iterator<double> psi(polyFragment);
   
        for (psi.reset(); psi.next(); ) 
        {
            int y = psi.scany();
            for (int x = psi.startx(); x <= psi.endx(); ++x)
            {

                if ( x < queryView_.ni() &&  y < queryView_.nj())
                {
                    //Populate apperance descriptor for query
                    fragAppearance.push_back(queryView_(x,y,0));
                }
               
            }
 
        }

        double fragment_cost=emd(modelApp,fragAppearance);
        sum_cost += fragment_cost;

        //Clear out vector before we use it again
        fragAppearance.clear();


    }

    return sum_cost;

}

// -----------------------------------------------------------------------------
//: Likelihood of a xshock graph using whole contour matching
double dbsks_xshock_app_likelihood::
f_whole_contour(const dbsksp_xshock_graph_sptr& xgraph, 
                const vcl_vector<unsigned >& ignored_edges, bool verbose)
{

    double cost = loglike(xgraph,ignored_edges,verbose);

    vcl_cout<<"cost of whole_contour with graph: "<<cost <<vcl_endl;
    return cost;

}

// -----------------------------------------------------------------------------
// Converts a shock edge to a polygon
vgl_polygon<double> dbsks_xshock_app_likelihood::
convert_edge_to_poly(bool& status, dbsksp_xshock_edge_sptr shockEdge)
{

    status = false;

    dbsksp_xshock_node_sptr xv_p, xv_c;
    if (shockEdge->source()->depth() < shockEdge->target()->depth())
    {
        xv_p = shockEdge->source();
        xv_c = shockEdge->target();
    }
    else
    {
        xv_p = shockEdge->target();
        xv_c = shockEdge->source();
    }

    assert(xv_p->depth() < xv_c->depth());

    // form an xshock fragment for the edge
    dbsksp_xshock_node_descriptor xd0 = *(xv_p->descriptor(shockEdge));
    dbsksp_xshock_node_descriptor xd1 = xv_c->descriptor(shockEdge)
        ->opposite_xnode();
    dbsksp_xshock_fragment xfrag(xd0, xd1);

    //Create a polygon out of the fragment
    int num_pts_per_side = vnl_math_max((50 - 2)/2, 2);
    vcl_vector<vgl_point_2d<double > > pts;

    // left boundary
    dbgl_biarc left_biarc;
    if (left_biarc.compute_biarc_params(xfrag.start().bnd_pt_left(), 
                                        xfrag.start().bnd_tangent_left(),
                                        xfrag.end().bnd_pt_left(), 
                                        xfrag.end().bnd_tangent_left()) 
        && left_biarc.is_consistent())
    {
        double len = left_biarc.len();
        for (int i=0; i< num_pts_per_side; ++i)
        {
            double s = (len*i) / (num_pts_per_side-1);
            vgl_point_2d<double > pt = left_biarc.point_at(s);
            pts.push_back(pt);
        }
    }
    else
    {
        status=true;
        vcl_cerr<<"Left Biarc could not be sampled properly"<<vcl_endl;
    }

    // end shock-point
    pts.push_back(xfrag.end().pt());

    // right boundary
    dbgl_biarc right_biarc;
    if (right_biarc.compute_biarc_params(xfrag.start().bnd_pt_right(), 
                                         xfrag.start().bnd_tangent_right(),
                                         xfrag.end().bnd_pt_right(), 
                                         xfrag.end().bnd_tangent_right()))
    {
        double len = right_biarc.len();
        for (int i= num_pts_per_side-1; i >= 0; --i)
        {
            double s = (len*i) / (num_pts_per_side-1);
            vgl_point_2d<double > pt = right_biarc.point_at(s);
            pts.push_back(pt);
        }
    }
    else
    {
        status = true;
        vcl_cerr<<"Right Biarc could not be sampled properly"<<vcl_endl;
    }

    pts.push_back(xfrag.start().pt());

    vgl_polygon<double> polyShape(pts);

    return polyShape;
}

// -----------------------------------------------------------------------------
// Converts a shock fragment to a polygon
vgl_polygon<double> dbsks_xshock_app_likelihood::
convert_frag_to_poly(bool& status,const dbsksp_xshock_fragment& xfrag)
{

    status = false;
    //Create a polygon out of the fragment
    int num_pts_per_side = vnl_math_max((50 - 2)/2, 2);
    vcl_vector<vgl_point_2d<double > > pts;

    // left boundary
    dbgl_biarc left_biarc;
    if (left_biarc.compute_biarc_params(xfrag.start().bnd_pt_left(), 
                                        xfrag.start().bnd_tangent_left(),
                                        xfrag.end().bnd_pt_left(), 
                                        xfrag.end().bnd_tangent_left()) 
        && left_biarc.is_consistent())
    {
        double len = left_biarc.len();
        for (int i=0; i< num_pts_per_side; ++i)
        {
            double s = (len*i) / (num_pts_per_side-1);
            vgl_point_2d<double > pt = left_biarc.point_at(s);
            pts.push_back(pt);
        }
    }
    else
    {
        status=true;
        vcl_cerr<<"Left Biarc could not be sampled properly"<<vcl_endl;
    }

    // end shock-point
    pts.push_back(xfrag.end().pt());

    // right boundary
    dbgl_biarc right_biarc;
    if (right_biarc.compute_biarc_params(xfrag.start().bnd_pt_right(), 
                                         xfrag.start().bnd_tangent_right(),
                                         xfrag.end().bnd_pt_right(), 
                                         xfrag.end().bnd_tangent_right()))
    {
        double len = right_biarc.len();
        for (int i= num_pts_per_side-1; i >= 0; --i)
        {
            double s = (len*i) / (num_pts_per_side-1);
            vgl_point_2d<double > pt = right_biarc.point_at(s);
            pts.push_back(pt);
        }
    }
    else
    {
        status = true;
        vcl_cerr<<"Right Biarc could not be sampled properly"<<vcl_endl;
    }

    pts.push_back(xfrag.start().pt());

    vgl_polygon<double> polyShape(pts);

    return polyShape;
}

static int increasing_compare(const void *x1, const void *x2)
{
    const unsigned short* f1 = (const unsigned short*)x1;
    const unsigned short* f2 = (const unsigned short*)x2;
    if (*f1<*f2)
        return -1;
    else if (*f1==*f2)
        return 0;
    else
        return 1;
}

static int decreasing_compare(const void *x1, const void *x2)
{
    const unsigned short* f1 = (const unsigned short*)x1;
    const unsigned short* f2 = (const unsigned short*)x2;
    if (*f1>*f2)
        return -1;
    else if (*f1==*f2)
        return 0;
    else
        return 1;
}




// -----------------------------------------------------------------------------
// Compute variant on earth mover distance
double dbsks_xshock_app_likelihood::emd
(vcl_vector<double>& model, vcl_vector<double>& query)
{
    // take from bdgl_region_algs.cxx
    const unsigned int min_npts = 5;
    const unsigned int n1 = model.size(); 
    const unsigned int n2 = query.size();
    if (n1<min_npts || n2<min_npts)
        return 1.0;

    unsigned short *I1 = new unsigned short[n1];
    unsigned short *I2 = new unsigned short[n2];

    //Copy over vectors
    for ( unsigned int i(0) ; i < n1 ; ++i)
    {
        I1[i]=model[i];
    }

    for ( unsigned int j(0) ; j < n2 ; ++j)
    {
        I2[j]=query[j];
    }

    //Sort the intensities in each region
    vcl_qsort( (void*)I1, n1, sizeof(unsigned short), increasing_compare );
    vcl_qsort( (void*)I2, n2, sizeof(unsigned short), decreasing_compare );
  
    //Match up the smallest intensities in the smaller region with
    //the largest intensities in the larger region.  This provides a
    //measure of the distance between the two regions
    double sum = 0;
    unsigned int n_smaller = n1; if (n2<n_smaller) n_smaller=n2;
    for (unsigned int i = 0; i<n_smaller; ++i)
    {
        //vcl_cout<<"I1[i] "<<I1[i]<<" I2[i] "<<I2[i]<<vcl_endl;
        double d = double(I1[i]) - double(I2[i]);
        sum += vcl_sqrt(d*d);
    }
  
    delete[] I1; 
    delete[] I2;
    sum /= n_smaller;

    return -sum;
}








