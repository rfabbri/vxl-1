// This is brcv/shp/dbsk2d/tests/test_ishock_belm.cxx

#include <testlib/testlib_test.h>
#include <vcl_map.h>

#include <dbsk2d/dbsk2d_ishock_belm.h>
#include <dbsk2d/dbsk2d_ishock_bpoint.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>
#include <dbsk2d/dbsk2d_ishock_barc.h>

#include <dbsk2d/dbsk2d_ishock_node.h>
#include <dbsk2d/dbsk2d_ishock_edge.h>
#include <dbsk2d/dbsk2d_ishock_pointline.h>
#include <dbsk2d/dbsk2d_ishock_contact.h>

#include <dbsk2d/dbsk2d_ishock_graph.h>

// Amir Tamrakar
// 04/10/2005

// test all the belement classes

void test_all_belm_classes()
{
  //--------------------------------------------------------------------------
  // Testing dbsk2d_ishock_bnd_key
  //--------------------------------------------------------------------------
  
  //The keys use eta1 (first parameter to order themselves)
  //only when eta1 are equal does the other parameter matter
  //This test is designed to make sure this is happening correctly.

  dbsk2d_ishock_bnd_key key1(0,dbsk2d_ishock_bnd_key::RIGHTCONTACT);
  dbsk2d_ishock_bnd_key key2(0,dbsk2d_ishock_bnd_key::RIGHT);

  dbsk2d_ishock_bnd_key key3(0.1,dbsk2d_ishock_bnd_key::LEFT);
  dbsk2d_ishock_bnd_key key4(0.1,dbsk2d_ishock_bnd_key::RIGHT);

  dbsk2d_ishock_bnd_key key5(0.2,dbsk2d_ishock_bnd_key::LEFT);
  dbsk2d_ishock_bnd_key key6(0.2,dbsk2d_ishock_bnd_key::LEFTCONTACT);

  //a map similar to the bnd_ishock_map for testing
  vcl_multimap<dbsk2d_ishock_bnd_key, int> test_map;

  //insert the keys into the map
  test_map.insert(vcl_pair<dbsk2d_ishock_bnd_key, int>(key1, 1));
  test_map.insert(vcl_pair<dbsk2d_ishock_bnd_key, int>(key2, 2));
  test_map.insert(vcl_pair<dbsk2d_ishock_bnd_key, int>(key3, 3));
  test_map.insert(vcl_pair<dbsk2d_ishock_bnd_key, int>(key4, 4));
  test_map.insert(vcl_pair<dbsk2d_ishock_bnd_key, int>(key5, 5));
  test_map.insert(vcl_pair<dbsk2d_ishock_bnd_key, int>(key6, 6));

  int id[6]; //array to store ids on the keys as they appear on the map
  vcl_multimap<dbsk2d_ishock_bnd_key, int>::iterator k_it = test_map.begin();
  for(int i=0; k_it!=test_map.end(); ++k_it, ++i)
    id[i]=k_it->second;

  // This is the order in which it should appear:
  // RIGHTCONTACT < RIGHT ; LEFT < RIGHT ; LEFT < LEFTCONTACT
  // i.e., 1 < 2 ; 3 < 4 ; 5 < 6

  TEST("Testing dbsk2d_ishock_bnd_key ordering", 
    id[0]==1 && id[1]==2 && id[2]==3 && id[3]==4 && id[4]==5 && id[5]==6, 
    true);

  //THE FOLLOWING TESTS ARE FOR QUERY SCENARIOS
  dbsk2d_ishock_bnd_key keyq1(0,dbsk2d_ishock_bnd_key::QUERY);
  dbsk2d_ishock_bnd_key keyq2(0.1,dbsk2d_ishock_bnd_key::QUERY);
  dbsk2d_ishock_bnd_key keyq3(0.2,dbsk2d_ishock_bnd_key::QUERY);

  int id1 = test_map.lower_bound(keyq1)->second;//greater than or equal to
  int id2 = test_map.lower_bound(keyq2)->second;//greater than or equal to
  int id3 = test_map.lower_bound(keyq3)->second;//greater than or equal to

  // The rules are:
  // RIGHTCONTACT < QUERY ; QUERY < L  < RIGHT ; QUERY < LEFT < LEFTCONTACT
  // i.e., 1 < q1 < 2 ; q2 < 3 < 4; q3 < 5 < 6;

  TEST("Testing dbsk2d_ishock_bnd_key query rules", 
    id1==2 && id2==3 && id3==5,
    true);

  //--------------------------------------------------------------------------
  // Testing dbsk2d_ishock_belm
  //--------------------------------------------------------------------------

  //create a few boundary elements
  dbsk2d_ishock_bpoint* bp1 = new dbsk2d_ishock_bpoint(0,0,1);
  dbsk2d_ishock_bpoint* bp2 = new dbsk2d_ishock_bpoint(1,0,2);
  dbsk2d_ishock_bline* bl1 = new dbsk2d_ishock_bline(bp1, bp2, 3);
  dbsk2d_ishock_bpoint* bp3 = new dbsk2d_ishock_bpoint(0.5,1,4);
  dbsk2d_ishock_bpoint* bp4 = new dbsk2d_ishock_bpoint(0,0.5,5);
  dbsk2d_ishock_bpoint* bp5 = new dbsk2d_ishock_bpoint(1.5,0.5,6);

  //                            bp3
  //                             .        
  //                   .                   
  //                  . .                 .       
  //                 .   .  s2       s3  ..    
  //                .       .         .    .  s4        
  // bp4  .        .             .          .              . bp5
  //             .                             .   
  //          .                                |   . s6    
  //      .     s1                             |      .
  //  c1  |                                    | c2
  //      |                                    |
  //  bp1 .------------------------------------. bp2
  //                         bl1
  //

  //create a shock graph container class
  dbsk2d_ishock_graph_sptr sg = new dbsk2d_ishock_graph(0);

  //create a few shock elements (no need to connect them) 
  dbsk2d_ishock_edge* c1 = new dbsk2d_ishock_contact(1, bp1, bl1, bp1->pt(), vnl_math::pi, 0, vnl_math::pi/2, vnl_math::pi/2, 0); //eta=0
  dbsk2d_ishock_edge* c2 = new dbsk2d_ishock_contact(2, bl1, bp2, bp2->pt(), 1, 0, vnl_math::pi/2, 1, vnl_math::pi/2); //eta=1
  bp1->set_vref(vnl_math::pi/2); //due to c1
  bp2->set_vref(vnl_math::pi/2); //due to c2
  bp4->set_vref(3*vnl_math::pi/2); //due to source between bp4 and bl1
  dbsk2d_ishock_edge* s1 = new dbsk2d_ishock_pointline(3, 0.25, 0, bp4, bl1, 2*vnl_math::pi, 0); //eta=0
  bp3->set_vref(3*vnl_math::pi/2);//due to source between bp3 and bl1
  dbsk2d_ishock_edge* s2 = new dbsk2d_ishock_pointline(4, 0.5, 0, bl1, bp3, 0.5, 0); //eta=0.5
  dbsk2d_ishock_edge* s3 = new dbsk2d_ishock_pointline(5, 0.5, 0, bp3, bl1, 2*vnl_math::pi, 0.5); //eta=0.5
  bp5->set_vref(2.5*vnl_math::pi/2);//due to source between bp5 and bp2
  dbsk2d_ishock_edge* s4 = new dbsk2d_ishock_pointline(6, 0.35, 0, bl1, bp5, 1, 0.25*vnl_math::pi); //eta=1

  //let c1 terminate at a junction with a shock not shown, producing s1 as the child shock
  c2->set_cSNode((dbsk2d_ishock_node*)(long)12345); //dummy node for dummy intersection
  c2->setEndTime(0.25);
  
  //let c2 terminate at a junction with s6, producing s4 as the child shock
  c2->set_cSNode((dbsk2d_ishock_node*)(long)12345); //dummy node for dummy intersection
  c2->setEndTime(0.75);
  
  //put these shocks into the shock graph
  sg->add_edge(c1);
  sg->add_edge(c2);
  sg->add_edge(s1);
  sg->add_edge(s2);
  sg->add_edge(s3);
  sg->add_edge(s4);

  //test bnd_ishock_map iterators + ordering on the line
  bnd_ishock_map_iter b_it = bl1->shock_map().begin();
  for(int i=0; b_it!=bl1->shock_map().end(); ++b_it, ++i)
    id[i]=b_it->second->id();

  TEST("Testing dbsk2d_ishock_map ordering", 
    id[0]==1 && id[1]==3 && id[2]==4 && id[3]==5 && id[4]==6 && id[5]==2,
    true);

  //Test wavefront relations between shocks 
  //These are set in ishock_belm->add_shock called by the shock edge constructors
  TEST("Testing wavefront relations on shocks edges",
    c1->rShock()==c2 &&  //c1 doesn't get to update its neighbor because the wavefront is passed on to s1
    s1->rShock()==s2 && s2->lShock()==s1 && //they share the same wavefront
    s2->rShock()==s3 && s3->lShock()==s2 && //s2 and s3 share the same wavefront from the point
    s3->rShock()==s4 && s4->lShock()==s3 && //s3 gets to interact with s4
    c2->lShock()==s3,  //c2 only gets to update its neighbor once because the wavefront moves on to s4
    true);

  //Test get_shock_at function
  dbsk2d_ishock_edge *qs1, *qs2, *qs3, *qs4, *qs5;
  bool degenerate = false;

  qs1 = bl1->get_shock_at(0, degenerate);
  qs2 = bl1->get_shock_at(0.25, degenerate);
  qs3 = bl1->get_shock_at(0.5, degenerate);
  qs4 = bl1->get_shock_at(0.8, degenerate);
  qs5 = bl1->get_shock_at(1, degenerate);

  TEST("Testing dbsk2d_ishock_belm get_shock_at(eta)",
    qs1==s1 &&
    qs2==s2 &&
    qs3==s2 &&
    qs4==s4 &&
    qs5==s4,
    true);

  //Test is_wavefront_alive function
  bool test1, test2, test3, test4, test5, test6;
  bool test7, test8, test9, test10, test11, test12;

  test1 = bl1->is_wavefront_alive(0, 0.1);    //true
  test2 = bl1->is_wavefront_alive(0, 0.25);   //false
  test3 = bl1->is_wavefront_alive(0, 0.5);    //false
  test4 = bl1->is_wavefront_alive(0.25, 0.1); //true
  test5 = bl1->is_wavefront_alive(0.25, 1);   //false
  test6 = bl1->is_wavefront_alive(0.5, 0.25); //true
  test7 = bl1->is_wavefront_alive(0.5, 0.5);  //false
  test8 = bl1->is_wavefront_alive(0.5, 0.75); //false
  test9 = bl1->is_wavefront_alive(0.8, 0.2);  //true
  test10 = bl1->is_wavefront_alive(0.8, 0.8); //false
  test11 = bl1->is_wavefront_alive(1, 0.1);   //true
  test12 = bl1->is_wavefront_alive(1, 2.0);   //false

  TEST("Testing dbsk2d_ishock_belm is_wavefront_alive(eta, r)",
    test1==true && test2==false && test3==false && test4==true &&
    test5==false && test6==true && test7==false && test8==false &&
    test9==true && test10==false && test11==true && test12==false,
    true);

  //Test wavefront relations after a shock is deleted
  sg->remove_edge(s1);

  TEST("Testing wavefront relations after deletion",
    c1->rShock()==s2 && s2->lShock()==c1, //they should now be sharing the same wavefront
    true);

  //repeat get_shock_at test at eta=0, after s1 is removed
  qs1 = bl1->get_shock_at(0, degenerate);

  TEST("Testing get_shock_at(eta) after deletion",
    qs1==s2,
    true);

  //repeat is_wavefront_alive after shock deletion
  test1 = bl1->is_wavefront_alive(0, 0.25);    //true

  TEST("Testing is_wavefront_alive after deletion",
    test1==true,
    true);

  //delete all the shocks
  sg->clear();
  //delete all the belements
  delete bl1; delete bp1; delete bp2; delete bp3; delete bp4; delete bp5;

  //--------------------------------------------------------------------------
  // Testing dbsk2d_ishock_bline
  //--------------------------------------------------------------------------
  //initialize a bline and its twin between two bpoints
  dbsk2d_ishock_bpoint* p1 = new dbsk2d_ishock_bpoint(10, 10, 1, 0); //id 1
  dbsk2d_ishock_bpoint* p2 = new dbsk2d_ishock_bpoint(20, 10, 2, 0); //id 2
  dbsk2d_ishock_bline* l1 = new dbsk2d_ishock_bline(p1, p2, 3, 1); //id 3 (GUI)
  dbsk2d_ishock_bline* l2 = new dbsk2d_ishock_bline(p2, p1, 4, 0); //id 4
  l1->set_twinLine(l2);
  l2->set_twinLine(l1);

  TEST("Testing dbsk2d_ishock_bline.Constructor", 
    l1 && l2, true);

  //check the connectivity functions

  //delete all the belements
  delete l1; delete l2; delete p1; delete p2; 
}

MAIN( test_ishock_belm )
{
  START( "dbsk2d_ishock_belm" );
  test_all_belm_classes();
  SUMMARY();
}
