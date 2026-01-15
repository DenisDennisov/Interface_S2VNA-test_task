// Abstract interface Config (Model).
// Allows you to replace the implementation without changing the logic.

#ifndef IVNACONFIG_H
#define IVNACONFIG_H


// Storing parameters for communication between Model and View.
// !! Run on the main thread !!
struct VnaConfig
{
    double startFreq{0.0};          // Start, GHz
    double stopFreq {0.0};          // Stop, GHz
    int points{0};                  // points, int
    double power{0.0};              // power, dBm
    int ifBw{0};                    // ifBw, Hz
};


class IConfigModel
{
public:
    explicit IConfigModel() {}
    virtual ~IConfigModel() = default;

    // ======== Getter data ========
    virtual VnaConfig getConfig() const = 0;

    // ======== Setter data ========
    virtual void setConfig(const VnaConfig&) = 0;
};

#endif // IVNACONFIG_H
