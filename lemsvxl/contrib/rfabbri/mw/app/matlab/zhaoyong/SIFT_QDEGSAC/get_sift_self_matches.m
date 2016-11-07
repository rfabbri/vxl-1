function matches = get_sift_self_matches(desriptors, distRatio, absolute_dist_threshold);

% because it is mathing the other features in the same image,
% for each feature, it only compares its 2nd nearest to the 3rd neartest
% feature. because the '1st nearest' feature is its self;

% For each descriptor in the first image, select its match to second image.

matched_pair_id = 1;
for i = 1 : size(desriptors,2)
   dotprods = desriptors(:, i)' * desriptors;        % Computes vector of dot products
   [vals,indx] = sort(acos(dotprods));  % Take inverse cosine and sort results

   % Check if nearest neighbor has angle less than distRatio times 2nd.
   if ((vals(2) < distRatio * vals(3)) || (vals(2) < absolute_dist_threshold))
      matches(matched_pair_id, 1) = i;
      matches(matched_pair_id, 2) = indx(2);
      matched_pair_id = matched_pair_id+1;
   end
end

if matched_pair_id == 1  %if there is no matched points found
    matches = 0;
end;
