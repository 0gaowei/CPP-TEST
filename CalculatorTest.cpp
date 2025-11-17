// C++Test 测试用例文件
// 此文件包含针对Calculator类的单元测试用例（包括多线程测试）

#include "calculator.h"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <thread>
#include <atomic>
#include <cmath>

// 测试用例1: 测试加法功能
void testAdd() {
    Calculator calc;
    assert(calc.add(2, 3) == 5);
    assert(calc.add(-5, 3) == -2);
    assert(calc.add(0, 0) == 0);
    assert(calc.add(1.5, 2.5) == 4.0);
    std::cout << "✓ 加法测试通过" << std::endl;
}

// 测试用例2: 测试减法功能
void testSubtract() {
    Calculator calc;
    assert(calc.subtract(5, 3) == 2);
    assert(calc.subtract(3, 5) == -2);
    assert(calc.subtract(0, 0) == 0);
    assert(calc.subtract(10.5, 3.2) == 7.3);
    std::cout << "✓ 减法测试通过" << std::endl;
}

// 测试用例3: 测试乘法功能
void testMultiply() {
    Calculator calc;
    assert(calc.multiply(2, 3) == 6);
    assert(calc.multiply(-2, 3) == -6);
    assert(calc.multiply(0, 5) == 0);
    assert(calc.multiply(2.5, 4) == 10.0);
    std::cout << "✓ 乘法测试通过" << std::endl;
}

// 测试用例4: 测试除法功能
void testDivide() {
    Calculator calc;
    assert(calc.divide(10, 2) == 5);
    assert(calc.divide(9, 3) == 3);
    assert(calc.divide(7, 2) == 3.5);
    
    // 测试除零异常
    try {
        calc.divide(10, 0);
        assert(false && "应该抛出异常");
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "除数不能为零");
    }
    std::cout << "✓ 除法测试通过" << std::endl;
}

// 测试用例5: 测试平方功能
void testSquare() {
    Calculator calc;
    assert(calc.square(5) == 25);
    assert(calc.square(-5) == 25);
    assert(calc.square(0) == 0);
    assert(calc.square(2.5) == 6.25);
    std::cout << "✓ 平方测试通过" << std::endl;
}

// 测试用例6: 测试阶乘功能
void testFactorial() {
    Calculator calc;
    assert(calc.factorial(0) == 1);
    assert(calc.factorial(1) == 1);
    assert(calc.factorial(5) == 120);
    assert(calc.factorial(10) == 3628800);
    
    // 测试负数异常
    try {
        calc.factorial(-1);
        assert(false && "应该抛出异常");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()) == "阶乘参数不能为负数");
    }
    std::cout << "✓ 阶乘测试通过" << std::endl;
}

// 测试用例7: 测试质数判断功能
void testIsPrime() {
    Calculator calc;
    assert(calc.isPrime(2) == true);
    assert(calc.isPrime(3) == true);
    assert(calc.isPrime(17) == true);
    assert(calc.isPrime(19) == true);
    assert(calc.isPrime(4) == false);
    assert(calc.isPrime(20) == false);
    assert(calc.isPrime(1) == false);
    assert(calc.isPrime(0) == false);
    assert(calc.isPrime(-5) == false);
    std::cout << "✓ 质数判断测试通过" << std::endl;
}

// 测试用例8: 测试最大公约数功能
void testGcd() {
    Calculator calc;
    assert(calc.gcd(48, 18) == 6);
    assert(calc.gcd(17, 13) == 1);
    assert(calc.gcd(100, 25) == 25);
    assert(calc.gcd(0, 5) == 5);
    assert(calc.gcd(5, 0) == 5);
    assert(calc.gcd(-48, 18) == 6);
    std::cout << "✓ 最大公约数测试通过" << std::endl;
}

// 测试用例9: 测试最小公倍数功能
void testLcm() {
    Calculator calc;
    assert(calc.lcm(48, 18) == 144);
    assert(calc.lcm(17, 13) == 221);
    assert(calc.lcm(100, 25) == 100);
    assert(calc.lcm(0, 5) == 0);
    assert(calc.lcm(5, 0) == 0);
    std::cout << "✓ 最小公倍数测试通过" << std::endl;
}

// ========== 多线程测试用例 ==========

// 测试用例10: 测试并行计算阶乘
void testParallelFactorial() {
    Calculator calc;
    std::vector<int> numbers = {5, 10, 7, 12};
    std::vector<long long> expected = {120, 3628800, 5040, 479001600};
    
    std::vector<long long> results = calc.parallelFactorial(numbers, 4);
    
    assert(results.size() == expected.size());
    for (size_t i = 0; i < results.size(); ++i) {
        assert(results[i] == expected[i]);
    }
    std::cout << "✓ 并行阶乘测试通过" << std::endl;
}

// 测试用例11: 测试并行判断质数
void testParallelIsPrime() {
    Calculator calc;
    std::vector<int> numbers = {2, 4, 17, 20, 23, 100};
    std::vector<bool> expected = {true, false, true, false, true, false};
    
    std::vector<bool> results = calc.parallelIsPrime(numbers, 4);
    
    assert(results.size() == expected.size());
    for (size_t i = 0; i < results.size(); ++i) {
        assert(results[i] == expected[i]);
    }
    std::cout << "✓ 并行质数判断测试通过" << std::endl;
}

// 测试用例12: 测试并行计算平方
void testParallelSquare() {
    Calculator calc;
    std::vector<double> numbers = {2.0, 5.0, 10.0, -3.0};
    std::vector<double> expected = {4.0, 25.0, 100.0, 9.0};
    
    std::vector<double> results = calc.parallelSquare(numbers, 4);
    
    assert(results.size() == expected.size());
    for (size_t i = 0; i < results.size(); ++i) {
        assert(std::abs(results[i] - expected[i]) < 0.0001);
    }
    std::cout << "✓ 并行平方测试通过" << std::endl;
}

// 测试用例13: 测试并行加法
void testParallelAdd() {
    Calculator calc;
    std::vector<std::pair<double, double>> pairs = {
        {2, 3}, {10, 5}, {-5, 3}, {0, 0}
    };
    std::vector<double> expected = {5, 15, -2, 0};
    
    std::vector<double> results = calc.parallelAdd(pairs, 4);
    
    assert(results.size() == expected.size());
    for (size_t i = 0; i < results.size(); ++i) {
        assert(std::abs(results[i] - expected[i]) < 0.0001);
    }
    std::cout << "✓ 并行加法测试通过" << std::endl;
}

// 测试用例14: 测试线程安全的计数器
void testThreadSafeCounter() {
    Calculator calc;
    calc.resetCallCount();
    
    // 创建多个线程同时调用add函数
    const int threadCount = 10;
    const int callsPerThread = 100;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&calc, callsPerThread]() {
            for (int j = 0; j < callsPerThread; ++j) {
                calc.add(1, 1);
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }
    
    // 验证计数器值
    long long expectedCount = threadCount * callsPerThread;
    assert(calc.getCallCount() == expectedCount);
    std::cout << "✓ 线程安全计数器测试通过 (调用次数: " << calc.getCallCount() << ")" << std::endl;
}

// 测试用例15: 测试并发访问（压力测试）
void testConcurrentAccess() {
    Calculator calc;
    const int threadCount = 8;
    const int operationsPerThread = 50;
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    // 创建多个线程执行不同的操作
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&calc, &successCount, operationsPerThread, i]() {
            for (int j = 0; j < operationsPerThread; ++j) {
                try {
                    switch (i % 4) {
                        case 0:
                            calc.add(j, i);
                            break;
                        case 1:
                            calc.factorial(j % 10);
                            break;
                        case 2:
                            calc.isPrime(j + 2);
                            break;
                        case 3:
                            calc.square(static_cast<double>(j));
                            break;
                    }
                    successCount++;
                } catch (...) {
                    // 忽略异常（如负数阶乘）
                }
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }
    
    // 验证至少有一些操作成功
    assert(successCount > 0);
    std::cout << "✓ 并发访问压力测试通过 (成功操作: " << successCount << ")" << std::endl;
}

// 主测试函数
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "开始执行 C++Test 测试用例" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        // 单线程测试
        std::cout << "\n--- 单线程功能测试 ---" << std::endl;
        testAdd();
        testSubtract();
        testMultiply();
        testDivide();
        testSquare();
        testFactorial();
        testIsPrime();
        testGcd();
        testLcm();
        
        // 多线程测试
        std::cout << "\n--- 多线程功能测试 ---" << std::endl;
        testParallelFactorial();
        testParallelIsPrime();
        testParallelSquare();
        testParallelAdd();
        testThreadSafeCounter();
        testConcurrentAccess();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "所有测试用例执行成功！" << std::endl;
        std::cout << "========================================" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}

