function I2 = awarp(I,H,meth,sz)
% awarp apply a projective warping (homography) H to an image I
% size can be 'same' or 'valid'
% see interp2 for the method option 

% needs p2t, vec, ivec; 


if nargin == 2
    sz='same';
    meth='linear';
elseif nargin ==3
    sz='same';
end

if strcmp(sz,'same')
    %[y,x]=ndgrid(1:size(I,1),1:size(I,2));
    [y,x] = ndgrid(1:2000,1:2000);
elseif strcmp(sz,'valid')
    error('not implemented yet!');
end

[xx,yy] = p2t(inv(H),vec(x),vec(y));
xi=ivec(xx,size(x,1));
yi=ivec(yy,size(y,1));
I2=interp2(double(I),xi,yi,meth);

