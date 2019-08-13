cd('~/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-synth/work/basic-random/p3p')
% always work from this folder
%cd /gpfs_home/rfabbri/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-synth/work/basic-random
%cd /users/rfabbri/src/diffgeom2pose/work
%cd /home/rfabbri/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-synth/work

%stamp='p2pt'
%script='rf_demo'

stamp='p3p'
script='rf_demo_p3p'

reps_warmup = 50;
for i=1:reps_warmup
  eval(script);
end
reps = 100000;
vsolve_time=zeros(1,reps);
vfail=zeros(1,reps);
vdBest=zeros(1,reps);
vdTbest=zeros(1,reps);
vnumber_of_solutions=zeros(1,reps);
for i=1:reps
  eval(script);
  vsolve_time(i) = solve_time;
  vfail(i) = fail;
  vdRbest(i) = dRbest;
  vdTbest(i) = dTbest;
  vnumber_of_solutions(i) = number_of_solutions;
end

[stat,cpuinfo]=unix('cat /proc/cpuinfo');
script_path = mfilename('fullpath');
timestamp = datetime('now');
save(['time-random-rf_time_experiment-' stamp '-nreps-' num2str(reps)],'vsolve_time', 'cpuinfo','script_path','timestamp', 'vfail', 'vdRbest', 'vdTbest', 'vnumber_of_solutions'); 
% boxplot
