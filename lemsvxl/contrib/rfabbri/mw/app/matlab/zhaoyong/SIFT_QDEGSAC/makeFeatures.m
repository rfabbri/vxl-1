function features = makeFeatures( corners, img, mws )
% features = makeFeatures( corners, img, mws )
% create features structure for an image
%   corners - feature locations, calculated if = []
%   img     - the image
%   mws     - match window size

if size(corners, 1) == 0
    corners = corner_detector( img );
end
imgg = rgb2gray( img );

features.count = size(corners,1);   % number of features
features.pos = corners;             % feature positions (matrix coords)
features.rmw = {};                  % relative match window
features.pixels = {};               % window pixels
features.npixels = {};              % normalized window pixels
features.matches = {};              % matches (index correlation)
for i = 1:features.count
    c = features.pos(i,:);
	mw = [max(c + 0.5 - mws / 2, [1 1]); min(c + 0.5 + mws / 2, size(imgg))];
	rmw = mw - [c;c];
	pixels = double(imgg(mw(1,1):mw(2,1), mw(1,2):mw(2,2)));
	npixels = pixels - mean( pixels(:) );   % zero mean
    npixels = npixels / norm( npixels(:));  % normalize

    features.rmw{i} = rmw;
    features.pixels{i} = pixels;
    features.npixels{i} = npixels;
end