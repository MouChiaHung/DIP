clear all;
close all;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% FFT LAB
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
row=80; col=80;

%
%raw data
file_raw = 'finger.raw';
fin=fopen(file_raw,'r');
R=fread(fin,row*col,'uint8=>uint8'); 
ZRaw=reshape(R,row,col);
ZRaw=ZRaw';
fclose(fin);
figure('Name',file_raw,'NumberTitle','off');
subplot(4,2,1);
imshow(ZRaw);
title('Raw data')

%DFT of raw data
file_raw = 'fingerSpec.raw';
fin=fopen(file_raw,'r');
spec=fread(fin,row*col,'double'); 
S=(spec);
S=reshape(S,row,col);
S=S';
S = fftshift(S);
fclose(fin);
subplot(4,2,3);
imshow(log(abs(S)), []);
title('DFT of raw data');

%DFT and IDFT
file_raw = 'fingerIft.raw';
fin=fopen(file_raw,'r');
IFT=fread(fin,row*col,'uint8=>uint8'); 
ZIFT=reshape(IFT,row,col);
ZIFT=ZIFT';
fclose(fin);
subplot(4,2,4);
imshow(ZIFT);
%surf(ZIFT);
title('DFT and IDFT by C++')

%DFT of output
file_raw = 'fingerSpecLP.raw';
fin=fopen(file_raw,'r');
specLP=fread(fin,row*col,'double'); 
SLP=(specLP);
SLP=reshape(SLP,row,col);
SLP=SLP';
SLP = fftshift(SLP);
fclose(fin);
subplot(4,2,5);
imshow(log(abs(SLP)), []);
title('DFT of output filtered by boxing low pass');

%output filtered by low pass
file_raw = 'fingerIftLP.raw';
fin=fopen(file_raw,'r');
IFTLP=fread(fin,row*col,'uint8=>uint8'); 
ZIFTLP=reshape(IFTLP,row,col);
ZIFTLP=ZIFTLP';
fclose(fin);
subplot(4,2,6);
imshow(ZIFTLP);
%surf(ZIFTLP);
title('Output filtered by boxing low pass'); 

%pgm.raw
file_raw = 'fingerFiltered';
fin=fopen(file_raw,'r');
Filtered=fread(fin,180*80,'uint8=>uint8');
Filtered=reshape(Filtered,180,80);
Filtered=Filtered';
Filtered=imcrop(Filtered,[50 1 80 80]);
fclose(fin);
subplot(4,2,8);
imshow(Filtered);
title('Output filtered by Gaussian low pass');

FFiltered = fft2(Filtered);
FFiltered = fftshift(FFiltered);
subplot(4,2,7);
imshow(log(abs(FFiltered))/(80*80), []);
title('DFT of output filtered by Gaussian low pass');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% FUNDAMETAL LAB
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
base = bf(1, 0);
BASE = fft2(base);
BASE = fftshift(BASE);
figure('Name','base image','NumberTitle','off');
imshow(base, []);
figure('Name','base image fft2','NumberTitle','off');
imshow((abs(BASE)), []);
%figure('Name','hist of base image fft2','NumberTitle','off');
%histogram((abs(BASE(:))));

noise = bf_noise(512, 1);
NOISE = fft2(noise);
NOISE = fftshift(NOISE);
figure('Name', 'noise','NumberTitle','off');
imshow(noise, []);
figure('Name','noise fft2','NumberTitle','off');
imshow((abs(NOISE)), []);
%figure('Name','hist of noise fft2','NumberTitle','off');
%histogram((abs(NOISE(:))));

noised = noise + base;
NOISED = fft2(noised);
NOISED = fftshift(NOISED);
figure('Name', 'noised image','NumberTitle','off');
imshow(noised, []);
figure('Name','noised image fft2','NumberTitle','off');
imshow((abs(NOISED)), []);
%figure('Name','hist of noised image fft2','NumberTitle','off');
%histogram((abs(NOISED(:))));

NOISE_diy = NOISED -  BASE;
figure('Name','NOISE_diy','NumberTitle','on');
imshow((abs(NOISE_diy)), []);

noise_diy = ifft2(NOISE_diy);
figure('Name','NOISE_diy','NumberTitle','on');
imshow(noise_diy, []);
return;
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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function im = bf(u, v)
    N = 512;
    [i, j] = meshgrid((0:(N-1)),(0:(N-1)));
    im = 128*real(exp(-1i*2*pi*((u*i/N)+(v*j/N))));
    if (u==0 && v==0)
        im = round(im);
    end
end

function im = bf_noise(n, mode)
    N = n;
    im = zeros(N);
    
    for i = 1 : N
        for j = 1 : N
            if (mode == 1)
                if ((i+(N-16))==j)
                    im(i,j) = 255;
                elseif (((i+(N-17))==j))
                    im(i,j) = 255;
                elseif (((i+(N-18))==j))
                    im(i,j) = 255;
                elseif (((i+(N-19))==j))
                    im(i,j) = 255;                 
                elseif ((i+(N-32))==j)
                    im(i,j) = 255;
                elseif (((i+(N-33))==j))
                    im(i,j) = 255;
                elseif (((i+(N-34))==j))
                    im(i,j) = 255;
                elseif (((i+(N-35))==j))
                    im(i,j) = 255;                 
                end     
            elseif (mode == 2)
                if (i==N/2||i==1+N/2||i==2+N/2||i==3+N/2)
                    im(i,j) = 255;                 
                end     
            elseif (mode == 3)
                if (j==N/2||j==1+N/2||j==2+N/2||j==3+N/2)
                    im(i,j) = 255;                 
                end     
            end
        end
    end    
    
end
