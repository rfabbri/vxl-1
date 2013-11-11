% Test routine for methods for calibrated camera pose from known 3D
% points. 

function out = tst_cal_pose(n,noise,flat,methods)
   if nargin<1
      % methods with '*' reorder points for stability before running
      methods = strvcat(...
	  'eliminate',...
	  'elim. L-Q',...
	  '3 point',...
	  '5x5 eigen.',...
	  '9x9 res.',...
	  '12x12 res.',...
	  '24x24 res.',...
	  '24x24 res+corr.',...
	  'eliminate*',...
	  'elim. L-Q*',...
	  '3 point*',...
	  '5x5 eigen.*',...
	  '9x9 res.*',...
	  '12x12 res.*',...
	  '24x24 res.*',...
	  '');
      mlabels = [1,2,3,5,9,12,24,25, 101,102,103,105,109,112,124];

      for i = 1:size(methods,1)
	 fprintf(1,'\t%3d => %s\n',mlabels(i),methods(i,:));
      end
      return
   end

   % Generate a random pose [R0,t0]. 
   R0 = quat_to_rot([0.2*randn(3,1);1]);
   t0 = [0;0;5] + 0.3*randn(3,1);
   P0 = [R0,t0];

   % Generate a random scene and its noisy projection. Noise is measured
   % in radians not pixels, e.g noise ~= 1e-3 might be typical.

   TX = [quat_to_rot([0.1*randn(3,1);1]), 0.1*randn(3,1); zeros(1,3),1];
   X = TX * diag([1,1,flat,1]) * [randn(3,n); ones(1,n)];
   % X = diag([1,1,flat,1]) * [eye(3),ones(3,1),randn(3,n-4);
   % ones(1,n)];

   if 0		%  Marc-Andre's test cases (all singular!)
      R0 = eye(3);
      t0 = [0;flat;0];
      P0 = [R0,t0];
      d = 3;
      e = 0;
      X = [[0,0,d; 1,0,d; 0,1,d; 1,1,d+e]'; ones(1,4)];
      X = [[-1,-1,d; 1,-1,d; -1,1,d; 1,1,d+e]'; ones(1,4)];
   end;

   xx = P0*X;
   x = xx ./ (ones(3,1)*xx(3,:)) + noise*[randn(2,n);zeros(1,n)];
   u = sqrt(sum( xx.^2 ));
   % x,X
   
   % Run the method
   out = [];
   for meth = methods
      [Rts,us] = cal_pose4(x,X,meth,u);
      % us
   
      % Post-process the solutions (there is only ever one in this case)
      dxs = [100,100];
      for i = 1:size(us,1)
	 Rt = Rts(3*i-2:3*i,:);
	 R = Rt(:,1:3);
	 t = Rt(:,4);
      
	 % Evaluate the relative ground-truth error of the solution
	 DR = R'*R0 - eye(3);
	 DT = (t-t0)/norm(t0);
	 dx = [norm(DR),norm(DT)];
	 dxs = [dxs; dx];
      end;
      dxs(2:size(dxs,1),:);
      i = find(sum(abs(dxs'))==min(sum(abs(dxs'))));
      out = [out; dxs(i(1),:)];
   end;
%end
