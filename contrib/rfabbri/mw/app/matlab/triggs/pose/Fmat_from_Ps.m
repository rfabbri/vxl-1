% F matrix from two projections
function F10 = Fmat_from_Ps(P1,P0)
   F10 = zeros(3,3);
   k=[1,2,3,1,2];
   for i=1:3
      for j=1:3
	 F10(i,j) = det([P1(k(i+1),:); P1(k(i+2),:); P0(k(j+1),:); P0(k(j+2),:)]);
      end;
   end;
   F10 = F10/norm(F10);
%end;
