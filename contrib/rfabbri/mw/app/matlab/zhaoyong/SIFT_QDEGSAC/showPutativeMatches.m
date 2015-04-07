% draws the matching points and some epipolar lines



x = features1.pos( pmatches(:,1), [2 1] ); x(:,3) = 1;
xp = features2.pos( pmatches(:,2), [2 1] ); xp(:,3) = 1;

iptsetpref( 'ImshowBorder', 'tight' ); %get rid of border
if not(exist('h1')), h1 = figure, end
figure(h1)
clf reset
i1 = imshow( img1, 'truesize' );
set( i1, 'HandleVisibility', 'off' );
hold on
for i=1:size(x,1)
    seg = [x(i,:);xp(i,:)];
    plot( seg(:,1), seg(:,2), 'c-', 'HandleVisibility', 'off' )
end

ix = x(bestInliers,:);
ixp = xp(bestInliers,:);
for i=1:size(ix,1)
    seg = [ix(i,:);ixp(i,:)];
    plot( seg(:,1), seg(:,2), 'y-', 'HandleVisibility', 'off' )
end
plot( ix(:,1), ix(:,2),'y.', 'HandleVisibility', 'off' );


set(h1, 'PaperPosition', [.01 .01 imsize(1)/100 imsize(2)/100] );
if exist('saveimg') & saveimg == 1
    print( h1, '-djpeg90', '-r100', 'pmatches.jpg' );
end    