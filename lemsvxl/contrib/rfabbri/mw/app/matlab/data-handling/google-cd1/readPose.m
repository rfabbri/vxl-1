function T = readPose(fname,n);
% fname - pose.txt file
% n     - number of poses to read. If empty reads the whole file
% UTM coordinates North ~ Y, East ~ X and yaw is around Z

if nargin == 1
  n = 1e+10;
end;

fp = fopen(fname);
k = 1;
if fp == -1
   error('file not found');
else
   line = fgetl(fp); % first line is the legend
   while k < n+1
      line = fgetl(fp); % file name
      if ~ischar(line)
         return;
      end;
      vec = sscanf(line,'%f');
      % long, lat
      [UTME, UTMN] = lltoutm(vec(2),vec(3));
      trans = [UTME, UTMN, vec(4)]';
      R = q2Rot(vec(5:end));
      T(:,:,k) = [R trans; 0 0 0 1];
      k = k+1;
   end;
   fclose(fp);
end;
