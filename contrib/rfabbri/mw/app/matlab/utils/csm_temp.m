n_iter=10;

for i=1:n_iter
  mynrm=n_sm.*[k_sm k_sm];
  mycss=gama1_sm+0.005*mynrm;
  smooth_compute;
end
