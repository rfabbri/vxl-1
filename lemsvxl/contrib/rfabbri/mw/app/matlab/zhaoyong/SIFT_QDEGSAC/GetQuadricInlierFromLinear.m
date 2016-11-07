function [tin,in] = GetQuadricInlierFromLinear(Q,m1,m2,th)
%
% Returns inlier from every projection matrix Q(1:3,1:4,:)
%

nn = size(m1,2);
e=zeros(nn,1);
for ns = 1:size(Q,3)
	
        qm1 = Q(:,:,ns)*[m1; ones(1,size(m1,2))];
        for i = 1 : size(qm1,2)
            dsq(i,1) = [m1(:,i)' 1]*qm1(:,i) ;
        end
		e=max(e,abs(dsq));
end
   
in = e < (th);
tin= sum(in);
