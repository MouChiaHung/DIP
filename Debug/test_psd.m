close all;
clear;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% MOU LAB
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

N = 16;
n1 = (0:1:N-1);
n2 = (0:1:N-1);
%n = [n1;n2];
n = (n1);
w = 0.25;
noise = randn(size(n));
x = cos(1*pi*w*n);
x_noised = cos(1*pi*w*n) + noise;

%
figure('name','PSD');
subplot(1,2,1)
pwelch(x);
subplot(1,2,2)
%pwelch(x_noised);
pwelch(x_noised,N/2,0,N/2,100);
%
