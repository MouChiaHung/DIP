row=512;  col=512;
file_raw = 'tif.raw';
fin=fopen(file_raw,'r');
I=fread(fin,row*col,'uint8=>uint8'); 
Z=reshape(I,row,col);
Z=Z';
fclose(fin);
figure('Name',file_raw,'NumberTitle','off');
%subplot(2,2,1);
imshow(Z);
title(file_raw);

%{
IDither = unifdither(I,64,128);
IDither=255*uint8(IDither);
ZD=reshape(IDither,row,col);
ZD=ZD';
%subplot(2,2,2);
figure('Name','unifdither','NumberTitle','off');
imshow(ZD);
title('unifdither');
%}

%ZH = halftone(Z, 'dots');
ZH = halftone(Z, 'bayer_2x2');
ZH=255*uint8(ZH);
ZH=reshape(ZH,row,col);
%subplot(2,2,2);
figure('Name','halftone','NumberTitle','off');
imshow(ZH);
title('halftone');
