clear;
close all
p = 1:-.1:.1;
saddle_tp_gauss = zeros(size(p));
saddle_tp_min = zeros(size(p));
saddle_tp_mean = zeros(size(p));
for i = 1:length(p)
    saddle_tp_gauss(i) = critical_sift_performance('/home/firat/Desktop/data/critical_sift', 'saddle', p(i), 'gauss', true);
    saddle_tp_min(i) = critical_sift_performance('/home/firat/Desktop/data/critical_sift', 'saddle', p(i), 'min', true);
    saddle_tp_mean(i) = critical_sift_performance('/home/firat/Desktop/data/critical_sift', 'saddle', p(i), 'mean', true);
end
save /home/firat/Desktop/data/_old_code/saddle_tp2.mat
