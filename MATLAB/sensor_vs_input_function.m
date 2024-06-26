load('sensor_vs_input.mat')
analog_write = sensorvsinput.AnalogWriteValue;
sensor_reading = sensorvsinput.SensorReading;

sweep_len = 256;

figure(1);
clf;
hold on;
full_range = zeros(size(sensorvsinput.AnalogWriteValue));
range_1 = logical(full_range);
range_1(1:sweep_len) = true;
range_2 = logical(full_range);
range_2(sweep_len + 1:3 * sweep_len) = true;
range_3 = logical(full_range);
range_3(sweep_len * 3 + 1:sweep_len * 5) = true;
range_4 = logical(full_range);
range_4(sweep_len * 5 + 1:end) = true;
% plot(analog_write(range_1), sensor_reading(range_1));
plot(analog_write(range_2), sensor_reading(range_2));
plot(analog_write(range_2), match_solenoid_value(analog_write(range_2)));
a = polyfit(analog_write(range_2), movmean(sensor_reading(range_2), 40), 3);
plot(analog_write(range_2), analog_write(range_2) .^ 3 .* a(1) + analog_write(range_2) .^ 2 .* a(2) + analog_write(range_2) .* a(3) + a(4));
%%
figure(2);
clf;
hold on;
resample_factor = 10;
resample_points = 1:resample_factor:length(analog_write(range_2));
x_vals_to_resample = analog_write(range_2);
resampled_x_vals = x_vals_to_resample(resample_points);
y_vals_to_resample = movmean(sensor_reading(range_2), 10);
resampled_y_vals = y_vals_to_resample(resample_points);

plot(analog_write(range_2), sensor_reading(range_2));
plot(analog_write(range_2), movmean(sensor_reading(range_2), 10));
plot(analog_write(range_2), match_solenoid_value(analog_write(range_2)));

resampled = resample(movmean(sensor_reading(range_2), 10), 1, resample_factor);
plot(resampled_x_vals, resampled_y_vals, 'o');

correction_pairs = [resampled_x_vals, resampled_y_vals]

figure(3);
clf;
hold on;
plot(analog_write(range_2), movmean(sensor_reading(range_2), 10) - match_solenoid_value(analog_write(range_2)));

%%
% sensor reading is 
sensor_voltage = (sensorvsinput.SensorReading).*(3.3/1023);

%terms from datasheet page 10, sens from table
Vq = 5/2;
sensitivity_term = 7.5; % changes by device & voltage
temp_effect = sensitivity_term*(1+(22-25)*.0012); %assuming 22 deg celsius 
% solve for magnetic flux b
sensor_flux = (sensor_voltage - Vq)/(temp_effect);
figure(4);
%values for plotting SOLENOID SCALING ARBITRARY
time = 1:size(sensorvsinput.AnalogWriteValue);
solenoid_scale = .1/1024;
hold on;
plot(time, sensor_flux, Color='r');
plot(time, analog_write .* solenoid_scale, Color='b');
xlabel("Sensor readings over time");
legend("Sensed flux in Teslas", "INCORRECT SCALE solenoid current");
title("Sensor Flux in Teslas");

% From reading the motor board page, it seems it has "magic PWM" support
% which sounds suspicious. I suspect that the noise we are seeing
% is from this PWM. Moving-mean should show what the average magnetic
% flux is, but this might indicate we will need to do some 
% averaging on the sensing end of our arduino code as well 
% chose 40 as its width of 0-ish sections
figure(5);
pwm_adjusted_sensor = movmean(sensor_flux, 40);
hold on;
plot(time, pwm_adjusted_sensor, Color='r');
plot(time, analog_write*solenoid_scale, Color='b');
xlabel("Sensor readings over time");
legend("PWM-removed Sensed flux in Teslas", "INCORRECT SCALE solenoid current");
title("PWM-removed flux over time");


% plot(analog_write(range_3), sensor_reading(range_3));
% plot(analog_write(range_4), sensor_reading(range_4));
