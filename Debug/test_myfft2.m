clear all;
close all;


filter = ones(8)/16;
im1 = ones(64);
im2 = imfilter(im1,filter);


%{
FILTER = fft2(f, 512, 512);
figure(1);
imshow(fftshift(log(abs(FILTER))), []);
figure(2);
surf(fftshift((abs(FILTER))), 'EdgeColor', 'none');
return;
%}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% FUNDAMETAL
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

%
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
%

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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% MOU LAB
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
row=180; col=80;
%row=376;  col=240;

%pgm.raw
%file_raw = 'pgm.raw';
fin=fopen(file_raw,'r');
IPGM=fread(fin,row*col,'uint8=>uint8');
ZPGM=uint8(IPGM);
ZPGM=reshape(ZPGM,row,col);
ZPGM=ZPGM';
fclose(fin);
figure('Name',file_raw,'NumberTitle','off');
imshow(ZPGM);
%title(file_raw)  


%fft2
FPGM = fft2(ZPGM);
FPGMAbs = abs(FPGM);
figure('Name','fft2 by MatLab spec','NumberTitle','off');
magFPGM = log(abs(fftshift(FPGMAbs))); 
imshow(magFPGM, []);
%surf(10*log(abs(fftshift(FPGMAbs))), 'EdgeColor', 'none');

figure('Name','fft2 by MatLab plot','NumberTitle','off');
%magFPGM_row = magFPGM(121,:);
magFPGM_row = magFPGM(:,189);
plot(magFPGM_row);

%
%lp.raw
file_raw = 'lp.raw';
fin=fopen(file_raw,'r');
R=fread(fin,row*col,'uint8=>uint8'); 
ZRaw=reshape(R,row,col);
ZRaw=ZRaw';
fclose(fin);
figure('Name',file_raw,'NumberTitle','off');
imshow(ZRaw);
title(file_raw)

%LP filtering by matlab
ZLP=imfilter(ZRaw,filter);
figure('Name','LP filtering by matlab','NumberTitle','off');
imshow(ZLP);
title('LP filtering by matlab')


%ift.raw
file_raw = 'ift.raw';
fin=fopen(file_raw,'r');
IFT=fread(fin,row*col,'uint8=>uint8'); 
ZIFT=reshape(IFT,row,col);
ZIFT=ZIFT';
fclose(fin);
figure('Name',file_raw,'NumberTitle','off');
imshow(ZIFT);
%surf(ZIFT);
title(file_raw)

%spec.raw
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
%surf(log(abs(S)));
title(file_raw);

%ift_lp.raw
file_raw = 'iftLP.raw';
fin=fopen(file_raw,'r');
IFTLP=fread(fin,row*col,'uint8=>uint8'); 
ZIFTLP=reshape(IFTLP,row,col);
ZIFTLP=ZIFTLP';
fclose(fin);
figure('Name',file_raw,'NumberTitle','off');
imshow(ZIFTLP);
%surf(ZIFTLP);
title(file_raw) 

%spec_lp.raw
file_raw = 'specLP.raw';
fin=fopen(file_raw,'r');
specLP=fread(fin,row*col,'double'); 
SLP=(specLP);
SLP=reshape(SLP,row,col);
SLP=SLP';
SLP = fftshift(SLP);
fclose(fin);
figure('Name',file_raw,'NumberTitle','off');
imshow(log(abs(SLP)), []);
%surf(log(abs(SLP)));
title(file_raw);

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



























