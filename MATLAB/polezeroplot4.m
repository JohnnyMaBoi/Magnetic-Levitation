b = 1.04e-1; % N * s / mm
m = 3.00e-4; % kg
a = 9.59e-1; % N / mm
Tdelay = 1e-3; % s
LowPass_delay = 10e-3; % s

Fm0 = 7.01e-02; % N / A

Kp = 1; % parameter to vary
Kd = 1 ./ 20;

C_s = tf([Kd, Kp], [1]);
G_s = tf([Fm0], [m, b, -a]);
Delay_s = tf([Tdelay .^ 2 ./ 12, -Tdelay ./ 2, 1], ...
             [Tdelay .^ 2 ./ 12,  Tdelay ./ 2, 1]);
LowPass_s = tf([1], [1, 1 ./ LowPass_delay]);

G2_s = C_s * G_s
G3_s = G2_s * Delay_s
G4_s = G3_s * LowPass_s

figure(1);
clf;
rlocus(G_s)

figure(2);
clf;
rlocus(G_s, G2_s)

figure(3);
clf;
rlocus(G3_s)


figure(4);
clf;
rlocus(LowPass_s)

figure(5);
clf;
rlocus(G4_s)