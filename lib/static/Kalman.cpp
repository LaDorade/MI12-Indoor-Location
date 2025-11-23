#include "Kalman.h"

Kalman::Kalman() {
    estimate = 0.0f;
    errorCovariance = 1.0f;
    Q = 0.1f; // Process noise covariance
    R = 2.0f;  // Measurement noise covariance
}

float Kalman::update(float measurement) {
    // Prediction update
    errorCovariance += Q;

    // Measurement update
    float kalmanGain = errorCovariance / (errorCovariance + R);
    estimate += kalmanGain * (measurement - estimate);
    errorCovariance *= (1 - kalmanGain);

    return estimate;
}

float Kalman::getEstimate() const {
    return estimate;
}