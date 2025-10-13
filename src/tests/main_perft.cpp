#include <gtest/gtest.h>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <cstdlib>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    auto start = std::chrono::steady_clock::now();
    int result = RUN_ALL_TESTS();
    auto end = std::chrono::steady_clock::now();

    double seconds =
        std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();

    auto t = std::time(nullptr);

    std::string gitHash = "unknown";
    {
        FILE *pipe = popen("git rev-parse --short HEAD 2>nul", "r");
        if (pipe)
        {
            char buffer[64];
            if (fgets(buffer, sizeof(buffer), pipe))
                gitHash = std::string(buffer);
            pclose(pipe);
        }
        gitHash.erase(std::remove(gitHash.begin(), gitHash.end(), '\n'), gitHash.end());
    }

    std::ofstream log("perft_benchmarks.txt", std::ios::app);
    log << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S")
        << " | Perft total time: " << std::fixed << std::setprecision(3) << seconds << "s"
        << " | Commit: " << gitHash << "\n";
    log.close();

    std::cout << "Perft tests completed in " << seconds << "s (commit " << gitHash << ")\n";
    return result;
}
