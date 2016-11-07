%function main_plot_CT_windows

% This script plot different windows used for CT reconstruction
% (c) Nhon Trinh
% Date: Feb 18, 2009



% Ram-Lak
figure(1);

B = 0.5;
x = [-5:0.001:5];
k = [-5:1:5];
w = 2*pi * [-B : 0.001:B];
%x = 1;

% frequency domain
H_RL = abs(w);
H_RL(1) = 0;
H_RL(end) = 0;

subplot(2, 1, 1); plot(w, H_RL);
grid on;
xlabel('w');
title('Ramlak - frequency domain');


% spatial domain
% note: Matlab's sinc is not the same as mathematical sinc
% Matlab sinc(x) = mathematical sinc(pi*x)
h_RL = 2* (B*B) * sinc(2*B*x) - (B*B) * (sinc(B*x).^2 );
subplot(2, 1, 2); plot(x, h_RL, 'b-');
grid on;
xlabel('x');
title('Ram-Lak - spatial domain');


% discrete form
dh_RL = zeros(size(k));
for i = 1 :length(k)
  if (k(i) == 0)
    dh_RL(i) = B*B;
  elseif (mod(k(i), 2) == 0)
    dh_RL(i) = 0;
  else
    dh_RL(i) = -4*B*B / (pi.^2 * k(i).^2);
  end;
end;
hold on;
plot(k, dh_RL, 'ro');
hold off;


% Shepp-Logan
figure(2);

% frequency domain
w_half = ceil(length(w) / 2);
H_SL = abs(w) .* sin(w / (4*B));
H_SL(1:w_half) = -H_SL(1:w_half);

H_SL(1) = 0;
H_SL(end) = 0;
subplot(2, 1, 1); plot(w, H_SL);
xlabel('w');
title('Shepp-Logan - frequency domain');
grid on;


% spatial window
h_SL = B / (pi^2) * ( (1-cos(2*pi*B*(1/(4*B) + x))) ./ (1/(4*B) + x) + ...
  (1-cos(2*pi*B*(1/(4*B) - x))) ./ (1/(4*B) - x));
subplot(2, 1, 2);
plot(x, h_SL);
xlabel('x');
title('Shepp-Logan - spatial domain');
grid on;

dh_SL = -8*B^2 ./ ( pi^2 * (4*k.^2 - 1));
hold on;
plot(k, dh_SL, 'ro');
hold off;

% Hamming windows
figure(3);


% frequency domain
subplot(2, 1, 1);
H_hamming = abs(w) .* (0.54 - 0.46 * cos(w + pi));
plot(w, H_hamming);
xlabel('w');
title('Hamming');
grid on;

% spatial domain
temp = real(ifft(H_hamming));
h_hamming = temp(1:6);
h_hamming = [h_hamming(end:-1:1), h_hamming(2:end)];

subplot(2, 1, 2);
plot(k, h_hamming);
title('Hamming - spatial domain');
xlabel('x');
grid on;
hold on;
plot(k, h_hamming, 'or');
hold off;







