% draws the matching points and some epipolar lines



x = features1.pos( :, [2 1] ); x(:,3) = 1;
xp = features2.pos( :, [2 1] ); xp(:,3) = 1;

iptsetpref( 'ImshowBorder', 'tight' ); %get rid of border
if not(exist('h1')), h1 = figure, end
figure(h1)
clf reset
i1 = imshow( img1, 'truesize' );
set( i1, 'HandleVisibility', 'off' );
hold on
plot( x(:,1), x(:,2),'y.', 'HandleVisibility', 'off' );

iptsetpref( 'ImshowBorder', 'tight' ); %get rid of border
if not(exist('h2')), h2 = figure, end
figure(h2)
clf reset
i2 = imshow( img2, 'truesize' );
set( i2, 'HandleVisibility', 'off' );
hold on
plot( xp(:,1), xp(:,2),'y.', 'HandleVisibility', 'off' );

set(h1, 'PaperPosition', [.01 .01 imsize(1)/100 imsize(2)/100] );
set(h2, 'PaperPosition', [.01 .01 imsize(1)/100 imsize(2)/100] );
if exist('saveimg') & saveimg == 1
    print( h1, '-djpeg90', '-r100', 'features1.jpg' );
    print( h2, '-djpeg90', '-r100', 'features2.jpg' );    
end    