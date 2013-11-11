//this is /contrib/bm/dsm2_observable_base.cxx
#include<dsm2/dsm2_subject_base.h>

#include<vcl_utility.h>

void dsm2_subject_base::notify( vcl_string const& interest )
{
	vcl_map<vcl_string,vcl_list<dsm2_observer_base_sptr> >::iterator 
		o_itr = this->observers_.find(interest);

	if( o_itr == this->observers_.end() )
	{
		vcl_cerr << "Observer interest: " << interest 
			     << " not found.\n" << vcl_flush;
		return;
	}

	vcl_list<dsm2_observer_base_sptr>::iterator 
		l_itr, l_end = o_itr->second.end();

	for( l_itr = o_itr->second.begin(); l_itr != l_end; ++l_itr )
	{
		(*l_itr)->update(this,interest);
	}//end observer pointer list iteration

}//end notify

void dsm2_subject_base::attatch( dsm2_observer_base_sptr const& o, vcl_string const& interest )
{
	vcl_map<vcl_string,vcl_list<dsm2_observer_base_sptr> >::iterator
		o_itr = this->observers_.find(interest);

	if( o_itr == this->observers_.end() )
	{
		vcl_list<dsm2_observer_base_sptr> temp(1,o);

		this->observers_.insert(vcl_make_pair(interest,temp));
	}
	else
	{
		o_itr->second.push_back(o);
	}
}//end attactch

void dsm2_subject_base::attatch( dsm2_observer_base_sptr const& o, 
	vcl_vector<vcl_string> const& interests )
{
	vcl_vector<vcl_string>::const_iterator 
		interests_itr, interests_end = interests.end();

	for(interests_itr = interests.begin(); interests_itr != interests_end; ++interests_itr )
		this->attatch(o,*interests_itr);
}//end attatch interests

void dsm2_subject_base::remove( dsm2_observer_base_sptr& o )
{
	vcl_map<vcl_string,vcl_list<dsm2_observer_base_sptr> >::iterator
		o_itr, o_end = this->observers_.end();

	unsigned id = o->id();
	
	for(o_itr = this->observers_.begin();
			o_itr != o_end; ++o_itr )
	{
		vcl_list<dsm2_observer_base_sptr>::iterator 
			l_itr, l_end = o_itr->second.end();

		for( l_itr = o_itr->second.begin(); 
				l_itr != l_end; ++l_itr )
		{
			if( (*l_itr)->id() == id )
			{
				o_itr->second.erase(l_itr);
			}
		}//end list iteration
	}//end map iteration
}//end remove