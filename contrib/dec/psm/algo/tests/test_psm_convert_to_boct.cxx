#include <testlib/testlib_test.h>
#if 0
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

#include "../psm_convert_to_boct.h"
#include <psm/psm_scene.h>
#include <psm/psm_scene_base.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_sample.h>
#include <psm/algo/psm_render_expected_aa.h>

#include <hsds/hsds_fd_tree.h>
#include <vcl_vector.h>


#include <vil/vil_image_view.h>
#include <vul/vul_file.h>

#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_gauss_if3.h>
#include <vnl/vnl_vector_fixed.h>

#include <boxm/boxm_scene.h>

#include <boxm/boxm_sample.h>
void gen_synthetic_psm_scene_root_only(psm_scene_base_sptr &scene)
{
  // Simple scene with only root node
  vgl_point_3d<double> simpleorigin(0.0, 0.0, 0.0);
  double simpleblock_len = 1.0;
  vcl_string simple_storage_dir = "./simple_synthetic_scene_test";
  vul_file::make_directory(simple_storage_dir);

  scene = new psm_scene<PSM_APM_MOG_GREY>(simpleorigin, simpleblock_len, simple_storage_dir, bgeo_lvcs_sptr(), 9);
  psm_scene<PSM_APM_MOG_GREY> *simple_scene_ptr = static_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene.ptr());
  simple_scene_ptr->init_block(vgl_point_3d<int>(0,0,0),0);


  psm_sample<PSM_APM_MOG_GREY> simple_sample;
  simple_sample.alpha = 0.2f;
  bsta_gauss_f1 simple_gauss_f1(0.0,1.0);
  bsta_num_obs<bsta_gauss_f1> simple_obs_gauss_val_f1(simple_gauss_f1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>, 3>  simple_mix_gauss_val_f1;
  simple_mix_gauss_val_f1.insert(simple_obs_gauss_val_f1,0.1);
  simple_mix_gauss_val_f1.insert(simple_obs_gauss_val_f1,0.1);
  simple_mix_gauss_val_f1.insert(simple_obs_gauss_val_f1,0.1);
  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>,3>  simple_bsta_mixture_fixed_f1_3;
  bsta_num_obs<simple_bsta_mixture_fixed_f1_3>  simple_obs_mix_gauss_val_f1(simple_mix_gauss_val_f1);

  simple_sample.appearance=simple_obs_mix_gauss_val_f1;

  hsds_fd_tree<psm_sample<PSM_APM_MOG_GREY>,3> &simple_block  = simple_scene_ptr->get_block(vgl_point_3d<int>(0,0,0));
  simple_block.data_at(vnl_vector_fixed<double,3>(0.5, 0.5, 0.5)) = simple_sample;
 
  boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > *simple_boxmscene
      =psm_convert_to_boct<PSM_APM_MOG_GREY,boxm_sample<BOXM_APM_MOG_GREY>,short > (*simple_scene_ptr,"./simple_synthetic_scene_test","block");
  simple_boxmscene->write_scene();

  //delete simple_boxmscene;
  boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > *simple_boxmscene1=new boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > ();
  simple_boxmscene1->load_scene("./simple_synthetic_scene_test/scene.bin");

  simple_boxmscene1->load_block(0,0,0);
  boxm_block<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > >* simpleboxmblock=simple_boxmscene1->get_active_block();
  boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> >* simple_tree=simpleboxmblock->get_tree();
  boct_tree_cell<short, boxm_sample<BOXM_APM_MOG_GREY> >* simple_cell=simple_tree->locate_point(vgl_point_3d<double>(0.5, 0.5, 0.5));
  
  TEST("Checking alpha of the cell",simple_sample.alpha,simple_cell->data().alpha);
  TEST("Checking appearance of the cell",simple_sample.appearance.num_components(),simple_cell->data().appearance.num_components());
}
 void gen_synthetic_psm_scene(psm_scene_base_sptr &scene)
 {
  // create scene
  vgl_point_3d<double> origin(0.0, 0.0, 0.0);
  double block_len = 1.0;
  vcl_string storage_dir = "./synthetic_scene_test";
  vul_file::make_directory(storage_dir);

  scene = new psm_scene<PSM_APM_MOG_GREY>(origin, block_len, storage_dir, 9);
  psm_scene<PSM_APM_MOG_GREY> *scene_ptr = static_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene.ptr());
  scene_ptr->init_block(vgl_point_3d<int>(0,0,0),1);
  
  // fill in scene
  psm_sample<PSM_APM_MOG_GREY> sample;
  sample.alpha = 2;
  //hsds_fd_tree<psm_sample<PSM_APM_MOG_GREY>,3> &block = scene_ptr->get_block(vgl_point_3d<int>(0,0,0));
  //block.fill(sample);
  bsta_gauss_f1 gauss_f1(0.0,1.0);
  bsta_num_obs<bsta_gauss_f1> obs_gauss_val_f1(gauss_f1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>, 3>  mix_gauss_val_f1;
  mix_gauss_val_f1.insert(obs_gauss_val_f1,0.1);
  mix_gauss_val_f1.insert(obs_gauss_val_f1,0.1);
  mix_gauss_val_f1.insert(obs_gauss_val_f1,0.1);
  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>,3>  bsta_mixture_fixed_f1_3;
  bsta_num_obs<bsta_mixture_fixed_f1_3>  obs_mix_gauss_val_f1(mix_gauss_val_f1);
  sample.appearance=obs_mix_gauss_val_f1;


  hsds_fd_tree<psm_sample<PSM_APM_MOG_GREY>,3> &block  = scene_ptr->get_block(vgl_point_3d<int>(0,0,0));
  block.data_at(vnl_vector_fixed<double,3>(0.2, 0.2, 0.2)) = sample;
  sample.alpha=4;
  block.data_at(vnl_vector_fixed<double,3>(0.7, 0.7, 0.7)) = sample;
  scene_ptr->set_block(vgl_point_3d<int>(0,0,0),block);
  boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > *boxmscene
      =psm_convert_to_boct<PSM_APM_MOG_GREY,boxm_sample<BOXM_APM_MOG_GREY>,short > (*scene_ptr,"./synthetic_scene_test","block");
  boxmscene->write_scene();
  delete boxmscene;

  
  boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > boxmscene1;
  boxmscene1.load_scene("./synthetic_scene_test/scene.bin");

  boxmscene1.load_block(0,0,0);
  boxm_block<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > >* boxmblock=boxmscene1.get_active_block();
  boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> >* tree=boxmblock->get_tree();
  boct_tree_cell<short, boxm_sample<BOXM_APM_MOG_GREY> >* cell=tree->locate_point(vgl_point_3d<double>(0.2, 0.2, 0.2));
  TEST("Checking alpha of the cell",2,cell->data().alpha);
  TEST("Checking appearance of the cell",sample.appearance.num_components(),cell->data().appearance.num_components());
  cell=tree->locate_point(vgl_point_3d<double>(0.7, 0.7, 0.7));
  TEST("Checking alpha of the cell",4,cell->data().alpha);
  TEST("Checking appearance of the cell",sample.appearance.num_components(),cell->data().appearance.num_components());

  return;
}

void convert_psm_scene_toboxm_scene()
{
  vgl_point_3d<double> origin(-180,-160,-75);
  double block_len = 358.4;
  vcl_string storage_dir = "D:/vj/data/CapitolSiteHigh/psm";
  int max_levels=10;
   psm_scene<PSM_APM_MOG_GREY> * scene_ptr =new psm_scene<PSM_APM_MOG_GREY>(origin, block_len, storage_dir, max_levels);
   boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > *boxmscene
       =psm_convert_to_boct<PSM_APM_MOG_GREY,boxm_sample<BOXM_APM_MOG_GREY>,short > (*scene_ptr,"D:/vj/data/CapitolSiteHigh/boxm1","block");
   boxmscene->write_scene();
   delete boxmscene;

}

bool test_conversion(psm_scene<PSM_APM_MOG_GREY> * scene_ptr, boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > *boxmscene)
{
    vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> > valid_blocks = scene_ptr->valid_blocks();
    vcl_cout<<"# of blocks" << valid_blocks.size()<<vcl_endl;
    vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> >::iterator vbit = valid_blocks.begin();

    int minx=10000; int miny=10000;  int minz=10000;
    int maxx=-10000;int maxy=-10000; int maxz=-10000;

    for (; vbit != valid_blocks.end(); ++vbit) {
        if(vbit->x()<minx)            minx=vbit->x();
        if(vbit->x()>maxx)            maxx=vbit->x();
        if(vbit->y()<miny)            miny=vbit->y();
        if(vbit->y()>maxy)            maxy=vbit->y();
        if(vbit->z()<minz)            minz=vbit->z();
        if(vbit->z()>maxz)            maxz=vbit->z();
    }
    vcl_cout<<"min/max xyz" <<minx<<" "<<maxx<<" "<<miny<<" "
                            <<maxy<<" "<<minz<<" "<<maxz<<vcl_endl;
    vcl_cout.flush();

    if(maxx-minx<0 || maxy-miny<0 || maxz-minz<0)
        return false;
    bool flag=true;
    for (vbit = valid_blocks.begin(); vbit != valid_blocks.end(); ++vbit) {
        hsds_fd_tree<psm_sample<PSM_APM_MOG_GREY>,3> &block = scene_ptr->get_block(*vbit);
        hsds_fd_tree<psm_sample<PSM_APM_MOG_GREY>,3>::iterator block_it = block.begin();
       
        //: converting array index from hsds to boxm ( can have only positive indices)
        boxmscene->load_block((*vbit).x()-minx,(*vbit).y()-miny,(*vbit).z()-minz);
        boxm_block<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > > * boxmblock=boxmscene->get_active_block();
        boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > * tree=boxmblock->get_tree();
        vcl_cout<<"# of leaf nodes "<<tree->leaf_cells().size()<<vcl_endl;
        //: iterating over cells in a block 
        for (; block_it != block.end(); ++block_it) {
            hsds_fd_tree_node_index<3> cell_index=block_it->first;
            boct_loc_code<short> cell_code;
            convert_hsds_node_index_to_boct_loc_code<short>(cell_index,cell_code);
            boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> > * atcell=tree->get_cell(cell_code);
            psm_sample<PSM_APM_MOG_GREY> cell_value=block[cell_index];
            if(atcell)
            {
                if(atcell->data().alpha!=cell_value.alpha || 
                    atcell->data().appearance.num_components()!=cell_value.appearance.num_components())
                {
                    flag=false;
                }
            }
        }
    }

return flag;
}

MAIN( test_convert_to_boct ){
START ("Conversion");
convert_psm_scene_toboxm_scene();
vgl_point_3d<double> origin(-180,-160,-75);
double block_len = 358.4;
vcl_string storage_dir = "D:/vj/data/CapitolSiteHigh/psm";
int max_levels=10;
psm_scene<PSM_APM_MOG_GREY> * scene_ptr =new psm_scene<PSM_APM_MOG_GREY>(origin, block_len, storage_dir, max_levels);
boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > boxmscene1;
boxmscene1.load_scene("D:/vj/data/CapitolSiteHigh/boxm/scene.xml");
if(test_conversion(scene_ptr,&boxmscene1))
 vcl_cout<<"PASSED \n";

  SUMMARY();

}

#endif
