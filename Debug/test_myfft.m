close all;

x1 = [1 2 3 4];
x2 = [5 6 7 8];
x3 = [x1 x2];
Nt = 8;
x3even = x3((1:2:length(x3)));
x3odd  = x3((2:2:length(x3)));
Wt = exp(-1*sqrt(-1)*2*pi*1/Nt).^(0:((Nt/2)-1)).';

%myfft(x3);
%signal
f0 = 0.1;
dt = 0.25;
t = (0:dt:256-dt);
N = length(t);
fs = 1/dt;
df = fs/N;
f = (0:df:fs-df);
t = t(1:length(t)-mod(length(t),2));
f = f(1:length(f)-mod(length(f),2));
x = 4*sin(2*pi*f0*t) + 1*sin(2*pi*8*f0*t);
x = x(1:length(x)-mod(length(x),2));
X_myfft = myfft(x);

figure('Name','myfft','NumberTitle','off');
%figure('Name','x','NumberTitle','off');
subplot(2, 2, 1);
plot(t,x);
xlabel('time');
ylabel('x');

%figure('Name','myfft','NumberTitle','off');
subplot(2, 2, 2);
plot(f,abs(X_myfft), '-.');
xlabel('freq');
ylabel('myfft');

X_myfftNN = myfftNN(x);
%figure('Name','myfftNN','NumberTitle','off');
subplot(2, 2, 3);
plot(f,abs(X_myfftNN), '-.');
xlabel('freq');
ylabel('myfftNN');

X_fft = fft(x, 1024);
%figure('Name','fft','NumberTitle','off');
subplot(2, 2, 4);
plot(f,abs(X_fft), '-.');
xlabel('freq');
ylabel('fft');

function X = myfft(x)
    N = length(x);
    if (N == 1) %exp(-1i*2*pi*k*1/1) = 1
        X = x;
    else
        xe = x(1:2:N);
        xo = x(2:2:N);
        XE = myfft(xe);
        XO = myfft(xo);
        W  = exp(-1*sqrt(-1)*2*pi*1/N).^(0:(N/2)-1).';
        X1 = XE + W.*XO; %X[k] = [X[0];X[1];...]
        X2 = XE - W.*XO; %X[k+N/2] = [X[0+N/2];X[1+N/2];...]
        X  = [X1;X2]; 
    end
end

function X = myfftNN(x)
    N = length(x);
    X = zeros(1,N);
    for k = 1:N
        for n = 1:N
            %X[k] = X[k] + x[n]*exp(-1*sqrt(-1)*2*pi*k*n/N);
            X(k) = X(k) + x(n)*exp(-1i*2*pi*(k-1)*(n-1)/N);
        end
    end
end

















