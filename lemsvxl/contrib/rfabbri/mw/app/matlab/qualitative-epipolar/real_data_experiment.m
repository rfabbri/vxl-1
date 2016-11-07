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

  nruns = 3;

  vmaxcost_ctroid = zeros(nruns,1);
  vmaxcost_8pt = vmaxcost_ctroid;

  p_id8_lst =zeros(nruns,8);

  for r=1:nruns
    p_id8 = randperm(npts);
    p_id8 = p_id8(1:8);
    p_id8_lst(r,:) = p_id8;
    pts0 = ap0(p_id8,:);
    pts1 = ap1(p_id8,:);
    disp(['Run: ' num2str(p_id8)]);


    clear polys0 polys1 fm;
    [polys0,polys1] = episearch(pts0,pts1,box);

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
    epi_polycost_f;

    compare_errors;

    vmaxcost_ctroid(r) = geom_err_newalg;
    vmaxcost_8pt(r) = geom_err_8pt;
  end
  vmaxcost_ctroid,vmaxcost_8pt
  fname = [exp_data_dir 'error_' data_func_names{idata} '_ctroid_8pt.mat'];
  disp (['Writing ' fname]);
  save(fname,'vmaxcost_ctroid','vmaxcost_8pt','p_id8_lst');
end
