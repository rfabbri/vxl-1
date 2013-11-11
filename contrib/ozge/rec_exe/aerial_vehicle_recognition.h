#ifndef aerial_vehicle_recognition_dll
#define aerial_vehicle_recognition_dll

#ifdef WIN32
#ifdef aerial_vehicle_recognition_api_EXPORTS
    #define VEHICLE_API __declspec(dllexport) 
#else 
    #define VEHICLE_API __declspec(dllimport) 
#endif 
#else 
    #define VEHICLE_API 
#endif // now declare a function...

//: db_list_filename is the xml file that stores each object, and its polygons
//  video_list_file_name is <id, directory> pair for each video which have objects in the database
extern "C" VEHICLE_API int createosl(char *db_object_list_filename, char *video_list_file_name, char *osl_file_name);

//: load all observations from osl binary file
extern "C" VEHICLE_API unsigned int loadosl(char *osl_file_name);

//: create a database from selected observations in the database
extern "C" VEHICLE_API unsigned int createdb(unsigned int osl_handle, char *selected_obs_list_file_name);

//: create a list which contains likelihood values for each class 
//  rms: root mean square error for line fitting before curve matching
//       the smaller rms gets, the coarser the polygon gets, the faster the algorithm runs, but the worse the correspondence gets
//       typical value 0.05 is used in our experiments
//  n  : the number of top matches to be considered to be used in score generation
//       typical value is 2 or 3 depending on database size, and the number of instances from each category
extern "C" VEHICLE_API int run_object_dt(unsigned int osl_handle, unsigned int db_handle, int i0, int p0, double *output_list, float rms, int n);

//: create a list which contains likelihood values for each class 
//: create a list which contains likelihood values for each class 
//  rms: root mean square error for line fitting before curve matching
//       the smaller rms gets, the coarser the polygon gets, the faster the algorithm runs, but the worse the correspondence gets
//       typical value 0.05 is used in our experiments
//  n  : the number of top matches to be considered to be used in score generation
//       typical value is 2 or 3 depending on database size, and the number of instances from each category
//  increment: internal parameter of line intersection algorithm which determines the density of correspondence
//             min value is 1, the densest and best results
//             typical 5 to 20, the higher the value, the faster the algorithm runs, but the worse the correspondence gets
extern "C" VEHICLE_API int run_object_line(unsigned int osl_handle, unsigned int db_handle, int i0, int p0, double *output_list, float rms, int n, int increment);

//  METHODS TO GO INTO THE FINAL SYSTEM INTEGRATION
//  CAUTION: The following functions are useful if Measurement and FEX module is in place and 
//           creates pointers to vxllems class instances of dbinfo_observation, dbru_label and dbsol_polygon_2d

//: observation, polygon and label constitutes the feature list extracted from Frame i by Measurement and FEX module
//  output_list contains the likelihoods for each category
//  if verbose = 0 --> no text printout to standard output
//  else           --> printout status reports
extern "C" VEHICLE_API int run_observation_dt(unsigned int osl_handle, unsigned int db_handle, unsigned int observationhandle, unsigned int polygonhandle, unsigned int labelhandle, double *output_list, float rms, int verbose);
extern "C" VEHICLE_API int run_observation_line(unsigned int osl_handle, unsigned int db_handle, unsigned int observationhandle, unsigned int polygonhandle, unsigned int labelhandle, double *output_list, float rms, int increment, int verbose);

//: float ratio in [0,1]: the ratio of query region randomly searched, performance gets better as ratio increases, but takes longer to complete a match
//  dx: translation range, dr: rotation range, ds: scale range
//  if verbose = 0 --> no text printout to standard output
//  else           --> printout status reports
extern "C" VEHICLE_API int run_observation_opt(unsigned int osl_handle, unsigned int db_handle, unsigned int observationhandle, unsigned int polygonhandle, unsigned int labelhandle, double *output_list, float ratio, float dx, float dr, float ds, int verbose);

//-----------------------------------------------------------------------------------------------------------
// METHODS THAT RETURN RAW MUTUAL INFORMATION SCORES INSTEAD OF CLASS LIKELIHOODS
//: observation, polygon and label constitutes the feature list extracted from Frame i by Measurement and FEX module
//  output_list contains the raw mutual info score for each category
//  if verbose = 0 --> no text printout to standard output
//  else           --> printout status reports
extern "C" VEHICLE_API int run_observation_dt2(unsigned int osl_handle, unsigned int db_handle, unsigned int observationhandle, unsigned int polygonhandle, unsigned int labelhandle, double *output_list, float rms, int verbose);
extern "C" VEHICLE_API int run_observation_line2(unsigned int osl_handle, unsigned int db_handle, unsigned int observationhandle, unsigned int polygonhandle, unsigned int labelhandle, double *output_list, float rms, int increment, int verbose);

//: float ratio in [0,1]: the ratio of query region randomly searched, performance gets better as ratio increases, but takes longer to complete a match
//  dx: translation range, dr: rotation range, ds: scale range
//  output_list contains the raw mutual info score for each category
//  if verbose = 0 --> no text printout to standard output
//  else           --> printout status reports
extern "C" VEHICLE_API int run_observation_opt2(unsigned int osl_handle, unsigned int db_handle, unsigned int observationhandle, unsigned int polygonhandle, unsigned int labelhandle, double *output_list, float ratio, float dx, float dr, float ds, int verbose);
//-----------------------------------------------------------------------------------------------------------

extern "C" VEHICLE_API int run_object_opt(unsigned int osl_handle, unsigned int db_handle, int i0, int p0, double *output_list, int n, float ratio, float dx, float dr, float ds);
#endif
