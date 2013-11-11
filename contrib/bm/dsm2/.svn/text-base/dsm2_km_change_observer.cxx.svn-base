//this is /contrib/bm/dsm2/dsm2_km_change_observer.txx
#ifndef DSM2_KM_CHANGE_OBSERVER_TXX_
#define DSM2_KM_CHANGE_OBSERVER_TXX_

#include<dsm2/dsm2_km_change_observer.h>
#include<dsm2/dsm2_state_machine_base_sptr.h>

//for exit
#include<vcl_cstdlib.h>

#include<vsl/vsl_vector_io.h>

void dsm2_km_change_observer::update( dsm2_subject_base* s, vcl_string const& interest )
{
    if( interest == "update" )
    {
        dsm2_state_machine_base_sptr state_machine_base_sptr =
                dynamic_cast<dsm2_state_machine_base*>(s);

        if( s == NULL )
        {
            vcl_cerr << "----ERROR---- dsm_km_change_observer::update:\n"
                     << "\tdynamic_cast<dsm2_state_machine_base*>(dsm2_subject_base*) failed.\n"
                     << __FILE__ << '\n'
                     << __LINE__ << '\n' << vcl_flush;
            exit(-1);
        }//end if s == NULL
        else
        {
            //if we needed to add a new node then there is a change
            if(state_machine_base_sptr->graph.size() != this->num_nodes_ )
                this->change_.push_back(true);
            else
                this->change_.push_back(false);

            this->num_nodes_ = 
                state_machine_base_sptr->graph.size();
        }//end else s == NULL
    }//end if interest == change
}

void dsm2_km_change_observer::b_write( vsl_b_ostream& os ) const
{
    const short version_no = 1;
    vsl_b_write(os, version_no);
    
    vsl_b_write(os, this->id_);

    vsl_b_write(os, this->change_);

    vsl_b_write(os, this->num_nodes_);

    return;
}//end b_write

void dsm2_km_change_observer::b_read( vsl_b_istream& is )
{
    if(!is) return;

    short v = 0;

    vsl_b_read(is,v);

    switch(v)
    {
    case 1:
        {
            vsl_b_read(is, this->id_);

            vsl_b_read(is, this->change_);

            vsl_b_read(is, this->num_nodes_);

            break;
        }//end case 1
    default:
        {
            vcl_cerr << "----ERROR---- dsm2_km_change_observer::b_read:\n"
                     << "\t UNKNOWN VERSION NUMBER: " << v << '\n'
                     << __FILE__ << '\n'
                     << __LINE__ << '\n' << vcl_flush;
            exit(-1);
        }//end default
    }//end switch
}//end b_read
#endif //DSM2_KM_CHANGE_OBSERVER_TXX_

void dsm2_km_change_observer::write_txt( vcl_string const& filename ) const
{
    vcl_ofstream of( filename.c_str(), vcl_ios::out );

    for( unsigned i = 0; i < this->change_.size(); ++i )
    {
        if( this->change_[i] == true )
            of << bool(0) << '\n';
        else
            of << bool(1) << '\n';
    }//end change iteration

    return;
}
