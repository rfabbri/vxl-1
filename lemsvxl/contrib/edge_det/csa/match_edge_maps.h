#ifndef __match_edge_maps_h__
#define __match_edge_maps_h__

#include <vector>
#include <dbdet/edge/dbdet_edgemap_sptr.h>

// returns the cost of the assignment
double matchEdgeMaps ( dbdet_edgemap_sptr edgemap1, dbdet_edgemap_sptr edgemap2,
                       double maxDist, double outlierCost,
                       std::vector<int> &matched1, std::vector<int> &matched2);

#endif // __match_edge_maps_h__

