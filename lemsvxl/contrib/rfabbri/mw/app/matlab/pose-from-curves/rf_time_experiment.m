reps = 10;
t=zeros(1,reps);
for i=1:reps
  demo
  t(i) = solve_time
end

save('time-random-rf_time_experiment-p2pt','t'):

% boxplot
