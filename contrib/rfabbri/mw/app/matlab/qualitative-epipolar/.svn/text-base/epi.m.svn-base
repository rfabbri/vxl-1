clear all;
e0 = [];
e0_new = [];
plot_costs = false;
show_centroids = false;
n_fig_poly_corresp = 1;
n_fig = n_fig_poly_corresp;

% lockheed_data;
% oxford_corridor_point_data
% oxford_house_point_data
pot_fragment_point_data;
% ge_point_data;
% angles_s = ['30,' num2str(30+180)]
% angles_s = ['30,' num2str(30+90)]
% sig=0.1;
% synth_point_data;

% angles_s = '30,60'
% rotate90 = true;
% synth_point_data;
  
% synth_point_data_90deg;
% test_valid_sectors_2 %data
% test_valid_sectors_1_non_degenerate

%---------------------------------------------------------
% Run the algorithm

clear polys0 polys1 fm;
worked = false;
try
  [polys0,polys1] = episearch(pts0,pts1,box);
  worked = true;
catch
  disp('epi: Catched an error in episearch -- degeneracy occured.'); 
  disp(['Error message: ' lasterror.message]);
end

fm = fmatrix(pts0,pts1,'vpgl8pt');
fm8pt = fm;
fm_fail = eye(3);
fm_fail(3,3)=0;
if (norm(fm-fm_fail) > 0.0001) 
  a = null(fm);
  e0_new = [a(1)/a(3) a(2)/a(3)];
  a = null(fm');
  e1_new = [a(1)/a(3) a(2)/a(3)];
end



% ---------------------------------------------
% Plotting


if worked
  plot_polys
%  [cost,vv,n_inst,epi_s] = epi_estimate_h_3pt_epipoles(e0',e1',pts0,pts1);cost,vv
end
