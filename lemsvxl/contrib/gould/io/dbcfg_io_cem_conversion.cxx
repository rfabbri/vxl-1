//      dbcfg_io_cem_conversion.cxx

// Implementation for conversion between dbcfg_contour_fragment_graph and a .cem file
// Only loads v2 .cem files.
// Benjamin Gould
// 7/22/09

// Some of this code was based on code written by Amir in dbdet_cem_file_io.cxx

#include <deque>
#include "dbcfg_io_cem_conversion.h"



void dbcfg_cem_to_cfg(dbcfg_contour_fragment_graph<double> &cfg, std::string filename) {
  // Open file (v2 .cem)
  char lineBuffer[1024];
  std::ifstream fstream(filename.c_str(), std::ios::in);
  if (!fstream){
    std::cout << " Error opening file  " << filename.c_str() << std::endl;
    return;
  }
  fstream.getline(lineBuffer, 1024);
  if (std::strncmp(lineBuffer, ".CEM v2.0", sizeof(".CEM v2.0") - 1)) {
    std::cout << " File " << filename.c_str() << " is not a .CEM v2.0 file" << std::endl;
    return;
  }

  // Read file
  std::vector<dbdet_edgel* > edgels;
  int width, height;
  double x, y, dir, conf, d2f;
  int num_contours = 0;

  while (fstream.getline(lineBuffer, 1024)) {
    // Comments
    if (std::strlen(lineBuffer) < 2 || lineBuffer[0] == '#') continue;

    // Size info
    if (!std::strncmp(lineBuffer, "size=", sizeof("size=") - 1)){
      sscanf(lineBuffer,"size=[%d %d]",&width, &height);
      cfg.reset(width, height);
      continue;
    }

    // Build edgels
    if (!std::strncmp(lineBuffer, "[Edgemap]", sizeof("[Edgemap]") - 1))
    {
      fstream.getline(lineBuffer,1024);
      int num_edges = 0;
      sscanf(lineBuffer, "count=%d", &num_edges);
      for (int i = 0; i < num_edges; i++){
        fstream.getline(lineBuffer,1024);
        sscanf(lineBuffer,"(%lf, %lf)\t%lf\t%lf\t%lf", &x, &y, &dir, &conf, &d2f);
        dbdet_edgel* e = new dbdet_edgel(vgl_point_2d<double>(x, y), dir, conf, d2f);
        e->id = edgels.size();
        edgels.push_back(e);
      }
      continue;
    }

    // Build curves
    if (!std::strncmp(lineBuffer, "[Contours]", sizeof("[Contours]") - 1))
    {
      fstream.getline(lineBuffer,1024);
      sscanf(lineBuffer,"count=%d", &num_contours);
      for (int i = 0; i < num_contours; i++) {
        dbdet_edgel_chain* chain = new dbdet_edgel_chain();
        unsigned char dummy;
        int e_id;
        fstream >> dummy;
        fstream >> e_id;
        
        while (!fstream.fail()){
          chain->push_back(edgels[e_id]);
          fstream >> e_id;
        }
        fstream.clear();
        dbcfg_curve<double>* curve = new dbcfg_curve<double>(chain);
        cfg.add_curve(curve);
        fstream.getline(lineBuffer, 20);
      }
    }
  }

  // Close file
  fstream.close();
}

void dbcfg_cfg_to_cem(dbcfg_contour_fragment_graph<double> &cfg, std::string filename, int depth) {
  std::ofstream fstream(filename.c_str(), std::ios::out);
  if (!fstream){
    std::cout << " Error opening file  " << filename.c_str() << std::endl;
    return;
  }

  // Compile curves/edgels first
  std::vector<dbcfg_curve<double>* > all_curves = cfg.get_curves();
  std::vector<dbcfg_curve<double>* > depth_curves;
  std::vector<dbdet_edgel* > edgels;
  for (std::vector<dbcfg_curve<double>* >::iterator iter = all_curves.begin(); iter < all_curves.end(); iter++) {
    if ((*iter)->exists_at(depth)) {
      depth_curves.push_back(*iter);
      std::deque<dbdet_edgel* >* elist = &((*iter)->get_edgel_chain()->edgels);
      for (std::deque<dbdet_edgel* >::iterator eiter = elist->begin(); eiter < elist->end(); eiter++) {
        edgels.push_back(*eiter);
      }
    }
  }

  // Output header
  fstream << ".CEM v2.0 " << std::endl;
  fstream << "size=[" << cfg.width() << " " << cfg.height() << "]" << std::endl;

  // Output edgels
  fstream << "[Edgemap]" << std::endl;
  fstream << "count=" << edgels.size() << std::endl;
  for (unsigned i = 0; i < edgels.size(); i++){
    dbdet_edgel* e = edgels[i];
    fstream << "(" << e->pt.x() << ", " << e->pt.y() << ")\t" << e->tangent << "\t" << e->strength << "\t" << e->deriv << std::endl; 
  }

  // Output curves
  fstream << "[Contours]" << std::endl;
  fstream << "count=" << depth_curves.size() << std::endl;
  unsigned total = 0;
  for (std::vector<dbcfg_curve<double>* >::iterator iter = depth_curves.begin(); iter < depth_curves.end(); iter++){
    dbdet_edgel_chain* chain = (*iter)->get_edgel_chain();
    fstream << "[";
    unsigned i;
    for (i = 0; i < chain->edgels.size(); i++) {
      fstream << (total + i) << " ";
    }
    fstream << "]" << std::endl;
    total += i;
  }

  // Close file
  fstream.close();
}

