syms fxx fxy fyy;

H = [fxx fxy; fxy fyy];
[V,Q] = eig(H);
t1 = atan((fyy - fxx - sqrt((fxx-fyy)^2 + 4*fxy^2))/2/fxy); 
t2 = atan((fyy - fxx + sqrt((fxx-fyy)^2 + 4*fxy^2))/2/fxy);
L1 = fxx*cos(t1)^2 + 2*fxy*cos(t1)*sin(t1)+fyy*sin(t1)^2; 
L2 = fxx*cos(t2)^2 + 2*fxy*cos(t2)*sin(t2)+fyy*sin(t2)^2; 
simplify(Q(1,1) - L1)
simplify(Q(2,2) - L2)

simplify(H*[cos(t1);sin(t1)] - L1*[cos(t1);sin(t1)])
