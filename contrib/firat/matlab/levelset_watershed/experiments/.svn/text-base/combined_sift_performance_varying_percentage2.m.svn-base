clear;
close all
p = 1:-.1:.1;
com_tp_gauss = zeros(length(p),length(p),length(p));
com_tp_min = zeros(length(p),length(p),length(p));
com_tp_mean = zeros(length(p),length(p),length(p));
for i = 1:length(p)
    for j = 1:length(p)
        for k = 1:length(p)
            com_tp_gauss(i,j,k) = critical_combined_sift_performance('/home/firat/Desktop/data/critical_sift', [p(i) p(j) p(k)], 'gauss');
            com_tp_min(i,j,k) = critical_combined_sift_performance('/home/firat/Desktop/data/critical_sift', [p(i) p(j) p(k)], 'min');
            com_tp_mean(i,j,k) = critical_combined_sift_performance('/home/firat/Desktop/data/critical_sift', [p(i) p(j) p(k)], 'mean');
        end
    end    
end
save /home/firat/Desktop/data/_old_code/combined_tp3.mat

fprintf('gauss\n');
max1 = max(com_tp_gauss(:));
ind = find(com_tp_gauss == max1);
[i,j,k] = ind2sub(size(com_tp_gauss), ind);
[p(i) p(j) p(k)]
max1
com_tp_gauss(i,j,k)

fprintf('min\n');
max1 = max(com_tp_min(:));
ind = find(com_tp_min == max1);
[i,j,k] = ind2sub(size(com_tp_min), ind);
[p(i) p(j) p(k)]
max1
com_tp_min(i,j,k)

fprintf('mean\n');
max1 = max(com_tp_mean(:));
ind = find(com_tp_mean == max1);
[i,j,k] = ind2sub(size(com_tp_mean), ind);
[p(i) p(j) p(k)]
max1
com_tp_mean(i,j,k)
