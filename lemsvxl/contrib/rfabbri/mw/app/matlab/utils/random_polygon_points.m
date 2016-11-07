function rand_pts = random_polygon_points(poly,npts)
  max_nruns = 10000;



  x = poly(:,1);
  y = poly(:,2);

  if polyarea(x,y) < 1e-7
    disp(['thin polygon - not using random points within']);
    rand_pts = [];
    return;
  end

  min_x = min(x);
  min_y = min(y);
  max_x = max(x);
  max_y = max(y);

  rand_pts = zeros(npts,2);
  for i=1:npts
    in_poly = false;
    n_runs = 0;
    while in_poly == false
      n_runs = n_runs + 1;
      xrand = (max_x-min_x)*rand() + min_x;
      yrand = (max_y-min_y)*rand() + min_y;
      if inpolygon(xrand,yrand,x,y)
        in_poly = true;
        rand_pts(i,1) = xrand;
        rand_pts(i,2) = yrand;
      end
      if n_runs > max_nruns
        disp(['Too many runs without results - thin polygon with area = ' num2str(polyarea(x,y))]);
        if i > 1
          error('Strangely, i > 1 for a sliver polygon');
        end
        rand_pts = [];
        return;
      end
    end
  end
