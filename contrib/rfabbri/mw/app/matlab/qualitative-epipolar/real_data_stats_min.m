% To be executed after real_data_experiments_min.m

real_data_name_defs

% 

ndata = max(size(data_func_names));

for idata=1:ndata
  fname = [exp_data_dir 'error_' data_func_names{idata} '_ctroid_8pt_MIN.mat'];
  clear vmaxcost_ctroid vmaxcost_8pt p_id8_lst geom_err_newalg_max geom_err_newalg_avg geom_err_8pt_avg vmeancost_8pt p_id8;
  disp(['Reading ' fname ]);

  load(fname);
  disp(['  New method avg: ' num2str(geom_err_newalg_avg) ' max: ' num2str(geom_err_newalg_max)]);
  disp(['  8pt avg: ' num2str(geom_err_8pt_avg) ' max: ' num2str(geom_err_8pt_max)]);
  disp(['Tuplet: ' num2str(p_id8)]);
  disp(' ');
end
