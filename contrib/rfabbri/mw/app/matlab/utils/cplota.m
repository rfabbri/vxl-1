% plot adjacency matrix of a graph in 3 dimensions.
% The graph is represented by list of xyz vertices pts
%
% The input adjacency matrix is W.

pts = X19  % lucia helix for now

% for fun: compute average distances
w_threshold = 0.98
maxalpha = 0.1

figure
clf
h = cplot(pts,'.b')
npts = size(pts,1)
%axis equal

maxw = max(W(:))
for i=2:npts
  for j=1:i
    if W(i,j) > w_threshold
      h = cplot([pts(i,:); pts(j,:)],[1 0 0],(W(i,j)/maxw)*maxalpha);
    end
  end
end

