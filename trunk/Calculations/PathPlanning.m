clear all

syms a t t_b x1_0 y1_0 x2_0 y2_0 v3 v4;
syms V1 V2 T1 T2

x1 = x1_0 + 1/2 * a * t_b^2 + V1 * cos(T1) * (t - t_b) + 1/2 * a * t_b^2; % robot position in x
y1 = y1_0 + 1/2 * a * t_b^2 + V1 * sin(T1) * (t - t_b) + 1/2 * a * t_b^2; % robot position in y

x2 = x2_0 + v3*t;
y2 = y2_0 + v4*t;

x1 = subs(x1, a, 200);
y1 = subs(y1, a, 200);
x1 = subs(x1, V1, 85);
y1 = subs(y1, V1, 85);