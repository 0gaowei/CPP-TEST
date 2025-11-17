# C++程序测试大作业

## 项目说明

本项目是一个**多线程C++计算器程序**，包含基本的数学运算功能和并行计算能力，并使用C++Test进行单元测试（包括多线程测试）。

## 项目结构

```
CPP-TEST/
├── calculator.h          # 计算器类头文件
├── calculator.cpp        # 计算器类实现文件
├── main.cpp              # 主程序入口
├── CalculatorTest.cpp    # C++Test测试用例文件
├── Makefile              # 编译配置文件
├── cpptest.properties    # C++Test配置文件
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

## 使用Parasoft C++Test进行测试

### 方法一：使用C++Test图形界面

1. **打开C++Test**
   - 启动Parasoft C++Test IDE

2. **创建新项目**
   - File → New → C++ Project
   - 选择项目类型：Executable
   - 项目名称：CalculatorProject
   - 选择项目位置：当前目录

3. **导入源代码**
   - 将以下文件添加到项目：
     - `calculator.h`
     - `calculator.cpp`
     - `main.cpp`
     - `CalculatorTest.cpp`

4. **配置测试**
   - 右键点击项目 → Properties → C++Test
   - 或点击测试三角形图标右侧的下拉菜单 → 测试配置
   - 选择"用户自定义" → 点击"新建"
   - 配置名称：执行测试用例
   - 在"静态"选项卡中，取消勾选"启动静态分析"
   - 在"执行"选项卡中：
     - 勾选"启动测试执行"
     - 插桩模式：选择"插桩"
     - 覆盖率指标：选择"语句覆盖率"、"分支覆盖率"、"函数覆盖率"

5. **执行测试**
   - 选择测试配置
   - 点击运行按钮（绿色三角形）
   - C++Test将自动执行所有测试用例

6. **查看测试报告**
   - 测试完成后，在C++Test视图中查看测试结果
   - 可以查看：
     - 测试用例执行情况
     - 代码覆盖率报告
     - 测试通过/失败统计

### 方法二：使用C++Test命令行

```bash
# 设置C++Test环境变量（根据实际安装路径调整）
export CPPTEST_HOME=/path/to/parasoft/cpptest

# 运行测试
$CPPTEST_HOME/cpptestcli \
  -config cpptest.properties \
  -compiler gcc \
  -source calculator.cpp \
  -source CalculatorTest.cpp \
  -include . \
  -report report.html
```

### 方法三：使用配置文件

1. 使用提供的`cpptest.properties`配置文件
2. 在C++Test中导入该配置文件
3. 执行测试

## 测试用例说明

`CalculatorTest.cpp`包含以下测试用例：

### 单线程功能测试（1-9）

1. **testAdd()** - 测试加法功能
   - 正数相加
   - 负数相加
   - 零值测试
   - 浮点数测试

2. **testSubtract()** - 测试减法功能
   - 正数相减
   - 负数相减
   - 零值测试
   - 浮点数测试

3. **testMultiply()** - 测试乘法功能
   - 正数相乘
   - 负数相乘
   - 零值测试
   - 浮点数测试

4. **testDivide()** - 测试除法功能
   - 正常除法
   - 除零异常测试

5. **testSquare()** - 测试平方功能
   - 正数平方
   - 负数平方
   - 零值测试
   - 浮点数测试

6. **testFactorial()** - 测试阶乘功能
   - 边界值测试（0, 1）
   - 正常值测试
   - 负数异常测试

7. **testIsPrime()** - 测试质数判断
   - 质数测试
   - 非质数测试
   - 边界值测试

8. **testGcd()** - 测试最大公约数
   - 正常值测试
   - 边界值测试（0值）
   - 负数测试

9. **testLcm()** - 测试最小公倍数
   - 正常值测试
   - 边界值测试（0值）

### 多线程功能测试（10-15）

10. **testParallelFactorial()** - 测试并行计算阶乘
    - 验证多个数的阶乘并行计算结果正确性

11. **testParallelIsPrime()** - 测试并行判断质数
    - 验证多个数的质数判断并行计算结果正确性

12. **testParallelSquare()** - 测试并行计算平方
    - 验证多个数的平方并行计算结果正确性

13. **testParallelAdd()** - 测试并行加法
    - 验证多个加法运算并行计算结果正确性

14. **testThreadSafeCounter()** - 测试线程安全计数器
    - 创建10个线程，每个线程调用100次add函数
    - 验证原子计数器的线程安全性
    - 验证总调用次数为1000

15. **testConcurrentAccess()** - 测试并发访问（压力测试）
    - 创建8个线程，每个线程执行50次不同操作
    - 测试多线程环境下的并发安全性
    - 验证程序在并发访问下的稳定性

## 测试覆盖率目标

- **语句覆盖率**：目标 100%
- **分支覆盖率**：目标 95%以上
- **函数覆盖率**：目标 100%

## 注意事项

1. 确保已安装Parasoft C++Test软件
2. 如果使用命令行方式，需要正确配置环境变量
3. 测试前确保代码能够正常编译
4. 建议在测试前先运行`make clean`清理旧的编译文件

## 测试结果分析

测试完成后，应该关注：

1. **测试通过率**：所有测试用例是否通过
2. **代码覆盖率**：哪些代码被测试覆盖，哪些未覆盖
3. **边界条件**：边界值和异常情况是否得到充分测试
4. **性能测试**：可以添加性能测试用例（可选）

## 扩展建议

如果需要更完善的测试，可以考虑：

1. 添加性能测试用例
2. 添加压力测试用例
3. 使用更高级的测试框架（如Google Test）
4. 添加集成测试
5. 添加回归测试

## 作者信息

- 项目名称：C++计算器程序测试
- 测试工具：Parasoft C++Test
- 创建日期：2024

