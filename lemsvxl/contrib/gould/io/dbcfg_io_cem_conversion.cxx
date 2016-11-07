//      dbcfg_io_cem_conversion.cxx

// Implementation for conversion between dbcfg_contour_fragment_graph and a .cem file
// Only loads v2 .cem files.
// Benjamin Gould
// 7/22/09

// Some of this code was based on code written by Amir in dbdet_cem_file_io.cxx

#include <vcl_deque.h>
#include "dbcfg_io_cem_conversion.h"



void dbcfg_cem_to_cfg(dbcfg_contour_fragment_graph<double> &cfg, vcl_string filename) {
  // Open file (v2 .cem)
  char lineBuffer[1024];
  vcl_ifstream fstream(filename.c_str(), vcl_ios::in);
  if (!fstream){
    vcl_cout << " Error opening file  " << filename.c_str() << vcl_endl;
    return;
  }
  fstream.getline(lineBuffer, 1024);
  if (vcl_strncmp(lineBuffer, ".CEM v2.0", sizeof(".CEM v2.0") - 1)) {
    vcl_cout << " File " << filename.c_str() << " is not a .CEM v2.0 file" << vcl_endl;
    return;
  }

  // Read file
  vcl_vector<dbdet_edgel* > edgels;
  int width, height;
  double x, y, dir, conf, d2f;
  int num_contours = 0;

  while (fstream.getline(lineBuffer, 1024)) {
    // Comments
    if (vcl_strlen(lineBuffer) < 2 || lineBuffer[0] == '#') continue;

    // Size info
    if (!vcl_strncmp(lineBuffer, "size=", sizeof("size=") - 1)){
      sscanf(lineBuffer,"size=[%d %d]",&width, &height);
      cfg.reset(width, height);
      continue;
    }

    // Build edgels
    if (!vcl_strncmp(lineBuffer, "[Edgemap]", sizeof("[Edgemap]") - 1))
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
    if (!vcl_strncmp(lineBuffer, "[Contours]", sizeof("[Contours]") - 1))
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

void dbcfg_cfg_to_cem(dbcfg_contour_fragment_graph<double> &cfg, vcl_string filename, int depth) {
  vcl_ofstream fstream(filename.c_str(), vcl_ios::out);
  if (!fstream){
    vcl_cout << " Error opening file  " << filename.c_str() << vcl_endl;
    return;
  }

  // Compile curves/edgels first
  vcl_vector<dbcfg_curve<double>* > all_curves = cfg.get_curves();
  vcl_vector<dbcfg_curve<double>* > depth_curves;
  vcl_vector<dbdet_edgel* > edgels;
  for (vcl_vector<dbcfg_curve<double>* >::iterator iter = all_curves.begin(); iter < all_curves.end(); iter++) {
    if ((*iter)->exists_at(depth)) {
      depth_curves.push_back(*iter);
      vcl_deque<dbdet_edgel* >* elist = &((*iter)->get_edgel_chain()->edgels);
      for (vcl_deque<dbdet_edgel* >::iterator eiter = elist->begin(); eiter < elist->end(); eiter++) {
        edgels.push_back(*eiter);
      }
    }
  }

  // Output header
  fstream << ".CEM v2.0 " << vcl_endl;
  fstream << "size=[" << cfg.width() << " " << cfg.height() << "]" << vcl_endl;

  // Output edgels
  fstream << "[Edgemap]" << vcl_endl;
  fstream << "count=" << edgels.size() << vcl_endl;
  for (unsigned i = 0; i < edgels.size(); i++){
    dbdet_edgel* e = edgels[i];
    fstream << "(" << e->pt.x() << ", " << e->pt.y() << ")\t" << e->tangent << "\t" << e->strength << "\t" << e->deriv << vcl_endl; 
  }

  // Output curves
  fstream << "[Contours]" << vcl_endl;
  fstream << "count=" << depth_curves.size() << vcl_endl;
  unsigned total = 0;
  for (vcl_vector<dbcfg_curve<double>* >::iterator iter = depth_curves.begin(); iter < depth_curves.end(); iter++){
    dbdet_edgel_chain* chain = (*iter)->get_edgel_chain();
    fstream << "[";
    unsigned i;
    for (i = 0; i < chain->edgels.size(); i++) {
      fstream << (total + i) << " ";
    }
    fstream << "]" << vcl_endl;
    total += i;
  }

  // Close file
  fstream.close();
}

