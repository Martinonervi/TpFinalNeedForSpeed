#ifndef CONSTANT_RATE_LOOP_H
#define CONSTANT_RATE_LOOP_H

#include <chrono>
#include <thread>

class ConstantRateLoop {
public:
    explicit ConstantRateLoop(double rate_hz)
        : rate_sec(1.0 / rate_hz),
          t1(std::chrono::steady_clock::now())
    {}

    void sleep_until_next_frame() {
        using namespace std::chrono;

        auto t2 = steady_clock::now();
        duration<double> dif = t2 - t1;

        double lost = dif.count() - rate_sec;
        if (lost > rate_sec) { //nos pasamos
            t1 = steady_clock::now();
            return;
        }

        t1 += duration_cast<steady_clock::duration>(duration<double>(rate_sec));

        auto now = steady_clock::now();
        if (t1 > now) {
            std::this_thread::sleep_until(t1);
        } else {
            t1 = now;
        }
    }

private:
    double rate_sec;
    std::chrono::steady_clock::time_point t1;
};

#endif




