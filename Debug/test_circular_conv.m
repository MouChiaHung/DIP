x = [1 2 1 1];
h = [1 1 -1 -1];
y = conv(h, x);
ny = (0:length(y)-1);
close all;
figure('Name','y=conv(h,x)','NumberTitle','off');
stem(ny, y);
xlim([-1 7]);

N = 7;
X = fft(x,N);
H = fft(h,N);

figure('Name','X','NumberTitle','off');
stem((0:N-1),abs(X), '-.');
xlim([-1 7]);

figure('Name','H','NumberTitle','off');
stem((0:N-1),abs(H), '-.');
xlim([-1 7]);

Y = H.*X;
figure('Name','Y','NumberTitle','off');
stem((0:N-1),abs(Y), '-.');
xlim([-1 7]);

yifft = ifft(Y,N);
figure('Name','y=ifft(Y,N)','NumberTitle','off');
stem((0:N-1),yifft, '-.');
xlim([-1 7]);

%circular convolution
lenx = length(x);
lenh = length(h);
x_ = [x, zeros(1,lenh)];
h_ = [h, zeros(1,lenx)];
y_ = zeros(1, lenx+lenh);
for n = 1:length(y_)
    for k = 1:length(h_)
        if ((n-k)>0)
            y_(n) = y_(n) + h_(k)*x_(mod((n-k),8)); 
        end
    end
end

ny_ = 0:length(y_)-1;
figure('Name','circular convolution','NumberTitle','off');
stem(ny_, y_, '-.');
xlim([-1 7]);









