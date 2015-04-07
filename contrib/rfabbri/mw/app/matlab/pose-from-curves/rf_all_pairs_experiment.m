clear all;
% Generate, say, 500 random samples of (id1, id2) pairs
% Run for these.

maxcount = 500;
%maxcount = 3;

[gama_all_img, gama_b_all_img, fmatrix, ...
proj1, proj2,...
tgt_all_img, tgt_b_all_img, ...
Gama_all, Tgt_all] = synthetic_data('36,77');

nsamples_pool = max(size(gama_all_img));

if (maxcount > nsamples_pool)
  error('maxcount too high');
end

clear gama_all_img gama_b_all_img fmatrix  proj1 proj2 tgt_all_img tgt_b_all_img Gama_all Tgt_all;

ids1 = zeros(maxcount,1);
ids2 = zeros(maxcount,1);

scount=0;
while scount < maxcount
  id1 = randint(1,1,[1 nsamples_pool]);
  while (~isempty(find(ids1 == id1)))
    id1 = randint(1,1,[1 nsamples_pool]);
  end
  scount = scount + 1;
  ids1(scount) = id1;

  id2 = randint(1,1,[1 nsamples_pool]);
  while (id2 ~= id1 & ~isempty(find(ids2 == id2)))
    id2 = randint(1,1,[1 nsamples_pool]);
  end
  ids2(scount) = id2;
end


allerrs = zeros(maxcount,nsamples_pool);
for i=1:maxcount
  id1 = ids1(i);
  id2 = ids2(i);
  [errors,errors_gt] = rf_pose_from_point_tangents_root_find_function(id1,id2);
  if isempty(errors)
    allerrs(i,:) = -1
  else
    % Accumulate the errors
    %   - Get best vector
    minsum = sum(errors{1});
    k_min = 1;
    for k=2:max(size(errors))
      minsum_tmp = sum(errors{k});
      if (minsum_tmp < minsum)
        minsum = minsum_tmp;
        k_min = k;
      end
    end
    allerrs(i,:) = errors{k_min};
  end
end


max_ae = max(allerrs');
max_ae_ok = max_ae;
max_ae_ok(max_ae==-1)=0;
clf;
plot(max_ae_ok);
title('Max reprojection errors for random samples of synthetic data')
xlabel('number of samples');
ylabel('max error in pixels');

x_fail = find(max_ae == -1);
y_fail = zeros(size(x_fail));
hold on;
h = plot (x_fail,y_fail,'ro');
%set(h,'markersize',8);

bad_ratio = max(size(find(abs(max_ae) > 0.01)))/max(size(max_ae));

x_bad_error = find(max_ae > 0.01);


