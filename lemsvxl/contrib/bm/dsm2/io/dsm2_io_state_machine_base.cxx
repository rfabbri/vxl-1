//this is /contrib/bm/dsm2_io_state_machine_base.cxx
#include<dsm2/io/dsm2_io_state_machine_base.h>

#include<dsm2/dsm2_state_machine.h>

void vsl_b_read(vsl_b_istream &is, dsm2_state_machine_base &state_machine_base)
{
	state_machine_base.b_read(is);
}

void vsl_b_write(vsl_b_ostream &os, dsm2_state_machine_base const& state_machine_base)
{
	state_machine_base.b_write(os);
}

void vsl_b_read(vsl_b_istream &is, dsm2_state_machine_base* &p)
{
	delete p;
	bool not_null_ptr;
	vsl_b_read(is, not_null_ptr);
	if(not_null_ptr)
	{
		if( p = dynamic_cast<dsm2_state_machine<dsm2_node_gaussian<bsta_gaussian_sphere<double,1> >,
							  dsm2_transition_map<unsigned,unsigned,unsigned> >*>(p) );
		else if(p = dynamic_cast<dsm2_state_machine<dsm2_node_gaussian<bsta_gaussian_full<double,2> >,
							  dsm2_transition_map<unsigned,unsigned,unsigned> >*> (p) );
		else if(p = dynamic_cast<dsm2_state_machine<dsm2_node_gaussian<bsta_gaussian_sphere<double,1> >,
							  dsm2_transition_map<unsigned,unsigned,double> >*> (p) );
		else if(p = dynamic_cast<dsm2_state_machine<dsm2_node_gaussian<bsta_gaussian_full<double,2> >,
							  dsm2_transition_map<unsigned,unsigned,double> >*> (p) );
		else
		{
			vcl_cerr << "---- ERROR ----\n"
#ifdef __FILE__
					 << __FILE__
#endif//__FILE__
					 << "\tvsl_b_read(vsl_b_istream &is,dsm2_feature_base* &p)\n"
					 << "\tCould not determine type of feature.\n" << vcl_flush;
		}//end default condition

		vsl_b_read(is,*p);
	}
	else
		p = 0;
}

void vsl_b_write(vsl_b_ostream &os, const dsm2_state_machine_base* p)
{
	if( p == 0 )
		vsl_b_write(os,false); //indicate null pointer stored
	else
	{
		vsl_b_write(os, true);//indicate non-null pointer stored
		vsl_b_write(os, *p);
	}
}

void vsl_print_summary(vcl_ostream& os, const dsm2_state_machine_base *p)
{
	os << "vsl_print_summary not yet implemented. " << vcl_flush;
}