clear;
close all;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% DCT LAB
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
x = [10, 7, 12, 15];
%x = [1, 2, 3, 4, -4, -3, -2, -1];
%x = [1, 2, 3, 4, 4, 3, 2, 1];
%x = [13, 22, 6, 99, 6];
%x = [10, 21, 32, 44, 57, 62, 79, 83];
%x = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16];
N = length(x);
ext_x = [x, x(end:-1:1)];

%h = [1, 0, 0.25, 0.25, 0, 0, 0, 1];
h = [1, 0, 0.25, -0.25];
ext_h = [h, h(end:-1:1)];

%{
Xc = dct(x);
ext_Xc = dct(ext_x);

Xs = dst(x);
ext_Xs = dst(ext_x);

X = fft(x);
ext_X = fft(ext_x);
ext_X_real = ceil(real(fft(ext_x)));
ext_X_imag = ceil(imag(fft(ext_x)));

h = [1, 0, 0.25, 0.25, 0, 0, 0, 1];
ext_h = [h, h(end:-1:1)];

Hc = dct(h);
ext_Hc = dct(ext_h);

Hs = dst(h);
ext_Hs = dst(ext_h);

H = fft(h);
ext_H = fft(ext_h);
ext_H_real = ceil(real(fft(ext_h)));
ext_H_imag = ceil(imag(fft(ext_h)));

y = conv(h, x);
y = y(1:1:2*N-1);
Y = fft(y);
Yc = dct(y);

Zc = Hc.*Xc;
zc = (idct(Zc));
ZZ = H.*X;
zz = (ifft(ZZ));

ext_y = conv(ext_h, ext_x);
ext_y = ext_y(1:1:2*N-1);
ext_Y = fft(ext_y);
ext_Yc = dct(ext_y);

ext_Zc = ext_Hc.*ext_Xc;
ext_zc =ceil(idct(ext_Zc));
ext_ZZ = ext_H.*ext_X;
ext_zz =ceil(ifft(ext_ZZ));
%}

%{
%FFT of even extension source

x_dft_ext = fft(x_ext);
x_dft_ext_real = real(fft(x_ext));
x_dft_ext_imag = imag(fft(x_ext));
%}

%{
%N*N DCT by (2N)*(2N) FFT
X_EXT = fft(x_ext);
k = (0:1:(2*N)-1);
W = cos(2*pi*(0.5*k)/(2*N))-i*sin(2*pi*(0.5*k)/(2*N));
factor = 0.5.*W;

X_DCT_2N = real(0.5.*W.*X_EXT);
X_DCT_N_scale = sqrt(2/N).*[sqrt(1/2), ones(1, N-1)];
X_DCT_N = X_DCT_N_scale.*X_DCT_2N(1:N);
X_DCT_N_matlab = dct(x);
%}

%(2N)*(2N) FFT by N*N DCT for X
%
X_DCT_N = dct(x);
k = (0:1:N-1);
W = cos(2*pi*(0.5*k)/(2*N)) + 1i*sin(2*pi*(0.5*k)/(2*N));
W_ = [2, sqrt(2.*(ones(1, N-1)))]*2.*W;
X_EXT_DFT_N = W_.*X_DCT_N;
X_EXT_DFT_2N = [X_EXT_DFT_N, 0, conj(X_EXT_DFT_N(end:-1:2))];
X_EXT_DFT_2N_matlab = fft(ext_x);
x_ext_fft_ifft = real(ifft(X_EXT_DFT_2N));
x_out = (x_ext_fft_ifft(1:1:N)); 
%

%(2N)*(2N) FFT by N*N DCT for H
H_DCT_N = dct(h);
k = (0:1:N-1);
W = cos(2*pi*(0.5*k)/(2*N)) + 1i*sin(2*pi*(0.5*k)/(2*N));
W_ = [2, sqrt(2.*(ones(1, N-1)))]*2.*W;
H_EXT_DFT_N = W_.*H_DCT_N;
H_EXT_DFT_2N = [H_EXT_DFT_N, 0, conj(H_EXT_DFT_N(end:-1:2))];
H_EXT_DFT_2N_matlab = fft(ext_h);
h_ext_fft_ifft = real(ifft(H_EXT_DFT_2N));
h_out = (h_ext_fft_ifft(1:1:N)); 

% Y = I.*H
Y = (H_EXT_DFT_2N) .* X_EXT_DFT_2N;
y = ceil(real(ifft(Y)));

% R = I.*(1/H)
Hinv = conj(H_EXT_DFT_2N)./(abs(H_EXT_DFT_2N)+0.1);
R = Hinv .* X_EXT_DFT_2N;
r = ceil(real((ifft(R))));

y_matlab = conv(ext_h, ext_x, 'same');
y_matlab = ceil(y_matlab);


return;

