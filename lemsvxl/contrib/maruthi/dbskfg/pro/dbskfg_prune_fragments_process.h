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

  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();
  
  void clear(){polygons_.clear(); prototype_list_.clear();}

  void write_out_data(std::string);

private:

  void brute_force_computation(std::string output_distance_matrix,
                               std::string output_dendrogram_complete,
                               std::string output_dendrogram_average);

  void linear_scan(std::string output_distance_matrix, double threshold);

                  
  void cluster_data_complete( unsigned int num_objects,
                              double* compressed_distance_matrix,
                              double* cluster_results);

  void cluster_data_average( unsigned int num_objects,
                             double* compressed_distance_matrix,
                             double* cluster_results);

  void write_distance_matrix(double size,
                             double* compressed_distance_matrix,
                             std::string output_file_path);

  void write_dendrogram(double size,
                        double* dendrogram,
                        std::string output_file_path);

  std::vector< vgl_polygon<double> > polygons_;
  std::vector<vgl_polygon<double> > prototype_list_;
  std::map<std::pair<unsigned int,unsigned int>, double> distance_matrix_;

};

#endif  //dbskfg_prune_fragments_process_h_
