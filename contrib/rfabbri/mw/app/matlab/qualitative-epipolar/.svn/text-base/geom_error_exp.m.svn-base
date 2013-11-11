% Generates curves of geometric error versus standard deviation of localization
% noise

clear all;
e0 = [];
e0_new = [];
plot_costs = false;
show_centroids = false;
n_fig_poly_corresp = 1;


sigma_v = 0:0.25:4;
%sigma_v = [4 4.5];
n_sigma = max(size(sigma_v));


% nruns per sigma
nruns = 1000;


datascript = 'synth_point_data';
%angles_s = ['30,60'];
angles_s = ['30,' num2str(30+180)]
%angles_s = ['30,' num2str(30+90)]


algs = {'newalg','8ptalg'};

for alg = algs
  disp(['Algorithm: ' alg]);

  fv_meancost = zeros(1,n_sigma);
  fv_maxcost = zeros(1,n_sigma);

  for is=1:n_sigma
    sig = sigma_v(is);
    eval(datascript);
    if sig == 0
      if strcmp(alg, 'newalg')
        [epi_s,top_poly_idx,polys0,polys1,vcost,isheet_vcost] = epi_estimate(pts0,pts1,box);
        [d,retval,n_inst,vv_newalg_minset] = epi_geometric_error(epi_s,pts0,pts1);
        vmeancost = vv_newalg_minset;
        vmaxcost = vv_newalg_minset;
      else
        fm = fmatrix(pts0,pts1,'vpgl8pt');
        fm8pt = fm;
        fm_fail = eye(3);
        fm_fail(3,3)=0;
        if norm(fm-fm_fail) < 0.0001
          error('attempt to run 8 pt algorithm failed');
        end
        [d,vv_8pt_minset] = epi_geometric_error_f(fm8pt,pts0,pts1);
        vmeancost = vv_8pt_minset;
        vmaxcost = vv_8pt_minset;
      end
    else
      if strcmp(alg,'newalg')
        epipole_position_exp_noise_newalg;
      else
        epipole_position_exp_noise;
      end
    end
    fv_meancost(is) = mean(vmeancost);
    fv_maxcost(is)  = max(vmaxcost);
  end
  if strcmp(alg,'newalg')
    fv_meancost_newalg = fv_meancost;
    fv_maxcost_newalg = fv_maxcost;
  elseif strcmp(alg,'8ptalg')
    fv_meancost_8pt = fv_meancost;
    fv_maxcost_8pt = fv_maxcost;
  end
end

mytimestamp = datestr(now,'yyyymmdd');
out_dir = 'data/stability/';
fname = [out_dir 'geom-err-exp-nruns_' num2str(nruns) '-angles_' angles_s '-sigma_dot25.mat']

save(fname,'fv_meancost_newalg','fv_maxcost_newalg','fv_meancost_8pt','fv_maxcost_8pt','sigma_v','nruns','datascript','angles_s');


%% Useful things to do after this script runs:
%
%
%
% save('data/stability/geom-err-exp-20071031.mat','fv_meancost_newalg','fv_maxcost_newalg','fv_meancost_8pt','fv_maxcost_8pt','sigma_v','nruns','datascript','angles_s');

% figure(); plot(sigma_v,fv_maxcost_newalg,sigma_v,fv_maxcost_8pt);
% title 'max geometric error'
% figure(); plot(sigma_v,fv_meancost_newalg,sigma_v,fv_maxcost_8pt);
% title 'mean geometric error'
