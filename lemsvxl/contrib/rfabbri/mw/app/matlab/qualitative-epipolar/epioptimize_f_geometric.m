% Function to perform optimization within specified corresponding polygon sheets

% Input: ip_opt, is0_opt, is1_opt  :  indices of polygon sheets within which to
% perform optimization

%function epioptimize_f(ev_ini,ip_opt,is0_opt,is1_opt)

%opt_poly0 = polys0{ip_opt}{is0_opt};
%opt_poly1 = polys1{ip_opt}{is1_opt};

% initial structure
r0 = max(norm(e0),1);
r1 = max(norm(e1),1);
epi_s = epi_init([ev_ini(1);ev_ini(2)] , [ev_ini(3);ev_ini(4)],r0,r1,zeros(2,2));

% initial homography
[h,res] = epi_estimate_h_points(epi_s, pts0, pts1);
epi_s.h = h;

[d,retval,n_inst,vv] = epi_geometric_error(epi_s,pts0,pts1);
clear n_inst;

npts = size(pts0,1);

disp (['Initial geometric error: ' show_error(vv)]);

vv_i = ep_f_geometric(ev_ini,pts0,pts1);


disp (['Initial geometric er(2): ' show_error(vv_i)]);

%disp ('vcost for optimal polys (should equal the above):');

%disp(vcost(ip_opt));

%opt = optimset('TolFun',1e-5,'MaxFunEvals',30,'MaxIter',100,'Display','iter');
opt = optimset('TolFun',1e-5,'MaxFunEvals',30,'MaxIter',100);
%opt = optimset('TolFun',1e-9,'MaxFunEvals',50,'MaxIter',20,'Display','iter');
%opt = optimset('Display','iter');

%bbox0 = [min(opt_poly0(:,1)) min(opt_poly0(:,2));
%         max(opt_poly0(:,1)) max(opt_poly0(:,2)) ];

%bbox1 = [min(opt_poly1(:,1)) min(opt_poly1(:,2));
%         max(opt_poly1(:,1)) max(opt_poly1(:,2)) ];

%lb = [bbox0(1,1); bbox0(1,2); bbox1(1,1); bbox1(1,2)];
%ub = [bbox0(2,1); bbox0(2,2); bbox1(2,1); bbox1(2,2)];

%[ev_opt,res] = lsqnonlin(@(ev) ep_f_geometric(ev,pts0,pts1),ev_ini,lb,ub,opt);
[ev_opt,res] = lsqnonlin(@(ev) ep_f_geometric(ev,pts0,pts1),ev_ini,[],[],opt);

vv_f = ep_f_geometric(ev_opt,pts0,pts1);

disp (['Initial geometric er(2): ' show_error(vv_i)]);
disp (['Final   geometric error: ' show_error(vv_f)]);

if max(size(e0)) ~= 0
  vv_gt = ep_f_geometric([e0(1); e0(2); e1(1); e1(2)],pts0,pts1);
  disp (['Truth geometric error: ' show_error(vv_gt)]);
end

%if max(size(e0_new)) ~= 0
%  vv_8pt = ep_f_geometric([e0_new(1); e0_new(2); e1_new(1); e1_new(2)],pts0,pts1);
%  disp (['8pt   geometric error: ' show_error(vv_8pt)]);
%end
