// This is dbcll_euclidean_cluster_light.h
#ifndef dbcll_euclidean_cluster_light_h
#define dbcll_euclidean_cluster_light_h

//:
// \file
// \brief A cluster in Euclidean space that does not keep track of the ideces of the points, only of the count 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  8-Apr-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_limits.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector_fixed.h>

#include <bxml/bxml_write.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>

//Forward declarations
template <unsigned dim>
class dbcll_euclidean_cluster_light; 

template<unsigned dim> 
void dbcll_xml_read(vcl_vector<dbcll_euclidean_cluster_light<dim> > &all_clusters, 
                    const vnl_vector_fixed<double, dim> &common_mean,
                    vcl_string xml_file);

template <unsigned dim>
void dbcll_xml_read_and_init(vcl_vector<dbcll_euclidean_cluster_light<dim> > &all_clusters, 
                    vcl_vector<vnl_vector_fixed<double, dim> > const &means,
                    vcl_string xml_file);


//#define DEBUG_BOF
//: A cluster in Euclidean space that does not keep track of the ideces of the points, only of the count
//  Basically, a container for mean, point counts, and variance
template <unsigned dim>
class dbcll_euclidean_cluster_light 
{
public:
  //: Constructor - Default
  dbcll_euclidean_cluster_light()
  : mean_(0.0), var_(0.0), size_(0) {} 
  
  //: Constructor - from a single point
  dbcll_euclidean_cluster_light(const vnl_vector_fixed<double,dim>& pt)
  : mean_(pt), var_(0.0), size_(1) {}
  
  //: Constructor
  dbcll_euclidean_cluster_light(const vnl_vector_fixed<double,dim>& m, double v, unsigned long size)
  : mean_(m), var_(v), size_(size) {}
  
  //: return a measure of cluster variance
  inline double var() const { return var_; }
  
  //: return the mean
  inline const vnl_vector_fixed<double,dim>& mean() const { return mean_; }
  
  //: return the size of this cluster
  const unsigned long size() const {return size_; }
  
  //: compute similarity with another cluster
  double similarity(const  dbcll_euclidean_cluster_light<dim> & c2) const
  { 
    return -(this->var_ + c2.var_ + vnl_vector_ssd(this->mean_,c2.mean_));
  }
  
  //: compute similarity with a point
  double similarity(const vnl_vector_fixed<double,dim>& pt) const
  {
    return -(this->var_ + vnl_vector_ssd(this->mean_, pt));
  }
  
  //: merge the other cluster into this one
  void merge(const  dbcll_euclidean_cluster_light<dim>& c2)
  {   
    if(c2.size()==0)
      return;
    
    double n = static_cast<double>(this->size());
    double m = static_cast<double>(c2.size());
    double t = n+m;
    
#ifdef DEBUG_BOF
    vcl_cout <<" Merging Clusters: \n Mean1: " << this->mean_ << " ,Mean2: " << c2.mean_ 
    << " \n Var1: " << this->var_ << " ,Mean2: " << c2.var_ 
    << " \n Size1: " << this->size_ << " ,Size2: " << c2.size_ <<  "\n";
#endif
    
    this->var_ = (n*this->var_ + m*c2.var_ 
                  + n*m/t * vnl_vector_ssd(this->mean_, c2.mean_))/t;
    
    this->mean_ = (n*this->mean_ + m*c2.mean_)/t;
    
    this->size_ = t;
    
#ifdef DEBUG_BOF
    vcl_cout <<" Merging Clusters: \n Total mean: " << this->mean_ 
    << " \n Total var: " << this->var_  << " \n Total Size: " << this->size_ << "\n";
#endif

    
  }
  
  friend void dbcll_xml_read<dim>(vcl_vector<dbcll_euclidean_cluster_light<dim> > &all_clusters, 
                                  const vnl_vector_fixed<double, dim> &common_mean,
                                  vcl_string xml_file);
  
  friend void dbcll_xml_read_and_init<dim>(vcl_vector<dbcll_euclidean_cluster_light<dim> > &all_clusters, 
                                  vcl_vector<vnl_vector_fixed<double, dim> > const &means,
                                  vcl_string xml_file);
  

protected:
  vnl_vector_fixed<double,dim> mean_;
  double var_;
  unsigned long size_;
  
};

//: Generate a vector of Eucleadian clusters from points, means and cluster_indeces
template <unsigned dim>
void dbcll_init_euclidean_clusters(const vcl_vector<vnl_vector_fixed<double,dim> >& points,
                                   const vcl_vector<vcl_vector<unsigned> >& clusters,
                                   const vcl_vector<vnl_vector_fixed<double,dim> >& means,
                                   vcl_vector<dbcll_euclidean_cluster_light<dim> > &all_clusters)
{
  
#ifdef DEBUG
  vcl_cout << "Means size(): " << means.size() << "\n";
  vcl_cout << "Points size(): " << points.size() << "\n";
  vcl_cout << "Clusters size(): " << clusters.size() << "\n";
#endif
  
  for(unsigned mi=0; mi<means.size(); ++mi){
    double var = 0.0;
    vnl_vector_fixed<double,dim> mean = means[mi];
    vcl_vector<unsigned> cluster_idxs = clusters[mi];
    
    for(unsigned ci = 0; ci < cluster_idxs.size(); ci++)
      var += vnl_vector_ssd(mean, points[cluster_idxs[ci]]);
    
    var = var/(double)cluster_idxs.size();
    
    all_clusters.push_back(dbcll_euclidean_cluster_light<dim>(mean,var,cluster_idxs.size()));
  }
  
}

namespace {
  
  inline double cluster_distance(unsigned i, unsigned j,
                                 const vcl_vector<vcl_vector<double> >& dcs)
  {
    if(i==j) return 0.0;
    if(i>j)  return dcs[i][j];
    return dcs[j][i];
  }
};



//: Compute a vector of Eucleadian clusters from points and means. Return the euclidean clusters
//  This function does not modify the centers. It just associates a point to the mean with the smallest sse
template <unsigned dim>
void dbcll_compute_euclidean_clusters(const vcl_vector<vnl_vector_fixed<double,dim> >& points,
                                      const vcl_vector<vnl_vector_fixed<double,dim> >& means,
                                      vcl_vector<dbcll_euclidean_cluster_light<dim> > &clusters_out)
{
  /////////////////////
  //Compute Clusters///
  /////////////////////
  
  typedef vnl_vector_fixed<double,dim> vector;
  
#ifdef DEBUG_BOF
  vcl_cout << "Number of points: " << points.size() << "\n";
#endif
  
  if(means.empty())
    return ;
  
  const unsigned k = means.size();
  
  vcl_vector<unsigned long > sizes;
  sizes.clear();
  sizes.resize(k,0);
  
  // the distances between each pair of means
  vcl_vector<vcl_vector<double> > dcs(k);
  // compute center distances
  for(unsigned i=0; i<k; ++i)
    for(unsigned j=0; j<i; ++j)
      dcs[i].push_back(vcl_sqrt(vnl_vector_ssd(means[i],means[j])));
  
  // a vector of minimum distance between between each mean and other means
  // (divided by 2)
  vcl_vector<double> sc(k,vcl_numeric_limits<double>::infinity());
  
  //a vector to hold sum of squeare errors for each cluster
  vcl_vector<double> sse(k, 0.0);
  
  // initialize the clusters
  for(unsigned pi=0; pi<points.size(); ++pi){
    const vector& point = points[pi];
    unsigned best_mean = 0;
    double  best_dist = vcl_numeric_limits<double>::infinity();
    for(unsigned mi=0; mi<k; ++mi){
      //triangle inequality to avoid redundant distance calculations
      if(cluster_distance(mi,best_mean,dcs) > 2*best_dist)
        continue;
      double dist = vcl_sqrt(vnl_vector_ssd(point,means[mi]));
      if(dist < best_dist){
        best_dist = dist;
        best_mean = mi;
      }
    }
    sizes[best_mean]++;
    sse[best_mean]+=best_dist*best_dist;
    
  }
  
  ////////////////////////
  //Form dbcll_clusters///
  ////////////////////////
  
  clusters_out.clear();
  unsigned long total_size = 0;
  for(unsigned mi=0; mi<means.size(); ++mi){
    double var = sse[mi]/sizes[mi];
    total_size+=sizes[mi];
    clusters_out.push_back(dbcll_euclidean_cluster_light<dim>(means[mi], var, sizes[mi]));
  }
  
#ifdef DEBUG_BOF
  vcl_cout << "Size of clusters: " << total_size<< "\n";
#endif
  return;
  
}

//: Write a vector of clusters to xml_file. The indeces and means are not saves because they may be large vectors
template <unsigned dim>
void dbcll_xml_write(const vcl_vector<dbcll_euclidean_cluster_light<dim> > &all_clusters, vcl_string xml_file)
{
  bxml_document doc;
  bxml_element *root = new bxml_element("dbcll_clusters");
  doc.set_root_element(root);
  root->append_text("\n");
  
  double total_sse = 0.0;
  unsigned long total_size = 0;
  for(unsigned ci = 0; ci<all_clusters.size(); ci++)
  {
    dbcll_euclidean_cluster_light<dim> cluster = all_clusters[ci];
    bxml_element* cluster_elm = new bxml_element("cluster");
    cluster_elm->append_text("\n");
    unsigned long size = cluster.size();
    cluster_elm->set_attribute("size", size);
    cluster_elm->set_attribute("id", ci );
    cluster_elm->set_attribute("var", cluster.var());
    double sse = cluster.var()*(double)size;
    cluster_elm->set_attribute("sse", sse );
    
    root->append_data(cluster_elm);
    root->append_text("\n");
    
    total_sse+=sse;
    total_size+=size;
    
  }
  
  bxml_element* general_elm = new bxml_element("general");
  general_elm->append_text("\n");
  general_elm->append_text("\n");
  general_elm->set_attribute("total_size", total_size);
  general_elm->set_attribute("total_sse", total_sse, 15);
  root->append_data(general_elm);
  root->append_text("\n");
  
  //write to disk  
  vcl_ofstream os(xml_file.c_str());
  os.precision(15);
  bxml_write(os, doc);
  os.close();  
  
}



//: Read a vector of clusters to xml_file and a vector of means
template <unsigned dim>
void dbcll_xml_read_and_init(vcl_vector<dbcll_euclidean_cluster_light<dim> > &all_clusters, 
                    vcl_vector<vnl_vector_fixed<double, dim> > const &means,
                    vcl_string xml_file)
{
  vcl_ifstream ifs(xml_file.c_str());
  if(!ifs.is_open()){
    vcl_cerr << "Error: Could not open file: " << xml_file <<  "\n";
    return;
  }
  
  bxml_document doc = bxml_read(ifs);
  bxml_element query("dbcll_clusters");
  bxml_data_sptr root = bxml_find_by_name(doc.root_element(), query);
  if (!root) {
    vcl_cerr << "Error: bof_info - could not parse xml root\n";
    return;
  }
  
  bxml_element cluster_query("cluster");
  vcl_vector<bxml_data_sptr> clusters_data = bxml_find_all_with_name(root, cluster_query);
  
  
  unsigned nclusters = clusters_data.size();
  all_clusters.clear();
  all_clusters.resize(nclusters);
  
  //vcl_cout<< "Number of clusters: " << nclusters << "\n";
  
  for (unsigned ci = 0; ci < nclusters; ci++) {
    bxml_element* cluster_elm = dynamic_cast<bxml_element*>(clusters_data[ci].ptr());
    unsigned cl_id;
    unsigned size = 0;
    double sse = 0.0;
    double norm_size = 0.0;
    cluster_elm->get_attribute("id", cl_id);
    cluster_elm->get_attribute("size", size);
    cluster_elm->get_attribute("sse", sse);
    
    dbcll_euclidean_cluster_light<dim> &cluster = all_clusters[cl_id];
    cluster.mean_ = means[cl_id];
    if(size>0)
      cluster.var_ = sse/(double)size;
    else 
      cluster.var_ = 0.0;
    
    cluster.size_ = size;
    
  }
  
  ifs.close();
  return;
  
}

//: Read a vector of clusters to xml_file. Use a dummy mean
template <unsigned dim>
void dbcll_xml_read(vcl_vector<dbcll_euclidean_cluster_light<dim> > &all_clusters, 
                    const vnl_vector_fixed<double, dim> &common_mean,
                    vcl_string xml_file)
{
  vcl_ifstream ifs(xml_file.c_str());
  if(!ifs.is_open()){
    vcl_cerr << "Error: Could not open file: " << xml_file <<  "\n";
    return;
  }
  
  bxml_document doc = bxml_read(ifs);
  bxml_element query("dbcll_clusters");
  bxml_data_sptr root = bxml_find_by_name(doc.root_element(), query);
  if (!root) {
    vcl_cerr << "Error: bof_info - could not parse xml root\n";
    return;
  }
  
  bxml_element cluster_query("cluster");
  vcl_vector<bxml_data_sptr> clusters_data = bxml_find_all_with_name(root, cluster_query);
  
  
  unsigned nclusters = clusters_data.size();
  all_clusters.clear();
  all_clusters.resize(nclusters);
  
   //vcl_cout<< "Number of clusters: " << nclusters << "\n";
  
  for (unsigned ci = 0; ci < nclusters; ci++) {
    bxml_element* cluster_elm = dynamic_cast<bxml_element*>(clusters_data[ci].ptr());
    unsigned cl_id;
    unsigned size = 0;
    double sse = 0.0;
    double norm_size = 0.0;
    cluster_elm->get_attribute("id", cl_id);
    cluster_elm->get_attribute("size", size);
    cluster_elm->get_attribute("sse", sse);
    
    dbcll_euclidean_cluster_light<dim> &cluster = all_clusters[cl_id];
    cluster.mean_ = common_mean;
    if(size>0)
      cluster.var_ = sse/(double)size;
    else 
      cluster.var_ = 0.0;
    
    cluster.size_ = size;
    
  }
  
  ifs.close();
  return;
  
}



//: Write a basic cluster info to xml_file. The indeces and means are not saves because they may be large vectors
void dbcll_xml_write(const vcl_vector<unsigned> &sizes, const vcl_vector<double> sse_vector, vcl_string xml_file);


#endif
