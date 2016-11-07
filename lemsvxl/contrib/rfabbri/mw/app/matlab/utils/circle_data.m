% npts == -1: use default delta, which is just enough to guarantee an 8-connected
% quantization
function [fdata,data,theta,k,kdot,t,t_angle,nrm] = circle_data(r,npts,x0,y0,theta0,theta1);

if npts == -1
% default dtheta TODO

  dtheta = 0.4/r; % little less than (sqrt(2) - 1) / r
else
  dtheta = (theta0 - theta1) / (npts-1);
end

if (nargin < 6)
  theta0 = 0;
  theta1 = 2*pi - dtheta;

  if (nargin < 4)
    x0 = 2*r + 5; % TODO just enough for all coords to be positive
    y0 = 2*r + 5;
  end
end


theta=(theta0:dtheta:theta1)';

cos_t = cos(theta);
sin_t = sin(theta);
xx = r*cos_t + x0;
yy = r*sin_t + y0;

data = [xx yy];

k = (1/r)*ones(size(theta));
kdot = zeros(size(theta));
t = [-sin_t, cos_t]; 
t_angle = atan2(t(:,2),t(:,1));

sz = max(size(t_angle,1));
for i=1:sz
  if t_angle(i) < 0
    t_angle(i) = t_angle(i) + 2*pi;
  end
end

nrm = [-t(:,2) t(:,1)];

fdata = myquantize(data);
