#ifndef dbdet_temporal_map_params_h
#define dbdet_temporal_map_params_h


//: class to store params for producing a temporal map.

class dbdet_temporal_map_params
{
public:
    dbdet_temporal_map_params();
    dbdet_temporal_map_params(dbdet_temporal_map_params & other);
    ~dbdet_temporal_map_params();

    int rframes_;
    unsigned int tneighbor;
    int sneighbor;
    unsigned int no_of_relaxation_iterations;
    float derr;
    enum model_type{constant_velocity, normal_velocity};
    model_type type;

private:
    
    

};
#endif
