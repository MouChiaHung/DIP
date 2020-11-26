
close all;

%{
im1 = bf(1, 0);
im2 = bf(0, 1);
im3 = bf(0, 4);
im4 = bf(16, 16);

figure('Name','myfft2','NumberTitle','off');
subplot(2, 2, 1);
surf(im1);
xlabel('x');
ylabel('y');
subplot(2, 2, 2);
surf(im2);
xlabel('x');
ylabel('y');
subplot(2, 2, 3);
surf(im3);
xlabel('x');
ylabel('y');
subplot(2, 2, 4);
surf(im4);
xlabel('x');
ylabel('y');
%}

%{
figure('Name','im','NumberTitle','off');
im5 = rgb2gray(imread('valley.jpg'));
imshow(im5);
figure('Name','fft2','NumberTitle','off');
IM5 = fft2(im5);
imshow(log(abs(IM5)), []);
figure('Name','fftshift im','NumberTitle','off');
im5shift = fftshift(im5);
imshow(im5shift);
figure('Name','fftshift fft2','NumberTitle','off');
IM5shift = fftshift(IM5);
imshow(log(abs(IM5shift)), []);
%figure('Name','hist fft2','NumberTitle','off');
%histogram(log(abs(IM5(:))));
%}

%{
im6 = bf(1, 1);
IM6 = fft2(im6);
figure('Name','bf','NumberTitle','off');
imshow(im6, []);
figure('Name','bf fft2','NumberTitle','off');
imshow(log(abs(IM6)), []);
figure('Name','hist bf fft2','NumberTitle','off');
histogram(log(abs(IM6(:))));
logIM6 = round(log(real(IM6)),0);
%}

%{
figure('Name','im','NumberTitle','off');
im5 = rgb2gray(imread('rock.jpg'));
imshow(im5);
%figure('Name','fft2','NumberTitle','off');
IM5 = fft2(im5);
%imshow(log(abs(IM5)), []);
figure('Name','fftshift fft2','NumberTitle','off');
IM5shift = fftshift(IM5);
imshow(log(abs(IM5shift)), []);
%}

%{
%go with DIP project
im5 = bf(0, 8);
figure('Name','bf','NumberTitle','off');
imshow(im5);
xlabel('x');
ylabel('y');
imwrite((im5), 'im.tif');
%}

%{
figure('Name','im','NumberTitle','off');
im5 = (imread('finger.tif'));
imshow(im5);
IM5 = fft2(im5);
IM5_real = real(IM5);
figure('Name','fftshift fft2','NumberTitle','off');
IM5shift = fftshift(IM5);
imshow(log(abs(IM5shift)), []);
%}

row=200;  col=200;
file_raw = 'ift.raw';
fin=fopen(file_raw,'r');
I=fread(fin,row*col,'uint8=>uint8'); 
Z=reshape(I,row,col);
Z=Z';
fclose(fin);
figure('Name',file_raw,'NumberTitle','off');
imshow(Z);
title(file_raw) 

file_raw = 'spec.raw';
fin=fopen(file_raw,'r');
spec=fread(fin,row*col,'double'); 
S=(spec);
S=reshape(S,row,col);
S=S';
S = fftshift(S);
fclose(fin);
figure('Name',file_raw,'NumberTitle','off');
imshow(log(abs(S)), []);
title(file_raw);

function im = bf(u, v)
    N = 32;
    [i, j] = meshgrid((0:(N-1)),(0:(N-1)));
    im = 128*real(exp(-1i*2*pi*((u*i/N)+(v*j/N))));
    if (u==0 && v==0)
        im = round(im);
    end
end






























