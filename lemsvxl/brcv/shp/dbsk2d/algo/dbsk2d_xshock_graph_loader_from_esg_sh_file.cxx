// This is brcv/shp/dbsk2d/algo/dbsk2d_xshock_graph_loader_from_esg_sh_file.cxx
//:
// \file

#include <vcl_iostream.h>
#include <vcl_utility.h>
#include <vcl_cstring.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_loader_from_esg_sh_file.h>

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_shock_node.h>
#include <dbsk2d/dbsk2d_shock_edge_sptr.h>
#include <dbsk2d/dbsk2d_shock_edge.h>

#include <dbsk2d/dbsk2d_xshock_edge.h>
#include <dbsk2d/dbsk2d_xshock_sample_sptr.h>
#include <dbsk2d/dbsk2d_xshock_sample.h>

//: Constructor
dbsk2d_xshock_graph_loader_from_esg_sh_file::dbsk2d_xshock_graph_loader_from_esg_sh_file() : 
shock(0)
{ 
  samples_map.clear(); 
  nodes_map.clear();
  //node_adjacency_map.clear();
  edge_adjacency_map.clear();
  edges_map.clear();
  edge_samples_map.clear();
  last_sample_id = -1;
} 

//:Destructor
dbsk2d_xshock_graph_loader_from_esg_sh_file::~dbsk2d_xshock_graph_loader_from_esg_sh_file() 
{ 
  clear_all();
}

//: clear all the cached information
void dbsk2d_xshock_graph_loader_from_esg_sh_file::clear_all()
{
  shock = 0;
  shocks_map.clear();
  samples_map.clear(); 
  nodes_map.clear();
  edge_adjacency_map.clear();
  edges_map.clear();
  edge_samples_map.clear();
}

//-----------------------------------------------------------
// Load a .esg file
//-----------------------------------------------------------
dbsk2d_shock_graph_sptr 
dbsk2d_xshock_graph_loader_from_esg_sh_file::load_xshock_graph(vcl_string esg_filename, vcl_string sh_filename)
{
  //clear all cached information before loading a new file
  clear_all();

  //2) instantiate a shock graph
  shock = new dbsk2d_shock_graph();

  //3) read the all info from the file to local memory
  load_esg_file_info(esg_filename);
  load_sh_file_info(sh_filename);
  load_xshock_nodes_and_edges();
  assign_edge_samples();
  setup_connectivity_between_nodes_and_edges();
  finish_load();

  //4) close file
  fp_in.close();

  return shock;
}

void dbsk2d_xshock_graph_loader_from_esg_sh_file::load_esg_file_info(vcl_string esg_filename){
  fp_in.open(esg_filename.c_str());
  if (!fp_in.is_open()){
    vcl_cout << " : Unable to Open " << esg_filename << vcl_endl;
  }
  while(!fp_in.eof()){
    fp_in.getline(buffer,2000);
    if (!vcl_strncmp(buffer, "begin shock_group", sizeof("begin shock_group")-1)){
      bool end_of_group = false;
      Group g;
      while (!end_of_group)
      {
        fp_in.getline(buffer,2000);
        if (!vcl_strncmp(buffer, "group_id ", sizeof("group_id ")-1))
          sscanf(buffer,"group_id %d", &(g.group_id));
        if (!vcl_strncmp(buffer, "group_type ", sizeof("group_type ")-1))
          sscanf(buffer,"group_type %c", &(g.group_type));
        if (!vcl_strncmp(buffer, "group_label ", sizeof("group_label ")-1))
          sscanf(buffer,"group_label %s", &(g.group_label));
        if (!vcl_strncmp(buffer, "ordered_shocks ", sizeof("ordered_shocks ")-1)){
          vcl_stringstream ss;
          vcl_string str1=buffer;
          size_t pos1=str1.find("[");
          size_t pos2=str1.find("]");
          vcl_string str2=str1.substr(pos1+1,pos2-pos1-1);
          ss << str2;
          int temp;
          ss >> temp;
          while(!ss.fail()){
            g.ordered_shocks.push_back(temp);
            ss >> temp;
          }
        }
        if (!vcl_strncmp(buffer, "incoming_groups ", sizeof("incoming_groups ")-1)){
          vcl_stringstream ss;
          vcl_string str1=buffer;
          size_t pos1=str1.find("[");
          size_t pos2=str1.find("]");
          vcl_string str2=str1.substr(pos1+1,pos2-pos1-1);
          ss << str2;
          int temp;
          ss >> temp;
          while(!ss.fail()){
            g.incoming_groups.push_back(temp);
            ss >> temp;
          }
        }
        if (!vcl_strncmp(buffer, "outgoing_groups ", sizeof("outgoing_groups ")-1)){
          vcl_stringstream ss;
          vcl_string str1=buffer;
          size_t pos1=str1.find("[");
          size_t pos2=str1.find("]");
          vcl_string str2=str1.substr(pos1+1,pos2-pos1-1);
          ss << str2;
          int temp;
          ss >> temp;
          while(!ss.fail()){
            g.outgoing_groups.push_back(temp);
            ss >> temp;
          }
        }
        if (!vcl_strncmp(buffer, "incoming_speeds ", sizeof("incoming_speeds ")-1)){
          vcl_stringstream ss;
          vcl_string str1=buffer;
          size_t pos1=str1.find("[");
          size_t pos2=str1.find("]");
          vcl_string str2=str1.substr(pos1+1,pos2-pos1-1);
          ss << str2;
          float temp;
          ss >> temp;
          while(!ss.fail()){
            g.incoming_speeds.push_back(temp);
            ss >> temp;
          }
        }
        if (!vcl_strncmp(buffer, "incoming_thetas ", sizeof("incoming_thetas ")-1)){
          vcl_stringstream ss;
          vcl_string str1=buffer;
          size_t pos1=str1.find("[");
          size_t pos2=str1.find("]");
          vcl_string str2=str1.substr(pos1+1,pos2-pos1-1);
          ss << str2;
          float temp;
          ss >> temp;
          while(!ss.fail()){
            g.incoming_thetas.push_back(temp);
            ss >> temp;
          }
        }
        if (!vcl_strncmp(buffer, "outgoing_speeds ", sizeof("outgoing_speeds ")-1)){
          vcl_stringstream ss;
          vcl_string str1=buffer;
          size_t pos1=str1.find("[");
          size_t pos2=str1.find("]");
          vcl_string str2=str1.substr(pos1+1,pos2-pos1-1);
          ss << str2;
          float temp;
          ss >> temp;
          while(!ss.fail()){
            g.outgoing_speeds.push_back(temp);
            ss >> temp;
          }
        }
        if (!vcl_strncmp(buffer, "outgoing_thetas ", sizeof("outgoing_thetas ")-1)){
          vcl_stringstream ss;
          vcl_string str1=buffer;
          size_t pos1=str1.find("[");
          size_t pos2=str1.find("]");
          vcl_string str2=str1.substr(pos1+1,pos2-pos1-1);
          ss << str2;
          float temp;
          ss >> temp;
          while(!ss.fail()){
            g.outgoing_thetas.push_back(temp);
            ss >> temp;
          }
        }
        if (!vcl_strncmp(buffer, "end shock_group", sizeof("end shock_group")-1)){
          end_of_group = true;
        }
      }
      group_list.push_back(g);
    }
  }
  fp_in.clear();
  fp_in.close();
}

void dbsk2d_xshock_graph_loader_from_esg_sh_file::load_sh_file_info(vcl_string sh_filename){
  fp_in.open(sh_filename.c_str());
  if (!fp_in.is_open()){
    vcl_cout << " : Unable to Open " << sh_filename << vcl_endl;
  }
  while(!fp_in.eof()){
    fp_in.getline(buffer,2000);
    if (!vcl_strncmp(buffer, "begin shock", sizeof("begin shock")-1)){
      bool end_of_sample = false;
      Shock s;
      while (!end_of_sample)
      {        
        fp_in.getline(buffer,2000);
        if (!vcl_strncmp(buffer, "id ", sizeof("id ")-1)){
          sscanf(buffer,"id %d", &(s.sample_id));
        }
        if (!vcl_strncmp(buffer, "x ", sizeof("x ")-1)){
          sscanf(buffer,"x %f", &(s.x));
        }
        if (!vcl_strncmp(buffer, "y ", sizeof("y ")-1)){
          sscanf(buffer,"y %f", &(s.y));
        }
        if (!vcl_strncmp(buffer, "t ", sizeof("t ")-1)){
          sscanf(buffer,"t %f", &(s.t));
        }
        if (!vcl_strncmp(buffer, "order ", sizeof("order ")-1)){
          sscanf(buffer,"order %d", &(s.order));
        }
        if (!vcl_strncmp(buffer, "group_id ", sizeof("group_id ")-1)){
          sscanf(buffer,"group_id %d", &(s.group_id));
        }
        if (!vcl_strncmp(buffer, "label ", sizeof("label ")-1)){
          sscanf(buffer,"label %s", &(s.label));
        }
        if (!vcl_strncmp(buffer, "theta ", sizeof("theta ")-1)){
          vcl_stringstream ss;
          vcl_string str1=buffer;
          size_t pos1=str1.find("[");
          size_t pos2=str1.find("]");
          vcl_string str2=str1.substr(pos1+1,pos2-pos1-1);
          ss << str2;
          float temp;
          ss >> temp;
          while(!ss.fail()){
            s.theta.push_back(temp);
            ss >> temp;
          }
        }
        if (!vcl_strncmp(buffer, "speed ", sizeof("speed ")-1)){
          vcl_stringstream ss;
          vcl_string str1=buffer;
          size_t pos1=str1.find("[");
          size_t pos2=str1.find("]");
          vcl_string str2=str1.substr(pos1+1,pos2-pos1-1);
          ss << str2;
          float temp;
          ss >> temp;
          while(!ss.fail()){
            s.speed.push_back(temp);
            ss >> temp;
          }
        }
        if (!vcl_strncmp(buffer, "end shock", sizeof("end shock")-1)){
          end_of_sample = true;
        }
      }
    shocks_map.insert(vcl_pair<int, Shock>(s.sample_id, s));

    //keep track of the last sample id, so that new ids can be assigned to new shock samples created later
    if (s.sample_id>last_sample_id)
      last_sample_id = s.sample_id;
    }
  }
  fp_in.clear();
  fp_in.close();
}

void dbsk2d_xshock_graph_loader_from_esg_sh_file::load_xshock_nodes_and_edges(){
  int node_id,edge_id; //src_node_id,tgt_node_id, 
  for(unsigned i=0;i<group_list.size();i++){
    switch(group_list[i].group_type){
        case 'I':
        case 'J':
        case '2':
        case '4':
          {
            node_id=group_list[i].group_id;
            //create this node and add it to the graph
            dbsk2d_shock_node_sptr new_node = new dbsk2d_shock_node();
            new_node->set_id(node_id);
            shock->add_vertex(new_node);

            //also add this shock node to the nodes map so that it can be used later to instantiate edges
            nodes_map.insert(vcl_pair<int, dbsk2d_shock_node_sptr>(node_id, new_node));
            vcl_pair<vcl_vector<int>,vcl_vector<int> >edge_pairs(group_list[i].incoming_groups,group_list[i].outgoing_groups);
            edge_adjacency_map.insert(vcl_pair<int, vcl_pair<vcl_vector<int>,vcl_vector<int> > >(node_id, edge_pairs));
          }
          break;
        default:
          break;
    }
  }
  for(unsigned i=0;i<group_list.size();i++){
    switch(group_list[i].group_type){
    case '1':
    case '3':
      {
        edge_id=group_list[i].group_id;
        dbsk2d_shock_node_sptr src_node = nodes_map.find(group_list[i].incoming_groups[0])->second;
        dbsk2d_shock_node_sptr tgt_node = nodes_map.find(group_list[i].outgoing_groups[0])->second;

        //create this edge and add it to the graph
        dbsk2d_shock_edge_sptr new_edge = new dbsk2d_xshock_edge(edge_id, src_node, tgt_node);
        //don't know how to set the I/O information yet

        //add this edge to the shock graph
        shock->add_edge(new_edge);

        //add this edge to the map
        edges_map.insert(vcl_pair<int, dbsk2d_shock_edge_sptr>(edge_id, new_edge));

        //int first_sample_id = group_list[i].ordered_shocks[0];
        //int last_sample_id = group_list[i].ordered_shocks[group_list[i].ordered_shocks.size()-1];

        ////add this edge to the edge map so that edge adjacency can be properly set later
        //vcl_pair<int, int> pp1(tgt_node_id, first_sample_id);
        //vcl_pair<int, vcl_pair<int, int> > p1(src_node_id, pp1);

        //edges_map.insert(vcl_pair<vcl_pair<int, vcl_pair<int, int> >, dbsk2d_shock_edge_sptr>(p1, new_edge));

        //vcl_pair<int, int> pp2(src_node_id, last_sample_id);
        //vcl_pair<int, vcl_pair<int, int> > p2(tgt_node_id, pp2);

        //edges_map.insert(vcl_pair<vcl_pair<int, vcl_pair<int, int> >, dbsk2d_shock_edge_sptr>(p2, new_edge));

        //also add the information to the edge_samples map so that the edge samples 
        //can later be assigned to the edge after reading all the edge samples from the file
        edge_samples_map.insert(vcl_pair<dbsk2d_shock_edge_sptr, vcl_vector<int> >(new_edge, group_list[i].ordered_shocks));
        for (unsigned int j = 1; j<group_list[i].ordered_shocks.size()-1; j++){
          dbsk2d_xshock_sample_sptr new_sample = load_xshock_sample(group_list[i].ordered_shocks[j], edge_id);
          vcl_pair<int, int> temp_p(new_sample->id, new_sample->edge_id);
          samples_map.insert(vcl_pair<vcl_pair<int, int>, dbsk2d_xshock_sample_sptr>(temp_p, new_sample));      
        }
      }
      break;
    default:
      break;
    }
  }

  //read the samples at the nodes and add it to the ends of edges
  //this is because esf does not store samples at nodes, instead it stores multiple samples at the edges
  //incident at the node
  for(unsigned i=0;i<group_list.size();i++){
    switch(group_list[i].group_type){
        case 'I':
          {
            for (unsigned int j = 0; j<group_list[i].outgoing_groups.size(); j++){
              dbsk2d_xshock_sample_sptr new_sample = load_xshock_sample(group_list[i].ordered_shocks[0], group_list[i].outgoing_groups[j]);
              vcl_pair<int, int> temp_p(new_sample->id, new_sample->edge_id);
              samples_map.insert(vcl_pair<vcl_pair<int, int>, dbsk2d_xshock_sample_sptr>(temp_p, new_sample));      
            }

          }
          break;
        case 'J':
        case '2':
        case '4':
          {
            for (unsigned int j = 0; j<group_list[i].incoming_groups.size(); j++){
              dbsk2d_xshock_sample_sptr new_sample = load_xshock_sample(group_list[i].ordered_shocks[0], group_list[i].incoming_groups[j], group_list[i].incoming_speeds[j], group_list[i].incoming_thetas[j]);
              new_sample->id = ++last_sample_id;

              //update the samples list of the edges for the last sample which corresponds to the node
              dbsk2d_shock_edge_sptr cur_edge = edges_map.find(new_sample->edge_id)->second;
              edge_samples_map.find(cur_edge)->second.back() = new_sample->id;

              vcl_pair<int, int> temp_p(new_sample->id, new_sample->edge_id);
              samples_map.insert(vcl_pair<vcl_pair<int, int>, dbsk2d_xshock_sample_sptr>(temp_p, new_sample));      
            }

            for (unsigned int j = 0; j<group_list[i].outgoing_groups.size(); j++){
              dbsk2d_xshock_sample_sptr new_sample = load_xshock_sample(group_list[i].ordered_shocks[0], group_list[i].outgoing_groups[j], group_list[i].outgoing_speeds[j], group_list[i].outgoing_thetas[j]);
              new_sample->id = ++last_sample_id;

              //update the samples list of the edges for the first sample which corresponds to the node
              dbsk2d_shock_edge_sptr cur_edge = edges_map.find(new_sample->edge_id)->second;
              edge_samples_map.find(cur_edge)->second.front() = new_sample->id;

              vcl_pair<int, int> temp_p(new_sample->id, new_sample->edge_id);
              samples_map.insert(vcl_pair<vcl_pair<int, int>, dbsk2d_xshock_sample_sptr>(temp_p, new_sample));      
            }

          }
          break;
        default:
          break;
    }
  }
}

dbsk2d_xshock_sample_sptr dbsk2d_xshock_graph_loader_from_esg_sh_file::load_xshock_sample(int sample_id, int edge_id, float speed, float theta)
{
  //instantiate a sample 
  Shock s_sample = shocks_map.find(sample_id)->second;
  dbsk2d_xshock_sample_sptr new_sample = new dbsk2d_xshock_sample(sample_id);
  new_sample->pt = vgl_point_2d<double>(s_sample.x,s_sample.y);
  new_sample->radius = vcl_sqrt(s_sample.t);
  new_sample->edge_id = edge_id;
  new_sample->label = dbsk2d_xshock_sample::REGULAR; //FIXME
  new_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE; //FIXME
  new_sample->theta = theta*vnl_math::pi/180;
  new_sample->speed = speed;
  new_sample->reconstruct_boundary();
  return new_sample;
}

dbsk2d_xshock_sample_sptr dbsk2d_xshock_graph_loader_from_esg_sh_file::load_xshock_sample(int sample_id, int edge_id)
{
  //instantiate a sample 
  Shock s_sample = shocks_map.find(sample_id)->second;
  dbsk2d_xshock_sample_sptr new_sample = new dbsk2d_xshock_sample(sample_id);
  new_sample->pt = vgl_point_2d<double>(s_sample.x,s_sample.y);
  new_sample->radius = vcl_sqrt(s_sample.t);
  new_sample->edge_id = edge_id;
  new_sample->label = dbsk2d_xshock_sample::REGULAR; //FIXME
  new_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE; //FIXME
  new_sample->theta = s_sample.theta[0]*vnl_math::pi/180;
  new_sample->speed = s_sample.speed[0];
  new_sample->reconstruct_boundary();
  return new_sample;
}

void dbsk2d_xshock_graph_loader_from_esg_sh_file::assign_edge_samples()
{
  //this function assigns the edges with the list of extrinsic samples
  //after having read it from the file

  //go over all the edges and set the sample lists
  dbsk2d_shock_graph::edge_iterator e_it = shock->edges_begin();
  for ( ; e_it != shock->edges_end(); ++e_it){
    dbsk2d_shock_edge_sptr cur_edge = (*e_it);
    
    //get the sample id list from the stored map
    vcl_vector<int> sample_ids_list = edge_samples_map.find(cur_edge)->second;

    for (unsigned int i=0; i<sample_ids_list.size(); i++){
      vcl_pair<int, int> pp(sample_ids_list[i], cur_edge->id());
      ((dbsk2d_xshock_edge*)cur_edge.ptr())->push_back(samples_map.find(pp)->second);
    }

    //set the extrinsic points for drawing purposes(why not do it now?)
    cur_edge->compute_extrinsic_locus();
    cur_edge->source()->ex_pts().push_back(cur_edge->ex_pts().front());
    cur_edge->target()->ex_pts().push_back(cur_edge->ex_pts().back());
    //This is not the best place to do this (FIX ME!!!)
    cur_edge->form_shock_fragment();
  }
}

void dbsk2d_xshock_graph_loader_from_esg_sh_file::setup_connectivity_between_nodes_and_edges()
{
  //set up the connectivity from the nodes to the edges respecting their original order

  //go over all the nodes and add the appropriate edges to the incoming and outgoing list
  dbsk2d_shock_graph::vertex_iterator v_it = shock->vertices_begin();
  for ( ; v_it != shock->vertices_end(); ++v_it)
  {
    dbsk2d_shock_node_sptr cur_node = (*v_it);

    //get the adjacency list of this node
    //vcl_vector<int> adjacent_nodes_list = node_adjacency_map.find(cur_node->id())->second;
    vcl_pair<vcl_vector<int>, vcl_vector<int> > adjacent_edges_list = edge_adjacency_map.find(cur_node->id())->second;
    vcl_vector<int> incoming_edges_list = adjacent_edges_list.first;
    vcl_vector<int> outgoing_edges_list = adjacent_edges_list.second;
    for (unsigned int i=0; i<incoming_edges_list.size(); i++){
      dbsk2d_shock_edge_sptr connected_edge = edges_map.find(incoming_edges_list[i])->second;
      cur_node->add_incoming_edge(connected_edge);
//      cur_node->form_shock_fragments();
    }
    for (unsigned int i=0; i<outgoing_edges_list.size(); i++){
      dbsk2d_shock_edge_sptr connected_edge = edges_map.find(outgoing_edges_list[i])->second;
      cur_node->add_outgoing_edge(connected_edge);
//      cur_node->form_shock_fragments();
    }
  }
}

void dbsk2d_xshock_graph_loader_from_esg_sh_file::finish_load()
{
  //The shock graph is finally complete
  //no need to keep the maps any more (in fact, keeping them is risky because 
  //these smart pointers might persist for a long time)
  samples_map.clear(); 
  nodes_map.clear();
  edge_adjacency_map.clear();
  edges_map.clear();
  edge_samples_map.clear();
  shocks_map.clear();
}
