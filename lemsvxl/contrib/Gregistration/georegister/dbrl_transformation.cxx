#include "dbrl_transformation.h"

void vsl_add_to_binary_loader(dbrl_transformation const& b)
    {
      vsl_binary_loader<dbrl_transformation >::instance().add(b);
    }
    


void dbrl_transformation::set_from_features(const vcl_vector<dbrl_feature_sptr> & from_features)
    {
        //if(from_features_.size()>0)
            from_features_.clear();
    for(unsigned i=0;i<from_features.size();i++)
        from_features_.push_back(from_features[i]);
    }
