function matches = get_sift_matches_with_distRatio_absDistTh(desriptors_1, desriptors_2, distRatio, absolute_dist_threshold);

% For each descriptor in the first image, select its match to second image.

matched_pair_id = 1;
for i = 1 : size(desriptors_1,2)
   dotprods = desriptors_1(:, i)' * desriptors_2;        % Computes vector of dot products
   [vals,indx] = sort(acos(dotprods));  % Take inverse cosine and sort results

   % Check if nearest neighbor has angle less than distRatio times 2nd.
   if vals(1) < absolute_dist_threshold
       if (vals(1) < distRatio * vals(2))
          matches(matched_pair_id, 1) = i;
          matches(matched_pair_id, 2) = indx(1);
          matched_pair_id = matched_pair_id+1;
       end
   end;
end

if matched_pair_id == 1  %if there is no matched points found
    matches = 0;
end;
