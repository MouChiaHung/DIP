function out = unifdither(im,amplitude,thr)
if nargin<3 
    thr=128;
end
%random = rand(size(im));
%noise = (rand(size(im))-0.5)*2*amplitude;
noise = (rand(size(im)))*amplitude*8;
row=512;  col=512;
figure('Name',"noise",'NumberTitle','off');
noise=uint8(noise);
ND=reshape(noise,row,col);
ND=ND';
imshow(ND);
title('noise');

%imn = double(im) + noise;
imn = (im) + noise;
out = imn > thr;