% This is plot_numpts_vs_performance_graph.m.

% Created on: Apr 21, 2013
%     Author: firat

clear
close all

rnd_pts_num = [500 1000 2000 3000 4000 5000 7056];
dense_dim = [20 30 50 60 71 84];
rnd_tp = zeros(size(rnd_pts_num));
dense_tp = zeros(size(dense_dim));

for i = 1:length(rnd_pts_num)
    rnd_tp(i) = random_sift_performance(...
    ['/home/firat/Desktop/data/rand' num2str(rnd_pts_num(i)) '_sift'], false);    
end

for i = 1:length(dense_dim)
	A = num2str(dense_dim(i));
    dense_tp(i) = dense_sift_performance(...
    ['/home/firat/Desktop/data/dense' A 'x' A '_sift'], false);    
end

regular_sift_folder = '/home/firat/Desktop/data/regular_sift';
TPsift = regular_sift_performance(regular_sift_folder, false);
files = dir([regular_sift_folder '/*.mat']);
num_images = length(files);
num_sift = 0;
for i = 1:num_images
    X = load([regular_sift_folder '/' files(i).name]);     
    num_sift = num_sift + size(X.reg_f, 2);
end


%%count max, min, saddle
crit_sift_folder = '/home/firat/Desktop/data/critical_sift';
files = dir([crit_sift_folder '/*.mat']);
num_min = 0;
num_max = 0;
num_saddle = 0;
for i = 1:num_images
    X = load([crit_sift_folder '/' files(i).name]); 
    A = X.min_f(1:2, :)';
    uA = unique(A, 'rows');
    num_min = num_min + size(uA, 1);
    
    A = X.max_f(1:2, :)';
    uA = unique(A, 'rows');
    num_max = num_max + size(uA, 1);
    
    A = X.saddle_f(1:2, :)';
    uA = unique(A, 'rows');
    num_saddle = num_saddle + size(uA, 1);
end
num_min / num_images
num_max / num_images
num_saddle / num_images
total_avg_crit = (num_min + num_max + num_saddle) / num_images;
%X = load('/home/firat/Desktop/data/_old_code/combined_tp2.mat');
Y = load('/home/firat/Desktop/data/_old_code/combined_tp3.mat');
[pj, pi, pk] = meshgrid(Y.p, Y.p, Y.p);
P = [pi(:), pj(:), pk(:)];
NumP = P*[num_min;  num_max;  num_saddle]/num_images;
[sorted_NumP, sort_index] = sort(NumP);
Z = Y.com_tp_min(:);
figure; hold on
plot(rnd_pts_num, rnd_tp/num_images, 'g');
plot(dense_dim.^2, dense_tp/num_images, 'b--');
%plot(X.p*total_avg_crit, X.com_tp_min/num_images, 'g');
plot(sorted_NumP, Z(sort_index)/num_images, 'k');
plot(num_sift/num_images, TPsift/num_images, 'r+',  'LineWidth', 2);
hold off
axis([0 Inf 0 1])
xlabel('Average number of points per image');
ylabel('TPR');
legend('Random', 'Dense', 'Critical Points', 'Regular SIFT');
grid on
