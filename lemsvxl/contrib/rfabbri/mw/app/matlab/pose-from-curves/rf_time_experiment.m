reps_warmup = 50;
for i=1:reps
  solve_time
end
reps = 10000;
t=zeros(1,reps);
for i=1:reps
  demo
  t(i) = solve_time
end

[stat,cpuinfo]=unix('cat /proc/cpuinfo');
p = mfilename('fullpath')
save('time-random-rf_time_experiment-p2pt','t', 'cpuinfo','p');

% boxplot
