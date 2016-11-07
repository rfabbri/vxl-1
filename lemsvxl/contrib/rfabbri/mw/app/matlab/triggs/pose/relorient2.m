% [Rts,cond] = relorient2(x1,x2)
% Driver routine for relative orientation of 2 calibrated cameras from
% points. This is just a set of fairly arbitrary heuristic thresholds
% and solution pruning hacks. There's not really any science here, so
% feel free to play with thresholds, etc. It returns a list of possible
% solutions [R,t] and their residual epipolar errors.
%
% Method: If we have enough points we try the essential matrix method
% first and monitor its condition numbers. This gives a twisted pair of
% possible solutions. If it looks like the points might not be too far
% from coplanar, we also try the planar homography method. This usually
% gives 2 feasible solutions (plus their twisted pairs). If the points
% really are coplanar, there is *no* way to choose which of these is
% correct: they are both self-consistent.  Once we have a list of
% solutions, we fix up the signs, delete twisted pairs and duplicates,
% run a nonlinear fit to refine each remaining solution, delete any
% resulting duplicates, and also any solutions that give large residual
% epipolar errors. This usually leaves only one solution if the points
% are far from coplanar, but 2 if they are within a few percent of
% coplanarity.

function [Rts,cond] = relorient2(x1,x2)
   consistent_6pts_thresh = 3e-2;	% condition thresholds for
%   coplanar_6pts_thresh = 5e-3;		%  maybe-planar decisions
   illcond_6pts_thresh = 1e-2;		%
   min_good_Rsign = 0.8;		% min fractions of correct signs
   min_good_tsign = 0.8;		%  in R-t sign tests
   max_dup_dR = 1e-4;			% max deviations for duplicate
   max_dup_dt = 1e-4;			%  solution detection
   max_rel_fit_err = 10;		% max fractional error for pruning
					%  bad fits (rel. to best fit found)
   Rts = [];
   n = size(x1,2);
   if (n<4) error('at least 4 planar, 6 nonplanar points needed'); end;
   if (n<6) try_e = 0; try_p = 1; 
   else     try_e = 1; try_p = 0;
   end;

   % essential matrix method
   if (try_e)
      [E21,cond] = Emat_from_pts_lin(x2,x1);
      [E21,Rts,cond1] = Emat_to_Rt(E21);

      if (size(cond,1)==2) 
	 fprintf(1,'E matrix 8pt method: cond=%g, %d solutions\n',...
	        cond(1)/max(cond(2),1e-16),size(Rts,2)/4);
      else      
	 fprintf(1,'E matrix 6pt method: cond=%g, coplanarity=%g, %d solutions\n',...
		 cond(1)/max(cond(2),1e-16),cond(3+1)/cond(3+9),size(Rts,2)/4);
      end;      
%cond'
      % When to try the planar method: if Emat_from_pts_lin() chose the
      % 8pt method, or the rank of the 6pt `N' matrix was greater than 4
      % and it was fairly well conditioned, the points should be far
      % from coplanar, otherwise run the planar method and see.

      if (size(cond,1)==2)  		% 8 point method
	 try_p = 0;
      elseif (cond(2)<consistent_6pts_thresh ...		%  consistent
              & cond(3+9)>illcond_6pts_thresh*cond(3+1))	% ~coplanar
	     % & cond(3+9)>coplanar_6pts_thresh*cond(3+4)	% ~ill-conditioned
         try_p = 0;
      else
	 try_p = 1;
      end;
   end;
%try_p=1;
   % plane homography method
   if (try_p)
      [H21,cond2] = proj_from_pts_lin(x2,x1);
      M = homog_to_Rt(H21);
      Rts = [Rts,M(1:3,:)];
      fprintf(1,'Plane homography method: cond=%g, %d solutions\n',...
	     1/cond2(2),size(M,2)/4);
   end;

%   [Rts,cond] = Rt_del_poor_fits(Rts,x1,x2,1e2); return;

   Rts = Rt_fix_signs(Rts,x1,x2,min_good_Rsign,min_good_tsign);
   Rts = Rt_del_duplicates(Rts,max_dup_dR,max_dup_dt);
%   Rts = Rt_del_poor_fits(Rts,x1,x2,1e2);
   fprintf(1,' ...corrected signs, %d solutions left\n',size(Rts,2)/4);

   if (n>=6)
      Rt1 = [];
      for i = 1:size(Rts,2)/4
	 % f0 = flops;
	 [Rt,cond2] = Rt_from_pts_nl(Rts(:,4*i-3:4*i),x1,x2);	 
	 % flops-f0
	 Rt1 = [Rt1,Rt];

% 	 RR = Rts(:,4*i-3:4*i-1)'
% 	 tt = Rts(:,4*i)
% 	 [Rt,cond2] = Rt_from_pts_nl([RR,tt],x1,x2)
% 	 Rt1 = [Rt1,Rt];
      end;
      Rts = Rt_del_duplicates(Rt1,max_dup_dR,max_dup_dt);
   end;
   [Rts,cond] = Rt_del_poor_fits(Rts,x1,x2,max_rel_fit_err);
   fprintf(1,' ...minimized residuals, %d solutions left\n',size(Rts,2)/4);
%end;
