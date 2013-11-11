function con=loadcon(filename,opt)
% Usage : loadcon(filename)
% Reads the .CON format. Returns a vector of points
% opt: if equal to 'image', use x-y image coordinates (as in the con file itself)
% else, invert the y coordinate.

fp = fopen(filename);
if (~fp)
  fprintf(1,'Could not open file %s\n',filename);
end

tag = fscanf(fp,'%s',1);
if tag ~= 'CONTOUR'
  fprintf(1,'Invalid Contour File\n');
  return;
end


opcls = fscanf(fp,'%s',1); %OPEN OR CLOSE
N = fscanf(fp,'%d',1); % number of points in the contour
con = fscanf(fp,'%f', [2,N] );

fclose(fp);

con = con';
if nargin == 2
  if ~strcmp(opt,'image')
    con(:,2) = max(con(:,2)) - con(:,2); 
  end
end
