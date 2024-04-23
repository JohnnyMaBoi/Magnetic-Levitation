load('smoothed_data.mat');
sweep_len = 256;
full_range = zeros(size(analog_write));
range_2 = logical(full_range);
range_2(sweep_len + 1:3 * sweep_len) = true;

solenoid_setting = analog_write(range_2);
hall_effect = pwm_adjusted_sensor(range_2);

figure(1);
clf;
hold on;
plot(solenoid_setting, hall_effect);
plot(solenoid_setting, -(solenoid_setting / 850) .^ 3 - 0.115 - solenoid_setting / 8e4);
title("-((solenoid setting) / 850)\^3 - (solenoid setting) / 8e4 - 0.115");
xlabel("Analog write value to solenoid");
ylabel("Sensor reading (T)");