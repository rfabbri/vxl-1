#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include "hor_flip_shock_graph.h"

int main()
{
    dbsk2d_xshock_graph_fileio loader;
    dbsk2d_shock_graph_sptr sg, sg_flip;
    int number;

    sg = loader.load_xshock_graph("beetle-15_original.esf");
    sg_flip = hor_flip_shock_graph(sg);

    loader.save_xshock_graph(sg_flip, "beetle-15_flipped.esf");
    
    return 0;
}
