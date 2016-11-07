#ifndef psm_convert_to_boct_h
#define psm_convert_to_boct_h

#include <vcl_vector.h>
#include <vcl_set.h>

#include <vbl/vbl_bounding_box.h>

#include <vgl/vgl_point_3d.h>

#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>

#include <hsds/hsds_fd_tree.h>
#include <hsds/hsds_fd_tree_node_index.h>

#include <boct/boct_tree.h>
#include <boct/boct_loc_code.h>
#include <boxm/boxm_scene.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_gauss_if3.h>

//: this function converts location code of hsds tree to boct_loc_code.
//: two key differences 
//: 1) LSB for hsds contain the root location code whereas it contains the lowest level for boct_loc_code.
//: 2) root is 0 in hsds as compared to  max_level_-1 in boct_loc_code.
template <class T>
void convert_hsds_node_index_to_boct_loc_code(hsds_fd_tree_node_index<3> cell_index, boct_loc_code<T> & code)
{

    short max_level=cell_index.MAX_LEVELS;
    
    unsigned int maskx=0;
    unsigned int masky=0;
    unsigned int maskz=0;

    code.level=max_level-cell_index.lvl;
int a=1<<2;
    for(unsigned int i=cell_index.lvl;i>0;i--)
    {
        maskx= 1 << (sizeof(unsigned int)*8 - i*3 + 0);
        masky= 1 << (sizeof(unsigned int)*8 - i*3 + 1);
        maskz= 1 << (sizeof(unsigned int)*8 - i*3 + 2);
        if(maskx & cell_index.idx)
            code.x_loc_+=(1<<(max_level-i));
        if(masky & cell_index.idx)
            code.y_loc_+=(1<<(max_level-i));
        if(maskz & cell_index.idx)
            code.z_loc_+=(1<<(max_level-i));
    }
}
boxm_sample<BOXM_APM_MOG_GREY> copy_data(psm_sample<PSM_APM_MOG_GREY> psm_cell)
{
    boxm_sample<BOXM_APM_MOG_GREY> boxm_cell_value(psm_cell.alpha,psm_cell.appearance);
    return boxm_cell_value;
}
boxm_sample<BOXM_APM_MOG_GREY> copy_data(psm_sample<PSM_APM_SIMPLE_GREY> psm_cell)
{

    float mean=psm_cell.appearance.color();
    float sigma=psm_cell.appearance.sigma();

    bsta_gauss_f1 gauss_f1(mean,sigma*sigma);
    bsta_num_obs<bsta_gauss_f1> obs_gauss_f1(gauss_f1,1);
    typedef bsta_num_obs<bsta_gauss_f1> type_obs_gauss_f1;
    bsta_mixture_fixed<type_obs_gauss_f1,3> mix_gauss_f1;
    typedef bsta_mixture_fixed<type_obs_gauss_f1,3> type_bsta_mixture_fixed;
    mix_gauss_f1.insert(obs_gauss_f1,1.0);
    bsta_num_obs<type_bsta_mixture_fixed> obs_mix_gauss_f1(mix_gauss_f1,1);

    boxm_sample<BOXM_APM_MOG_GREY> boxm_cell_value(psm_cell.alpha,obs_mix_gauss_f1);
    return boxm_cell_value;
}
//: copy it to boxm scene
template<psm_apm_type APM, class T_data, class T_loc>
boxm_scene<boct_tree<T_loc,T_data> > *  psm_convert_to_boct(psm_scene<APM> &scene, vcl_string dirpath, vcl_string prefix)
{   
    vcl_cout<<"Scene Length"<<scene.block_len()<<vcl_endl;

    //: copied the block dimension
    vgl_vector_3d<double> block_dim(scene.block_len(),
        scene.block_len(),
        scene.block_len());

    //: iterate over each block
    vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> > valid_blocks = scene.valid_blocks();
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
    vcl_cout<<"min/max xyz" << minx<<" "<<maxx<<" "<<miny<<" "<<maxy<<" "<<minz<<" "<<maxz<<vcl_endl;
    vcl_cout.flush();

    if(maxx-minx<0 || maxy-miny<0 || maxz-minz<0)
        return new boxm_scene<boct_tree<T_loc,T_data> >();
    //: compute the world dimensions
    vgl_vector_3d<double> worlddim(((maxx-minx)+1)*scene.block_len(),
                                    ((maxy-miny)+1)*scene.block_len(),
                                    ((maxz-minz)+1)*scene.block_len());

    vgl_point_3d<double> boxmorigin(scene.origin().x()+minx*scene.block_len(),
                                    scene.origin().y()+miny*scene.block_len(),
                                    scene.origin().z()+minz*scene.block_len());
    vcl_cout<<"boxmorigin" << boxmorigin<<vcl_endl;
    vcl_cout.flush();
    boxm_scene<boct_tree<T_loc,T_data> >  *boxmscene=new boxm_scene<boct_tree<T_loc,T_data> >(boxmorigin,block_dim,worlddim);
    boxmscene->set_paths(dirpath,prefix);

    for (vbit = valid_blocks.begin(); vbit != valid_blocks.end(); ++vbit) {
        vcl_cout<<"Round";vcl_cout.flush();
        hsds_fd_tree<psm_sample<APM>,3> &block = scene.get_block(*vbit);
        typename hsds_fd_tree<psm_sample<APM>,3>::iterator block_it = block.begin();

        //: converting array index from hsds to boxm ( can have only positive indices)
        boxmscene->load_block((*vbit).x()-minx,(*vbit).y()-miny,(*vbit).z()-minz);
        vcl_cout<<(*vbit).x()-minx<<" "<<(*vbit).y()-miny<<" "<<(*vbit).z()-minz<<"\n";
        vcl_cout.flush();
        boxm_block<boct_tree<T_loc,T_data> > * boxmblock=boxmscene->get_active_block();
        //: create tree 
        boct_tree<T_loc,T_data > * tree=new boct_tree<T_loc,T_data >(hsds_fd_tree_node_index<3>::MAX_LEVELS+1);
        vcl_cout<<"Created Tree"<<vcl_endl;
        //: iterating over cells in a block 
        for (; block_it != block.end(); ++block_it) {
            hsds_fd_tree_node_index<3> cell_index=block_it->first;
            boct_loc_code<T_loc> cell_code;
            convert_hsds_node_index_to_boct_loc_code<T_loc>(cell_index,cell_code);
            boct_tree_cell<T_loc,T_data > * atcell=tree->get_cell(cell_code);
            if(atcell)
            {
                //: code to create if missing children.
                while(atcell->level()>cell_code.level)
                {
                    atcell->split();
                    atcell=atcell->children()+cell_code.child_index(atcell->level());
                }
                psm_sample<APM> cell_value=block[cell_index];
                boxm_sample<BOXM_APM_MOG_GREY> boxm_cell_value=copy_data(cell_value);
                atcell->set_data(boxm_cell_value);
                
            }
            else
            {
                vcl_cout<<"ERROR "<<vcl_endl;
            }

        }
        vcl_cout<<tree->leaf_cells().size();
        //assert(tree->leaf_cells().size()==block.size());
        //vcl_cout<<"\n Writing the active block "<<vcl_cout.flush();
        boxmblock->init_tree(tree);
        boxmscene->write_active_block();
    }
    return boxmscene;
}




#endif

