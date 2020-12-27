clear all;
close all;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% MOU LAB
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%row=180;  col=80;
%row=376;  col=240;
row=100;  col=110;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%{
base = bf(row,col,1,0);
BASE = fft2(base);
BASE = fftshift(BASE);
figure('Name','example','NumberTitle','on');
subplot(4,2,1);

imshow(base, [0,255]);
title('base image');
subplot(4,2,2);
imshow((abs(BASE)), []);
title('2D FFT of base image');

noise = bf_noise(row,col,1);
NOISE = fft2(noise);
NOISE = fftshift(NOISE);
subplot(4,2,3);
imshow(noise, [0,255]);
title('noise');
subplot(4,2,4);
imshow((abs(NOISE)), []);
title('2D FFT of noise');

noised = noise + base;
NOISED = fft2(noised);
NOISED = fftshift(NOISED);
subplot(4,2,5);
imshow(noised, [0,255]);
title('noised image');
subplot(4,2,6);
imshow((abs(NOISED)), []);
title('2D FFT of noised image');

DE_NOISE = NOISED - NOISE;
subplot(4,2,8);
imshow((abs(DE_NOISE)), []);
title('2D FFT of noised image - 2D FFT of noise');

de_noise = uint8(ifft2(ifftshift(DE_NOISE), 'nonsymmetric'));
subplot(4,2,7);
imshow(de_noise, [0,255]);
title('final image filtered by inverse 2D FFT of (2D FFT of noised image - 2D FFT of noise)');
return;
%}

%foo('Finger', row, col);
%foo('Finger_filtered', row, col);
%filter('Finger','Finger_filtered', row, col);
%filter('NG','GND', row, col);
%filter('No_GND_no_charge\1605654214424','GND_no_charge\1605654090835', row, col);
%filter('No_GND_no_charge\1605654215487','GND_no_charge\1605654093272', row, col);
%filter('No_GND_no_charge\1605654216505','GND_no_charge\1605654094949', row, col);
%filter('No_GND_no_charge\1605654217487','GND_no_charge\1605654096608', row, col);
%filter('No_GND_no_charge\1605654218469','GND_no_charge\1605654098065', row, col);

%foo('files\gnd_charger_laptop', row, col);
foo('finger_on_corner_1', row, col);
%foo('finger_on_corner_2', row, col);

psd('finger_on_corner_1', row, col);
psd('files\gnd_charger_laptop', row, col);

return;


function ret = psd(file, row, col)
    fin=fopen(file,'r');
    I=fread(fin,row*col,'uint8=>uint8');
    Z=uint8(I);
    Z=reshape(Z,row,col);
    Z=Z';
    fclose(fin);
    figure('Name',file,'NumberTitle','off');
    subplot(1,2,1);
    imshow(Z, [0,255]);
    title(file);
    
    I_VCL = double(Z(row/2,:));
    subplot(1,2,2);
    pwelch(I_VCL);
end



function ret = foo(file, row, col)
    fin=fopen(file,'r');
    IPGM=fread(fin,row*col,'uint8=>uint8');
    ZPGM=uint8(IPGM);
    ZPGM=reshape(ZPGM,row,col);
    ZPGM=ZPGM';
    fclose(fin);
    figure('Name',file,'NumberTitle','off');
    subplot(2,2,1);
    imshow(ZPGM, [0,255]);
    title(file);

    %fft2
    FPGM = fft2(ZPGM);
    FPGMAbs = abs(FPGM);
    %remove DC
    %
    for j = 1 : 1
        for i = 1 : 1
            FPGMAbs(i,j) = 0;
            FPGMAbs((col+1)-i,j) = 0;
            FPGMAbs(i,(row+1)-j) = 0;
            FPGMAbs((col+1)-i,(row+1)-j) = 0;    
        end
    end
    %
    magFPGM = (1/(row*col)*abs(fftshift(FPGMAbs)));
    %magFPGM = abs(fftshift(FPGMAbs));
    subplot(2,2,2);
    imshow(magFPGM, []);
    title('fft2');

    magFPGM_HCL = magFPGM(:,row/2);
    subplot(2,2,3);
    plot(magFPGM_HCL);
    title('fft2 horizontal central line');
    
    magFPGM_VCL = magFPGM(col/2,:);
    subplot(2,2,4);
    plot(magFPGM_VCL);
    title('fft2 vertical central line');
    
    subplot(2,2,4);
    %max_ = max(ZPGM(:));
    histogram(ZPGM);
    title('hist');
end

function ret = diff(file1, file2, row, col)
    fin=fopen(file1,'r');
    I1=fread(fin,row*col,'uint8=>uint8');
    Z1=uint8(I1);
    Z1=reshape(Z1,row,col);
    Z1=Z1';
    fclose(fin);

    fin=fopen(file2,'r');
    I2=fread(fin,row*col,'uint8=>uint8');
    Z2=uint8(I2);
    Z2=reshape(Z2,row,col);
    Z2=Z2';
    fclose(fin);
    
    ZPGM = Z2 - Z1;
    string = file1+"-"+file2;
    figure('Name',string,'NumberTitle','off');
    subplot(2,2,1);
    imshow(ZPGM, [min(min(Z1(:)):min(Z2(:))),max(max(Z1(:)):max(Z2(:)))]);
    title(string);
    
    %fft2
    FPGM = fft2(ZPGM);
    FPGMAbs = abs(FPGM);
    %remove DC
    %{
    for j = 1 : 4
        for i = 1 : 4
            FPGMAbs(i,j) = 0;
            FPGMAbs((col+1)-i,j) = 0;
            FPGMAbs(i,(row+1)-j) = 0;
            FPGMAbs((col+1)-i,(row+1)-j) = 0;    
        end
    end
    %}
    magFPGM = (1/(row*col)*abs(fftshift(FPGMAbs)));
    %magFPGM = (1/(row*col)*abs((FPGMAbs)));
    subplot(2,2,2);
    imshow(magFPGM, [0,255]);
    title('fft2');

    magFPGM_HCL = magFPGM(:,189);
    subplot(2,2,3);
    plot(magFPGM_HCL);
    title('fft2 horizontal central line');
    
    magFPGM_VCL = magFPGM(121,:);
    subplot(2,2,4);
    plot(magFPGM_VCL);
    title('fft2 vertical central line');
    
    subplot(2,2,4);
    histogram(ZPGM);
    title('hist');
end

function ret = filter(file1, file2, row, col)
    fin=fopen(file1,'r');
    I1=fread(fin,row*col,'uint8=>uint8');
    Z1=uint8(I1);
    Z1=reshape(Z1,row,col);
    Z1=Z1';
    fclose(fin);

    fin=fopen(file2,'r');
    I2=fread(fin,row*col,'uint8=>uint8');
    Z2=uint8(I2);
    Z2=reshape(Z2,row,col);
    Z2=Z2';
    fclose(fin);
    
    figure('Name','filter','NumberTitle','off');
    %%%%%%%%%%%%%%%fft2 for src
    F1 = fft2(Z1);
    F1_DC_pre = abs(F1);
    F1_DC = F1_DC_pre(1,1);
    %remove DC
    %
    for j = 1 : 1
        for i = 1 : 1
            F1(i,j) = 0;
            F1((col+1)-i,j) = 0;
            F1(i,(row+1)-j) = 0;
            F1((col+1)-i,(row+1)-j) = 0;    
        end
    end
    %
    F1ABS = abs(F1);
    F1ABS = (1/(row*col))*(fftshift(F1ABS));
    
    subplot(4,3,1);
    imshow(Z1, [0,255]);
    string1 = "raw image of " + file1;
    title(string1);
    
    subplot(4,3,2);
    imshow(F1ABS, []);
    string1 = "2D FFT of " + file1;
    title(string1);
    
    F1ABS_CL = F1ABS(:,189);
    subplot(4,3,3);
    plot(F1ABS_CL);
    string11 = "horizontal central line" + string1;
    title(string11);
    
    %%%%%%%%%%%%%%% fft2 for kernel
    F2 = fft2(Z2);
    F2_DC_pre = abs(F2);
    F2_DC = F2_DC_pre(1,1);
    %remove DC
    %
    for j = 1 : 1
        for i = 1 : 1
            F2(i,j) = 0;
            F2((col+1)-i,j) = 0;
            F2(i,(row+1)-j) = 0;
            F2((col+1)-i,(row+1)-j) = 0;    
        end
    end
    %
    subplot(4,3,4);
    imshow(Z2, [0,255]);
    string2 = "raw image of " + file2;
    title(string2);
    
    F2ABS = abs(F2);
    F2ABS = (1/(row*col))*(fftshift(F2ABS));
    subplot(4,3,5);
    imshow(F2ABS, []);
    string2 = "2D FFT of " + file2;
    title(string2);
    
    F2ABS_CL = F2ABS(:,189);
    subplot(4,3,6);
    plot(F2ABS_CL);
    string22 = "horizontal central line" + string2;
    title(string22);

    F3 = F1 - F2;
    F3ABS = abs(F3);
    F3ABS = (1/(row*col))*(fftshift(F3ABS));
    f3 = uint8(ifft2((F3),'nonsymmetric'));
    subplot(4,3,7);
    imshow(F3ABS, []);
    string3 = string1 + ' - ' + string2;
    title(string3);
    
    F3ABS_CL = F3ABS(:,189);
    subplot(4,3,12);
    plot(F3ABS_CL);
    string33 = "horizontal central line of " + string3;
    title(string33);
    
    subplot(4,3,10);
    imshow(f3, [0,255]);
    string4 = "IFFT of (" + string3 + ")";
    title(string4);

    figure('Name','plot','NumberTitle','off');
    F1ABS_CL_Half = (1)*F1ABS_CL(121:240);
    F2ABS_CL_Half = (1)*F2ABS_CL(121:240);
    plot((1:120), F1ABS_CL_Half, (1:120), F2ABS_CL_Half);
    
    figure('Name','plotNormalized','NumberTitle','off');
    F1ABS_CL_Half = (1/F1_DC)*F1ABS_CL(121:240);
    F2ABS_CL_Half = (1/F2_DC)*F2ABS_CL(121:240);
    plot((1:120), F1ABS_CL_Half, (1:120), F2ABS_CL_Half);
    
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function im = bf(row, col, u, v)
    N = row;
    M = col;
    [i, j] = meshgrid((0:(N-1)),(0:(M-1)));
    im = 255*real(exp(-1i*2*pi*((u*i/N)+(v*j/M))));
    if (u==0 && v==0)
        im = round(im);
    end
end

function im = bf_noise(row, col, mode)
    im = zeros(col,row);
    
    for i = 1 : row
        for j = 1 : col
            if (mode == 1)
                if (i==j)
                    im(i,j) = 255;
                end     
            elseif (mode == 2)
                if (i==row/2||i==1+row/2||i==2+row/2||i==3+row/2)
                    im(i,j) = 255;                 
                end     
            elseif (mode == 3)
                if (j==col/2||j==1+col/2||j==2+col/2||j==3+col/2)
                    im(i,j) = 255;                 
                end     
            end
        end
    end    
    
end