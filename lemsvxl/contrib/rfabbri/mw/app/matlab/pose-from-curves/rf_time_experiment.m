%cd /gpfs_home/rfabbri/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-synth/basic_random
cd /users/rfabbri/src/diffgeom2pose/work

reps_warmup = 50;
for i=1:reps_warmup
  rf_demo;
end
reps = 100000;
vsolve_time=zeros(1,reps);
vfail=zeros(1,reps);
vdBest=zeros(1,reps);
vdTbest=zeros(1,reps);
vnumber_of_solutions=zeros(1,reps);
for i=1:reps
  rf_demo;
  vsolve_time(i) = solve_time;
  vfail(i) = fail;
  vdRbest(i) = dRbest;
  vdTbest(i) = dTbest;
  vnumber_of_solutions(i) = number_of_solutions;
end

[stat,cpuinfo]=unix('cat /proc/cpuinfo');
script_path = mfilename('fullpath');
timestamp = datetime('now');
save(['time-random-rf_time_experiment-p2pt-nreps-' num2str(reps)],'vsolve_time', 'cpuinfo','script_path','timestamp', 'vfail', 'vdRbest', 'dTbest', 'vnumber_of_solutions'); 
% boxplot
