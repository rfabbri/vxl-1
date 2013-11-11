p_pts=poly_polar_grid(xy0,[0 0]);
[xx,yy] = pol2cart(p_pts(:,1),p_pts(:,2));

p_pts1=poly_polar_grid(xy1,[0 0]);
[xx1,yy1] = pol2cart(p_pts1(:,1),p_pts1(:,2));

dmin = Inf;imin = -1;
for ip=1:size(p_pts,1); 
  d = norm([xx(ip); yy(ip)] - e0);
  if d < dmin
    dmin = d;
    imin = ip;
  end
end
dmin
imin

for ip=1:size(p_pts,1); 
    for ip1=1:size(p_pts1,1)
     [d2,e0_best,e1_best,epi_s] = epioptimize_polar2_function(pts0,pts1,[xx(ip); yy(ip);xx1(ip1); yy1(ip1)],'precise'); 
     [d1,e0_best,e1_best,epi_s] = epioptimize_polar_function(pts0,pts1,[xx(ip); yy(ip);xx1(ip1); yy1(ip1)],'precise'); 
     [d0,e0_best,e1_best,epi_s] = epioptimize_function(pts0,pts1,[xx(ip); yy(ip);xx1(ip1); yy1(ip1)],'precise'); 
%     [d2,e0_best,e1_best,epi_s] = epioptimize_polar2_function(pts0,pts1,[xx(ip); yy(ip);e1(1); e1(2)],'precise'); 
 %    [d1,e0_best,e1_best,epi_s] = epioptimize_polar_function(pts0,pts1,[xx(ip); yy(ip);e1(1); e1(2)],'precise'); 
  %   [d0,e0_best,e1_best,epi_s] = epioptimize_function(pts0,pts1,[xx(ip); yy(ip);e1(1); e1(2)],'precise'); 
     ip, norm([xx(ip); yy(ip)] - e0),
     
     disp([d2 d1 d0]);
     if d2 < 0.0001 || d1 < 0.0001 || d0 < 0.0001
        keyboard;
     end
    end
end