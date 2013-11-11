% Function to perform optimization within specified corresponding polygon sheets

% Input: ip_opt, is0_opt, is1_opt  :  indices of polygon sheets within which to
% perform optimization

%function epioptimize_f(ev_ini,ip_opt,is0_opt,is1_opt)

opt_poly0 = polys0{ip_opt}{is0_opt};
opt_poly1 = polys1{ip_opt}{is1_opt};

disp('Initial residual (squared norm):');
vv_i = ep_f(ev_ini,pts0,pts1);

res_i = sum(vv_i.*vv_i);
disp(res_i);

%disp ('vcost for optimal polys (should equal the above):');

%disp(vcost(ip_opt));

opt = optimset('TolFun',vcost(ip_opt)/10000,'MaxFunEvals',2000,'Display','iter');
%opt = optimset('Display','iter');

bbox0 = [min(opt_poly0(:,1)) min(opt_poly0(:,2));
         max(opt_poly0(:,1)) max(opt_poly0(:,2)) ];

bbox1 = [min(opt_poly1(:,1)) min(opt_poly1(:,2));
         max(opt_poly1(:,1)) max(opt_poly1(:,2)) ];

lb = [bbox0(1,1); bbox0(1,2); bbox1(1,1); bbox1(1,2)];
ub = [bbox0(2,1); bbox0(2,2); bbox1(2,1); bbox1(2,2)];

%[ev_opt,res] = lsqnonlin(@(ev) ep_f(ev,pts0,pts1),ev_ini,lb,ub,opt);
[ev_opt,res] = lsqnonlin(@(ev) ep_f(ev,pts0,pts1),ev_ini,[],[],opt);

vv_f = ep_f(ev_opt,pts0,pts1);

disp('Final residual (squared norm):');

res_f = sum(vv_f.*vv_f);
disp(res_f);

