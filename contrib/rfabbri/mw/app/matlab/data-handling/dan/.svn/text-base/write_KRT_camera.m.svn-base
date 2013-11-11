% write_KRT_camera(K,R,T,filename)
function write_KRT_camera(K,R,T,filename)

fd = fopen(filename,'w');
if (fd < 0)
    disp(['error opening ' filename ' for write.']);
    return
end

write_matrix(fd,K);
fprintf(fd,'\n');
write_matrix(fd,R);
fprintf(fd,'\n');
write_matrix(fd,T');
fprintf(fd,'\n');

fclose(fd);

return



function write_matrix(fd,M)

[nrows,ncols] = size(M);
for i=1:nrows
    for j=1:ncols
        fprintf(fd,'%f ',M(i,j));
    end
    fprintf(fd,'\n');
end

return


