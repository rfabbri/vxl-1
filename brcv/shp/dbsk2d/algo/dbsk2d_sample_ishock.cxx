// This is brcv/shp/dbsk2d/algo/dbsk2d_sample_ishock.cxx

//:
// \file

#include <dbsk2d/algo/dbsk2d_sample_ishock.h>

#include <dbsk2d/dbsk2d_ishock_graph.h>

#include <dbsk2d/dbsk2d_shock_node.h>
#include <dbsk2d/dbsk2d_shock_edge.h>
#include <dbsk2d/dbsk2d_shock_graph.h>

#include <dbsk2d/dbsk2d_shock_grouped_ishock_edge.h>

#include <dbsk2d/dbsk2d_xshock_sample.h>
#include <dbsk2d/dbsk2d_xshock_node.h>
#include <dbsk2d/dbsk2d_xshock_edge.h>

//: Constructor
dbsk2d_sample_ishock::dbsk2d_sample_ishock(dbsk2d_shock_graph_sptr coarse_shock_graph) :
  shock_graph(coarse_shock_graph), xshock_graph(0), next_available_sample_id(1)
{
  nodes_map.clear();
  edges_map.clear();
  cc_label_map.clear();
  cc_elms.clear();

  largest_component_id=0;
}

//: Destructor
dbsk2d_sample_ishock::~dbsk2d_sample_ishock() 
{
  nodes_map.clear();
  edges_map.clear();
  cc_label_map.clear();
  cc_elms.clear();
}

//: sample the coarse shock graph
void dbsk2d_sample_ishock::sample(double resolution, int option)
{
  delta_sample = resolution;

  //1) Mark inside/outside 
  if (option==INSIDE)
    mark_inside_outside();

  //2) Instantiate new shock graph
  xshock_graph = new dbsk2d_shock_graph();

  //3) Sample all nodes
  sample_all_nodes(option);

  //4) sample all the edges
  sample_all_edges(option);

  //5) Add the edge adjacency info to the nodes (respecting the ordering)
  add_edge_adjacency_info(option);

}

//: mark the shocks as inside and outside
void dbsk2d_sample_ishock::mark_inside_outside()
{
  //1) Initialize labels and connected components
  dbsk2d_shock_graph::vertex_iterator curN = shock_graph->vertices_begin();
  for (; curN != shock_graph->vertices_end(); curN++)
  {
    dbsk2d_shock_node_sptr cur_node = (*curN);

    //create a new component with it and its children
    cc_label_map.insert(vcl_pair<int, int>(cur_node->id(), cur_node->id()));
    
    vcl_vector<int> components;
    components.push_back(cur_node->id());

    for (dbsk2d_shock_node::edge_iterator e_it = cur_node->out_edges_begin();
          e_it != cur_node->out_edges_end(); e_it++)
    {
      dbsk2d_shock_edge_sptr ch_edge = (*e_it);

      cc_label_map.insert(vcl_pair<int, int>(ch_edge->id(), cur_node->id()));
      components.push_back(ch_edge->id());
    }

    //save this component
    cc_elms.insert(vcl_pair<int, vcl_vector<int> >(cur_node->id(), components));
  }

  ////FOR DEBUG
  //vcl_cout << "Components list before connecting: \n";
  //for (vcl_map<int, vcl_vector<int> >::iterator it = cc_elms.begin();
  //     it != cc_elms.end(); it++)
  //{
  //  vcl_cout << it->first << ": " << it->second.size() << vcl_endl;
  //}

  //2) Traverse over all junctions and sinks and relabel
  //   the components connected to them
  curN = shock_graph->vertices_begin();
  for (; curN != shock_graph->vertices_end(); curN++)
  {
    dbsk2d_shock_node_sptr cur_node = (*curN);

    if (cur_node->is_a_sink() || cur_node->is_a_junction())
    {
      //first find the smallest component label
      int smallest_comp_id = cc_label_map[cur_node->id()];

      for (dbsk2d_shock_node::edge_iterator e_it = cur_node->in_edges_begin();
           e_it != cur_node->in_edges_end(); e_it++)
      {
        dbsk2d_shock_edge_sptr ch_edge = (*e_it);
        int cur_label = cc_label_map[ch_edge->id()];

        if (cur_label<smallest_comp_id)
          smallest_comp_id = cur_label;
      }

      //now set the smallest label on all the components at this node
      //first itself 
      int cur_label = cc_label_map[cur_node->id()];
      if (cur_label != smallest_comp_id)
      {
        //merge this component with the smallest id component
        for (unsigned i=0; i<cc_elms[cur_label].size(); i++){
          int item_id = cc_elms[cur_label][i];
          //relabel this item
          cc_label_map[item_id] = smallest_comp_id;
          //and merge the components
          cc_elms[smallest_comp_id].push_back(item_id);
        }
        //remove this component from the components list
        cc_elms.erase(cur_label);
      }
      //then all the incoming components
      for (dbsk2d_shock_node::edge_iterator e_it = cur_node->in_edges_begin();
           e_it != cur_node->in_edges_end(); e_it++)
      {
        dbsk2d_shock_edge_sptr ch_edge = (*e_it);
        int cur_label = cc_label_map[ch_edge->id()];

        if (cur_label != smallest_comp_id)
        {
          //merge this component with the smallest id component
          for (unsigned i=0; i<cc_elms[cur_label].size(); i++){
            int item_id = cc_elms[cur_label][i];
            //relabel this item
            cc_label_map[item_id] = smallest_comp_id;
            //and merge the components
            cc_elms[smallest_comp_id].push_back(item_id);
          }
          //remove this component from the components list
          cc_elms.erase(cur_label);
        }
      }
    }
  }

  ////FOR DEBUG
  //vcl_cout << "Components list: \n";
  //for (vcl_map<int, vcl_vector<int> >::iterator it = cc_elms.begin();
  //     it != cc_elms.end(); it++)
  //{
  //  vcl_cout << it->first << ": " << it->second.size() << vcl_endl;
  //}

  //3) Go over all the components and remove all the components
  //   that go to the edge of the boundary
  curN = shock_graph->vertices_begin();
  for (; curN != shock_graph->vertices_end(); curN++)
  {
    dbsk2d_shock_node_sptr cur_node = (*curN);
    if (cur_node->degree()==0) //remove this node's component
      cc_elms[cc_label_map[cur_node->id()]].clear();
  }

  //now the edges
  dbsk2d_shock_graph::edge_iterator curE = shock_graph->edges_begin();
  for (; curE != shock_graph->edges_end(); curE++)
  {
    dbsk2d_shock_edge_sptr cur_edge = (*curE);
    if (!cur_edge->target()){ //remove this edge's component
      cc_elms[cc_label_map[cur_edge->id()]].clear();
      //vcl_cout << "E:"<<cur_edge->id() << "goes to inf.\n";
    }
  }

  //4) label the largest remaining group as the inside shock graph
  unsigned cur_largest_size = 0;
  for (vcl_map<int, vcl_vector<int> >::iterator it = cc_elms.begin();
       it != cc_elms.end(); it++)
  {
    int component_id = it->first;
    if (it->second.size()>cur_largest_size){
      cur_largest_size = it->second.size();
      largest_component_id = component_id;
    }
  }
}

void dbsk2d_sample_ishock::sample_all_nodes(int option)
{
  //go through all the nodes of the coarse shock graph and duplicate them 
  dbsk2d_shock_graph::vertex_iterator curN = shock_graph->vertices_begin();
  for (; curN != shock_graph->vertices_end(); curN++)
  {
    dbsk2d_shock_node_sptr cur_node = (*curN);

    // if only the inside shock graph is to be sampled,
    // make sure this node belongs to the largest shock component
    if (option==INSIDE)
      if (cc_label_map[cur_node->id()] != largest_component_id)
        continue;

    //instantiate new extrinsic shock node
    dbsk2d_shock_node_sptr new_xnode = new dbsk2d_xshock_node(cur_node->id());

    //compute extrinsic points for display purposes
    new_xnode->ex_pts().push_back(cur_node->ex_pts().front());

    //set the intrinsic properties at this node
    new_xnode->set_pt(cur_node->pt());
    new_xnode->set_radius(cur_node->radius());

    //insert it into the extrinsic shock graph
    xshock_graph->add_vertex(new_xnode->cast_to_shock_node());

    //insert it into the nodes map
    nodes_map.insert(vcl_pair<int, dbsk2d_shock_node_sptr>(cur_node->id(), new_xnode));

    //sample this node if degenerate
    sample_shock_node(cur_node, (dbsk2d_xshock_node*)new_xnode.ptr());
  }
}

//: Sample each shock node
void dbsk2d_sample_ishock::sample_shock_node(dbsk2d_shock_node_sptr snode, 
                                             dbsk2d_xshock_node* new_xnode)
{
  // sample any (virtual) A-infinity edges on this node
  vcl_list<dbsk2d_shock_node_descriptor>::iterator p_itr = snode->descriptor_list().begin();
  for (; p_itr != snode->descriptor_list().end(); ++ p_itr)
  {
    dbsk2d_shock_node_descriptor cur_descriptor = (*p_itr);

    if (!cur_descriptor.edge) //signals A-infinity (need to sample this)
    {
      //for this node phi=0 for all the samples
      // only the tangents are sweeping the range between the two tangents (CCW)
      double cur_tan = cur_descriptor.tangent;
      double end_tan = cur_descriptor.tangent2;
      
      if (end_tan < cur_tan)
        end_tan += 2*vnl_math::pi;

      while (cur_tan < end_tan){
        dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

        cur_sample->pt = snode->pt();
        cur_sample->radius = snode->radius();
        cur_sample->edge_id = -1; //FIX ME! (this is not and edge)
        cur_sample->label = dbsk2d_xshock_sample::DEGENERATE;
        cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
        cur_sample->theta = angle0To2Pi(cur_tan);
        cur_sample->speed = 1;
        cur_sample->left_bnd_pt = _translatePoint(snode->pt(), cur_tan, snode->radius());
        cur_sample->right_bnd_pt = snode->pt(); //actually this only sweeps one side of the sample point
        cur_sample->left_bnd_tangent = 0; //xx
        cur_sample->right_bnd_tangent = 0; //xx

        //add to referenced list to store with the edge
        new_xnode->push_back(cur_sample); 

        //increment the tangent angle (going CCW)
        cur_tan += 0.02;
      }
    }
  }
}

void dbsk2d_sample_ishock::sample_all_edges(int option)
{
  //go through all the shock edges of the coarse shock graph and sample them
  dbsk2d_shock_graph::edge_iterator curE = shock_graph->edges_begin();
  for (; curE != shock_graph->edges_end(); curE++)
  {
    dbsk2d_shock_grouped_ishock_edge* cur_edge = (dbsk2d_shock_grouped_ishock_edge*)curE->ptr();

    // if only the inside shock graph is to be sampled,
    // make sure this node belongs to the largest shock component
    if (option==INSIDE)
      if (cc_label_map[cur_edge->id()] != largest_component_id)
        continue;

    //1) create the extrinsic edge
    dbsk2d_shock_node_sptr src_xnode = nodes_map.find(cur_edge->source()->id())->second;
    dbsk2d_shock_node_sptr tgt_xnode = 0;
    if (cur_edge->target()) //some edges go to infinity
      tgt_xnode = nodes_map.find(cur_edge->target()->id())->second;
    dbsk2d_shock_edge_sptr new_xedge = new dbsk2d_xshock_edge(cur_edge->id(), src_xnode, tgt_xnode);

    //add the shock edge to the extrinsic shock graph
    xshock_graph->add_edge(new_xedge);

    //insert it into the edges map
    edges_map.insert(vcl_pair<int, dbsk2d_shock_edge_sptr>(cur_edge->id(), new_xedge));

    //For now, don't sample shock edges that go to infinity
    if (!tgt_xnode){
      continue;
    }

    //2) go through the list of intrinsic shock edges and sample them
    dbsk2d_ishock_edge* last_iedge=0;
    vcl_list<dbsk2d_ishock_edge*>::iterator curIE = cur_edge->edges().begin();
    for (; curIE != cur_edge->edges().end(); ++ curIE)
    {
      dbsk2d_ishock_edge* cur_iedge = (*curIE);

      //if there is a degenerate (A1-Ainf) node on the list, sample it as such
      if (cur_iedge == 0) 
      {
        unsigned int start=((dbsk2d_xshock_edge*)new_xedge.ptr())
            ->num_samples();  
        //sample the A1-Ainf node between the last edge and the next edge
        sample_A1_Ainf_node(last_iedge->cSNode(), (dbsk2d_xshock_edge*)new_xedge.ptr());
        unsigned int stop=((dbsk2d_xshock_edge*)new_xedge.ptr())
            ->num_samples();
        
        for ( unsigned int s=start; s < stop ; ++s)
        {
            ishock_sample_map_[last_iedge->id()].push_back(
                ((dbsk2d_xshock_edge*)new_xedge.ptr())->sample(s));
        }
        continue;
      }

      unsigned int start=((dbsk2d_xshock_edge*)new_xedge.ptr())->num_samples();

      //sample the regular intrinsic shock edges (A1^2) here
      switch (cur_iedge->type()){
        case dbsk2d_ishock_elm::POINTPOINT:
          sample_ishock_edge((dbsk2d_ishock_pointpoint*)cur_iedge, (dbsk2d_xshock_edge*)new_xedge.ptr());
          break;
        case dbsk2d_ishock_elm::POINTLINE:
          sample_ishock_edge((dbsk2d_ishock_pointline*)cur_iedge, (dbsk2d_xshock_edge*)new_xedge.ptr());
          break;
        case dbsk2d_ishock_elm::LINELINE:
          sample_ishock_edge((dbsk2d_ishock_lineline*)cur_iedge, (dbsk2d_xshock_edge*)new_xedge.ptr());
          break;
        case dbsk2d_ishock_elm::POINTARC:
          sample_ishock_edge((dbsk2d_ishock_pointarc*)cur_iedge, (dbsk2d_xshock_edge*)new_xedge.ptr());
          break;
        case dbsk2d_ishock_elm::LINEARC:
          sample_ishock_edge((dbsk2d_ishock_linearc*)cur_iedge, (dbsk2d_xshock_edge*)new_xedge.ptr());
          break;
        case dbsk2d_ishock_elm::ARCARC:
          sample_ishock_edge((dbsk2d_ishock_arcarc*)cur_iedge, (dbsk2d_xshock_edge*)new_xedge.ptr());
          break;
        case dbsk2d_ishock_elm::LINELINE_THIRDORDER:
          sample_ishock_edge((dbsk2d_ishock_lineline_thirdorder*)cur_iedge, (dbsk2d_xshock_edge*)new_xedge.ptr());
          break;
        case dbsk2d_ishock_elm::ARCARC_THIRDORDER:
          sample_ishock_edge((dbsk2d_ishock_arcarc_thirdorder*)cur_iedge, (dbsk2d_xshock_edge*)new_xedge.ptr());
          break;
        default: break;
      }

      unsigned int stop=((dbsk2d_xshock_edge*)new_xedge.ptr())->num_samples();
      for ( unsigned int s=start; s < stop ; ++s)
      {
          ishock_sample_map_[cur_iedge->id()].push_back(
              ((dbsk2d_xshock_edge*)new_xedge.ptr())->sample(s));
      }
      
      last_iedge = cur_iedge;
    }

    //compute extrinsic points for display purposes
    new_xedge->compute_extrinsic_locus();

    //This is not the best place to do this (FIX ME!!!)
    new_xedge->form_shock_fragment();
  }

  // Uncomment for debugging
  // vcl_ofstream file("actual_shock_sample.txt");
  // vcl_map<int,vcl_vector<dbsk2d_xshock_sample_sptr> >::iterator mit;
  // for ( mit = ishock_sample_map_.begin(); mit != ishock_sample_map_.end() ; ++mit)
  // {
  //     for ( unsigned int k=0; k < (*mit).second.size() ; ++k)
  //     {
  //         file<<(*mit).second[k]->pt.x()<<" "
  //             <<(*mit).second[k]->pt.y()<<vcl_endl;
  //     }
  // }
  // file.close();

}

//: Sample each shock branch
//1) Explicitly include the first and last points
//   using begin and end tau for every shock link
//2) Sample each vgl_point_2d<double> into dbsk2d_xshock_sample class at this stage
void dbsk2d_sample_ishock::sample_ishock_edge(dbsk2d_ishock_pointpoint* spp, dbsk2d_xshock_edge* new_xedge)
{
  double tau, stau, etau, next_tau;
  dbsk2d_ishock_edge::TAU_DIRECTION_TYPE tau_direction;

  tau_direction = spp->tauDir();
  tau = stau = spp->sTau();
  etau = spp->eTau();

  while ( (tau<etau && tau_direction==dbsk2d_ishock_edge::TAU_INCREASING) ||
          (tau>etau && tau_direction==dbsk2d_ishock_edge::TAU_DECREASING)) 
  {
    dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

    cur_sample->pt = spp->getPtFromLTau(tau);
    cur_sample->radius = spp->r(tau);
    cur_sample->edge_id = spp->edgeID();
    cur_sample->label = dbsk2d_xshock_sample::DEGENERATE;
    cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
    double angle = angle0To2Pi (spp->tangent(tau));
    cur_sample->theta = angle;
    cur_sample->speed = spp->v(tau);
    cur_sample->left_bnd_pt = spp->getLFootPt(tau);
    cur_sample->right_bnd_pt = spp->getRFootPt(tau);
    cur_sample->left_bnd_tangent = 0; //xx
    cur_sample->right_bnd_tangent = 0; //xx

    //add to referenced list to store with the edge
    new_xedge->push_back(cur_sample); 

    double ds = delta_sample;
    //increment tau to unit arclength on shock
    next_tau = vcl_atan(2*ds/spp->H() + vcl_tan(tau));

    //use this next_tau
    tau = next_tau;
  }

  dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());
  cur_sample->pt = spp->getPtFromLTau(etau);
  cur_sample->radius = spp->r(etau);
  cur_sample->edge_id = spp->edgeID();
  cur_sample->label = dbsk2d_xshock_sample::DEGENERATE;
  cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
  cur_sample->theta = angle0To2Pi(spp->tangent(etau));
  cur_sample->speed = spp->v(etau);
  cur_sample->left_bnd_pt = spp->getLFootPt(etau);
  cur_sample->right_bnd_pt = spp->getRFootPt(etau);
  cur_sample->left_bnd_tangent = 0; //xx
  cur_sample->right_bnd_tangent = 0; //xx
  
  //add to referenced list to store with the edge
  new_xedge->push_back(cur_sample);
}

void dbsk2d_sample_ishock::sample_ishock_edge(dbsk2d_ishock_pointline* spl, dbsk2d_xshock_edge* new_xedge)
{
  double tau, stau, etau, next_tau;
  dbsk2d_ishock_edge::TAU_DIRECTION_TYPE tau_direction;

  tau_direction = spl->tauDir();
  tau = stau = spl->sTau();
  etau = spl->eTau();

  while ((tau<etau && tau_direction==dbsk2d_ishock_edge::TAU_INCREASING) ||
         (tau>etau && tau_direction==dbsk2d_ishock_edge::TAU_DECREASING)) 
  {
    dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

    cur_sample->pt =  spl->getPtFromTau(tau);
    cur_sample->radius = spl->r(tau);
    cur_sample->edge_id = spl->edgeID();
    cur_sample->label = dbsk2d_xshock_sample::SEMI_DEGENERATE;
    cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
    double angle = angle0To2Pi (spl->tangent(tau));
    cur_sample->theta = angle;
    cur_sample->speed = spl->v(tau);
    cur_sample->left_bnd_pt = spl->getLFootPt(tau);
    cur_sample->right_bnd_pt = spl->getRFootPt(tau);
    cur_sample->left_bnd_tangent = 0; //xx
    cur_sample->right_bnd_tangent = 0; //xx

    //add to referenced list to store with the edge
    new_xedge->push_back(cur_sample);

    double ds = delta_sample;
    //increment tau to unit arclength on shock
    if (spl->nu()==1)
      next_tau = tau + ds/spl->r(tau);
    else
      next_tau = tau - ds/spl->r(tau);

    //use this next_tau
    tau = next_tau;
  }

  dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

  cur_sample->pt =  spl->getPtFromTau(etau);
  cur_sample->radius = spl->r(etau);
  cur_sample->edge_id = spl->edgeID();
  cur_sample->label = dbsk2d_xshock_sample::SEMI_DEGENERATE;
  cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
  cur_sample->theta = angle0To2Pi(spl->tangent(etau));
  cur_sample->speed = spl->v(etau);
  cur_sample->left_bnd_pt = spl->getLFootPt(etau);
  cur_sample->right_bnd_pt = spl->getRFootPt(etau);
  cur_sample->left_bnd_tangent = 0; //xx
  cur_sample->right_bnd_tangent = 0; //xx  

  //add to referenced list to store with the edge
  new_xedge->push_back(cur_sample);
}

void dbsk2d_sample_ishock::sample_ishock_edge(dbsk2d_ishock_lineline* sll, dbsk2d_xshock_edge* new_xedge)
{
  double tau, stau, etau;
  dbsk2d_xshock_sample_sptr first_sample = NULL;
  dbsk2d_ishock_edge::TAU_DIRECTION_TYPE tau_direction;

  tau_direction = sll->tauDir();
  tau = stau = sll->sTau();
  etau = sll->eTau();

  while ((tau<etau && tau_direction==dbsk2d_ishock_edge::TAU_INCREASING) ||
         (tau>etau && tau_direction==dbsk2d_ishock_edge::TAU_DECREASING)) 
  {
    dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

    cur_sample->pt =  sll->getPtFromLTau(tau);
    cur_sample->radius = sll->r(tau);
    cur_sample->edge_id = sll->edgeID();
    cur_sample->label = dbsk2d_xshock_sample::REGULAR;
    cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
    double angle = angle0To2Pi (sll->tangent(tau));
    cur_sample->theta = angle;
    cur_sample->speed = sll->v(tau);
    cur_sample->left_bnd_pt = sll->getLFootPt(tau);
    cur_sample->right_bnd_pt = sll->getRFootPt(tau);
    cur_sample->left_bnd_tangent = 0; //xx
    cur_sample->right_bnd_tangent = 0; //xx

    //add to referenced list to store with the edge
    new_xedge->push_back(cur_sample);

    double ds = delta_sample;
    //increment tau to unit arclength on shock
    double next_tau = tau + vcl_sqrt(ds*ds/(1+sll->N1L()*sll->N1L())); 

    //use this next_tau
    tau = next_tau;
  }

  dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

  cur_sample->pt =  sll->getPtFromLTau(etau);
  cur_sample->radius = sll->r(etau);
  cur_sample->edge_id = sll->edgeID();
  cur_sample->label = dbsk2d_xshock_sample::REGULAR;
  cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
  cur_sample->theta = angle0To2Pi(sll->tangent(etau));
  cur_sample->speed = sll->v(etau);
  cur_sample->left_bnd_pt = sll->getLFootPt(etau);
  cur_sample->right_bnd_pt = sll->getRFootPt(etau);
  cur_sample->left_bnd_tangent = 0; //xx
  cur_sample->right_bnd_tangent = 0; //xx

  //add to referenced list to store with the edge
  new_xedge->push_back(cur_sample);
}

void dbsk2d_sample_ishock::sample_ishock_edge(dbsk2d_ishock_lineline_thirdorder* sto, dbsk2d_xshock_edge* new_xedge)
{
  double tau, stau, etau;
  dbsk2d_xshock_sample_sptr first_sample = NULL;
  dbsk2d_ishock_edge::TAU_DIRECTION_TYPE tau_direction;

  tau_direction = sto->tauDir();
  tau = stau = sto->sTau();
  etau = sto->eTau();

  while ((tau<etau && tau_direction==dbsk2d_ishock_edge::TAU_INCREASING) ||
         (tau>etau && tau_direction==dbsk2d_ishock_edge::TAU_DECREASING)) 
  {
    dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

    cur_sample->pt =  sto->getPtFromLTau(tau);
    cur_sample->radius = sto->r(tau);
    cur_sample->edge_id = sto->edgeID();
    cur_sample->label = dbsk2d_xshock_sample::REGULAR;
    cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
    cur_sample->theta = sto->tangent(tau);
    cur_sample->speed = sto->v(tau);
    cur_sample->left_bnd_pt = sto->getLFootPt(tau);
    cur_sample->right_bnd_pt = sto->getRFootPt(tau);
    cur_sample->left_bnd_tangent = 0; //xx
    cur_sample->right_bnd_tangent = 0; //xx

    //add to referenced list to store with the edge
    new_xedge->push_back(cur_sample);

    double ds = delta_sample;
    //increment tau to unit arclength on shock
    double next_tau = tau - ds;

    //use this next_tau
    tau = next_tau;
  } 

  dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

  cur_sample->pt =  sto->getPtFromLTau(etau);
  cur_sample->radius = sto->r(etau);
  cur_sample->edge_id = sto->edgeID();
  cur_sample->label = dbsk2d_xshock_sample::REGULAR;
  cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
  cur_sample->theta = sto->tangent(etau);
  cur_sample->speed = sto->v(etau);
  cur_sample->left_bnd_pt = sto->getLFootPt(etau);
  cur_sample->right_bnd_pt = sto->getRFootPt(etau);
  cur_sample->left_bnd_tangent = 0; //xx
  cur_sample->right_bnd_tangent = 0; //xx

  //add to referenced list to store with the edge
  new_xedge->push_back(cur_sample);

}

void dbsk2d_sample_ishock::sample_ishock_edge(dbsk2d_ishock_pointarc* spa, dbsk2d_xshock_edge* new_xedge)
{
  double tau, stau, etau, next_tau;
  dbsk2d_ishock_edge::TAU_DIRECTION_TYPE tau_direction;

  tau_direction = spa->tauDir();
  tau = stau = spa->sTau();
  etau = spa->eTau();

  while ((tau<etau && tau_direction==dbsk2d_ishock_edge::TAU_INCREASING) ||
         (tau>etau && tau_direction==dbsk2d_ishock_edge::TAU_DECREASING)) 
  {
    dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

    cur_sample->pt =  spa->getPtFromTau(tau);
    cur_sample->radius = spa->r(tau);
    cur_sample->edge_id = spa->edgeID();
    cur_sample->label = dbsk2d_xshock_sample::DEGENERATE;
    cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
    cur_sample->theta = spa->tangent(tau);
    cur_sample->speed = spa->v(tau);
    cur_sample->left_bnd_pt = spa->getLFootPt(tau);
    cur_sample->right_bnd_pt = spa->getRFootPt(tau);
    cur_sample->left_bnd_tangent = 0; //xx
    cur_sample->right_bnd_tangent = 0; //xx

    //add to referenced list to store with the edge
    new_xedge->push_back(cur_sample);

    double ds = delta_sample;
    //increment tau to unit arclength on shock
    if (tau_direction==dbsk2d_ishock_edge::TAU_INCREASING)
      next_tau = tau + ds/spa->d(tau);
    else
      next_tau = tau - ds/spa->d(tau);

    //use this next_tau
    tau = next_tau;
  }

  dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

  cur_sample->pt =  spa->getPtFromTau(etau);
  cur_sample->radius = spa->r(etau);
  cur_sample->edge_id = spa->edgeID();
  cur_sample->label = dbsk2d_xshock_sample::DEGENERATE;
  cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
  cur_sample->theta = spa->tangent(etau);
  cur_sample->speed = spa->v(etau);//incomplete
  cur_sample->left_bnd_pt = spa->getLFootPt(etau);
  cur_sample->right_bnd_pt = spa->getRFootPt(etau);
  cur_sample->left_bnd_tangent = 0; //xx
  cur_sample->right_bnd_tangent = 0; //xx  

  //add to referenced list to store with the edge
  new_xedge->push_back(cur_sample);

}

void dbsk2d_sample_ishock::sample_ishock_edge(dbsk2d_ishock_linearc* sla, dbsk2d_xshock_edge* new_xedge)
{
  double tau, stau, etau, next_tau;
  dbsk2d_ishock_edge::TAU_DIRECTION_TYPE tau_direction;

  tau_direction = sla->tauDir();
  tau = stau = sla->sTau();
  etau = sla->eTau();

  while ((tau<etau && tau_direction==dbsk2d_ishock_edge::TAU_INCREASING) ||
         (tau>etau && tau_direction==dbsk2d_ishock_edge::TAU_DECREASING)) 
  {
    dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

    cur_sample->pt =  sla->getPtFromTau(tau);
    cur_sample->radius = sla->r(tau);
    cur_sample->edge_id = sla->edgeID();
    cur_sample->label = dbsk2d_xshock_sample::DEGENERATE;
    cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
    cur_sample->theta = sla->tangent(tau);
    cur_sample->speed = sla->v(tau);
    cur_sample->left_bnd_pt = sla->getLFootPt(tau);
    cur_sample->right_bnd_pt = sla->getRFootPt(tau);
    cur_sample->left_bnd_tangent = 0; //xx
    cur_sample->right_bnd_tangent = 0; //xx

    //add to referenced list to store with the edge
    new_xedge->push_back(cur_sample);

    double ds = delta_sample;
    //increment tau to unit arclength on shock
    if (tau_direction==dbsk2d_ishock_edge::TAU_INCREASING)
      next_tau = tau + ds/sla->d(tau);
    else
      next_tau = tau - ds/sla->d(tau);

    //use this next_tau
    tau = next_tau;
  }

  dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

  cur_sample->pt =  sla->getPtFromTau(etau);
  cur_sample->radius = sla->r(etau);
  cur_sample->edge_id = sla->edgeID();
  cur_sample->label = dbsk2d_xshock_sample::DEGENERATE;
  cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
  cur_sample->theta = sla->tangent(etau);
  cur_sample->speed = sla->v(etau);
  cur_sample->left_bnd_pt = sla->getLFootPt(etau);
  cur_sample->right_bnd_pt = sla->getRFootPt(etau);
  cur_sample->left_bnd_tangent = 0; //xx
  cur_sample->right_bnd_tangent = 0; //xx  

  //add to referenced list to store with the edge
  new_xedge->push_back(cur_sample);

}

void dbsk2d_sample_ishock::sample_ishock_edge(dbsk2d_ishock_arcarc* saa, dbsk2d_xshock_edge* new_xedge)
{
  double tau, stau, etau, next_tau;
  dbsk2d_ishock_edge::TAU_DIRECTION_TYPE tau_direction;

  tau_direction = saa->tauDir();
  tau = stau = saa->sTau();
  etau = saa->eTau();

  while ((tau<etau && tau_direction==dbsk2d_ishock_edge::TAU_INCREASING) ||
         (tau>etau && tau_direction==dbsk2d_ishock_edge::TAU_DECREASING)) 
  {
    dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

    cur_sample->pt =  saa->getPtFromTau(tau);
    cur_sample->radius = saa->r(tau);
    cur_sample->edge_id = saa->edgeID();
    cur_sample->label = dbsk2d_xshock_sample::DEGENERATE;
    cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
    cur_sample->theta = saa->tangent(tau);
    cur_sample->speed = saa->v(tau);
    cur_sample->left_bnd_pt = saa->getLFootPt(tau);
    cur_sample->right_bnd_pt = saa->getRFootPt(tau);
    cur_sample->left_bnd_tangent = 0; //xx
    cur_sample->right_bnd_tangent = 0; //xx

    //add to referenced list to store with the edge
    new_xedge->push_back(cur_sample);

    double ds = delta_sample;
    //increment tau to unit arclength on shock
    if (tau_direction==dbsk2d_ishock_edge::TAU_INCREASING)
      next_tau = tau + ds/saa->dFromLTau(tau);
    else
      next_tau = tau - ds/saa->dFromLTau(tau);

    //use this next_tau
    tau = next_tau;
  }

  dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

  cur_sample->pt =  saa->getPtFromTau(etau);
  cur_sample->radius = saa->r(etau);
  cur_sample->edge_id = saa->edgeID();
  cur_sample->label = dbsk2d_xshock_sample::DEGENERATE;
  cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
  cur_sample->theta = saa->tangent(etau);
  cur_sample->speed = saa->v(etau);
  cur_sample->left_bnd_pt = saa->getLFootPt(etau);
  cur_sample->right_bnd_pt = saa->getRFootPt(etau);
  cur_sample->left_bnd_tangent = 0; //xx
  cur_sample->right_bnd_tangent = 0; //xx  

  //add to referenced list to store with the edge
  new_xedge->push_back(cur_sample);

}

void dbsk2d_sample_ishock::sample_ishock_edge(dbsk2d_ishock_arcarc_thirdorder* sato, dbsk2d_xshock_edge* new_xedge)
{
  double tau, stau, etau, next_tau;
  dbsk2d_ishock_edge::TAU_DIRECTION_TYPE tau_direction;

  tau_direction = sato->tauDir();
  tau = stau = sato->sTau();
  etau = sato->eTau();

  while ((tau<etau && tau_direction==dbsk2d_ishock_edge::TAU_INCREASING) ||
         (tau>etau && tau_direction==dbsk2d_ishock_edge::TAU_DECREASING)) 
  {
    dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

    cur_sample->pt =  sato->getPtFromTau(tau);
    cur_sample->radius = sato->r(tau);
    cur_sample->edge_id = sato->edgeID();
    cur_sample->label = dbsk2d_xshock_sample::DEGENERATE;
    cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
    cur_sample->theta = sato->tangent(tau);
    cur_sample->speed = sato->v(tau);
    cur_sample->left_bnd_pt = sato->getLFootPt(tau);
    cur_sample->right_bnd_pt = sato->getRFootPt(tau);
    cur_sample->left_bnd_tangent = 0; //xx
    cur_sample->right_bnd_tangent = 0; //xx

    //add to referenced list to store with the edge
    new_xedge->push_back(cur_sample);

    double ds = delta_sample;
    //increment tau to unit arclength on shock
    if (tau_direction==dbsk2d_ishock_edge::TAU_INCREASING)
      next_tau = tau + ds/sato->d(tau);
    else
      next_tau = tau - ds/sato->d(tau);

    //use this next_tau
    tau = next_tau;
  }

  dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

  cur_sample->pt =  sato->getPtFromTau(etau);
  cur_sample->radius = sato->r(etau);
  cur_sample->edge_id = sato->edgeID();
  cur_sample->label = dbsk2d_xshock_sample::DEGENERATE;
  cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
  cur_sample->theta = sato->tangent(etau);
  cur_sample->speed = sato->v(etau);
  cur_sample->left_bnd_pt = sato->getLFootPt(etau);
  cur_sample->right_bnd_pt = sato->getRFootPt(etau);
  cur_sample->left_bnd_tangent = 0; //xx
  cur_sample->right_bnd_tangent = 0; //xx  

  //add to referenced list to store with the edge
  new_xedge->push_back(cur_sample);
}

//: sample an ishock node as an A1Ainf node
void dbsk2d_sample_ishock::sample_A1_Ainf_node(dbsk2d_ishock_node* a1ainf, 
                                               dbsk2d_xshock_edge* new_xedge)
{
  //this function currently only works for A1-Ainf not Ainf-Ainf
  
  //get the incoming and outgoing shock at this node
  dbsk2d_ishock_edge* last_iedge = a1ainf->get_parent_edge();
  // ozge: put this
  if (!last_iedge)
    return;

  dbsk2d_ishock_edge* next_iedge = last_iedge->get_next_edge();

  //get the two tangents and two phis
  VECTOR_TYPE tan_in = last_iedge->tangent(last_iedge->eTau());
  VECTOR_TYPE tan_out = next_iedge->tangent(next_iedge->sTau());
  double phi_in = last_iedge->phi(last_iedge->eTau());
  //double phi_out = next_iedge->phi(next_iedge->sTau());

  //determine the correct parameterization
  double dtan1 = CCW(tan_in, tan_out);
  double dtan2 = CCW(tan_out, tan_in);
  double dtan;
  int nu;

  if (dtan1<dtan2){ //arc on the left
    dtan = dtan1;
    nu = 1;
  }
  else {
    dtan = dtan2;
    nu = -1;
  }

  //now sample the node
  double tau, etau;
  tau = 0;
  etau = dtan;

  while (tau<etau)
  {
    dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

    cur_sample->pt =  a1ainf->origin();
    cur_sample->radius = a1ainf->startTime();
    cur_sample->edge_id = new_xedge->id();
    cur_sample->label = dbsk2d_xshock_sample::DEGENERATE;
    cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
    double tangent = angle0To2Pi(tan_in + nu*tau);
    cur_sample->theta = tangent;
    double phi = phi_in - tau; //phi always decreases
    cur_sample->speed = -1/vcl_cos(phi);
    cur_sample->left_bnd_pt = _translatePoint(a1ainf->origin(), tangent+phi, a1ainf->startTime()); //FIX ME!! (this will only work with A1-Ainf not Ainf-Ainf)
    cur_sample->right_bnd_pt = _translatePoint(a1ainf->origin(), tangent-phi, a1ainf->startTime());//FIX ME!!
    cur_sample->left_bnd_tangent = 0; //xx
    cur_sample->right_bnd_tangent = 0; //xx

    //add to referenced list to store with the edge
    new_xedge->push_back(cur_sample);

    //increment tau 
    tau += delta_sample/cur_sample->radius; 
  }

  //final sample
  dbsk2d_xshock_sample_sptr cur_sample = new dbsk2d_xshock_sample (new_sample_id());

  cur_sample->pt =  a1ainf->origin();
  cur_sample->radius = a1ainf->startTime();
  cur_sample->edge_id = new_xedge->id();
  cur_sample->label = dbsk2d_xshock_sample::DEGENERATE;
  cur_sample->type = dbsk2d_xshock_sample::NORMALSAMPLE;
  double tangent = angle0To2Pi(tan_in + nu*etau);
  cur_sample->theta = tangent;
  double phi = phi_in - etau; //phi always decreases
  cur_sample->speed = -1/vcl_cos(phi);
  cur_sample->left_bnd_pt = _translatePoint(a1ainf->origin(), tangent+phi, a1ainf->startTime()); //FIX ME!! (this will only work with A1-Ainf not Ainf-Ainf)
  cur_sample->right_bnd_pt = _translatePoint(a1ainf->origin(), tangent-phi, a1ainf->startTime());//FIX ME!!
  cur_sample->left_bnd_tangent = 0; //xx
  cur_sample->right_bnd_tangent = 0; //xx 

  //add to referenced list to store with the edge
  new_xedge->push_back(cur_sample);
}

//: Add the edge adjacency info to the nodes (respecting the ordering)
void dbsk2d_sample_ishock::add_edge_adjacency_info(int option)
{
  //go through all the nodes of the coarse shock graph and duplicate the 
  //edge adjacency information onto the corresponding extrinsic node 
  dbsk2d_shock_graph::vertex_iterator curN = shock_graph->vertices_begin();
  for (; curN != shock_graph->vertices_end(); curN++)
  {
    dbsk2d_shock_node_sptr cur_node = (*curN);
    
    // if only the inside shock graph is to be sampled,
    // make sure this node belongs to the largest shock component
    if (option==INSIDE)
      if (cc_label_map[cur_node->id()] != largest_component_id)
        continue;

    //find the corresponding extrinsic node
    dbsk2d_shock_node_sptr xnode = nodes_map.find(cur_node->id())->second;

    dbsk2d_shock_graph::edge_iterator curE = cur_node->in_edges_begin();
    for (; curE != cur_node->in_edges_end(); curE++)
    {
      dbsk2d_shock_edge_sptr cur_edge = (*curE);

      //find the corresponding extrinsic edge
      dbsk2d_shock_edge_sptr xedge = edges_map.find(cur_edge->id())->second;

      //update the node connectivity
      xnode->add_incoming_edge(xedge);
    }

    dbsk2d_shock_graph::edge_iterator curE2 = cur_node->out_edges_begin();
    for (; curE2 != cur_node->out_edges_end(); curE2++)
    {
      dbsk2d_shock_edge_sptr cur_edge = (*curE2);

      //find the corresponding extrinsic edge
      dbsk2d_shock_edge_sptr xedge = edges_map.find(cur_edge->id())->second;

      //update the node connectivity
      xnode->add_outgoing_edge(xedge);
    }
  }
}


