#include "PuzzleSolving.h"
#include <bxml/bxml_document.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_read.h>
#include <stdio.h>

// writes experiment search state to a .sest file
// writes puzzle solving object to a .puso file

void PuzzleSolving::write_experiment_search_state_and_puzzle_solving_objects(vcl_string fname, vcl_vector<searchState> &states, bool write_matches)
{
  bxml_document doc;
  bxml_element *root = new bxml_element("root");
  doc.set_root_element(root);

  bxml_element *data = new bxml_element("num_states");
  //vcl_cout<<"hmmm1"<<vcl_endl;
  ////vcl_cout<<"states.size()"<<vcl_endl;
 // vcl_cout<<states.size()<<vcl_endl;
  data->set_attribute("value", states.size());
  root->append_data(data);

  for(unsigned i=0; i<states.size(); i++)
  {//Basically this always seems to be 1. i.e the soze of the vector
    vcl_stringstream dump_state_file_full_path;
    vcl_stringstream dump_state_file_relative_path;
	//vcl_cout<< states.size()<<vcl_endl;
    dump_state_file_full_path << fname << "//state" << i << ".sest";
    dump_state_file_relative_path << "state" << i << ".sest";

    bxml_element *data = new bxml_element("state");
    data->set_attribute("dump_filename", dump_state_file_relative_path.str());
    root->append_data(data);
    // dump the search state to file
    states[i].write_out(dump_state_file_full_path.str());
	//vcl_cout<<dump_state_file_full_path.str()<<vcl_endl;
    // populate the state element more here
  }

  vcl_stringstream dump_puzzle_solving_object_file_full_path;
  vcl_stringstream dump_puzzle_solving_object_file_relative_path;
  dump_puzzle_solving_object_file_full_path << fname << "//puzzlesolving.puso";
  dump_puzzle_solving_object_file_relative_path << "puzzlesolving.puso";

  bxml_element *data2 = new bxml_element("puzzle_solving");
  data2->set_attribute("dump_filename", dump_puzzle_solving_object_file_relative_path.str());
  root->append_data(data2);
  // dump the puzzle solving object to file
  this->write_out(dump_puzzle_solving_object_file_full_path.str());

  vcl_stringstream dump_matches_file_full_path;
  vcl_stringstream dump_matches_file_relative_path;
  dump_matches_file_full_path << fname << "//matches.mtc";
  dump_matches_file_relative_path << "matches.mtc";

  bxml_element *data3 = new bxml_element("matches");
  data3->set_attribute("dump_filename", dump_matches_file_relative_path.str());
  root->append_data(data3);
  if(write_matches == true)
    this->write_out_matches(dump_matches_file_full_path.str());

  vcl_stringstream dump_frags_file_full_path;
  vcl_stringstream dump_frags_file_relative_path;
  dump_frags_file_full_path << fname << "//frags.frg";//Changed order of slashes!!!!!!!!
  dump_frags_file_relative_path << "frags.frg";

  bxml_element *data4 = new bxml_element("frags");
  data4->set_attribute("dump_filename", dump_frags_file_relative_path.str());
  root->append_data(data4);
  this->write_out_frags(dump_frags_file_full_path.str());

  vcl_stringstream output_name;
  output_name << fname << "//output.xml";//Changed order of slashes!!!!!!!!
//vcl_cout<<output_name.str()+"ppppp"<<vcl_endl;
  bxml_write(output_name.str(), doc);
}

void PuzzleSolving::read_experiment_search_state_and_puzzle_solving_objects(vcl_string fname, vcl_vector<searchState> &states, bool read_matches)
{
  vcl_stringstream temp;
  
  temp << fname << "//output.xml";//Changed order of slashes!!!!!!!!
  
  vcl_string fname2 = temp.str();
  // open input file
  //vcl_cout<<fname2+"this is the output file path"<<vcl_endl;
  //vcl_cout << fname2.c_str() << vcl_endl;
  FILE *fp = fopen(fname2.c_str(), "r");
  vcl_ifstream in(fname2.c_str());
  vcl_ifstream in2(fname2.c_str());
  // create xml reader
  bxml_stream_read reader, reader2;
  bxml_data_sptr data;
  unsigned int depth;
  // get the number of states
  unsigned num_states;
  //vcl_cout<<"reached here"<<vcl_endl;
  while(data = reader2.next_element(in2, depth))
  {
	  vcl_cout << "entered this loop" <<vcl_endl;
    bxml_element* elem = static_cast<bxml_element*>(data.ptr());
	vcl_cout << "elem name: " << elem->name();
    if(elem->name() == "num_states")
    {
      elem->get_attribute("value", num_states);
      break;
    }
  }
  states.resize(num_states);
  unsigned i=0;
  while(data = reader.next_element(in, depth))
  {
    bxml_element* elem = static_cast<bxml_element*>(data.ptr());
    if(elem->name() == "state")
    {
      vcl_string filename;
      elem->get_attribute("dump_filename", filename);
      vcl_string full_path = fname;
      full_path.append("//"); //changed slashes
      full_path.append(filename.c_str());
      states[i].read_in(full_path);
      i++;
    }
    else if(elem->name() == "puzzle_solving")
    {
      vcl_string filename;
      elem->get_attribute("dump_filename", filename);
      vcl_string full_path = fname;
      full_path.append("//");
      full_path.append(filename.c_str());
      this->read_in(full_path);
    }
    else if(elem->name() == "matches")
    {
      vcl_string filename;
      elem->get_attribute("dump_filename", filename);
      vcl_string full_path = fname;
      full_path.append("//");
      full_path.append(filename.c_str());
      if(read_matches == true && _matches.size() == 0)
        this->read_in_matches(full_path);
    }
    else if(elem->name() == "frags")
    {
      vcl_string filename;
      elem->get_attribute("dump_filename", filename);
      vcl_string full_path = fname;
      full_path.append("//");
      full_path.append(filename.c_str());
      this->read_in_frags(full_path);
    }
  }
  in.close();
  in2.close();
}

void PuzzleSolving::write_out(vcl_string fname)
{
  unsigned size;
  vcl_ofstream out(fname.c_str());

  out << "START_DUMPING_PUZZLE_SOLVING_OBJECT" << vcl_endl;

  size = _pairMatchesByPiece.size();
  out << "_pairMatchesByPiece " << size << vcl_endl;
  for(unsigned i=0; i < size; i++)
  {
    unsigned size2 = _pairMatchesByPiece[i].size();
    out << size2 << vcl_endl;
    for(unsigned j=0; j < size2; j++)
      out << _pairMatchesByPiece[i][j] << " ";
    out << vcl_endl;
  }

  size = _Contours.size();
  out << "_Contours " << size << vcl_endl;
  for(unsigned i=0; i < size; i++)
   _Contours[i].write_out(out);//}
/*out << "_cList " << size << vcl_endl;
  for(unsigned i=0; i < size; i++)
   _cList[i].write_out(out);*/
  out << "_nPieces: " << _nPieces << vcl_endl;

  out << "tot_num_iters_: " << tot_num_iters_ << vcl_endl;

  out << "END_DUMPING_PUZZLE_SOLVING_OBJECT" << vcl_endl;
}

void PuzzleSolving::read_in(vcl_string fname)
{
  unsigned size;
  vcl_string dummy;
  vcl_ifstream in(fname.c_str());

  in >> dummy;
  assert(dummy == "START_DUMPING_PUZZLE_SOLVING_OBJECT");
  
  in >> dummy;
  assert(dummy == "_pairMatchesByPiece");
  in >> size;
  _pairMatchesByPiece.resize(size);
  for(unsigned i=0; i < size; i++)
  {
    unsigned size2;
    in >> size2;
    _pairMatchesByPiece[i].resize(size2);
    for(unsigned j=0; j < size2; j++)
      in >> _pairMatchesByPiece[i][j];
  }

  in >> dummy;
  assert(dummy == "_Contours");
  in >> size;
  _Contours.resize(size);
  for(unsigned i=0; i<size; i++)
    _Contours[i].read_in(in);
  in >> dummy;
  assert(dummy == "_nPieces:");
  in >> _nPieces;

  in >> dummy;
  assert(dummy == "tot_num_iters_:");
  in >> tot_num_iters_;
}

void PuzzleSolving::write_out_matches(vcl_string fname)//Creates the matches.mtc file.
{
  unsigned size;
  vcl_ofstream out(fname.c_str());

  out << "START_DUMPING_MATCHES" << vcl_endl;
  
  size = _matches.size();
  out << "_matches " << size << vcl_endl;
  for(unsigned i=0; i < size; i++)
    _matches[i].write_out(out);
}

void PuzzleSolving::write_out_frags(vcl_string fname)
{
  unsigned size;
  vcl_ofstream out(fname.c_str());

  out << "START_DUMPING_FRAGS" << vcl_endl;
  
  size = _Contours.size();
  out << "_cList " << size << vcl_endl;
  for(unsigned i=0; i < size; i++)
    _Contours[i].write_out(out);//i've changed this from contours to clist. lemme see if it helps.
}

void PuzzleSolving::read_in_matches(vcl_string fname)
{
  unsigned size;
  vcl_string dummy;
  vcl_ifstream in(fname.c_str());

  in >> dummy;
  assert(dummy == "START_DUMPING_MATCHES");
  
  in >> dummy;
  assert(dummy == "_matches");
  in >> size;
  _matches.resize(size);
  for(unsigned i=0; i<size; i++)
    _matches[i].read_in(in);

  // CAN: Do not take all matches, eliminate some of them by taking a top number of them
  // Do this only at the first iteration if necessary
  if(this->tot_num_iters_ == 0 && TOP_MATCHES_TO_KEEP < _matches.size())
  {
    vcl_sort( _matches.begin(), _matches.end(), pairwiseMatchSort2() );
    _matches.erase(_matches.begin()+TOP_MATCHES_TO_KEEP, _matches.end());
  }
}

void PuzzleSolving::read_in_frags(vcl_string fname)
{
  unsigned size;
  vcl_string dummy;
  vcl_ifstream in(fname.c_str());

  in >> dummy;
  assert(dummy == "START_DUMPING_FRAGS");
  
  in >> dummy;
  assert(dummy == "_cList");
  in >> size;
  _Contours.resize(size);
  for(unsigned i=0; i<size; i++)
    _Contours[i].read_in(in);
}

void PuzzleSolving::write_frag_assemblies_in_xml(vcl_string fname, vcl_vector<searchState> &states)
{
  if(fname.size() > 0)
  {
	 // vcl_cout<<"see this does work"<<vcl_endl;
	  //vcl_cout<<states.size()<<vcl_endl;
    bxml_document doc;
    bxml_element *root = new bxml_element("fragment_assembly");
    doc.set_root_element(root);

    for(unsigned j=0; j < states.size(); j++)
    {
      if(states[j].process.size() > 0)
      {
        bxml_element *data = new bxml_element("assembly");
        for(unsigned i=0; i < states[j].process.size(); i++)
        {
          vcl_stringstream idname;
          vcl_stringstream posename;
          idname << "id" << i;
          posename << "pose" << i;
          bfrag_curve *c = &(_cList[states[j].process[i]]);
          data->set_attribute(idname.str(), c->frag_id_);
          vnl_matrix_fixed<double,3,3> transform = states[j].transform_list_[states[j].process[i]];
          char pose[10000];
          sprintf(pose, "%f, %f, %f, %f, %f, %f, %f, %f, %f", 
                  transform(0,0), transform(0,1), transform(0,2), 
                  transform(1,0), transform(1,1), transform(1,2), 
                  transform(2,0), transform(2,1), transform(2,2));
          data->set_attribute(posename.str(), pose);
        }
        data->set_attribute("cost", states[j].tCost);
        root->append_data(data);
      }
    }
	//Added by Anant
//vcl_stringstream ffname;
	//vcl_cout<< states.size()<<vcl_endl;
    bxml_write(fname.c_str(), doc);
  }
}

void searchState::write_out(vcl_string fname)
{
  unsigned size;
  vcl_ofstream out(fname.c_str());

  out << "START_DUMPING_SEARCH_STATE" << vcl_endl;
  
//  size = _cList.size();
//  out << "_cList " << size << vcl_endl;
//  for(unsigned i=0; i < size; i++)
//    _cList[i].write_out(out);

//  size = _matches.size();
//  out << "_matches " << size << vcl_endl;
//  for(unsigned i=0; i < size; i++)
//    _matches[i].write_out(out);

  size = transform_list_.size();
  out << "transform_list_ " << size << vcl_endl;
  for(unsigned i=0; i < size; i++)
  {
    vnl_matrix_fixed<double,3,3> transform = transform_list_[i];
    out << transform(0,0) << " " << transform(0,1) << " " << transform(0,2) << " " <<
           transform(1,0) << " " << transform(1,1) << " " << transform(1,2) << " " <<
           transform(2,0) << " " << transform(2,1) << " " << transform(2,2) << vcl_endl;
  }

  size = rot_ang_list_.size();
  out << "rot_ang_list_ " << size << vcl_endl;
  for(unsigned i=0; i < size; i++)
    out << rot_ang_list_[i] << " ";
  out << vcl_endl;

  size = matches_ez_list_.size();
  out << "matches_ez_list_ " << size << vcl_endl;
  for(unsigned i=0; i < size; i++)
    out << matches_ez_list_[i].first << " " << matches_ez_list_[i].second << vcl_endl;

  out << "_numMatch: " << _numMatch << vcl_endl;
  out << "_num_new: " << _num_new << vcl_endl;

  out << "_merged" << vcl_endl;
  _merged.write_out(out);

  size = _constr.size();
  out << "_constr " << size << vcl_endl;
  for(unsigned i=0; i < size; i++)
    _constr[i].write_out(out);

  out << "active: " << active << vcl_endl;

  size = process.size();
  out << "process " << size << vcl_endl;
  for(unsigned i=0; i < size; i++)
    out << process[i] << vcl_endl;

  out << "nProcess: " << nProcess << vcl_endl;
  out << "tCost: " << tCost << vcl_endl;
  out << "sCost: " << sCost << vcl_endl;

  size = open_junc.size();
  out << "open_junc " << size << vcl_endl;
  for(unsigned i=0; i < size; i++)
  {
    vcl_vector<int> temp = open_junc[i].first;
    out << temp.size() << " / ";
	//vcl_cout<<"wow i do come here"<<vcl_endl;
    for(unsigned j=0; j < temp.size(); j++)
      out << temp[j] << " ";
    out << " / ";
    out << open_junc[i].second.x() << " " << open_junc[i].second.y() << vcl_endl;
  }

  size = closed_junc.size();
  out << "closed_junc " << size << vcl_endl;
  for(unsigned i=0; i < size; i++)
  {
    vcl_vector<int> temp = closed_junc[i].first;
    out << temp.size() << " / ";
    for(unsigned j=0; j < temp.size(); j++)
      out << temp[j] << " ";
    out << " / ";
    out << closed_junc[i].second.x() << " " << closed_junc[i].second.y() << vcl_endl;
  }

  size = old_edges.size();
  out << "old_edges " << size << vcl_endl;
  for(unsigned i=0; i < size; i++)
    out << old_edges[i].first << " " << old_edges[i].second << vcl_endl;

  size = new_edges.size();
  out << "new_edges " << size << vcl_endl;
  for(unsigned i=0; i < size; i++)
    out << new_edges[i].first << " " << new_edges[i].second << vcl_endl;

  out << "new_edge: " << new_edge.first << " " << new_edge.second << vcl_endl;

  out << "END_DUMPING_SEARCH_STATE" << vcl_endl;

}

void searchState::read_in(vcl_string fname)
{
  unsigned size;
  vcl_string dummy;
  vcl_ifstream in(fname.c_str());

  in >> dummy;
  assert(dummy == "START_DUMPING_SEARCH_STATE");
  
//  in >> dummy;
//  assert(dummy == "_cList");
//  in >> size;
//  _cList.resize(size);
//  for(unsigned i=0; i<size; i++)
//    _cList[i].read_in(in);

//  in >> dummy;
//  assert(dummy == "_matches");
//  in >> size;
//  _matches.resize(size);
//  for(unsigned i=0; i<size; i++)
//    _matches[i].read_in(in);

  state_id_ = -1;

  in >> dummy;
  assert(dummy == "transform_list_");
  in >> size;
  transform_list_.resize(size);
  for(unsigned i=0; i<size; i++)
  {
    vnl_matrix_fixed<double,3,3> transform;
    in >> transform(0,0); in >> transform(0,1); in >> transform(0,2);
    in >> transform(1,0); in >> transform(1,1); in >> transform(1,2);
    in >> transform(2,0); in >> transform(2,1); in >> transform(2,2);
    transform_list_[i] = transform;
  }

  in >> dummy;
  assert(dummy == "rot_ang_list_");
  in >> size;
  rot_ang_list_.resize(size);
  for(unsigned i=0; i < size; i++)
    in >> rot_ang_list_[i];

  in >> dummy;
  assert(dummy == "matches_ez_list_");
  in >> size;
  matches_ez_list_.resize(size);
  for(unsigned i=0; i<size; i++)
  {
    in >> matches_ez_list_[i].first;
    in >> matches_ez_list_[i].second;
  }

  in >> dummy;
  assert(dummy == "_numMatch:");
  in >> _numMatch;

  in >> dummy;
  assert(dummy == "_num_new:");
  in >> _num_new;

  in >> dummy;
  assert(dummy == "_merged");
  _merged.read_in(in);

  in >> dummy;
  assert(dummy == "_constr");
  in >> size;
  _constr.resize(size);
  for(unsigned i=0; i<size; i++)
    _constr[i].read_in(in);

  in >> dummy;
  assert(dummy == "active:");
  in >> active;

  in >> dummy;
  assert(dummy == "process");
  in >> size;
  process.resize(size);
  for(unsigned i=0; i<size; i++)
    in >> process[i];

  in >> dummy;
  assert(dummy == "nProcess:");
  in >> nProcess;

  in >> dummy;
  assert(dummy == "tCost:");
  in >> tCost;

  in >> dummy;
  assert(dummy == "sCost:");
  in >> sCost;

  in >> dummy;
  assert(dummy == "open_junc");
  in >> size;
  open_junc.resize(size);
  for(unsigned i=0; i < size; i++)
  {
    unsigned temp_size;
    in >> temp_size;
    in >> dummy;
    vcl_vector<int> temp(temp_size);
    assert(dummy == "/");
    for(unsigned j=0; j < temp_size; j++)
      in >> temp[j];
    in >> dummy;
    assert(dummy == "/");
    open_junc[i].first = temp;
    in >> open_junc[i].second;
  }

  in >> dummy;
  assert(dummy == "closed_junc");
  in >> size;
  closed_junc.resize(size);
  for(unsigned i=0; i < size; i++)
  {
    unsigned temp_size;
    in >> temp_size;
    in >> dummy;
    vcl_vector<int> temp(temp_size);
    assert(dummy == "/");
    for(unsigned j=0; j < temp_size; j++)
      in >> temp[j];
    in >> dummy;
    assert(dummy == "/");
    closed_junc[i].first = temp;
    in >> closed_junc[i].second;
  }

  in >> dummy;
  assert(dummy == "old_edges");
  in >> size;
  old_edges.resize(size);
  for(unsigned i=0; i<size; i++)
    in >> old_edges[i].first >> old_edges[i].second;

  in >> dummy;
  assert(dummy == "new_edges");
  in >> size;
  new_edges.resize(size);
  for(unsigned i=0; i<size; i++)
    in >> new_edges[i].first >> new_edges[i].second;

  in >> dummy;
  assert(dummy == "new_edge:");
  in >> new_edge.first >> new_edge.second;

  in >> dummy;
  assert(dummy == "END_DUMPING_SEARCH_STATE");

  // CAN: Do not take all matches, eliminate some of them by taking a top number of them
  // Do this only at the first iteration if necessary
  if(this->nProcess == 0 && TOP_MATCHES_TO_KEEP < matches_ez_list_.size())
  {
	  vcl_cout<<"I came here"<<vcl_endl;
    vcl_sort( matches_ez_list_.begin(), matches_ez_list_.end(), pairwiseMatchSort() );
    matches_ez_list_.erase(matches_ez_list_.begin()+TOP_MATCHES_TO_KEEP, matches_ez_list_.end());
  }
}

void searchState::write_frag_pairs_in_xml(vcl_string fname)
{
  if(fname.size() > 0)
  {
    bxml_document doc;
    bxml_element *root = new bxml_element("fragment_pairs");
    doc.set_root_element(root);

    for(unsigned i=0; i<_matches.size(); i++)
    {
      pairwiseMatch match = _matches[i];
      bxml_element *data = new bxml_element("pair");
      data->set_attribute("frag_id_1", _cList[match.whichCurves.first].frag_id_);
      data->set_attribute("frag_id_2", _cList[match.whichCurves.second].frag_id_);
      data->set_attribute("cost", match.cost);
      char pose[1024];
      XForm3x3 xform = match.xForm;
      sprintf(pose, "%f, %f, %f, %f, %f, %f, %f, %f, %f", xform.vv[0], xform.vv[1], xform.vv[2],
        xform.vv[3], xform.vv[4], xform.vv[5], xform.vv[6], xform.vv[7], xform.vv[8]);
      data->set_attribute("pose", pose);
      root->append_data(data);
    }

    bxml_write(fname.c_str(), doc);
  }
}

void pairwiseMatch::write_out(vcl_ofstream &out)
{
  unsigned size;
  out << "START_DUMPING_PAIRWISE_MATCH" << vcl_endl;
  out << "cost: " << cost << vcl_endl;
  out << "myIndex: " << myIndex << vcl_endl;

  size = pointMap.size();
  out << "pointMap " << size << vcl_endl;
  for(unsigned i=0; i<size; i++)
    out << pointMap[i].first << " " << pointMap[i].second << vcl_endl;

  out << "XForm3x3" << vcl_endl;
  for(unsigned i=0; i<9; i++)
    out << xForm.vv[i] << " ";
  out << vcl_endl;
  out << xForm.transx << " " << xForm.transy << " " << xForm.rotAngle << vcl_endl;

  out << "whichCurves: " << whichCurves.first << " " << whichCurves.second << vcl_endl;
  out << "END_DUMPING_PAIRWISE_MATCH" << vcl_endl;
}

void pairwiseMatch::read_in(vcl_ifstream &in)
{
  unsigned size;
  vcl_string dummy;
  
  in >> dummy;
  assert(dummy == "START_DUMPING_PAIRWISE_MATCH");

  in >> dummy;
  assert(dummy == "cost:");
  in >> cost;

  in >> dummy;
  assert(dummy == "myIndex:");
  in >> myIndex;

  in >> dummy;
  assert(dummy == "pointMap");
  in >> size;
  pointMap.resize(size);
  for(unsigned i=0; i<size; i++)
    in >> pointMap[i].first >> pointMap[i].second;

  in >> dummy;
  assert(dummy == "XForm3x3");
  for(unsigned i=0; i<9; i++)
    in >> xForm.vv[i];
  in >> xForm.transx >> xForm.transy >> xForm.rotAngle;

  in >> dummy;
  assert(dummy == "whichCurves:");
  in >> whichCurves.first >> whichCurves.second;

  in >> dummy;
  assert(dummy == "END_DUMPING_PAIRWISE_MATCH");
}

