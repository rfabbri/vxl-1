% Call this script within directory containing result files
%
%*synthetic_med-err_pos0.01-angles_0,30,60,90,120,240,270,300*


%% Actually these are parsed from list of filenames:
%err_pos=0.01
%%exp_type='epi_gain'
%exp_type='band_stat'
%n_points=1500
%
%% for synthetic
%synthetic='synthetic_med'
%angles='0,30,60,90,120,240,270,300'
%
%% for real data
%data_name='ge1630'
%
%pid_stamp='27201'
%
%fname_max='max-synthetic_med-err_pos0.01-angles_0,30,60,90,120,240,270,300-valid-Nov12_pid27201.txt'
%fname_min='min-synthetic_med-err_pos0.01-angles_0,30,60,90,120,240,270,300-valid-Nov12_pid27201.txt'
%fname_med='med-synthetic_med-err_pos0.01-angles_0,30,60,90,120,240,270,300-valid-Nov12_pid27201.txt'
%fname_avg='avg-synthetic_med-err_pos0.01-angles_0,30,60,90,120,240,270,300-valid-Nov12_pid27201.txt'
%



max_v=read_vector_ascii(fname_max);
med_v=read_vector_ascii(fname_med);
min_v=read_vector_ascii(fname_min);
avg_v=read_vector_ascii(fname_avg);

figure
clf 
%clg
hold on
x = 1:length(max_v);
x = x + 1;
plot(x,max_v,'b.-');
plot(x,min_v,'r.-');
plot(x,med_v,'g.-');
plot(x,avg_v,'k.-');
xlabel('nviews')
if (~strcmp(exp_type,'epi_gain'))
  for k=1:length(n_points)
    plot(x,n_points(k)*ones(size(x)),'--m');
  end
  legend('max','min','med','avg','ntups');
  ylabel('ntuplets')
else
  legend('max','min','med','avg');
  ylabel('gain')
end
if (length(synthetic) ~=0)
  title(['Data: ' strrep(synthetic,'_','\_') ' ErrPos: ' num2str(err_pos) ' Angles: ' angles])
  
else
  title(['Data: ' strrep(data_name,'_','\_') ' ErrPos: ' num2str(err_pos)])
end
