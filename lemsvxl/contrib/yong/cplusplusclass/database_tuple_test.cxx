#include <vul/vul_arg.h>
#include <cstring> // needed for strcmp()
#include <iostream>
#include <iomanip>
#include <list>
#include <algorithm>
#include <vul/vul_file.h>
#include <vul/vul_sequence_filename_map.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <dbpro/dbpro_db_value.h>
#include <dbpro/dbpro_db_relation.h>


#if 0



int main(int argc, char* argv[])
{
  std::cout << "Registered Database Value Types:" << std::endl;

  dbpro_db_value_t<float> float_examplar(2.3f);
  dbpro_db_value_t<int> float_examplar1(true);
  dbpro_db_value_t<float> float_examplar3(2.34f);

  std::map<std::string, const dbpro_db_value*>::const_iterator itr = dbpro_db_value::registry().begin();
  bool registration_test = true;
  for( ; itr != dbpro_db_value::registry().end(); ++itr){
    std::cout << "   " << itr->first << std::endl;
    registration_test = (itr->first == itr->second->is_a()) && registration_test;
  }

  if(registration_test)
  {
    std::cout << "registration test pass" << std::endl;
  }

  dbpro_db_value_t<float> float_val(2.3f);
  float f = float_val.value();

  if(f == float_val && float_val == f)
  {
    std::cout << "assigning dpro_db_value_t<float> and == operator pass." << std::endl;
  }


  if(float_val.is_a() == std::string("float"))
  {
      std::cout << "is_a() test pass." << std::endl;
  }

  if(float_val.type() == std::string("float"))
  {
      std::cout << "type() test pass." << std::endl;
  }

  dbpro_db_value& base_val = float_val;

  int i = 1;
  if( base_val.assign(dbpro_db_value_t<int>(10)) )
  {
    std::cout << "invalid assign test pass" << std::endl;
  }

  if(base_val.assign(dbpro_db_value_t<float>(f)))
  {
    std::cout << "Valid assign test pass" << std::endl;
  }

  float f1 = 0.1f;
  float_val = 1.0f;
  if(!(base_val < dbpro_db_value_t<float>(f1)))
  {
    std::cout << "Operator < test pass" << std::endl;
  }
  if(base_val > dbpro_db_value_t<float>(f1))
  {
    std::cout << "Operator > test pass" << std::endl;
  }
  if(!(base_val == dbpro_db_value_t<float>(f1)))
  {
    std::cout << "Operator == test pass" << std::endl;
  }
  if(base_val != dbpro_db_value_t<float>(f1))
  {
    std::cout << "Operator != test pass" << std::endl;
  }

/*
  // why this code can pass compilation?

  dbpro_db_value_t<float> float_val_2(3.4f);
  if((float_val_2 + float_val) == (2.3+3.4))
  {
      std::cout << "value() test pass." << std::endl;
  }
*/

  // NOTE: Detected memory leaks!

  return 1;
}

#endif

