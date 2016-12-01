function [data_db, mean_db] = rf_pca(data, n_compress)
  mean_db = mean(data,2);
  data = data - mean_db*ones(1,size(data,2))
  [u,s] = svds(data, n_compress)
  data_db = u*s
