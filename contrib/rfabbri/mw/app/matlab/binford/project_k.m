function k = project_k(T,t,F,gama,Gama,N,K)
  lambda=Gama'*F;
  g = norm(T - (T'*F)*gama)/lambda;
  g2= g*g;

  n = cross(t,F);


  k = (N - (N'*F)*gama)'*n*K;

  k = k/(lambda*g2);
