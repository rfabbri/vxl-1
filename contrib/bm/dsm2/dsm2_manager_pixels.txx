//this is /contrib/bm/dsm2_manager_pixels.txx
#ifndef DSM2_MANAGER_PIXELS_TXX_
#define DSM2_MANAGER_PIXELS_TXX_
#include<dsm2/dsm2_state_machine_base_sptr.h>
#include<dsm2/dsm2_manager_pixels.h>

#include<dsm2/io/dsm2_io_state_machine.h>

#include<vgl/io/vgl_io_point_2d.h>

template<class pixelT, class smT>
void dsm2_manager_pixels<pixelT, smT>::b_write( vsl_b_ostream& os ) const
{
    const short version_no = 1;
    vsl_b_write(os,version_no);

    //write the number of pixels
    vsl_b_write(os, this->pixel_state_machine_map.size());

    typename vcl_map<vgl_point_2d<pixelT>, dsm2_state_machine_base_sptr,
        dsm_vgl_point_2d_coord_compare<pixelT> >::const_iterator
        p_itr, p_end = this->pixel_state_machine_map.end();

    for( p_itr = this->pixel_state_machine_map.begin(); p_itr != p_end;
            ++p_itr )
    {
        //write the point
        vsl_b_write(os,p_itr->first);

        //write the node
        vsl_b_write(os, static_cast<smT*>(p_itr->second.as_pointer()));
    }//end pixel iterations

    return;
}//end b_write

template<class pixelT, class smT>
void dsm2_manager_pixels<pixelT, smT>::b_read( vsl_b_istream& is )
{
    if(!is) return;

    short v;
    vsl_b_read(is,v);

    switch(v)
    {
    case 1:
        {
            //read the number of pixels
            vcl_size_t nnodes;
            vsl_b_read(is, nnodes);

            for( vcl_size_t n = 0; n < nnodes; ++n )
            {
                //read the point
                vgl_point_2d<pixelT> pt;
                vsl_b_read(is, pt);

                //read the state machine
                smT* state_machine_ptr;
                vsl_b_read(is, state_machine_ptr);

                dsm2_state_machine_base_sptr state_machine_sptr(state_machine_ptr);
                this->pixel_state_machine_map[pt] = state_machine_sptr;
            }//end node iteration
        }//end case 1
    default:
        {
            vcl_cerr << "ERROR: dsm2_manager_pixels::b_read() -- unknown version number." << vcl_flush;
            return;
        }//end default
    }//end switch(v)
}//end b_read

#define DSM2_MANAGER_PIXELS_INSTANTIATE(pixelT,...)\
template class dsm2_manager_pixels<pixelT, __VA_ARGS__ >

#endif //DSM2_MANAGER_PIXELS_TXX_
