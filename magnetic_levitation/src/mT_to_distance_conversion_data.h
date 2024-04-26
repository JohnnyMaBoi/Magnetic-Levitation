const unsigned int n_mT_to_distance_conversion_data_points = 18;

// Hall to Distance Lookups
float mT_to_distance_conversion_distances[n_mT_to_distance_conversion_data_points] = {
    // closest to farthest
    0.190909092,
    0.254545456,
    0.31818182,
    0.381818184,
    0.445454545,
    0.509090912,
    0.572727276,
    0.63636364,
    0.700000004,
    1,
    1.5,
    2,
    2.5,
    3,
    3.5,
    4,
    4.5,
    5,
};

float mT_to_distance_conversion_mT[n_mT_to_distance_conversion_data_points] = {
    // closest to farthest, magnet-specific
    330,
    275,
    225,
    190,
    160,
    138.5,
    122,
    109.7,
    83,
    37.24,
    18.3,
    9.8,
    5.88,
    3.27,
    1.31,
    .65,
    0,
    -.65,
};
