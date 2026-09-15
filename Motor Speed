#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <deque>
#include <numeric>

constexpr double PULSES_PER_REV  = 12.0;          // 12 pulses per mechanical revolution
constexpr uint32_t SAMPLE_TIME_MS = 5000;         // Sample window in milliseconds
constexpr size_t AVG_WINDOW_SIZE = 20;            // Moving average window length

std::atomic<uint32_t> pulseCount(0);

// --- Called when a tach pulse is detected
void onTachPulse() {
    pulseCount++;
}

// --- Calculate instantaneous motor RPM (using milliseconds) ---
double calculateMotorRPM(uint32_t sampleTimeMs = SAMPLE_TIME_MS) {
    uint32_t startCount = pulseCount.load();
    
    std::this_thread::sleep_for(
        std::chrono::milliseconds(sampleTimeMs)
    );

    uint32_t endCount = pulseCount.load();
    uint32_t pulses   = endCount - startCount;

    // Convert to pulses per minute:
    // (pulses / pulsesPerRev) * (60000 ms per min / sampleTimeMs)
    double rpm = (pulses / PULSES_PER_REV) * (60000.0 / sampleTimeMs);
    return rpm;
}

// --- Compute moving average ---
double movingAverage(std::deque<double>& history, double newValue) {
    history.push_back(newValue);

    if (history.size() > AVG_WINDOW_SIZE)
        history.pop_front();

    double sum = std::accumulate(history.begin(), history.end(), 0.0);
    return sum / history.size();
}

// --- Main command: compute speed + optional gear ratio ---
void calcMotorSpeed(double gearRatio = 0.0) {
    static std::deque<double> rpmHistory;

    double motorRPM  = calculateMotorRPM();
    double smoothRPM = movingAverage(rpmHistory, motorRPM);

    std::cout << "Motor Speed: " << smoothRPM << " RPM (smoothed)\n";

    if (gearRatio > 0.0) {
        double diggingRPM = smoothRPM / gearRatio;
        std::cout << "Digging Speed (gear-adjusted): "
                  << diggingRPM << " RPM (ratio " << gearRatio << ")\n";
    } else {
        std::cout << "No gear ratio provided — skipping adjusted speed.\n";
    }
}

int main() {
    std::cout << "Collecting tachometer pulses...\n";

    // Simulated pulse generator (1 RPM motor example)
    std::thread pulseSim([]() {
        double targetRPM = 1.0;
        double targetPPS = (targetRPM / 60.0) * PULSES_PER_REV;
        double intervalMs = 1000.0 / targetPPS;

        while (true) {
            onTachPulse();
            std::this_thread::sleep_for(
                std::chrono::milliseconds((int)intervalMs)
            );
        }
    });

    // Sample continuously at SAMPLE_TIME_MS
    while (true) {
        calcMotorSpeed(5.0);  // gear ratio 
        std::this_thread::sleep_for(std::chrono::milliseconds(SAMPLE_TIME_MS));
    }

    pulseSim.join();
    return 0;
}
