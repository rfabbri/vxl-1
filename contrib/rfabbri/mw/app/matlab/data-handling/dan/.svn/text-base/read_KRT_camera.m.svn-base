% reads a camera matrix file and decomposes
function [K,R,T] = read_KRT_camera(filename)

fd = fopen(filename,'r');
if (fd < 0)
    disp(['error opening ' filename ' for read.']);
    return
end



Kin = fscanf(fd,'%f',[3 3]);
Rin = fscanf(fd,'%f',[3 3]);
Tin = fscanf(fd,'%f',[1 3]);

K = Kin';
R = Rin';
T = Tin';



fclose(fd);


return


