const unsigned int n_mT_to_distance_conversion_data_points = 18;

// Hall to Distance Lookups
float mT_to_distance_conversion_distances[n_mT_to_distance_conversion_data_points] = {
    // closest to farthest
    5,            // -0.65  (mT)    // 0
    4.5,          // 0.65   (mT)    // 1
    4,            // 0      (mT)    // 2
    3.5,          // 1.31   (mT)    // 3
    3,            // 3.27   (mT)    // 4
    2.5,          // 5.88   (mT)    // 5
    2,            // 9.8    (mT)    // 6
    1.5,          // 18.3   (mT)    // 7
    1,            // 37.24  (mT)    // 8
    0.700000004,  // 83     (mT)    // 9
    0.63636364,   // 109.7  (mT)    // 10
    0.572727276,  // 122    (mT)    // 11
    0.509090912,  // 138.5  (mT)    // 12
    0.445454545,  // 160    (mT)    // 13
    0.381818184,  // 190    (mT)    // 14
    0.31818182,   // 225    (mT)    // 15
    0.254545456,  // 275    (mT)    // 16
    0.190909092,  // 330    (mT)    // 17
};

float mT_to_distance_conversion_mT[n_mT_to_distance_conversion_data_points] = {
    // closest to farthest, magnet-specific
    -0.65,  // 0
    0.65,   // 1
    0,      // 2
    1.31,   // 3
    3.27,   // 4
    5.88,   // 5
    9.8,    // 6
    18.3,   // 7
    37.24,  // 8
    83,     // 9
    109.7,  // 10
    122,    // 11
    138.5,  // 12
    160,    // 13
    190,    // 14
    225,    // 15
    275,    // 16
    330,    // 17
};
