class Kalman {
public:
    Kalman();
    float update(float measurement);
    float getEstimate() const;

private:
    float estimate; // Current state estimate
    float errorCovariance; // Estimate error covariance
    float Q; // Process noise covariance
    float R; // Measurement noise covariance
};