//this is /contrib/bm/dsm/dsm_frame_clock.cxx
#include"dsm_frame_clock.h"

dsm_frame_clock* dsm_frame_clock::instance_ = 0;
//dsm_frame_clock_destroyer dsm_frame_clock::destroyer_;//definition and hence instance creation

dsm_frame_clock* dsm_frame_clock::instance()
{
	if( dsm_frame_clock::instance_ == 0 )
	{
		instance_ = new dsm_frame_clock;
		//destroyer_.set_singleton(instance_);
	}

	return instance_;
}//end dsm_frame_clock::instance()

void dsm_frame_clock::b_write(vsl_b_ostream& os) const
{
	const short version_no = 1;
	vsl_b_write(os, version_no);
	vsl_b_write(os, this->curr_time_);
}//end dsm_frame_clock::b_write

void dsm_frame_clock::b_read(vsl_b_istream& is)
{
	if(!is) return;

	short v;
	vsl_b_read(is,v);
	switch(v)
	{
	case 1:
		{
			vsl_b_read(is, this->curr_time_);
			break;
		}
	default:
		{
			vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, dsm_frame_clock frame_clock)\n"
				     << "				UNKNOWN VERSION NUMBER " << v << "\n";
			is.is().clear(vcl_ios::badbit); //set an unrecoverable IO error on stream
			return;
		}
	}
	return;
}//end dsm_frame_clock::b_read

