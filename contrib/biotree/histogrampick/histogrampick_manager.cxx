#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_cstring.h> // for vcl_exit()
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_sequence_filename_map.h>
#include <vil3d/file_formats/vil3d_slice_list.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_slice.h>
#include <vgui/vgui_soview2D.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_point_2d.h>

#include <bbas/bgui/bgui_image_tableau.h>
#include <bbas/bgui/bgui_picker_tableau.h> 
#include <bbas/bsta/bsta_histogram.h> 
#include "histogrampick_manager.h"
#include <slice/sliceFileManager.h>

void parse_globbed_filenames(const vcl_string & input,
                    vcl_vector<vcl_string> &filenames)  ;
//static histogrampick_manager instance
histogrampick_manager* histogrampick_manager::instance_ = 0;

//insure only one instance is created
histogrampick_manager *histogrampick_manager::instance()
{
  if (!instance_)
  {
    instance_ = new histogrampick_manager();
    instance_->init();
  }
  return histogrampick_manager::instance_;
}

// constructor/destructor
histogrampick_manager::histogrampick_manager():vgui_wrapper_tableau(),w_(0),h_(0)
{
}

histogrampick_manager::~histogrampick_manager(){}


void histogrampick_manager::init()
{
  deck_ = vgui_deck_tableau_new();

  easy2D_ = vgui_easy2D_tableau_new(deck_);
  easy2D_->set_line_width(2);
  easy2D_->set_point_radius(5);
  easy2D_->set_foreground(1,0,0,0.5);

  viewer_ = vgui_viewer2D_tableau_new(easy2D_);

  pick_ = bgui_picker_tableau_new(viewer_);
  pick_->set_line_width(2);
  pick_->set_color(1,0,0);

  samples_.resize(histogrampick_manager::N_REGION_TYPES);
  for(int i = 0; i < histogrampick_manager::N_REGION_TYPES; i++){
          samples_[i].resize(0);
  }

  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(pick_);
  this->add_child(shell);
}



void histogrampick_manager::pick_polygon()
{
        vcl_cout << "pick polygon, " << histogrampick_manager::nameOf(region_select_) << "\n";
        polygon_ = 0;
        pick_->pick_polygon(polygon_);
        if(polygon_){

                vcl_cout << "chose polygon\n";
                polygon_->describe(vcl_cout);
                vcl_cout << " convex = " <<  polygon_->is_convex() << "\n";
                vsol_box_2d_sptr bbox = polygon_->get_bounding_box();
                vcl_cout << "bbox ... ";
                bbox->print_summary(vcl_cout);

                vglpoly_.clear(); 
                vglpoly_.new_sheet();
                unsigned i = 0;
                while(polygon_->valid_index(i)){
                        vsol_point_2d_sptr pt = polygon_->vertex(i++);
                        vglpoly_.push_back(pt->x(),pt->y());
                }

                vcl_cout << "vglpoly = " ;
                vglpoly_.print(vcl_cout);

                vil_image_view<float> im;
                vil_convert_cast(curimg_->get_view(),im);

                int starty = static_cast<int>(vcl_floor(bbox->get_min_y()));
                int endy =   static_cast<int>(vcl_ceil(bbox->get_max_y()));
                int startx = static_cast<int>(vcl_floor(bbox->get_min_x()));
                int endx =   static_cast<int>(vcl_ceil(bbox->get_max_x()));

                starty = starty < 0 ? 0 : starty;
                endy = endy > im.nj() ? im.nj()-1 : endy;
                startx = startx < 0 ? 0 : startx;
                endx = endx > im.ni() ? im.ni()-1 : endx;

                for(int y =starty;  y < endy; y++){
                        for(int x = startx; x < endx; x++){
                                        if(vglpoly_.contains(x,y)){
                                                samples_[region_select_].push_back(im(x,y));
                                        }
                        }
                }

                vgl_polygon_sheet_as_array<float> x(vglpoly_);
                display_polygons_.push_back(easy2D_->add_polygon(x.n,x.x,x.y));
        }
}

void histogrampick_manager::inside_vessel(){
        region_select_ = INSIDE_VESSEL;
        vcl_cout << "easy2D_->set_foreground(1,0,0,0.5);\n";
        easy2D_->set_foreground(1,0,0,0.5);
        pick_->set_color(1,0,0);
        this->pick_polygon();
}

void histogrampick_manager::inside_boundary(){
        region_select_ = INSIDE_BOUNDARY;

        vcl_cout << "easy2D_->set_foreground(1,0.2,0.5,0.5);\n";
        easy2D_->set_foreground(1,0.2,0.5,0.5);
        pick_->set_color(1,0,0);

        this->pick_polygon();
}

void histogrampick_manager::outside_vessel(){
        region_select_ = OUTSIDE_VESSEL;
        vcl_cout << "easy2D_->set_foreground(0,0,1,0.5);\n";
        easy2D_->set_foreground(0,0,1,0.5);
        pick_->set_color(0,0,1);
        this->pick_polygon();
}

void histogrampick_manager::outside_boundary(){
        region_select_ = OUTSIDE_BOUNDARY;
        vcl_cout << "easy2D_->set_foreground(0.2,0.5,1,0.5);\n";
        easy2D_->set_foreground(0.2,0.5,1,0.5);
        pick_->set_color(0,0,1);
        this->pick_polygon();
}

//the event handler
bool histogrampick_manager::handle(vgui_event const & e)
{
  if (e.key == 'p'){
          this->pick_polygon();
  }
  else if (e.key == 'w'){
          this->write_xml();
  }
  else if (e.key == vgui_PAGE_UP || e.key == vgui_PAGE_DOWN){
          
          easy2D_->clear();
          if(deck_->current()){
                  vgui_image_tableau_sptr itab; 
                  itab.vertical_cast(deck_->current()); 
                  if(itab){
                          itab->set_image_resource(0);
                  }
                  else{ vcl_cerr << " error: no itab \n";}
          }
  }

  bool res =  this->child.handle(e);

  if (e.key == vgui_PAGE_UP || e.key == vgui_PAGE_DOWN){
          if(deck_->current()){
                  vgui_image_tableau_sptr itab; 
                  itab.vertical_cast(deck_->current()); 
                  curimg_ = vil_load_image_resource(filenames_[deck_->index()].c_str());
                  itab->set_image_resource(curimg_);
                  vgui::out << "Slice " << deck_->index() << ": " << filenames_[deck_->index()] <<   vcl_endl;
                  }
          }
          
  if(deck_->current()){
          vgui_image_tableau_sptr itab; 
          itab.vertical_cast(deck_->current()); 
          if(itab){
                  itab->set_mapping(rmp_);
                  itab->post_redraw();
                  viewer_->post_redraw();
                  easy2D_->post_redraw();
                  vgui::flush();
          }
          else{ vcl_cerr << " error: no itab \n";}
  }
  return res;
}

void histogrampick_manager::quit()
{
        vul_file::delete_file_glob("/tmp/tmp_tiff*.tif");
        vcl_exit(1);
}

void histogrampick_manager::save_histogram()
{
        vgui_dialog fname_dlg("Histogram File Output");
        static vcl_string fname;
        fname_dlg.field("Filename:", fname);
        if (!fname_dlg.ask()) return;

        float min,max;
        max = rmp_->max_L_;
        unsigned nbins = 10;
        //defaults to starting at zero
        vcl_vector<bsta_histogram<float> > histograms;
        for(int region = 0; region < histogrampick_manager::N_REGION_TYPES; region++){
                histograms.push_back(bsta_histogram<float> (max,nbins));
                for(int sample = 0; sample < samples_[region].size(); sample++)
                        histograms[region].upcount(samples_[region][sample],1);
        }
        for(int region = 0; region < histogrampick_manager::N_REGION_TYPES; region++){
                vcl_cout << nameOf(static_cast<region_type>(region)) << "\n";

                for(int i = 0; i < nbins; i++){
                        histograms[region].value_range(i,min,max);
                        vcl_cout << min << "-" << max << ": " << histograms[region].counts(i) << "\n";
                }
        }

        vcl_ofstream out(fname.c_str());
        for(int region = 0; region < histogrampick_manager::N_REGION_TYPES; region++){
                out << nameOf(static_cast<region_type>(region)) << ' ';
                out << histograms[region];
        }

        out.close();
}

void histogrampick_manager::create_temporary_tifs(const char* fname, const char* output_prefix)
{
        int ni,nj,nk;
        vcl_ifstream* in =  sliceFileManager<float>::openSliceFileForRead(fname,ni,nj,nk);
        vil_image_view<float> slice(ni,nj);

        for(int i = 0; i < nk; i++){
                if(sliceFileManager<float>::readOneSlice(in, slice, ni, nj)){
                vcl_string num; vcl_stringstream s; s << i ; s >> num;
                if(i < 10) num = "0" + num; 
                if(i < 100) num = "0" + num; 
                if(i < 1000) num = "0" + num; 
                num = output_prefix + num + ".tif";
                vil_save(slice,num.c_str(), "tiff");
                }
                else{
                        vcl_cerr << "failed to read slice " << i << "\n";
                        exit(1);
                }
        }
}
void histogrampick_manager::load_image(const char* image_filename)
{

  path_ =  vul_file::dirname(image_filename);
  extension_ =  vul_file::extension(image_filename);
  prefix_ = vul_file::basename(image_filename,extension_.c_str());

  if(!vcl_strncmp(extension_.c_str(),".slicefile",10)){
          create_temporary_tifs(image_filename,"/tmp/tmp_tiff");
          path_ = "/tmp";
          extension_ =  ".tif";
          prefix_ = "tmp_tiff0000";
  }

  int i;
  int digitcount = 0;
  for( i = prefix_.length()-1; i >= 0; i--){
          if(prefix_[i] < '0' || prefix_[i] > '9') break;
          digitcount++;
  }

  prefix_ = prefix_.substr(0,i+1);

  vcl_cerr <<  "path_ = " << path_ << vcl_endl;
  vcl_cerr <<  "prefix_ = " << prefix_ << vcl_endl;
  vcl_cerr <<  "extension_ = " << extension_ << vcl_endl;

  if(prefix_ != "" && extension_ != ""){
  vcl_cerr << "load data from " << path_ <<  vcl_endl;
  vcl_string slash = "/";
  vcl_string pounds = "";
  for(int i = 0; i < digitcount; i++)
          pounds = pounds + "#";
            

  vcl_string pattern = path_ + slash + prefix_ + pounds + extension_;
  vcl_cerr << "pattern = " << pattern << "\n" ;
  parse_globbed_filenames(pattern,filenames_);

  for(int i = 0; i < filenames_.size(); i++){
          vcl_cerr << filenames_[i] << "\n" ; 
          deck_->add( bgui_image_tableau_new());
  }

  vcl_cerr << "deck size is " << deck_->size() << "\n" ;
  d_ = deck_->size();

  if(deck_->current()){
          vgui_image_tableau_sptr itab;
          itab.vertical_cast(deck_->current()); 
          vil_image_resource_sptr res = vil_load_image_resource(filenames_[deck_->index()].c_str());
          if(res){
                  itab->set_image_resource(res);
                  vgui::out << "Slice " << deck_->index() << ": " << filenames_[deck_->index()] <<   vcl_endl;
                  vcl_cerr << "Slice " << deck_->index() << ": " << filenames_[deck_->index()] <<   vcl_endl;
          }
          else{
                  vcl_cerr << "failed to load  " << filenames_[deck_->index()].c_str() << "\n";
          }
  }

  if(deck_->current()){
                vgui_image_tableau_sptr itab; 
                itab.vertical_cast(deck_->current()); 
                vil_image_resource_sptr res = itab->get_image_resource();

                //set up a mapping parameter block
                float min_val = 0;
                float max_val = 65000;
                float gamma = 1.0;
                bool invert = false;
                bool use_glPixelMap = false;
                bool cache_buffer = true;

                rmp_ = new vgui_range_map_params(min_val,max_val, gamma,
                                invert, use_glPixelMap, cache_buffer);


                if(res){
                        w_ = res->ni();
                        h_ = res->nj();
                        switch(res->pixel_format()){
                                case VIL_PIXEL_FORMAT_BYTE:
                                        {
                                        vil_image_view<vxl_byte> v = res->get_view();
                                        vxl_byte min,max;
                                        vil_math_value_range(v,min,max);
                                        min_val = min;
                                        max_val = max;
                                        }
                                        break;
                                case VIL_PIXEL_FORMAT_UINT_16:
                                        {
                                        vil_image_view<vxl_uint_16> v = res->get_view();
                                        vxl_uint_16 min,max;
                                        vil_math_value_range(v,min,max);
                                        min_val = min;
                                        max_val = max;
                                        }
                                        break;
                                case VIL_PIXEL_FORMAT_FLOAT:
                                        {
                                        vil_image_view<float> v = res->get_view();
                                        vil_math_value_range(v,min_val,max_val);
                                        }
                                        break;

                        }

                        rmp_->min_L_ = min_val;
                        rmp_->max_L_ = max_val;

                        itab->set_mapping(rmp_);
                        curimg_ = res;

                }
                else{
                        vcl_cerr << "no resource " << "\n" ;
                }
        }
  } 
  else{
    vcl_cerr << "specify full path to an image file" << "\n" ;
  } 

  vgui_event noevent;
  this->handle(noevent);
}

void histogrampick_manager::set_levelset_params()
{

}

void histogrampick_manager::write_xml()
{
        vgui_dialog xml_name_dlg("XML Output Name");
        static vcl_string xmlname;
        xml_name_dlg.field("Filename:", xmlname);
        if (!xml_name_dlg.ask()) return;
}

void histogrampick_manager::load_image()
{
  vgui_dialog load_image_dlg("Load image file");
  static vcl_string image_filename = "";
  static vcl_string ext = "*.*";
  load_image_dlg.file("Image Filename:", ext,
      image_filename);
  if (!load_image_dlg.ask())
    return;

  viewer_->remove_child(deck_);
  deck_ = vgui_deck_tableau_new();
  viewer_->add_child(deck_);
  load_image(image_filename.c_str());
}

void histogrampick_manager::set_range(){
  vgui_dialog load_image_dlg("Set Range");
  float min = rmp_->min_L_;
  float max = rmp_->max_L_;
  load_image_dlg.field("Min:", min);
  load_image_dlg.field("Max:", max);
  if (!load_image_dlg.ask())
    return;

  vcl_cerr << "setting min to " << min << "\n" ;
  vcl_cerr << "settaxg max to " << max << "\n" ;
  rmp_->min_L_ = min;
  rmp_->max_L_ = max;
  vgui_event noevent;
  this->handle(noevent);
}
