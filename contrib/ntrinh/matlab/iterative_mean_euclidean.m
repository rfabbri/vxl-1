function [x, err] = iterative_mean_euclidean(X, tol, max_iters)
% This function computes mean of a set of points X (dim x num_pts) by
% iteratively computing mean of pairs of points
% (c) Nhon Trinh
% Feb 24, 2010

num_pts = size(X, 2);
mean_x = mean(X, 2);

% iteratively compute the mean
err = zeros(1, max_iters);
for k = 1: max_iters
  p = randperm(num_pts);
  
  % randomly permute members of X
  X = X(:, p);
  
  XX = circshift(X, [0, 1]);
  X = (X+XX) / 2;

  
  % computer error
  err_vec = X - circshift(X, [0, 1]);
  err_vec = sum(err_vec .* err_vec, 1);
  len = sum (sqrt(err_vec));
  
  %x1 = X(:, 1);
  %err(k) = norm(x1-mean_x);
  err(k) = len;
  
  if (err(k) < tol)
    break;
  end;
end;

x = X(:, 1);
err = err(1:k);

return;

