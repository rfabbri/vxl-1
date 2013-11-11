function match = dpmatch(vec1, vec2)

%% vec = {vec(i)}
%% vec(i).relpos vec(i).relgrad vec{i}.or

num1 = length(vec1);
num2 = length(vec2);
num = num1+num2+2;
A = Inf*ones(num,num);

% 1, 2...1+num1, 2+num1...1+num1+num2, 2+num1+num2
A(1,2:1+num1) = (0:num1-1);
A(2:1+num1,1) = (0:num1-1)';

A(end,1+num1+num2:-1:2+num1) = (0:num2-1);
A(1+num1+num2:-1:2+num1,end) = (0:num2-1)';

for i=1:num1
    for j=1:num2
       c = closeness(vec1(i), vec2(j));
       A(1+i,1+num1+j) = c;
       A(1+num1+j,1+i) = c;
    end
end

[D,P] = dijkstra(A,1,num)
cnt = 1;
for k = 2:2:length(P)-1
    match(cnt,:) = [P(k)-1 P(k+1)-num1-1];
    cnt = cnt+1;
end

function b = closeness(a1,a2)

p = abs(a1.relpos-a2.relpos);
g = abs(a1.relgrad-a2.relgrad);
o = abs(sin(a1.or-a2.or));
b  = (.4*p+.3*g+.3*o);
