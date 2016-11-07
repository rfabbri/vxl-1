// This is brcv/shp/dbskfg/pro/dbskfg_prune_fragments_process.h
#ifndef dbskfg_prune_fragments_process_h_
#define dbskfg_prune_fragments_process_h_

//:
// \file
// \brief This process prunes a set of fragments based on size,overlap
//
// \author Maruthi Narayanan
// \date 12/02/2011
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <vgl/vgl_polygon.h>


class dbskfg_prune_fragments_process : public bpro1_process 
{

public:
  //: Constructor
  dbskfg_prune_fragments_process();
  
  //: Destructor
  virtual ~dbskfg_prune_fragments_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();
  
  void clear(){polygons_.clear(); prototype_list_.clear();}

  void write_out_data(vcl_string);

private:

  void brute_force_computation(vcl_string output_distance_matrix,
                               vcl_string output_dendrogram_complete,
                               vcl_string output_dendrogram_average);

  void linear_scan(vcl_string output_distance_matrix, double threshold);

                  
  void cluster_data_complete( unsigned int num_objects,
                              double* compressed_distance_matrix,
                              double* cluster_results);

  void cluster_data_average( unsigned int num_objects,
                             double* compressed_distance_matrix,
                             double* cluster_results);

  void write_distance_matrix(double size,
                             double* compressed_distance_matrix,
                             vcl_string output_file_path);

  void write_dendrogram(double size,
                        double* dendrogram,
                        vcl_string output_file_path);

  vcl_vector< vgl_polygon<double> > polygons_;
  vcl_vector<vgl_polygon<double> > prototype_list_;
  vcl_map<vcl_pair<unsigned int,unsigned int>, double> distance_matrix_;

};

#endif  //dbskfg_prune_fragments_process_h_
