Experiments in Qualitative Epipolar Geometry

epi - toplevel function to compute the polygon partitions and plot them, given
many different types of data that can be set in the beginning of the script

epi_polycost - to be called after epi, this rank-orders the polygons based on
some criteria (the cost of centroid, if the function four_line_poly_test is used
to evaluate the cost of two polygons, or the cost of centroid after a gradient
descent, if epi_cost_of_polygons is used). It shows the highest-ranking
polygons, and the remaining ones with intensity proportional to cos

plot_polys - can be called either after epi.m or epi_polycost.m to show the
polygons

episearch - generage polygons, given two vectors of points pts0 and pts1, and a
bounding box representing the image. This calls a C++ function

fmatrix - given 8 corresponding points, it returns a fundamental matrix. This
calls a C++ function

epi_polycost_f - given two polygon partitions polys0 and polys1, pts0, and pts1,
this matlab script returns the costs of the polygons based on ?centroid? and a
?gradient descent?

