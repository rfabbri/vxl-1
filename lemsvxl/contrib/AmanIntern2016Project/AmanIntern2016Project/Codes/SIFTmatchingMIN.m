pfx = fullfile(vl_root,'figures','demo') ;
randn('state',0) ;
rand('state',0) ;
figure(1) ; clf ;

% --------------------------------------------------------------------
%                                                    Create image pair
% --------------------------------------------------------------------

Ia = imread(fullfile(vl_root,'data','roofs1.jpg')) ;
Ib = imread(fullfile(vl_root,'data','roofs2.jpg')) ;
Ia=single(rgb2gray(Ia));
Ib=single(rgb2gray(Ib));

maxa=imregionalmin(Ia);
maxb=imregionalmin(Ib);
[a,b]=size(Ia);
l=0;
for i=1:a
    for j=1:b
        if (maxa(i,j)==1)
            l=l+1;
            fa(1,l)=i;
            fa(2,l)=j;
        end
    end
end
fa(3,:)=1;
fa(4,:)=0;

l=0;
for i=1:a
    for j=1:b
        if (maxb(i,j)==1)
            l=l+1;
            fb(1,l)=i;
            fb(2,l)=j;
        end
    end
end
fb(3,:)=1;
fb(4,:)=0;

[fa,da]=vl_sift(Ia,'frames',fa,'orientations');
[fb,db]=vl_sift(Ib,'frames',fb,'orientations');

[matches, scores] = vl_ubcmatch(da,db) ;

[drop, perm] = sort(scores, 'descend') ;
matches = matches(:, perm) ;
scores  = scores(perm) ;

figure(1) ; clf ;
imagesc(cat(2, Ia, Ib)) ;
axis image off ;

figure(2) ; clf ;
imagesc(cat(2, Ia, Ib)) ;

xa = fa(1,matches(1,:)) ;
xb = fb(1,matches(2,:)) + size(Ia,2) ;
ya = fa(2,matches(1,:)) ;
yb = fb(2,matches(2,:)) ;

hold on ;
h = line([xa ; xb], [ya ; yb]) ;
set(h,'linewidth', 1, 'color', 'b') ;

vl_plotframe(fa(:,matches(1,:))) ;
fb(1,:) = fb(1,:) + size(Ia,2) ;
vl_plotframe(fb(:,matches(2,:))) ;
axis image off ;
