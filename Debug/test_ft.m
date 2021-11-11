%signal
dt = 0.005;
t = (-6:dt:6);
w1 = 1;
w2 = 8;
x = 10*sin(2*pi*w1*t) + 2*sin(2*pi*w2*t);

figure('Name','FT','NumberTitle','off');
subplot(3, 2, 1);
plot(t, x);
xlabel('time(s)');
ylabel('x');

%FT
df = 0.005;
f = (-10:df:10);
X = zeros(1,length(f));
for k = 1:length(f)
    for n = 1:length(t)
        %X[k] = X[k] + x[n]*exp(-1i*2*pi*f[k]*t[n])*dt;
        X(k) = X(k) + x(n)*exp(-1i*2*pi*f(k)*t(n))*dt;
    end
end

magX = abs(X);

subplot(3, 2, 2);
plot(f, magX);
xlabel('freq(Hz)');
ylabel('X')

%Filter FT - BPF
wb = 6;
BPF = heaviside(f+wb) - heaviside(f-wb);
subplot(3, 2, 4);
plot(f, BPF);
xlabel('freq(Hz)');
ylabel('BPF');

%W = HX
X_ = zeros(1, length(f));
for k = 1 : length(f)
    for n = 1 : length(t)
        X_(k) = X_(k) + x(n)*exp(-1i*2*pi*f(k)*t(n))*dt;
    end
end

W = BPF.*X_;
%plot(f, abs(W));
%xlabel('freq(Hz)');
%ylabel('HX');
%title('W = HX');

%Filter IFT - bpf
bpf = zeros(1, length(t));
for n = 1 : length(t)
    for k = 1 : length(f)
        bpf(n) = bpf(n) + BPF(k)*exp(1i*2*pi*f(k)*t(n))*df;
    end
end

subplot(3, 2, 3);
plot(t, real(bpf));
xlabel('time(s)');
ylabel('bpf');

%y=h*x
len_bpf = length(bpf);
len_x = length(x);
h = [bpf, zeros(1, len_x)];
s = [x, zeros(1, len_bpf)];
y = zeros(1, len_bpf+len_x-1);
l = length(y);
for n = 1 : l
   for k = 1 : len_bpf
       if ((n-k)>1)
           y(n) = y(n) + h(k)*s(n-k)*dt;
       end
   end
end

t_ = (-12:dt:12);
if (length(t_) == l)
    subplot(3, 2, 5);
    plot(t_, real(y));
    xlim([-6 6]);
    xlabel('time(s)');
    ylabel('y');
    title('y = bpf * x');
end

%Y
Y = zeros(1,length(f));
for k = 1 : length(f)
    for n = 1 : length(y)
        Y(k) = Y(k) + y(n)*exp(-1i*2*pi*f(k)*t_(n))*df;
    end
end

%figure('Name','Y','NumberTitle','off');
subplot(3, 2, 6);
plot(f, abs(Y));
title('Y = FT of y');
xlabel('freq(Hz)');
ylabel('Y');




