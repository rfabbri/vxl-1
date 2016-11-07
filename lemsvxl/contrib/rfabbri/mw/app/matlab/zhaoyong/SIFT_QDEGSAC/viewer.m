%function viewer( F, img1, img2 )
% viewer( F, img1, img2 )
%   displays epipolar lines corresponding to points clicked in 
%   img1. Right-click to end

if exist('h1')
    figure(h1)
else
    h1 = figure;
end    
clf reset
i1 = imshow( img1 );
set( i1, 'HandleVisibility', 'off' );
xlabel('Right click to end');
hold on


if exist('h2')
    figure(h2)
else
    h2 = figure;
end
clf reset
i2 = imshow(img2);
hold on
set( i2, 'HandleVisibility', 'off' );


b = 0;
e = null(F);
imsize = [size(img1,2) size(img1,1)];
while b~=3
    figure(h1);
    [px py b] = ginput(1);
    x = [px py 1];    
    s = clipline( cross(e,x)', imsize);
    cla    
	plot( s(:,1), s(:,2), 'c-' )
   	plot( px, py, 'y.' )
    
    figure(h2), cla    
    s = clipline( F * x', imsize );
	plot( s(:,1), s(:,2), 'y-' )
end
