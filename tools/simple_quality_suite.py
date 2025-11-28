#!/usr/bin/env python3
"""Lightweight alternative to Parasoft C++Test.

The script autogenerates tests, runs them with coverage flags, collects
coverage numbers via gcov, performs static analysis (using cppcheck when
available, otherwise a heuristic fallback) and emits JSON/HTML/XML reports.
"""
from __future__ import annotations

import argparse
import datetime as dt
import json
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Dict, List, Sequence

ROOT = Path(__file__).resolve().parents[1]
TOOLS_DIR = ROOT / "tools"
TESTS_DIR = ROOT / "tests"
REPORTS_DIR = ROOT / "reports"
COVERAGE_DIR = REPORTS_DIR / "coverage"
STATIC_DIR = REPORTS_DIR / "static"
TEST_REPORT_DIR = REPORTS_DIR / "tests"
BUILD_DIR = ROOT / "build"
BIN_PATH = BUILD_DIR / "auto_tests"
GENERATED_CPP = TESTS_DIR / "generated_tests.cpp"
SUMMARY_HTML = REPORTS_DIR / "quality_summary.html"
SUMMARY_XML = REPORTS_DIR / "quality_summary.xml"
TIMESTAMP_FMT = "%Y-%m-%dT%H:%M:%SZ"
UTC = dt.timezone.utc

COMMAND_PREFIX = "[quality-suite]"


def log(message: str) -> None:
    print(f"{COMMAND_PREFIX} {message}")


def ensure_dirs() -> None:
    for path in (REPORTS_DIR, COVERAGE_DIR, STATIC_DIR, TEST_REPORT_DIR, BUILD_DIR):
        path.mkdir(parents=True, exist_ok=True)


def run_cmd(cmd: Sequence[str], *, cwd: Path | None = None, check: bool = True) -> subprocess.CompletedProcess:
    log("$ " + " ".join(cmd))
    result = subprocess.run(cmd, cwd=cwd or ROOT, text=True, capture_output=True)
    if check and result.returncode != 0:
        log(result.stdout.strip())
        log(result.stderr.strip())
        raise subprocess.CalledProcessError(result.returncode, cmd, output=result.stdout, stderr=result.stderr)
    return result


def generate_tests(force: bool = False) -> None:
    if not GENERATED_CPP.exists() or force:
        log("Generating randomized tests ...")
        run_cmd([sys.executable, str(TOOLS_DIR / "generate_tests.py")])
    else:
        log("Using existing generated tests")


def compile_tests(compiler: str = "g++") -> None:
    cmd = [
        compiler,
        "-std=c++17",
        "-O0",
        "--coverage",
        "-g",
        "-I.",
        str(ROOT / "calculator.cpp"),
        str(GENERATED_CPP),
        "-pthread",
        "-o",
        str(BIN_PATH),
    ]
    run_cmd(cmd)


def execute_tests() -> Dict:
    result = subprocess.run([str(BIN_PATH)], cwd=ROOT, text=True, capture_output=True)
    stdout = result.stdout.strip().splitlines()
    stderr_lines = [line for line in result.stderr.splitlines() if line.strip()]
    summary = {"total": 0, "failed": 0, "passed": 0, "success": result.returncode == 0}
    for line in stdout:
        if line.startswith("TOTAL_TESTS"):
            summary["total"] = int(line.split()[1])
        elif line.startswith("FAILED_TESTS"):
            summary["failed"] = int(line.split()[1])
    summary["passed"] = summary["total"] - summary["failed"]
    summary["failures"] = stderr_lines
    summary["stdout"] = stdout
    summary["timestamp"] = dt.datetime.now(UTC).strftime(TIMESTAMP_FMT)

    TEST_REPORT_DIR.mkdir(parents=True, exist_ok=True)
    (TEST_REPORT_DIR / "results.json").write_text(json.dumps(summary, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    (TEST_REPORT_DIR / "failures.txt").write_text("\n".join(stderr_lines) + "\n", encoding="utf-8")
    return summary


def find_gcov_artifact() -> Path:
    pattern = f"{BIN_PATH.name}-*.gcno"
    matches = list(ROOT.glob(pattern)) + list(BUILD_DIR.glob(pattern))
    if not matches:
        raise FileNotFoundError(f"No gcno files found for pattern {pattern}")
    return matches[0]


def parse_percentage_line(line: str) -> Dict:
    # Example: "Lines executed:78.33% of 120"
    metric, rest = line.split(":", 1)
    percent_str, _, total_str = rest.partition("% of")
    percent = float(percent_str.strip())
    total = int(total_str.strip())
    covered = int(round(percent * total / 100.0))
    return {"metric": metric.strip(), "percent": percent, "covered": covered, "total": total}


def parse_function_coverage(gcov_path: Path) -> Dict:
    total = 0
    covered = 0
    uncovered: List[str] = []
    for line in gcov_path.read_text(encoding="utf-8").splitlines():
        if line.startswith("function "):
            total += 1
            parts = line.split(" called ")
            if len(parts) < 2:
                continue
            name = parts[0].split()[1]
            call_info = parts[1].split()[0]
            try:
                count = int(call_info)
            except ValueError:
                count = 0
            if count > 0:
                covered += 1
            else:
                uncovered.append(name)
    percent = 100.0 if total == 0 else (covered / total) * 100.0
    return {
        "percent": percent,
        "covered": covered,
        "total": total,
        "uncovered": uncovered,
    }


def collect_coverage() -> Dict:
    gcov_artifact = find_gcov_artifact()
    result = run_cmd(["gcov", "-b", str(gcov_artifact)])
    coverage = {"lines": {}, "branches": {}, "functions": {}}
    current_file = None
    current_basename = None
    for line in result.stdout.splitlines():
        if line.startswith("File '"):
            current_file = line.split("'")[1]
            current_basename = Path(current_file).name
        elif current_basename == "calculator.cpp" and line.startswith("Lines executed:"):
            coverage["lines"] = parse_percentage_line(line)
        elif current_basename == "calculator.cpp" and line.startswith("Branches executed:"):
            coverage["branches"] = parse_percentage_line(line)
    gcov_file = ROOT / "calculator.cpp.gcov"
    if gcov_file.exists():
        coverage["functions"] = parse_function_coverage(gcov_file)
        shutil.move(str(gcov_file), COVERAGE_DIR / gcov_file.name)
    coverage["timestamp"] = dt.datetime.now(UTC).strftime(TIMESTAMP_FMT)
    (COVERAGE_DIR / "summary.json").write_text(json.dumps(coverage, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    return coverage


def fallback_static_scan(files: Sequence[Path]) -> Dict:
    findings: List[Dict] = []
    suspicious_tokens = (" new ", "new[", "->")
    for path in files:
        text = path.read_text(encoding="utf-8")
        for token in suspicious_tokens:
            if token in text:
                findings.append({
                    "file": str(path.relative_to(ROOT)),
                    "line": None,
                    "severity": "info",
                    "message": f"Token '{token.strip()}' detected; review manually.",
                })
    return {
        "tool": "heuristic",
        "issues": findings,
        "note": "cppcheck not available; ran heuristic scan",
    }


def run_static_analysis(skip: bool = False) -> Dict:
    if skip:
        log("Skipping static analysis as requested")
        return {"tool": "skipped", "issues": []}

    cppcheck = shutil.which("cppcheck")
    files = [ROOT / "calculator.cpp", ROOT / "calculator.h", ROOT / "main.cpp", ROOT / "CalculatorTest.cpp", GENERATED_CPP]
    if cppcheck:
        args = [
            cppcheck,
            "--enable=warning,style,performance,portability",
            "--std=c++17",
            "--inline-suppr",
            "--template",
            "{file}:{line}:{severity}:{message}",
            "--quiet",
        ] + [str(path) for path in files]
        proc = run_cmd(args, check=False)
        issues = []
        for line in proc.stderr.splitlines():
            parts = line.split(":", 3)
            if len(parts) != 4:
                continue
            rel_file, line_no, severity, message = parts
            issues.append({
                "file": rel_file,
                "line": int(line_no) if line_no.isdigit() else None,
                "severity": severity,
                "message": message.strip(),
            })
        payload = {"tool": "cppcheck", "issues": issues, "exit_code": proc.returncode}
    else:
        payload = fallback_static_scan(files)
    payload["timestamp"] = dt.datetime.now(UTC).strftime(TIMESTAMP_FMT)
    STATIC_DIR.mkdir(parents=True, exist_ok=True)
    (STATIC_DIR / "static_analysis.json").write_text(json.dumps(payload, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    return payload


def render_html(test_summary: Dict, coverage: Dict, static_result: Dict) -> None:
    rows = []
    rows.append("<h2>测试执行</h2>")
    rows.append(f"<p>共 {test_summary['total']} 个测试，失败 {test_summary['failed']} 个。</p>")
    if test_summary.get("failures"):
        rows.append("<pre>" + "\n".join(test_summary["failures"]) + "</pre>")

    def coverage_block(title: str, data: Dict) -> str:
        if not data:
            return f"<p>{title}: 无数据</p>"
        return (
            f"<p>{title}: {data.get('percent', 0):.2f}% ("
            f"{data.get('covered', 0)}/{data.get('total', 0)})</p>"
        )

    rows.append("<h2>覆盖率</h2>")
    rows.append(coverage_block("语句覆盖率", coverage.get("lines", {})))
    rows.append(coverage_block("分支覆盖率", coverage.get("branches", {})))
    rows.append(coverage_block("函数覆盖率", coverage.get("functions", {})))

    rows.append("<h2>静态分析</h2>")
    rows.append(f"<p>工具：{static_result.get('tool')}</p>")
    if static_result.get("issues"):
        rows.append("<ul>")
        for issue in static_result["issues"][:20]:
            rows.append(
                f"<li>{issue.get('severity','info')}: {issue.get('file')}:{issue.get('line')} - {issue.get('message')}</li>"
            )
        if len(static_result["issues"]) > 20:
            rows.append("<li>……</li>")
        rows.append("</ul>")
    else:
        rows.append("<p>未发现问题。</p>")

    SUMMARY_HTML.write_text("\n".join(rows), encoding="utf-8")


def render_xml(test_summary: Dict, coverage: Dict, static_result: Dict) -> None:
    def fmt(name: str, data: Dict) -> str:
        if not data:
            return f"    <{name} percent=\"0\" covered=\"0\" total=\"0\"/>"
        return (
            f"    <{name} percent=\"{data.get('percent', 0):.2f}\" covered=\"{data.get('covered', 0)}\" "
            f"total=\"{data.get('total', 0)}\"/>"
        )

    xml = ["<qualitySummary>"]
    xml.append(
        f"  <tests total=\"{test_summary['total']}\" failed=\"{test_summary['failed']}\" "
        f"passed=\"{test_summary['passed']}\" success=\"{str(test_summary['success']).lower()}\"/>"
    )
    xml.append(fmt("statements", coverage.get("lines", {})))
    xml.append(fmt("branches", coverage.get("branches", {})))
    xml.append(fmt("functions", coverage.get("functions", {})))
    xml.append(f"  <static tool=\"{static_result.get('tool')}\" issues=\"{len(static_result.get('issues', []))}\"/>")
    xml.append("</qualitySummary>")
    SUMMARY_XML.write_text("\n".join(xml) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser(description="Simple C++ testing/coverage/static-analysis pipeline")
    parser.add_argument("--skip-generate", action="store_true", help="跳过测试用例自动生成")
    parser.add_argument("--skip-tests", action="store_true", help="跳过测试执行阶段")
    parser.add_argument("--skip-coverage", action="store_true", help="跳过覆盖率统计")
    parser.add_argument("--skip-static", action="store_true", help="跳过静态分析")
    parser.add_argument("--compiler", default="g++", help="自定义编译器 (默认 g++)")
    args = parser.parse_args()

    ensure_dirs()
    if not args.skip_generate:
        generate_tests()
    compile_tests(compiler=args.compiler)
    if args.skip_tests:
        test_summary = {"total": 0, "failed": 0, "passed": 0, "success": False, "failures": []}
    else:
        test_summary = execute_tests()

    if args.skip_coverage:
        coverage = {"lines": {}, "branches": {}, "functions": {}}
    else:
        coverage = collect_coverage()

    static_result = run_static_analysis(skip=args.skip_static)

    render_html(test_summary, coverage, static_result)
    render_xml(test_summary, coverage, static_result)

    log("质量报告已生成：" + str(SUMMARY_HTML.relative_to(ROOT)))


if __name__ == "__main__":
    main()
