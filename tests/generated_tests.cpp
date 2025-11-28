// 本文件由 tools/generate_tests.py 自动生成
#include "calculator.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace {
constexpr double kEps = 1e-7;
bool approx_equal(double lhs, double rhs) {
    return std::fabs(lhs - rhs) < kEps;
}

struct BinaryCase {
    double a;
    double b;
    double expected;
};

struct UnaryCase {
    double value;
    double expected;
};

struct IntCase {
    int value;
    long long expected;
};

struct BoolCase {
    int value;
    bool expected;
};

struct DivideCase {
    double a;
    double b;
    std::optional<double> expected;
};

struct ResultCounter {
    std::size_t total{0};
    std::size_t failed{0};

    void record(bool ok, const std::string& name, const std::string& detail) {
        ++total;
        if (!ok) {
            ++failed;
            std::cerr << "[失败] " << name << ": " << detail << std::endl;
        }
    }
};
}  // namespace

int main() {
    Calculator calc;
    ResultCounter counter;

    const auto add_cases = std::vector<BinaryCase>{
        {28.876000000000, 72.922000000000, 101.798000000000},
        {-11.197000000000, -163.358000000000, -174.555000000000},
        {-249.776000000000, 236.646000000000, -13.130000000000},
        {34.854000000000, -217.107000000000, -182.253000000000},
        {-49.564000000000, -228.324000000000, -277.888000000000},
        {133.635000000000, 241.002000000000, 374.637000000000},
        {-200.326000000000, -225.385000000000, -425.711000000000},
        {-236.065000000000, 5.110000000000, -230.955000000000},
        {139.439000000000, -152.868000000000, -13.429000000000},
        {-155.327000000000, 104.115000000000, -51.212000000000},
        {240.613000000000, 220.152000000000, 460.765000000000},
        {69.350000000000, 210.640000000000, 279.990000000000},
        {-72.765000000000, 206.089000000000, 133.324000000000},
        {47.836000000000, 23.066000000000, 70.902000000000},
        {-145.911000000000, -128.274000000000, -274.185000000000},
        {-233.807000000000, 142.228000000000, -91.579000000000},
        {26.362000000000, -66.020000000000, -39.658000000000},
        {244.031000000000, -19.691000000000, 224.340000000000},
        {-111.117000000000, 156.327000000000, 45.210000000000},
        {38.675000000000, 161.193000000000, 199.868000000000},
        {-87.944000000000, -26.671000000000, -114.615000000000},
        {149.552000000000, -21.761000000000, 127.791000000000},
        {95.182000000000, 128.510000000000, 223.692000000000},
        {31.346000000000, 77.397000000000, 108.743000000000},
        {43.690000000000, 128.956000000000, 172.646000000000}
    };
    for (const auto& tc : add_cases) {
        double result = calc.add(tc.a, tc.b);
        counter.record(approx_equal(result, tc.expected), "add",
                       std::to_string(tc.a) + "," + std::to_string(tc.b));
    }

    const auto subtract_cases = std::vector<BinaryCase>{
        {177.687000000000, 169.915000000000, 7.772000000000},
        {-77.868000000000, 21.981000000000, -99.849000000000},
        {49.831000000000, -169.539000000000, 219.370000000000},
        {-110.029000000000, -84.409000000000, -25.620000000000},
        {-69.480000000000, 28.878000000000, -98.358000000000},
        {83.296000000000, -112.159000000000, 195.455000000000},
        {-112.732000000000, 69.268000000000, -182.0},
        {-175.801000000000, -74.479000000000, -101.322000000000},
        {114.992000000000, 61.955000000000, 53.037000000000},
        {155.726000000000, 19.551000000000, 136.175000000000},
        {35.610000000000, 1.156000000000, 34.454000000000},
        {-58.163000000000, -78.326000000000, 20.163000000000},
        {-126.825000000000, 44.009000000000, -170.834000000000},
        {2.619000000000, 36.827000000000, -34.208000000000},
        {93.690000000000, -147.154000000000, 240.844000000000},
        {-147.618000000000, -178.290000000000, 30.672000000000},
        {-28.268000000000, 36.403000000000, -64.671000000000},
        {132.323000000000, -120.773000000000, 253.096000000000},
        {-244.310000000000, 41.107000000000, -285.417000000000},
        {-152.300000000000, -178.853000000000, 26.553000000000},
        {-163.834000000000, 90.332000000000, -254.166000000000},
        {115.628000000000, -120.317000000000, 235.945000000000},
        {203.847000000000, -181.758000000000, 385.605000000000},
        {64.260000000000, 120.690000000000, -56.430000000000},
        {-174.715000000000, 223.374000000000, -398.089000000000}
    };
    for (const auto& tc : subtract_cases) {
        double result = calc.subtract(tc.a, tc.b);
        counter.record(approx_equal(result, tc.expected), "subtract",
                       std::to_string(tc.a) + "," + std::to_string(tc.b));
    }

    const auto multiply_cases = std::vector<BinaryCase>{
        {-135.394000000000, 125.723000000000, -17022.139862000000},
        {146.061000000000, -211.249000000000, -30855.240189000000},
        {4.985000000000, -129.723000000000, -646.669155000000},
        {-40.595000000000, 149.780000000000, -6080.319100000000},
        {150.511000000000, 87.681000000000, 13196.954990999999},
        {44.192000000000, -135.895000000000, -6005.471840000000},
        {193.116000000000, -73.497000000000, -14193.446652000001},
        {107.096000000000, 8.229000000000, 881.292984000000},
        {-65.312000000000, -12.238000000000, 799.288256000000},
        {-120.066000000000, -216.673000000000, 26015.060418000001},
        {215.703000000000, -96.217000000000, -20754.295550999999},
        {3.604000000000, 58.496000000000, 210.819584000000},
        {60.110000000000, -41.350000000000, -2485.548500000000},
        {-84.500000000000, 139.478000000000, -11785.891000000001},
        {230.344000000000, 231.709000000000, 53372.777896000000},
        {202.910000000000, -172.167000000000, -34934.405970000000},
        {180.888000000000, -82.930000000000, -15001.041840000002},
        {119.421000000000, -2.303000000000, -275.026563000000},
        {-158.586000000000, 6.959000000000, -1103.599974000000},
        {-213.571000000000, -158.305000000000, 33809.357154999998},
        {-74.728000000000, 146.547000000000, -10951.164215999999},
        {-220.016000000000, 162.990000000000, -35860.407840000000},
        {214.791000000000, -189.251000000000, -40649.411541000001},
        {93.387000000000, 47.850000000000, 4468.567950000001},
        {-81.807000000000, -101.365000000000, 8292.366555000001}
    };
    for (const auto& tc : multiply_cases) {
        double result = calc.multiply(tc.a, tc.b);
        counter.record(approx_equal(result, tc.expected), "multiply",
                       std::to_string(tc.a) + "," + std::to_string(tc.b));
    }

    const std::vector<DivideCase> divide_cases = std::vector<DivideCase>{
        {89.203000000000, 53.679000000000, 1.661785800779},
        {-153.649000000000, -222.823000000000, 0.689556284585},
        {-50.485000000000, 42.340000000000, -1.192371280113},
        {107.500000000000, -108.044000000000, -0.994965014253},
        {-117.040000000000, -91.043000000000, 1.285546390167},
        {200.889000000000, -20.528000000000, -9.786097038192},
        {201.106000000000, 247.799000000000, 0.811569053951},
        {-222.747000000000, -238.284000000000, 0.934796293499},
        {15.727000000000, 160.431000000000, 0.098029682543},
        {105.234000000000, -17.011000000000, -6.186232437834},
        {-175.369000000000, 192.124000000000, -0.912790697674},
        {64.890000000000, 164.415000000000, 0.394672018976},
        {162.742000000000, -50.102000000000, -3.248213644166},
        {-75.101000000000, -225.518000000000, 0.333015546431},
        {-205.622000000000, 172.388000000000, -1.192786040792},
        {-106.059000000000, 155.570000000000, -0.681744552292},
        {206.332000000000, 216.223000000000, 0.954255560232},
        {-36.326000000000, 21.817000000000, -1.665031855892},
        {-10.0, 0.0, std::nullopt},
        {-1.0, 0.0, std::nullopt},
        {0.0, 0.0, std::nullopt},
        {25.0, 0.0, std::nullopt},
        {99.0, 0.0, std::nullopt}
    };
    for (const auto& tc : divide_cases) {
        bool raised = false;
        std::optional<double> value;
        try {
            value = calc.divide(tc.a, tc.b);
        } catch (const std::runtime_error&) {
            raised = true;
        }
        if (tc.expected.has_value()) {
            bool ok = (!raised) && approx_equal(*value, *tc.expected);
            counter.record(ok, "divide", std::to_string(tc.a) + "/" + std::to_string(tc.b));
        } else {
            counter.record(raised, "divide_exception", std::to_string(tc.a));
        }
    }

    const auto square_cases = std::vector<UnaryCase>{
        {197.847000000000, 39143.435409000005},
        {-123.301000000000, 15203.136601000000},
        {-81.685000000000, 6672.439225000000},
        {-66.707000000000, 4449.823848999999},
        {129.341000000000, 16729.094281000002},
        {-236.263000000000, 55820.205169000001},
        {171.310000000000, 29347.116099999999},
        {22.430000000000, 503.104900000000},
        {-20.201000000000, 408.080401000000},
        {-96.352000000000, 9283.707904000001},
        {-77.713000000000, 6039.310368999999},
        {-120.256000000000, 14461.505536000001},
        {87.128000000000, 7591.288384000000},
        {-249.421000000000, 62210.835240999993},
        {62.149000000000, 3862.498201000000},
        {-91.704000000000, 8409.623615999999},
        {74.733000000000, 5585.021289000000},
        {-227.188000000000, 51614.387343999995},
        {84.067000000000, 7067.260488999998},
        {233.980000000000, 54746.640399999997}
    };
    for (const auto& tc : square_cases) {
        double result = calc.square(tc.value);
        counter.record(approx_equal(result, tc.expected), "square",
                       std::to_string(tc.value));
    }

    const auto factorial_cases = std::vector<IntCase>{
        {4, 24},
        {4, 24},
        {10, 3628800},
        {6, 720},
        {0, 1},
        {2, 2},
        {11, 39916800},
        {5, 120},
        {8, 40320},
        {5, 120},
        {9, 362880},
        {5, 120}
    };
    for (const auto& tc : factorial_cases) {
        long long result = calc.factorial(tc.value);
        counter.record(result == tc.expected, "factorial", std::to_string(tc.value));
    }

    const auto prime_cases = std::vector<BoolCase>{
        {115, false},
        {64, false},
        {90, false},
        {-9, false},
        {3, true},
        {66, false},
        {24, false},
        {-5, false},
        {42, false},
        {80, false},
        {27, false},
        {58, false},
        {63, false},
        {-7, false},
        {22, false},
        {8, false},
        {25, false},
        {-8, false},
        {25, false},
        {44, false}
    };
    for (const auto& tc : prime_cases) {
        bool result = calc.isPrime(tc.value);
        counter.record(result == tc.expected, "isPrime", std::to_string(tc.value));
    }

    const auto gcd_cases = std::vector<BinaryCase>{
        {162.0, -363.0, 3.0},
        {366.0, -361.0, 1.0},
        {-344.0, 127.0, 1.0},
        {334.0, -260.0, 2.0},
        {-229.0, -472.0, 1.0},
        {174.0, 10.0, 2.0},
        {406.0, -13.0, 1.0},
        {-19.0, 495.0, 1.0},
        {-311.0, -294.0, 1.0},
        {-258.0, 126.0, 6.0},
        {309.0, -389.0, 1.0},
        {-76.0, 244.0, 4.0},
        {434.0, -213.0, 1.0},
        {-350.0, -379.0, 1.0},
        {201.0, -216.0, 3.0},
        {342.0, -223.0, 1.0},
        {-256.0, -442.0, 2.0},
        {-72.0, 336.0, 24.0},
        {43.0, 67.0, 1.0},
        {-364.0, -116.0, 4.0}
    };
    for (const auto& tc : gcd_cases) {
        int result = calc.gcd(static_cast<int>(tc.a), static_cast<int>(tc.b));
        counter.record(result == static_cast<int>(tc.expected), "gcd",
                       std::to_string(tc.a) + "," + std::to_string(tc.b));
    }

    const auto lcm_cases = std::vector<BinaryCase>{
        {162.0, -363.0, -19602.0},
        {366.0, -361.0, -132126.0},
        {-344.0, 127.0, -43688.0},
        {334.0, -260.0, -43420.0},
        {-229.0, -472.0, 108088.0},
        {174.0, 10.0, 870.0},
        {406.0, -13.0, -5278.0},
        {-19.0, 495.0, -9405.0},
        {-311.0, -294.0, 91434.0},
        {-258.0, 126.0, -5418.0},
        {309.0, -389.0, -120201.0},
        {-76.0, 244.0, -4636.0},
        {434.0, -213.0, -92442.0},
        {-350.0, -379.0, 132650.0},
        {201.0, -216.0, -14472.0},
        {342.0, -223.0, -76266.0},
        {-256.0, -442.0, 56576.0},
        {-72.0, 336.0, -1008.0},
        {43.0, 67.0, 2881.0},
        {-364.0, -116.0, 10556.0}
    };
    for (const auto& tc : lcm_cases) {
        int result = calc.lcm(static_cast<int>(tc.a), static_cast<int>(tc.b));
        counter.record(result == static_cast<int>(tc.expected), "lcm",
                       std::to_string(tc.a) + "," + std::to_string(tc.b));
    }

    // 并行阶乘 / 质数 / 平方 / 加法
    const std::vector<std::vector<int>> factorial_vectors = {
        std::vector<int>{11, 11, 12, 4, 4},
        std::vector<int>{2, 6, 8, 2, 4, 8}
    };
    for (const auto& vec : factorial_vectors) {
        auto result = calc.parallelFactorial(vec);
        bool ok = true;
        for (std::size_t i = 0; i < vec.size(); ++i) {
            long long expected = calc.factorial(vec[i]);
            if (result[i] != expected) {
                ok = false;
            }
        }
        counter.record(ok, "parallelFactorial", std::to_string(vec.size()));
    }

    const std::vector<std::vector<int>> prime_vectors = {
        std::vector<int>{2, 3, 4, 5, 99, 101},
        std::vector<int>{17, 18, 19, 23, 25, 29}
    };
    for (const auto& vec : prime_vectors) {
        auto result = calc.parallelIsPrime(vec);
        bool ok = true;
        for (std::size_t i = 0; i < vec.size(); ++i) {
            if (result[i] != calc.isPrime(vec[i])) {
                ok = false;
            }
        }
        counter.record(ok, "parallelIsPrime", std::to_string(vec.size()));
    }

    const std::vector<std::vector<double>> square_vectors{
        std::vector<double>{1.5, -2.0, 3.5, 10.0},
        std::vector<double>{0.0, 2.2, -3.3, 4.4}
    };
    for (const auto& vec : square_vectors) {
        auto result = calc.parallelSquare(vec);
        bool ok = true;
        for (std::size_t i = 0; i < vec.size(); ++i) {
            if (!approx_equal(result[i], calc.square(vec[i]))) {
                ok = false;
            }
        }
        counter.record(ok, "parallelSquare", std::to_string(vec.size()));
    }

    const std::vector<std::vector<std::pair<double, double>>> add_batches{
        std::vector<std::pair<double, double>>{{1.0, 2.0}, {-5.5, 10.1}, {99.9, -12.2}},
        std::vector<std::pair<double, double>>{{0.0, 0.0}, {42.0, -17.5}, {8.3, 9.7}}
    };
    for (const auto& vec : add_batches) {
        auto result = calc.parallelAdd(vec);
        bool ok = true;
        for (std::size_t i = 0; i < vec.size(); ++i) {
            if (!approx_equal(result[i], calc.add(vec[i].first, vec[i].second))) {
                ok = false;
            }
        }
        counter.record(ok, "parallelAdd", std::to_string(vec.size()));
    }

    const std::vector<int> batch_values{5, 7, 9, 11, 13, 15};
    calc.parallelBatchCompute(batch_values, 3);
    counter.record(true, "parallelBatchCompute", std::to_string(batch_values.size()));

    calc.resetCallCount();
    const int threads = 4;
    const int calls_per_thread = 200;
    std::vector<std::thread> workers;
    for (int i = 0; i < threads; ++i) {
        workers.emplace_back([&calc]() {
            for (int j = 0; j < calls_per_thread; ++j) {
                calc.add(1.0, 2.0);
            }
        });
    }
    for (auto& t : workers) {
        t.join();
    }
    counter.record(calc.getCallCount() == threads * calls_per_thread,
                   "threadSafeCounter", std::to_string(calc.getCallCount()));

    std::cout << "TOTAL_TESTS " << counter.total << std::endl;
    std::cout << "FAILED_TESTS " << counter.failed << std::endl;
    return counter.failed == 0 ? 0 : 1;
}
