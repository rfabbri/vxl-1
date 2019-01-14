#include<georegister/dbrl_id_point_2d_sptr.h>
#include<georegister/dbrl_id_point_2d.h>

#include "dbrl_crop_id_points_process.h"

dbrl_crop_id_points_process::dbrl_crop_id_points_process() : bpro1_process()
{
    if( !parameters()->add( "Topx" ,"-topx" ,(int)0 ) ||
        !parameters()->add( "Topy" ,"-topy" ,(int)0 ) ||
        !parameters()->add( "Lenx" ,"-lenx" ,(int)100 ) ||
        !parameters()->add( "Leny" ,"-leny" ,(int)100 ) 
        )

    {
        std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
    }
    
}


/*************************************************************************
* Function Name: dbrl_crop_id_points_process::~dbrl_crop_id_points_process
* Parameters: 
* Effects: 
*************************************************************************/

dbrl_crop_id_points_process::~dbrl_crop_id_points_process()
{
   
}


//: Clone the process
bpro1_process*
dbrl_crop_id_points_process::clone() const
{
    return new dbrl_crop_id_points_process(*this);
}


/*************************************************************************
* Function Name: dbrl_crop_id_points_process::name
* Parameters: 
* Returns: std::string
* Effects: 
*************************************************************************/
std::string
dbrl_crop_id_points_process::name()
{
    return "Crop id points";
}


/*************************************************************************
* Function Name: ddbil_osl_canny_edges_process::get_input_type
* Parameters: 
* Returns: std::vector< std::string >
* Effects: 
*************************************************************************/
std::vector< std::string > dbrl_crop_id_points_process::get_input_type()
{
    std::vector< std::string > to_return;
    to_return.push_back( "dbrl_id_point_2d" );
    return to_return;
}


/*************************************************************************
* Function Name: dbrl_crop_id_points_process::get_output_type
* Parameters: 
* Returns: std::vector< std::string >
* Effects: 
*************************************************************************/
std::vector< std::string > dbrl_crop_id_points_process::get_output_type()
{
    std::vector< std::string > to_return;
    to_return.push_back( "dbrl_id_point_2d" );
    return to_return;
}
//: Returns the number of input frames to this process
int
dbrl_crop_id_points_process::input_frames()
{
    return 1;
}


//: Returns the number of output frames from this process
int
dbrl_crop_id_points_process::output_frames()
{
    return 1;
}


/*************************************************************************
* Function Name: ddbil_osl_canny_edges_process::execute
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
bool
dbrl_crop_id_points_process::execute()
{
    if ( input_data_.size() != 1 ){
        std::cout << "In dbrl_crop_id_points_process::execute() - not exactly one"
            << " input image \n";
        return false;
    }
    clear_output();

    // get image from the storage class

    dbrl_id_point_2d_storage_sptr frame_idpoints;
    frame_idpoints.vertical_cast(input_data_[0][0]);

    std::vector<dbrl_id_point_2d_sptr> pts=frame_idpoints->points();
    std::vector<dbrl_id_point_2d_sptr> crppts;

    static int topx=0;
    static int topy=0;
    static int lenx=0;
    static int leny=0;
    

    parameters()->get_value( "-topx" ,topx);
    parameters()->get_value( "-topy" ,topy);
    parameters()->get_value( "-lenx" ,lenx);
    parameters()->get_value( "-leny" ,leny);
    
    for(int i=0;i<static_cast<int>(pts.size());i++)
        {
            double x =pts[i]->x();
            double y =pts[i]->y();

            if(x<topx+lenx && x>topx && y<(topy+leny) && y>topy)
                crppts.push_back(pts[i]);

        }

     dbrl_id_point_2d_storage_sptr id_point_storage = dbrl_id_point_2d_storage_new();
     id_point_storage->set_id_points(crppts);
     output_data_[0].push_back(id_point_storage);


    clear_input();
    return true;
}


/*************************************************************************
* Function Name: dbrl_crop_id_points_process::finish
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
bool
dbrl_crop_id_points_process::finish()
{
    return true;
}


