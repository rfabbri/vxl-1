#include "dbdet_temporal_map_params.h"


dbdet_temporal_map_params::dbdet_temporal_map_params()
{
rframes_=2;
tneighbor=5;
sneighbor=5;
}

dbdet_temporal_map_params::~dbdet_temporal_map_params()
{

}

dbdet_temporal_map_params::dbdet_temporal_map_params(dbdet_temporal_map_params & other)
{
rframes_  =other.rframes_  ;
tneighbor =other.tneighbor ;
sneighbor =other.sneighbor ;
type      =other.type;
}
