% Call this script within directory containing result files
%
%*synthetic_med-err_pos0.01-angles_0,30,60,90,120,240,270,300*


do_all=0

if (~do_all)
  % Actually these are parsed from list of filenames:
  err_pos=0.01
  %exp_type='epi_gain'
  exp_type='band_stat'
  n_points=1500

  % for synthetic
  synthetic='synthetic_med'
  angles='0,30,60,90,120,240,270,300'

  % for real data
  data_name='ge1630'

  pid_stamp='27201'

  fname_max='max-synthetic_med-err_pos0.01-angles_0,30,60,90,120,240,270,300-valid-Nov12_pid27201.txt'
  fname_min='min-synthetic_med-err_pos0.01-angles_0,30,60,90,120,240,270,300-valid-Nov12_pid27201.txt'
  fname_med='med-synthetic_med-err_pos0.01-angles_0,30,60,90,120,240,270,300-valid-Nov12_pid27201.txt'
  fname_avg='avg-synthetic_med-err_pos0.01-angles_0,30,60,90,120,240,270,300-valid-Nov12_pid27201.txt'

  plot_stereo_exp_include
else

  % write shell script to parse this

  plot_stereo_exp_include
end
