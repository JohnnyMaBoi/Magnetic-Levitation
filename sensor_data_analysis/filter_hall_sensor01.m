load('raw_mT_01.mat');

%% plot raw data
figure(1);
clf;
hold on;
plot(hall_mT.timestampms, hall_mT.hall_mT, 'b-');
xlabel("Timestamp (ms)");
ylabel("Hall effect sensor (mT)");
title("Unfiltered data");

%% plot raw data
figure(2);
clf;
hold on;
plot(hall_mT.timestampms, hall_mT.hall_mT, 'b-');
plot(hall_mT.timestampms, movmean(hall_mT.hall_mT, [25, 0]), 'k-');
xlabel("Timestamp (ms)");
ylabel("Hall effect sensor (mT)");
title("25-point moving average");

%% peak detectors v1
% softmax([x1, x2, ..., xn]) = log(sum(exp([x1, x2, ..., xn]))
% moving softmax = log(
filter_len = 3;
buffer_factor = -0.2;
moving_softmax_filter = ones(1, filter_len);
peak_detector_top = log(conv(exp(hall_mT.hall_mT), moving_softmax_filter, 'same') ./ filter_len);
peak_detector_bottom = -log(conv(exp(-hall_mT.hall_mT), moving_softmax_filter, 'same') ./ filter_len);
peak_detector_diff = peak_detector_top - peak_detector_bottom;
peak_detector_top = peak_detector_top + peak_detector_diff * buffer_factor;
peak_detector_bottom = peak_detector_bottom - peak_detector_diff * buffer_factor;
figure(3);
clf;
hold on;
plot(hall_mT.timestampms, hall_mT.hall_mT, 'b-');
plot(hall_mT.timestampms, peak_detector_top, 'r-');
plot(hall_mT.timestampms, peak_detector_bottom, 'g-');
xlabel("Timestamp (ms)");
ylabel("Hall effect sensor (mT)");
title("Softmax peak detectors");

%% peak detectors v2
moving_softmax_filter = ones(1, filter_len);
peak_detector_top = movmax(hall_mT.hall_mT, [filter_len, 0]);
peak_detector_bottom = movmin(hall_mT.hall_mT, [filter_len, 0]);
peak_detector_diff = peak_detector_top - peak_detector_bottom;
peak_detector_top = peak_detector_top + peak_detector_diff * buffer_factor;
peak_detector_bottom = peak_detector_bottom - peak_detector_diff * buffer_factor;
figure(4);
clf;
hold on;
plot(hall_mT.timestampms, hall_mT.hall_mT, 'b-');
plot(hall_mT.timestampms, peak_detector_top, 'r-');
plot(hall_mT.timestampms, peak_detector_bottom, 'g-');

%% bouncer average
filter_len = 3;
buffer_factor = -0.8;

filtered_mT = zeros(size(hall_mT.hall_mT));
filtered_mT(1:filter_len) = hall_mT.hall_mT(1);
max_vals = zeros(size(hall_mT.hall_mT));
min_vals = zeros(size(hall_mT.hall_mT));
max_vals(1:filter_len) = hall_mT.hall_mT(1);
min_vals(1:filter_len) = hall_mT.hall_mT(1);

for idx = 2:length(hall_mT.hall_mT)
    moving_max_len = min(idx - 1, filter_len);
    moving_max_value = max(hall_mT.hall_mT(idx - moving_max_len:idx - 1));
    moving_min_value = min(hall_mT.hall_mT(idx - moving_max_len:idx - 1));
    max_vals(idx) = moving_max_value;
    min_vals(idx) = moving_min_value;
    minmax_diff = max_vals(idx) - min_vals(idx);
    max_vals(idx) = max_vals(idx) + minmax_diff * buffer_factor;
    min_vals(idx) = moving_min_value - minmax_diff * buffer_factor;

    if filtered_mT(idx - 1) < moving_min_value || filtered_mT(idx - 1) > moving_max_value
        filtered_mT(idx) = (moving_min_value + moving_max_value) ./ 2;
    else
        filtered_mT(idx) = filtered_mT(idx - 1);
    end
end
figure(5);
clf;
hold on;
plot(hall_mT.timestampms, hall_mT.hall_mT, 'b-');
plot(hall_mT.timestampms, max_vals, 'r-');
plot(hall_mT.timestampms, min_vals, 'g-');
plot(hall_mT.timestampms, filtered_mT, 'k-');
xlabel("Timestamp (ms)");
ylabel("Hall effect sensor (mT)");
title("Moving max peak detectors");

%% derivative signal
figure(6);
clf;
hold on;
plot(hall_mT.timestampms, hall_mT.hall_mT, 'b-');
plot(hall_mT.timestampms, filtered_mT, 'k-');
plot(hall_mT.timestampms, movmean(filtered_mT, [2, 0]), 'g.-');
plot(hall_mT.timestampms, movmean(hall_mT.hall_mT, [3, 0]), 'r.-');
xlabel("Timestamp (ms)");
ylabel("Hall effect sensor (mT)");
title("Moving max peak detectors plus low pass filter");