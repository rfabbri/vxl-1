function [ac, bc] = compact(a,b)

c = sign(a.*b);
ac = nonzeros(a.*c);
bc = nonzeros(b.*c);
