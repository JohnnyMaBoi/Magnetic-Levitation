#pragma once
#include <Arduino.h>

// Perform moving average filter
const unsigned int moving_average_len = 40;
int moving_average_array[moving_average_len];

int moving_average_filter(int new_val) {
    int moving_average = new_val;
    for (unsigned int i = 0; i < moving_average_len - 1; i++) {
        moving_average += moving_average_array[moving_average_len - i - 1];
        moving_average_array[moving_average_len - i - 1] = moving_average_array[moving_average_len - i - 2];
    }
    moving_average_array[0] = new_val;
    return moving_average / moving_average_len;
}

// Perform min-max filter
const unsigned int moving_minmax_len = 20;
float buffer_factor = 0;
float moving_minmax_array[moving_minmax_len];

int minmax_filter(int new_val) {
    int prev_val = moving_minmax_array[0];
    int max_val = -10000;
    int min_val = 10000;
    for (unsigned int i = 0; i < moving_minmax_len - 1; i++) {
        max_val = max(max_val, moving_minmax_array[moving_minmax_len - i - 1]);
        min_val = min(min_val, moving_minmax_array[moving_minmax_len - i - 1]);
        moving_minmax_array[moving_minmax_len - i - 1] = moving_minmax_array[moving_minmax_len - i - 2];
    }
    int minmax_diff = max_val - min_val;
    int max_val_overall = max_val + minmax_diff * buffer_factor;
    int min_val_overall = min_val - minmax_diff * buffer_factor;

    moving_minmax_array[0] = new_val;
    if ((new_val > max_val_overall) || (new_val < min_val_overall)) {
        return (max_val + min_val) / 2;
    } else {
        return prev_val;
    }
}
