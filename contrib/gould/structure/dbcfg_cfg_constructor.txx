//      dbcfg_cfg_constructor.txx
#ifndef dbcfg_cfg_constructor_txx_
#define dbcfg_cfg_constructor_txx_

// Template for contour fragment graph constructor
// Benjamin Gould
// 8/03/09

#include "dbcfg_cfg_constructor.h"
#include <vcl_deque.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>



// creates a new contour fragment graph constructor
template <class T>
dbcfg_cfg_constructor<T>::dbcfg_cfg_constructor(int xsize, int ysize, T epsilon) :
xsize_(xsize),
ysize_(ysize),
epsilon_(epsilon) {
}

// destruct the contour fragment graph constructor
template <class T>
dbcfg_cfg_constructor<T>::~dbcfg_cfg_constructor() {
}

// adds a new line to the constructor
template <class T>
void dbcfg_cfg_constructor<T>::add_line(line_t& line) {
  if (vgl_distance(line.p1(), line.p2()) <= epsilon_ * 2) return;

  if (line.depth() >= (int) _lines.size()) {
    _lines.resize(line.depth() + 1);
  }

  // do not check intersections until all lines exist
  // see transform method

  _lines[line.depth()].push_back(line);
}

// adds a new line to the constructor
template <class T>
void dbcfg_cfg_constructor<T>::add_line(point_t& p1, point_t& p2, int depth) {
  this->add_line(line_t(p1, p2, depth));
}

// turns the lines in this constructor into a cfg
template <class T>
void dbcfg_cfg_constructor<T>::transform(dbcfg_contour_fragment_graph<T>& cfg) {
  cfg.reset(xsize_, ysize_);

  // forge lines
  vcl_vector<lines_t> final_lines;
  forge_transform(final_lines);

  // And end with the 'proper' list of lines : final_lines
  // which are organized by depth

  // Now, compile chains

  // TODO - TURN lines and intersections into chains
  // TODO - USE chains to determine parentage
  // TODO - CONVERT chains and parent chains into cfg

}

// creates a depth sorted collection of transformed lines
template <class T>
void dbcfg_cfg_constructor<T>::forge_transform(vcl_vector<lines_t>& final_lines) {

  vcl_vector<lines_t> merged_lines(_lines.size());

  // Merge lines for each depth
  for (unsigned depth = _lines.size() - 1; depth >= 0; depth--) {
    lines_t& lines = _lines[depth];
    vcl_vector<lines_t> clusters;

    // Cluster duplicate lines
    for (unsigned lnum = 0; lnum < lines.size(); lnum++) {
      line_t& line = lines[lnum];
      unsigned cnum = 0;
      for (; cnum < clusters.size(); cnum++) {
        lines_t& cluster = clusters[cnum];
        unsigned match = 0;
        for (; match < cluster.size(); match++) {
          if (line.is_duplicate(cluster[match], epsilon_)) break;
        }
        if (match < cluster.size()) break;
      }
      if (cnum >= clusters.size()) {
        clusters.resize(clusters.size() + 1);
      }
      clusters[cnum].push_back(line);
    }

    // Merge clusters
    lines_t& merges = merged_lines[depth];
    for (unsigned cnum = 0; cnum < clusters.size(); cnum++) {
      lines_t& cluster = clusters[cnum];
      line_t line = merge_line_cluster(cluster);
      merges.push_back(line);
    }
  }

  // Move parents to overlap children
  for (unsigned depth = merged_lines.size() - 2; depth >= 0; depth--) {
    lines_t& lines = merged_lines[depth];

    // For each line, search for a child
    for (unsigned lnum = 0; lnum < lines.size(); lnum++) {
      line_t& line = lines[lnum];

      // Look in each lower depth
      unsigned cdepth = depth + 1;
      for (; cdepth < merged_lines.size(); cdepth++) {
        lines_t& clines = merged_lines[cdepth];

        // Look at each potential child
        unsigned cnum = 0;
        for (; cnum < clines.size(); cnum++) {
          line_t& cline = clines[cnum];

          if (line.is_duplicate(cline)) {
            line = cline;
            cdepth = merged_lines.size();
            break;
          }
        }
      }
    }
  }

  // Create junctions
  
  /*
  1. Get all 'approx' junctions between pairs of edges, recording the position of junction, and the two edges
  2. Cluster close junctions just like lines were clustered
  3. Merge clusters via average position, retaining a no-repeat list of involved edges
  4. Alter all involved edges to reflect the new junction

  Then, consider chaining.
  */

  // Find all approx. junctions (line pair's intersection location)
  points_t raw_intersects;
  vcl_vector<vcl_vector<unsigned> > raw_line_depths;
  vcl_vector<vcl_vector<unsigned> > raw_line_nums;
  for (unsigned depth1 = 0; depth1 < merged_lines.size(); depth1++) {
    for (unsigned num1 = 0; num1 < merged_lines[depth1].size(); num1++) {
      line_t& line1 = merged_lines[depth1][num1];

      for (unsigned depth2 = depth1; depth2 < merged_lines.size(); depth2++) {
        for (unsigned num2 = (depth2 == depth1 ? num1 + 1 : 0); num2 < merged_lines[depth2].size(); num2++) {
          line_t& line2 = merged_lines[depth2][num2];

          if (line1.is_duplicate(line2, epsilon_)) continue;
          if (line1.intersects(line2, epsilon_)) {
            raw_intersects.push_back(line1.get_intersection(line2, epsilon_));
            raw_line_depths.push_back(vcl_vector<unsigned>());
            raw_line_depths[raw_intersects.size()].push_back(depth1);
            raw_line_depths[raw_intersects.size()].push_back(depth2);
            raw_line_nums.push_back(vcl_vector<unsigned>());
            raw_line_nums[raw_intersects.size()].push_back(num1);
            raw_line_nums[raw_intersects.size()].push_back(num2);
          }
        }
      }
    }
  }

  // Cluster raw junctions
  vcl_vector<vcl_vector<unsigned> > junction_clusters;
  for (unsigned junc_num = 0; junc_num < raw_intersects.size(); junc_num++) {
    unsigned cluster_num = 0;
    for (; cluster_num < junction_clusters.size(); cluster_num++) {
      unsigned cluster_member = 0;
      vcl_vector<unsigned>& cluster = junction_clusters[cluster_num];
      for (; cluster_member < cluster.size(); cluster_member++) {
        if (vgl_distance(raw_intersects[junc_num], raw_intersects[cluster[cluster_member]]) <= epsilon_) {
          cluster.push_back(junc_num);
          break;
        }
      }
      if (cluster_member < cluster.size()) {
        break;
      }
    }
    if (cluster_num == junction_clusters.size()) {
      junction_clusters.push_back(vcl_vector<unsigned>());
      junction_clusters[cluster_num].push_back(junc_num);
    }
  }

  // Merge clusters
  points_t intersects;
  vcl_vector<vcl_vector<unsigned> > line_depths;
  vcl_vector<vcl_vector<unsigned> > line_nums;
  for (unsigned cluster_num = 0; cluster_num < junction_clusters.size(); cluster_num++) {
    vcl_vector<unsigned>& cluster = junction_clusters[cluster_num];

    T xtotal = 0;
    T ytotal = 0;
    vcl_vector<unsigned> member_line_depths;
    vcl_vector<unsigned> member_line_nums;
    for (unsigned cluster_member = 0; cluster_member < cluster.size(); cluster_member++) {
      unsigned junc_num = cluster[cluster_member];

      xtotal += raw_intersects[junc_num].x();
      ytotal += raw_intersects[junc_num].y();

      for (unsigned member_scanner = 0; member_scanner < raw_line_depths[junc_num].size(); member_scanner++) {
        unsigned member_scan = 0;
        for (; member_scan < member_line_depths.size(); member_scan++) {
          if (raw_line_depths[junc_num][member_scanner] == member_line_depths[member_scan] &&
              raw_line_nums[junc_num][member_scanner] == member_line_nums[member_scan]) {
            break;
          }
        }
        if (member_scan == member_line_depths.size()) {
          member_line_depths.push_back(raw_line_depths[junc_num][member_scanner]);
          member_line_nums.push_back(raw_line_nums[junc_num][member_scanner]);
        }
      }
    }
    intersects.push_back(point_t(xtotal / cluster.size(), ytotal / cluster.size()));
    line_depths.push_back(member_line_depths);
    line_nums.push_back(member_line_nums);
  }

  // Alter lines at junctions
  final_lines.clear();

  // First, compile intersections for each line
  vcl_vector<vcl_vector<vcl_vector<unsigned> > > juncs_by_line;
  for (unsigned depth = 0; depth < merged_lines.size(); depth++) {
    juncs_by_line.push_back(vcl_vector<vcl_vector<unsigned> >());
    for (unsigned num = 0; num < merged_lines[depth].size(); num++) {
      juncs_by_line[depth].push_back(vcl_vector<unsigned>());
    }
  }
  for (unsigned junc = 0; junc < intersects.size(); junc++) {
    for (unsigned junc_line = 0; junc_line < line_depths[junc].size(); junc_line++) {
      juncs_by_line[line_depths[junc][junc_line]][line_nums[junc][junc_line]].push_back(junc);
    }
  }

  // Then, get the modified line(s) for each base line and its intersections
  for (unsigned depth = 0; depth < merged_lines.size(); depth++) {
    final_lines.push_back(lines_t());
    for (unsigned num = 0; num < merged_lines[depth].size(); num++) {
      lines_t new_lines = merged_lines[depth][num].get_transform(juncs_by_line[depth][num], intersects, epsilon_);
      for (unsigned new_line = 0; new_line < new_lines.size(); new_line++) {
        final_lines[depth].push_back(new_lines[new_line]);
      }
    }
  }

  // END with final_lines
}

// merges the given line cluster into a single line
template <class T>
dbcfg_line<T> dbcfg_cfg_constructor<T>::merge_line_cluster(lines_t& cluster) {
  points_t p1s;
  points_t p2s;
  p1s.push_back(cluster[0].p1());
  p2s.push_back(cluster[0].p2());
  for (unsigned lnum = 1; lnum < cluster.size(); lnum++) {
    line_t& line = cluster[lnum];
    for (unsigned match = 0; match < p1s.size(); match++) {
      if (vgl_distance(line.p1(), p1s[match]) <= epsilon_ && vgl_distance(line.p2(), p2s[match]) <= epsilon_) {
        p1s.push_back(line.p1());
        p2s.push_back(line.p2());
        break;
      } else if (vgl_distance(line.p1(), p2s[match]) <= epsilon_ && vgl_distance(line.p2(), p1s[match]) <= epsilon_) {
        p2s.push_back(line.p1());
        p1s.push_back(line.p2());
        break;
      }
    }
  }

  T x1total = 0;
  T y1total = 0;
  T x2total = 0;
  T y2total = 0;
  for (unsigned p = 0; p < p1s.size(); p++) {
    x1total += p1s[p].x();
    y1total += p1s[p].y();
    x2total += p2s[p].x();
    y2total += p2s[p].y();
  }
  x1total /= p1s.size();
  y1total /= p1s.size();
  x2total /= p1s.size();
  y2total /= p1s.size();

  point_t p1(x1total, y1total);
  point_t p2(x2total, y2total);
  line_t line(p1, p2, cluster[0].depth());
  return line;
}


// -------------------------
// INSTANTIATION
// -------------------------

#define DBCFG_CFG_CONSTRUCTOR_INSTANTIATE(T) \
template class dbcfg_cfg_constructor<T>;\

#endif dbcfg_cfg_constructor_txx_
// end dbcfg_cfg_constructor.txx

