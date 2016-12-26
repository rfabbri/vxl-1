// This is brcv/rec/dbskr/pro/dbskr_shock_patch_storage.cxx

//:
// \file

#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbskr/dbskr_sm_cor.h>
#include <dbskr/dbskr_shock_patch.h>

#include <vul/vul_psfile.h>
#include <vil/vil_image_resource.h>
#include <vsol/vsol_polygon_2d.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbsol/algo/dbsol_curve_algs.h>

//: Constructor
dbskr_shock_patch_storage::dbskr_shock_patch_storage() 
{
}

//: Destructor
dbskr_shock_patch_storage::~dbskr_shock_patch_storage() 
{
  clear();
}

void dbskr_shock_patch_storage::clear() 
{ 
  for (vcl_map<int, dbskr_shock_patch_sptr>::iterator iter = id_sptr_map_.begin(); iter != id_sptr_map_.end(); iter++) {
    iter->second = 0;
  }
    
  id_sptr_map_.clear();

  for (unsigned i = 0; i < patches_.size(); i++) 
    patches_[i] = 0;

  patches_.clear(); 
}

void dbskr_shock_patch_storage::add_patch(dbskr_shock_patch_sptr sp) 
{ 
  patches_.push_back(sp); 
  id_sptr_map_[sp->id()] = sp; 
}

//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* dbskr_shock_patch_storage::clone() const
{
  return new dbskr_shock_patch_storage(*this);
}


//: Binary save self to stream.
void dbskr_shock_patch_storage::b_write(vsl_b_ostream &os) const
{
  unsigned ver = version();
  vsl_b_write(os, ver);

  unsigned size = patches_.size();
  vsl_b_write(os, size);
  for (unsigned i = 0; i < patches_.size(); i++) {
    patches_[i]->b_write(os);
  }

  return;
}

//: Binary load self from stream.
void dbskr_shock_patch_storage::b_read(vsl_b_istream &is)
{
  unsigned ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 1:
      {
        unsigned cnt;
        vsl_b_read(is, cnt);
        for (unsigned i = 0; i < cnt; i++) {
          dbskr_shock_patch_sptr sp = new dbskr_shock_patch();
          sp->b_read(is);
          id_sptr_map_[sp->id()] = sp;
          patches_.push_back(sp);
        }
        break;
      }
  }
}

bool dbskr_shock_patch_storage::load_patch_shocks_and_create_trees(vcl_string storage_name, vcl_string st_postfix, 
  bool elastic_splice_cost, bool construct_circular_ends, bool combined_edit,
  float scurve_sample_ds, float scurve_interp_ds)
{
  dbsk2d_xshock_graph_fileio loader;

  //: load esfs for each patch
  for (unsigned iii = 0; iii < patches_.size(); iii++) {
    dbskr_shock_patch_sptr sp = patches_[iii];
    vcl_string patch_esf_name = storage_name.substr(0, storage_name.length()-st_postfix.size());
    vcl_ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(patch_esf_name);
    if (!test_shock_graph_for_rec(sg))
    {
      vcl_cout << patch_esf_name << " shock graph has ZERO nodes or edges!!!!!!!!!\n";
      sg = 0;
      //return false;
    }
    sp->set_shock_graph(sg);
    sp->set_tree_parameters(elastic_splice_cost,construct_circular_ends,combined_edit,scurve_sample_ds,scurve_interp_ds); // so that prepare_tree() and tree() methods of patch have the needed params
  }

  return true;
}

bool dbskr_shock_patch_storage::load_patch_shocks(
    vcl_string storage_name, vcl_string st_postfix)
{
  dbsk2d_xshock_graph_fileio loader;

  //: load esfs for each patch
  for (unsigned iii = 0; iii < patches_.size(); iii++) {
    dbskr_shock_patch_sptr sp = patches_[iii];
    vcl_string patch_esf_name = 
        storage_name.substr(0, 
                            storage_name.length()-st_postfix.size());
    vcl_ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(patch_esf_name);
    if (!test_shock_graph_for_rec(sg))
    {
      vcl_cout << patch_esf_name 
               << " shock graph has ZERO nodes or edges!!!!!!!!!\n";
      sg = 0;
      //return false;
    }
    sp->set_shock_graph(sg);
  }

  return true;
}


void dbskr_shock_patch_storage::set_tree_params_for_matching(bool elastic_splice_cost, bool construct_circular_ends, bool combined_edit,
                                    float scurve_sample_ds, float scurve_interp_ds)
{
  for (unsigned i = 0; i < patches_.size(); i++) {
    patches_[i]->set_tree_parameters(elastic_splice_cost, construct_circular_ends, combined_edit, scurve_sample_ds, scurve_interp_ds);
  }
}


bool dbskr_shock_patch_storage::create_ps_images(vil_image_resource_sptr background_img, 
                                                 vcl_string filename_base, bool outer_poly, vil_rgb<int>& color)
{
  for (unsigned i = 0; i < patches_.size(); i++) {
    vcl_stringstream app;
    app << patches_[i]->id();

    //1)If file open fails, return.
    vul_psfile psfile1((filename_base+ "_" + app.str() + ".ps").c_str(), false);

    if (!psfile1) {
      vcl_cout << " Error opening file  " << (filename_base+ "_" + app.str() + ".ps").c_str() << vcl_endl;
      return false;
    }

    vil_image_view<vxl_byte> image = background_img->get_view(0, background_img->ni(), 0, background_img->nj());
    int sizex = image.ni();
    int sizey = image.nj();
    int planes = image.nplanes();

    unsigned char *buf = new unsigned char[sizex*sizey*3];
    if (planes == 3) {
      vcl_cout << "processing color image\n";
      for (int x=0; x<sizex; ++x) 
        for (int y=0; y<sizey; ++y) {
          buf[3*(x+sizex*y)  ] = image(x,y,0);
          buf[3*(x+sizex*y)+1] = image(x,y,1);
          buf[3*(x+sizex*y)+2] = image(x,y,2);
      }
      
      
    } else if (planes == 1) {
      vcl_cout << "processing grey image\n";
      for (int x=0; x<sizex; ++x) 
        for (int y=0; y<sizey; ++y) {
          buf[3*(x+sizex*y)  ] = image(x,y,0);
          buf[3*(x+sizex*y)+1] = image(x,y,0);
          buf[3*(x+sizex*y)+2] = image(x,y,0);
      }
    }
    psfile1.print_color_image(buf,sizex,sizey);
    delete [] buf;
    psfile1.reset_bounding_box();

    psfile1.set_scale_x(50);
    psfile1.set_scale_y(50);
    
    vsol_polygon_2d_sptr poly;
    if (outer_poly) {
      poly = patches_[i]->get_outer_boundary();
      poly = fit_lines_to_contour(poly, 0.05f);
    } else {
      poly = patches_[i]->get_traced_boundary();
      poly = fit_lines_to_contour(poly, 0.05f);
    }

    // parse through all the vsol classes and save curve objects only
    psfile1.set_fg_color(1, 1, 1);
    psfile1.set_line_width(4.0);
    for (unsigned int i=1; i<poly->size();i++)
    {
        vsol_point_2d_sptr p1 = poly->vertex(i-1);
        vsol_point_2d_sptr p2 = poly->vertex(i);
        psfile1.line((float)p1->x(), (float)p1->y(), (float)p2->x(), (float)p2->y());
    }
    psfile1.set_line_width(2.0);
    psfile1.set_fg_color((float)color.R(),(float)color.G(),(float)color.B());
    
    for (unsigned int i=1; i<poly->size();i++)
    {
      vsol_point_2d_sptr p1 = poly->vertex(i-1);
      vsol_point_2d_sptr p2 = poly->vertex(i);
      psfile1.line((float)p1->x(), (float)p1->y(), (float)p2->x(), (float)p2->y());
    }

    //close file
    psfile1.close();
  }

  return true;
}



