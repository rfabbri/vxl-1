function tst_hartley(equalf) 
   % global f0,f1,R0,R1,F10,e10,e01;
   fscale = 1;
   f0 = fscale * exp(randn(1));
   if (equalf>0) f1 = f0;
   else f1 = fscale * exp(randn(1));
   end;
   t = [1;0;0];
   pp = [0;0;1];
   K0 = [f0,0,pp(1); 0,f0,pp(2); 0,0,pp(3)];
   K1 = [f1,0,pp(1); 0,f1,pp(2); 0,0,pp(3)];
   [R0,R] = qr(randn(3));
   [R1,R] = qr(randn(3));
   R0 = (R1 = [cos(1),0,-sin(1); 0,1,0; sin(1),0,cos(1)])';
   P0 = K0 * R0 * [eye(3), t];
   P1 = K1 * R1 * [eye(3), -t];
   F10 = Fmat_from_Ps(P1,P0);
   e10 = epipole_from_Ps(P1,P0);
   e01 = epipole_from_Ps(P0,P1);
F10,e10,e01
   if (equalf<0)
      fs = bougnoux_kruppa_f(F10,e10,e01,pp,pp);
      fs, abs(fs-[f0,f1])./[f0,f1]      
   else if (equalf)
      [f,cond] = hartley_equal_f(F10,e10,pp,pp);
      f, abs(f-f0)/f0, cond
   else
      [fs,cond] = hartley_kruppa_f(F10,e10,pp,pp);
%      [fs,cond] = hartley_kruppa_f1(F10,e10,e01,pp,pp);
      fs, abs(fs-[f0,f1])./[f0,f1], cond
   end
end;