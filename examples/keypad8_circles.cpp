#include "makcu/makcu.h"

#include <Windows.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <utility>

#ifndef NOMINMAX
#define NOMINMAX
#endif

namespace {

constexpr int kTriggerKey = VK_NUMPAD8;
constexpr int kExitKey = VK_ESCAPE;
constexpr int kRadius = 18;
constexpr int kStepsPerCircle = 48;
const auto kTickDelay = std::chrono::milliseconds{8};
constexpr double kPi = 3.14159265358979323846;

bool isKeyDown(int virtualKey) {
    return (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
}

std::pair<int, int> pointOnCircle(int step) {
    const double angle = (2.0 * kPi * static_cast<double>(step)) /
                         static_cast<double>(kStepsPerCircle);

    return std::make_pair(
        static_cast<int>(std::lround(std::cos(angle) * kRadius)),
        static_cast<int>(std::lround(std::sin(angle) * kRadius))
    );
}

} // namespace

int main() {
    try {
        makcu::SerialTransport::Config config;
        config.debug = false;
        config.sendInit = true;
        config.autoReconnect = true;

        makcu::SerialTransport transport(config);

        std::cout << "Connecting to MAKCU device...\n";
        transport.connect();

        if (!transport.isConnected()) {
            std::cerr << "Failed to connect to a MAKCU device.\n";
            return 1;
        }

        makcu::Mouse mouse(transport);

        std::cout << "Connected to " << transport.getPort() << " at "
                  << transport.getBaudrate() << " baud\n";
        std::cout << "Hold keypad 8 to move the mouse in circles.\n";
        std::cout << "Press Esc to quit.\n";

        bool circleActive = false;
        int step = 0;
        auto previousPoint = pointOnCircle(0);

        while (!isKeyDown(kExitKey)) {
            if (isKeyDown(kTriggerKey)) {
                if (!circleActive) {
                    circleActive = true;
                    step = 0;
                    previousPoint = pointOnCircle(step);
                }

                const int nextStep = (step + 1) % kStepsPerCircle;
                const auto nextPoint = pointOnCircle(nextStep);
                const int dx = nextPoint.first - previousPoint.first;
                const int dy = nextPoint.second - previousPoint.second;

                if (dx != 0 || dy != 0) {
                    mouse.move(dx, dy);
                }

                step = nextStep;
                previousPoint = nextPoint;
            } else {
                circleActive = false;
            }

            std::this_thread::sleep_for(kTickDelay);
        }

        transport.disconnect();
        return 0;
    } catch (const makcu::MakcuError& e) {
        std::cerr << "MAKCU error: " << e.what() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 1;
}
