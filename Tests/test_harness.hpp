/**
 * @file test_harness.hpp
 * @author KleaSCM
 * @email KleaSCM@gmail.com
 * @brief Simple, Zig-friendly test harness with zero external dependencies
 * 
 * This test harness provides basic testing functionality without requiring
 * any external libraries or complex linking. It's designed to work seamlessly
 * with Zig's C++ toolchain and provides clear pass/fail reporting.
 */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>

namespace TestHarness {

/**
 * @brief Test result structure
 */
struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    
    TestResult(const std::string& test_name, bool success, const std::string& msg = "")
        : name(test_name), passed(success), message(msg) {}
};

/**
 * @brief Simple test harness class
 */
class TestRunner {
private:
    std::vector<TestResult> results_;
    std::string current_suite_;
    
public:
    /**
     * @brief Start a new test suite
     * @param suite_name Name of the test suite
     */
    void beginSuite(const std::string& suite_name) {
        current_suite_ = suite_name;
        std::cout << "\n=== " << suite_name << " ===\n";
    }
    
    /**
     * @brief Run a test function
     * @param test_name Name of the test
     * @param test_func Function to run
     */
    void runTest(const std::string& test_name, std::function<bool()> test_func) {
        std::cout << "Running: " << test_name << " ... ";
        
        try {
            bool result = test_func();
            results_.emplace_back(test_name, result);
            
            if (result) {
                std::cout << "PASS\n";
            } else {
                std::cout << "FAIL\n";
            }
        } catch (const std::exception& e) {
            results_.emplace_back(test_name, false, e.what());
            std::cout << "FAIL (Exception: " << e.what() << ")\n";
        } catch (...) {
            results_.emplace_back(test_name, false, "Unknown exception");
            std::cout << "FAIL (Unknown exception)\n";
        }
    }
    
    /**
     * @brief Assert that a condition is true
     * @param condition Condition to check
     * @param message Error message if condition is false
     * @return true if condition is true, false otherwise
     */
    static bool assert(bool condition, const std::string& message = "") {
        if (!condition) {
            if (!message.empty()) {
                std::cout << "Assertion failed: " << message << "\n";
            } else {
                std::cout << "Assertion failed\n";
            }
        }
        return condition;
    }
    
    /**
     * @brief Assert that two values are equal
     * @param expected Expected value
     * @param actual Actual value
     * @param message Error message if values don't match
     * @return true if values match, false otherwise
     */
    template<typename T>
    static bool assertEqual(const T& expected, const T& actual, const std::string& message = "") {
        bool result = (expected == actual);
        if (!result) {
            std::cout << "Assertion failed: expected " << expected << ", got " << actual;
            if (!message.empty()) {
                std::cout << " (" << message << ")";
            }
            std::cout << "\n";
        }
        return result;
    }
    
    /**
     * @brief Print test summary
     */
    void printSummary() {
        std::cout << "\n=== Test Summary ===\n";
        
        int passed = 0;
        int failed = 0;
        
        for (const auto& result : results_) {
            if (result.passed) {
                passed++;
            } else {
                failed++;
                std::cout << "FAILED: " << result.name;
                if (!result.message.empty()) {
                    std::cout << " - " << result.message;
                }
                std::cout << "\n";
            }
        }
        
        std::cout << "\nTotal: " << (passed + failed) << " tests\n";
        std::cout << "Passed: " << passed << "\n";
        std::cout << "Failed: " << failed << "\n";
        
        if (failed == 0) {
            std::cout << "All tests passed! 🎉\n";
        } else {
            std::cout << "Some tests failed! ❌\n";
        }
    }
    
    /**
     * @brief Get the number of failed tests
     * @return Number of failed tests
     */
    int getFailedCount() const {
        int failed = 0;
        for (const auto& result : results_) {
            if (!result.passed) failed++;
        }
        return failed;
    }
};

} // namespace TestHarness 