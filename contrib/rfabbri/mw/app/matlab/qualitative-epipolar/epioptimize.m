% To be called after epi_polycost.m

% Input: ip_opt, is0_opt, is1_opt  :  indices of polygon sheets within which to
% perform optimization

%ip_opt = sorted_ip(end);
%is0_opt = isheet_vcost(ip_opt,1);
%is1_opt = isheet_vcost(ip_opt,2);

%opt_poly0 = polys0{ip_opt}{is0_opt};
%opt_poly1 = polys1{ip_opt}{is1_opt};

%c0 = [mean(opt_poly0(:,1)); mean(opt_poly0(:,2))];
%c1 = [mean(opt_poly1(:,1)); mean(opt_poly1(:,2))]; 

ev_ini = [c0; c1];

%epioptimize_f;
epioptimize_f_geometric;
%epioptimize_f(ev_ini,ip_opt,is0_opt,is1_opt);

%delete_hopt;
%plot_opt;
