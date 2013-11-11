% This script visualizes a list of xfrags from a file
% (c) Nhon Trinh
% Date: Oct 10, 2008

close all;
clear all;

%% input
xfrag_folder = 'D:\vision\projects\symseg\xshock\ETHZ-shapes\training\giraffes-eid_8-nrm';
xfrag_filename = 'detect-giraffes-eid_8.using.train3.model19+giraffes_african.xfrag';
bhog_filename = 'detect-giraffes-eid_8.using.train3.model19+giraffes_african.bhog';
predict_filename = 'detect-giraffes-eid_8.using.train3.model19+giraffes_african.predict';



image_folder = 'D:\vision\projects\symseg\xshock\ETHZ-shapes\training\all_originals';
image_filename = 'giraffes_african.jpg';

%% process

% load file
xfrag_data = dlmread(fullfile(xfrag_folder, xfrag_filename), ' ', 0, 0);
%bhog_data = dlmread(fullfile(xfrag_folder, bhog_filename), ' ', 0, 0);
predict_data = dlmread(fullfile(xfrag_folder, predict_filename), ' ', 0, 0);

predict_prob = predict_data;
pos_idx = find(predict_prob > 0.999);
data = xfrag_data(pos_idx, :);


% parse data
start_x = data(:, 1);
start_y = data(:, 2);
start_psi = data(:, 3);
start_phi = data(:, 4);
start_radius = data(:, 5);

end_x = data(:, 6);
end_y = data(:, 7);
end_psi = data(:, 8);
end_phi = data(:, 9);
end_radius = data(:, 10);

graph_area = data(:, 11);
num_xfrags = size(data, 1);

% Computer other fragment info

nsl = [cos(start_psi+start_phi), sin(start_psi+start_phi)]; % normal vector, start, left
nsr = [cos(start_psi-start_phi), sin(start_psi-start_phi)]; % normal vector, start, right
nel = [cos(end_psi+end_phi), sin(end_psi+end_phi)]; % normal vector, end, left
ner = [cos(end_psi-end_phi), sin(end_psi-end_phi)]; % normal vector, end, right

ss = [start_x, start_y]; % shock point, start
se = [end_x, end_y]; % shock point, end

bsl = ss + nsl .* repmat(start_radius, 1, 2); % boundary, start, left
bsr = ss + nsr .* repmat(start_radius, 1, 2); % boundary, start, right
bel = se + nel .* repmat(end_radius, 1, 2); % boundary, end, left
ber = se + ner .* repmat(end_radius, 1, 2); % boundary, end, right

% plot image and the xfrags
im = imread(fullfile(image_folder, image_filename));
figure(1);
imshow(im);

%hold on;
for k = 1 : num_xfrags %10: 10 % 
%  line([start_x(k), end_x(k)], [start_y(k), end_y(k)]);
  x = [ss(k, 1), bsl(k, 1), bel(k, 1), se(k, 1), ber(k, 1), bsr(k, 1), ss(k, 1)];
  y = [ss(k, 2), bsl(k, 2), bel(k, 2), se(k, 2), ber(k, 2), bsr(k, 2), ss(k, 2)];
  line(x, y);
end;
%hold off;

