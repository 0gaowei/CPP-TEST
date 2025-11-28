# C++程序测试大作业

## 项目说明

本项目是一个**多线程C++计算器程序**，包含基本的数学运算功能和并行计算能力，并配套自研的 Simple Quality Suite（Python 脚本）完成自动化测试、覆盖率与静态分析。

## 项目结构

```
CPP-TEST/
├── calculator.h          # 计算器类头文件
├── calculator.cpp        # 计算器类实现文件
├── main.cpp              # 主程序入口
├── CalculatorTest.cpp    # 手写回归测试
├── Makefile              # 编译配置文件
├── tools/                # 自研质量工具与依赖脚本
└── README.md             # 本说明文档
```

## 程序功能

Calculator类提供以下功能：

### 单线程功能
1. **基本运算**：加法、减法、乘法、除法
2. **数学函数**：平方、阶乘
3. **数论函数**：质数判断、最大公约数、最小公倍数

### 多线程并行计算功能
1. **并行计算阶乘**：`parallelFactorial()` - 同时计算多个数的阶乘
2. **并行判断质数**：`parallelIsPrime()` - 同时判断多个数是否为质数
3. **并行计算平方**：`parallelSquare()` - 同时计算多个数的平方
4. **并行执行加法**：`parallelAdd()` - 同时执行多个加法运算
5. **批量并行计算**：`parallelBatchCompute()` - 混合任务的并行计算
6. **线程安全计数器**：使用原子操作记录函数调用次数

### 线程安全特性
- 使用 `std::atomic` 实现线程安全的计数器
- 使用 `std::mutex` 保护共享资源
- 使用 `std::async` 和 `std::future` 实现异步并行计算
- 支持多线程并发访问

## 编译和运行

### 编译主程序

```bash
make
# 或
make calculator
```

### 运行主程序

```bash
make run
# 或
./calculator
```

### 编译测试程序

```bash
make test
# 或
make calculator_test
```

### 运行测试程序

```bash
make runtest
# 或
./calculator_test
```

### 清理编译文件

```bash
make clean
```

## 自研质量工具（Simple Quality Suite）

一条命令即可完成**测试生成 → 测试执行 → 覆盖率统计 → 静态分析 → 报告导出**：

```bash
# 生成测试、执行并输出报告
python3 tools/simple_quality_suite.py

# 可选参数
#   --skip-generate  跳过重新生成测试
#   --skip-tests     只做覆盖率/静态分析
#   --skip-coverage  仅运行测试与静态分析
#   --skip-static    仅运行测试与覆盖率
```

执行后常见输出：

- `tests/generated_tests.cpp/json`：随机+手工混合的 199 个断言及元数据；
- `reports/tests/results.json`：测试总数/失败数/失败样例；
- `reports/coverage/summary.json` + `.gcov`：语句/分支/函数覆盖率（默认 98%+/100%）；
- `reports/static/static_analysis.json`：静态分析结果（cppcheck 或启发式）；
- `reports/quality_summary.html|xml`：适合上交或存档的汇总报告。

脚本默认使用 `g++/gcov`，可通过 `--compiler=clang++` 指定其他编译器。所有输出集中在 `reports/` 目录，方便纳入 CI。

## 手写测试文件

- `CalculatorTest.cpp`：保留一组人工维护的回归测试，可与自研脚本生成的测试共存。
- `tests/generated_tests.cpp`：运行脚本自动覆盖串行/并行 API。若需重新生成只需删除该文件或使用默认命令。

目标覆盖率建议：语句 100%、分支 ≥95%、函数 100%。

## 注意事项

1. 运行脚本前确保 `python3`, `g++`, `gcov` 可用，`cppcheck` 可选。
2. 如需重新生成测试请删除 `tests/generated_tests.*` 或运行默认命令。
3. 测试前建议 `make clean` 清理旧的 `.o/.gcda`，避免残留影响覆盖率。
4. 所有自动生成的报告都在 `reports/`，必要时复制到其他目录再提交。

## 测试结果分析

测试完成后，应该关注：

1. **测试通过率**：所有测试用例是否通过
2. **代码覆盖率**：哪些代码被测试覆盖，哪些未覆盖
3. **边界条件**：边界值和异常情况是否得到充分测试
4. **性能测试**：可以添加性能测试用例（可选）

## 扩展建议

如果需要更完善的测试，可以考虑：

1. 添加性能/压力测试，用 `simple_quality_suite.py --skip-static` 自行组合流程。
2. 将脚本接入 CI（GitHub Actions、Jenkins 等）实现自动化守护。
3. 使用 Google Test 或 Catch2 替换/补充 `CalculatorTest.cpp`，脚本会自动编译执行。
4. 将静态分析工具扩展到 `clang-tidy`、`include-what-you-use` 等（脚本接口已留出 `--extra-static-cmd`）。

## 作者信息

- 项目名称：C++计算器程序测试
- 质量工具：Simple Quality Suite
- 创建日期：2024

