% This script plots the subsequent positions of 2D points in a
% iterative curve-shortening method to compute mean
% The goal is to illustrate how the algorithm works
% (c) Nhon Trinh
% Date: Mar 17, 2010

close all;
clear all;

%% Input
dim = 2;
num_pts = 500;
max_iters = 30;

tol = 1e-5;
gauss_mu = 0;
gauss_sigma = 1;

plot_frequency = 5;
%% Process


Z = random('uniform', 0, 1, [dim, num_pts]);
colors = hsv(max_iters+1);

xmean = mean(Z, 2);

figure;
hold on;
legend_text = [];
X = Z;
for i = 1 : max_iters
  % padding X to draw a close contour
  Y = [X, X(:, 1)];
  
  % plot every 10 iterations
  if (rem(i, plot_frequency) == 1)
  
    plot(Y(1, :), Y(2, :), 'mo', ...
      'linewidth', 2, ...
      'color', colors(i, :), ...
      'MarkerSize', 3, ...
      'MarkerFaceColor', colors(i, :));

    legend_text{end+1} = sprintf('iter %d', i);
  end;
  
  XX = circshift(X, [0, 1]);
  X = (X+XX) / 2;
end;
hold off;
legend(legend_text);



figure;
hold on;
legend_text = [];
X = Z;
for i = 1 : max_iters
  % padding X to draw a close contour
  Y = [X, X(:, 1)];
  
  % plot every 5 iterations
  if (rem(i, plot_frequency) == 1)
  
    plot(Y(1, :), Y(2, :), 'mo', ...
      'linewidth', 2, ...
      'color', colors(i, :), ...
      'MarkerSize', 3, ...
      'MarkerFaceColor', colors(i, :));

    legend_text{end+1} = sprintf('iter %d', i);
  end;
  
  p = randperm(num_pts);
  
  % randomly permute members of X
  X = X(:, p);
  
  XX = circshift(X, [0, 1]);
  X = (X+XX) / 2;
end;

hold off;
legend(legend_text);
