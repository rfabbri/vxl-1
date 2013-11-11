function str = int2strfixed(int, n)

str = [];
m = int;
for i = n:-1:1
    q =floor(m/10^(i-1));
    str = [str int2str(q)];
    m = m-q*10^(i-1); 
end

