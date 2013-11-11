// This is /lemsvxl/contrib/firat/dbul_octave/test.cxx

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 17, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#include<dbul/dbul_octave.h>
#include<vcl_cstdio.h>
#include<bxml/bxml_read.h>

int main()
{
    dbul_octave_value pos_tree;
    bxml_document pos_doc = bxml_read("/home/firat/Desktop/b.xml");
    bxml_data_sptr pos_root_xml = pos_doc.root_element();
    dbul_octave_convert_xml_to_octave_value(pos_root_xml, pos_tree);
    dbul_octave_argument_list inargs;
    dbul_octave_argument_list outargs;
    inargs(0) = pos_tree;
    dbul_octave.run("/home/firat/Desktop","deneme", inargs, outargs);
}
