#ifndef hsds_fd_tree_node_index_h_
#define hsds_fd_tree_node_index_h_


template<unsigned d>
class hsds_fd_tree_node_index
{
public:
  //: default constructor
  hsds_fd_tree_node_index() {}

  //: constructor taking an index value, and a level (0 is the root)
  hsds_fd_tree_node_index(unsigned int index, unsigned char level) : idx(index), lvl(level) {}

  //: copy constructor
  hsds_fd_tree_node_index(const hsds_fd_tree_node_index& that) : idx(that.idx), lvl(that.lvl) {}

  //: the cell index
  unsigned int idx;

  //: the level of the cell (0 is root)
  unsigned int lvl;

  //: get the integer coordinate of the node in the given dimension at its level
  //: result will be in the range [0, 2^lvl - 1]
  unsigned int get_node_coordinate(unsigned int dimension) const {
    unsigned int coord = 0;
    unsigned int coord_mask = 1;
    unsigned int idx_mask = bit_mask(lvl,dimension);
    for (unsigned int l = lvl; l > 0; --l) {
      if(idx & idx_mask) {
        coord |= coord_mask;
      }
      idx_mask = idx_mask << d;
      coord_mask = coord_mask << 1;
    }
    return coord;
  }

  //: set the coordinate of the node in the given dimension at its current level
  //: coord should be in range [0, 2^lvl - 1]
  void set_node_coordinate(unsigned int dimension, unsigned int coord) {
    unsigned int idx_mask = bit_mask(lvl,dimension);
    unsigned int coord_mask = 1;
    for (unsigned int l=lvl; l > 0; --l) {
      if (coord & coord_mask) {
        // set bit
        idx |= idx_mask;
      }
      else {
        // clear bit
        idx &= ~idx_mask;
      }
      idx_mask = idx_mask << d;
      coord_mask = coord_mask << 1;
    }
  }

  //: provide a mask indicating the ith level and the dth index
  static unsigned int bit_mask(unsigned char level, unsigned int dimension) {
    return 1 << (sizeof(unsigned int)*8 - level*d + dimension);
  }

  //: provide a mask indicating the bits for the given level
  static unsigned int level_mask(unsigned char level) {
    return ((1 << d) - 1) << (sizeof(unsigned int)*8 - level*d);
  }

  //: provide a mask indicating all relevant bits for a node at the given level
  static unsigned int full_mask(unsigned char level) {
    return (~((unsigned int)0) << (sizeof(unsigned int)*8 - level*d));
  }

  //: return the number of bits to shift from lsb to get to first bit of specified level
  static unsigned int level_shift(unsigned char level) {
    return sizeof(unsigned int)*8 - level*d;
  }

  //: return the index corresponding to the parent of the specified index
  hsds_fd_tree_node_index<d> parent_index() {
    hsds_fd_tree_node_index<d> parent(*this);
    // zero out current level bits
    parent.idx &= ~(hsds_fd_tree_node_index<2>::level_mask(parent.lvl));
    // move up one level
    --parent.lvl;
    return parent;
  }

  //: return the maximum possible number of levels
  static const unsigned char MAX_LEVELS = static_cast<unsigned char>(sizeof(unsigned int)*8 / d);

  //: equality operator 
  //: note: This will return true even if the levels are different.
  //:       There cannot be two nodes in a tree with the same code but different level.
  bool operator == (hsds_fd_tree_node_index const& that) const {return (idx == that.idx);}

  //: inequality operator
  //: note: this will return false even if the levels are different
  bool operator != (hsds_fd_tree_node_index const& that) const {return (idx != that.idx);}

  //: less than operator
  bool operator < (hsds_fd_tree_node_index  const& that) const {return (idx < that.idx);}

  //: greater than operator
  bool operator > (hsds_fd_tree_node_index const& that) const {return idx > that.idx;}


};

#endif

