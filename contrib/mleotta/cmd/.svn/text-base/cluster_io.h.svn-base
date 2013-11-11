// This is mleotta/cmd/cluster2vrml.cxx
#ifndef cluster_io_h_
#define cluster_io_h_

//:
// \file
// \brief functions to read cluster and feature files
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 2/19/08
//
// \verbatim
//  Modifications
// \endverbatim


#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <vcl_fstream.h>
#include <vbl/vbl_triple.h>

#include <modrec/modrec_feature_3d.h>
#include <modrec/io/modrec_io_desc_feature_3d.h>

#include <dbcll/dbcll_euclidean_cluster.h>
#include <dbcll/dbcll_rnn_agg_clustering.h>

#include <vil/vil_image_view.h>
#include <vil/vil_crop.h>
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>


template <unsigned dim>
void read_features(const vcl_string& glob,
                   vcl_vector<modrec_desc_feature_3d<dim> >& features)
{
  vcl_vector<vcl_string> filenames;
  for (vul_file_iterator fit=glob; fit; ++fit) {
    // check to see if file is a directory.
    if (vul_file::is_directory(fit()))
      continue;
    filenames.push_back(fit());
  }

  vcl_sort(filenames.begin(), filenames.end());

  features.clear();

  for(unsigned i=0; i<filenames.size(); ++i){
    vsl_b_ifstream is(filenames[i]);
    while(is.is().good()){
      vcl_vector<modrec_desc_feature_3d<dim> > f;
      vsl_b_read(is, f);
      for(unsigned j=0; j<f.size(); ++j){
        features.push_back(f[j]);
      }
      is.is().peek();
    }
    is.close();
  }
}


template <unsigned dim>
void read_features(const vcl_string& glob,
                   vcl_vector<modrec_desc_feature_3d<dim> >& features,
                   vcl_vector<vbl_triple<unsigned,unsigned,unsigned> >& idx_array)
{
  vcl_vector<vcl_string> filenames;
  for (vul_file_iterator fit=glob; fit; ++fit) {
    // check to see if file is a directory.
    if (vul_file::is_directory(fit()))
      continue;
    filenames.push_back(fit());
  }

  vcl_sort(filenames.begin(), filenames.end());

  typedef vbl_triple<unsigned,unsigned,unsigned> utriple;
  idx_array.clear();
  features.clear();

  utriple idx(0,0,0);
  for(; idx.first<filenames.size(); ++idx.first){
    vsl_b_ifstream is(filenames[idx.first]);
    while(is.is().good()){
      vcl_vector<modrec_desc_feature_3d<dim> > f;
      vsl_b_read(is, f);
      for(idx.third=0; idx.third<f.size(); ++idx.third){
        features.push_back(f[idx.third]);
        idx_array.push_back(idx);
      }
      ++idx.second;
      is.is().peek();
    }
    idx.second=0;
    is.close();
  }
}


void read_on_surface_flags(const vcl_string& glob,
                           vcl_vector<bool>& on_surf)
{
  vcl_vector<vcl_string> filenames;
  for (vul_file_iterator fit=glob; fit; ++fit) {
    // check to see if file is a directory.
    if (vul_file::is_directory(fit()))
      continue;
    filenames.push_back(fit());
  }

  vcl_sort(filenames.begin(), filenames.end());

  on_surf.clear();

  for(unsigned i=0; i<filenames.size(); ++i){
    vsl_b_ifstream is(filenames[i]);
    while(is.is().good()){
      vcl_vector<bool> f;
      vsl_b_read(is, f);
      for(unsigned j=0; j<f.size(); ++j){
        on_surf.push_back(f[j]);
      }
      is.is().peek();
    }
    is.close();
  }
}


//: Read the cluster indices from the cluster file
void read_clusters(const vcl_string& filename,
                   vcl_vector<vcl_vector<unsigned> >& clusters)
{
  vcl_ifstream ifs(filename.c_str());
  unsigned num, a;
  clusters.clear();
  ifs >> num;
  while(ifs.good()){
    vcl_vector<unsigned> cluster_idx;
    for(unsigned i=0; i<num; ++i){
      ifs >> a;
      cluster_idx.push_back(a);
    }
    clusters.push_back(cluster_idx);
    ifs >> num;
  }

  ifs.close();
}


//: Write the cluster indices to the cluster file
void write_clusters(const vcl_string& filename,
                    const vcl_vector<vcl_vector<unsigned> >& clusters)
{
  // write the new cluster file
  vcl_ofstream ofs(filename.c_str());
  for(unsigned i=0; i<clusters.size(); ++i){
    vcl_vector<unsigned> c = clusters[i];
    ofs << c.size();
    for(unsigned j=0; j<c.size(); ++j){
      ofs << ' '<< c[j];
    }
    ofs << vcl_endl;
  }

  ofs.close();
}


//: Write the cluster indices to the cluster file
void write_clusters(const vcl_string& filename,
                    const vcl_vector<dbcll_cluster_sptr>& clusters)
{
  // write the new cluster file
  vcl_ofstream ofs(filename.c_str());
  for(unsigned i=0; i<clusters.size(); ++i){
    vcl_vector<unsigned> c = clusters[i]->members();
    ofs << c.size();
    for(unsigned j=0; j<c.size(); ++j){
      ofs << ' '<< c[j];
    }
    ofs << vcl_endl;
  }

  ofs.close();
}


//: Read the cluster trace from the file
void read_trace(const vcl_string& filename,
                vcl_vector<dbcll_trace_pt>& trace)
{
  trace.clear();
  // read the trace file
  vcl_ifstream ifs(filename.c_str());
  while(ifs.good()){
    dbcll_trace_pt t;
    ifs >> t.c1 >> t.c2 >> t.sim >> t.var;
    trace.push_back(t);
    ifs.ignore(1,'\n');
    ifs.peek();
  }

  ifs.close();
}


//: Write the cluster trace to the file
void write_trace(const vcl_string& filename,
                 const vcl_vector<dbcll_trace_pt>& trace)
{
  // write the trace file
  vcl_ofstream ofs(filename.c_str());
  for(unsigned i=0; i<trace.size(); ++i){
    const dbcll_trace_pt& t = trace[i];
    ofs << t.c1 <<' '<< t.c2 <<' '<< t.sim <<' '<< t.var << vcl_endl;
  }

  ofs.close();
}


//: Read patch image
void read_patch_image(const vcl_string& filename,
                      vcl_vector<vil_image_view<vxl_byte> >& patches)
{
  vil_image_view<vxl_byte> image = vil_load(filename.c_str());
  unsigned ni = image.ni()/16;
  assert(16*ni == image.ni());
  unsigned nj = image.nj()/16;
  assert(16*nj == image.nj());

  unsigned count = 0;
  bool done = false;
  for(unsigned j=0; j<nj && !done; ++j){
    for(unsigned i=0; i<ni && !done; ++i){
      vil_image_view<vxl_byte> p = vil_crop(image,16*i,16,16*j,16);
      // check for end block
      if(p(0,0) == 0){
        vxl_byte min=255,max=0;
        vil_math_value_range(p,min,max);
        if(max == 0){
          done = true;
          break;
        }
      }
      patches.push_back(p);
      ++count;
    }
  }
}


//: Read patch image
void read_patches(const vcl_string& glob,
                  vcl_vector<vil_image_view<vxl_byte> >& patches)
{
  vcl_vector<vcl_string> filenames;
  for (vul_file_iterator fit=glob; fit; ++fit) {
    filenames.push_back(fit());
  }

  vcl_sort(filenames.begin(), filenames.end());


  for(unsigned i=0; i<filenames.size(); ++i){
    // check to see if file is a directory.
    if (vul_file::is_directory(filenames[i])){
      vcl_cout << "directory: "<<filenames[i]<<vcl_endl;
      vcl_string dir = vul_file::strip_directory(filenames[i]);
      if(dir != "." && dir != "..")
        read_patches(filenames[i]+"/*",patches);
    }
    else{
      read_patch_image(filenames[i],patches);
    }
  }
}


//: Read patch image
void write_patch_image(const vcl_string& name,
                       const vcl_vector<vil_image_view<vxl_byte> >& patches)
{
  unsigned sx = static_cast<unsigned>(vcl_ceil(vcl_sqrt(patches.size())));
  unsigned sy = static_cast<unsigned>(vcl_ceil(double(patches.size())/sx));
  unsigned sb = patches[0].ni();
  vil_image_view<vxl_byte> image(sx*sb,sy*sb,patches[0].nplanes());
  image.fill(0);
  for(unsigned i=0; i<patches.size(); ++i){
    unsigned x = i%sx, y = i/sx;
    vil_image_view<vxl_byte> p = vil_crop(image,x*sb,sb,y*sb,sb);
    p.deep_copy(patches[i]);
  }

  vil_save(image,name.c_str());
}

#endif // cluster_io_h_
