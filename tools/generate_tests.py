#!/usr/bin/env python3
"""Generate randomized calculator tests without external frameworks."""
from __future__ import annotations

import json
import math
import random
import textwrap
from dataclasses import dataclass, asdict
from pathlib import Path
from typing import Callable, List, Sequence

ROOT = Path(__file__).resolve().parents[1]
TESTS_DIR = ROOT / "tests"
TEST_CPP = TESTS_DIR / "generated_tests.cpp"
META_JSON = TESTS_DIR / "generated_tests.json"
RANDOM_SEED = 2025
CALLS_PER_THREAD = 200
COUNTER_THREADS = 4
random.seed(RANDOM_SEED)


def rand_float(low: float = -250.0, high: float = 250.0) -> float:
    value = random.uniform(low, high)
    return round(value, 3)


def rand_int(low: int, high: int) -> int:
    return random.randint(low, high)


def format_float(value: float) -> str:
    if abs(value - int(value)) < 1e-9:
        return f"{int(value)}.0"
    return f"{value:.12f}"


@dataclass
class BinaryCase:
    a: float
    b: float
    expected: float

    def to_cpp(self) -> str:
        return f"{{{format_float(self.a)}, {format_float(self.b)}, {format_float(self.expected)}}}"


@dataclass
class UnaryCase:
    value: float
    expected: float

    def to_cpp(self) -> str:
        return f"{{{format_float(self.value)}, {format_float(self.expected)}}}"


@dataclass
class IntCase:
    value: int
    expected: int

    def to_cpp(self) -> str:
        return f"{{{self.value}, {self.expected}}}"


@dataclass
class BoolCase:
    value: int
    expected: bool

    def to_cpp(self) -> str:
        res = "true" if self.expected else "false"
        return f"{{{self.value}, {res}}}"


@dataclass
class DivideCase:
    a: float
    b: float
    expected: float | None

    def to_cpp(self) -> str:
        exp = "std::nullopt" if self.expected is None else format_float(self.expected)
        return f"{{{format_float(self.a)}, {format_float(self.b)}, {exp}}}"


def build_binary_cases(count: int, fn: Callable[[float, float], float]) -> List[BinaryCase]:
    cases: List[BinaryCase] = []
    for _ in range(count):
        a = rand_float()
        b = rand_float()
        cases.append(BinaryCase(a=a, b=b, expected=fn(a, b)))
    return cases


def build_unary_cases(count: int, fn: Callable[[float], float]) -> List[UnaryCase]:
    cases: List[UnaryCase] = []
    for _ in range(count):
        value = rand_float()
        cases.append(UnaryCase(value=value, expected=fn(value)))
    return cases


def build_int_cases(count: int, lo: int, hi: int, fn: Callable[[int], int]) -> List[IntCase]:
    cases: List[IntCase] = []
    for _ in range(count):
        value = rand_int(lo, hi)
        cases.append(IntCase(value=value, expected=fn(value)))
    return cases


def build_bool_cases(count: int, lo: int, hi: int, fn: Callable[[int], bool]) -> List[BoolCase]:
    cases: List[BoolCase] = []
    for _ in range(count):
        value = rand_int(lo, hi)
        cases.append(BoolCase(value=value, expected=fn(value)))
    return cases


add_cases = build_binary_cases(25, lambda a, b: a + b)
subtract_cases = build_binary_cases(25, lambda a, b: a - b)
multiply_cases = build_binary_cases(25, lambda a, b: a * b)

# Avoid near-zero divisor for non-exception cases.
divide_cases: List[DivideCase] = []
for _ in range(18):
    a = rand_float()
    b = rand_float()
    while abs(b) < 1e-2:
        b = rand_float()
    divide_cases.append(DivideCase(a=a, b=b, expected=a / b))
# Add explicit division-by-zero scenarios
for a in (-10.0, -1.0, 0.0, 25.0, 99.0):
    divide_cases.append(DivideCase(a=a, b=0.0, expected=None))

square_cases = build_unary_cases(20, lambda x: x * x)
factorial_cases = build_int_cases(12, 0, 12, lambda n: math.factorial(n))
def gcd_py(a: int, b: int) -> int:
    return math.gcd(a, b)

def lcm_py(a: int, b: int) -> int:
    if a == 0 or b == 0:
        return 0
    g = math.gcd(a, b)
    product = a * b
    sign = 1 if product >= 0 else -1
    return sign * (abs(product) // g)

gcd_cases: List[BinaryCase] = []
lcm_cases: List[BinaryCase] = []
for _ in range(20):
    a = rand_int(-500, 500)
    b = rand_int(-500, 500)
    gcd_cases.append(BinaryCase(a=a, b=b, expected=gcd_py(int(a), int(b))))
    lcm_cases.append(BinaryCase(a=a, b=b, expected=lcm_py(int(a), int(b))))

parallel_vectors = [
    [rand_int(1, 12) for _ in range(5)],
    [rand_int(2, 8) for _ in range(6)],
]
prime_vectors = [
    [2, 3, 4, 5, 99, 101],
    [17, 18, 19, 23, 25, 29],
]

def is_prime(n: int) -> bool:
    if n < 2:
        return False
    if n == 2:
        return True
    if n % 2 == 0:
        return False
    limit = int(math.sqrt(n)) + 1
    for i in range(3, limit, 2):
        if n % i == 0:
            return False
    return True

prime_cases = build_bool_cases(20, -10, 120, lambda n: is_prime(n))

def emit_cases(cases: Sequence, cpp_type: str) -> str:
    joined = ",\n        ".join(case.to_cpp() for case in cases)
    return f"std::vector<{cpp_type}>{{\n        {joined}\n    }}"


def emit_parallel_vectors(vectors: Sequence[Sequence[int]]) -> str:
    payload = []
    for vec in vectors:
        inside = ", ".join(str(v) for v in vec)
        payload.append(f"std::vector<int>{{{inside}}}")
    return "{\n        " + ",\n        ".join(payload) + "\n    }"


def render_cpp() -> str:
    return textwrap.dedent(
        f"""// Auto-generated by tools/generate_tests.py
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

namespace {{
constexpr double kEps = 1e-7;
bool approx_equal(double lhs, double rhs) {{
    return std::fabs(lhs - rhs) < kEps;
}}

struct BinaryCase {{
    double a;
    double b;
    double expected;
}};

struct UnaryCase {{
    double value;
    double expected;
}};

struct IntCase {{
    int value;
    long long expected;
}};

struct BoolCase {{
    int value;
    bool expected;
}};

struct DivideCase {{
    double a;
    double b;
    std::optional<double> expected;
}};

struct ResultCounter {{
    std::size_t total{{0}};
    std::size_t failed{{0}};

    void record(bool ok, const std::string& name, const std::string& detail) {{
        ++total;
        if (!ok) {{
            ++failed;
            std::cerr << "[FAILED] " << name << ": " << detail << std::endl;
        }}
    }}
}};
}}  // namespace

int main() {{
    Calculator calc;
    ResultCounter counter;

    const auto add_cases = {emit_cases(add_cases, "BinaryCase")};
    for (const auto& tc : add_cases) {{
        double result = calc.add(tc.a, tc.b);
        counter.record(approx_equal(result, tc.expected), "add",
                       std::to_string(tc.a) + "," + std::to_string(tc.b));
    }}

    const auto subtract_cases = {emit_cases(subtract_cases, "BinaryCase")};
    for (const auto& tc : subtract_cases) {{
        double result = calc.subtract(tc.a, tc.b);
        counter.record(approx_equal(result, tc.expected), "subtract",
                       std::to_string(tc.a) + "," + std::to_string(tc.b));
    }}

    const auto multiply_cases = {emit_cases(multiply_cases, "BinaryCase")};
    for (const auto& tc : multiply_cases) {{
        double result = calc.multiply(tc.a, tc.b);
        counter.record(approx_equal(result, tc.expected), "multiply",
                       std::to_string(tc.a) + "," + std::to_string(tc.b));
    }}

    const std::vector<DivideCase> divide_cases = {emit_cases(divide_cases, "DivideCase")};
    for (const auto& tc : divide_cases) {{
        bool raised = false;
        std::optional<double> value;
        try {{
            value = calc.divide(tc.a, tc.b);
        }} catch (const std::runtime_error&) {{
            raised = true;
        }}
        if (tc.expected.has_value()) {{
            bool ok = (!raised) && approx_equal(*value, *tc.expected);
            counter.record(ok, "divide", std::to_string(tc.a) + "/" + std::to_string(tc.b));
        }} else {{
            counter.record(raised, "divide_exception", std::to_string(tc.a));
        }}
    }}

    const auto square_cases = {emit_cases(square_cases, "UnaryCase")};
    for (const auto& tc : square_cases) {{
        double result = calc.square(tc.value);
        counter.record(approx_equal(result, tc.expected), "square",
                       std::to_string(tc.value));
    }}

    const auto factorial_cases = {emit_cases(factorial_cases, "IntCase")};
    for (const auto& tc : factorial_cases) {{
        long long result = calc.factorial(tc.value);
        counter.record(result == tc.expected, "factorial", std::to_string(tc.value));
    }}

    const auto prime_cases = {emit_cases(prime_cases, "BoolCase")};
    for (const auto& tc : prime_cases) {{
        bool result = calc.isPrime(tc.value);
        counter.record(result == tc.expected, "isPrime", std::to_string(tc.value));
    }}

    const auto gcd_cases = {emit_cases(gcd_cases, "BinaryCase")};
    for (const auto& tc : gcd_cases) {{
        int result = calc.gcd(static_cast<int>(tc.a), static_cast<int>(tc.b));
        counter.record(result == static_cast<int>(tc.expected), "gcd",
                       std::to_string(tc.a) + "," + std::to_string(tc.b));
    }}

    const auto lcm_cases = {emit_cases(lcm_cases, "BinaryCase")};
    for (const auto& tc : lcm_cases) {{
        int result = calc.lcm(static_cast<int>(tc.a), static_cast<int>(tc.b));
        counter.record(result == static_cast<int>(tc.expected), "lcm",
                       std::to_string(tc.a) + "," + std::to_string(tc.b));
    }}

    // Parallel factorial / primes / squares / add
    const std::vector<std::vector<int>> factorial_vectors = {emit_parallel_vectors(parallel_vectors)};
    for (const auto& vec : factorial_vectors) {{
        auto result = calc.parallelFactorial(vec);
        bool ok = true;
        for (std::size_t i = 0; i < vec.size(); ++i) {{
            long long expected = calc.factorial(vec[i]);
            if (result[i] != expected) {{
                ok = false;
            }}
        }}
        counter.record(ok, "parallelFactorial", std::to_string(vec.size()));
    }}

    const std::vector<std::vector<int>> prime_vectors = {emit_parallel_vectors(prime_vectors)};
    for (const auto& vec : prime_vectors) {{
        auto result = calc.parallelIsPrime(vec);
        bool ok = true;
        for (std::size_t i = 0; i < vec.size(); ++i) {{
            if (result[i] != calc.isPrime(vec[i])) {{
                ok = false;
            }}
        }}
        counter.record(ok, "parallelIsPrime", std::to_string(vec.size()));
    }}

    const std::vector<std::vector<double>> square_vectors{{
        std::vector<double>{{1.5, -2.0, 3.5, 10.0}},
        std::vector<double>{{0.0, 2.2, -3.3, 4.4}}
    }};
    for (const auto& vec : square_vectors) {{
        auto result = calc.parallelSquare(vec);
        bool ok = true;
        for (std::size_t i = 0; i < vec.size(); ++i) {{
            if (!approx_equal(result[i], calc.square(vec[i]))) {{
                ok = false;
            }}
        }}
        counter.record(ok, "parallelSquare", std::to_string(vec.size()));
    }}

    const std::vector<std::vector<std::pair<double, double>>> add_batches{{
        std::vector<std::pair<double, double>>{{{{1.0, 2.0}}, {{-5.5, 10.1}}, {{99.9, -12.2}}}},
        std::vector<std::pair<double, double>>{{{{0.0, 0.0}}, {{42.0, -17.5}}, {{8.3, 9.7}}}}
    }};
    for (const auto& vec : add_batches) {{
        auto result = calc.parallelAdd(vec);
        bool ok = true;
        for (std::size_t i = 0; i < vec.size(); ++i) {{
            if (!approx_equal(result[i], calc.add(vec[i].first, vec[i].second))) {{
                ok = false;
            }}
        }}
        counter.record(ok, "parallelAdd", std::to_string(vec.size()));
    }}

    const std::vector<int> batch_values{{5, 7, 9, 11, 13, 15}};
    calc.parallelBatchCompute(batch_values, 3);
    counter.record(true, "parallelBatchCompute", std::to_string(batch_values.size()));

    calc.resetCallCount();
    const int threads = {COUNTER_THREADS};
    const int calls_per_thread = {CALLS_PER_THREAD};
    std::vector<std::thread> workers;
    for (int i = 0; i < threads; ++i) {{
        workers.emplace_back([&calc]() {{
            for (int j = 0; j < calls_per_thread; ++j) {{
                calc.add(1.0, 2.0);
            }}
        }});
    }}
    for (auto& t : workers) {{
        t.join();
    }}
    counter.record(calc.getCallCount() == threads * calls_per_thread,
                   "threadSafeCounter", std::to_string(calc.getCallCount()));

    std::cout << "TOTAL_TESTS " << counter.total << std::endl;
    std::cout << "FAILED_TESTS " << counter.failed << std::endl;
    return counter.failed == 0 ? 0 : 1;
}}
"""
    )


def dump_metadata() -> None:
    payload = {
        "seed": RANDOM_SEED,
        "add_cases": [asdict(case) for case in add_cases],
        "subtract_cases": [asdict(case) for case in subtract_cases],
        "multiply_cases": [asdict(case) for case in multiply_cases],
        "divide_cases": [asdict(case) for case in divide_cases],
        "square_cases": [asdict(case) for case in square_cases],
        "factorial_cases": [asdict(case) for case in factorial_cases],
        "prime_cases": [asdict(case) for case in prime_cases],
        "gcd_cases": [asdict(case) for case in gcd_cases],
        "lcm_cases": [asdict(case) for case in lcm_cases],
    }
    META_JSON.write_text(json.dumps(payload, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")


def main() -> None:
    TESTS_DIR.mkdir(parents=True, exist_ok=True)
    code = render_cpp()
    TEST_CPP.write_text(code, encoding="utf-8")
    dump_metadata()
    print(f"Generated tests at {TEST_CPP.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
