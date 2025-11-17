# Makefile for Calculator Project
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g -pthread
TARGET = calculator
TEST_TARGET = calculator_test
SRC_DIR = .
OBJ_DIR = obj

# 源文件
MAIN_SRC = main.cpp
CALC_SRC = calculator.cpp
TEST_SRC = CalculatorTest.cpp

# 目标文件
MAIN_OBJ = $(OBJ_DIR)/main.o
CALC_OBJ = $(OBJ_DIR)/calculator.o
TEST_OBJ = $(OBJ_DIR)/CalculatorTest.o

# 创建目标目录
$(shell mkdir -p $(OBJ_DIR))

# 默认目标：编译主程序
all: $(TARGET)

# 编译主程序
$(TARGET): $(MAIN_OBJ) $(CALC_OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(MAIN_OBJ) $(CALC_OBJ)

# 编译测试程序
test: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJ) $(CALC_OBJ)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJ) $(CALC_OBJ)

# 编译主程序目标文件
$(OBJ_DIR)/main.o: $(MAIN_SRC) calculator.h
	$(CXX) $(CXXFLAGS) -c $(MAIN_SRC) -o $(OBJ_DIR)/main.o

# 编译计算器目标文件
$(OBJ_DIR)/calculator.o: $(CALC_SRC) calculator.h
	$(CXX) $(CXXFLAGS) -c $(CALC_SRC) -o $(OBJ_DIR)/calculator.o

# 编译测试目标文件
$(OBJ_DIR)/CalculatorTest.o: $(TEST_SRC) calculator.h
	$(CXX) $(CXXFLAGS) -c $(TEST_SRC) -o $(OBJ_DIR)/CalculatorTest.o

# 运行主程序
run: $(TARGET)
	./$(TARGET)

# 运行测试
runtest: $(TEST_TARGET)
	./$(TEST_TARGET)

# 清理
clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(TEST_TARGET)

.PHONY: all test run runtest clean

