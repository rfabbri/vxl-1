% draws the matching points and some epipolar lines

x = features1.pos( matches(:,1), [2 1] ); x(:,3) = 1;
xp = features2.pos( matches(:,2), [2 1] ); xp(:,3) = 1;
imsize = [size(img1,2) size(img1,1)];

if exist('h1')
    figure(h1)
else
    h1 = figure
end
iptsetpref( 'ImshowBorder', 'tight' ); %get rid of border
i1 = imshow( img1, 'truesize' );
hold on
set( i1, 'HandleVisibility', 'off' );

plot( x(:,1), x(:,2),'y.', 'HandleVisibility', 'off' );
for i=1:10
    p = imsize * i / 10;
    p(3) = 1;
    s = clipline( F' * p', imsize );
    plot( s(:,1), s(:,2), 'y-');
end
    


if exist('h2')
    figure(h2)
else
    h2 = figure
end
iptsetpref( 'ImshowBorder', 'tight' ); %get rid of border
i2 = imshow(img2, 'truesize');
hold on
set( i2, 'HandleVisibility', 'off' );
plot( xp(:,1), xp(:,2),'y.', 'HandleVisibility', 'off' );

for i=1:10
    p = imsize * i / 10;
    p(3) = 1;
    s = clipline( F * p', imsize );
    plot( s(:,1), s(:,2), 'y-');
end

set(h1, 'PaperPosition', [.01 .01 imsize(1)/100 imsize(2)/100] );
set(h2, 'PaperPosition', [.01 .01 imsize(1)/100 imsize(2)/100] );

if exist('saveimg') & saveimg == 1
    print( h1, '-djpeg90', '-r100', 'matches1.jpg' );
    print( h2, '-djpeg90', '-r100', 'matches2.jpg' );
end    