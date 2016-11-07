// This is breye/dbxml/examples/dbxml_example.cxx
//:
// \file

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <stdlib.h>
#include <dbxml/dbxml_io.h>
#include <dbxml/dbxml_generic_ptr.h>
#include <dbxml/dbxml_track.h>
#include <dbxml/dbxml_param.h>

static char*                    gXmlFile               = 0;

int main(int argC, char* argV[])
{
    int parmInd =1;
    dbxml_io dbxml_instance;
    dbxml_instance.register_input_converters(); 

    vcl_vector<dbxml_generic_ptr> object;

     if (parmInd + 1 != argC)
    {
         return 1;
    }
    /* file to parse*/
    gXmlFile = argV[parmInd];

    //
    //  Create our parser, then attach an error handler to the parser.
    //  The parser will call back to methods of the ErrorHandler if it
    //  discovers errors during the course of parsing the XML document.
    //
        if (dbxml_instance.parse_xml(gXmlFile,object)){
    // If the parse was successful, output the document data from the DOM tree
    
        for (vcl_vector<dbxml_generic_ptr>::iterator cit = object.begin();
                cit != object.end(); cit++){

                        void *p  = (*cit).get_anyobject();
                           vcl_cout                                << " startframe: " << ((dbxml_track *)p)->startframe_
           << " numframes: " << ((dbxml_track *)p)->numframes_
               << " width: " << ((dbxml_track *)p)->width_
               << " height: " << ((dbxml_track *)p)->height_
                                   << vcl_endl;
             vcl_vector<dbxml_generic_ptr> track = ((dbxml_track *)p)->tracks_;

                        for (vcl_vector<dbxml_generic_ptr>::iterator citt = track.begin();
                citt != track.end(); citt++){

                        void *p  = (*citt).get_anyobject();
                           vcl_cout                                << " x: " << ((dbxml_tracks *)p)->x_
           << " y: " << ((dbxml_tracks *)p)->y_
                                   << vcl_endl;
    }

        
    }
    /*
                for (vcl_vector<dbxml_generic_ptr>::iterator cit = object.begin();
                cit != object.end(); cit++){

                        void *p  = (*cit).get_anyobject();
                           vcl_cout                                << " startframe: " << ((dbxml_param *)p)->command_
           << " numframes: " << ((dbxml_param *)p)->desc_
               << " width: " << ((dbxml_param *)p)->type_
               << " height: " << ((dbxml_param *)p)->value_
                                   << vcl_endl;
    }
    */
                }
  else
                           vcl_cout                                << " Error parsing file " << gXmlFile
                                   << vcl_endl;

       return 0;
}
