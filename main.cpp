#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include "calculator.h"

int main() {
    Calculator calc;
    
    std::cout << "=== C++ 多线程计算器程序 ===" << std::endl;
    std::cout << "硬件并发线程数: " << std::thread::hardware_concurrency() << std::endl;
    std::cout << std::endl;
    
    // ========== 单线程测试 ==========
    std::cout << "--- 单线程基本运算测试 ---" << std::endl;
    std::cout << "10 + 5 = " << calc.add(10, 5) << std::endl;
    std::cout << "10 - 5 = " << calc.subtract(10, 5) << std::endl;
    std::cout << "10 * 5 = " << calc.multiply(10, 5) << std::endl;
    std::cout << "10 / 5 = " << calc.divide(10, 5) << std::endl;
    std::cout << "5 的平方 = " << calc.square(5) << std::endl;
    std::cout << "5! = " << calc.factorial(5) << std::endl;
    std::cout << "17 是质数吗? " << (calc.isPrime(17) ? "是" : "否") << std::endl;
    std::cout << "gcd(48, 18) = " << calc.gcd(48, 18) << std::endl;
    std::cout << std::endl;
    
    // ========== 多线程并行计算测试 ==========
    std::cout << "--- 多线程并行计算测试 ---" << std::endl;
    
    // 1. 并行计算多个数的阶乘
    std::cout << "\n1. 并行计算阶乘:" << std::endl;
    std::vector<int> factNumbers = {5, 10, 15, 20, 7, 12};
    // 开始时间
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<long long> factResults = calc.parallelFactorial(factNumbers, 4);
    // 结束时间
    auto end = std::chrono::high_resolution_clock::now();
    // 计算时间
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    for (size_t i = 0; i < factNumbers.size(); ++i) {
        std::cout << "  " << factNumbers[i] << "! = " << factResults[i] << std::endl;
    }
    std::cout << "  并行计算耗时: " << duration.count() << " 微秒" << std::endl;
    
    // 2. 并行判断多个数是否为质数
    std::cout << "\n2. 并行判断质数:" << std::endl;
    std::vector<int> primeNumbers = {17, 20, 23, 29, 31, 100, 101, 97};
    // 开始时间
    start = std::chrono::high_resolution_clock::now();
    // 并行判断多个数是否为质数
    std::vector<bool> primeResults = calc.parallelIsPrime(primeNumbers, 4);
    // 结束时间
    end = std::chrono::high_resolution_clock::now();
    // 计算时间
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    for (size_t i = 0; i < primeNumbers.size(); ++i) {
        std::cout << "  " << primeNumbers[i] << " 是质数? " 
                  << (primeResults[i] ? "是" : "否") << std::endl;
    }
    std::cout << "  并行计算耗时: " << duration.count() << " 微秒" << std::endl;
    
    // 3. 并行计算多个数的平方
    std::cout << "\n3. 并行计算平方:" << std::endl;
    std::vector<double> squareNumbers = {2.5, 5.0, 7.5, 10.0, 12.5, 15.0};
    // 开始时间
    start = std::chrono::high_resolution_clock::now();
    // 并行计算多个数的平方
    std::vector<double> squareResults = calc.parallelSquare(squareNumbers, 4);
    // 结束时间
    end = std::chrono::high_resolution_clock::now();
    // 计算时间
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    for (size_t i = 0; i < squareNumbers.size(); ++i) {
        std::cout << "  " << squareNumbers[i] << "^2 = " << squareResults[i] << std::endl;
    }
    std::cout << "  并行计算耗时: " << duration.count() << " 微秒" << std::endl;
    
    // 4. 并行执行多个加法运算
    std::cout << "\n4. 并行执行加法:" << std::endl;
    std::vector<std::pair<double, double>> addPairs = {
        {10, 5}, {20, 15}, {30, 25}, {40, 35}, {50, 45}
    };
    // 开始时间
    start = std::chrono::high_resolution_clock::now();
    // 并行执行多个加法运算
    std::vector<double> addResults = calc.parallelAdd(addPairs, 4);
    // 结束时间
    end = std::chrono::high_resolution_clock::now();
    // 计算时间
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    for (size_t i = 0; i < addPairs.size(); ++i) {
        std::cout << "  " << addPairs[i].first << " + " << addPairs[i].second 
                  << " = " << addResults[i] << std::endl;
    }
    std::cout << "  并行计算耗时: " << duration.count() << " 微秒" << std::endl;
    
    // 5. 批量并行计算（混合任务）
    std::cout << "\n5. 批量并行计算（混合任务）:" << std::endl;
    std::vector<int> batchNumbers = {5, 7, 10, 13, 15, 17, 20, 23};
    calc.parallelBatchCompute(batchNumbers, 4);
    
    // 6. 显示函数调用统计
    std::cout << "\n--- 统计信息 ---" << std::endl;
    std::cout << "函数总调用次数: " << calc.getCallCount() << std::endl;
    
    return 0;
}

