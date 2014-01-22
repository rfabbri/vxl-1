data = [
     0     1
     0     1];
% manifold is line from (0,0) to (1,1)

[data_db, mean_db]= rf_pca(data,1)

x_test = [0; 0];
[x_proj_pca, x_proj_orig] = rf_pca_project(x_test, data_db, mean_db)
disp 'result of projection in original space should be [0,0]'

x_test = [1; 1];
[x_proj_pca, x_proj_orig] = rf_pca_project(x_test, data_db, mean_db)
disp 'result of projection in original space should be [1,1]'
