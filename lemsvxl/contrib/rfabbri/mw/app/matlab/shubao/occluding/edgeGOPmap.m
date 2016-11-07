function [edgemap, edgegradmap, edgeormap, edgeposmap] = edgeGOPmap(im, sigma, low, high, minlength)

[gradmap, ormap] = canny(im,sigma); % Canny edge detection, producing gradient map and some rough orientation map. This orientation map cannot be used in reconstruction, because it is not accurate for its locality nature.
[gradpeakmap, posmap] = nonmaxsup(gradmap, ormap, 1.5); % Perform nonmaximum suppression, which finds the maximum and its position with sub-pixel acccuracy.
edgemapraw = hysthresh(gradpeakmap, low, high); % Thresholding.
[edgelist edgeim] = edgelink(edgemapraw, minlength, posmap); % Link the edges, where edgelist holding the list of positions with 
% edgelist{.}: Nx2 matrix;
[rownum, colnum] = size(edgemapraw);
edgemap = list2map(edgelist,rownum,colnum); % edge maps
edgegradmap = edgegrad(edgelist, gradmap);
edgeormap = edgetangent(edgelist, rownum, colnum);
edgeposmap = posmap.*edgemap;
