// This is basic/dbxml/dbxml_io.cxx

//:
// \file
// \brief Parse and xml file.
// \author J Mundy
// 
//

// \verbatim
//  Modifications
//   5/17/2004   T Orechia Updated for 2.5.0 xerces lib.
// \endverbatim

// This file was updated from the code in brl/bbas/bxml directory.
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include "dbxml_io.h"
#include "dbxml_input_converter.h"
#include "dbxml_track_input_converter.h"
#include "dbxml_dom.h"

#include <vcl_string.h>
#include <vcl_iostream.h>
//#include <stdlib.h>


vcl_vector<dbxml_input_converter_sptr > dbxml_io::input_converter_;

 
dbxml_io::dbxml_io()
{
  // Initialize the XML4C2 system
  try{
    XMLPlatformUtils::Initialize();
  }
  catch(const XMLException &toCatch){
    vcl_cerr << "Error during Xerces-c Initialization.\n"
             << "  Exception message:"
             << StrX(toCatch.getMessage()) << vcl_endl;
  }

}

 
dbxml_io::~dbxml_io()
{
  XMLPlatformUtils::Terminate();
}


dbxml_input_converter_sptr
dbxml_io::find_input_converter(const vcl_string& class_name)
{     
  for (vcl_vector<dbxml_input_converter_sptr>::iterator cit=input_converter_.begin();
       cit != input_converter_.end(); cit++) {
    vcl_string conv_name = (*cit)->get_class_name();
    if (conv_name == class_name)
      return *cit;
  }
  
  return 0;
}


dbxml_input_converter_sptr
dbxml_io::find_converter_from_tag(const vcl_string& tag_name)
{      
  for (vcl_vector<dbxml_input_converter_sptr>::iterator cit=input_converter_.begin();
       cit != input_converter_.end(); cit++) {
    vcl_string tname = (*cit)->get_class_name();
    vcl_string ref_tname = (*cit)->get_ref_tag_name();
    if ( (tname == tag_name) || (ref_tname == tag_name) )
      return *cit;
  }
  return 0;
}


void
dbxml_io::register_input_converter(const dbxml_input_converter_sptr& conv)
{
  if (!conv) {
    vcl_cerr << "Can not register null converter" << vcl_endl;
    return;
  }

  //see if converter already exists
  vcl_string class_name = conv->get_class_name();
  if (dbxml_io::find_input_converter(class_name))
    return;

  //if not found then add it
#ifdef DEBUG
  vcl_cout << "registering " << conv->get_class_name() << " converter\n";
#endif

 input_converter_.push_back(conv);
  
}

  
void dbxml_io::register_input_converters()
{
  dbxml_io::register_input_converter(new dbxml_param_input_converter());
  dbxml_io::register_input_converter(new dbxml_track_input_converter());
}


bool dbxml_io::parse_xml(char *xmlfile,  vcl_vector<dbxml_generic_ptr>& objs)
{
  DOMNode *childnext=0;
  DOMNode *root = dbxml_io::getroot(xmlfile);
  if (!root){
    vcl_cerr << "Error: Unable to get root from file "  << vcl_endl;
    return false;
  }

  dbxml_input_converter_sptr conv = find_converter_from_tag(XMLString::transcode(root->getNodeName()));
  if (!conv){
    vcl_cerr << "Error: Unable to find converter from tag ="
             << XMLString::transcode(root->getNodeName()) << vcl_endl;
    return false;
  }

  if (conv->check_tag(root, 0) == 0)
    return false;

  DOMNode *child = root->getFirstChild();
  int cnode_type = child->getNodeType();

  while ( child != 0 )
  {
    if (conv->check_tag(child, 1) != 0){    
      bool bExtract = conv->extract_from_dom_1(child);
        
      if (bExtract == false)
        return(false); 
            
      DOMNode *paramchild = child->getFirstChild();
      cnode_type = paramchild->getNodeType();

      while ( paramchild != 0 )
      {
        int rc;
        if ((rc=conv->check_tag(paramchild, 2)) != 0){

#ifdef DEBUG
          vcl_cout << "calling " << conv->get_class_name() << vcl_endl;
#endif

          if (rc == 2){
            bExtract = conv->extract_from_dom_2(paramchild);
            if (bExtract ==false)
              return false;
            dbxml_generic_ptr gp = conv->construct_object_2(objs);
          }
          else if (rc ==3){
            bExtract = conv->extract_from_dom_3(paramchild);
            if (bExtract ==false)
              return false;
            dbxml_generic_ptr gp = conv->construct_object_3(objs);
          }
          else
            vcl_cerr << "Error: unable to parse tag ="
                     << XMLString::transcode(paramchild->getNodeName()) << vcl_endl;
        }

        childnext = paramchild->getNextSibling();
        paramchild = childnext;
      }//while

      dbxml_generic_ptr gp = conv->construct_object_1(objs);
      if (gp.get_anyobject() == NULL)
        return(false);
    }//if
         
 
  childnext = child->getNextSibling();
  child = childnext;
        
  }//while

  return true;
}


DOMNode *dbxml_io::getroot(char *xmlfile)
{ 
  XercesDOMParser *parser = new XercesDOMParser;
  //parser->setValidationScheme(gValScheme);
  parser->setDoNamespaces(false);
  parser->setDoSchema(false);
  parser->setValidationSchemaFullChecking(false);
  parser->setCreateEntityReferenceNodes(false);

  DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter();
  parser->setErrorHandler(errReporter);

  //
  //  Parse the XML file, catching any XML exceptions that might propogate
  //  out of it.
  //
  bool errorsOccured = false;
  try
  {
    parser->parse(xmlfile);
  }
  catch (const XMLException& e)
  {
    vcl_cerr << "An error occurred during parsing\n   Message: "
             << StrX(e.getMessage()) << vcl_endl;
    errorsOccured = true;
  }
  catch (const DOMException& e)
  {
    const unsigned int maxChars = 2047;
    XMLCh errText[maxChars + 1];

    vcl_cerr << "\nDOM Error during parsing: '" << xmlfile << "'\n"
             << "DOMException code is:  " << e.code << vcl_endl;

    if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
      vcl_cerr << "Message is: " << StrX(errText) << vcl_endl;

    errorsOccured = true;
  }
  catch (...)
  {
    vcl_cerr << "An error occurred during parsing\n " << vcl_endl;
    errorsOccured = true;
  }

  // get the DOM representation
  if (errorsOccured || errReporter->getSawErrors())
    return NULL;

  DOMNode *doc = 0;
  doc = parser->getDocument();

  // delete parser;
  // delete errReporter;

  short node_type = doc->getNodeType();

  if (node_type != DOMNode::DOCUMENT_NODE) {
    vcl_cerr << "Error: node_type=" << node_type << vcl_endl;
    return NULL;
  }
  // char *tag = XMLString::transcode(doc->getNodeName());

  return doc->getFirstChild();
}

