clear;
close all
p = 1:-.1:.1;
min_tp_gauss = zeros(size(p));
min_tp_min = zeros(size(p));
min_tp_mean = zeros(size(p));
for i = 1:length(p)
    min_tp_gauss(i) = critical_sift_performance('/home/firat/Desktop/data/critical_sift', 'min', p(i), 'gauss', true);
    min_tp_min(i) = critical_sift_performance('/home/firat/Desktop/data/critical_sift', 'min', p(i), 'min', true);
    min_tp_mean(i) = critical_sift_performance('/home/firat/Desktop/data/critical_sift', 'min', p(i), 'mean', true);
end
save /home/firat/Desktop/data/_old_code/min_tp2.mat
