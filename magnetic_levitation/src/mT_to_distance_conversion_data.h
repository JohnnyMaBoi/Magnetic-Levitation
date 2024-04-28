const unsigned int n_mT_to_distance_conversion_data_points = 18;

// Hall to Distance Lookups
float mT_to_distance_conversion_distances[n_mT_to_distance_conversion_data_points] = {
    // closest to farthest
    5,
    4.5,
    4,
    3.5,
    3,
    2.5,
    2,
    1.5,
    1,
    0.700000004,
    0.63636364,
    0.572727276,
    0.509090912,
    0.445454545,
    0.381818184,
    0.31818182,
    0.254545456,
    0.190909092,
};

float mT_to_distance_conversion_mT[n_mT_to_distance_conversion_data_points] = {
    // closest to farthest, magnet-specific
    -.65,
    .65,
    0,
    1.31,
    109.7,
    122,
    138.5,
    160,
    18.3,
    190,
    225,
    275,
    3.27,
    330,
    37.24,
    5.88,
    83,
    9.8,
};
