figure
clf
hold on
nv=2:10;

% M: approx. number of candidate edgels along a 1-pixel wide epipolar band for
% the capitol building sequence
M = 300; 

% number of buckets in each dimension for a hypothetical voxel bucketing of
% BxBxB buckets

B = 10;

vM = sqrt(M)*ones(size(nv));

plot(nv, vM.^nv, 'b');
plot(nv, (vM/B).^nv, 'r');
xlabel ('#views');
ylabel('max #ntuplets');
title('How much volumetric reasoning tames combinatorics for 10x10x10 buckets');
legend('image-based reasoning', 'volumetric reasoning')
