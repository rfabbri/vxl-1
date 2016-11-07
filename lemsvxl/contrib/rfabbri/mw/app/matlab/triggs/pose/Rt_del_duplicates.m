% Rts = Rt_del_duplicates(Rts,min_dr,min_dt)
% delete duplicate entries from list of transformations [R,t]

function Rts = Rt_del_duplicates(Rt1,min_dr,min_dt)
   Rts = [];
   for i = 1:4:size(Rt1,2)
      duplicate = 0;
      R = Rt1(:,i:i+2); t = Rt1(:,i+3);
      for j = 1:4:size(Rts,2)
	 if (norm(t-Rts(:,j+3),Inf)<min_dt)
	    if (norm(R'*Rts(:,j:j+2)-eye(3),Inf)<min_dr)
	       duplicate = 1;
	       break;
	    end;
	 end;
      end;
      if (~duplicate) Rts = [Rts,R,t]; end;
   end;
%end;
