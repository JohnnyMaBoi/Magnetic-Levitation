function sensor_analog_read = match_solenoid_value(solenoid_analog_write)
    sensor_analog_read = -(solenoid_analog_write ./ 62.5) .^ 3 - solenoid_analog_write * 0.015 + 509
end