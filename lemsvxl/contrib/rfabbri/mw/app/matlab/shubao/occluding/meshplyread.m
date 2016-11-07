function [pts3d tri] = meshplyread(filename)

fid = fopen(filename, 'r+');
numpts = str2num(fgets(fid));
numtri = str2num(fgets(fid));
pts3d = zeros(3,numpts);
for i = 1:numpts
    string = fgets(fid);
    temp = sscanf(string, '%f %f %f');
    pts3d(:,i) = temp;
end

tri = zeros(3,numtri);
for i = 1:numtri
    string = fgets(fid);
    A = sscanf(string, '%d %d %d %d');
    tri(:,i) = A(2:4);
end
