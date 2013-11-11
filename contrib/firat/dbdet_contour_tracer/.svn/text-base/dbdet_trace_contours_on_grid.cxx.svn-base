// This is /lemsvxl/contrib/firat/dbdet_contour_tracer/dbdet_trace_contours_on_grid.cxx.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date Aug 12, 2011

#include "dbdet_trace_contours_on_grid.h"
#include <vsol/vsol_point_2d.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>


double find_angle_of_vector(dbdet_contour_tracer_xing* a, dbdet_contour_tracer_xing* b)
{
	return vcl_atan2(b->y - a->y, b->x - a->x);
}

void dbdet_trace_all_contours_on_grid(dbdet_contour_tracer_grid& grid, vcl_vector<vcl_vector<vsol_point_2d_sptr> >& contours)
{
	dbdet_contour_tracer_xing *p1, *p2;
	int srow = 0;
	grid.determine_unambiguous_and_bad_neighbors();
	grid.get_an_unambiguous_point_pair(srow, p1, p2);
	while(p1 != 0)
	{
		p1->traced = true;
		p2->traced = true;
		vcl_vector<vsol_point_2d_sptr> contour;
		dbdet_trace_contour_given_first_two_points(grid, p1, p2, contour);
		/*vcl_cout << "Contour Size: " << contour.size() << vcl_endl;
		for(int i = 0; i < contour.size(); i++)
		{
			vcl_cout << "(" << contour[i]->x() << ", " << contour[i]->y() << ") ";
		}
		vcl_cout << vcl_endl;*/
		contours.push_back(contour);
		grid.get_an_unambiguous_point_pair(srow, p1, p2);
	}
}

dbdet_contour_tracer_xing* dbdet_trace_contour_given_first_two_points(dbdet_contour_tracer_grid& grid, dbdet_contour_tracer_xing* prev, dbdet_contour_tracer_xing* curr, vcl_vector<vsol_point_2d_sptr>& contour)
{
	dbdet_contour_tracer_xing* starting_point = prev;
	vsol_point_2d_sptr prevP = new vsol_point_2d(prev->x, prev->y);
	contour.push_back(prevP);
	double min_angle = find_angle_of_vector(prev, curr);
	while(true)
	{
		vsol_point_2d_sptr currP = new vsol_point_2d(curr->x, curr->y);
		contour.push_back(currP);

		if(grid.belong_to_same_cell(curr, starting_point) && contour.size() > 3)
		{
			curr->traced = true;
			return curr;
		}

		dbdet_contour_tracer_xing* unambiguous_neighbor = curr->get_untraced_unambiguous_neighbor();

		if(unambiguous_neighbor)
		{
			min_angle = find_angle_of_vector(curr, unambiguous_neighbor);
			prev = curr;
			curr = unambiguous_neighbor;
			curr->traced = true;
		}
		else
		{
			double src_angle = min_angle;
			vcl_vector<dbdet_contour_tracer_xing*> next_point_candidates;
			grid.get_next_point_candidates(curr, next_point_candidates);
			double angle, diff, min_diff = 10;
			int min_index = -1;
			for(int i = 0; i < next_point_candidates.size();i++)
			{
				//vcl_cout << "Candidate " << i << " (" << next_point_candidates[i]->x << ", " << next_point_candidates[i]->y << ")" << vcl_endl;
				angle = find_angle_of_vector(curr, next_point_candidates[i]);
				diff = vcl_acos(vcl_cos(angle - src_angle));
				if(/*diff < vnl_math::pi/10 &&*/ diff < min_diff && !curr->is_bad_neighbor(next_point_candidates[i]))
				{
					min_diff = diff;
					min_index = i;
					min_angle = angle;
				}
			}
			prev = curr;
			if(min_index > -1)
			{
				curr = next_point_candidates[min_index];
				curr->traced = true;
			}
			else
			{
				return curr;
				//curr = 0;
			}
		}
	}
}
