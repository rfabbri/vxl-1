% plot adjacency matrix of a graph in 3 dimensions.
% The graph is represented by list of xyz vertices pts
%
% The input adjacency matrix is W, points is pts.

%pts = X19  % lucia helix for now

% for fun: compute average distances
w_threshold = 0.50
maxalpha = 0.2

figure
clf
h = cplot2(pts,'.b')
set(h,'markersize',22);
npts = size(pts,1)
%axis equal

maxw = max(W(:))
for i=2:npts
  for j=1:i
    if W(i,j) > w_threshold
      h = cplot([pts(i,:) 0; pts(j,:) 0],[1 0 0],((W(i,j)-w_threshold)/(maxw-w_threshold))*maxalpha);
    end
  end
end

