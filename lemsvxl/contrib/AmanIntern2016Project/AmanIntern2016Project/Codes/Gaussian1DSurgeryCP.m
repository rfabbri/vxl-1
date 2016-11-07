%{
Create Gaussian signal and add padding of 20 points on each side, with each points set to last define point
ie, F(1:20)=F(21);
    F(222:241)=F(221);
Or change according to size of the function but padding is important.
%}
z = -12:0.1:12;
x = -10:.1:10;
mu = 0;
sigma = 3;
gauss = normpdf(x,mu,sigma);
% figure(1);
% plot(x,gauss)

%%Create small Gaussian noise 1
mu = 2;
sigma = 0.5;
gauss1 = normpdf(x,mu,sigma);

%%Create small Gaussian Noise 2
mu = -8;
sigma = 0.8;
gauss2 = normpdf(x,mu,sigma);

%%Create small Gaussian noise 3
mu = -2;
sigma = .9;
gauss3 = normpdf(x,mu,sigma);

%Create small Gaussian noise 3
mu = 5;
sigma = .7;
gauss4 = normpdf(x,mu,sigma);

%%Create small Gaussian noise 3
mu = 8;
sigma = .5;
gauss5 = normpdf(x,mu,sigma);

%%Add noise to signal
gaussfinal(21:221) = gauss +  0.1*gauss1 +  0.1*gauss2 +  0.1*gauss3 +  0.1*gauss4 +  0.1*gauss5;
gaussfinal(1:20)=gaussfinal(21);                        %Set padding values
gaussfinal(222:241)=gaussfinal(221);                    %Set padding values


% figure()
% plot(z,gaussfinal);
% xlim([-12 12]);
% ax=gca;
% ax.XTick = -12:2:12;
% title('Gaussian Function');
[v1,l1]= findpeaks(gaussfinal);
%%To find minima, enter vector as negative in findpeaks function.
[v2(2:5),l2(2:5)]= findpeaks(-gaussfinal);
l2(1)=20;
v2(1)=(-1)*gaussfinal(20);
l2(6)=221;
v2(6)=(-1)*gaussfinal(221);

rec=zeros(1,length(z));

rec(l1)=v1;
%%This is become we have passed vector as a negative and hence we need to convert the values back
%%into positive values
rec(l2)=(-1).*v2;
n=1;
% l1=setdiff(l1,105);
% l2=setdiff(l2,127);
% v1(2)=[];
% v2(3)=[];
fin=zeros(1,length(z));
while n<50000
   for i=3:length(z)-2
       fin(i)=(rec(i-1) + rec(i-2) + rec(i+1) + rec(i+2))/4;
   end 
   fin(1)=fin(2);
   fin(length(z))=fin(length(z)-1);
   %%Fixing the maxima and minima points
   fin(l1)=v1;
   fin(l2)=(-1).*v2;
   fin(l1-1)=gaussfinal(l1-1);
   fin(l1+1)=gaussfinal(l1+1);
   fin(l2-1)=gaussfinal(l2-1);
   fin(l2+1)=gaussfinal(l2+1);
   rec=fin;   
   n=n+1; 
end
figure;
plot(z,fin);
xlim([-10 10]);
% ylim([0 0.2]);
ax=gca;
ax.XTick = -12:2:12;
title('Reconstruction without removing any max,min');
