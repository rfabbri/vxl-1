// This is brcv/shp/dbsk2d/algo/dbsk2d_xshock_graph_fileio.cxx

//:
// \file

#include <vcl_iostream.h>
#include <vcl_utility.h>
#include <vcl_cstring.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

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
dbsk2d_xshock_graph_fileio::dbsk2d_xshock_graph_fileio() : 
  shock(0)
{ 
  samples_map.clear(); 
  nodes_map.clear();
  node_adjacency_map.clear();
  edges_map.clear();
  edge_samples_map.clear();
} 

//:Destructor
dbsk2d_xshock_graph_fileio::~dbsk2d_xshock_graph_fileio() 
{ 
  clear_all();
}

//: clear all the cached information
void dbsk2d_xshock_graph_fileio::clear_all()
{
  shock = 0;
  samples_map.clear(); 
  nodes_map.clear();
  node_adjacency_map.clear();
  edges_map.clear();
  edge_samples_map.clear();
}

//-----------------------------------------------------------
// Load a .esf file
//-----------------------------------------------------------
dbsk2d_shock_graph_sptr 
dbsk2d_xshock_graph_fileio::load_xshock_graph(vcl_string filename)
{
  //clear all cached information before loading a new file
  clear_all();

  //1) open the esf file
  fp_in.open(filename.c_str());

  if (!fp_in.is_open()){
    vcl_cout << " : Unable to Open " << filename << vcl_endl;
    return 0;
  }

  //2) instantiate a shock graph
  shock = new dbsk2d_shock_graph();

  //3) read the info from the  file in the shock graph
  load_xshock_header();
  load_xshock_node_description();
  load_xshock_node_samples();
  load_xshock_edge_description();
  load_xshock_edge_samples();
  assign_edge_samples();
  setup_connectivity_between_nodes_and_edges();
  finish_load();

  //4) close file
  fp_in.close();

  return shock;
}

void dbsk2d_xshock_graph_fileio::load_xshock_header()
{
  bool end_of_header = false;

  while (!end_of_header)
  {
    //read next line
    fp_in.getline(buffer,2000);

    if (!vcl_strncmp(buffer, "Extrinsic Shock File v1.0", sizeof("Extrinsic Shock File v1.0")-1))
      version = 1;

    //else if (!vcl_strncmp(buffer, "Begin [GENERAL INFO]", sizeof("Begin [GENERAL INFO]")-1))

    else if (!vcl_strncmp(buffer, "Number of Nodes: ", sizeof("Number of Nodes: ")-1))
      sscanf(buffer,"Number of Nodes: %d", &(num_nodes));

    else if (!vcl_strncmp(buffer, "Number of Edges: ", sizeof("Number of Edges: ")-1))
      sscanf(buffer,"Number of Edges: %d", &(num_edges));

    else if (!vcl_strncmp(buffer, "Number of Sample Points: ", sizeof("Number of Sample Points: ")-1))
      sscanf(buffer,"Number of Sample Points: %d", &(num_samples));

    else if (!vcl_strncmp(buffer, "End [GENERAL INFO]", sizeof("End [GENERAL INFO]")-1))
      end_of_header = true;
  }
}

void dbsk2d_xshock_graph_fileio::load_xshock_node_description()
{
  int node_id, adj_node_id, edge_id;
  unsigned char node_type, b_IO, dummy;
  bool end_of_node_description = false;

  while (!end_of_node_description)
  {
    //read next line
    fp_in.getline(buffer,2000);

    if (!vcl_strncmp(buffer, "Begin [NODE DESCRIPTION]", sizeof("Begin [NODE DESCRIPTION]")-1)){
      //read next line
      fp_in.getline(buffer,2000); //this is the comment line 

      //the node info should be next
      //read the node infos
      for (int i=0; i< num_nodes; i++)
      {
        //we don't know how many nodes this node is connected to
        //so we can't use readline here
        fp_in >> node_id;
        fp_in >> node_type;
        fp_in >> b_IO;
        
        //read in the nodes adjacency information
        vcl_vector<int> adjacency_list;
        fp_in >> dummy;  // "["
        fp_in >> adj_node_id;
        while (!fp_in.fail()) //read in all the adj node ids until the "]" is reached
        { 
          adjacency_list.push_back(adj_node_id);
          fp_in >> adj_node_id;
        }
        fp_in.clear(); //clear the fail bit

        fp_in >> dummy;  // "]"
        fp_in >> dummy;  // "["

        vcl_vector<int> edge_list;
        fp_in >> edge_id;
        while (!fp_in.fail()) //read in all the adj node ids until the "]" is reached
        { 
          edge_list.push_back(edge_id);
          fp_in >> edge_id;
        }
        fp_in.clear(); //clear the fail bit

        //: prepare the pair vector
        vcl_vector<vcl_pair<int, int> > adjacency_edge_list;
        assert(adjacency_list.size() == edge_list.size());

        for (unsigned ii = 0; ii < adjacency_list.size(); ii++) {
          vcl_pair<int, int> p(adjacency_list[ii], edge_list[ii]); 
          adjacency_edge_list.push_back(p);
        }

        //node_adjacency_map.insert(vcl_pair<int, vcl_vector<int> >(node_id, adjacency_list));
        node_adjacency_map.insert(vcl_pair<int, vcl_vector<vcl_pair<int, int> > >(node_id, adjacency_edge_list));

        //read in the rest of the line so that we can start at the 
        //beginning of the new line on the next iteration
        fp_in.getline(buffer, 2000);

        //create this node and add it to the graph
        dbsk2d_shock_node_sptr new_node = new dbsk2d_shock_node();
        new_node->set_id(node_id);

        //don't have to set the node type (this is implicit)
        //don't know how to set the I/O information yet
        shock->add_vertex(new_node);

        //also add this shock node to the nodes map so that it can be used later to instantiate edges
        nodes_map.insert(vcl_pair<int, dbsk2d_shock_node_sptr>(node_id, new_node));
      }
    }

    else if (!vcl_strncmp(buffer, "End [NODE DESCRIPTION]", sizeof("End [NODE DESCRIPTION]")-1))
      end_of_node_description = true;
  }
}

void dbsk2d_xshock_graph_fileio::load_xshock_node_samples()
{
  bool end_of_node_samples = false;

  while (!end_of_node_samples)
  {
    //read next line
    fp_in.getline(buffer,2000);

    if (!vcl_strncmp(buffer, "Begin [NODE SAMPLE POINTS]", sizeof("Begin [NODE SAMPLE POINTS]")-1))
      end_of_node_samples = false; //do nothing

    //I didn't bother to count how many node samples we expect
    //so just read all the samples until the end of the block is reached
    else if (!vcl_strncmp(buffer, "Begin SAMPLE", sizeof("Begin SAMPLE")-1))
    {
      dbsk2d_xshock_sample_sptr new_sample = load_xshock_sample();
      samples_map.insert(vcl_pair<int, dbsk2d_xshock_sample_sptr>(new_sample->id, new_sample));
    }

    else if (!vcl_strncmp(buffer, "End [NODE SAMPLE POINTS]", sizeof("End [NODE SAMPLE POINTS]")-1))
      end_of_node_samples = true;
  }
}

dbsk2d_xshock_sample_sptr 
dbsk2d_xshock_graph_fileio::load_xshock_sample()
{
  int sample_id, edge_id;
  char label[25];
  unsigned char sample_type;
  float x, y, t, theta, speed, lbndx, lbndy, rbndx, rbndy, ltheta, rtheta;

  bool end_of_sample = false;

  //the first line has already been read
  while (!end_of_sample)
  {
    //read the next line
    fp_in.getline(buffer,2000);

    //read the sample_id
    if (!vcl_strncmp(buffer, "sample_id", sizeof("sample_id")-1))
      sscanf(buffer,"sample_id %d", &(sample_id));

    //read the extrinsic coordinates
    else if (!vcl_strncmp(buffer, "(x, y, t)", sizeof("(x, y, t)")-1))
      sscanf(buffer,"(x, y, t) (%f, %f, %f)", &(x), &(y), &(t));

    //read the edge_id
    else if (!vcl_strncmp(buffer, "edge_id", sizeof("edge_id")-1))
      sscanf(buffer,"edge_id %d", &(edge_id));

    //read the label
    else if (!vcl_strncmp(buffer, "label", sizeof("label")-1))
      sscanf(buffer,"label %s", label);

    //read the sample_type
    else if (!vcl_strncmp(buffer, "type", sizeof("type")-1))
      sscanf(buffer,"type %c", &(sample_type));

    //read the theta parameter
    else if (!vcl_strncmp(buffer, "theta", sizeof("theta")-1))
      sscanf(buffer,"theta %f", &(theta));

    //read the speed parameter
    else if (!vcl_strncmp(buffer, "speed", sizeof("speed")-1))
      sscanf(buffer,"speed %f", &(speed));

    //read the boundary points
    else if (!vcl_strncmp(buffer, "boundaryPoints", sizeof("boundaryPoints")-1))
      sscanf(buffer,"boundaryPoints [(%f, %f), (%f, %f)]", 
      &(lbndx), &(lbndy), &(rbndx), &(rbndy));

    //read the boundary tangents
    else if (!vcl_strncmp(buffer, "boundaryTangents", sizeof("boundaryTangents")-1))
      sscanf(buffer,"boundaryTangents [%f, %f]", &(ltheta), &(rtheta));

    //end of sample marker
    else if (!vcl_strncmp(buffer, "End SAMPLE", sizeof("End SAMPLE")-1))
      end_of_sample = true;

  }

  //instantiate a sample 
  dbsk2d_xshock_sample_sptr new_sample = new dbsk2d_xshock_sample(sample_id);
  new_sample->pt = vgl_point_2d<double>(x,y);
  new_sample->radius = t;
  new_sample->edge_id = edge_id;
  new_sample->label = dbsk2d_xshock_sample::REGULAR; //FIXME
  new_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE; //FIXME
  new_sample->theta = theta*vnl_math::pi/180;
  new_sample->speed = speed;
  new_sample->left_bnd_pt = vgl_point_2d<double>(lbndx, lbndy);
  new_sample->right_bnd_pt = vgl_point_2d<double>(rbndx, rbndy);
  new_sample->left_bnd_tangent = ltheta;
  new_sample->right_bnd_tangent = rtheta;

  return new_sample;
}

void dbsk2d_xshock_graph_fileio::load_xshock_edge_description()
{
  int edge_id, src_node_id, tgt_node_id, sample_id;
  unsigned char b_IO, dummy;
  bool end_of_edge_description = false;

  while (!end_of_edge_description)
  {
    //read next line
    fp_in.getline(buffer,2000);

    if (!vcl_strncmp(buffer, "Begin [EDGE DESCRIPTION]", sizeof("Begin [EDGE DESCRIPTION]")-1)){
      //read next line
      fp_in.getline(buffer,2000); //this is the comment line 

      //the edge info should be next
      //read the edge infos
      for (int i=0; i< num_edges; i++)
      {
        vcl_vector<int> samples_list;
        samples_list.clear();

        //we don't know how many sample points we expect per edge so we can't use readline here
        fp_in >> edge_id;
        fp_in >> b_IO;
        fp_in >> dummy;
        fp_in >> src_node_id;
        fp_in >> tgt_node_id;
        fp_in >> dummy;
        fp_in >> dummy;

        fp_in >> sample_id;
        while (!fp_in.fail()){ //read in all the sample ids until the end bracket is reached
          samples_list.push_back(sample_id);
          fp_in >> sample_id;
        }
        fp_in.clear(); //clear the fail bit

        //read in the line end character so that we can start at the 
        //beginning of the new line on the next iteration
        fp_in.getline(buffer, 20);
        
        //find the node objects from their ids from the nodes map
        dbsk2d_shock_node_sptr src_node = nodes_map.find(src_node_id)->second;
        dbsk2d_shock_node_sptr tgt_node = nodes_map.find(tgt_node_id)->second;

        //create this edge and add it to the graph
        dbsk2d_shock_edge_sptr new_edge = new dbsk2d_xshock_edge(edge_id, src_node, tgt_node);
        //don't know how to set the I/O information yet

        //add this edge to the shock graph
        shock->add_edge(new_edge);

        int first_sample_id = samples_list[0];
        int last_sample_id = samples_list[samples_list.size()-1];

        //add this edge to the edge map so that edge adjacency can be properly set later
        vcl_pair<int, int> pp1(tgt_node_id, first_sample_id);
        vcl_pair<int, vcl_pair<int, int> > p1(src_node_id, pp1);

        edges_map.insert(vcl_pair<vcl_pair<int, vcl_pair<int, int> >, dbsk2d_shock_edge_sptr>(p1, new_edge));

        vcl_pair<int, int> pp2(src_node_id, last_sample_id);
        vcl_pair<int, vcl_pair<int, int> > p2(tgt_node_id, pp2);

        edges_map.insert(vcl_pair<vcl_pair<int, vcl_pair<int, int> >, dbsk2d_shock_edge_sptr>(p2, new_edge));

        //also add the information to the edge_samples map so that the edge samples 
        //can later be assigned to the edge after reading all the edge samples from the file
        edge_samples_map.insert(vcl_pair<dbsk2d_shock_edge_sptr, vcl_vector<int> >(new_edge, samples_list));
      }
    }

    else if (!vcl_strncmp(buffer, "End [EDGE DESCRIPTION]", sizeof("End [EDGE DESCRIPTION]")-1))
      end_of_edge_description = true;

  }
}

void dbsk2d_xshock_graph_fileio::load_xshock_edge_samples()
{
  bool end_of_edge_samples = false;

  while (!end_of_edge_samples)
  {
    //read next line
    fp_in.getline(buffer,2000);

    if (!vcl_strncmp(buffer, "Begin [EDGE SAMPLE POINTS]", sizeof("Begin [EDGE SAMPLE POINTS]")-1))
      end_of_edge_samples = false; //do nothing

    //I didn't bother to count how many edge samples we expect
    //so just read all the samples until the end of the block is reached
    else if (!vcl_strncmp(buffer, "Begin SAMPLE", sizeof("Begin SAMPLE")-1))
    {
      dbsk2d_xshock_sample_sptr new_sample = load_xshock_sample();
      samples_map.insert(vcl_pair<int, dbsk2d_xshock_sample_sptr >(new_sample->id, new_sample));
    }

    else if (!vcl_strncmp(buffer, "End [EDGE SAMPLE POINTS]", sizeof("End [EDGE SAMPLE POINTS]")-1))
      end_of_edge_samples = true;
  }
}

void dbsk2d_xshock_graph_fileio::assign_edge_samples()
{
  //this function assigns the edges with the list of extrinsic samples
  //after having read it from the file

  //go over all the edges and set the sample lists
  dbsk2d_shock_graph::edge_iterator e_it = shock->edges_begin();
  for ( ; e_it != shock->edges_end(); ++e_it){
    dbsk2d_shock_edge_sptr cur_edge = (*e_it);
    
    //get the sample id list from the stored map
    vcl_vector<int> sample_ids_list = edge_samples_map.find(cur_edge)->second;

    for (unsigned int i=0; i<sample_ids_list.size(); i++)
      ((dbsk2d_xshock_edge*)cur_edge.ptr())->push_back(samples_map.find(sample_ids_list[i])->second);

    //set the extrinsic points for drawing purposes(why not do it now?)
    cur_edge->compute_extrinsic_locus();
    cur_edge->source()->ex_pts().push_back(cur_edge->ex_pts().front());
    cur_edge->target()->ex_pts().push_back(cur_edge->ex_pts().back());
    //This is not the best place to do this (FIX ME!!!)
    cur_edge->form_shock_fragment();
  }
}

void dbsk2d_xshock_graph_fileio::setup_connectivity_between_nodes_and_edges()
{
  //set up the connectivity from the nodes to the edges respecting their original order

  //go over all the nodes and add the appropriate edges to the incoming and outgoing list
  dbsk2d_shock_graph::vertex_iterator v_it = shock->vertices_begin();
  for ( ; v_it != shock->vertices_end(); ++v_it)
  {
    dbsk2d_shock_node_sptr cur_node = (*v_it);

    //get the adjacency list of this node
    //vcl_vector<int> adjacent_nodes_list = node_adjacency_map.find(cur_node->id())->second;
    vcl_vector<vcl_pair<int, int> > adjacent_nodes_list = node_adjacency_map.find(cur_node->id())->second;

    //go over this list and find the corresponding edges
    for (unsigned int i=0; i<adjacent_nodes_list.size(); i++)
    {
      dbsk2d_shock_edge_sptr connected_edge = 
        edges_map.find(vcl_pair<int, vcl_pair<int, int> >(cur_node->id(), adjacent_nodes_list[i]))->second;

      if (cur_node == connected_edge->source())
        cur_node->add_outgoing_edge(connected_edge);
      else
        cur_node->add_incoming_edge(connected_edge);
    }
    //ozge added
    cur_node->form_shock_fragments();
  }
}

void dbsk2d_xshock_graph_fileio::finish_load()
{
  //The shock graph is finally complete
  //no need to keep the maps any more (in fact, keeping them is risky because 
  //these smart pointers might persist for a long time)
  samples_map.clear(); 
  nodes_map.clear();
  node_adjacency_map.clear();
  edges_map.clear();
  edge_samples_map.clear();
}

//-----------------------------------------------------------
// Save as .esf file
//-----------------------------------------------------------

bool dbsk2d_xshock_graph_fileio::save_xshock_graph(dbsk2d_shock_graph_sptr shock_graph, 
                                                   vcl_string filename)
{
  shock = shock_graph;

  // 1. open the esf file
  fp_out.open(filename.c_str());
  if (!fp_out.is_open()){
    vcl_cout<<" : Unable to Open "<<filename<<vcl_endl;
    return false;
  }

  fp_out.precision(5);

  //2. write out the info to file
  write_xshock_header();
  write_xshock_node_description();
  write_xshock_node_samples();
  write_xshock_edge_description();
  write_xshock_edge_samples();

  //3. close file
  fp_out.close();

  return true;
}

void dbsk2d_xshock_graph_fileio::write_xshock_header()
{
  fp_out <<"Extrinsic Shock File v1.0"<<vcl_endl<<vcl_endl;

  fp_out <<"Begin [GENERAL INFO]"<<vcl_endl<<vcl_endl;
  fp_out <<"Other parameters used: Default"<<vcl_endl;
  fp_out <<"Date of Creation: "<<vcl_endl<<vcl_endl;

  fp_out <<"Number of Nodes: "<< shock->number_of_vertices() <<vcl_endl;
  fp_out <<"Number of Edges: "<< shock->number_of_edges() <<vcl_endl;
  fp_out <<"Number of Sample Points: "<< vcl_endl<<vcl_endl;

  fp_out <<"End [GENERAL INFO]"<<vcl_endl<<vcl_endl;

}

void dbsk2d_xshock_graph_fileio::write_xshock_node_description()
{
  fp_out <<"Begin [NODE DESCRIPTION]"<<vcl_endl;
  fp_out <<"# node_ID  node_type Inside/Outside [CW linked node_IDs] [corresponding sample points]"<<vcl_endl;

  dbsk2d_shock_graph::vertex_iterator v_it = shock->vertices_begin();
  for (; v_it != shock->vertices_end(); ++v_it )
  {
    dbsk2d_shock_node_sptr cur_node = (*v_it);

    //node_ID
    fp_out <<cur_node->id()<<" ";
    //node_type
    switch (cur_node->type())
    {
      case dbsk2d_shock_node::A3:        fp_out <<"A "; break;
      case dbsk2d_shock_node::SOURCE:    fp_out <<"S "; break;
      case dbsk2d_shock_node::SINK:      fp_out <<"F "; break;
      case dbsk2d_shock_node::JUNCT:     fp_out <<"J "; break;
      case dbsk2d_shock_node::TERMINAL:  fp_out <<"T "; break;
    }

    //inside/outside
    fp_out <<"I "; //inside  TEMPORARY!!!!

    //if (cur_node->is_inside_shock())      fp_out <<"I "; //inside
    //else                                  fp_out <<"O "; //outside

    //write out the list of adjacent nodes
    fp_out <<"[";
    //incoming edges first
    
    for (dbsk2d_shock_node::edge_iterator e_it = cur_node->in_edges_begin();
         e_it != cur_node->in_edges_end(); ++e_it )
    {
      dbsk2d_shock_node_sptr adj_node = (*e_it)->opposite(cur_node);
      fp_out << adj_node->id() <<" ";
    }
    //then out going edges
    for (dbsk2d_shock_node::edge_iterator e_it = cur_node->out_edges_begin(); 
         e_it != cur_node->out_edges_end(); ++e_it )
    {
      dbsk2d_shock_node_sptr adj_node = (*e_it)->opposite(cur_node);
      fp_out << adj_node->id() <<" ";
    }
    fp_out <<"] ";

    //write out the sample ids corresponding to the adjacent edges in the same order as before
    fp_out <<"[";
    //incoming edges first
    for (dbsk2d_shock_node::edge_iterator e_it = cur_node->in_edges_begin(); 
         e_it != cur_node->in_edges_end(); ++e_it )
    {
      dbsk2d_xshock_edge* adj_in_edge = (dbsk2d_xshock_edge*)((*e_it).ptr());
      fp_out << adj_in_edge->last_sample()->id <<" ";
    }
    //then out going edges
    for (dbsk2d_shock_node::edge_iterator e_it = cur_node->out_edges_begin(); 
         e_it != cur_node->out_edges_end(); ++e_it )
    {
      dbsk2d_xshock_edge* adj_out_edge = (dbsk2d_xshock_edge*)((*e_it).ptr());
      fp_out << adj_out_edge->first_sample()->id <<" ";
    }
    fp_out <<"] "<<vcl_endl;
  }

  fp_out <<"End [NODE DESCRIPTION]"<<vcl_endl<<vcl_endl;
}

void dbsk2d_xshock_graph_fileio::write_xshock_sample (dbsk2d_xshock_sample_sptr sample)
{
  fp_out <<"Begin SAMPLE"<<vcl_endl;
  fp_out <<"sample_id "<< sample->id<<vcl_endl;
  fp_out <<"(x, y, t) ("<< sample->pt.x() <<", " << sample->pt.y() <<", " << sample->radius <<")"<<vcl_endl;
  fp_out <<"edge_id "<< sample->edge_id <<vcl_endl;

  if (sample->label==dbsk2d_xshock_sample::REGULAR)               fp_out <<"label regular";
  else if (sample->label==dbsk2d_xshock_sample::SEMI_DEGENERATE)  fp_out <<"label regular";
  else if (sample->label==dbsk2d_xshock_sample::DEGENERATE)       fp_out <<"label degenerate";
  fp_out <<vcl_endl;

  if (sample->type == dbsk2d_xshock_sample::NORMALSAMPLE)         fp_out <<"type "<< "N"<<vcl_endl;
  else if (sample->type == dbsk2d_xshock_sample::PRUNEDSAMPLE)    fp_out <<"type "<< "P"<<vcl_endl;
  else                                                            fp_out <<"type "<< "C"<<vcl_endl;

  fp_out <<"theta "<< sample->theta*180/vnl_math::pi<<vcl_endl;
  fp_out <<"speed "<< sample->speed<<vcl_endl;
  // ozge changed to "boundaryPoints" from "boundarypoints" to be competible with older esf files
  fp_out <<"boundaryPoints [(" << sample->left_bnd_pt.x() <<", "<< sample->left_bnd_pt.y()<<")";
  fp_out <<", (" << sample->right_bnd_pt.x()<<", " << sample->right_bnd_pt.y()<<")]"<<vcl_endl;
  fp_out <<"boundaryTangents ["<< sample->left_bnd_tangent <<", "<< sample->right_bnd_tangent <<"]"<<vcl_endl;
  fp_out <<"End SAMPLE"<<vcl_endl<<vcl_endl;
}

void dbsk2d_xshock_graph_fileio::write_xshock_node_samples()
{
  fp_out <<"Begin [NODE SAMPLE POINTS]"<<vcl_endl<<vcl_endl;

  dbsk2d_shock_graph::vertex_iterator v_it = shock->vertices_begin();
  for (; v_it!=shock->vertices_end(); ++v_it )
  {
    dbsk2d_shock_node_sptr cur_node = *(v_it);

    //incoming edges first
    dbsk2d_shock_node::edge_iterator e_it = cur_node->in_edges_begin();
    for (; e_it != cur_node->in_edges_end(); ++e_it )
    {
      dbsk2d_xshock_edge* adj_in_edge = (dbsk2d_xshock_edge*)((*e_it).ptr());
      write_xshock_sample(adj_in_edge->last_sample());
    }
    //then out going edges
    e_it = cur_node->out_edges_begin();
    for (; e_it != cur_node->out_edges_end(); ++e_it )
    {
      dbsk2d_xshock_edge* adj_out_edge = (dbsk2d_xshock_edge*)((*e_it).ptr());
      write_xshock_sample(adj_out_edge->first_sample());
    }
  }
  fp_out <<"End [NODE SAMPLE POINTS]"<<vcl_endl<<vcl_endl;
}

void dbsk2d_xshock_graph_fileio::write_xshock_edge_description()
{
  fp_out <<"Begin [EDGE DESCRIPTION]"<<vcl_endl;
  fp_out <<"# Edge_ID Inside/Outside [From_NODE To_NODE] [IDs of the sample points]"<<vcl_endl;

  dbsk2d_shock_graph::edge_iterator e_it = shock->edges_begin();
  for (; e_it!=shock->edges_end(); ++e_it )
  {
    dbsk2d_xshock_edge* cur_edge = (dbsk2d_xshock_edge*)((*e_it).ptr());

    // edge id
    fp_out <<cur_edge->id()<<" ";

    //inside /outside
    if (cur_edge->is_inside_shock())  fp_out <<"I ";
    else                              fp_out <<"O ";

    //from node - to node
    fp_out <<"[";
    fp_out << cur_edge->source()->id()<<" ";
    fp_out << cur_edge->target()->id();
    fp_out <<"] ";

    //samples including node samples
    fp_out <<"[";
    for (int j=0; j<cur_edge->num_samples(); j++){
      dbsk2d_xshock_sample_sptr cur_sample = cur_edge->sample(j);
      fp_out << cur_sample->id <<" ";
    }
    fp_out <<"]"<<vcl_endl;
  }

  fp_out <<"End [EDGE DESCRIPTION]"<<vcl_endl<<vcl_endl;
}

void dbsk2d_xshock_graph_fileio::write_xshock_edge_samples()
{
  fp_out <<"Begin [EDGE SAMPLE POINTS]"<<vcl_endl<<vcl_endl;

  dbsk2d_shock_graph::edge_iterator e_it = shock->edges_begin();
  for (; e_it!=shock->edges_end(); ++e_it )
  {
    dbsk2d_xshock_edge* cur_edge = (dbsk2d_xshock_edge*)((*e_it).ptr());

    //No need to write the first and last sample of each edge...
    for (int j=1; j<cur_edge->num_samples()-1; j++){
      dbsk2d_xshock_sample_sptr cur_sample = cur_edge->sample(j);
      write_xshock_sample (cur_sample);
    }
  }
  fp_out <<"End [EDGE SAMPLE POINTS]"<<vcl_endl<<vcl_endl;
}





