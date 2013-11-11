function F = computeFPP(P1,P2)

% Image dimensions and Normalizing Matrix.
% Marc's favourite Normalizing Matrix.
C1 = null(P1);
e21 = P2 * C1;
Fn = crossM(e21) * P2 * pinv(P1);
F = Fn;
