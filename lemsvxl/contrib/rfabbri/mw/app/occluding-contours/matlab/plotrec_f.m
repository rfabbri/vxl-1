% same as plotrec, but does not clear figure nor open new window
% suf: filename suffix
% ext: filename extension
function c3=plotrec_f(suf,ext,mystyle)

if nargin ~= 3
  mystyle='.-'
  if nargin == 0
    suf='-cam01'; %:< suffix
    ext='.dat';
  end
end



nimages = [2];
%ncons = 0:12;
ncons = 1;

for i=nimages
  for j=ncons
    c3 = readrec(i,j,suf,ext);
    c3 = c3(2:size(c3,1),:);
    plot3(c3(:,1),c3(:,2),c3(:,3),mystyle);
  end
end
