%p_id = [801   332   748   540   269   951   824 27]
% sig=0
%epi;
%epi_polycost;

%specific_poly1 = polys1{57}
%specific_poly0 = polys0{57}

%epifail_plot

%[d,e0_best,e1_best,epi_s] = epioptimize_polar_function(pts0,pts1,[c0; c1],'precise');

%[d,retval,n_inst,vv]=epi_geometric_error(epi_s,pts0,pts1);mean(vv)

%d=0; n_it=0; 
%while d < 0.1 && n_it < 100; 
%  n_it = n_it + 1; 
%  [d,e0_best,e1_best,epi_s] = epioptimize_polar_function(pts0,pts1,[e0 e1+1000*randn(1,2)]','precise');
%end



%% Draw a line from c0 to e0, c1 to e1

%e0 = e0';
%e1 = e1';
d0 = e0;
d0 = d0/norm(d0);

d1 = e1;
d1 = d1/norm(d1);

% Walk quadratically along this line

%vradius = [0,1,10,100,1000,5000,10000];
vradius = 7500:100:8000;
nr = max(size(vradius));
for ir=1:nr;
  disp(['radius = ' num2str(vradius(ir))]);

%  p0 = e0+d0*vradius(ir);
  p0 = e0;
  p1 = e1+d1*vradius(ir);

  [d,e0_best,e1_best,epi_s] = epioptimize_polar_function(pts0,pts1,[p0;p1],'precise');

  if (d > 0.1)
    disp('cannot converge anymore');
    break;
  end
end
