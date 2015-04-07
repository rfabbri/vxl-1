function [tin,in] = GetFInlierFromLinearAlg(F,m1,m2,th)
%
% Returns inlier from every fundamental matrix in F(1:3,1:3,:)
%

eqns = GetLinearEquationsF(m1, m2);

nn = size(m1,2);
e=zeros(nn,1);
for ns = 1:size(F,3)
	if (norm(F(:,:,ns)) > 0)
	
		d1=abs(eqns*[F(1,:,ns) F(2,:,ns) F(3,:,ns)]');
		
		
		%l2=F(:,:,ns)*[m1;ones(1,nn)];
		%l1=F(:,:,ns)'*[m2;ones(1,nn)];
		
		%de=sqrt(l1(1,:)'.^2+l1(2,:)'.^2);
		%d1=abs(diag(l1'*[m1;ones(1,nn)])./de);
		
		%de = sqrt(l2(1,:)'.^2+l2(2,:)'.^2);
		%d2=abs(diag(l2'*[m2;ones(1,nn)])./de);
		
		
		e=max(e,d1);
	end
end
   
in = e < th;
tin= sum(in);
