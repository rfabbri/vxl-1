//: MingChing Chang 
//  Nov 30, 2004

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>

#include <dbsk3dr/dbsk3dr_match.h>

#include <dbsk3dr/vis/dbsk3dr_draw.h>

#include <dbsk3d/vis/dbsk3d_vis_ms_hypg.h>
#include <dbsk3d/vis/dbsk3d_vis_ms_elm.h>

#include <Inventor/SbColor.h>
#include <Inventor/nodes/SoTranslation.h>

void generate_match_vertices_ctable (unsigned long seed, 
                                     dbsk3dr_match* smatch, 
                                     vcl_vector<SbColor>& v_color_table1,
                                     vcl_vector<SbColor>& v_color_table2)
{
  SbColor UNMATCHED_COLOR = SbColor (0.5f, 0.5f, 0.5f); //Gray
  //: v_color_table1 size = vertex_id_counter-1
  //  initialize to Gray
  v_color_table1.clear();
  for (int i=0; i<smatch->ms_hypg_G()->vertex_id_counter(); i++) {
    v_color_table1.push_back (UNMATCHED_COLOR);
  }
  //  v_color_table2 size = vertex_id_counter-1
  //  initialize to Gray
  v_color_table2.clear();
  for (int i=0; i<smatch->ms_hypg_g()->vertex_id_counter(); i++) {
    v_color_table2.push_back (UNMATCHED_COLOR);
  }

  //: initialize the random color by given seed
  init_rand_color (seed);  

  for (int a=0; a<smatch->ga_match()->M_row()-1; a++) {
    int i = smatch->ga_match()->labelGg(a);
    int SGid = smatch->G_sid (a);
    if (i != -1) { //a match
      int Sgid = smatch->g_sid (i);

      SbColor color = get_next_rand_color ();
      v_color_table1[SGid] = color;
      v_color_table2[Sgid] = color;
    }
  }
}

void generate_match_curves_ctable (unsigned long seed, dbsk3dr_match* smatch, 
                                   vcl_vector<SbColor>& v_color_table1,
                                   vcl_vector<SbColor>& v_color_table2)
{
  //v_color_table1 size = # edgemap
  //  initialize to Gray
  v_color_table1.clear();
  for (int i=0; i<smatch->ms_hypg_G()->edge_id_counter(); i++) {
    v_color_table1.push_back (UNMATCHED_COLOR);
  }

  //v_color_table2 size = # edgemap
  //  initialize to Gray
  v_color_table2.clear();
  for (int i=0; i<smatch->ms_hypg_g()->edge_id_counter(); i++) {
    v_color_table2.push_back (UNMATCHED_COLOR);
  }

  //initialize the random color by given seed
  init_rand_color (seed);  

  vcl_map<int, dbmsh3d_edge*>::iterator SC_it = smatch->ms_hypg_G()->edgemap().begin();
  for (; SC_it != smatch->ms_hypg_G()->edgemap().end(); SC_it++) {
    dbsk3d_ms_curve* GMC = (dbsk3d_ms_curve*) (*SC_it).second;
    if (GMC->selected() == false)
      continue;
    bool flip;
    dbsk3d_ms_curve* gMC = smatch->matched_g_curve (GMC, flip);

    //if there is a match, generate a random color and put to both table
    if (gMC) {
      SbColor color = get_next_rand_color ();
      int id1 = GMC->id();
      int id2 = gMC->id();

      v_color_table1[id1] = color;
      v_color_table2[id2] = color;
    }
  }
}

//######################################################################
//: VISUALIZATION OF THE SHOCK MATCHING

SoSeparator* draw_sg_match (dbsk3dr_match* smatch, 
                            const float radius, const float width,
                            const bool showid, const float shift, 
                            const vcl_vector<SbColor>& v_color_table1,
                            const vcl_vector<SbColor>& v_color_table2, 
                            const vcl_vector<SbColor>& c_color_table1,
                            const vcl_vector<SbColor>& c_color_table2)
{  
  SoSeparator* root = new SoSeparator;

  SoSeparator* root1 = new SoSeparator;
  SoSeparator* root2 = new SoSeparator;
  root->addChild (root1);
  root->addChild (root2);
  SoTranslation *trans = new SoTranslation;
  trans->translation.setValue (SbVec3f (shift, 0, 0));
  root2->addChild (trans);

  root1->addChild (draw_ms_vertices_ctable (smatch->ms_hypg_G(), v_color_table1, radius));

  root2->addChild (draw_ms_vertices_ctable (smatch->ms_hypg_g(), v_color_table2, radius));

  root1->addChild (draw_ms_curves_ctable (smatch->ms_hypg_G(), c_color_table1, width, false, showid));

  root2->addChild (draw_ms_curves_ctable (smatch->ms_hypg_g(), c_color_table2, width, false, showid));

  //draw the connecting lines between matching nodes of G and g.
  for (int a=0; a<smatch->ga_match()->M_row()-1; a++) {
    int i = smatch->ga_match()->labelGg(a);
    int SGid = smatch->G_sid (a);
    if (i != -1) { //a match
      int Sgid = smatch->g_sid (i);

      SbColor color = v_color_table1[SGid];
      dbsk3d_ms_node* MN1 = (dbsk3d_ms_node*) smatch->ms_hypg_G()->vertexmap (SGid);
      dbsk3d_ms_node* MN2 = (dbsk3d_ms_node*) smatch->ms_hypg_g()->vertexmap (Sgid);

      SoDrawStyle*  drawStyle = new SoDrawStyle ();
      drawStyle->lineWidth = 1;

      root->addChild (draw_line (static_cast<float>(MN1->V()->pt().x()), 
                                 static_cast<float>(MN1->V()->pt().y()), 
                                 static_cast<float>(MN1->V()->pt().z()), 
                                 static_cast<float>(MN2->V()->pt().x()) + shift, 
                                 static_cast<float>(MN2->V()->pt().y()), 
                                 static_cast<float>(MN2->V()->pt().z()), 
                                 color, drawStyle));
    }
  }

  return root;
}

SoSeparator* draw_match_MC_alignment (dbsk3d_ms_curve* MC1, dbsk3d_ms_curve* MC2, 
                                      vcl_vector< vcl_pair<int,int> >& alignment)
{
  SoSeparator* root = new SoSeparator;
  
  SoDrawStyle*  drawStyle = new SoDrawStyle ();
  drawStyle->lineWidth = 1;

  vcl_vector<dbmsh3d_vertex*> MC1_V_vec, MC2_V_vec;
  MC1->get_V_vec (MC1_V_vec);
  MC2->get_V_vec (MC2_V_vec);

  for (unsigned int i=0; i<alignment.size(); i++) {
    int vi1 = alignment[i].first;
    int vi2 = alignment[i].second;
    dbmsh3d_vertex* V1 = MC1_V_vec[vi1];
    dbmsh3d_vertex* V2 = MC2_V_vec[vi2];

    root->addChild (draw_line (V1->pt(), V2->pt(), SbColor (0, 1, 0), drawStyle));
  }

  return root;
}

