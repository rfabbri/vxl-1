#include <vul/vul_arg.h>
#include <vcl_cstring.h> // needed for strcmp()
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_list.h>
#include <vcl_algorithm.h>
#include <vul/vul_file.h>
#include <vul/vul_sequence_filename_map.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_string.h>
#include <vcl_cstdio.h>
#include <dbpro/dbpro_db_value.h>
#include <dbpro/dbpro_db_tuple.h>
#include <dbpro/dbpro_db_relation.h>


#if 0



int main(int argc, char* argv[])
{
  vcl_cout << "Registered Database Value Types:" << vcl_endl;

  dbpro_db_value_t<float> float_examplar(2.3f);
  dbpro_db_value_t<int> float_examplar1(true);
  dbpro_db_value_t<float> float_examplar3(2.34f);

  vcl_map<vcl_string, const dbpro_db_value*>::const_iterator itr = dbpro_db_value::registry().begin();
  bool registration_test = true;
  for( ; itr != dbpro_db_value::registry().end(); ++itr){
    vcl_cout << "   " << itr->first << vcl_endl;
    registration_test = (itr->first == itr->second->is_a()) && registration_test;
  }

  if(registration_test)
  {
    vcl_cout << "registration test pass" << vcl_endl;
  }

  dbpro_db_value_t<float> float_val(2.3f);
  float f = float_val.value();

  if(f == float_val && float_val == f)
  {
    vcl_cout << "assigning dpro_db_value_t<float> and == operator pass." << vcl_endl;
  }


  if(float_val.is_a() == vcl_string("float"))
  {
      vcl_cout << "is_a() test pass." << vcl_endl;
  }

  if(float_val.type() == vcl_string("float"))
  {
      vcl_cout << "type() test pass." << vcl_endl;
  }

  dbpro_db_value& base_val = float_val;

  int i = 1;
  if( base_val.assign(dbpro_db_value_t<int>(10)) )
  {
    vcl_cout << "invalid assign test pass" << vcl_endl;
  }

  if(base_val.assign(dbpro_db_value_t<float>(f)))
  {
    vcl_cout << "Valid assign test pass" << vcl_endl;
  }

  float f1 = 0.1f;
  float_val = 1.0f;
  if(!(base_val < dbpro_db_value_t<float>(f1)))
  {
    vcl_cout << "Operator < test pass" << vcl_endl;
  }
  if(base_val > dbpro_db_value_t<float>(f1))
  {
    vcl_cout << "Operator > test pass" << vcl_endl;
  }
  if(!(base_val == dbpro_db_value_t<float>(f1)))
  {
    vcl_cout << "Operator == test pass" << vcl_endl;
  }
  if(base_val != dbpro_db_value_t<float>(f1))
  {
    vcl_cout << "Operator != test pass" << vcl_endl;
  }

/*
  // why this code can pass compilation?

  dbpro_db_value_t<float> float_val_2(3.4f);
  if((float_val_2 + float_val) == (2.3+3.4))
  {
      vcl_cout << "value() test pass." << vcl_endl;
  }
*/

  // NOTE: Detected memory leaks!

  return 1;
}

#endif

