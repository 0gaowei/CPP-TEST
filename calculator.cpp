#include "calculator.h"
#include <stdexcept>    // 异常处理
#include <cmath>        // 数学函数
#include <algorithm>    // 算法函数
#include <chrono>       // 时间函数
#include <iostream>     // 输入输出流

// 加法
double Calculator::add(double a, double b) {
    callCount++;  // 原子操作，线程安全
    return a + b;
}

// 减法
double Calculator::subtract(double a, double b) {
    return a - b;
}

// 乘法
double Calculator::multiply(double a, double b) {
    return a * b;
}

// 除法
double Calculator::divide(double a, double b) {
    if (b == 0) {
        throw std::runtime_error("除数不能为零");
    }
    return a / b;
}

// 计算平方
double Calculator::square(double x) {
    return x * x;
}

// 计算阶乘
long long Calculator::factorial(int n) {
    if (n < 0) {
        throw std::invalid_argument("阶乘参数不能为负数");
    }
    if (n == 0 || n == 1) {
        return 1;
    }
    long long result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

// 判断是否为质数
bool Calculator::isPrime(int n) {
    if (n < 2) {
        return false;
    }
    if (n == 2) {
        return true;
    }
    if (n % 2 == 0) {
        return false;
    }
    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

// 计算最大公约数（欧几里得算法）
int Calculator::gcd(int a, int b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

// 计算最小公倍数
int Calculator::lcm(int a, int b) {
    if (a == 0 || b == 0) {
        return 0;
    }
    return (a * b) / gcd(a, b);
}

// ========== 多线程功能实现 ==========

// 并行计算多个数的阶乘
std::vector<long long> Calculator::parallelFactorial(const std::vector<int>& numbers, int /* threadCount */) {
    std::vector<long long> results(numbers.size());
    std::vector<std::future<long long>> futures;
    
    // 使用线程池方式计算
    auto computeTask = [this](int n) -> long long {
        return this->factorial(n);
    };
    
    // 为每个数字创建异步任务
    for (size_t i = 0; i < numbers.size(); ++i) {
        futures.push_back(std::async(std::launch::async, computeTask, numbers[i]));
    }
    
    // 收集结果
    for (size_t i = 0; i < futures.size(); ++i) {
        results[i] = futures[i].get();
    }
    
    return results;
}

// 并行判断多个数是否为质数
std::vector<bool> Calculator::parallelIsPrime(const std::vector<int>& numbers, int /* threadCount */) {
    std::vector<bool> results(numbers.size());
    std::vector<std::future<bool>> futures;
    
    auto computeTask = [this](int n) -> bool {
        return this->isPrime(n);
    };
    
    for (size_t i = 0; i < numbers.size(); ++i) {
        futures.push_back(std::async(std::launch::async, computeTask, numbers[i]));
    }
    
    for (size_t i = 0; i < futures.size(); ++i) {
        results[i] = futures[i].get();
    }
    
    return results;
}

// 并行计算多个数的平方
std::vector<double> Calculator::parallelSquare(const std::vector<double>& numbers, int /* threadCount */) {
    std::vector<double> results(numbers.size());
    std::vector<std::future<double>> futures;
    
    auto computeTask = [this](double x) -> double {
        return this->square(x);
    };
    
    for (size_t i = 0; i < numbers.size(); ++i) {
        futures.push_back(std::async(std::launch::async, computeTask, numbers[i]));
    }
    
    for (size_t i = 0; i < futures.size(); ++i) {
        results[i] = futures[i].get();
    }
    
    return results;
}

// 并行执行多个加法运算
std::vector<double> Calculator::parallelAdd(const std::vector<std::pair<double, double>>& pairs, int /* threadCount */) {
    std::vector<double> results(pairs.size());
    std::vector<std::future<double>> futures;
    
    auto computeTask = [this](double a, double b) -> double {
        return this->add(a, b);
    };
    
    for (size_t i = 0; i < pairs.size(); ++i) {
        futures.push_back(std::async(std::launch::async, computeTask, pairs[i].first, pairs[i].second));
    }
    
    for (size_t i = 0; i < futures.size(); ++i) {
        results[i] = futures[i].get();
    }
    
    return results;
}

// 批量并行计算（混合任务）
void Calculator::parallelBatchCompute(const std::vector<int>& numbers, int threadCount) {
    std::vector<std::thread> threads;
    std::mutex printMtx;  // 用于保护输出
    
    auto worker = [this, &numbers, &printMtx](int start, int end) {
        for (int i = start; i < end && i < static_cast<int>(numbers.size()); ++i) {
            int n = numbers[i];
            
            // 并行执行多种计算
            long long fact = factorial(n);
            bool prime = isPrime(n);
            double sq = square(static_cast<double>(n));
            
            // 线程安全地输出结果
            {
                std::lock_guard<std::mutex> lock(printMtx);
                std::cout << "线程 " << std::this_thread::get_id() 
                          << ": 数字 " << n 
                          << " - 阶乘=" << fact 
                          << ", 质数=" << (prime ? "是" : "否")
                          << ", 平方=" << sq << std::endl;
            }
        }
    };
    
    // 创建多个线程
    int chunkSize = numbers.size() / threadCount;
    if (chunkSize == 0) chunkSize = 1;
    
    for (int i = 0; i < threadCount; ++i) {
        int start = i * chunkSize;
        int end = (i == threadCount - 1) ? numbers.size() : (i + 1) * chunkSize;
        threads.emplace_back(worker, start, end);
    }
    
    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }
}

// 获取函数调用计数（线程安全）
long long Calculator::getCallCount() const {
    return callCount.load();
}

// 重置计数器
void Calculator::resetCallCount() {
    callCount.store(0);
}

