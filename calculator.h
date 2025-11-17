#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <vector>
#include <future>
#include <thread>
#include <mutex>
#include <atomic>

class Calculator {
private:
    std::mutex mtx;  // 用于线程安全的互斥锁
    std::atomic<long long> callCount{0};  // 原子计数器，记录函数调用次数

public:
    // 加法
    double add(double a, double b);
    
    // 减法
    double subtract(double a, double b);
    
    // 乘法
    double multiply(double a, double b);
    
    // 除法
    double divide(double a, double b);
    
    // 计算平方
    double square(double x);
    
    // 计算阶乘
    long long factorial(int n);
    
    // 判断是否为质数
    bool isPrime(int n);
    
    // 计算最大公约数
    int gcd(int a, int b);
    
    // 计算最小公倍数
    int lcm(int a, int b);
    
    // ========== 多线程功能 ==========
    
    // 并行计算多个数的阶乘
    std::vector<long long> parallelFactorial(const std::vector<int>& numbers, int threadCount = 4);
    
    // 并行判断多个数是否为质数
    std::vector<bool> parallelIsPrime(const std::vector<int>& numbers, int threadCount = 4);
    
    // 并行计算多个数的平方
    std::vector<double> parallelSquare(const std::vector<double>& numbers, int threadCount = 4);
    
    // 并行执行多个加法运算
    std::vector<double> parallelAdd(const std::vector<std::pair<double, double>>& pairs, int threadCount = 4);
    
    // 批量并行计算（混合任务）
    void parallelBatchCompute(const std::vector<int>& numbers, int threadCount = 4);
    
    // 获取函数调用计数（线程安全）
    long long getCallCount() const;
    
    // 重置计数器
    void resetCallCount();
};

#endif // CALCULATOR_H

