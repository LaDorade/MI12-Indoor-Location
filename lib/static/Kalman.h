class Kalman {
public:
    Kalman();
    float update(float measurement);
    float getEstimate() const;
    
    // Setters pour les paramètres
    void setQ(float q);
    void setR(float r);
    void reset();
    
    // Getters pour les paramètres
    float getQ() const;
    float getR() const;

private:
    float estimate; // Current state estimate
    float errorCovariance; // Estimate error covariance
    float Q; // Process noise covariance
    float R; // Measurement noise covariance
};