function [con]=read_edg_file(file)

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
count=1;
con=zeros(length(lines)-9,2);

for k=10:length(lines)
          point=lines{k};
          index1=max(strfind(point,'['));
          index2=max(strfind(point,']'));
          [x,y]=strread(point(index1+1:index2-1),'%f %f','delimiter',',');
          con(k-9,:)=[x y];
end
