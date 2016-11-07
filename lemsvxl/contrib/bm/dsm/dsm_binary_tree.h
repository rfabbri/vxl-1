#ifndef DSM_BINARY_TREE_H_
#define DSM_BINARY_TREE_H_
//this is /contrib/bm/dsm/dsm_binary_tree.h

#include<vbl/vbl_ref_count.h>
#include<vbl/vbl_smart_ptr.h>



class dsm_binary_tree_node: public vbl_ref_count
{
public:
	dsm_binary_tree_node( double const& bb = 0.0; double const& c = 0.0; unsigned const& id = 0.0):
	  bin_boundary(bb), counts(c), node_id(id){}
	~dsm_binary_tree_node(){}
	unsigned node_id;
	double counts;
	double bin_boundary;
	vbl_smart_ptr<dsm_binary_tree_node> llink_sptr;
	vbl_smart_ptr<dsm_binary_tree_node> rlink_sptr;
};

class dsm_binary_tree: public vbl_ref_count
{
public:
	dsm_binary_tree(){}
}

typedef vbl_smart_ptr<dsm_binary_tree_node> dsm_binary_tree_node_sptr;

#endif //DSM_BINARY_TREE_H_