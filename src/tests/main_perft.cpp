#include <gtest/gtest.h>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <vector>
#include <numeric>

static std::string getGitHash()
{
    std::string hash = "unknown";
#ifdef _WIN32
    FILE *pipe = _popen("git rev-parse --short HEAD 2>nul", "r");
#else
    FILE *pipe = popen("git rev-parse --short HEAD 2>/dev/null", "r");
#endif
    if (pipe)
    {
        char buffer[64];
        if (fgets(buffer, sizeof(buffer), pipe))
            hash = std::string(buffer);
#ifdef _WIN32
        _pclose(pipe);
#else
        pclose(pipe);
#endif
    }
    hash.erase(std::remove(hash.begin(), hash.end(), '\n'), hash.end());
    return hash;
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    unsigned int threads = 1;
    if (const char *env = std::getenv("PERFT_THREADS"))
        threads = std::clamp(std::atoi(env), 1, 64);

    std::string gitHash = getGitHash();

    if (threads == 1)
    {
        std::cout << "[Perft Benchmark] Running single threaded perft 5x\n";

        std::vector<double> runTimes;
        for (int i = 1; i <= 5; ++i)
        {
            std::cout << "Run " << i << "/5..." << std::flush;
            auto start = std::clock();
            (void)RUN_ALL_TESTS();
            auto end = std::clock();
            double seconds = static_cast<double>(end - start) / CLOCKS_PER_SEC;
            runTimes.push_back(seconds);
            std::cout << " done (" << std::fixed << std::setprecision(3) << seconds << "s)\n";
        }

        double avg = std::accumulate(runTimes.begin(), runTimes.end(), 0.0) / runTimes.size();

        auto t = std::time(nullptr);
        std::ostringstream line;
        line << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S")
             << " | Total time: " << std::accumulate(runTimes.begin(), runTimes.end(), 0.0) << "s"
             << " | Average: " << avg << "s"
             << " | Commit: " << gitHash;

        std::vector<std::string> benchmarks, normal;
        {
            std::ifstream in("perft_benchmarks.txt");
            std::string l;
            while (std::getline(in, l))
            {
                if (l.find("Benchmark") != std::string::npos)
                    benchmarks.push_back(l);
                else
                    normal.push_back(l);
            }
        }

        benchmarks.push_back(line.str());

        std::ofstream out("perft_benchmarks.txt", std::ios::trunc);
        for (const auto &l : benchmarks)
            out << l << "\n";
        for (const auto &l : normal)
            out << l << "\n";

        std::cout << "\n=== Benchmark Summary ===\n";
        std::cout << "Average: " << avg << "s\n";
        std::cout << "Commit: " << gitHash << "\n";
        std::cout << "Results appended to perft_benchmarks.txt\n";
        return 0;
    }

    std::cout << "[Perft] Running with " << threads << " thread"
              << (threads > 1 ? "s" : "") << "...\n";

    auto start = std::clock();
    int result = RUN_ALL_TESTS();
    auto end = std::clock();

    double seconds = static_cast<double>(end - start) / CLOCKS_PER_SEC;

    std::cout << "Perft tests completed in " << seconds << "s with " << threads
              << " thread" << (threads > 1 ? "s" : "")
              << " (commit " << gitHash << ")\n";

    return result;
}
