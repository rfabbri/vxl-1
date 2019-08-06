
% Compute boundingbox of 2D images

%for i=1:nviews
%  fname2d = dir(mydir + '*pts-2D*txt');
%  curves2d(:,:,i) = load(mydir + fname2d(i).name);
%end
disp 'data bounds'
allvals=squeeze(curves2d(:,1,:));
min(allvals(:))
max(allvals(:))

allvals=squeeze(curves2d(:,2,:));
min(allvals(:))
max(allvals(:))
