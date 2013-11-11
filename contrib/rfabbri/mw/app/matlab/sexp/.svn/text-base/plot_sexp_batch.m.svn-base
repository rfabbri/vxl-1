% 2 Experiments for now:
% - epi_gain
% - band_stat
%
save_all=1
n_fig=0


[ret,fail_str]=unix('grep FAILED *.txt');
fail_str = strip_trailing_blanks(fail_str);

if length(fail_str) ~= 0
  disp('ERROR: invalid tests in this directory!');
  fail_str
  return
end


[ret,allfiles]=unix('ls *avg*|xargs echo');
all_datasets='ge1630 capitol_shortbaseline capitol_widebaseline capitol_medbaseline capitol_medbaseline2 fragment synthetic_med synthetic_3 synthetic_2'

while length(all_datasets) ~= 0
  [data_set_name,rem]=strtok(all_datasets);
  all_datasets = rem;
  data_set_name = strip_trailing_blanks(data_set_name);


  [ret,files_this_data]=unix(['ls *avg* | grep ' data_set_name]);
  files_this_data

  while length(files_this_data) ~= 0
    [f,rem]=strtok(files_this_data);
    files_this_data = rem;
    if length(f) == 0
      break;
    end
    f = strip_trailing_blanks(f)
    
    [ret,err_pos] = unix(['echo ' f ' | grep -o ''err_pos[^-]*-''|sed ''s/err_pos//g''|tr - \ ]']);
    err_pos = strip_trailing_blanks(err_pos)


    [ret,synthresult] = unix(['echo ' data_set_name ' |grep synth']);
    synthetic=''
    if length(synthresult) ~=0
       [ret,angles] = unix(['echo ' f ' | grep -o ''angles_[^-]*-''|sed ''s/angles_//g''|tr - \ ']);
       angles = strip_trailing_blanks(angles)
       synthetic=data_set_name;
       % determine exp_type
    else
      data_name = data_set_name;
    end

    [ret,pid_stamp] = unix(['echo ' f ' |egrep -o ''pid[0-9]+''']);
    pid_stamp=strip_trailing_blanks(pid_stamp);

    [ret,f_basis] = unix(['echo ' f ' |sed ''s/avg//g''']);
    f_basis=strip_trailing_blanks(f_basis);


    [ret,exptype_str] = unix(['echo ' f ' | grep -o -- ''-gain-''']);
    exptype_str = strip_trailing_blanks(exptype_str);
    if length(exptype_str) ~= 0
      exp_type = 'epi_gain'
    else
      exp_type = 'band_stat'
      [ret,output_file] = unix(['ls sexp-output' f_basis]);
      output_file=strip_trailing_blanks(output_file)
      [ret,all_npoints] = unix(['cat ' output_file '| egrep ''(Number of points|Number of samples)'' sexp-output*.txt|egrep -o ''[0-9]+$''|sort|uniq']);
      all_npoints=strip_trailing_blanks(all_npoints)

      sampling_subset = true; %< xxx fixme: get this info from data.
      if (sampling_subset)
      % for capitol - only 10 random samples, thus 10 n_points.
        n_points = 10;
      else
        n_points = 0;
        k=1;
        while length(all_npoints) ~= 0
          [npt,rem]=strtok(all_npoints);
          all_npoints = rem;
          if length(npt) == 0
            break;
          end
          n_points(k) = eval(npt);
          k = k + 1;
        end
      end
    end


    [ret,fname_max] = unix(['ls max' f_basis]);
    fname_max=strip_trailing_blanks(fname_max)

    [ret,fname_min] = unix(['ls min' f_basis]);
    fname_min=strip_trailing_blanks(fname_min)

    [ret,fname_med] = unix(['ls med' f_basis]);
    fname_med=strip_trailing_blanks(fname_med)

    [ret,fname_avg] = unix(['ls avg' f_basis]);
    fname_avg=strip_trailing_blanks(fname_avg)

    
    plot_stereo_exp_include
    
    if save_all
      set(gcf, 'PaperPositionMode', 'auto')   % Use screen size
      print('-dpng',[num2str(n_fig)]);
    end

    n_fig = n_fig + 1;

    % determine fname_max
    % determine fname_med
    % determine fname_min
    % determine fname_avg

%    end
  end
end
