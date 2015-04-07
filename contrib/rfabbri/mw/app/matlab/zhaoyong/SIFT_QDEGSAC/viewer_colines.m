if exist('h1')
    figure(h1)
else
    h1 = figure;
end    
clf reset
i1 = imshow( img1 );
set( i1, 'HandleVisibility', 'off' );
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

number_of_lines = 20;

imsize = [size(img1,2) size(img1,1)];
figure(h1);
e = null(F);
for i=1:number_of_lines
    x = [imsize(1)/2, imsize(2)*i/number_of_lines, 1];    
    s = clipline( cross(e,x)', imsize);
   	plot(imsize(1)/2, imsize(2)*i/number_of_lines, 'y.' )    
	plot( s(:,1), s(:,2), 'c-' )
end;
NoInitialInliers = size(m1in1, 2);
NoExtraInInliers = size(m1in2, 2);

for i=1:NoInitialInliers
   	plot(m1in1(1,i), m1in1(2,i), 'b.' )        
end;
for i=1:NoExtraInInliers
   	plot(m1in2(1,i), m1in2(2,i), 'r.' )        
end;


figure(h2); cla
for i=1:number_of_lines
    x = [imsize(1)/2, imsize(2)*i/number_of_lines, 1];    
    s = clipline( F * x', imsize );
	plot( s(:,1), s(:,2), 'y-' )
end;
NoInitialInliers = size(m2in1, 2);
NoExtraInInliers = size(m2in2, 2);
for i=1:NoInitialInliers
   	plot(m2in1(1,i), m2in1(2,i), 'b.' )        
end;
for i=1:NoExtraInInliers
   	plot(m2in2(1,i), m2in2(2,i), 'r.' )        
end;
