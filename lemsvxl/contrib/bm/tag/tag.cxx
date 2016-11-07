// this is /contrib/bm/tag/tag.cxx

#include "tag.h"

//using namespace tagNameSpace;
namespace tagNameSpace{

    tag::tag(unsigned binWidth):binWidth_(binWidth){}

    vcl_pair<tag_row::iterator,bool> tag::addNode(unsigned nodeID)
    {
        vcl_pair<tag_row::iterator,bool> ret;
        if(tag_.count(nodeID))
        {
            vcl_cerr << "Error tag::addNode " << nodeID 
                     << " already exists." << vcl_flush;
            ret.first = tag_.end();
            ret.second = false;
            return ret;
        }
    
        //sanity check, see if the node exists in the columns as well
        tag_row::iterator tr_itr = tag_.begin();
        for(; tr_itr != tag_.end();++tr_itr)
        {
            if( tr_itr->second.count(nodeID) )
            {
                vcl_cerr << "Error tag::addNode" << nodeID 
                         << " exists in a column but not a row!" << vcl_flush;
                ret.first =  tag_.end();
                ret.second = false;
                return ret;
            }
        }
    
        freq_array temp(32,0);

        tag_[nodeID][nodeID]  = temp;
        ret.first = tag_.find(nodeID);
        ret.second = true;
        return ret;
    }//end tag::addNode

    bool tag::deleteNode(unsigned nodeID)
    {
        if( !tag_.erase(nodeID) )
        {
            vcl_cerr << "Error tag::deleteNode attempting to erase "
                     << nodeID << " but does not exist in row." << vcl_flush;
            return false;
        }

        tag_row::iterator tr_itr = tag_.begin();
        for(;tr_itr!=tag_.end();++tr_itr)
        {
            if( !tr_itr->second.erase(nodeID) )
            {
                vcl_cerr << "Error tag::deleteNode attempting to delete "
                         << nodeID << " but does not exist in column." 
                         << vcl_flush;
                return false;
            }
        }

        return true;
    }//end tag::deleteNode

    bool tag::incFreq(node_id_type nodeID1,
                      node_id_type nodeID2, time_type trel)
    {
        unsigned binIndx = trel/binWidth_;
        //the truncated integer is the appropriate bin index.
        
        tag_row::iterator tr_itr = tag_.find(nodeID1);
        tag_col::iterator tc_itr;
        if(tr_itr!=tag_.end())
        {
            tc_itr = tr_itr->second.find(nodeID2);
            if(tc_itr!=tr_itr->second.end())
            {
                //at this point we know the nodeID1 and nodeID2 exist
                while( tag_[nodeID1][nodeID2].size() < binIndx )
                {
                    //keep the vecto lengths a factor of 2 long.
                    unsigned curr_size = tag_[nodeID1][nodeID2].size();
                    tag_[nodeID1][nodeID2].resize(2*curr_size,0);
                }
                ++tag_[nodeID1][nodeID2][binIndx];
            }
            else
            {
                vcl_cerr << "Error tag::incFreq SOMETHING VERY WRONG! "
                         << '\n' << " nodeID1 " << nodeID1 << "exists "
                         << "but " << "nodeID2" << nodeID2 << " does not."
                         << vcl_flush;
                return false;
            }
        }
        else
        {
            vcl_cerr << "Error tag::incFreq nodeID1 " << nodeID1 
                     << " does not exist." << vcl_flush;
            return false;
        }
        
        return true;
    }//end tag::incFreq

    frequency_type tag::freq(node_id_type nodeID1,
                             node_id_type nodeID2, time_type trel)
    {
        unsigned binIndx = trel/binWidth_;
        if( tag_[nodeID1][nodeID2].size() < binIndx )
            return 0;
        //if the binIndx is larger than the length of our vector we havn't
        //observed this time yet therefore the frequency is zero. Had we
        //increased the frequency at this time, the incfreq function would
        //have resized the vector appropriately.
        
        return tag_[nodeID1][nodeID2][binIndx];
    }//end tag::freq

    frequency_type tag::sumFreq(node_id_type nodeID1, time_type trel)
    {
        unsigned retSum = 0;
        unsigned binIndx = trel/binWidth_;

        tag_row::iterator tr_itr = tag_.find(nodeID1);
        tag_col::iterator tc_itr;
        if( tr_itr == tag_.end() )
        {
            vcl_cerr << "Error tag::sumFreq node id: " << nodeID1 
                     << " not found. " << vcl_flush;
        }

        tc_itr = tr_itr->second.begin();
        for(; tc_itr!=tr_itr->second.end(); ++tc_itr)
        {
            if( !(tc_itr->second.size() < binIndx) )
                retSum = retSum + tc_itr->second[binIndx];
            //if the binIndx is greater than the length of the vector
            //we havn't observed anything at this time for the transition
            //frequency, hence don't add anything to retSum
        }
        
        return retSum;
    }//end tag::sumFreq
    
    bool tag::writeDotTrellis(vcl_ostream& os, time_type trelMin, 
                              time_type trelMax)
    {
        unsigned nRows = tag_.size();
        unsigned maxIndx = trelMax/binWidth_;
        unsigned minIndx = trelMin/binWidth_;
        unsigned nCols = maxIndx - minIndx;
        //implies our trellis will have nRows x nCols nodes

        //write the dot header
        os << "digraph G{\n"
           << "\t graph [center rankdir=LR]\n"
           << "\t { node \n";

        os << '\t' << '\t';
        unsigned nodeNum = 0;
        for(unsigned col = 0; col < nCols; ++col)
            for(unsigned row = 0; row < nRows; ++row)
            {
                os << nodeNum;
                ++nodeNum;
            }

        os << "}//end node" << '\n';

        os << "} //end digraph";
        

        
        return true;
    }//end tag::writeDotTrellis
    

} //end tagNameSpace


