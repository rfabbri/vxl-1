function [tin,in] = GetH3DInlierFromLinear(H,m1,m2,th)
%
% Returns inlier from every 3D homographie in H(1:4,1:4,:)
%
nn = size(m1,2);
e=zeros(nn,1);
for ns = 1:size(H,3)
	
	
	Hm2 = H(:,:,ns)*[m1;ones(1,size(m1,2))];
	Hm2 = Hm2(1:3,:)./[ Hm2(4,:);Hm2(4,:);Hm2(4,:)];
	d = sum((m2-Hm2).^2)';
	e=max(e,d);
end
   
in = e < th*th;
tin= sum(in);
