close all;
clear;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% EDGE DETECTION LAB
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

N = 32;
n = (-(0):1:N-1);
sigma = 1.5;
i = (-(N):1:N-1);

%exp distribution
%{
figure('name','exp distribution');
x = 100*exp((-2)*(n-8).^2);
x_diff = diff(x);
x_diff2 = diff(x_diff);
n_diff = n(1:length(n)-1);
%n_diff = n_diff+1;
n_diff2 = n_diff(1:length(n_diff)-1);
%n_diff2 = n_diff2+1;
subplot(3,2,1);
plot(n,x);
title('x');
subplot(3,2,3);
plot(n_diff,x_diff);
title('x');
subplot(3,2,5);
plot(n_diff2,x_diff2);
title('x"');
%}

%Gaussion distribution 1D
%{
y = 1*(exp((-1)*(i.^2)/(2*sigma^2)));
y_diff = diff(y);
y_diff2 = diff(y_diff);
i_diff = i(1:length(i)-1);
i_diff2 = i_diff(1:length(i_diff)-1);
figure('name','Gaussion distribution 1D');
subplot(3,2,1);
plot(i,y);
title('y');
subplot(3,2,3);
plot(i_diff,y_diff);
title('x'' ');
subplot(3,2,5);
plot(i_diff2,y_diff2);
title('x"');
subplot(3,2,6);
plot(i,y,i_diff,y_diff,i_diff2,y_diff2);
title('x x'' x"');
%}

%Gaussion distribution 2D
for j = 1 : N
   for i = 1 : N
       %y(j,i) = (-1)*(1/sqrt(2*(sigma^2)))*(exp((-1)*((i-1)^2+(j-1)^2)/(2*sigma^2)));
       y(j,i) = 255*(exp((-1)*((i-N/2)^2+(j-N/2)^2)/(2*sigma^2)));
   end
end

figure('name','Gaussion distribution 2D');
y_diff = diff(y);
y_diff2 = diff(y_diff);
subplot(3,2,1);
imshow(y, [0, 255]);
title('y');
subplot(3,2,3);
imshow(y_diff, [-255, 255]);
title('x'' ');
subplot(3,2,5);
y_diff2 = int64(y_diff2);
imshow(y_diff2, [-255, 255]);
title('x"');

subplot(3,2,2);
surf(y);
subplot(3,2,4);
surf(y_diff);
subplot(3,2,6);
surf(y_diff2);
