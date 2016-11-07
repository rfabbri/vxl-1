% To be called after epi_polycost.m

% Input: ip_opt, is0_opt, is1_opt  :  indices of polygon sheets within which to
% perform optimization

for i=0:min(5,max(size(sorted_ip))-1)

  ip_opts = sorted_ip(end-i);
ip_opt = sorted_ip(end);

is0_opt = isheet_vcost(ip_opt,1);
is1_opt = isheet_vcost(ip_opt,2);

opt_poly0 = polys0{ip_opt}{is0_opt};
opt_poly1 = polys1{ip_opt}{is1_opt};

c0 = [mean(opt_poly0(:,1)); mean(opt_poly0(:,2))];
c1 = [mean(opt_poly1(:,1)); mean(opt_poly1(:,2))]; 

ev_ini = [c0; c1];

%epioptimize_f;
%epioptimize_f_geometric;
%epioptimize_f(ev_ini,ip_opt,is0_opt,is1_opt);

%delete_hopt;
%plot_opt;

% initial structure
r0 = max(norm(c0),1);
r1 = max(norm(c1),1);
epi_s = epi_init([ev_ini(1);ev_ini(2)] , [ev_ini(3);ev_ini(4)],r0,r1,zeros(2,2));

% initial homography
[h,res] = epi_estimate_h_points(epi_s, pts0, pts1);
epi_s.h = h;

[d,retval,n_inst,vv] = epi_geometric_error(epi_s,pts0,pts1);
clear n_inst;

npts = size(pts0,1);

disp (['Poly  geometric error: ' show_error(vv)]);

vv_i = ep_f_geometric(ev_ini,pts0,pts1);


disp (['Poly  geometric er(2): ' show_error(vv_i)]);

disp (['      vcost for optimal polys (should equal the above ssq): ' num2str(vcost(ip_opt))]);

if max(size(e0)) ~= 0
  vv_gt = ep_f_geometric([e0(1); e0(2); e1(1); e1(2)],pts0,pts1);
  disp (['Truth geometric error: ' show_error(vv_gt)]);
end

if max(size(e0_new)) ~= 0
  vv_8pt = ep_f_geometric([e0_new(1); e0_new(2); e1_new(1); e1_new(2)],pts0,pts1);
  disp (['8pt   geometric error: ' show_error(vv_8pt)]);
end
