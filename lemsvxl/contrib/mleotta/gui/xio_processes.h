// This is contrib/mleotta/gui/xio_processes.h
#ifndef xio_processes_h_
#define xio_processes_h_

//:
// \file
// \brief Processes to read and write data in XML streams
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/17/06
//
// \verbatim
//  Modifications
// \endverbatim



#include <dbpro/xio/dbpro_xio_sink.h>
#include <dbpro/xio/dbpro_xio_source.h>
#include <rgrl/rgrl_feature.h>
#include <vidreg/vidreg_feature_edgel.h>
#include <vnl/vnl_double_2.h>
#include <vcl_vector.h>
#include <vcl_utility.h>


class vidreg_xml_write_tracks_filter : public dbpro_filter
{
  public:
    //: Execute this process
    dbpro_signal execute()
    {
      typedef vcl_vector<vcl_pair<unsigned long, rgrl_feature_sptr> > Tvec;
      assert(input_type_id(0) == typeid(Tvec));
      Tvec tracks = input<Tvec >(0);

      bxml_element* frame = new bxml_element("frame");

      bxml_element* track_el = new bxml_element("track");
      track_el->set_attribute("type","edgel");
      frame->append_data(track_el);
      frame->append_text("\n");
      track_el->append_text("\n");
      for(Tvec::const_iterator ti = tracks.begin(); ti!=tracks.end(); ++ti){
        bxml_element* feature = NULL;
        if(const vidreg_feature_edgel* e =
            dynamic_cast<const vidreg_feature_edgel*>(ti->second.ptr()))
        {
          feature = new bxml_element("edgel");
          feature->set_attribute("x",e->location()[0]);
          feature->set_attribute("y",e->location()[1]);
          double angle = vcl_atan2(e->normal()[1],e->normal()[0]);
          feature->set_attribute("n",angle);
          feature->set_attribute("mag",e->magnitude());
        }
        else if(const rgrl_feature_point* p =
            dynamic_cast<const rgrl_feature_point*>(ti->second.ptr()))
        {
          feature = new bxml_element("point");
          feature->set_attribute("x",p->location()[0]);
          feature->set_attribute("y",p->location()[1]);
        }
        else
          return DBPRO_INVALID;

        feature->set_attribute("track_id",ti->first);
        track_el->append_data(feature);
        track_el->append_text("\n");
      }

      output(0, bxml_data_sptr(frame));
      return DBPRO_VALID;
    }
};


class vidreg_xml_read_tracks_filter : public dbpro_filter
{
  public:
    typedef vcl_vector<vcl_pair<unsigned long, rgrl_feature_sptr> > Tvec;
    //: Execute this process
    dbpro_signal execute()
    {
      assert(input_type_id(0) == typeid(bxml_data_sptr));
      bxml_data_sptr data = input<bxml_data_sptr >(0);

      if(!data || data->type() != bxml_data::ELEMENT)
        return DBPRO_INVALID;

      bxml_element* frame = static_cast<bxml_element*>(data.ptr());
      assert(frame->name() == "frame");

      for(bxml_element::const_data_iterator i = frame->data_begin();
          i != frame->data_end();  ++i)
      {
        if((*i)->type() != bxml_data::ELEMENT)
          continue;

        bxml_element* track_el = static_cast<bxml_element*>(i->ptr());
        assert(track_el->name() == "track");
        if(track_el->attribute("type") == "edgel"){
          Tvec tracks = read_edgels(track_el);
          vcl_cout << "read "<<tracks.size() << " tracks" <<vcl_endl;
          output(0, tracks);
          return DBPRO_VALID;
        }
        else if(track_el->attribute("type") == "point"){
          Tvec tracks = read_points(track_el);
          output(0, tracks);
          return DBPRO_VALID;
        }
      }

      return DBPRO_INVALID;
    }

    Tvec read_edgels(const bxml_element* track_el)
    {
      Tvec tracks;
      for(bxml_element::const_data_iterator i = track_el->data_begin();
          i != track_el->data_end();  ++i)
      {
        if((*i)->type() != bxml_data::ELEMENT)
          continue;
        bxml_element* e = static_cast<bxml_element*>(i->ptr());
        assert(e->name() == "edgel");
        unsigned long id;
        e->get_attribute("track_id",id);
        double x,y,n,mag;
        e->get_attribute("x",x);
        e->get_attribute("y",y);
        e->get_attribute("n",n);
        e->get_attribute("mag",mag);
        vidreg_feature_edgel* edgel = new vidreg_feature_edgel(vnl_double_2(x,y),
                                                               vnl_double_2(vcl_cos(n),vcl_sin(n)),
                                                               mag);
        tracks.push_back(vcl_pair<unsigned long, rgrl_feature_sptr>(id,edgel));
      }
      return tracks;
    }

    Tvec read_points(const bxml_element* track_el)
    {
      Tvec tracks;
      for(bxml_element::const_data_iterator i = track_el->data_begin();
          i != track_el->data_end();  ++i)
      {
        if((*i)->type() != bxml_data::ELEMENT)
          continue;
        bxml_element* e = static_cast<bxml_element*>(i->ptr());
        assert(e->name() == "point");
        unsigned long id;
        e->get_attribute("track_id",id);
        double x,y;
        e->get_attribute("x",x);
        e->get_attribute("y",y);
        rgrl_feature_point* edgel = new rgrl_feature_point(vnl_double_2(x,y));
        tracks.push_back(vcl_pair<unsigned long, rgrl_feature_sptr>(id,edgel));
      }
      return tracks;
    }
};


class vidreg_xml_write_xform_filter : public dbpro_filter
{
  public:
    //: Execute this process
    dbpro_signal execute()
    {
      assert(input_type_id(0) == typeid(vcl_vector<vidreg_salient_group_sptr>));
      vcl_vector<vidreg_salient_group_sptr> groups
          = input<vcl_vector<vidreg_salient_group_sptr> >(0);

      bxml_element* frame = new bxml_element("frame");
      for(unsigned int i=0; i<groups.size(); ++i){
        bxml_element* xform = new bxml_element("transform");
        frame->append_data(xform);
        xform->set_attribute("type","similarity");
        xform->set_attribute("dimension","2");
        xform->set_attribute("name",i);
        vcl_stringstream s;
        s << *(groups[i]->view()->xform_estimate());
        xform->append_text(s.str());
      }

      output(0, bxml_data_sptr(frame));
      return DBPRO_VALID;
    }
};


class vidreg_xml_read_xform_filter : public dbpro_filter
{
  public:
    //: Execute this process
    dbpro_signal execute()
    {
      assert(input_type_id(0) == typeid(bxml_data_sptr));
      bxml_data_sptr data = input<bxml_data_sptr >(0);

      if(!data || data->type() != bxml_data::ELEMENT)
        return DBPRO_INVALID;

      bxml_element* frame = static_cast<bxml_element*>(data.ptr());
      assert(frame->name() == "frame");


      vcl_vector<rgrl_transformation_sptr> results;
      for(bxml_element::const_data_iterator i = frame->data_begin();
          i != frame->data_end();  ++i)
      {
        if((*i)->type() != bxml_data::ELEMENT)
          continue;
        bxml_element* xform = static_cast<bxml_element*>(i->ptr());
        assert(xform->name() == "transform");
        assert(xform->num_data() > 0);
        assert((*xform->data_begin())->type() == bxml_data::TEXT);
        bxml_text* text = static_cast<bxml_text*>(xform->data_begin()->ptr());
        rgrl_transformation_sptr sim_xform = new rgrl_trans_similarity;
        vcl_stringstream s(text->data());
        sim_xform->read(s);
        results.push_back(sim_xform);
      }

      output(0, results);
      return DBPRO_VALID;
    }
};


#endif // xio_processes_h_
