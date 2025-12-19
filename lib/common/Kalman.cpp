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

void Kalman::setQ(float q) {
    Q = q;
}

void Kalman::setR(float r) {
    R = r;
}

void Kalman::reset() {
    estimate = 0.0f;
    errorCovariance = 1.0f;
}

float Kalman::getQ() const {
    return Q;
}

float Kalman::getR() const {
    return R;
}