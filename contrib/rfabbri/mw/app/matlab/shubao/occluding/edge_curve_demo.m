im = imread('./rendering/0001.jpg');
im = rgb2gray(im);
sil = im2bw(im);
[gradmap, ormap] = canny(im, 5);
[gradpeakmap, posmap] = nonmaxsup(gradmap, ormap, 1.5);
edgemap = hysthresh(gradpeakmap, 0, 30);
[edgelist, edgeim] = edgelink(edgemap, 10, posmap);
[rownum, colnum] = size(edgemap);
edgegradmap = edgegrad(edgelist, gradmap);
edgeormap = edgetangent(edgelist, rownum, colnum);
edgeposmap = sparse(posmap.*edgemap);

imagesc(edgegradmap);
colormap gray; axis image;
print('-dpdf', './result/edgegradmap.pdf');
print('-dpng', './result/edgegradmap.png');

imagesc(edgemap);
colormap gray; axis image;
print('-dpdf', './result/edgemap.pdf');
print('-dpng', './result/edgemap.png');

imagesc(edgegradmap);
hold on;
[row,col,v] = find(edgeormap);
quiver(col,row,cos(v),sin(v));
colormap gray; axis image;
hold off;
print('-dpdf','./result/edgeormap.pdf');
print('-dpng','./result/edgeormap.png');