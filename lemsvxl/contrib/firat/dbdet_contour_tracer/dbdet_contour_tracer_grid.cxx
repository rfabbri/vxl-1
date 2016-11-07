// This is /lemsvxl/contrib/firat/dbdet_contour_tracer/dbdet_contour_tracer_grid.cxx.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date Aug 11, 2011

#include "dbdet_contour_tracer_grid.h"

dbdet_contour_tracer_xing* dbdet_contour_tracer_xing::get_untraced_unambiguous_neighbor()
{
	if(unambiguous_neighbor1 && !unambiguous_neighbor1->traced)
	{
		return unambiguous_neighbor1;
	}
	else if(unambiguous_neighbor2 && !unambiguous_neighbor2->traced)
	{
		return unambiguous_neighbor2;
	}
	else
	{
		return 0;
	}
}

bool dbdet_contour_tracer_xing::is_unambiguous_neighbor(dbdet_contour_tracer_xing* n)
{
	return (n == unambiguous_neighbor1) || (n == unambiguous_neighbor2);
}

bool dbdet_contour_tracer_xing::add_unambiguous_neighbor(dbdet_contour_tracer_xing* unambiguous_neighbor)
{
	if(!unambiguous_neighbor1)
	{
		unambiguous_neighbor1 = unambiguous_neighbor;
	}
	else if(!unambiguous_neighbor2)
	{
		unambiguous_neighbor2 = unambiguous_neighbor;
	}
	else
	{
		return false;
	}
}

bool dbdet_contour_tracer_xing::remove_all_unambiguous_neighbors()
{
	if(unambiguous_neighbor1)
	{
		if(unambiguous_neighbor1->unambiguous_neighbor1 == this)
		{
			unambiguous_neighbor1->unambiguous_neighbor1 = 0;
		}
		else if(unambiguous_neighbor1->unambiguous_neighbor2 == this)
		{
			unambiguous_neighbor1->unambiguous_neighbor2 = 0;
		}
		unambiguous_neighbor1 = 0;
	}
	if(unambiguous_neighbor2)
	{
		if(unambiguous_neighbor2->unambiguous_neighbor1 == this)
		{
			unambiguous_neighbor2->unambiguous_neighbor1 = 0;
		}
		else if(unambiguous_neighbor2->unambiguous_neighbor2 == this)
		{
			unambiguous_neighbor2->unambiguous_neighbor2 = 0;
		}
		unambiguous_neighbor2 = 0;
	}
	return true;
}

bool dbdet_contour_tracer_xing::add_bad_neighbor(dbdet_contour_tracer_xing* bad_neighbor)
{
	bad_neighbors.push_back(bad_neighbor);
	return true;
}

bool dbdet_contour_tracer_xing::is_bad_neighbor(dbdet_contour_tracer_xing* n)
{
	for(int i = 0; i < bad_neighbors.size(); i++)
	{
		if(n == bad_neighbors[i])
		{
			return true;
		}
	}
	return false;
}


dbdet_contour_tracer_grid::dbdet_contour_tracer_grid(int width, int height, Xings* xings)
{
	width_ = width;
	height_ = height;
	grid_points_ = new vcl_vector<dbdet_contour_tracer_xing*>[width*height];
	horiz_lines_ = new vcl_vector<dbdet_contour_tracer_xing*>[width*height];
	vert_lines_ = new vcl_vector<dbdet_contour_tracer_xing*>[width*height];

	int pos;
	double xp, yp;
	double epsilon1 = 0.00001;
	int x_index;
	int y_index;

	for(int y=0; y<height; y++)
	{
		for(int x=0; x<width; x++)
		{
			int pos = y*width+x;
			if (xings->vert[pos].loc[0] >=0)
			{
				yp = xings->vert[pos].loc[0];
				xp = x;
				x_index = x;
				int yr = (int)(yp+0.5);
				double rem_yr = fabs(yp-yr);
				if (rem_yr < epsilon1)
				{
					yp = yr;
					y_index = yr;
					if(get_grid_points(x_index, y_index).empty())
					{
						get_grid_points(x_index, y_index).push_back(new dbdet_contour_tracer_xing(xp, yp, XING_GRID));
					}
				}
				else
				{
					y_index = (int) yp;
					get_vert_points(x_index, y_index).push_back(new dbdet_contour_tracer_xing(xp, yp, XING_VERT));
				}
			}
			if (xings->vert[pos].loc[1] >=0)
			{
				yp = xings->vert[pos].loc[1];
				xp = (double) x;
				x_index = x;
				int yr = (int)(yp+0.5);
				double rem_yr = fabs(yp-yr);
				if (rem_yr < epsilon1)
				{
					yp = yr;
					y_index = yr;
					if(get_grid_points(x_index, y_index).empty())
					{
						get_grid_points(x_index, y_index).push_back(new dbdet_contour_tracer_xing(xp, yp, XING_GRID));
					}
				}
				else
				{
					y_index = (int) yp;
					get_vert_points(x_index, y_index).push_back(new dbdet_contour_tracer_xing(xp, yp, XING_VERT));
				}
			}
			if (xings->horiz[pos].loc[0] >=0) {
				yp = (double) y;
				xp = (double) xings->horiz[pos].loc[0];
				y_index = y;
				int xr = (int)(xp+0.5);
				double rem_xr = fabs(xp-xr);
				if (rem_xr < epsilon1)
				{
					xp = xr;
					x_index = xr;
					if(get_grid_points(x_index, y_index).empty())
					{
						get_grid_points(x_index, y_index).push_back(new dbdet_contour_tracer_xing(xp, yp, XING_GRID));
					}
				}
				else
				{
					x_index = (int) xp;
					get_horiz_points(x_index, y_index).push_back(new dbdet_contour_tracer_xing(xp, yp, XING_HORIZ));
				}
			}
			if (xings->horiz[pos].loc[1] >=0) {
				yp = (double) y;
				xp = (double) xings->horiz[pos].loc[1];
				y_index = y;
				int xr = (int)(xp+0.5);
				double rem_xr = fabs(xp-xr);
				if (rem_xr < epsilon1)
				{
					xp = xr;
					x_index = xr;
					if(get_grid_points(x_index, y_index).empty())
					{
						get_grid_points(x_index, y_index).push_back(new dbdet_contour_tracer_xing(xp, yp, XING_GRID));
					}
				}
				else
				{
					x_index = (int) xp;
					get_horiz_points(x_index, y_index).push_back(new dbdet_contour_tracer_xing(xp, yp, XING_HORIZ));
				}
			}
		}
	}
}

vcl_vector<dbdet_contour_tracer_xing*>& dbdet_contour_tracer_grid::get_grid_points(int x, int y)
{
	return grid_points_[y*width_ + x];

}

vcl_vector<dbdet_contour_tracer_xing*>& dbdet_contour_tracer_grid::get_horiz_points(int x, int y)
{
	return horiz_lines_[y*width_ + x];
}

vcl_vector<dbdet_contour_tracer_xing*>& dbdet_contour_tracer_grid::get_vert_points(int x, int y)
{
	return vert_lines_[y*width_ + x];
}

void add_untraced_points(vcl_vector<dbdet_contour_tracer_xing*>& next_point_candidates, vcl_vector<dbdet_contour_tracer_xing*>& A)
{
	for(int i = 0; i < A.size(); i++)
	{
		if(!A[i]->traced)
		{
			next_point_candidates.push_back(A[i]);
		}
	}
}

void dbdet_contour_tracer_grid::get_next_point_candidates(dbdet_contour_tracer_xing* p, vcl_vector<dbdet_contour_tracer_xing*>& next_point_candidates)
{
	int xx = (int)p->x;
	int yy = (int)p->y;
	switch(p->type)
	{
	case XING_HORIZ:
	{
		vcl_vector<dbdet_contour_tracer_xing*>& G_0_0 = get_grid_points(xx, yy);
		add_untraced_points(next_point_candidates, G_0_0);
		vcl_vector<dbdet_contour_tracer_xing*>& H_0_0 = get_horiz_points(xx, yy);
		add_untraced_points(next_point_candidates, H_0_0);
		vcl_vector<dbdet_contour_tracer_xing*>& V_0_0 = get_vert_points(xx, yy);
		add_untraced_points(next_point_candidates, V_0_0);

		vcl_vector<dbdet_contour_tracer_xing*>& G_0_N1 = get_grid_points(xx, yy-1);
		add_untraced_points(next_point_candidates, G_0_N1);
		vcl_vector<dbdet_contour_tracer_xing*>& H_0_N1 = get_horiz_points(xx, yy-1);
		add_untraced_points(next_point_candidates, H_0_N1);
		vcl_vector<dbdet_contour_tracer_xing*>& V_0_N1 = get_vert_points(xx, yy-1);
		add_untraced_points(next_point_candidates, V_0_N1);

		vcl_vector<dbdet_contour_tracer_xing*>& G_0_P1 = get_grid_points(xx, yy+1);
		add_untraced_points(next_point_candidates, G_0_P1);
		vcl_vector<dbdet_contour_tracer_xing*>& H_0_P1 = get_horiz_points(xx, yy+1);
		add_untraced_points(next_point_candidates, H_0_P1);

		vcl_vector<dbdet_contour_tracer_xing*>& G_P1_N1 = get_grid_points(xx+1, yy-1);
		add_untraced_points(next_point_candidates, G_P1_N1);
		vcl_vector<dbdet_contour_tracer_xing*>& V_P1_N1 = get_vert_points(xx+1, yy-1);
		add_untraced_points(next_point_candidates, V_P1_N1);

		vcl_vector<dbdet_contour_tracer_xing*>& G_P1_0 = get_grid_points(xx+1, yy);
		add_untraced_points(next_point_candidates, G_P1_0);
		vcl_vector<dbdet_contour_tracer_xing*>& V_P1_0 = get_vert_points(xx+1, yy);
		add_untraced_points(next_point_candidates, V_P1_0);

		vcl_vector<dbdet_contour_tracer_xing*>& G_P1_P1 = get_grid_points(xx+1, yy+1);
		add_untraced_points(next_point_candidates, G_P1_P1);
	}
	break;

	case XING_VERT:
	{
		vcl_vector<dbdet_contour_tracer_xing*>& G_0_0 = get_grid_points(xx, yy);
		add_untraced_points(next_point_candidates, G_0_0);
		vcl_vector<dbdet_contour_tracer_xing*>& H_0_0 = get_horiz_points(xx, yy);
		add_untraced_points(next_point_candidates, H_0_0);
		vcl_vector<dbdet_contour_tracer_xing*>& V_0_0 = get_vert_points(xx, yy);
		add_untraced_points(next_point_candidates, V_0_0);

		vcl_vector<dbdet_contour_tracer_xing*>& G_N1_0 = get_grid_points(xx-1, yy);
		add_untraced_points(next_point_candidates, G_N1_0);
		vcl_vector<dbdet_contour_tracer_xing*>& H_N1_0 = get_horiz_points(xx-1, yy);
		add_untraced_points(next_point_candidates, H_N1_0);
		vcl_vector<dbdet_contour_tracer_xing*>& V_N1_0 = get_vert_points(xx-1, yy);
		add_untraced_points(next_point_candidates, V_N1_0);

		vcl_vector<dbdet_contour_tracer_xing*>& G_P1_0 = get_grid_points(xx+1, yy);
		add_untraced_points(next_point_candidates, G_P1_0);
		vcl_vector<dbdet_contour_tracer_xing*>& V_P1_0 = get_vert_points(xx+1, yy);
		add_untraced_points(next_point_candidates, V_P1_0);

		vcl_vector<dbdet_contour_tracer_xing*>& G_0_P1 = get_grid_points(xx, yy+1);
		add_untraced_points(next_point_candidates, G_0_P1);
		vcl_vector<dbdet_contour_tracer_xing*>& H_0_P1 = get_horiz_points(xx, yy+1);
		add_untraced_points(next_point_candidates, H_0_P1);

		vcl_vector<dbdet_contour_tracer_xing*>& G_N1_P1 = get_grid_points(xx-1, yy+1);
		add_untraced_points(next_point_candidates, G_N1_P1);
		vcl_vector<dbdet_contour_tracer_xing*>& H_N1_P1 = get_horiz_points(xx-1, yy+1);
		add_untraced_points(next_point_candidates, H_N1_P1);

		vcl_vector<dbdet_contour_tracer_xing*>& G_P1_P1 = get_grid_points(xx+1, yy+1);
		add_untraced_points(next_point_candidates, G_P1_P1);
	}
	break;

	case XING_GRID:
	{
		vcl_vector<dbdet_contour_tracer_xing*>& G_N1_N1 = get_grid_points(xx-1, yy-1);
		add_untraced_points(next_point_candidates, G_N1_N1);
		vcl_vector<dbdet_contour_tracer_xing*>& H_N1_N1 = get_horiz_points(xx-1, yy-1);
		add_untraced_points(next_point_candidates, H_N1_N1);
		vcl_vector<dbdet_contour_tracer_xing*>& V_N1_N1 = get_vert_points(xx-1, yy-1);
		add_untraced_points(next_point_candidates, V_N1_N1);

		vcl_vector<dbdet_contour_tracer_xing*>& G_0_N1 = get_grid_points(xx, yy-1);
		add_untraced_points(next_point_candidates, G_0_N1);
		vcl_vector<dbdet_contour_tracer_xing*>& H_0_N1 = get_horiz_points(xx, yy-1);
		add_untraced_points(next_point_candidates, H_0_N1);
		vcl_vector<dbdet_contour_tracer_xing*>& V_0_N1 = get_vert_points(xx, yy-1);
		add_untraced_points(next_point_candidates, V_0_N1);

		vcl_vector<dbdet_contour_tracer_xing*>& G_N1_0 = get_grid_points(xx-1, yy);
		add_untraced_points(next_point_candidates, G_N1_0);
		vcl_vector<dbdet_contour_tracer_xing*>& H_N1_0 = get_horiz_points(xx-1, yy);
		add_untraced_points(next_point_candidates, H_N1_0);
		vcl_vector<dbdet_contour_tracer_xing*>& V_N1_0 = get_vert_points(xx-1, yy);
		add_untraced_points(next_point_candidates, V_N1_0);

		vcl_vector<dbdet_contour_tracer_xing*>& G_0_0 = get_grid_points(xx, yy);
		add_untraced_points(next_point_candidates, G_0_0);
		vcl_vector<dbdet_contour_tracer_xing*>& H_0_0 = get_horiz_points(xx, yy);
		add_untraced_points(next_point_candidates, H_0_0);
		vcl_vector<dbdet_contour_tracer_xing*>& V_0_0 = get_vert_points(xx, yy);
		add_untraced_points(next_point_candidates, V_0_0);

		vcl_vector<dbdet_contour_tracer_xing*>& G_P1_N1 = get_grid_points(xx+1, yy-1);
		add_untraced_points(next_point_candidates, G_P1_N1);
		vcl_vector<dbdet_contour_tracer_xing*>& V_P1_N1 = get_vert_points(xx+1, yy-1);
		add_untraced_points(next_point_candidates, V_P1_N1);

		vcl_vector<dbdet_contour_tracer_xing*>& G_P1_0 = get_grid_points(xx+1, yy);
		add_untraced_points(next_point_candidates, G_P1_0);
		vcl_vector<dbdet_contour_tracer_xing*>& V_P1_0 = get_vert_points(xx+1, yy);
		add_untraced_points(next_point_candidates, V_P1_0);

		vcl_vector<dbdet_contour_tracer_xing*>& G_N1_P1 = get_grid_points(xx-1, yy+1);
		add_untraced_points(next_point_candidates, G_N1_P1);
		vcl_vector<dbdet_contour_tracer_xing*>& H_N1_P1 = get_horiz_points(xx-1, yy+1);
		add_untraced_points(next_point_candidates, H_N1_P1);

		vcl_vector<dbdet_contour_tracer_xing*>& G_0_P1 = get_grid_points(xx, yy+1);
		add_untraced_points(next_point_candidates, G_0_P1);
		vcl_vector<dbdet_contour_tracer_xing*>& H_0_P1 = get_horiz_points(xx, yy+1);
		add_untraced_points(next_point_candidates, H_0_P1);

		vcl_vector<dbdet_contour_tracer_xing*>& G_P1_P1 = get_grid_points(xx+1, yy+1);
		add_untraced_points(next_point_candidates, G_P1_P1);
	}
	break;
	}
}

bool _test_intersection(dbdet_contour_tracer_xing *p1, dbdet_contour_tracer_xing *p2, dbdet_contour_tracer_xing *p3, dbdet_contour_tracer_xing *p4)
{
	double ua1 = ((p4->x - p3->x)*(p1->y - p3->y) - (p4->y - p3->y)*(p1->x - p3->x));
	double ua2 = ((p4->y - p3->y)*(p2->x - p1->x) - (p4->x - p3->x)*(p2->y - p1->y));
	double ua = 10;
	if(ua2 != 0)
	{
		ua = ua1/ua2;
	}
	return ua >= 0 && ua <= 1;
}

bool _make_bad_neighbors(dbdet_contour_tracer_xing *p1, dbdet_contour_tracer_xing *p2, dbdet_contour_tracer_xing *p3, dbdet_contour_tracer_xing *p4)
{
	bool intersection_flag = _test_intersection(p1,p2,p3,p4);
	if(intersection_flag)
	{
		p1->add_bad_neighbor(p2);
		p2->add_bad_neighbor(p1);
		p3->add_bad_neighbor(p4);
		p4->add_bad_neighbor(p3);
	}
	return intersection_flag;
}

void dbdet_contour_tracer_grid::determine_unambiguous_and_bad_neighbors()
{
	for(int yy = 0; yy < height_-1; yy++)
	{
		for(int xx = 0; xx < width_-1; xx++)
		{
			vcl_vector<dbdet_contour_tracer_xing*> points;
			vcl_vector<dbdet_contour_tracer_xing*>& G_0_0 = get_grid_points(xx, yy);
			add_untraced_points(points, G_0_0);
			vcl_vector<dbdet_contour_tracer_xing*>& H_0_0 = get_horiz_points(xx, yy);
			add_untraced_points(points, H_0_0);
			vcl_vector<dbdet_contour_tracer_xing*>& V_0_0 = get_vert_points(xx, yy);
			add_untraced_points(points, V_0_0);

			vcl_vector<dbdet_contour_tracer_xing*>& G_0_P1 = get_grid_points(xx, yy+1);
			add_untraced_points(points, G_0_P1);
			vcl_vector<dbdet_contour_tracer_xing*>& H_0_P1 = get_horiz_points(xx, yy+1);
			add_untraced_points(points, H_0_P1);

			vcl_vector<dbdet_contour_tracer_xing*>& G_P1_0 = get_grid_points(xx+1, yy);
			add_untraced_points(points, G_P1_0);
			vcl_vector<dbdet_contour_tracer_xing*>& V_P1_0 = get_vert_points(xx+1, yy);
			add_untraced_points(points, V_P1_0);

			vcl_vector<dbdet_contour_tracer_xing*>& G_P1_P1 = get_grid_points(xx+1, yy+1);
			add_untraced_points(points, G_P1_P1);

			if(points.size() == 2)
			{
				dbdet_contour_tracer_xing *p1 = points[0];
				dbdet_contour_tracer_xing *p2 = points[1];
				p1->add_unambiguous_neighbor(p2);
				p2->add_unambiguous_neighbor(p1);
			}
			else if(points.size() == 4)
			{
				//vcl_cout << xx << " " << yy << vcl_endl;
				bool intersection_flag;
				dbdet_contour_tracer_xing *p1, *p2, *p3, *p4;
				p1 = points[0];	p2 = points[1];	p3 = points[2];	p4 = points[3];
				//1-2 3-4
				intersection_flag = _make_bad_neighbors(p1,p2,p3,p4);
				if(!intersection_flag)
				{
					//1-3 2-4
					intersection_flag = _make_bad_neighbors(p1,p3,p2,p4);
					if(!intersection_flag)
					{
						//2-3 1-4
						_make_bad_neighbors(p2,p3,p1,p4);
					}
				}
			}
			/*else if(points.size() != 0)
			{
				vcl_cout << "(" << xx << ", " << yy << "): num_points = " << points.size() << vcl_endl;
			}*/
		}
	}
}

void dbdet_contour_tracer_grid::get_an_unambiguous_point_pair(int& srow, dbdet_contour_tracer_xing*& p1, dbdet_contour_tracer_xing*& p2)
{
	p1 = 0;
	p2 = 0;
	for(int yy = srow; yy < height_ && !p1; yy++)
	{
		for(int xx = 0; xx < width_ && !p1; xx++)
		{
			vcl_vector<dbdet_contour_tracer_xing*>& H = get_horiz_points(xx, yy);
			for(int i = 0; i < H.size() && !p1; i++)
			{
				if(!H[i]->traced)
				{
					dbdet_contour_tracer_xing* unambiguous_neighbor = H[i]->get_untraced_unambiguous_neighbor();
					if(unambiguous_neighbor)
					{
						p1 = H[i];
						p2 = unambiguous_neighbor;
					}
				}
			}
			if(!p1)
			{
				vcl_vector<dbdet_contour_tracer_xing*>& V = get_vert_points(xx, yy);
				for(int i = 0; i < V.size() && !p1; i++)
				{
					if(!V[i]->traced)
					{
						dbdet_contour_tracer_xing* unambiguous_neighbor = V[i]->get_untraced_unambiguous_neighbor();
						if(unambiguous_neighbor)
						{
							p1 = V[i];
							p2 = unambiguous_neighbor;
						}
					}
				}
			}
			if(!p1)
			{
				vcl_vector<dbdet_contour_tracer_xing*>& G = get_grid_points(xx, yy);
				for(int i = 0; i < G.size() && !p1; i++)
				{
					if(!G[i]->traced)
					{
						dbdet_contour_tracer_xing* unambiguous_neighbor = G[i]->get_untraced_unambiguous_neighbor();
						if(unambiguous_neighbor)
						{
							p1 = G[i];
							p2 = unambiguous_neighbor;
						}
					}
				}
			}
		}
		srow = yy;
	}
}

void dbdet_contour_tracer_grid::remove_all_traced_points()
{
	for(int y = 0; y < height_; y++)
	{
		for(int x = 0; x < width_; x++)
		{
			//GRID
			vcl_vector<dbdet_contour_tracer_xing*>& G = get_grid_points(x, y);
			if(!G.empty() && G[0]->traced)
			{

				G[0]->remove_all_unambiguous_neighbors();
				delete G[0];
				G.clear();
			}
			if(x != width_-1) //HORIZ
			{
				vcl_vector<dbdet_contour_tracer_xing*>& H = get_horiz_points(x, y);
				for(int i = 0; i < H.size(); i++)
				{
					if (H[i]->traced)
					{
						H[i]->remove_all_unambiguous_neighbors();
						delete H[i];
						H.erase(H.begin()+i);
						i--;
					}
				}
			}
			if(y != height_-1) //VERT
			{
				vcl_vector<dbdet_contour_tracer_xing*>& V = get_vert_points(x, y);
				for(int i = 0; i < V.size(); i++)
				{
					if (V[i]->traced)
					{
						V[i]->remove_all_unambiguous_neighbors();
						delete V[i];
						V.erase(V.begin()+i);
						i--;
					}
				}
			}
		}
	}
}

bool dbdet_contour_tracer_grid::get_associated_cell_ids(dbdet_contour_tracer_xing* p, vcl_vector<int>& idx, vcl_vector<int>& idy)
{
	int xx = p->x;
	int yy = p->y;
	idx.push_back(xx);
	idy.push_back(yy);
	switch(p->type)
	{
	case XING_HORIZ:
	{
		if(yy > 0)
		{
			idx.push_back(xx);
			idy.push_back(yy-1);
		}
	}
	break;
	case XING_VERT:
	{
		if(xx > 0)
		{
			idx.push_back(xx-1);
			idy.push_back(yy);
		}
	}
	break;
	case XING_GRID:
	{
		if(xx > 0 && yy > 0)
		{
			idx.push_back(xx-1);
			idy.push_back(yy);
			idx.push_back(xx);
			idy.push_back(yy-1);
			idx.push_back(xx-1);
			idy.push_back(yy-1);
		}
		else if(xx > 0)
		{
			idx.push_back(xx-1);
			idy.push_back(yy);
		}
		else if(yy > 0)
		{
			idx.push_back(xx);
			idy.push_back(yy-1);
		}
	}
	break;
	}
	return true;
}

bool dbdet_contour_tracer_grid::belong_to_same_cell(dbdet_contour_tracer_xing* p1, dbdet_contour_tracer_xing* p2)
{

	vcl_vector<int> idx1, idy1, idx2, idy2;
	get_associated_cell_ids(p1, idx1, idy1);
	get_associated_cell_ids(p2, idx2, idy2);
	for(int i = 0; i < idx1.size(); i++)
	{
		for(int j = 0; j < idx2.size(); j++)
		{
			if(idx1[i] == idx2[j] && idy1[i] == idy2[j])
			{
				return true;
			}
		}
	}
	return false;
}

dbdet_contour_tracer_grid::~dbdet_contour_tracer_grid()
{
	for(int y = 0; y < height_; y++)
	{
		for(int x = 0; x < width_; x++)
		{
			//GRID
			vcl_vector<dbdet_contour_tracer_xing*>& G = get_grid_points(x, y);
			if(!G.empty())
			{
				delete G[0];
				G.clear();
			}
			if(x != width_-1) //HORIZ
			{
				vcl_vector<dbdet_contour_tracer_xing*>& H = get_horiz_points(x, y);
				for(int i = 0; i < H.size(); i++)
				{
					delete H[i];
				}
				H.clear();
			}
			if(y != height_-1) //VERT
			{
				vcl_vector<dbdet_contour_tracer_xing*>& V = get_vert_points(x, y);
				for(int i = 0; i < V.size(); i++)
				{
					delete V[i];
				}
				V.clear();
			}
		}
	}
}
