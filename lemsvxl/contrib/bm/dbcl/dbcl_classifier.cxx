//this is contrib/bm/dbcl/dbcl_classifier.cxx

#include"dbcl_classifier.h"

//SETTERS

void dbcl_classifier::add_feature( dbcl_temporal_feature_sptr tf, feature_map_type& fm )
{
    unsigned time = tf->time();

    vcl_pair<feature_map_type::iterator,bool> ret;

    ret = fm.insert(vcl_pair<unsigned,dbcl_temporal_feature_sptr>(time,tf));

    if( ret.second == false )
    {
        fm.erase(ret.first);
        fm.insert(vcl_pair<unsigned, dbcl_temporal_feature_sptr>(time,tf));
    }

}//end dbcl_classifier::add_feature

void dbcl_classifier::add_training_feature( dbcl_temporal_feature_sptr tf )
{
    this->add_feature(tf,this->training_data_);
}//end dbcl_classifier::add_training_feature

void dbcl_classifier::add_test_feature( dbcl_temporal_feature_sptr tf )
{
    this->add_feature(tf,this->test_data_);
}//end dbcl_classifier::add_test_feature

bool dbcl_classifier::set_label( unsigned const& time, unsigned& label, feature_map_type& fm )
{
    feature_map_type::iterator it = fm.find(time);

    if( it != fm.end() )
    {
        it->second->set_label(label);
        return true;
    }
    else
        return false;
}//end dbcl_classifier::set_label

bool dbcl_classifier::set_training_label( unsigned const& time, unsigned& label )
{
    if( !this->set_label(time,label,this->training_data_) )
    {
        vcl_cerr << "ERROR dbcl_classifier::set_training_label training feature with time stamp " << time << " not found." << vcl_flush;
        return false;
    }
    else
        return true;

}//end dbcl_classifier::set_training_label

bool dbcl_classifier::set_test_label( unsigned const& time, unsigned& label )
{
    if( !this->set_label(time,label,this->test_data_) )
    {
        vcl_cerr << "ERROR dbcl_classifier::set_test_label training feature with time stamp " << time << " not found." << vcl_flush;
        return false;
    }
    else
        return true;
}//end dbcl_classifier::set_test_label

//GETTERS

bool dbcl_classifier::label( unsigned const& time, unsigned& label, feature_map_type& fm )
{
    feature_map_type::iterator it = fm.find(time);

    if( it != fm.end() )
    {
        label = it->second->label();
        return true;
    }
    else
        return false;
}//end dbcl_classifier::label

bool dbcl_classifier::training_label( unsigned const& time, unsigned& label )
{
    if( this->label(time,label,this->training_data_) )
        return true;
    else
    {
        vcl_cerr << "ERROR dbcl_classifier::training_label training feature with time stamp " << time << " not found." << vcl_flush;
        return false;
    }
}//end dbcl_classifier::training_label

bool dbcl_classifier::test_label( unsigned const& time, unsigned& label )
{
        if( this->label(time,label,this->test_data_) )
        return true;
    else
    {
        vcl_cerr << "ERROR dbcl_classifier::test_label training feature with time stamp " << time << " not found." << vcl_flush;
        return false;
    }
}//end dbcl_classifier::test_data

bool dbcl_classifier::feature_vector( unsigned const& time, dbcl_temporal_feature::feature_vector_type& feature_vector, feature_map_type& fm )
{
    feature_map_type::iterator it = fm.find(time);

    if( it != fm.end() )
    {
        feature_vector = it->second->feature_vector();
        return true;
    }
    else
        return false;
}//end dbcl_classifier::feature_vector

bool dbcl_classifier::training_feature_vector( unsigned const& time, dbcl_temporal_feature::feature_vector_type& feature_vector )
{
    if( this->feature_vector(time,feature_vector,this->training_data_) )
        return true;
    else
    {
        vcl_cerr << "ERROR dbcl_classifier::training_feature_vector training feature with time stamp " << time << " not found." << vcl_flush;
        return false;
    }
}//end dbcl_classifier::training_feature_vector

bool dbcl_classifier::test_feature_vector( unsigned const& time, dbcl_temporal_feature::feature_vector_type& feature_vector )
{
    if( this->feature_vector(time,feature_vector,this->test_data_) )
        return true;
    else
    {
        vcl_cerr << "ERROR dbcl_classifier::test_feature_vector training feature with time stamp " << time << " not found." << vcl_flush;
        return false;
    }
}//end dbcl_classifier::test_feature_vector

bool dbcl_classifier::feature_label( unsigned const& time, dbcl_temporal_feature::feature_vector_type& feature_vector, unsigned& label, feature_map_type& fm)
{
    feature_map_type::iterator it = fm.find(time);

    if( it != fm.end() )
    {
        feature_vector = it->second->feature_vector();
        label = it->second->label();
        return true;
    }
    else
        return false;
}//end dbcl_classifier::feature_label

bool dbcl_classifier::training_feature_label( unsigned const& time, dbcl_temporal_feature::feature_vector_type& feature_vector, unsigned& label )
{
    if( this->feature_label(time,feature_vector,label,this->training_data_) )
        return true;
    else
    {
        vcl_cerr << "ERROR dbcl_classifier::training_feature_label training feature with time stamp " << time << " not found." << vcl_flush;
        return false;
    }
}//end dbcl_classifier::training_feature_label

bool dbcl_classifier::test_feature_label( unsigned const& time, dbcl_temporal_feature::feature_vector_type& feature_vector, unsigned& label )
{
    if( this->feature_label(time,feature_vector,label,this->test_data_) )
        return true;
    else
    {
        vcl_cerr << "ERROR dbcl_classifier::test_feature_label training feature with time stamp " << time << " not found." << vcl_flush;
        return false;
    }
}//end dbcl_classifier::training_feature_label