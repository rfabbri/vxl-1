%this script was modified to record subpixel position matrix from cemv

function [M,N]=read_cem_file(file)

lines={};
fid=fopen([file]);

count=1;
while 1
    tline = fgetl(fid);
    if ~ischar(tline)
        break
    end
    lines{count}=tline;
    count=count+1;
end
fclose(fid);

count=1; % conta 1 curva
contours={};
M=[];
N=[]; %get edges per contour!!
for k=1:length(lines)
   if (strcmp(lines{k},'[BEGIN CONTOUR]'))
       edge_count_line = lines{k+1};
       [token, remain] = strtok(edge_count_line,'=');
       edge_count = str2num(remain(2:end));
       N = [N, edge_count];
       start=k+2;
       stop=k+1+edge_count;
       con=zeros(edge_count,2);
       index=1;
       for s=start:stop
          point=lines{s};
          index1=max(strfind(point,'['));
          index2=max(strfind(point,']'));
          [x,y]=strread(point(index1+1:index2-1),'%f %f','delimiter',',');
          con(index,:)=[x y];
          index=index+1;
          %usei para encontrar junctions
%           if(x == 56.6215 & y == 71.9505)
%             count
%           end
       end
       contours{count}=con;
       M = [M;con]; %created just to capture subpixel position coordinates

       
       count=count+1; % AQUI! Aumenta qtd curvas
   end
end
