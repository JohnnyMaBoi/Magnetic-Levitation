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
% plot(analog_write(range_3), sensor_reading(range_3));
% plot(analog_write(range_4), sensor_reading(range_4));