/*
 * Preprocessor Definitions used here:
 * 
 * DEBUG1 : Writes file and header for each ENO point file read
 * DEBUG2 : Writes coordinates of every ENO point read in file
 * DEBUG3 : Writes coordinates of ENO points which are recorded in the ENO array
 * DEBUG4 : Prints the Wave Front at each step
 * DEBUG5 : Prints the list of polygons at the end of Surface Tracing
 * DEBUG6 : In FindENOPoints_old(), prints the number of ENOs in each slice.
 *          In FindENOPoints(), prints the number of ENOs in each dimension.
 * DEBUG7 : Prints all non-zero DT values in FindENOPoints_old()
 * DEBUG8 : Prints the distance transform of each slice in FindENOPoints_old()
 *          and the coordinates of the found ENO points
 * DEBUG9 : Prints indexes and coords of each ENO point when it is added in
 *          subfunction GetENODataFromZerox() of FindENOPoints()
 * DEBUG10 : Prints information on a single hard_coded voxel (in various places)
 * DEBUG11 : Prints the number of ENOs found in each interval in FindENOPoints()
 * DEBUG12 : When an ambiguous Voxel is encountered in InitWaveFront(),
 *           prints the coordinates of the ENO point being checked.
 * DEBUG13 : Renders all ENO points as small green points and the unused ones
 *           as larger blue points
 * DEBUG14 : Prints information on memory usage
 * DEBUG15 : Prints which voxel is the start of a new wave
 * DEBUG16 : Prints ENO indexes of each polygon made in GetUnambiguousPolygon()
 * DEBUG17 : Prints ENO indexes of each triangle made in MakeTriangles() within
 *           a specified range of Voxels.
 * DEBUG18 : Renders the gridlines of a specified Voxel
 * DEBUG19 : Prints INFO message when SegmentsDoNotCross() returns FALSE
 * DEBUG_TRANSLATE : Prints information on translation of image produced by
 *                   mouse interaction in KeyboardFunction() of gl_main.cxx
 * DEBUG_ZOOM : Prints information on zoom (depth translation) of image produced
 *              by mouse interaction in KeyboardFunction() of gl_main.cxx
 *************************************************************************
 * DT_SHOCK_TWEAK : Changes the values of the distance transform to accomodate a
 *                  special type of file which includes shocks.  A true distance
 *                  transform does *not* result, but the surface is the
 *                  zero-level set.
 *************************************************************************
 * RENDER_OFF : Shuts off rendering of the resulting surface
 * SKIP_AMBIGUITY : Avoids ambiguous voxels, leaving holes in the surface where
 *                  ambiguities lie.
 */

/******************************************************************************/
/* Libraries */

#include <vcl_cstdio.h>
#include <vcl_cstring.h>
#include <vcl_cctype.h>
#include <vcl_cstdlib.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vcl_deque.h> //<queue>
using namespace std;

#include "surface_tracer.h"

/******************************************************************************/
/* Function Prototypes for just this file */

#ifdef DEBUG4
void PrintWaveFront(const deque<WaveElement> & wave_front,
		Voxel *** voxel_array,
		const unsigned int file_dimensions[NUM_DIMENSIONS]);
#endif

#ifdef DEBUG14
void PrintMemoryUsage(const vector<double> & eno_coords,
		const vector<EnoPoint> & eno_array,
		const vector<unsigned int> & poly_list,
		const vector<double> & normal_list,
		const unsigned int file_dimensions[NUM_DIMENSIONS]);
#endif

/******************************************************************************/
/* Surface Tracer Main Function */

#ifdef RENDER_OFF
int main(int argc, char ** argv)
{
	// Store coordinates in 1D array for easy use in OpenGL
	// Assume we can access the whole array pointer using vector::begin()
	vector<double> eno_coords;
	// Other ENO point info
	vector<EnoPoint> eno_array;

	vector<unsigned int> poly_list;
	vector<double> normal_list;
	unsigned int file_dimensions[NUM_DIMENSIONS];
#else
	void SurfaceTracer(vector<double> & eno_coords,
			vector<EnoPoint> & eno_array,
			vector<unsigned int> & poly_list,
			vector<double> & normal_list,
			unsigned int file_dimensions[NUM_DIMENSIONS])
	{
#endif

		// Set Default values of Inputs File parameters
		char dt_file_dir[MAX_STRING_LENGTH] = "";
		char dt_file_name[MAX_STRING_LENGTH] = "";
		char eno_file_dir[MAX_STRING_LENGTH] = "";
		char eno_file_prefix[MAX_STRING_LENGTH] = "";
		char output_file_dir[MAX_STRING_LENGTH] = "";
		char output_file_name[MAX_STRING_LENGTH] = "";
		bool dt_file_has_header = TRUE;
		bool dt_file_has_shocks = FALSE;

		ReadInputsFile(INPUTS_FILE_NAME,
				dt_file_dir,
				dt_file_name,
				eno_file_dir,
				eno_file_prefix,
				output_file_dir,
				output_file_name,
				dt_file_has_header,
				dt_file_has_shocks,
				file_dimensions);

		float *** distance_transform = ReadDTFile(dt_file_dir,
				dt_file_name,
				dt_file_has_header,
				dt_file_has_shocks,
				file_dimensions);

		SpaceUnit *** space_array = ReadENOFiles(eno_file_dir,
				eno_file_prefix,
				file_dimensions,
				eno_array,
				eno_coords);

		if(space_array == 0)
		{
			space_array = FindENOPoints(distance_transform,
					file_dimensions,
					eno_array,
					eno_coords);
		}

		Voxel *** voxel_array = MakeVoxels(space_array,
				file_dimensions);

		/***********************
    cout.precision(5);
          Voxel * vox = &voxel_array[26][69][51];
          //cout << "["<<i<<"]["<<j<<"]["<<k<<"]:" << endl;
          for(int dim = 0; dim < NUM_DIMENSIONS; dim++)
          {
            for(int side = 0; side < 2; side++)
            {
              Face * face = vox->faces[dim][side];
              if(face)
              {
                cout << DIMENSION_CHARS[dim] << side << ":";
                for(int p = 0; p < face->num_enos; p++)
                {
                  cout << face->eno_indexes[p] << " ("
                       <<eno_coords[NUM_DIMENSIONS*face->eno_indexes[p]+0]<<", "
                       <<eno_coords[NUM_DIMENSIONS*face->eno_indexes[p]+1]<<", "
                       <<eno_coords[NUM_DIMENSIONS*face->eno_indexes[p]+2]
                       << "), ";
                }
                cout << endl;
              }
            }
          }
    exit(0);
		 ***********************/

		deque<WaveElement> wave_front;
		//vector<unsigned int> poly_list;
		//vector<double> normal_list;
		//WaveElement next_element;
		unsigned int step;

		while(InitWaveFront(distance_transform,
				voxel_array,
				file_dimensions,
				eno_array,
				eno_coords,
				wave_front,
				poly_list,
				normal_list))
		{
#ifdef DEBUG4
			cout << "\nWave Front Initialized to:" << endl;
			PrintWaveFront(wave_front, voxel_array, file_dimensions);
#endif
			step = 1;

			while(PropagateWave(voxel_array,
					file_dimensions,
					eno_array,
					eno_coords,
					wave_front,
					poly_list,
					normal_list))
			{
				step++;
#ifdef DEBUG4
				cout << "\nWave Front Propagation Step " << step << ":" << endl;
				PrintWaveFront(wave_front, voxel_array, file_dimensions);
				cin.get();
#endif
			}
		}

		cout << "Surface Trace Complete." << endl;
		cout << poly_list.size() / 3 << " Polygons created." << endl;

#ifdef DEBUG4
		PrintWaveFront(wave_front, voxel_array, file_dimensions);
#endif

#ifdef DEBUG5
		cout << "\nPolygons:" << endl;
		for(unsigned int poly = 0; poly < poly_list.size(); poly++)
		{
			cout << poly_list[poly] << "\t";
			if(poly%NUM_DIMENSIONS == NUM_DIMENSIONS-1)
				cout << endl;
		}
#endif

#ifdef DEBUG14
		PrintMemoryUsage(eno_coords, eno_array, poly_list, normal_list,
				file_dimensions);
#endif

		if(output_file_name && (output_file_name[0] != '\0'))
		{
			char output_file_path[MAX_STRING_LENGTH];
			sprintf(output_file_path, "%s%s", output_file_dir, output_file_name);

			cout << "Writing to VTK PolyData file to " << output_file_path << " ..."
					<< endl;

			// Coordinates are *backwards* -- Z Y X
			char output_header[MAX_STRING_LENGTH];
			sprintf(output_header, "%d %d %d -- Output of Surface Tracer for %s%s",
					file_dimensions[DIM_X], file_dimensions[DIM_Y],
					file_dimensions[DIM_Z], dt_file_dir, dt_file_name);

			WriteVTKPolyFile(output_file_path,
					&*eno_coords.begin(),
					eno_coords.size()/3,
					&*poly_list.begin(),
					poly_list.size()/3,
					output_header);
		}

		cout << "Deallocating..." << endl;
		// Deallocate everything
		delete[](distance_transform[0][0]);
		delete[](distance_transform[0]);
		delete[](distance_transform);
		distance_transform = 0;
		delete[](space_array[0][0]);
		delete[](space_array[0]);
		delete[](space_array);
		space_array = 0;
		delete[](voxel_array[0][0]);
		delete[](voxel_array[0]);
		delete[](voxel_array);
		voxel_array = 0;

#ifdef RENDER_OFF  
		return 0;
#endif
	}




	/******************************************************************************/
	/* Surface Tracer Main Function for Incremental Control */

	bool SurfaceTracerIncrement(vector<double> & eno_coords,
			vector<EnoPoint> & eno_array,
			vector<unsigned int> & poly_list,
			vector<double> & normal_list,
			unsigned int file_dimensions[NUM_DIMENSIONS])
	{
		static bool init = FALSE;
		static float *** distance_transform;
		static SpaceUnit *** space_array;
		static Voxel *** voxel_array;

		// Store coordinates in 1D array for easy use in OpenGL
		// Assume we can access the whole array pointer using vector::begin()
		//vector<double> eno_coords;
		// Other ENO point info
		//static vector<EnoPoint> eno_array;
		static deque<WaveElement> wave_front;
		//vector<unsigned int> poly_list;
		//vector<double> normal_list;
		//static WaveElement next_element;
		unsigned int step;
		static char dt_file_dir[MAX_STRING_LENGTH] = "";
		static char dt_file_name[MAX_STRING_LENGTH] = "";
		static char output_file_dir[MAX_STRING_LENGTH] = "";
		static char output_file_name[MAX_STRING_LENGTH] = "";

		if(!init)
		{
			char eno_file_dir[MAX_STRING_LENGTH] = "";
			char eno_file_prefix[MAX_STRING_LENGTH] = "";
			bool dt_file_has_header = TRUE;
			bool dt_file_has_shocks = FALSE;

			// Set Default values of Inputs File parameters
			ReadInputsFile(INPUTS_FILE_NAME,
					dt_file_dir,
					dt_file_name,
					eno_file_dir,
					eno_file_prefix,
					output_file_dir,
					output_file_name,
					dt_file_has_header,
					dt_file_has_shocks,
					file_dimensions);

			distance_transform = ReadDTFile(dt_file_dir,
					dt_file_name,
					dt_file_has_header,
					dt_file_has_shocks,
					file_dimensions);

			space_array = ReadENOFiles(eno_file_dir,
					eno_file_prefix,
					file_dimensions,
					eno_array,
					eno_coords);

			if(space_array == 0)
			{
				space_array = FindENOPoints(distance_transform,
						file_dimensions,
						eno_array,
						eno_coords);
			}
			//PUT BY FIRAT

			vcl_cout << "eno_array.size() = " << eno_array.size() << vcl_endl;
			vcl_cout << "eno_coords.size() = " << eno_coords.size() << vcl_endl;
			/*for(int i = 0; i < eno_array.size(); i++)
			{
				EnoPoint ep = eno_array[i];
				vcl_cout << "Indices: " << ep.indexes[0] << " " << ep.indexes[1] << " " << ep.indexes[2];
				vcl_cout << " dim_index: " << int(ep.dim_index) << " used: " << ep.used << " coord:"<< eno_coords[i]<< vcl_endl;

			}*/
			vcl_ofstream eno_file("/home/firat/lemsvxl/src/contrib/firat/courses/engn2500/rodehorst/eno/temp.eno");
			for(int pnt = 0; pnt < eno_coords.size(); pnt+=3)
			{
				eno_file << eno_coords[pnt] << " " << eno_coords[pnt+1] << " " << eno_coords[pnt+2] << vcl_endl;
			}
			eno_file.close();
			//exit(0);
			///
			/// FIRAT
			voxel_array = MakeVoxels(space_array,
					file_dimensions);

			/***********************
    cout.precision(5);
    for(int i = 19; i <= 21; i++)
    {
      for(int j = 9; j <= 11; j++)
      {
        for(int k = 18; k <= 20; k++)
        {
          Voxel * vox = &voxel_array[i][j][k];
          cout << "["<<i<<"]["<<j<<"]["<<k<<"]:" << endl;
          for(int dim = 0; dim < NUM_DIMENSIONS; dim++)
          {
            for(int side = 0; side < 2; side++)
            {
              Face * face = vox->faces[dim][side];
              if(face)
              {
                cout << DIMENSION_CHARS[dim] << side << ":";
                for(int p = 0; p < face->num_enos; p++)
                {
                  cout << face->eno_indexes[p] << " ("
                       <<eno_coords[NUM_DIMENSIONS*face->eno_indexes[p]+0]<<", "
                       <<eno_coords[NUM_DIMENSIONS*face->eno_indexes[p]+1]<<", "
                       <<eno_coords[NUM_DIMENSIONS*face->eno_indexes[p]+2]
                       << "), ";
                }
                cout << endl;
              }
            }
          }
        }
      }
    }
    exit(0);
			 ***********************/

			init = TRUE;
		}

		if(wave_front.empty())
		{
			step = 1;
			if(InitWaveFront(distance_transform,
					voxel_array,
					file_dimensions,
					eno_array,
					eno_coords,
					wave_front,
					poly_list,
					normal_list))
			{
				//FIRAT
				vcl_cout << "AA" << vcl_endl;
				vcl_cout << "eno_array.size() = " << eno_array.size() << vcl_endl;
				vcl_cout << "eno_coords.size() = " << eno_coords.size() << vcl_endl;
				vcl_cout << "poly_list.size() = " << poly_list.size() << vcl_endl;
				vcl_cout << "normal_list.size() = " << normal_list.size() << vcl_endl;
				vcl_cout << "wave_front.size() = " << wave_front.size() << vcl_endl;
				//FIRAT
				return TRUE;
			}
			else
			{
				cout << "Surface Trace Complete." << endl;
				cout << poly_list.size() / 3 << " Polygons created." << endl;

#ifdef DEBUG4
				// This should print nothing; the wave front should be empty.
				PrintWaveFront(wave_front, voxel_array, file_dimensions);
#endif

#ifdef DEBUG5
				cout << "\nPolygons:" << endl;
				for(unsigned int poly = 0; poly < poly_list.size(); poly++)
				{
					cout << poly_list[poly] << "\t";
					if(poly%NUM_DIMENSIONS == NUM_DIMENSIONS-1)
						cout << endl;
				}
#endif

#ifdef DEBUG14
				PrintMemoryUsage(eno_coords, eno_array, poly_list, normal_list,
						file_dimensions);
#endif

				if(output_file_name && (output_file_name[0] != '\0'))
				{
					char output_file_path[MAX_STRING_LENGTH];
					sprintf(output_file_path, "%s%s", output_file_dir, output_file_name);

					cout << "Writing to VTK PolyData file to " << output_file_path << " ..."
							<< endl;

					// Coordinates are *backwards* -- Z Y X
					char output_header[MAX_STRING_LENGTH];
					sprintf(output_header,
							"%d %d %d -- Polygon Data from Surface Tracer output using %s%s",
							file_dimensions[DIM_X], file_dimensions[DIM_Y],
							file_dimensions[DIM_Z], dt_file_dir, dt_file_name);

					WriteVTKPolyFile(output_file_path,
							&*eno_coords.begin(),
							eno_coords.size()/3,
							&*poly_list.begin(),
							poly_list.size()/3,
							output_header);
				}

				cout << "Deallocating..." << endl;
				// Deallocate everything
				delete[](distance_transform[0][0]);
				delete[](distance_transform[0]);
				delete[](distance_transform);
				distance_transform = 0;
				delete[](space_array[0][0]);
				delete[](space_array[0]);
				delete[](space_array);
				space_array = 0;
				delete[](voxel_array[0][0]);
				delete[](voxel_array[0]);
				delete[](voxel_array);
				voxel_array = 0;

				//eno_array.clear();
				wave_front.clear();
				//FIRAT
				vcl_cout << "BB" << vcl_endl;
				//FIRAT
				return FALSE;
			}
		}
		else
		{
			if(PropagateWave(voxel_array,
					file_dimensions,
					eno_array,
					eno_coords,
					wave_front,
					poly_list,
					normal_list))
			{
				step++;
			}

			return TRUE;
		}


	}




	/******************************************************************************/

#ifdef DEBUG4

	void PrintWaveFront(const deque<WaveElement> & wave_front,
			Voxel *** voxel_array,
			const unsigned int file_dimensions[NUM_DIMENSIONS])
	{
		unsigned int i = 0;
		Face * face_ptr;
		int offsets[NUM_DIMENSIONS] = {0,0,0};
		cout.precision(4);

		for(deque<WaveElement>::const_iterator it = wave_front.begin();
				it != wave_front.end(); it++)
		{

			if((*it).voxel_idxs[(*it).face_dim] < file_dimensions[(*it).face_dim]-1)
			{
				cout << "Valid ";
				face_ptr = voxel_array[(*it).voxel_idxs[0]]
				                       [(*it).voxel_idxs[1]]
				                        [(*it).voxel_idxs[2]].
				                        faces[(*it).face_dim]
				                              [(*it).face_side];
			}
			else
			{
				cout << "INVALID ";
				offsets[(*it).face_dim] = 2*(*it).face_side - 1;
				face_ptr = voxel_array[(*it).voxel_idxs[0] + offsets[0]]
				                       [(*it).voxel_idxs[1] + offsets[1]]
				                        [(*it).voxel_idxs[2] + offsets[2]].
				                        faces[(*it).face_dim]
				                              [!(*it).face_side];
				offsets[(*it).face_dim] = 0;
			}

			cout << "Wave element " << i << ":" << endl;
			cout << "ENOs: " << face_ptr->eno_indexes[(*it).eno0]
			                                          << "(" << (*it).eno0 << ") "
			                                          << face_ptr->eno_indexes[(*it).eno1]
			                                                                   << "(" << (*it).eno1 << ")" << endl;
			cout << "Normal: " << (*it).normal[0] << " "
					<< (*it).normal[1] << " "
					<< (*it).normal[2] << endl;
			cout << "Voxel: " << (*it).voxel_idxs[0] << " "
					<< (*it).voxel_idxs[1] << " "
					<< (*it).voxel_idxs[2] << endl;
			cout << "Face: " << DIMENSION_CHARS[(*it).face_dim] << " "
					<< (*it).face_side << endl;

			i++;
		}
	}

#endif




	/******************************************************************************/

#ifdef DEBUG14

	void PrintMemoryUsage(const vector<double> & eno_coords,
			const vector<EnoPoint> & eno_array,
			const vector<unsigned int> & poly_list,
			const vector<double> & normal_list,
			const unsigned int file_dimensions[NUM_DIMENSIONS])
	{
		cout << "Memory Usage:" << endl;
		cout << "distance_transform: " << file_dimensions[DIM_Z]*
				file_dimensions[DIM_Y]*
				file_dimensions[DIM_X]*sizeof(float) +
				(file_dimensions[DIM_Y]+1)*
				file_dimensions[DIM_X]*sizeof(float*)
				<< " bytes" << endl;
		cout << "space_array: " << file_dimensions[DIM_Z]*
				file_dimensions[DIM_Y]*
				file_dimensions[DIM_X]*sizeof(SpaceUnit) +
				(file_dimensions[DIM_Y]+1)*
				file_dimensions[DIM_X]*sizeof(SpaceUnit*)
				<< " bytes" << endl;
		cout << "voxel_array: " << file_dimensions[DIM_Z]*
				file_dimensions[DIM_Y]*
				file_dimensions[DIM_X]*sizeof(Voxel) +
				(file_dimensions[DIM_Y]+1)*
				file_dimensions[DIM_X]*sizeof(Voxel*)
				<< " bytes (plus Faces)" << endl;
		cout << "eno_coords: " << eno_coords.capacity()*sizeof(double)
                        								 << " bytes" << endl;
		cout << "eno_array: " << eno_array.capacity()*sizeof(EnoPoint)
                        								<< " bytes" << endl;
		cout << "poly_list: " << poly_list.capacity()*sizeof(unsigned int)
                        								<< " bytes" << endl;
		cout << "normal_list: " << normal_list.capacity()*sizeof(double)
                        								  << " bytes" << endl;
		cout << "Individual Type sizes:"
				<< "\nSpaceUnit: " << sizeof(SpaceUnit)
				<< "\nVoxel: " << sizeof(Voxel)
				<< "\nEnoPoint: " << sizeof(EnoPoint) << endl;
	}

#endif




	/******************************************************************************/
