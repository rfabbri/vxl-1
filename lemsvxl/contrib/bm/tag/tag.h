// this is /contrib/bm/tag/tag.h

#ifndef TAG_H_
#define TAG_H_

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_set.h>
#include <vcl_utility.h>

#include <vcl_iostream.h>
namespace tagNameSpace{

    typedef unsigned time_type;
    typedef unsigned node_id_type;
    typedef unsigned frequency_type;
    typedef vcl_vector<frequency_type> freq_array;
    typedef vcl_map<node_id_type, freq_array> tag_col;
    typedef vcl_map<node_id_type, tag_col> tag_row;

    class tag
    {
    public:
    
        tag(unsigned binWidth);
        ~tag(){}
    
        vcl_pair<tag_row::iterator,bool> addNode(node_id_type nodeID);
        //add a node with nodeID to the tag

        bool deleteNode(node_id_type nodeID);
        //remove a node with nodeID from the tag

        bool incFreq(node_id_type nodeID1, 
                     node_id_type nodeID2, time_type trel);
        //increase the transition frequency from nodeID1 to nodeID2
        //at time trel

        frequency_type freq(node_id_type nodeID1,
                            node_id_type nodeID2,
                            time_type trel);
        //return the transition frequency from nodeID1 to nodeID2
        //at time trel
        
        frequency_type sumFreq(node_id_type nodeID1, time_type trel);
        //return the total number of observations we have at at a nodeID1
        //at time t. This quantity is important to calculating the transition
        //probability normalizing constant.

        bool writeDotTrellis(vcl_ostream& os, time_type trelBegin,
                         time_type trelEnd);
        //time expanded graph

        unsigned size(){return tag_.size();}
        //returns number of nodes in the graph
        
    private:
        unsigned binWidth_;
        //the number of frames per "bin"

        tag_row tag_;  

        //map<node_id_type,time_type> trelMax_;
    };
}//end namespace tagNameSpace
#endif //TAG_H_
