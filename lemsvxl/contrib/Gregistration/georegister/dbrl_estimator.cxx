#include "dbrl_estimator.h"

void vsl_add_to_binary_loader(dbrl_estimator const& b)
    {
      vsl_binary_loader<dbrl_estimator >::instance().add(b);
    }
