n = 4;
flat = 1;
noise = 1;
focal = 1024; % for noise
fnoise = noise/focal;
trials = 500
maple_out = 1;

% methods = strvcat(...
%     'eliminate';...
%     '3 point';...
%     '5x5 eigen.';...
%     '9x9 res.';...
%     '12x12 res.';...
%     '24x24 res.';...
%     'eliminate*';...
%     '3 point*';...
%     '5x5 eigen.*';...
%     '9x9 res.*';...
%     '12x12 res.*';...
%     '24x24 res.*';...
%     '');
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
ilabels = zeros(1,200);
ilabels(mlabels) = [1:size(mlabels,2)];

% test noise, 4 point methods
if 1
   n = 4;
   meths = mlabels;
   % meths = [1,3,5,9,12,24];
   meths = [3,24,25];
   seq = [0,0.1,0.2,0.4,0.7,1,1.5,2,3,4,5];
   % seq = [0,3];
   nmeth = size(meths,2);
   nseq = size(seq,2);
   e_dR = zeros(nseq,nmeth);
   e_dt = zeros(nseq,nmeth);
   e_fail = zeros(nseq,nmeth);
   for i = 1:size(seq,2)
      seq(i)
      e2 = zeros(trials,2*nmeth);
      for t = 1:trials
	 e = tst_cal_pose(n,seq(i)/focal,flat,meths);
	 e2(t,:) = reshape(e',1,2*nmeth);
      end;
      e = reshape(my_median(e2),2,nmeth);
      fails = reshape(max(reshape((e2>=0.5)',2,trials*nmeth)),nmeth,trials)';
      e_dR(i,:) = e(1,:);
      e_dt(i,:) = e(2,:);
      e_fail(i,:) = sum(fails)/trials*100;
   end;
   e_dR,e_dt,e_fail
   plot(seq,min(e_dt,0.5));
   e_methods = methods(ilabels(meths),:);
   save SAVE_noise4;
   % save SAVE_noise4_planar;
   if maple_out
      out='maple_noise4';
      % out='maple_noise4_planar';
      maple_mat(seq,out,'w','noise:='); 
      maple_mat(e_methods,out,'a','methods:='); 
      maple_mat(e_dR,out,'a','e_dR:='); 
      maple_mat(e_dt,out,'a','e_dt:='); 
      maple_mat(e_fail,out,'a','e_fail:='); 
   end;
end;

% test # points, elimination and 3 point only
if 0
   meths = [1,2,3,5,101,102,103,105];
   seq = [3:8,10,12,15,20];
   %seq = [3,4,5];
   nmeth = size(meths,2);
   nseq = size(seq,2);
   e_dR = zeros(nseq,nmeth);
   e_dt = zeros(nseq,nmeth);
   e_fail = zeros(nseq,nmeth);
   for i = 1:size(seq,2)
      seq(i)
      e2 = zeros(trials,2*nmeth);
      for t = 1:trials
	 e = tst_cal_pose(seq(i),fnoise,flat,meths);
	 e2(t,:) = reshape(e',1,2*nmeth);
      end;
      e = reshape(my_median(e2),2,nmeth);
      fails = reshape(max(reshape((e2>=0.5)',2,trials*nmeth)),nmeth,trials)';
      e_dR(i,:) = e(1,:);
      e_dt(i,:) = e(2,:);
      e_fail(i,:) = sum(fails)/trials*100;
   end;
   e_dR,e_dt,e_fail
   plot(seq,min(e_dt,0.5));
   save SAVE_npoints;
   if maple_out
      out='maple_npoints';
      maple_mat(seq,out,'w','npoints:='); 
      maple_mat(e_methods,out,'a','methods:='); 
      maple_mat(e_dR,out,'a','e_dR:='); 
      maple_mat(e_dt,out,'a','e_dt:='); 
      maple_mat(e_fail,out,'a','e_fail:='); 
   end;
end;


% test approach to singularity 
if 0
   n = 4;
   meths = mlabels;
   seq = [0,1e-2,3e-2,0.1*(1:13),1.35,sqrt(2),1.45,1.5,1.6,1.8,2.0,2.3,2.6,3.0];
   nmeth = size(meths,2);
   nseq = size(seq,2);
   e_dR = zeros(nseq,nmeth);
   e_dt = zeros(nseq,nmeth);
   e_fail = zeros(nseq,nmeth);
   for i = 1:size(seq,2)
      seq(i)
      e2 = zeros(trials,2*nmeth);
      for t = 1:trials
	 e = tst_cal_pose(n,fnoise,seq(i),meths);
	 e2(t,:) = reshape(e',1,2*nmeth);
      end;
      e = reshape(my_median(e2),2,nmeth);
      fails = reshape(max(reshape((e2>=0.5)',2,trials*nmeth)),nmeth,trials)';
      e_dR(i,:) = e(1,:);
      e_dt(i,:) = e(2,:);
      e_fail(i,:) = sum(fails)/trials*100;
   end;
   e_dR,e_dt,e_fail
   plot(seq,min(e_dt,0.5));
   save SAVE_singular;
   if maple_out
      out='maple_singular';
      maple_mat(seq,out,'w','singular:='); 
      maple_mat(e_methods,out,'a','methods:='); 
      maple_mat(e_dR,out,'a','e_dR:='); 
      maple_mat(e_dt,out,'a','e_dt:='); 
      maple_mat(e_fail,out,'a','e_fail:='); 
   end;
end;
