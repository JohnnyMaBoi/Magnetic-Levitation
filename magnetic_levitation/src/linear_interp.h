// NOTE: arrays must be in increasing order ////////////////////////////
int linear_interp(int x_val, int* x_data, int* y_data, unsigned int n_data_points) {
    // interpolate between two points for known sets of x and y data
    // x_val: value to do y interpolation for
    // x_data, y_data: arrays of known coordinates in x and y
    // n_data_points: lengths of both of the arrays

    unsigned int idx = 0;
    for (; idx < n_data_points; ++idx) {
        // iterate until x_data is exceeded by one of the data points we have.
        // this means x_val is between x_data[idx - 1] and x_data[i].
        if (x_val < x_data[idx]) {
            break;  // out of the for loop
        }
    }
    if (idx == 0) {  // return the lowest value in the range
        return y_data[0];
    } else if (idx == n_data_points - 1) {  // return the highest value in the range
        return y_data[n_data_points - 1];
    } else {  // compute the linear interpolation
        return int(((float(x_val - x_data[idx - 1]) / float(x_data[idx] - x_data[idx - 1])) * float(y_data[idx] - y_data[idx - 1])) + y_data[idx - 1]);
    }
}

// verbatim copy of above, just using floats
float linear_interp(float x_val, float* x_data, float* y_data, unsigned int n_data_points) {
    unsigned int idx = 0;
    for (; idx < n_data_points; ++idx) {
        // iterate until x_data is exceeded by one of the data points we have.
        // this means x_val is between x_data[idx - 1] and x_data[i].
        if (x_val < x_data[idx]) {
            break;  // out of the for loop
        }
    }
    if (idx == 0) {  // return the lowest value in the range
        return x_data[0];
    } else if (idx == n_data_points - 1) {  // return the highest value in the range
        return x_data[n_data_points - 1];
    } else {  // compute the linear interpolation
        return (((float(x_val - x_data[idx - 1]) / float(x_data[idx] - x_data[idx - 1])) * float(y_data[idx] - y_data[idx - 1])) + y_data[idx - 1]);
    }
}
