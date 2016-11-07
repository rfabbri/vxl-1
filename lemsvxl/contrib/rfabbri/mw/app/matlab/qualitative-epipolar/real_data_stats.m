% To be executed after real_data_experiments.m

real_data_name_defs

% 

ndata = max(size(data_func_names));

for idata=1:ndata
  fname = [exp_data_dir 'error_' data_func_names{idata} '_ctroid_8pt.mat'];
  clear vmaxcost_ctroid vmaxcost_8pt p_id8_lst;
  disp(['Reading ' fname ]);

  load(fname);
  disp('New method:');
  disp(show_error(vmaxcost_ctroid));
  disp('8pt:');
  disp(show_error(vmaxcost_8pt));
  disp(' ');
end
