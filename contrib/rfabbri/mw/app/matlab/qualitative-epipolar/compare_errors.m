% To be called after epi_polycost.m

% Input: ip_opt, is0_opt, is1_opt  :  indices of polygon sheets within which to
% perform optimization

if ~exist('epi_verbose');
  epi_verbose = true;
end

geom_err_max = [];
geom_err_avg = [];
for i=0:min(15,max(size(sorted_ip))-1)
  ip_opt    = sorted_ip(end-i);
  is0_opt   = isheet_vcost(ip_opt,1);
  is1_opt   = isheet_vcost(ip_opt,2);

  opt_poly0 = polys0{ip_opt}{is0_opt};
  opt_poly1 = polys1{ip_opt}{is1_opt};

  [d_tmp,e0_best,e1_best,epi_s] = epi_cost_of_polygon(opt_poly0,opt_poly1,pts0,pts1);

  [d,retval,n_inst,vv] = epi_geometric_error(epi_s,ap0,ap1);
  
  if (epi_verbose)
    disp (['Poly  geometric error: ' show_error(vv)]);
    disp (['      vcost for optimal polys (should equal the above ssq): ' num2str(vcost(ip_opt))]);
  end
  geom_err_max(end+1) = max(vv);
  geom_err_avg(end+1) = mean(vv);
end
geom_err_newalg = min(geom_err_max);
geom_err_newalg_max = min(geom_err_max);
geom_err_newalg_avg = min(geom_err_avg);


if max(size(e0)) ~= 0
  vv_gt = ep_f_geometric([e0(1); e0(2); e1(1); e1(2)],ap0,ap1);
  if epi_verbose
    disp (['Truth geometric error: ' show_error(vv_gt)]);
  end
end

if max(size(e0_new)) ~= 0
  [d,vv_8pt] = epi_geometric_error_f(fm8pt,ap0,ap1);
  if epi_verbose
    disp (['8pt error: ' show_error(vv_8pt)]);
  end
end
geom_err_8pt     = max(vv_8pt);
geom_err_8pt_max = max(vv_8pt);
geom_err_8pt_avg = mean(vv_8pt);

if epi_verbose
  disp(' ');
  disp(['FINAL COMPARISON: newalg avg: ' num2str(geom_err_newalg_avg) ' max: ' num2str(geom_err_newalg_max)...
        ',  8pt avg: ' num2str(geom_err_8pt_avg) ' max: ' num2str(geom_err_8pt_max) ]);
end
