%iter_values = [30; 100 ;150; 200; 300; 400];
iter_values = [30; 100 ;150; 200; 300; 400; 1000; 2000; 3000; 4000];

n_values = size(iter_values,1)
dt_energy = zeros(size(iter_values));
dk_enegry = dt_energy;
dt_dk_energy = dt_energy;


for i=1:n_values
  n_it_pos = sprintf('%g',iter_values(i));
  smooth_compute_misc; gplot_perturb;
  dt_energy(i) = sum(dt_posref_arc.*dt_posref_arc)
  dk_energy(i) = sum(dk_posref_arc.*dk_posref_arc);
  dt_dk_energy(i) = dt_energy(i)+dk_energy(i);
end

figure(269)
clf
hold on
plot(iter_values,log(dt_energy))
plot(iter_values,log(dk_energy),'r')
plot(iter_values,dt_dk_energy,'g')
