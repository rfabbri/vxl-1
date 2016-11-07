clear;
close all
p = 1:-.1:.1;
com_tp_gauss = zeros(size(p));
com_tp_min = zeros(size(p));
com_tp_mean = zeros(size(p));
for i = 1:length(p)
    com_tp_gauss(i) = critical_combined_sift_performance('/home/firat/Desktop/data/critical_sift', p(i), 'gauss');
    com_tp_min(i) = critical_combined_sift_performance('/home/firat/Desktop/data/critical_sift', p(i), 'min');
    com_tp_mean(i) = critical_combined_sift_performance('/home/firat/Desktop/data/critical_sift', p(i), 'mean');
end
save /home/firat/Desktop/data/_old_code/combined_tp2.mat
