% Compares 8pt algthm vs. centroid of polygons for real data.


real_data_name_defs
epi_verbose = false;

ndata = max(size(data_func_names));

for idata=1:ndata
  disp(['Running experiments on ' data_names{idata} '.']);

  % Load data
  show_synth_point_selection = false;
  show_point_selection = false;
  clear pts0  pts1  ap0  ap1  sig y_invert  angles_s  rotate90  box;
  e0 = []; e0_new = [];
  eval(data_func_names{idata});
  npts = size(ap0,1);

  nruns = 100;

%  vmaxcost_ctroid = zeros(nruns,1);
  vmeancost_8pt = zeros(nruns,1);

  if strcmp(data_func_names{idata}, 'ge_point_data')
    p_id8 = [5    13     6    10    15     8    11     4];
  else
    p_id8_lst =zeros(nruns,8);

%    for r=1:20*nruns
%      p_id8 = randperm(npts);
%    end
    for r=1:nruns
      p_id8 = randperm(npts);
      p_id8 = p_id8(1:8);
      p_id8_lst(r,:) = p_id8;
      pts0 = ap0(p_id8,:);
      pts1 = ap1(p_id8,:);
  %    disp(['Run: ' num2str(p_id8)]);


      clear polys0 polys1 fm;
  %    [polys0,polys1] = episearch(pts0,pts1,box);

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
  %    epi_polycost_f;

      [d,vv_8pt] = epi_geometric_error_f(fm8pt,ap0,ap1);

      geom_err_8pt = mean(vv_8pt);

      vmeancost_8pt(r) = geom_err_8pt;
  %    vmeancost_ctroid(r) = geom_err_newalg;
    end
    [m,min_id] = min(vmeancost_8pt);

    p_id8 = p_id8_lst(min_id,:)
  end


  pts0 = ap0(p_id8,:);
  pts1 = ap1(p_id8,:);
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

  [d,vv_8pt] = epi_geometric_error_f(fm8pt,ap0,ap1);
  d = mean(vv_8pt);
  geom_err_8pt = max(vv_8pt);
  geom_err_8pt_max = geom_err_8pt;
  geom_err_8pt_avg = mean(vv_8pt)

  clear polys0 polys1 fm;
  [polys0,polys1] = episearch(pts0,pts1,box);

  epi_polycost_f;
  compare_errors;

  geom_err_newalg_avg

  fname = [exp_data_dir 'error_' data_func_names{idata} '_ctroid_8pt_MIN.mat'];
  disp (['Writing ' fname]);
  save(fname,'geom_err_newalg_max','geom_err_newalg_avg','geom_err_8pt_max','geom_err_8pt_avg','vmeancost_8pt','p_id8');
end
