// NEW PUZZLE SOLVING

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include <vcl_cstdio.h>
#include "PuzzleSolving.h"
#include <time.h>

//List of indexes of pairwise matches categorized by piece number.
//All pairs featuring the i'th piece will be listed in order of increasing cost
//in the _pairMatchesByPiece[i] element.
//moved this next one into puzzleSolving.h
//vcl_vector<vcl_vector<int> > _pairMatchesByPiece;
//vcl_vector<vcl_vector<vcl_vector<int> > > _matchesByPair;

int _totalIterateSearchTicks=0;
int _totalSearchTicks=0;
int _totalPairMatchTicks=0;
int _totalTestDPTicks=0;
int _totalComboMatchTicks=0;
int _totalLocalRegTicks=0;
int _totalFineScaleMatchTicks =0;

// DEBUGGING STUFF FOR CAN
void write_map(intMap &map, vcl_string fname, double cost)
{
  vcl_ofstream fstream(fname.c_str());
  fstream << "Cost: " << cost << vcl_endl;
  fstream << "Size: " << map.size() << vcl_endl;
  for(unsigned i=0; i < map.size(); i++)
    fstream << map[i].first << " " << map[i].second << vcl_endl;
  fstream.close();
}

double point_dist(vsol_point_2d p1, vsol_point_2d p2)
{
  return p1.distance(p2);
}

double point_dist(double x1, double y1, double x2, double y2)
{
  return vcl_sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
}

void PuzzleSolving::setContours(const vcl_vector<bfrag_curve> &contoursIN )
{ 
  //this will envoke the copy constructor and we will get a local copy
  _Contours = contoursIN;
  _nPieces = _Contours.size();
};

void PuzzleSolving::preProcessCurves()
{
  vcl_cout << "Preprocessing curves..." << vcl_endl;
  for(int i = 0; i < _nPieces; i++)
  {
    vcl_cout << "  preprocessing contour [" << i << "]." << vcl_endl;
    _Contours[i].smooth_by_discrete_curvature_algo(float(PSI), NUM_TIMES);
    _Contours[i].resample(FINE_RESAMPLE_DS);
    _Contours[i].resample_coarsely(COARSE_RESAMPLE_DS);
  }
  vcl_cout << "Preprocessing complete." << vcl_endl;
}

searchState PuzzleSolving::pairMatch()
{
  int i,j,k;
  searchState init = searchState(_Contours);
  vcl_vector<map_with_cost> this_pair;
  vcl_string exname1,exname2;

  _cList.clear();
  unsigned size = _Contours.size();
  for(unsigned i=0; i < size; i++)
    _cList.push_back(_Contours[i]);

  if (_nPieces>0)
  {
    for(i=0;i<_nPieces-1;i++)
    {
      for(j=i+1;j<_nPieces;j++)
      {
        vcl_cout << "matching piece [" << i << "] to piece [" << j << "]." << vcl_endl;
        clock_t start, end;
        start = clock();
        this_pair = testDP(_Contours[j],_Contours[i]);
        end = clock();
        vcl_cout << (double(end)-start) / CLOCKS_PER_SEC << vcl_endl;
        if(this_pair.size()>0)
        {
          int this_pair_size_int = static_cast<int>(this_pair.size());
          assert(this_pair_size_int>=0);
          for(k=0;k<this_pair_size_int;k++)
          {
            double cost = this_pair[k].first;
            intMap map = this_pair[k].second;
            bfrag_curve c1 = _Contours[j];
            bfrag_curve c2 = _Contours[i];

            // Update for state curve transformation lists not necessary here
            regContour(&c1,&c2,map);

            // CAN : This is not necessary! When I commented it out, I got the same result.
//            localReg(&c1,&c2,&d1,&d2,&d3,1);
            // CAN : This operation should be moved in the end part of fineScaleMatching

            cost-=SMOOTH_EDGE_BONUS*edgeCheck(&c1, &c2, map);
            init.addMatch(map,cost,j,i);
          }
        }
      }
    }
  }
  return init;
}

vcl_vector<searchState> PuzzleSolving::search(vcl_vector<searchState> all_states)
{ 
  /*
  // added by CAN
  // structure the states first
  if(all_states[0].nProcess > 0)
  {
    for(unsigned i=0; i < all_states.size(); i++)
    {
      all_states[i].structure();
      all_states[i]._merged = all_states[i]._constr[0];
    }
  }
  // added by CAN*/
  tot_num_iters_++;
  if(1)
  {
    if(all_states[0].nProcess == _nPieces)
      return all_states;

    vcl_cout << "Working..." << vcl_endl;

    vcl_vector<searchState> all_states2;
    indexedMeasures tCosts;

    // CAN ADDED
    vcl_sort(all_states.begin(), all_states.end(), search_state_sort());
    unsigned num_states_to_process = NUM_TOP_STATES_PROCESSED;
    if(all_states.size() < NUM_TOP_STATES_PROCESSED)
      num_states_to_process = all_states.size();

//    for( unsigned i = 0; i < all_states.size(); i++) 
    for(unsigned i=0; i < num_states_to_process; i++)
    {
      vcl_cout << "Processing State " << i << vcl_endl;
      if( all_states[i].active )
      {
        vcl_vector<searchState> top_state = iterateSearch(all_states[i],0);
        //Drawing states found with their costs
        for (unsigned a=0; a <top_state.size(); a++)
          vcl_cout << a << ": tCost: " << top_state[a].tCost << vcl_endl;

        double min_cost=0.0;
        double sc;
        for(unsigned k=0;k<top_state.size();k++)
        {
          sc=top_state[k].sCost;
          tCosts.push_back(vcl_pair<double,int>(top_state[k].tCost,all_states2.size()));
          all_states2.push_back( top_state[k] );
          if( sc < min_cost )
            min_cost = sc;
        } 
      }
    }

    if( all_states2.size() == 0 )
      return all_states;

    vcl_sort(tCosts.begin(),tCosts.end(),cost_ind_less());
    all_states.clear();

    int add;
    int index;
    double top=0.0;
    for( unsigned i = 0; i < all_states2.size(); i++)
    {
      index=tCosts[i].second;
      add=1;
      if(all_states.size()>0) 
      {
        for(unsigned k=0;k<all_states.size();k++) 
        {
          if(all_states2[index]==all_states[k]) 
          {
            all_states[k].tCost-= IDENTICAL_BONUS;
            add=0;
            break;
          }
        }
      } 
      else 
        top=all_states2[index].tCost;

      if( add == 1 )
      {
        if(all_states2[index].tCost-top<CULLING_RANGE)
          all_states.push_back(all_states2[index]);
      }
      if( all_states.size() == NUM_TOP_STATES_SAVED && all_states[0].nProcess > 2 )
        break;
    }

    all_states2.clear();
    tCosts.clear();

    if( all_states[0].nProcess > 2 )
    {
      int num_disp=NUM_TOP_STATES_SAVED;
      int all_states_size_int = all_states.size();
      if((all_states_size_int<num_disp) && !(all_states_size_int<0))
        num_disp=all_states.size();

      for( int z = 0; z < num_disp; z++) 
      {
        if(APPLY_COMBO_MATCH == true)
          all_states[z].structure();
        vcl_cout <<"tCost: "<< all_states[z].tCost << vcl_endl;
        vcl_cout <<"sCost: "<< all_states[z].sCost << vcl_endl;
      }
      vcl_cout << "Completed Phase " << all_states[0].nProcess << vcl_endl;
    }
  }
  else 
  {
    if( all_states[0].nProcess == _nPieces )
      return all_states;

    vcl_cout << "Working..." << vcl_endl;

    vcl_vector<searchState> all_states2;
    indexedMeasures tCosts;

    for(unsigned i = 0; i < all_states.size(); i++ )
    {
      if( all_states[i].active )
      {
        vcl_vector<searchState> top_state = iterateSearch(all_states[i],0);
        double min_cost=0.0;
        double sc;
        for(unsigned k = 0; k < top_state.size(); k++ )
        {
          sc=top_state[k].sCost;
          tCosts.push_back(vcl_pair<double,int>(top_state[k].tCost,all_states2.size()));
          all_states2.push_back(top_state[k]);
          if( sc < min_cost )
            min_cost=sc;
        }
      }
    }
    if( all_states2.size() == 0 ) 
      return all_states;

    vcl_sort(tCosts.begin(),tCosts.end(),cost_ind_less());
    all_states.clear();

    int add;
    int index;
    double top;
    for(unsigned i=0;i<all_states2.size();i++)
    {
      index=tCosts[i].second;
      add=1;
      if(all_states.size()>0)
      {
        for(unsigned k=0;k<all_states.size();k++)
        {
          if(all_states2[index]==all_states[k])
          {
            all_states[k].tCost-=IDENTICAL_BONUS;
            add=0;
            break;
          }
        }
      } 
      else
        top=all_states2[index].tCost;

      if(add == 1 && all_states2[index].tCost-top < CULLING_RANGE)
        all_states.push_back(all_states2[index]);

      if(all_states.size()==NUM_TOP_STATES_SAVED && all_states[0].nProcess>2)
        break;
    }

    all_states2.clear();
    tCosts.clear();

    if(all_states[0].nProcess>2)
    {
      unsigned num_disp=NUM_TOP_STATES_SAVED;
      if( all_states.size() < num_disp )
        num_disp=all_states.size();

      for( unsigned i = 0; i < num_disp; i++ )
      {
        if(APPLY_COMBO_MATCH == true)
          all_states[i].structure();
        vcl_cout << "tCost: "<< all_states[i].tCost << " sCost: " <<  all_states[i].sCost << vcl_endl;
      }
      vcl_cout << "Completed Phase " << all_states[0].nProcess << vcl_endl;
    }
  }
  return all_states;
}

vcl_vector<searchState> PuzzleSolving::iterateSearch(searchState this_state, int f)
{  
  // load cLists and update them
  this_state.load_state_curves_list();

  unsigned i;
  bool flag=0;
  if(f==1) 
  {
    flag=1;
    APPLY_COMBO_MATCH = true;
  }
  else
    APPLY_COMBO_MATCH = false;

  searchState temp_state;
  vcl_vector<bfrag_curve> temp_cList;
  vcl_vector<searchState> states;
  //In the current configuration, flag is never 1.
  if(APPLY_COMBO_MATCH)   this_state.comboMatch();
  //Sort the pairwise matchces
  this_state.sortPairwiseMatches();

  temp_state = this_state;
  temp_cList = _cList;

  //If there have not yet been any pieces added:
  if(this_state.nProcess==0)
  { 
    unsigned num = _matches.size();
    //assert( num == sorted_cost.size() );
//    if(num>2*NUM_TOP_STATES_SAVED) num=2*NUM_TOP_STATES_SAVED;
    for(i=0;/*i<num*/1;i++)
    {
      if(i >= this_state.matches_ez_list_.size())
        break;

      int index = this_state.matches_ez_list_[i].first;
      int p1 = _matches[index].whichCurves.first;
      int p2 = _matches[index].whichCurves.second;

      bfrag_curve *c1 = this_state.piece(p1);
      bfrag_curve *c2 = this_state.piece(p2);

      XForm3x3 xform = regContour( c1, c2, _matches[index].pointMap );

      this_state.process.push_back(p2);
      this_state.process.push_back(p1);
      this_state.nProcess = 2;

      double d1,d2,d3;
      intMap tmap;

      //Add piece c2 to the puzzle.
      this_state.merge(c2, tmap);
      //this_state.merged()->computeProperties();

      //Add piece c1 to the puzzle.
      tmap = localReg(c1, this_state.merged(), &d1, &d2, &d3, LOCAL_REG_ITER_PUZ);

      double costReduction = SMOOTH_EDGE_BONUS*edgeCheck(c1, c2, _matches[index].pointMap);

      this_state.merge(c1, tmap);
      //this_state.merged()->computeProperties();

      int l=tmap.size();

      //Find junction points
      vsol_point_2d pt1=c1->point(tmap[0].first);
      vsol_point_2d pt2=c2->point(tmap[0].second);
      vsol_point_2d pt3=c1->point(tmap[l-1].first);
      vsol_point_2d pt4=c2->point(tmap[l-1].second);
      vgl_point_2d<double> j1((pt1.x()+pt2.x())/2.0,(pt1.y()+pt2.y())/2.0);
      vgl_point_2d<double> j2((pt3.x()+pt4.x())/2.0,(pt3.y()+pt4.y())/2.0);

      //Add the junction points to the list of junction points
      vcl_vector<int> jj;
      jj.push_back(p1);
      jj.push_back(p2);
      this_state.open_junc.push_back(vcl_pair<vcl_vector<int>, vgl_point_2d<double> >(jj,j1));
      this_state.open_junc.push_back(vcl_pair<vcl_vector<int>, vgl_point_2d<double> >(jj,j2));

      //Update the tCost and sCost
      this_state.tCost+=DIST_COEF*d1+LENGTH_COEF*sqrt(d2)+DIAG_COEF*sqrt(d3)-costReduction;
      this_state.sCost=DIST_COEF*d1+LENGTH_COEF*sqrt(d2)+DIAG_COEF*sqrt(d3)-costReduction;

      //Categorize this match as a new edge.
      this_state.new_edge=vcl_pair<int,int>(p1,p2);
      //CAN ADDED FOR BETTER DEBUGGING
      this_state.matches_ez_list_[i].second = DP_VERY_LARGE_COST;
//      this_state._matches[i].cost = DP_VERY_LARGE_COST;
      //CAN ADDED FOR BETTER DEBUGGING

      //Check to make sure that the match satisfies the minimum diagnostic threshold.
      if(d3>DIAG_THRESH_LOW)
      {
        // ADDED BY CAN
        unsigned size = _cList.size();
        for(unsigned iii=0; iii < size; iii++)
        {
          this_state.transform_list_[iii] = _cList[iii].transform_;
          // update rotation angles list, too
          double cos_val = _cList[iii].transform_(0,0);
          double sin_val = _cList[iii].transform_(1,0);
          double angle = vcl_atan2(sin_val, cos_val);
          this_state.rot_ang_list_[iii] += angle;
        }
        // ADDED BY CAN

        states.push_back(this_state);
        // ADDED BY CAN
        if(states.size() == NUM_TOP_STATES_SAVED)
          break;
      }
      this_state = temp_state;
      _cList = temp_cList;
    }
  }
  else
  { 
    //else for the "if (state.nprocess == 0)"
    int num_good=0;
    for(i=0;i<this_state.numMatch();i++)
    { 
      int index = this_state.matches_ez_list_[i].first;
      double matchescost = this_state.matches_ez_list_[i].second;
//      double matchescost = this_state._matches[i].cost;
      if( matchescost == DP_VERY_LARGE_COST )
      {
        //its ok, theyre gunna be skipped anyways
        break;
      }

      int p1 = _matches[index].whichCurves.first;
      int p2 = _matches[index].whichCurves.second;

      bfrag_curve *c1;
      bfrag_curve *c2;

      if((p1 == 23 || p1 == 21)  && (p2 == 23 || p2 == 21))
        int can = 1;
      int chk1=1;
      int chk2=1;
      vcl_vector<int> proc = this_state.process;

      //Make sure that the pieces that we are dealing with have not
      //yet been added to the puzzle.
      if(vcl_find(proc.begin(),proc.end(),p1)==proc.end())
        chk1=0;
      if(vcl_find(proc.begin(),proc.end(),p2)==proc.end())
        chk2=0;
      if(p2>=_nPieces)
        chk2=1;

      int add,ext;

//      if( this_state._matches[i].cost == DP_VERY_LARGE_COST ) //this is not necessary
//        break;

      if( this_state.matches_ez_list_[i].second == DP_VERY_LARGE_COST ) //this is not necessary
        break;

      //If we are dealing with a match whose first piece has been added
      //and whose second piece has been added or has an index
      //greater than or equal to the number of pieces then we set the
      //cost to be very high.
      if(chk1==1 && chk2==1) 
      {
        this_state.matches_ez_list_[i].second = DP_VERY_LARGE_COST;
        temp_state.matches_ez_list_[i].second = DP_VERY_LARGE_COST;
        //this_state._matches[i].cost = DP_VERY_LARGE_COST;
        //temp_state._matches[i].cost = DP_VERY_LARGE_COST;
      }
      //Otherwise if the values of chk1 and 2 are different
      //and (p2>=_nPieces)==flag is 1.
      else if(chk1 != chk2 && (p2>=_nPieces)==flag) 
      {
        intMap fmap;
        bfrag_curve *c1EdgeCheck=this_state.piece(p1);
        bfrag_curve *c2EdgeCheck=this_state.piece(p2);

        //Check to see which piece has been added already
        if(chk1==0)
        {
          add=p1;
          ext=p2;
          fmap = _matches[index].pointMap;
        }
        else 
        {
          add=p2;
          ext=p1;
          fmap = _matches[index].pointMap;

          //Swap the order of the matches to make the indices from p2 be the first
          //and from p1 to be the second.
          for(unsigned cnt=0;cnt<fmap.size();cnt++)
            fmap[cnt]=vcl_pair<int,int>(fmap[cnt].second,fmap[cnt].first);
        }

        c2=this_state.piece(ext);

        bool triple_cond = 0;
        bool cont=1;

        //See if adding the next piece will line up with at least one junction already present
        if(this_state.nProcess==2)
        {
          cont=0;
          vsol_point_2d pe1=this_state.open_junc[0].second;
          vsol_point_2d pe2=this_state.open_junc[1].second;
          vsol_point_2d pn1=c2->point(fmap[0].second);
          vsol_point_2d pn2=c2->point(fmap[fmap.size()-1].second);

          if(point_dist(pe1,pn1) < LINEUP_DIST_THRESH) cont=1;
          if(point_dist(pe1,pn2) < LINEUP_DIST_THRESH) cont=1;
          if(point_dist(pe2,pn1) < LINEUP_DIST_THRESH) cont=1;
          if(point_dist(pe2,pn2) < LINEUP_DIST_THRESH) cont=1;
        }

        //If so, then proceed.
        if(cont) 
        {
          //regContour makes a copy of fmap, it wont change it, this is OK
          c1 = this_state.piece(add);
          XForm3x3 xform = regContour(c1, c2, fmap);

          xform = regContour(c1EdgeCheck, c2EdgeCheck, _matches[index].pointMap );

          double costReduction2 = SMOOTH_EDGE_BONUS*edgeCheck(c1EdgeCheck, c2EdgeCheck, _matches[index].pointMap);

          double olap=0.0;
          double ol;
          for(int pp=0;pp<this_state.nProcess;pp++)
          {
            ol = detectOverlap(c1,this_state.piece(this_state.process[pp]));
            if(ol>olap)
              olap=ol;
          }

          if(olap>OLAP_THRESH_LOW)
          {
            this_state.matches_ez_list_[i].second = DP_VERY_LARGE_COST;
            temp_state.matches_ez_list_[i].second = DP_VERY_LARGE_COST;
            //this_state._matches[i].cost = DP_VERY_LARGE_COST;
            //temp_state._matches[i].cost = DP_VERY_LARGE_COST;
          } 
          else 
          {  
            double dist_meas,leng_meas,diag_meas;

            intMap refined_map = localReg(c1, this_state.merged(), &dist_meas, &leng_meas, &diag_meas, LOCAL_REG_ITER_PUZ);

            vcl_vector<vcl_pair<int,vgl_point_2d<double> > > newj;

            //Add the new edge to the list.
            this_state.old_edges.push_back(this_state.new_edge);

            //Add the new edges to the list of old edges.
            for(unsigned cnt=0;cnt<this_state.new_edges.size();cnt++)
              this_state.old_edges.push_back(this_state.new_edges[cnt]);

            this_state.new_edges.clear();
            this_state.new_edge=vcl_pair<int,int>(p1,p2);

            for(int pc=0;pc<this_state.nProcess;pc++) 
            {
              double d1,d2,d3;
              int pp=this_state.process[pc];
              bfrag_curve ct = *c1;
              bfrag_curve *c2a = this_state.piece(pp);
              intMap tmap;

              searchState dummy;
              tmap = localReg(&ct, c2a, &d1, &d2, &d3, 1);

              if(d2/d1>5.0) 
              {
                if(pp!=p1 && pp!=p2)
                  this_state.new_edges.push_back(vcl_pair<int,int>(pp,add));

                int l=tmap.size();

                //Take care of junctions again  
                vsol_point_2d pt1=c1->point(tmap[0].first);
                vsol_point_2d pt2=c2a->point(tmap[0].second);
                vsol_point_2d pt3=c1->point(tmap[l-1].first);
                vsol_point_2d pt4=c2a->point(tmap[l-1].second);
                vgl_point_2d<double> j1((pt1.x()+pt2.x())/2.0,(pt1.y()+pt2.y())/2.0);
                vgl_point_2d<double> j2((pt3.x()+pt4.x())/2.0,(pt3.y()+pt4.y())/2.0);
                newj.push_back(vcl_pair<int,vgl_point_2d<double> >(pp,j1));
                newj.push_back(vcl_pair<int,vgl_point_2d<double> >(pp,j2));
              }
            }

            for(unsigned nj=0;nj<newj.size();nj++) 
            {
              vsol_point_2d pn = newj[nj].second;
              bool addj=1;

              for(unsigned oj=0;oj<this_state.open_junc.size();oj++) 
              {
                vsol_point_2d po = this_state.open_junc[oj].second;
                if(point_dist(po,pn)<40.0) {
                  addj=0;
                  vcl_vector<int> other = this_state.open_junc[oj].first;
                  int l=other.size();
                  vgl_point_2d<double> pa((po.x()*(double)(l-1)+pn.x())/(double)l,
                    (po.y()*(double)(l-1)+pn.y())/(double)l);
                  if(vcl_find(other.begin(),other.end(),add)==other.end()) 
                  {
                    this_state.open_junc[oj].first.push_back(add);
                    this_state.open_junc[oj].second=pa;
                  }
                  else
                  {
                    this_state.open_junc[oj].second=pa;
                    this_state.closed_junc.push_back(this_state.open_junc[oj]);
                    vcl_vector<vcl_pair<vcl_vector<int>,vgl_point_2d<double> > > temp_junc;
                    for(unsigned pop=0;pop<oj;pop++)
                      temp_junc.push_back(this_state.open_junc[pop]);
                    for(unsigned pop2=oj+1;pop2<this_state.open_junc.size();pop2++)
                      temp_junc.push_back(this_state.open_junc[pop2]);
                    this_state.open_junc=temp_junc;
                    this_state.tCost -= CLOSED_JUNCTION_BONUS;
                    triple_cond = 1;
                  }
                  break;
                }
              }
              if(addj) 
              {
                vcl_vector<int> jj;
                jj.push_back(newj[nj].first);
                jj.push_back(add);
                this_state.open_junc.push_back(vcl_pair<vcl_vector<int>,vgl_point_2d<double> >(jj,newj[nj].second));
              }
            }

            triple_cond = triple_cond || (this_state.nProcess>2);

            vcl_cout << "------------------------------"<< vcl_endl;
            vcl_cout << vcl_endl;
            vcl_cout << vcl_endl;
            vcl_cout << "Piece "<< p1 << " matching with Piece " << p2 << vcl_endl;

            this_state.tCost += DIST_COEF*dist_meas+LENGTH_COEF*sqrt(leng_meas)+DIAG_COEF*sqrt(diag_meas)-costReduction2;
            this_state.sCost  = DIST_COEF*dist_meas+LENGTH_COEF*sqrt(leng_meas)+DIAG_COEF*sqrt(diag_meas)-costReduction2;

            this_state.process.push_back(add);
            this_state.nProcess++;

            vcl_cout << "sCost: " << this_state.sCost << vcl_endl;

            if(leng_meas>LENGTH_THRESH_HIGH && triple_cond && 
              ((diag_meas>DIAG_THRESH_LOW && dist_meas<DIST_THRESH_LOW) || 
              (diag_meas>DIAG_THRESH_HIGH && dist_meas<DIST_THRESH_HIGH))) 
            {
              // CAN ADDED FOR EASIER DEBUGGING
              this_state.matches_ez_list_[i].second = DP_VERY_LARGE_COST;
              //this_state._matches[i].cost = DP_VERY_LARGE_COST;
              // CAN ADDED FOR EASIER DEBUGGING
              this_state.merge(c1, refined_map);
              //this_state.updateCost();
              if(flag)
                this_state.purge();

              // ADDED BY CAN
              unsigned size = _cList.size();
              for(unsigned iii=0; iii < size; iii++)
              {
                this_state.transform_list_[iii] = _cList[iii].transform_;
                // update rotation angles list, too
                double cos_val = _cList[iii].transform_(0,0);
                double sin_val = _cList[iii].transform_(1,0);
                double angle = vcl_atan2(sin_val, cos_val);
                this_state.rot_ang_list_[iii] += angle;
              }
              // ADDED BY CAN

              states.push_back(this_state);
              num_good++;
              if(num_good==NUM_STATES_ITER) 
                break;
            }
          }
        }
      }
      this_state = temp_state;
      _cList = temp_cList;
    }
  }
  return states;
}

searchState::searchState(vcl_vector<bfrag_curve> contours)
{
  state_id_ = -1;
  active = 1;

  unsigned num_frags = contours.size();
  transform_list_.resize(num_frags);
  rot_ang_list_.resize(num_frags);
  for(unsigned i=0; i<num_frags; i++)
  {
    transform_list_[i].set_identity();
    rot_ang_list_[i] = 0;
  }

//  _cList = contours;
  nProcess = 0;
  tCost = 0;
  sCost = 0;
  _numMatch = 0;
  _num_new = 0;
}
/*JONAH'S VERSION: NOT VALID ANYMORE
bool searchState::operator==(searchState state2) 
{  
  if(nProcess != state2.nProcess)
    return 0;
  
  vcl_vector<int> proc1 = process;
  vcl_vector<int> proc2 = state2.process;
  
  assert(proc1.size()==proc2.size());

  vcl_sort(proc1.begin(),proc1.end());
  vcl_sort(proc2.begin(),proc2.end()); 
  for(unsigned i=0;i<proc1.size();i++) 
  {
    if(proc1[i] != proc2[i])
      return 0;
  }

  bfrag_curve c1;
  bfrag_curve c2;
  intMap map;
  int pts = 0;

  for(unsigned i=0;i<proc1.size();i++) 
  {
    int num=piece(proc1[i])->num_coarse_points();
    for(int j=0;j<num;j++) 
    {
      map.push_back(vcl_pair<int,int>(pts,pts));
      c1.append(piece(proc1[i])->Cpoint(j));
      c2.append(state2.piece(proc1[i])->Cpoint(j));
      pts++;
    }
  }

  assert(c1.num_fine_points() == c2.num_fine_points());
  // Update for state curve transformation lists not necessary here
  regContour(&c1,&c2,map);
  
  return c1==c2;
}*/

// CAN'S VERSION (After getting rid of cList in searchState)
bool searchState::operator==(searchState state2) 
{  
  if(nProcess != state2.nProcess)
    return 0;
  
  vcl_vector<int> proc1 = process;
  vcl_vector<int> proc2 = state2.process;
  
  assert(proc1.size()==proc2.size());

  vcl_sort(proc1.begin(),proc1.end());
  vcl_sort(proc2.begin(),proc2.end()); 
  for(unsigned i=0;i<proc1.size();i++) 
  {
    if(proc1[i] != proc2[i])
      return 0;
  }

  bfrag_curve c1;
  bfrag_curve c2;
  intMap map;
  int pts = 0;

  for(unsigned i=0;i<proc1.size();i++) 
  {
    bfrag_curve add_curve = _cList[proc1[i]];
    vnl_matrix_fixed<double,3,3> trans1 = this->transform_list_[proc1[i]];
    vnl_matrix_fixed<double,3,3> trans2 = state2.transform_list_[proc1[i]];
    unsigned size = add_curve.num_coarse_points();
    for(unsigned i=0; i<size; i++)
    {
      map.push_back(vcl_pair<int,int>(pts,pts));
      double x = add_curve.level2_[i].x();
      double y = add_curve.level2_[i].y();
      
      double trans_x_1 = x * trans1(0,0) + y * trans1(0,1) + trans1(0,2);
      double trans_y_1 = x * trans1(1,0) + y * trans1(1,1) + trans1(1,2);

      double trans_x_2 = x * trans2(0,0) + y * trans2(0,1) + trans2(0,2);
      double trans_y_2 = x * trans2(1,0) + y * trans2(1,1) + trans2(1,2);

      c1.append(vgl_point_2d<double> (trans_x_1, trans_y_1));
      c2.append(vgl_point_2d<double> (trans_x_2, trans_y_2));

      pts++;
    }
  }

  assert(c1.num_fine_points() == c2.num_fine_points());
  // Update for state curve transformation lists not necessary here
  regContour(&c1,&c2,map);
  
  return c1==c2;
}

void searchState::merge(bfrag_curve *cv, intMap &map) 
{
  if(_merged.num_fine_points()==0) 
  {
    _merged=*cv;
    return;
  }
 
  vcl_pair<double,int> dist;
  bfrag_curve new_merge;
  for(unsigned i=0;i<cv->num_fine_points();i++) 
  {
  dist=ptDist(cv->x(i),cv->y(i),&_merged);
  if(dist.first>10.0)
  new_merge.append(cv->point(i));
  }

  for(unsigned i=0;i<_merged.num_fine_points();i++) 
  {
  dist=ptDist(_merged.x(i),_merged.y(i),cv);
  if(dist.first>10.0)
  new_merge.append(_merged.point(i));
  }

  _merged=new_merge;
  /*
  bfrag_curve new_merge;
  // no pieces added yet
  if(_merged.num_fine_points() == 0)
  {
    for(unsigned i=0;i<cv->num_fine_points();i++) 
      new_merge.append(cv->point(i));
  }
  else
  {
    // TIME EFFICIENT
    vcl_vector<bool> common_points_cv(cv->num_fine_points(), true);
    vcl_vector<bool> common_points_merged(_merged.num_fine_points(), true);
    for(unsigned i=0; i < map.size(); i++)
    {
      common_points_cv[map[i].first] = false;
      common_points_merged[map[i].second] = false;
    }
    for(unsigned i=0; i < cv->num_fine_points(); i++)
    {
      if(common_points_cv[i] != false)
        new_merge.append(cv->point(i));
    }
    for(unsigned i=0; i < _merged.num_fine_points(); i++)
    {
      if(common_points_merged[i] != false)
        new_merge.append(_merged.point(i));
    }
  }
  _merged = new_merge;*/
}

void searchState::structure() 
{
  _constr.clear();

  bfrag_curve temp;
  vcl_vector<bfrag_curve> frags;
  
  temp.append(_merged.point(0));
  for(unsigned i=1;i<_merged.num_fine_points();i++) 
  {
    if(point_dist(_merged.x(i),_merged.y(i),_merged.x(i-1),_merged.y(i-1))>15.0)  
    {
      frags.push_back(temp);
      temp.empty();
    }
    temp.append(_merged.point(i));
  }
  if(temp.num_fine_points()>0)
    frags.push_back(temp);

  temp.empty();
  vcl_vector<int> used;  
  for(unsigned i=0;i<frags.size();i++)
    used.push_back(0);
  
  double d,mind;
  int ref;

  for(unsigned i=0;i<frags.size();i++) 
  {
    if(used[i]==0) 
    {
      used[i]=1;
      temp=frags[i];
      ref=-1;
//SOMETHING VERY WEIRD IS GOING ON HERE -- PLEASE FIX!!! -MM      
      while(ref!=static_cast<int>(i)) 
      {
        mind=DP_VERY_LARGE_COST;
        for(unsigned j=i;j<frags.size();j++) 
        {
          if(used[j]==0 || j==i) 
          {
            d=point_dist(temp.x(temp.num_fine_points()-1),temp.y(temp.num_fine_points()-1),
              frags[j].x(0),frags[j].y(0));
            if(d<mind) 
            {
              mind=d;
              ref=j;
            }
          }
        }
//SOMETHING VERY WEIRD IS GOING ON HERE -- PLEASE FIX!!! -MM      
        if(ref!=static_cast<int>(i))
        {
          for(unsigned q=0;q<frags[ref].num_fine_points();q++)
            temp.append(frags[ref].point(q));
          used[ref]=1;
        }
      }
      if(temp.num_fine_points()>100) 
      {
        temp.compute_properties();
        _constr.push_back(temp);
      }
    }
  }
}

//Runs testDP on all the possible combinations?
void searchState::comboMatch() 
{
  vcl_vector<map_with_cost> this_pair;
  vcl_vector<int> proc = process;
  int num=_cList.size();

  if(_num_new>0) 
  {
    vcl_cout << "Warning! New Matches Not Purged!" << vcl_endl;
    return;
  }

  //structure();

  for(unsigned i=0;i<numCon();i++)
    _constr[i].resample_coarsely(COARSE_RESAMPLE_DS);

  for(unsigned i=0;i<numCon();i++)
  {
    for(int j=0;j<num;j++)
    {
      if(vcl_find(proc.begin(),proc.end(),j)==proc.end())
      {
        this_pair = testDP(_cList[j], _constr[i]);
        vcl_cout << "Matching: " << j+1 << " to " << i+num+1 << vcl_endl;
        if(this_pair.size()>0)
          for(unsigned k=0;k<this_pair.size();k++) 
          {
            addMatch(this_pair[k].second,this_pair[k].first,j,i+num);
            _num_new++;
          }
      }
    }
  }
}

void searchState::addMatch(intMap map, double cst, int c1, int c2) 
{  
  bfrag_curve curve1 = _cList[c1];
  bfrag_curve curve2 = _cList[c2];

  // Update for state curve transformation lists not necessary here
  XForm3x3 theXform = regContour(&curve1,&curve2,map);

  //non-pointer style
  pairwiseMatch newMatch;
  newMatch.cost = cst;
  newMatch.myIndex = _numMatch;
  newMatch.pointMap = intMap(map);
  newMatch.whichCurves = vcl_pair<int,int>(c1,c2);
  newMatch.xForm = XForm3x3( theXform );
  _matches.push_back(newMatch);

  vcl_pair<int,double> temp;
  temp.first = newMatch.myIndex;
  temp.second = newMatch.cost;
  matches_ez_list_.push_back(temp);

  _numMatch++;
} 

//non pointer style
void searchState::sortPairwiseMatches()
{
  vcl_sort( matches_ez_list_.begin(), matches_ez_list_.end(), pairwiseMatchSort() );
//  vcl_sort( _matches.begin(), _matches.end(), pairwiseMatchSort() );
}

void searchState::purge() 
{
  int cnt=_num_new;
  //FORK
  for(int i=0;i<cnt;i++) 
  {
    //_maps.pop_back();
    //cost.pop_back();
    //_pairs.pop_back();
    _numMatch--;
    _num_new--;
  }  
  _matches.empty();
}

void searchState::updateCost() 
{
  int i;
  int total=0;
  int exter=0;
  int inter=0;

  for(i=0;i<nProcess;i++)
    total+=piece(process[i])->num_fine_points();
  
  exter=merged()->num_fine_points();
  inter=static_cast<int>(floor((double)(total-exter)/2.0));

  sCost=static_cast<double>((double)exter/(double)inter);

}

// **********************************************************************
/*
- Extract sub-curves of each fragment and apply matching in coarse scale
- Sort and pick the top NUM_COARSE_OUT matches for the next stage
- Do matching in fine scale for the picked matches above
- Sort and pick the top NUM_FINE_OUT at most
*/

vcl_vector<map_with_cost> testDP(bfrag_curve &c1, bfrag_curve &c2)
{
  unsigned s1,i,j;
  int k,ji,ki;
  bfrag_curve c1i,c2i,c1_cs,c2_cs,c1i_cs,c2i_cs;

  // set up contours
  c1i = c1; 
  c1i.invert();
  c2i = c2;
  c2i.invert();
  c1_cs = c1; 
  c2_cs = c2; 
  c1i_cs = c1i;
  c2i_cs = c2i;

  c1_cs.become_coarse();     // curve1 at coarse scale
  c2_cs.become_coarse();     // curve2 at coarse scale
  c1i_cs.become_coarse();    // curve1 inverted at coarse scale
  c2i_cs.become_coarse();    // curve2 inverted at coarse scale

  int n=c1_cs.num_fine_points();  // number of points on curve1
  int m=c2_cs.num_fine_points();  // number of points on curve2
  int num_corners1=c1_cs.num_corners(); // number of extrema(probably corners) points on curve1
  int num_corners2=c2_cs.num_corners(); // number of extrema(probably corners) points on curve2

  // set up handling of matches
  vcl_vector<map_with_cost > allMaps;  // vector of (map, cost) pairs used for holding the coarse-scale maps
  vcl_vector<map_with_cost > allMaps2; // vector of (map, cost) pairs used for holding the fine-scale maps
  vcl_vector<map_with_cost> maps_out;  // vector of (map, cost) pairs used for outputting the best maps
  map_with_cost map_and_cost; // used for outputting the map and its cost together
  double cost;
  intMap fmap; // a map used for getting final map after matching
  intMap fmapi; // a map used for getting final map

  int num_maps=0; // number of maps
  int no_match_found = 1; // flag to denote if any matches are found, not the best name to use!

  // begin loop - tries all possible pairs of
  // corner points as starting points for match

  for(int kr=0;kr<num_corners2;kr++)
  { 
    // for each extrema on curve2
    k=c2_cs.corner(kr); // index of the kr'th extrema of curve2 at coarse scale
    ki=c2i_cs.corner(kr); // index of the kr'th extrema of curve2 inverted at coarse scale

    for(int jr=0;jr<num_corners1;jr++)
    { 
      // for each extrema on curve1
      j=c1_cs.corner(jr); // index of the jr'th extrema of curve1 at coarse scale
      ji=c1i_cs.corner(jr); // index of the jr'th extrema of curve1 at coarse scale

      // extend correspondence in both directions
      // points on the curves are assumed to be given in one specific direction (CW or counter-CW)
      DPMatch<bfrag_curve,double> d1(c1i_cs, c2_cs, ji, k);
      DPMatch<bfrag_curve,double> d2(c1_cs, c2i_cs, j, ki);

      d1.match();
      d2.match();

      fmap = d1.finalMap(); // final map
      s1 = fmap.size();     // final map size
      cost=d1.finalCost();  // final cost

      // if the map is big enough and the cost is negative, add to the maps list
      // if it is positive, do not add it to the list
      if(s1>=MIN_MAP_SIZE_CRSE && cost<0)
      {
        // correct map for starting point offset
        for (i=0;i<s1;i++)
        {    
          int index1,index2;
          index1=fmap[i].first+ji;
          if(index1>n-1) 
            index1-=n;
          index2=fmap[i].second+k;
          if(index2>m-1)
            index2-=m;
          fmap[i].first=index1;
          fmap[i].second=index2;
        }
        map_and_cost.first=cost;
        map_and_cost.second=fmap;
        allMaps.push_back(map_and_cost);

        num_maps++;
        no_match_found=0;
      }
      // The same operations for map1 are applied to map2
      fmap = d2.finalMap();
      cost=d2.finalCost();
      fmapi = fmap;
      s1 = fmap.size();

      if(s1>=MIN_MAP_SIZE_CRSE && cost<0)
      {  
        // correct map for starting point offset - reverse direction  
        for (i=0;i<s1;i++)
        {    
          int index1,index2;
          index1=fmap[i].first+j;
          if(index1>n-1)
            index1-=n;
          index2=fmap[i].second+ki;
          if(index2>m-1)
            index2-=m;
          fmapi[s1-i-1].first=n-index1-1;
          fmapi[s1-i-1].second=m-index2-1;
        }
        map_and_cost.first=cost;
        map_and_cost.second=fmapi;
        allMaps.push_back(map_and_cost);

        num_maps++;
        no_match_found=0;
      }
    }
  }

  if(no_match_found==0)
  { 
    // Matches found!
    vcl_vector<bfrag_curve> group;
    int add,cnt;

    // sort the coarse maps according to their costs
    vcl_sort(allMaps.begin(),allMaps.end(),map_cost_less());

    //Make sure the overlap of each match is not too high
    //and that it has not already been added.
    unsigned allmaps_size_int = static_cast<unsigned>(allMaps.size());
    for (j=0;j<allmaps_size_int;j++)
    {
      fmap = allMaps[j].second;
      // Update for state curve transformation lists not necessary here
      regContour(&c1i_cs,&c2_cs,fmap);

      c1_cs=c1i_cs;
      c1_cs.invert();
      // the overlap should be small enough
      if(detectOverlap(&c1_cs,&c2_cs)<OLAP_THRESH_HIGH) 
      {
        add=1;
        if(group.size()>0) 
        {
          int group_size_int = static_cast<int>(group.size());
          // check to see if the map is already in the list
          for(cnt=0;cnt<group_size_int;cnt++) 
          {
            if(c1i_cs==group[cnt])
            {
              add=0;
              break;
            }
          }
        }
        // add the map to the group list if it was not added before
        if(add==1)
        {
          // do fine scale matching
          map_and_cost = fineScaleMatch(c1i, c2, fmap);
          allMaps2.push_back(map_and_cost);
          group.push_back(c1i_cs);
        }
      }
      // stop the fine scale matching when we have enough number of top matches
      if(allMaps2.size()==NUM_COARSE_OUT)
        break;
    }

    unsigned num_fine = allMaps2.size();
    // output a fixed number of fine maps
    if(num_fine>NUM_FINE_OUT)
      num_fine=NUM_FINE_OUT;

    // Sort the fine scale maps according to their costs
    vcl_sort(allMaps2.begin(),allMaps2.end(),map_cost_less());

    for (j=0;j<num_fine;j++)
    { 
      // if the cost is too large, do not output the remaining maps
      if(allMaps2[j].first>MAX_FINE_COST)
        break;
      maps_out.push_back(allMaps2[j]);
    }
  }
  return maps_out;
}


map_with_cost fineScaleMatch(bfrag_curve c1i, bfrag_curve c2, intMap fmap)
{
  int iMap,i;
  int nf = c1i.num_fine_points();
  int mf = c2.num_fine_points();
  intMap fine_map;
  map_with_cost map_and_cost;
  bfrag_curve c1_sub;
  bfrag_curve c2_sub;

  double distance_measure=0;
  double length_measure=0;
  double diagnostic_measure=0;

  int st1=0;
  int st2=0;
  int en1=0;
  int en2=0;
  int type=0;

  if(c1i.is_corner(fmap[fmap.size()-1].first))
    st1=1;
  if(c2.is_corner(fmap[fmap.size()-1].second))
    st2=1;
  if(c1i.is_corner(fmap[0].first))
    en1=1;
  if(c2.is_corner(fmap[0].second))
    en2=1;

  //Make sure we are dealing with curves that have at least 2 corners between them
  type=st1+st2+en1+en2-2;
  if(type<0) 
  {
    vcl_cout << vcl_endl << "warning - invalid match" << vcl_endl;
    map_and_cost.first = DP_VERY_LARGE_COST;
    map_and_cost.second = fmap;
    return map_and_cost;
  }

  //Find the starting and ending points
  int c1_start = c1i.coarse_ref(fmap[fmap.size()-1].first);
  int c2_start = c2.coarse_ref(fmap[fmap.size()-1].second);
  int c1_end = c1i.coarse_ref(fmap[0].first);
  int c2_end = c2.coarse_ref(fmap[0].second);

  //Find the distances between the starting and ending points of each curve
  double dist1 = point_dist(c1i.point(c1_start),c1i.point(c1_end));
  double dist2 = point_dist(c2.point(c2_start),c2.point(c2_end));

  //Determine if type 1 match.
  if(st1==1 && st2==1 && en1==1 && en2==1)
  {
    if(fabs(dist1-dist2)>10.0)
      type=1;
  }      
  //Sorting out starting point.
  else if(en1==1 && en2==1)
  {
    if(dist1<dist2)
    {
      double dif=dist2-dist1;
      double pdif=DP_VERY_LARGE_COST;
      while(dif<pdif)
      {
        c2_start++;
        c2_start%=mf;
        pdif=dif;
        dif=fabs(point_dist(c2.point(c2_start),c2.point(c2_end))-dist1);
      }
      c2_start--;
      c2_start=(c2_start+mf)%mf;
    }
    else 
    {
      double dif=dist1-dist2;
      double pdif=DP_VERY_LARGE_COST;
      while(dif<pdif) 
      {
        c1_start++;
        c1_start%=nf;
        pdif=dif;
        dif=fabs(point_dist(c1i.point(c1_start),c1i.point(c1_end))-dist2);
      }
      c1_start--;
      c1_start=(c1_start+nf)%nf;
    }
  }
  else 
  {
    if(dist1<dist2) 
    {
      double dif=dist2-dist1;
      double pdif=DP_VERY_LARGE_COST;
      while(dif<pdif)
      {
        c2_end--;
        c2_end=(c2_end+mf)%mf;
        pdif=dif;
        dif=fabs(point_dist(c2.point(c2_start),c2.point(c2_end))-dist1);
      }
      c2_end++;
      c2_end%=mf;
    }
    else
    {
      double dif=dist1-dist2;
      double pdif=DP_VERY_LARGE_COST;
      while(dif<pdif)
      {
        c1_end--;
        c1_end=(c1_end+nf)%nf;
        pdif=dif;
        dif=fabs(point_dist(c1i.point(c1_start),c1i.point(c1_end))-dist2);
      }
      c1_end++;
      c1_end%=nf;
    }
  }
  dist1=point_dist(c1i.point(c1_start),c1i.point(c1_end));
  dist2=point_dist(c2.point(c2_start),c2.point(c2_end));

  c1_sub.append(c1i.point(c1_start));
  if(c1_end < c1_start) c1_end+=nf;
  int offset1=static_cast<int>(floor((double)((c1_end-c1_start)%FINE_SAMPLE_SKIP)/2.0));
  // ADDED BY CAN TO MAKE THINGS FASTER
  if(offset1 == 0)
    offset1 += FINE_SAMPLE_SKIP;
  // ADDED BY CAN TO MAKE THINGS FASTER
  for(iMap=c1_start+offset1;iMap<c1_end;iMap+=FINE_SAMPLE_SKIP)
    c1_sub.append(c1i.point(iMap%nf));
  c1_end%=nf;
  c1_sub.append(c1i.point(c1_end));

  c2_sub.append(c2.point(c2_start));
  if(c2_end < c2_start) c2_end+=mf;
  int offset2=static_cast<int>(floor((double)((c2_end-c2_start)%FINE_SAMPLE_SKIP)/2.0));
  // ADDED BY CAN TO MAKE THINGS FASTER
  if(offset2 == 0)
    offset2 += FINE_SAMPLE_SKIP;
  // ADDED BY CAN TO MAKE THINGS FASTER
  for(iMap=c2_start+offset2;iMap<c2_end;iMap+=FINE_SAMPLE_SKIP)
    c2_sub.append(c2.point(iMap%mf));
  c2_end%=mf;
  c2_sub.append(c2.point(c2_end));

  c1_sub.compute_properties();
  c2_sub.compute_properties();

  FineDPMatch<bfrag_curve,double> fsmatch(c1_sub,c2_sub);
  fsmatch.match();
  fine_map=fsmatch.finalMap();

  int index1,index2;
  int s=fine_map.size();

  fine_map[0].first=c1_end;
  fine_map[0].second=c2_end; 

  for (i=1;i<s-1;i++)
  {
    //JONAH
    //index1=((fine_map[i].first-1)*FINE_SAMPLE_SKIP+offset1+c1_start) % nf;
    //CAN: The above was a potential crash line, I replaced it with the below safe line
    index1=(((fine_map[i].first-1)*FINE_SAMPLE_SKIP+offset1+c1_start) + nf) % nf;
    fine_map[i].first=index1;
    //JONAH
    //index2=((fine_map[i].second-1)*FINE_SAMPLE_SKIP+offset2+c2_start) % mf;
    //CAN: The above was a crashing line, I replaced it with the below line
    index2=(((fine_map[i].second-1)*FINE_SAMPLE_SKIP+offset2+c2_start) + mf) % mf;
    fine_map[i].second=index2;
  }

  fine_map[s-1].first=c1_start;
  fine_map[s-1].second=c2_start;

  // Update for state curve transformation lists not necessary here
  regContour(&c1i,&c2,fine_map);

  c1i.invert();

  intMap map;
  searchState dummy_state;
  map = localReg(&c1i,&c2,&distance_measure,&length_measure,&diagnostic_measure,LOCAL_REG_ITER_PAIR);
  // Update for state curve transformation lists not necessary here
  regContour(&c1i,&c2,map);
  localReg(&c1i,&c2,&distance_measure,&length_measure, &diagnostic_measure,LOCAL_REG_ITER_PAIR);

//  double ang_T = ANGLE_SIM_THRESH;
  //-spinner-
  double dl=0.0;
  double ang=0.0;

  double cost = DIST_COEF*distance_measure + LENGTH_COEF*sqrt(length_measure) + 
    DIAG_COEF*sqrt(diagnostic_measure) + EDGE_COEF*dl;

  //if(distance_measure>3.0) cost+=25.0;
  if(length_measure<LENGTH_THRESH_LOW) 
    cost+=DP_VERY_LARGE_COST;
  if(diagnostic_measure<(DIAG_THRESH_LOW-1.5) && type!=2) 
    cost+=DP_VERY_LARGE_COST;
  if(map.size()<MIN_MAP_SIZE_FINE) 
    cost+=DP_VERY_LARGE_COST;
  //cost-=10.0*(double)type;

  map_and_cost.first=cost;
  map_and_cost.second=map;

  return map_and_cost;
}    

int tot_local_reg = 0;
int num_entrance = 0;

intMap localReg(bfrag_curve *cv, bfrag_curve *mg, double* dis,double* len, double* dgn, int num_iter)
{
  double dif=DP_VERY_LARGE_COST;
  indexedMeasures d_list;
  //A list of the distance of each point of cv from mg
  vcl_pair<double,int> dist;
  bfrag_curve last;
  double t_dist=0;
  double length,diag;
  intMap map;
  unsigned i,j,i2,index;
  int iter,start;
  unsigned sec=0;

  double T = LOCAL_REG_THRESH_HIGH;
  double T2 = LOCAL_REG_THRESH_LOW;
  unsigned num = cv->num_fine_points();
  unsigned ncp = cv->num_coarse_points();

  for(iter=0;iter<num_iter;iter++)
  {
    last=*cv;
    t_dist=0.0;
    map.clear();
    d_list.clear();
    start=-1;

    d_list.resize(num);
    int d_list_index = 0;

    //JONAH
    if(ncp==0)
    {
      // compute the distance of each point on cv to mg
      // store the distance and the index of the closest point on mg
      for(i=0;i<num;i++)
      {
        dist = ptDist(cv->level3_[i].x(), cv->level3_[i].y(), mg);
        //dist = ptDist(cv->x(i), cv->y(i), mg);
        //d_list.push_back(dist);
        d_list[d_list_index++] = dist;
        //Locate the start of the aligning of the curve.
        if(start==-1 && dist.first>T)
          start=i;
      }
    }
    else
    {
      unsigned end=0;
      int st;
      for(j=0;j<ncp;j++)
      {
        st=end;
        if(j==ncp-1)
          end=num;
        else
          end=cv->coarse_ref(j);
        assert(end<=num);
        dist=ptDist(cv->level2_[j].x(), cv->level2_[j].y(), mg);
        //dist=ptDist(cv->Cx(j), cv->Cy(j), mg);
        if(dist.first<100.0)
        {
          for(i=st;i<end;i++)
          {
            dist=ptDist(cv->level3_[i].x(), cv->level3_[i].y(), mg);
            //dist=ptDist(cv->x(i),cv->y(i),mg);
            //d_list.push_back(dist);
            d_list[d_list_index++] = dist;
            if(start==-1 && dist.first>T)
              start=i;
          }
        }
        else
        {
          if(start==-1) 
            start=st; 
          for(i=st;i<end;i++)
            //d_list.push_back(vcl_pair<double,int>(100.0,0));
            d_list[d_list_index++] = vcl_pair<double,int>(100.0,0);
        }
      }
      // CAN added for taking care of small pieces
      // If no point on cv has a distance less than T, then take the point with the biggest distance
      if(start == -1)
      {
        double max_dist = 0;
        for(unsigned ii=0; ii < num; ii++)
        {
          if(d_list[ii].first > max_dist)
          {
            max_dist = d_list[ii].first;
            start = ii;
          }
        }
      }
      // CAN added for taking care of small pieces
    }
    
    //assert(static_cast<int>(d_list.size())==num);
    assert(d_list_index == num);

    if(start==-1) 
    {
      for(i=0;i<num;i++) 
      {
        assert(index>=0 && index<num);
        map.push_back(vcl_pair<int,int>(i,d_list[i].second));
        t_dist+=d_list[index].first;
      }
    }
    else 
    {
      double cave;
      double pave=0;
      int flag=0;

      for(i=1;i<num+1;i++) 
      {
        index=(i+start)%num;
        assert(index>=0 && index<num);

        if(flag==0) // to be safe, double-finds the starting point
        {
          if(d_list[index].first<T) 
          {
            flag=1;
            pave=100;
          }
        }
        if(flag==1) //
        {
          cave=(d_list[(index-1+num)%num].first +
            d_list[index].first +
            d_list[(index+1)%num].first)/3.0;
          if((pave-cave)<0.1 || cave<T2) 
          {
            sec=0;
            flag=2;
          }
          pave=cave;
        }

        if(flag==2) 
        {
          map.push_back(vcl_pair<int,int>(index,d_list[index].second));
          t_dist+=d_list[index].first;
          sec++;
          if(d_list[index].first>T)
          {
            flag=3;
          }
        }
        if(flag==3) 
        {
          pave=100;
          i2=index;
          unsigned interval=sec;
          for(j=0;j < interval;j++) 
          {
            //vcl_cout << i2 << " ";
            assert(i2>=0 && i2<num);
            cave=(d_list[(i2-1+num)%num].first +
              d_list[i2].first +
              d_list[(i2+1)%num].first)/3.0;
            if((pave-cave)<0.1 || cave<T2)
              break;
            map.pop_back();
            t_dist-=d_list[i2].first;
            sec--;
            i2--;
            i2=(i2+num)%num;
            pave=cave;
          }
          if(sec<5) 
          {
            for(j=0;j<sec;j++) 
            {
              map.pop_back();
              t_dist-=d_list[i2].first;
              i2--;
              i2=(i2+num)%num;
            }
          }
          flag=0;
        }
      }
    }

    if(map.size()<2) 
    {
      *dis=DP_VERY_LARGE_COST;
      *len=0;
      *dgn=0;
      return map;
    }

    t_dist/=map.size();

    XForm3x3 xform = regContour(cv,mg,map);

    dif=last-*cv;
    if(dif<1.0)
      break;
  }

  // This is the common boundary
  bfrag_curve ave;
  unsigned map_size_int = static_cast<unsigned>(map.size());
  for(i=0;i<map_size_int;i++)
    ave.append(vgl_point_2d<double>((cv->x(map[i].first)+mg->x(map[i].second))/2.0, 
                                    (cv->y(map[i].first)+mg->y(map[i].second))/2.0));

  double l;
  length=0.0;
  vcl_pair<int,int> endPts;
  for(i=0;i<ave.num_fine_points()-1;i++)
    for(j=i+1;j<ave.num_fine_points();j++)
    {
      l=point_dist(ave.level3_[i].x(), ave.level3_[i].y(), ave.level3_[j].x(), ave.level3_[j].y());
      //l=point_dist(ave.x(i),ave.y(i),ave.x(j),ave.y(j));
      if(l>length)
      {
        length=l;
        endPts.first=i;
        endPts.second=j;
      }  
    }

  intMap dmap;
  bfrag_curve dummy;
  
  dummy.append(vgl_point_2d<double>(0.0,0.0));
  dummy.append(vgl_point_2d<double>(length,0.0));  
  
  dmap.push_back(vcl_pair<int,int>(endPts.first,0));
  dmap.push_back(vcl_pair<int,int>(endPts.second,1)); 

  // Update for state curve transformation lists not necessary here
  regContour(&ave,&dummy,dmap);
  diag=flat(&ave);
  
  *dis=t_dist;
  *len=length;
  *dgn=diag;

  return map;
}

// Given two curves, c1 and c2, this function works as follows:
// For all points of c1/c2 inside c2/c1, find the one point with the maximum distance,
// and return this distance
// If two curves do not overlap, return 0
// This function uses the coarse-level-curves if available
// If not available, the fine-level-curves are used
double detectOverlap(bfrag_curve *c1, bfrag_curve *c2) 
{
  vcl_pair<double,int> dist;
  double m_dist=0.0;
  int num1 = int(c1->num_coarse_points());
  int num2 = int(c2->num_coarse_points());
  bool oFlag = 1;

  if(num1>0) 
  {  
    for(int i=0;i<num1;i++)
    {
      if(inPolygon(c1->Cx(i),c1->Cy(i),c2)==oFlag) 
      {
        dist=ptDist(c1->Cx(i),c1->Cy(i),c2);
        if(dist.first>m_dist)
          m_dist=dist.first;
      }
    }
  }
  
  else 
  {
    for(unsigned i=0;i<c1->num_fine_points();i++)
    {
      //if(inPolygon(c1->x(i),c1->y(i),c2)==oFlag) 
      if(inPolygon(c1->level3_[i].x(), c1->level3_[i].y(),c2)==oFlag) 
      {
        dist=ptDist(c1->level3_[i].x(), c1->level3_[i].y(),c2);
        //dist=ptDist(c1->x(i),c1->y(i),c2);
        if(dist.first>m_dist)
          m_dist=dist.first;
      }
    }  
  }
  
  if(num2>0)
  {
    for(int i=0;i<num2;i++)
    {
      if(inPolygon(c2->Cx(i),c2->Cy(i),c1)==oFlag) 
      {
        dist=ptDist(c2->Cx(i),c2->Cy(i),c1);
        if(dist.first>m_dist)
          m_dist=dist.first;
      }
    }
  }
  
  else 
  {
    for(unsigned i=0;i<c2->num_fine_points();i++) 
    {
      //if(inPolygon(c2->x(i),c2->y(i),c1)==oFlag) 
      if(inPolygon(c2->level3_[i].x(), c2->level3_[i].y(), c1)==oFlag) 
      {
        dist=ptDist(c2->level3_[i].x(), c2->level3_[i].y(), c1);
        if(dist.first>m_dist)
          m_dist=dist.first;
      }
    }
  }
  return m_dist;
} 

// Given the coordinates of a point and a curve, this function returns true if
// the point is inside the curve, and false otherwise
// It uses the course-level-curve if available
// If the point is outside the curve, the sum of the angle differences
// should be a very small value, else the point is inside the curve
bool inPolygon(double x, double y, bfrag_curve *c) 
{ 
  double t_angle = 0;
  double p1x,p1y,p2x,p2y;
  int num = int(c->num_coarse_points());

  if(num>0)
  {
    for(int i=0;i<num;i++)
    {
      p1x=c->Cx(i%num);
      p1y=c->Cy(i%num);
      p2x=c->Cx((i+1)%num);
      p2y=c->Cy((i+1)%num);
      
      t_angle+=angleDiff(atan2(p2y-y,p2x-x),atan2(p1y-y,p1x-x));
    }
  }
  else 
  {  
    num=c->num_fine_points();
    
    for(int i=0;i<num;i++) 
    {
      p1x=c->x(i%num);
      p1y=c->y(i%num);
      p2x=c->x((i+1)%num);
      p2y=c->y((i+1)%num);
      
      t_angle+=angleDiff(atan2(p2y-y,p2x-x),atan2(p1y-y,p1x-x));
    }
  }
  assert((fabs(t_angle) > 6.0 && fabs(t_angle) < 6.5) || fabs(t_angle) < 0.1);
  return fabs(t_angle) > 1.0;
}

// Given a point p(x,y) and a curve c, this function finds the closest
// point on the curve to the point, and returns the distance with the 
// closest point's index of the curve
vcl_pair<double,int> ptDist(double x, double y, bfrag_curve *c)
{
  double d;
  vcl_pair<double,int> dist;
  unsigned i;
  unsigned num = c->num_coarse_points();
  unsigned num_fine = c->num_fine_points();

  if(num==0)
  {
    // if there is no coarse level representation for the curve,
    // use the fine level representation
    dist.first=point_dist(x,y,c->level3_[0].x(), c->level3_[0].y());
//    dist.first=point_dist(x,y,c->x(0),c->y(0));
    dist.second=0;
    for(i=1;i < num_fine;i++)
    {
      d=point_dist(x,y,c->level3_[i].x(),c->level3_[i].y());
//      d=point_dist(x,y,c->x(i),c->y(i));
      if(d < dist.first)
      {
        dist.first=d;
        dist.second=i;
      }   
    }  
  }
  else
  {
    // if there is coarse level representation of the curve, use it first!
    dist.first=point_dist(x,y,c->level2_[0].x(),c->level2_[0].y());
//    dist.first=point_dist(x,y,c->Cx(0),c->Cy(0));
    dist.second=0;
    for(i=1;i<num;i++) 
    {
      d=point_dist(x,y,c->level2_[i].x(),c->level2_[i].y());
//      d=point_dist(x,y,c->Cx(i),c->Cy(i));
      if(d<dist.first) 
      {
        dist.first=d;
        dist.second=i;
      }
    }
    // find the previous and next points' indices in the fine scale
    // compare all fine points between these two points for smaller distance
    int start=c->coarse_ref((dist.second-1+num)%num);
    int end=c->coarse_ref((dist.second+1)%num);
    num=c->num_fine_points();
    if(start>end)
      end+=num;

    dist.first=point_dist(x,y,c->level3_[start].x(), c->level3_[start].y());
//    dist.first=point_dist(x,y,c->x(start),c->y(start));
    dist.second=start;
    for(int i=start+1;i<end;i++) 
    {
      d=point_dist(x,y,c->level3_[i%num].x(), c->level3_[i%num].y());
//      d=point_dist(x,y,c->x(i%num),c->y(i%num));
      if(d<dist.first)
      {
        dist.first=d;
        dist.second=i%num;
      }
    }
  }
  return dist;
} 

// Given a point p(x,y) and a curve c, this function finds the closest
// point on the curve to the point, and returns the distance with the 
// closest point's index of the curve
vcl_pair<double,int> new_ptDist(double x, double y, bfrag_curve *c, unsigned index)
{
  double d;
  vcl_pair<double,int> dist;
  unsigned i;
  unsigned num_fine = c->num_fine_points();

  // if there is no coarse level representation for the curve,
  // use the fine level representation
  dist.first=point_dist(x, y, c->level3_[index].x(), c->level3_[index].y());
  dist.second=index;
  for(i=index-CLOSEST_POINT_NEIGHBORHOOD;i < index+CLOSEST_POINT_NEIGHBORHOOD;i++)
  {
    int corr_i = (i + num_fine) % num_fine;
    d=point_dist(x, y, c->level3_[corr_i].x(), c->level3_[corr_i].y());
    if(d < dist.first)
    {
      dist.first=d;
      dist.second=corr_i;
    }   
  }  
  return dist;
} 

// definition in PuzzleSolving.h, bool flip = 0
// registers the given two curves according to the given alignment
XForm3x3 regContour(bfrag_curve *c1, bfrag_curve *c2, intMap map, bool flip, XForm3x3 *out) 
{  
  if(map.size()<2)
    return XForm::identity();

  // if flip is set, swap the curves, i.e. copy c1 into c2 and c2 into c1 using a temporary variable
  if(flip)
  {
    bfrag_curve *ct = c1;
    c1=c2;
    c2=ct;
    for(unsigned cnt=0;cnt<map.size();cnt++)
        map[cnt]=vcl_pair<int,int>(map[cnt].second,map[cnt].first);
  }

  vcl_vector<vsol_point_2d > p1;
  vcl_vector<vsol_point_2d > p2;  
  double c1x=0, c1y=0, c2x=0, c2y=0; 
  double h1=0, h2=0, h3=0, h4=0;
  double a, b, theta, Tx, Ty;
  int i, map_size;

  map_size = map.size();
  
  // load p1 and p2 with the aligned points on the two curves starting from the last aligned points
  for(i = map_size-1; i >= 0; i--)
  {
    p1.push_back(c1->point(map[i].first));
    p2.push_back(c2->point(map[i].second));
  }

  for(i = 0; i < map_size; i++)
  {
    c1x += p1[i].x();
    c1y += p1[i].y();
    c2x += p2[i].x();
    c2y += p2[i].y();
  }

  c1x /= map_size; 
  c1y /= map_size; 
  c2x /= map_size; 
  c2y /= map_size;

  // (c1x, c1y) holds the average coordinates of curve1 points here
  // (c2x, c2y) holds the average coordinates of curve2 points here

  for(i = 0; i < map_size; i++)
  {
    p1[i] = vsol_point_2d(p1[i].x() - c1x, p1[i].y() - c1y);
    p2[i] = vsol_point_2d(p2[i].x() - c2x, p2[i].y() - c2y);
  }

  for(i = 0; i < map_size; i++)
  {
    h1 += p1[i].x() * p2[i].x();
    h2 += p1[i].x() * p2[i].y();
    h3 += p1[i].y() * p2[i].x();
    h4 += p1[i].y() * p2[i].y();
  }

  a = h2 - h3;
  b = h1 + h4;
  
  theta = atan2(-a,b); 
  Tx = c2x - ( cos(theta)*c1x + sin(theta)*c1y);
  Ty = c2y - (-sin(theta)*c1x + cos(theta)*c1y);
  
//  c1->rotateTranslate(theta,Tx,Ty);
  c1->rotate_translate(theta, Tx, Ty);
  c1->update_angles(-theta);

  // CAN removed! This is very stupid to do! The angle is converted to degrees
  // and then used in rotateMe function without converting back inside cos and sin
  // functions. This would result in a wrong transformation matrix!!!
  //converting theta in radians to degrees
//  theta *= 57.2957;

  XForm3x3 axf, bxf;
  axf.translateMe(Tx,Ty);
  bxf.rotateMe(-theta);

  XForm3x3::mult( axf, bxf );

  if(out)
    XForm3x3::deep_copy(out, axf);
  
  return axf;
}


double flat(bfrag_curve *c) 
{
  int i,n;
  double a,b,x,y;
  double mean_x=0;
  double mean_y=0;
  double xx=0;
  double xy=0;
  double Err=0;

  n=c->num_fine_points();

  for(i=0;i<n;i++) 
  {
    //x=c->x(i);
    //y=c->y(i);
    x = c->level3_[i].x();
    y = c->level3_[i].y();
    mean_x+=x;
    mean_y+=y;
    xx+=x*x;
    xy+=x*y;
  }
  mean_x/=n;
  mean_y/=n;

  a=(mean_y*xx-mean_x*xy)/(xx-n*mean_x*mean_x);
  b=(xy-n*mean_x*mean_y) /(xx-n*mean_x*mean_x);

  for(i=0;i<n;i++) 
    Err+=fabs(c->level3_[i].y() - (a+b*c->level3_[i].x()));
//    Err+=fabs(c->y(i)-(a+b*c->x(i)));
  Err/=n;
  
  return Err;
}

vcl_pair<double,double> center(bfrag_curve *c) 
{
  double mx=0;
  double my=0;
  int N = int(c->num_coarse_points());
  
  for(int i=0;i<N;i++) 
  {
    mx+=c->Cx(i);
    my+=c->Cy(i);
  }

  mx/=N;
  my/=N;

  return vcl_pair<double,double>(mx,my);
}



// Registers the two curves according to the given alignment, and 
// checks the number of continuous edges
int edgeCheck(bfrag_curve *c1, bfrag_curve *c2, intMap map)
{
  //Initialize variables
  int startIndx1 = map[0].first;
  int startIndx2 = map[0].second;
  int endIndx1 = map[map.size()-1].first;
  int endIndx2 = map[map.size()-1].second;
  int numPts1 = c1->num_fine_points();
  int numPts2 = c2->num_fine_points();
  int numberContinuousEdges=0;
  double angle1=0;
  double angle2=0;
  //Boolean variables that indicate whether the map moves in forward or reverse order for each curve
  //Move the two contours close to one another
  XForm3x3 xform = regContour(c1,c2,map);

//  c1->compute_properties();
//  c2->compute_properties();

  //If the intMap goes forward for c1 move back along the contour
  //and find the average of the angles of the tangents over the curve.
  //if (movesForward1){
  int startTestIndx1 = startIndx1 - static_cast<int>(MOVE_BACK);
  while (startTestIndx1 < 0)
    startTestIndx1 += numPts1;
  if (startTestIndx1 >= numPts1)
    startTestIndx1 %=numPts1;

  for (int i =0; i < NUM_POINTS_AVERAGED; i++)
  {
    if (startTestIndx1+i+1>numPts1)
      startTestIndx1=-i;
    angle1 += c1->angle(startTestIndx1+i);
  }
  angle1 = angle1/NUM_POINTS_AVERAGED;
  if (angle1 < -M_PI)
    angle1=2*M_PI-angle2;
  if (angle1 < 0)
    angle1 +=M_PI;
  if (angle1 >= M_PI)
    angle1 -= M_PI;

  int startTestIndx2 = startIndx2 + static_cast<int>(MOVE_BACK);
  if (startTestIndx2 >= numPts2)
    startTestIndx2 %= numPts2;

  for (int k =0; k > -NUM_POINTS_AVERAGED; k--)
  {
    if (startTestIndx2+k < 0)
      startTestIndx2 = numPts2-k-1;
    angle2 += c2->angle(startTestIndx2+k);
  }
  angle2 = angle2/NUM_POINTS_AVERAGED;
  if (angle2 < -M_PI)
    angle2=2*M_PI-angle2;
  if (angle2 < 0)
    angle2 +=M_PI;
  if (angle2 >= M_PI)
    angle2 -= M_PI;
  
  if ((anglesAreClose(angle1, angle2))&&(distanceCheck(c1, c2, map[0].first, map[0].second)))
    numberContinuousEdges++;
    
  angle1 = 0;
  angle2 = 0;

  int endTestIndx1 = endIndx1 + static_cast<int>(MOVE_BACK);
  if (endTestIndx1 >= numPts1)
    endTestIndx1 %= numPts1;

    for (int i2 =0; i2 > -NUM_POINTS_AVERAGED; i2--)
    {
      if (endTestIndx1+i2 < 0)
        endTestIndx1 = numPts1-i2-1;
      angle1 += c1->angle(endTestIndx1+i2);
    }

    angle1 = angle1/NUM_POINTS_AVERAGED;
    if (angle1 < -M_PI)
      angle1=2*M_PI-angle2;
    if (angle1 < 0)
      angle1 +=M_PI;
    if (angle1 >= M_PI)
      angle1 -= M_PI;

    int endTestIndx2 = endIndx2 - static_cast<int>(MOVE_BACK);
    while (endTestIndx2 < 0)
      endTestIndx2 += numPts2;
    if (endTestIndx2 >= numPts2)
      endTestIndx2 %=numPts2;

    for (int k2 =0; k2 < NUM_POINTS_AVERAGED; k2++)
    {
      if (endTestIndx2+k2+1>numPts2)
        endTestIndx2=0-k2;
      angle2 += c2->angle(endTestIndx2+k2);
    }
    angle2 = angle2/NUM_POINTS_AVERAGED;
    
    if (angle2 < -M_PI)
      angle2=2*M_PI-angle2;
    if (angle2 < 0)
      angle2 +=M_PI;
    if (angle2 >= M_PI)
      angle2 -= M_PI;
      
    if ((anglesAreClose(angle1, angle2))&&(distanceCheck(c1, c2, map[map.size()-1].first, map[map.size()-1].second)))
      numberContinuousEdges++;
   
 return numberContinuousEdges;
}

//Checks to see whether the two corners being dealt with are close enough together.
//First it checks to see whether there are two extrema near enough to each other
//and then determines whether the extrema on the first curve is close enough to the
//beginning of match.
// CAN says: Not "to the beginning of the match", but to the "check point" which
// can be the endpoint of the match in this case
bool distanceCheck(bfrag_curve *c1, bfrag_curve *c2, int checkIndex1, int checkIndex2)
{
  vsol_point_2d checkPoint1 = c1->point(checkIndex1);
  vsol_point_2d checkPoint2 = c2->point(checkIndex2);

  //Cycles through all combinations of corners in order to compare their distances from
  //one another.
  for (unsigned i=0; i < c1->num_corners(); i++)
  {
    for (unsigned j=0; j < c2->num_corners(); j++)
    {
      int index = c1->coarse_ref(c1->corner(i));
      vsol_point_2d point1 = c1->point(index);
      vsol_point_2d point2 = c2->point(c2->coarse_ref(c2->corner(j)));

      //The distance of this pair of corners from each other
      double distance1 = 
        sqrt((point1.x()-point2.x())*(point1.x()-point2.x())+(point1.y()-point2.y())*(point1.y()-point2.y()));

      //The distance of the corner on contour 1 from the start of the match
      double distance2 = 
        sqrt((point1.x()-checkPoint1.x())*(point1.x()-checkPoint1.x())
        +(point1.y()-checkPoint1.y())*(point1.y()-checkPoint1.y()));

      //The distance of the corner on contour 1 from the start of the match
      double distance3 = 
        sqrt((point2.x()-checkPoint2.x())*(point2.x()-checkPoint2.x())
        +(point2.y()-checkPoint2.y())*(point2.y()-checkPoint2.y()));

      //If the distances are small, return 1
      if ((distance1 < DISTANCE_THRESH_1)&&((distance2 < DISTANCE_THRESH_2)||(distance3 < DISTANCE_THRESH_2)))
        return 1;
    }
  }
  return 0; 
}

//Determines whether two angles in radians are within a certain distance of each other
bool anglesAreClose(double angle1, double angle2)
{
  //Make sure that the angles are within the range of 0 to 2*pi
  while(angle1<0)
    angle1+=2*M_PI;
  while(angle2<0)
    angle2+=2*M_PI;
  while(angle1>2*M_PI)
    angle1-=2*M_PI;
  while(angle2>2*M_PI)
    angle2-=2*M_PI;

  //Check to see if the angles are currently lined up
  if ((angle2>(angle1-CLOSE_ANGLE_THRESH))&&(angle2<(angle1+CLOSE_ANGLE_THRESH)))
    return 1;

  if ((fabs(angle1-angle2)<(M_PI+CLOSE_ANGLE_THRESH))&&(fabs(angle1-angle2)>(M_PI-CLOSE_ANGLE_THRESH)))
    return 1;

  return 0;
}

void searchState::load_state_curves_list()
{
  _cList.clear();
  unsigned size = _Contours.size();
  for(unsigned i=0; i < size; i++)
    _cList.push_back(_Contours[i]);
  assert(_cList.size() == transform_list_.size());
  for(unsigned j=0; j < size; j++)
  {
    _cList[j].transform_ = transform_list_[j];
    vnl_matrix_fixed<double,3,3> trans = transform_list_[j];
    if(trans.is_identity() == false)
    {
      bfrag_curve *c = &(_cList[j]);

      for(unsigned i=0; i < c->num_coarse_points(); i++)
      {
        double x = c->level2_[i].x();
        double y = c->level2_[i].y();
        double trans_x = x * trans(0,0) + y * trans(0,1) + trans(0,2);
        double trans_y = x * trans(1,0) + y * trans(1,1) + trans(1,2);
        c->level2_[i].set(trans_x, trans_y);
      }
      for(unsigned i=0; i < c->num_fine_points(); i++)
      {
        double x = c->level3_[i].x();
        double y = c->level3_[i].y();
        double trans_x = x * trans(0,0) + y * trans(0,1) + trans(0,2);
        double trans_y = x * trans(1,0) + y * trans(1,1) + trans(1,2);
        c->level3_[i].set(trans_x, trans_y);
      }
      for(unsigned i=0; i < c->num_orig_points(); i++)
      {
        double x = c->level4_[i].x();
        double y = c->level4_[i].y();
        double trans_x = x * trans(0,0) + y * trans(0,1) + trans(0,2);
        double trans_y = x * trans(1,0) + y * trans(1,1) + trans(1,2);
        c->level4_[i].set(trans_x, trans_y);
      }
      c->update_angles(-rot_ang_list_[j]);
      c->compute_box();
//      c->compute_properties();
    }
  }
}
