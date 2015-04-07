function [tin,in] = GetH3DInlierFromLinearAlg(H,m1,m2,th)
%
% Returns inlier from every 3D homographie in H(1:4,1:4,:)
%

eqns=GetLinearEquationsH3D(m1,m2);
NoConstr=size(eqns,1)/size(m1,2);
nn = size(m1,2);
e=zeros(nn,1);
for ns = 1:size(H,3)
	dd=zeros(size(m1,2),1);
	d=abs(eqns*[H(1,:,ns) H(2,:,ns) H(3,:,ns) H(4,:,ns)]'); 
	for c = 0:NoConstr-1
		dd(1:size(m1,2))=dd(1:size(m1,2))+d(c*size(m1,2)+1:(c+1)*size(m1,2));
	end
	
	e=max(e,dd);
end
   
in = e < th;
tin= sum(in);
