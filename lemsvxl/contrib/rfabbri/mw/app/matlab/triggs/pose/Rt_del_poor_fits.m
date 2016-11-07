% Rts = Rt_del_poor_fits(Rts,x1,x2,max_rel_err)
% Delete entries in a list of transformations [R,t]
% whose essential matrix E = R*mcross(t) gives a large relative residual
% error against x1,x2.

function [Rts,errs] = Rt_del_poor_fits(Rt1,x1,x2,max_rel_err)
   Rts = []; errs = [];
   if (size(Rt1,2)==0) return; end;
   for i = 1:size(Rt1,2)/4
      R = Rt1(:,4*i-3:4*i-1); t = Rt1(:,4*i);
      err(i) = sqrt(epipolar_err_proj(R*mcross(t),x2,x1)/size(x1,2));
   end;
   for i = find(err < max_rel_err*min(err))
      Rts = [Rts,Rt1(:,4*i-3:4*i)];	 
      errs = [errs,err(i)];
   end;
%end;
