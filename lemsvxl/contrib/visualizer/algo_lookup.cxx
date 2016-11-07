#include <vcl_iostream.h>

#include <vcl_fstream.h>
#include <vcl_cstdio.h>


#include <algo_euler.h>

#define   ANGLE_STEP                  5

int EulerSpiral::load_lookup_table()
  {
    return 0;

//    EulerSpiralParams temp;
//    char line[1024];
//
//    ifstream lookup_file(LOOKUP_DATA_FILE);
//
//    if (!lookup_file)
//      {
//        cout<<" Error : Unable to open lookup data file : "<<LOOKUP_DATA_FILE<<endl;
//        return -1;
//       }
//
//    /*
//     * Assumption : the data needs to be pre-sorted according to starting and ending angles.
//     * */
//
//    double start_angle=0, end_angle=0;
//
//    int count=0;
//    while(lookup_file.getline(line, 1024))
//      {
//        sscanf(line,"%d %d %lf %lf %lf %lf %lf",&start_angle, &end_angle, &(temp.K0), &(temp.Length), &(temp.K2));
//
//        count++;
//
//        _lookup_table.push_back(temp);
//       }
//
//    return 0;
   }

EulerSpiralParams EulerSpiral::get_initial_estimates(int count)
  {
    EulerSpiralParams es_params;

    es_params.setTurningAngle(_intial_bi_arc_estimates.bi_arc_params[count].compute_angle_diff());
    es_params.setK0( _intial_bi_arc_estimates.bi_arc_params[count].K1);
    es_params.setK2( _intial_bi_arc_estimates.bi_arc_params[count].K2);
    es_params.setLength( (_intial_bi_arc_estimates.bi_arc_params[count].Length1+_intial_bi_arc_estimates.bi_arc_params[count].Length2));
    return es_params;

#if 0
    else    
      {
        if (_intial_bi_arc_estimates.size()==2)
          {
            angle_diff1 = _intial_bi_arc_estimates.bi_arc_params[0].compute_angle_diff();
            angle_diff2 = _intial_bi_arc_estimates.bi_arc_params[1].compute_angle_diff();
           }
        else
          {
            if (_intial_bi_arc_estimates.size()==1)
              {
                angle_diff1 = _intial_bi_arc_estimates.bi_arc_params[0].compute_angle_diff();
                angle_diff2 = angle_diff1;
               }
           }

        int start=0, end=0, pos=0;
        start = (int) _normalized_start_angle*(180/M_PI);
        end   = (int) _normalized_end_angle*(180/M_PI);

        start -=start%5;
        end   -=end%5;

        start /=5;
        end   /=5;

        pos = start*72+end;

        if (pos<_lookup_table.size())
          {
            if (((lookup_file[pos].angle_diff-angle_diff1)<=10)||((lookup_file[pos].angle_diff+angle_diff1)<=10))
              {
                es_params.angle_diff = angle_diff1;
                es_params.K0         = lookup_file[pos].K0;
                es_params.K2         = lookup_file[pos].K2;
                es_params.Length     = lookup_file[pos].Length;

                return es_params;
               }
            else
              {
                if (angle_diff1!=angle_diff2)
                  {
                    if (((lookup_file[pos].angle_diff-angle_diff1)<=10)||((lookup_file[pos].angle_diff+angle_diff1)<=10))
                      {
                        es_params.angle_diff = angle_diff1;
                        es_params.K0         = lookup_file[pos].K0;
                        es_params.K2         = lookup_file[pos].K2;
                        es_params.Length     = lookup_file[pos].Length;

                        return es_params;
                       }
                   }
               }

           }

        pos +=1;

        if (pos<_lookup_table.size())
          {
            if (((lookup_file[pos].angle_diff-angle_diff1)<=10)||((lookup_file[pos].angle_diff+angle_diff1)<=10))
              {
                es_params.angle_diff = angle_diff1;
                es_params.K0         = lookup_file[pos].K0;
                es_params.K2         = lookup_file[pos].K2;
                es_params.Length     = lookup_file[pos].Length;

                return es_params;
               }
            else
              {
                if (angle_diff1!=angle_diff2)
                  {
                    if (((lookup_file[pos].angle_diff-angle_diff1)<=10)||((lookup_file[pos].angle_diff+angle_diff1)<=10))
                      {
                        es_params.angle_diff = angle_diff1;
                        es_params.K0         = lookup_file[pos].K0;
                        es_params.K2         = lookup_file[pos].K2;
                        es_params.Length     = lookup_file[pos].Length;

                        return es_params;
                       }
                   }
               }

           }

        pos -=1;
        pos +=72;

        if (pos<_lookup_table.size())
          {
            if (((lookup_file[pos].angle_diff-angle_diff1)<=10)||((lookup_file[pos].angle_diff+angle_diff1)<=10))
              {
                es_params.angle_diff = angle_diff1;
                es_params.K0         = lookup_file[pos].K0;
                es_params.K2         = lookup_file[pos].K2;
                es_params.Length     = lookup_file[pos].Length;

                return es_params;
               }
            else
              {
                if (angle_diff1!=angle_diff2)
                  {
                    if (((lookup_file[pos].angle_diff-angle_diff1)<=10)||((lookup_file[pos].angle_diff+angle_diff1)<=10))
                      {
                        es_params.angle_diff = angle_diff1;
                        es_params.K0         = lookup_file[pos].K0;
                        es_params.K2         = lookup_file[pos].K2;
                        es_params.Length     = lookup_file[pos].Length;

                        return es_params;
                       }
                   }
               }

           }

       }

    pos +=1;

    if (pos<_lookup_table.size())
      {
        if (((lookup_file[pos].angle_diff-angle_diff1)<=10)||((lookup_file[pos].angle_diff+angle_diff1)<=10))
          {
            es_params.angle_diff = angle_diff1;
            es_params.K0         = lookup_file[pos].K0;
            es_params.K2         = lookup_file[pos].K2;
            es_params.Length     = lookup_file[pos].Length;

            return es_params;
           }
        else
          {
            if (angle_diff1!=angle_diff2)
              {
                if (((lookup_file[pos].angle_diff-angle_diff1)<=10)||((lookup_file[pos].angle_diff+angle_diff1)<=10))
                  {
                    es_params.angle_diff = angle_diff1;
                    es_params.K0         = lookup_file[pos].K0;
                    es_params.K2         = lookup_file[pos].K2;
                    es_params.Length     = lookup_file[pos].Length;

                    return es_params;
                   }
               }
           }

       }


#endif

   }
