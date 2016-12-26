%function main_iterative_mean

% This script computes mean of a set of points by iteratively averaging
% pairs of points
% (c) Nhon Trinh
% Date: Feb 23, 2010


close all;

%% Input
dim = 3;
max_iters = 100000;

tol = 1e-5;
gauss_mu = 0;
gauss_sigma = 1;

num_trials_per_setting = 100;

%% Process

% Plot number of iterations against number of points
list_num_pts = [10, 20, 50, 100, 200, 500]; % 1000, 2000, 5000];
list_dim = [1, 2, 5, 10, 20, 50, 100, 200];

list_num_iters = zeros(length(list_dim), length(list_num_pts));


for q = 1 : length(list_dim)
  dim = list_dim(q);
  for kk = 1 : length(list_num_pts)
    num_pts = list_num_pts(kk);
    % compute the number of iterations 100 times
    num_iters = zeros(1, num_trials_per_setting);

    for k = 1 : num_trials_per_setting
      % Generate the points
      X = random('Normal', 0, 1, [dim, num_pts]);

      % Compute mean till close to real mean
      [x, err] = iterative_mean_euclidean(X, tol, max_iters);
      num_iters(k) = length(err);
    end;
    list_num_iters(q, kk) = mean(num_iters);

    fprintf(1, 'num_pts = %d - num_iters = %g\n', num_pts, mean(num_iters));
  end;
end;



%% Plot results

figure;
hold on;
colors = hsv(length(list_dim));
for q = 1 : length(list_dim) 
  plot(log10(list_num_pts), list_num_iters(q, :), '-o', 'linewidth', 2, 'color', colors(q, :), 'Markersize', 3);
  legend_text{q} = sprintf('N=%d', list_dim(q));
end;
hold off;

xlabel('Number of points (log10-scale)');
ylabel('Number of iters');
title(sprintf('Iterative mean | mu=%g | sigma=%g | dim=%d | tol=%g', gauss_mu, gauss_sigma, dim, tol));
legend(legend_text);
grid on;

%% Plot number of iterations vs. dimension of the points


figure;
hold on;
colors = hsv(length(list_num_pts));
for q = 1 : length(list_num_pts) 
  plot(log10(list_dim), list_num_iters(:, q), '-o', 'linewidth', 2, 'color', colors(q, :), 'Markersize', 3);
  legend_text{q} = sprintf('K = %d', list_num_pts(q));
end;
hold off;

xlabel('Dimension (log10-scale)');
ylabel('Number of iters');
title(sprintf('Iterative mean | mu=%g | sigma=%g | dim=%d | tol=%g', gauss_mu, gauss_sigma, dim, tol));
legend(legend_text);
grid on;

