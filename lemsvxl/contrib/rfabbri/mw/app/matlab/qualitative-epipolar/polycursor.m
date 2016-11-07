f=gcf;

if f == figure(n_fig_poly_corresp)  || f == figure(n_fig_cost) 
  viewnum = 0;
  polys = polys0;
  disp('Using polys0');
elseif f == figure(n_fig_poly_corresp+1) || f == figure(n_fig_cost+1)
  viewnum = 1;
  polys = polys1;
  disp('Using polys1');
else
  error('Not a poly figure');
end


polycursor_f;
