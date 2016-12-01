function z = mvn(mu, Sigma,n)
// Generates n multivariate Gaussian samples
  R = chol(Sigma);
  z = repmat(mu,n,1) + rand(n,2,'gaussian')*R;
endfunction
