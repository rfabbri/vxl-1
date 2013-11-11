//this is contrib/bm/dncn/dncn_target_2d.cxx

#include"dncn_target_2d.h"

void dncn_target_2d::add_feature_sptr( dncn_feature_sptr feature )
{
    vcl_pair<feature_map_type::iterator, bool> ret;
    ret = this->feature_map_.insert( vcl_pair<unsigned, dncn_feature_sptr>(feature->frame(),feature) );
    if( !ret.second )
    {
        this->feature_map_.erase( feature->frame() );
        this->feature_map_.insert( vcl_pair<unsigned, dncn_feature_sptr>(feature->frame(), feature) );
    }
}//end dncn_target_2d::add_feature_sptr

dncn_feature_sptr dncn_target_2d::feature_sptr( unsigned const& frame )
{
    feature_map_type::iterator it = this->feature_map_.find( frame );

    if( it == this->feature_map_.end() )
    {
        vcl_cerr << "ERROR: dncn_target_2d::feature_sptr feature with time stamp " << frame << " does not exist in the feature map." << vcl_flush;
        return NULL;
    }
    else
        return it->second;
}//end dncn_target_2d::feature_sptr

dncn_feature_sptr dncn_target_2d::target_sptr( unsigned const& frame )
{
	feature_map_type::iterator it = this->target_map_.find( frame );

	if( it == this->target_map_.end() )
	{
		vcl_cerr << "ERROR: dncn_target_2d::target_sptr feature with time stamp " << frame << " does not exist in the feature map." << vcl_flush;
		return NULL;
	}
	else
		return it->second;
}//end dncn_target_2d::feature_sptr


