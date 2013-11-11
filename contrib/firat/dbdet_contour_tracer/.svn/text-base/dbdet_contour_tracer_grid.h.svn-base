// This is /lemsvxl/contrib/firat/dbdet_contour_tracer/dbdet_contour_tracer_grid.h.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date Aug 11, 2011

#ifndef DBDET_CONTOUR_TRACER_GRID_H_
#define DBDET_CONTOUR_TRACER_GRID_H_

#define dbdet_contour_tracer_internal_code 1
#include "dbdet_contour_tracer_core.h"

#include <vcl_vector.h>

enum dbdet_contour_tracer_xing_type {XING_HORIZ, XING_VERT, XING_GRID};

//This class represents a zero crossing being stored in a grid.
class dbdet_contour_tracer_xing
{
public:
	double x;
	double y;
	bool traced;
	//type: on a horizontal grid line, on a vertical grid line or on a grid node
	dbdet_contour_tracer_xing_type type;
	//neighboring xings that must be connected by the tracer
	dbdet_contour_tracer_xing* unambiguous_neighbor1;
	dbdet_contour_tracer_xing* unambiguous_neighbor2;
	//tracer should never connect this xing to a bad neighbor
	vcl_vector<dbdet_contour_tracer_xing*> bad_neighbors;
	//Constructor
	dbdet_contour_tracer_xing(double x0, double y0, dbdet_contour_tracer_xing_type t): x(x0), y(y0), type(t), traced(false),
			unambiguous_neighbor1(0), unambiguous_neighbor2(0)
	{}
	dbdet_contour_tracer_xing* get_untraced_unambiguous_neighbor();
	bool is_unambiguous_neighbor(dbdet_contour_tracer_xing* n);
	bool add_unambiguous_neighbor(dbdet_contour_tracer_xing* unambiguous_neighbor);
	bool remove_all_unambiguous_neighbors();
	bool add_bad_neighbor(dbdet_contour_tracer_xing* bad_neighbor);
	bool is_bad_neighbor(dbdet_contour_tracer_xing* n);
};


class dbdet_contour_tracer_grid
{
private:
	vcl_vector<dbdet_contour_tracer_xing*>* grid_points_;
	vcl_vector<dbdet_contour_tracer_xing*>* horiz_lines_;
	vcl_vector<dbdet_contour_tracer_xing*>* vert_lines_;
	int width_, height_;


public:
	dbdet_contour_tracer_grid(int width, int height, Xings* xings);
	~dbdet_contour_tracer_grid();
	vcl_vector<dbdet_contour_tracer_xing*>& get_grid_points(int x, int y);
	vcl_vector<dbdet_contour_tracer_xing*>& get_horiz_points(int x, int y);
	vcl_vector<dbdet_contour_tracer_xing*>& get_vert_points(int x, int y);
	void determine_unambiguous_and_bad_neighbors();
	void get_next_point_candidates(dbdet_contour_tracer_xing* p, vcl_vector<dbdet_contour_tracer_xing*>& next_point_candidates);
	void get_an_unambiguous_point_pair(int& srow, dbdet_contour_tracer_xing*& p1, dbdet_contour_tracer_xing*& p2);
	void remove_all_traced_points();
	bool belong_to_same_cell(dbdet_contour_tracer_xing* p1, dbdet_contour_tracer_xing* p2);
	bool get_associated_cell_ids(dbdet_contour_tracer_xing* p, vcl_vector<int>& idx, vcl_vector<int>& idy);
	int width(){return width_;}
	int height(){return height_;}
};



#endif /* DBDET_CONTOUR_TRACER_GRID_H_ */
