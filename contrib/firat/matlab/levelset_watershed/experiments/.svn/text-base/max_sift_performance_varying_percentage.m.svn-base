clear;
close all
p = 1:-.1:.1;
max_tp_gauss = zeros(size(p));
max_tp_min = zeros(size(p));
max_tp_mean = zeros(size(p));
for i = 1:length(p)
    max_tp_gauss(i) = critical_sift_performance('/home/firat/Desktop/data/critical_sift', 'max', p(i), 'gauss', true);
    max_tp_min(i) = critical_sift_performance('/home/firat/Desktop/data/critical_sift', 'max', p(i), 'min', true);
    max_tp_mean(i) = critical_sift_performance('/home/firat/Desktop/data/critical_sift', 'max', p(i), 'mean', true);
end
save /home/firat/Desktop/data/_old_code/max_tp2.mat
