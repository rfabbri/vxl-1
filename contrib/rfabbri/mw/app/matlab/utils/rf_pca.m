function data_til = rf_pca(data, n_compress)
  % zero-mean
  media = mean(data,2);
  [nr,nc] = size(data);
  data = data - media*ones(1,nc)
  [u,s] = svds(data)
  data_til = u*s

