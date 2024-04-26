b = 1.04e-2; % N * s / mm
m = 6.00e-3; % g
a = 4.06e-0; % N / mm

Fm0 = 7.01e-02; % N / A

Kp = 1; % parameter to vary
Kd = 1 ./ 60;

C_s = tf([Kd, Kp], [1]);
G_s = tf([Fm0], [m, b, -a]);

G2_s = C_s * G_s

figure(1);
clf;
rlocus(G_s)
figure(2);
clf;
rlocus(G_s, G2_s)