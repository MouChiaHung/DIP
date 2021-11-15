close all;
clear;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% PSF LAB
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
RUN_HIST = 0;
RUN_CIRCLE = 0;
RUN_DFT    = 0;
RUN_IDFT   = 0;
RUN_KERNEL_DIY = 0;
RUN_DCT_IDCT = 0;
RUN_FILTER_DFT = 0;
RUN_FILTER_DCT = 1;
RUN_CIRCLE_DFT = 0;
RUN_CIRCLE_DCT = 0;

RUN_GRAY_SCALE_16BITS = 0;

file_name = 'lena';
row=256; col=256;
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% HIST lab
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
if (RUN_HIST == 1)
file = append(file_name,'_hist.raw');
fin=fopen(file,'r');
S=fread(fin,row*col,'double'); 
S=reshape(S,row,col);
S=S';
fclose(fin);
figure('name','Hist','NumberTitle','off');
imshow(S, []);
title('Histogram equalization');
%
file = append(file_name,'_im_histed.raw');
fin=fopen(file,'r');
if (contains(file_name,'finger')) 
    raw=fread(fin,row*col,'uint16=>uint8'); 
else
    raw=fread(fin,row*col,'uint8=>uint8'); 
end
S_raw_histed=reshape(raw,row,col);
S_raw_histed=S_raw_histed';
fclose(fin);
figure('name','Raw histed','NumberTitle','off');
imshow(S_raw_histed, []);
title('Raw of Histogram equalization');
%
%
file = append(file_name);
fin=fopen(file,'r');
if (contains(file_name,'finger')) 
    raw=fread(fin,row*col,'uint16=>uint8'); 
else
    raw=fread(fin,row*col,'uint8=>uint8'); 
end
S_raw=reshape(raw,row,col);
S_raw=S_raw';
fclose(fin);
figure('name','Raw','NumberTitle','off');
imshow(S_raw, []);
title('Raw');
%
return;
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CIRCLE
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
if (RUN_CIRCLE)
    if (RUN_FILTER_DFT == 1)
        row_circle=179; 
        col_circle=79;
        row=179; 
        col=79;
    elseif (RUN_FILTER_DCT == 1)
        row_circle=180; 
        col_circle=80;
        row=180; 
        col=180;
    else 
        row_circle=0; 
        col_circle=0;
        row=0; 
        col=0;
    end

    file_circle = 'circle_img.raw';
    fin=fopen(file_circle,'r');
    R_circel=fread(fin,row_circle*col_circle,'uint8=>uint8'); 
    ZRaw_circle=reshape(R_circel,row_circle,col_circle);
    ZRaw_circle=ZRaw_circle';
    fclose(fin);

    figure('Name','Cir PSF','NumberTitle','off');
    subplot(2,2,1);
    imshow(ZRaw_circle);
    title('circle')

    file = 'circle_img.raw';
    fin=fopen(file,'r');
    R_h=fread(fin,row*col,'uint8=>uint8'); 
    ZRaw_h=reshape(R_h,row,col);
    ZRaw_h=ZRaw_h';
    fclose(fin);

    subplot(2,2,2);
    imshow(ZRaw_h);
    title('my h')

    if (RUN_CIRCLE_DFT == 1)
        %DFT spectrum
        file_H = 'circle_DFTSpec.raw';
        fin=fopen(file_H,'r');
        spec_circle=fread(fin,row*col,'double'); 
        S_circle_DFTSpec=(spec_circle);
        S_circle_DFTSpec=reshape(S_circle_DFTSpec,row,col);
        S_circle_DFTSpec=S_circle_DFTSpec';
        S_circle_DFTSpec = fftshift(S_circle_DFTSpec);
        fclose(fin);
        subplot(2,2,3);
        imshow(log(abs(S_circle_DFTSpec)), []);
        title('My DFT H Spectrum');

        H_matlab = fft2(ZRaw_h);
        H_matlab = fftshift(H_matlab);
        subplot(2,2,4);
        imshow(log(abs(H_matlab)), []);
        title('Matlab fft2 of my h');
    end

    if (RUN_CIRCLE_DCT == 1)
        %DCT spectrum
        file_H_DCT = 'circle_DCTSpec.raw';
        fin=fopen(file_H_DCT,'r');
        spec_circle_DCT=fread(fin,row*col,'double'); 
        S_circle_DCTSpec=(spec_circle_DCT);
        S_circle_DCTSpec=reshape(S_circle_DCTSpec,row,col);
        %S_circle_DCTSpec=S_circle_DCTSpec';
        fclose(fin);
        subplot(2,2,3);
        imshow((abs(S_circle_DCTSpec)), []);
        title('My DCT H Spectrum');
    end
end
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% DFT
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if (RUN_DFT == 1)
%
row_dft=179; col_dft=79;
row_dft=179+1; col_dft=79+1;

%raw data of filtered
file_raw_dft = append(file_name,'_im_PGMDft.raw');
fin=fopen(file_raw_dft,'r'); 

if (contains(file_raw_dft,'finger')) 
    if (RUN_GRAY_SCALE_16BITS)
        R_im=fread(fin,row_dft*col_dft,'uint16=>uint8'); 
    else
        R_im=fread(fin,row_dft*col_dft,'uint8=>uint8'); 
    end
else
    R_im=fread(fin,row_dft*col_dft,'uint8=>uint8'); 
end

ZRaw_im=reshape(R_im,row_dft,col_dft);
ZRaw_im=ZRaw_im';
fclose(fin);
figure('Name','DFT','NumberTitle','off');
subplot(3,2,1);
imshow(ZRaw_im, []);
title('Raw data of DFT')

%DFT spectrum
file_raw_dft = append(file_name,'DFTSpec.raw');
fin=fopen(file_raw_dft,'r');
spec=fread(fin,row_dft*col_dft,'double'); 
S=(spec);
S=reshape(S,row_dft,col_dft);
S=S';
S = fftshift(S);
fclose(fin);
subplot(3,2,3);
imshow(log(abs(S)), []);
title('DFT Spectrum');

%DFT real
file_raw_dft = append(file_name,'DFTReal.raw');
fin=fopen(file_raw_dft,'r');
spec_re=fread(fin,row_dft*col_dft,'double'); 
S_re=(spec_re);
S_re=reshape(S_re,row_dft,col_dft);
S_re=S_re';
S_re = fftshift(S_re);
fclose(fin);
subplot(3,2,5);
imshow(log(abs(S_re)), []);
title('DFT Real');

%DFT imag
file_raw_dft = append(file_name,'DFTImag.raw');
fin=fopen(file_raw_dft,'r');
spec_imag=fread(fin,row_dft*col_dft,'double'); 
S_imag=(spec_imag);
S_imag=reshape(S_imag,row_dft,col_dft);
S_imag=S_imag';
S_imag = fftshift(S_imag);
fclose(fin);
subplot(3,2,6);
imshow(log(abs(S_imag)), []);
title('DFT Imag');
%

figure('Name','ifft2imag','NumberTitle','off');
s_imag = abs(ifft2(1i.*S_imag)); 
imshow((s_imag), [0,255]);
figure('Name','ifft2real','NumberTitle','off');
s_re = abs(ifft2(S_re)); 
imshow((s_re), [0,255]);
return;
end

%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% IDFT
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
if (RUN_IDFT == 1)
file_raw_idft = append(file_name,'IDFT.raw');
fin=fopen(file_raw_idft,'r');
R=fread(fin,row_dft*col_dft,'uint8=>uint8'); 
ZRaw=reshape(R,row_dft,col_dft);
ZRaw=ZRaw';
fclose(fin);
subplot(4,2,7);
imshow(ZRaw);
title('IDFT')
end
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Filter BY DFT DFT DFT DFT DFT DFT DFT DFT DFT DFT DFT DFT DFT DFT DFT
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if (RUN_FILTER_DFT)
file_name = 'lena';
row=256; col=256;
row=255; col=255;

window = 79;

row_dft=row; col_dft=col;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% filtering by convolution
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%raw img used by convolution
file = append(file_name,'_im_Filter.raw');
fin=fopen(file,'r');
Rim=fread(fin,row*col,'double'); 
fclose(fin);
Zim=reshape(Rim,row,col);
Zim=Zim';
figure('Name', 'im_Filter','NumberTitle','off');
subplot(4,3,1);
imshow(Zim, []);
title('** Raw image to be filtered of filtering by conv **');    

%kernel used by convolution
file = append(file_name,'_kernel_Filter.raw');
fin=fopen(file,'r');
Rim_kernel=fread(fin,window*window,'double'); 
fclose(fin);
Zim_kernel=reshape(Rim_kernel,window,window);
Zim_kernel=Zim_kernel';
subplot(4,3,2);
imshow(Zim_kernel, []);
title('** kernel of filtering by conv **'); 

%img filtered by convolution
file = append(file_name,'Filtered');
fin=fopen(file,'r');
Rfiltered=fread(fin,row*col,'uint8=>uint8'); 
fclose(fin);
Zfiltered=reshape(Rfiltered,row,col);
Zfiltered=Zfiltered';
subplot(4,3,3);
imshow(Zfiltered, []);
title('** Image filtered of filtering by conv **')

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% filtering by DFT
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%copy of kernel
w_copy = 79;
h_copy = 79;
file = 'copy_179x79_to_79x79.raw';
fin=fopen(file,'r');
Rim_copy=fread(fin,w_copy*h_copy,'double'); 
fclose(fin);
Zim_copy=reshape(Rim_copy,w_copy,h_copy);
Zim_copy=Zim_copy';
subplot(4,3,5);
imshow(Zim_copy, []);
title('copy of kernel matrix');  

%copy of h
w_copy = 179;
h_copy = 79;
file = 'copy_179x79_to_179x79.raw';
fin=fopen(file,'r');
Rim_copy_h=fread(fin,w_copy*h_copy,'double'); 
fclose(fin);
Zim_copy_h=reshape(Rim_copy_h,w_copy,h_copy);
Zim_copy_h=255*Zim_copy_h';
subplot(4,3,6);
imshow(Zim_copy_h, []);
title('extension of kernel matrix');  

%kernel
row_h=179; col_h=79;
file = append(file_name,'_filteringByDFT_kernel.raw');
fin=fopen(file,'r');
Rim_filteringByDFT_kernel=fread(fin,row_h*col_h,'double'); 
fclose(fin);
Zim_filteringByDFT_kernel=reshape(Rim_filteringByDFT_kernel,row_h,col_h);
Zim_filteringByDFT_kernel=Zim_filteringByDFT_kernel';
subplot(4,3,7);
imshow(Zim_filteringByDFT_kernel, []);
title('kernel of filtering by DFT'); 

%DFT spectrum of kernel
row_dft=179; col_dft=79;
file = append(file_name,'_filteringByDFT_kernel_DFT.raw');
fin=fopen(file,'r');
spec_filteringyDFT_kernel_DFT=fread(fin,row_dft*col_dft,'double'); 
S_filteringyDFT_kernel_DFT=(spec_filteringyDFT_kernel_DFT);
S_filteringyDFT_kernel_DFT=reshape(S_filteringyDFT_kernel_DFT,row_dft,col_dft);
S_filteringyDFT_kernel_DFT=S_filteringyDFT_kernel_DFT';
S_filteringyDFT_kernel_DFT = fftshift(S_filteringyDFT_kernel_DFT);
fclose(fin);
subplot(4,3,8);
imshow(log(abs(S_filteringyDFT_kernel_DFT)), []);
title('DFT of kernel of filtering by DFT'); 

%image
row_img=179; col_img=79;
file = append(file_name,'_im_PGMDft_Double.raw');
fin=fopen(file,'r');
Rim_filteringByDFT_img=fread(fin,row_img*col_img,'double'); 
fclose(fin);
Zim_filteringByDFT_img=reshape(Rim_filteringByDFT_img,row_img,col_img);
Zim_filteringByDFT_img=Zim_filteringByDFT_img';
subplot(4,3,9);
imshow(Zim_filteringByDFT_img, []);
title('image of filtering by DFT'); 

%DFT spectrum of image
file = append(file_name,'DFTSpec.raw');
fin=fopen(file,'r');
spec_filteringyDFT_img_DFT=fread(fin,row_dft*col_dft,'double'); 
S_filteringyDFT_img_DFT=(spec_filteringyDFT_img_DFT);
S_filteringyDFT_img_DFT=reshape(S_filteringyDFT_img_DFT,row_dft,col_dft);
S_filteringyDFT_img_DFT=S_filteringyDFT_img_DFT';
S_filteringyDFT_img_DFT = fftshift(S_filteringyDFT_img_DFT);
fclose(fin);
subplot(4,3,10);
imshow(log(abs(S_filteringyDFT_img_DFT)), []);
title('DFT of image of filtering by DFT'); 

%product

%filtered
file = append(file_name,'_filteringByDFT_filtered.raw');
fin=fopen(file,'r');
R_filteringByDFT_filtered=fread(fin,row_dft*col_dft,'double'); 
S_filteringByDFT_filtered=(R_filteringByDFT_filtered);
S_filteringByDFT_filtered=reshape(S_filteringByDFT_filtered,row_dft,col_dft);
S_filteringByDFT_filtered=S_filteringByDFT_filtered';
S_filteringByDFT_filtered = fftshift(S_filteringByDFT_filtered);
fclose(fin);
subplot(4,3,12);
imshow(((S_filteringByDFT_filtered)), []);
title('filtered by filtering by DFT'); 

%DFT spectrum of filtered
file = append(file_name,'_filteringByDFT_filtered_DFT.raw');
fin=fopen(file,'r');
spec_filteringyDFT_filtered_DFT=fread(fin,row_dft*col_dft,'double'); 
S_filteringyDFT_filtered_DFT=(spec_filteringyDFT_filtered_DFT);
S_filteringyDFT_filtered_DFT=reshape(S_filteringyDFT_filtered_DFT,row_dft,col_dft);
S_filteringyDFT_filtered_DFT=S_filteringyDFT_filtered_DFT';
S_filteringyDFT_filtered_DFT = fftshift(S_filteringyDFT_filtered_DFT);
fclose(fin);
subplot(4,3,11);
imshow(log(abs(S_filteringyDFT_filtered_DFT)), []);
title('DFT of image of filtering by DFT');
return;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if (RUN_KERNEL_DIY == 1)
r=180-1; c=1;
file = append('table','.raw');
fin=fopen(file,'r');
R=fread(fin,r*c,'double'); 
S=(R);
S=reshape(S,c,r);
fclose(fin);
figure('name',' table','NumberTitle','off');
imshow(S, []);
title('table'); 

r=180-1; c=80-1;
file = append('kernel_diy','.raw');
fin=fopen(file,'r');
R=fread(fin,r*c,'double'); 
S=(R);
S=reshape(S,r,c);
S=S';
fclose(fin);
figure('name',' table','NumberTitle','off');
imshow(S, []);
title('kernel diy');
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
return;
%lab of bin import as kernel

file = append('h_re','.bin');
fin=fopen(file,'r');
R=fread(fin,r*c,'double'); 
S=(R);
S=reshape(S,c,r);
S=S';
fclose(fin);
figure('name',' import','NumberTitle','off');
imshow(S, []);
title('kernel imported'); 

%lab of DFT
I_DFT_matlab = fft2(Zim_filteringByDFT_img);
H_DFT_matlab = fft2(Zim_filteringByDFT_kernel);
Y_DFT_matlab = I_DFT_matlab .* H_DFT_matlab;
y_DFT_matlab = fftshift(ifft2(Y_DFT_matlab));

figure('name',' Y_dft','NumberTitle','off');
imshow(log(abs(Y_DFT_matlab)), []);
title('Y by matlab DFT');

figure('name',' y_dft','NumberTitle','off');
imshow(y_DFT_matlab, []);
title('y by matlab DFT');

%lab of DCT
I_DCT_matlab = dct2(Zim_filteringByDFT_img);
H_DCT_matlab = dct2(Zim_filteringByDFT_kernel);
Y_DCT_matlab = I_DCT_matlab .* H_DCT_matlab;
y_DCT_matlab = fftshift(idct2(Y_DCT_matlab));

figure('name',' Y_dct','NumberTitle','off');
imshow(log(abs(Y_DCT_matlab)), []);
title('Y by matlab dct');

figure('name',' y_dct','NumberTitle','off');
imshow(y_DCT_matlab, []);
title('y by matlab dct');
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Filter BY DCT DCT DCT DCT DCT DCT DCT DCT DCT DCT DCT DCT DCT DCT DCT 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if (RUN_FILTER_DCT)
file_name = 'lena';
row=256; col=256;
row=255; col=255;

row_conv_image =179; col_conv_image =79;
row_conv_kernel=79;  col_conv_kernel=79;

row_dct=row; col_dct=col;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% filtering by convolution
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%raw img used by convolution
file = append(file_name,'_im_Filter.raw');
fin=fopen(file,'r');
Rim=fread(fin,row_conv_image*col_conv_image,'double'); 
fclose(fin);
Zim=reshape(Rim,row_conv_image,col_conv_image);
Zim=Zim';
figure('Name', 'im_Filter','NumberTitle','off');
subplot(4,3,1);
imshow(Zim, []);
title('** Raw image to be filtered of filtering by conv **');    

%kernel used by convolution
file = append(file_name,'_kernel_Filter.raw');
fin=fopen(file,'r');
Rim_kernel=fread(fin,row_conv_kernel*col_conv_kernel,'double'); 
fclose(fin);
Zim_kernel=reshape(Rim_kernel,row_conv_kernel,col_conv_kernel);
Zim_kernel=Zim_kernel';
subplot(4,3,2);
imshow(Zim_kernel, []);
title('** kernel of filtering by conv **'); 

%img filtered by convolution
file = append(file_name,'Filtered');
fin=fopen(file,'r');
Rfiltered=fread(fin,row_conv_image*col_conv_image,'uint8=>uint8'); 
fclose(fin);
Zfiltered=reshape(Rfiltered,row_conv_image,col_conv_image);
Zfiltered=Zfiltered';
subplot(4,3,3);
imshow(Zfiltered, []);
title('** Image filtered of filtering by conv **')

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% filtering by DCT
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
row_h=row;   col_h=col;
row_dct=row; col_dct=col;
row_img=row; col_img=col;

%copy of kernel
w_copy = 79;
h_copy = 79;
file = 'copy_360x160_to_79x79.raw';
fin=fopen(file,'r');
Rim_copy=fread(fin,w_copy*h_copy,'double'); 
fclose(fin);
Zim_copy=reshape(Rim_copy,w_copy,h_copy);
Zim_copy=Zim_copy';
subplot(4,3,5);
imshow(Zim_copy, []);
title('copy of kernel matrix');  

%copy of h
w_copy = 180;
h_copy = 80;
%file = 'copy_180x80_to_180x80.raw';
file = 'cp_180x80_to_360x160.raw';

fin=fopen(file,'r');
Rim_copy_h=fread(fin,w_copy*h_copy,'double'); 
fclose(fin);
Zim_copy_h=reshape(Rim_copy_h,w_copy,h_copy);
Zim_copy_h=255*Zim_copy_h';
subplot(4,3,6);
imshow(Zim_copy_h, []);
title('extension of kernel matrix');  

%kernel
file = append(file_name,'_filteringByDCT_kernel.raw');
fin=fopen(file,'r');
Rim_filteringByDCT_kernel=fread(fin,row_h*col_h,'double'); 
fclose(fin);
Zim_filteringByDCT_kernel=reshape(Rim_filteringByDCT_kernel,row_h,col_h);
Zim_filteringByDCT_kernel=Zim_filteringByDCT_kernel';
subplot(4,3,7);
imshow(Zim_filteringByDCT_kernel, []);
title('kernel of filtering by DCT'); 

%DCT spectrum of kernel
file = append(file_name,'_filteringByDCT_kernel_DCT.raw');
fin=fopen(file,'r');
spec_filteringyDCT_kernel_DCT=fread(fin,row_dct*col_dct,'double'); 
S_filteringyDCT_kernel_DCT=(spec_filteringyDCT_kernel_DCT);
S_filteringyDCT_kernel_DCT=reshape(S_filteringyDCT_kernel_DCT,row_dct,col_dct);
S_filteringyDCT_kernel_DCT=S_filteringyDCT_kernel_DCT';
S_filteringyDCT_kernel_DCT = (S_filteringyDCT_kernel_DCT);
fclose(fin);
subplot(4,3,8);
imshow(log(abs(S_filteringyDCT_kernel_DCT)), []);
title('DCT of kernel of filtering by DCT'); 

%image
file = append(file_name,'_filteringByDCT_image.raw');
fin=fopen(file,'r');
Rim_filteringByDCT_img=fread(fin,row_img*col_img,'double'); 
fclose(fin);
Zim_filteringByDCT_img=reshape(Rim_filteringByDCT_img,row_img,col_img);
Zim_filteringByDCT_img=Zim_filteringByDCT_img';
subplot(4,3,9);
imshow(Zim_filteringByDCT_img, []);
title('image of filtering by DCT'); 

%{
figure();
H = dct2(Zim_filteringByDCT_kernel);
Hinv = 1 ./ (H);
%Hinv = normalize(Hinv, 'range', [-0.5 0]);
Hinv = 0.00001 + Hinv;
Y = dct2(Zim_filteringByDCT_img) .* Hinv;
y = idct2(Y);
imshow(y, []);
title('y'); 
return;
%}

%DCT spectrum of image
file = append(file_name,'_filteringByDCT_image_DCT.raw');
fin=fopen(file,'r');
spec_filteringyDCT_img_DCT=fread(fin,row_dct*col_dct,'double'); 
S_filteringyDCT_img_DCT=(spec_filteringyDCT_img_DCT);
S_filteringyDCT_img_DCT=reshape(S_filteringyDCT_img_DCT,row_dct,col_dct);
S_filteringyDCT_img_DCT=S_filteringyDCT_img_DCT';
S_filteringyDCT_img_DCT = (S_filteringyDCT_img_DCT);
fclose(fin);
subplot(4,3,10);
imshow(log(abs(S_filteringyDCT_img_DCT)), []);
title('DCT of image of filtering by DCT'); 

%product

%filtered
file = append(file_name,'_filteringByDCT_filtered.raw');
fin=fopen(file,'r');
R_filteringByDCT_filtered=fread(fin,row_dct*col_dct,'double'); 
S_filteringByDCT_filtered=(R_filteringByDCT_filtered);
S_filteringByDCT_filtered=reshape(S_filteringByDCT_filtered,row_dct,col_dct);
S_filteringByDCT_filtered=S_filteringByDCT_filtered';
S_filteringByDCT_filtered = (S_filteringByDCT_filtered);
fclose(fin);
subplot(4,3,12);
imshow(((S_filteringByDCT_filtered)), []);
title('filtered by filtering by DCT'); 

%DCT spectrum of image
file = append(file_name,'_filteringByDCT_filtered_DCT.raw');
fin=fopen(file,'r');
spec_filteringyDCT_filtered_DCT=fread(fin,row_dct*col_dct,'double'); 
S_filteringyDCT_filtered_DCT=(spec_filteringyDCT_filtered_DCT);
S_filteringyDCT_filtered_DCT=reshape(S_filteringyDCT_filtered_DCT,row_dct,col_dct);
S_filteringyDCT_filtered_DCT=S_filteringyDCT_filtered_DCT';
S_filteringyDCT_filtered_DCT = (S_filteringyDCT_filtered_DCT);
fclose(fin);
subplot(4,3,11);
imshow(log(abs(S_filteringyDCT_filtered_DCT)), []);
title('DCT of filtered of filtering by DCT'); 

end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% DCT and IDCT
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if (RUN_DCT_IDCT)
%raw data
if true
    file_raw_dct = append(file_name,'_im_PGMDct.raw');
    fin=fopen(file_raw_dct,'r'); 
    if (contains(file_raw_dct,'finger')) 
        R_im=fread(fin,row*col,'uint16=>uint8'); 
    else
        R_im=fread(fin,row*col,'uint8=>uint8'); 
    end
    ZRaw_im_dct=reshape(R_im,row_dct,col_dct);
    ZRaw_im_dct=ZRaw_im_dct';
    fclose(fin);
else
    file_raw_dct = append(file_name,'_im_Filter.raw');
    fin=fopen(file_raw_dct,'r');
    ZRaw_im_dct=fread(fin,row_dct*col_dct,'double'); 
    ZRaw_im_dct=reshape(ZRaw_im_dct,row_dct,col_dct);
    ZRaw_im_dct=ZRaw_im_dct';
    fclose(fin);
end
figure('Name', 'DCT','NumberTitle','off');
subplot(2,2,1);
imshow(ZRaw_im_dct, [0,255]);
title('Raw image');    

%DCT spectrum
%
file_raw_dct = append(file_name,'DCTSpec.raw');
fin=fopen(file_raw_dct,'r');
spec_dct=fread(fin,row_dct*col_dct,'double'); 
S_dct=(spec_dct);
S_dct=reshape(S_dct,row_dct,col_dct);
S_dct=S_dct';
%S_dct = fftshift(S_dct);
fclose(fin);
subplot(2,2,4);
imshow(log(abs(S_dct)), []);
title('DCT Spec');
%

%IDCT
file = append(file_name,'IDCT.raw');
fin=fopen(file,'r');
R_IDCT=fread(fin,row_dct*col_dct,'uint8=>uint8'); 
ZRaw_IDCT=reshape(R_IDCT,row_dct,col_dct);
ZRaw_IDCT=ZRaw_IDCT';
subplot(2,2,3);
imshow(ZRaw_IDCT, [0,255]);
title('IDCT');   

end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Noise estimate, mean and variance 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%{
img = im2double(raw);
img_PSF = imfilter(img, 'PSF', 'circular', 'conv');
figure('Name','PSF','NumberTitle','off');
subplot(2,1,1);
imshow(img, []);
subplot(2,1,2);
imshow(img_PSF, []);
%}

%{
n = (0:1:255);
avg = 100;
sig = 10;
g_tmp = 1/((sqrt(2*pi)*sig))*exp((-1)*(n-avg).^2/(2*(sig^2)));
figure('Name','10','NumberTitle','off');
plot(n, g_tmp);

avg = 100;
sig = 5;
g_tmp = 1/((sqrt(2*pi)*sig))*exp((-1)*(n-avg).^2/(2*(sig^2)));
figure('Name','5','NumberTitle','off');
plot(n, g_tmp);

avg = 100;
sig = 0.5;
g_tmp = 1/((sqrt(2*pi)*sig))*exp((-1)*(n-avg).^2/(2*(sig^2)));
figure('Name','0.5','NumberTitle','off');
plot(n, g_tmp);

avg = 100;
sig = 0.2;
g_tmp = 1/((sqrt(2*pi)*sig))*exp((-1)*(n-avg).^2/(2*(sig^2)));
figure('Name','0.2','NumberTitle','off');
plot(n, g_tmp);
%}

%ret = noise_est(raw);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Adaptive filter powered by mean and the rate of global/local sigma 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%{
adp = wiener2(raw, [3,3], 5);
figure('Name', 'wiener2','NumberTitle','off');
imshow(adp, []);

%comparison
avg = imfilter(raw, ones(3)/(3*3));
figure('Name', 'avg','NumberTitle','off');
imshow(avg, []);
%}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% FUNC
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function block = noise_est(im)
    in = round(ginput(2));
    fprintf("g(1,1):%d\n", in(1,1));
    fprintf("g(1,2):%d\n", in(1,2));
    fprintf("g(2,1):%d\n", in(2,1));
    fprintf("g(2,2):%d\n", in(2,2));
    x = in(1,1):in(2,1);
    y = in(1,2):in(2,2);
    block = im(x, y);
    h = imhist(block);
    n = (0:1:(size(h)-1));
    vec_block = block(:);
    doubleVec_block = double(block(:));
    avg = mean(vec_block);
    sig = std(doubleVec_block);
    
    fprintf("avg:%f\n", avg);
    fprintf("sig:%f\n", sig);
    
    g = 1/((sqrt(2*pi)*sig))*exp((-1)*(n-avg).^2/(2*(sig^2)));
    
    num_pixels = sum(h);
    area = sum(g);
    pmf = g;
    hg = g*(num_pixels/area);
    
    figure('Name','noise_est','NumberTitle','off');
    subplot(3,2,1);
    imshow(im, []);
    
    subplot(3,2,2);
    imshow(block, []);
    
    subplot(3,2,3);
    bar(n, h);
    
    subplot(3,2,4);
    plot(n, g);
    
    subplot(3,2,5);
    plot(n, pmf);

    subplot(3,2,6);
    hold on;
    bar(n, h);
    plot(n, hg);
end

%{
kr = zeros(size(im_cos));
kr_cos = zeros(size(im_cos));
kr_sin = zeros(size(im_cos));

delX = 0;
delY = 0;
delR = 0;
centerX = 1;
centerY = 1;
w = row;
h = col;
signal = 0;
decay = 0;
decay_factor = 2.0
init = -0;
rmax = 16
period = 22
f0 = 2
limitR = sqrt(power(w,2)+power(h,2));

for j=1:col
    for i=1:row
        delX = i - centerX;
        delY = j - centerY;
        delR = sqrt(power(delX, 2)+power(delY, 2));
        decay = abs(exp((-1)*decay_factor*delR/limitR));
        if (delR > rmax)
            kr_cos(j, i) = 128;
            kr_sin(j, i) = 128;
            continue;
        end
        
        inverse_kernel_cos = 128;
        inverse_kernel_cos = inverse_kernel_cos + 80*sin(1*f0*2*pi*delR/period);
        inverse_kernel_cos = inverse_kernel_cos + 32*sin(3*f0*2*pi*delR/period);
        inverse_kernel_cos = inverse_kernel_cos + 16*sin(5*f0*2*pi*delR/period);
        kr_cos(j, i) = decay*inverse_kernel_cos;
        if (kr_cos(j, i) < 0)
            kr_cos(j, i) = 0;
        end
        
        inverse_kernel_sin = 128;
        inverse_kernel_sin = inverse_kernel_sin + 80*cos(1*f0*2*pi*delR/period);
        inverse_kernel_sin = inverse_kernel_sin + 32*cos(3*f0*2*pi*delR/period);
        inverse_kernel_sin = inverse_kernel_sin + 16*cos(5*f0*2*pi*delR/period);
        kr_sin(j, i) = decay*inverse_kernel_sin;
        if (kr_sin(j, i) < 0)
            kr_sin(j, i) = 0;
        end
    end
end

kr_cos_norm = kr_cos ./ sum(kr_cos(:));
kr_sin_norm = kr_sin ./ sum(kr_sin(:));

imshow(kr_cos_norm, []); title('kernel of cos');
imshow(kr_sin_norm, []); title('kernel of sin');

% without concept of conjugate
Hcos = dct2(kr_cos_norm);
Hsin = dct2(kr_sin_norm);

Hreal = Hcos;
Himag = Hsin;

Ireal = dct2(im_cos);
Iimag = dct2(im_sin);

Yreal = Ireal.*Hreal + Iimag.*Himag;
Yimag = Ireal.*Himag - Iimag.*Hreal;
y_cos = idct2(Yreal);
y_sin = idct2(Yimag);

if true
figure,imshow(abs(log(Hreal)), []); title('H of real');
figure,imshow(abs(log(Himag)), []); title('H of imaginary');
figure,imshow(abs(log(Ireal)), []); title('I of real');
figure,imshow(abs(log(Iimag)), []); title('I of imaginary');
figure,imshow(abs(log(Yreal)), []); title('Y of real');
figure,imshow(abs(log(Yimag)), []); title('Y of imaginary');
end

figure, imshow(y_cos, []); title('y cos'); colorbar;
figure, imshow(y_sin, []); title('y sin'); colorbar;
figure, imshow(y_cos+y_sin, []); title('output');

% with concept of conjugate
Hcos = dct2(kr_cos);
Hsin = dct2(kr_sin);

absH = sqrt(abs(Hcos)+abs(Hsin));

Hreal = Hcos./absH;
Himag = (-1).*Hsin./absH;

Ireal = dct2(im_cos);
Iimag = dct2(im_sin);

Y = Ireal.*Hreal - Iimag.*Himag;
y = idct2(Y);

imshow(y, []); 
title('output of cos with H conjugate and normalize');

%}
