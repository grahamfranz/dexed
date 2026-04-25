#include <cstdint>
#include <iostream>
#include <iomanip>

// DX7 DAC Quantization Function
static inline int32_t d7DacQuantize(int32_t value) {
    if (value == 0) return 0;

    bool negative = value < 0;
    uint32_t absval = negative ? uint32_t(-value) : uint32_t(value);

    int bits = 31 - __builtin_clz(absval);
    int exponent = bits;
    if (exponent > 15) exponent = 15;

    int shift = exponent - 11;
    if (shift < 0) shift = 0;

    uint32_t rounding = (shift > 0) ? (1u << (shift - 1)) : 0;
    uint32_t quantized = (absval + rounding) >> shift;

    if (quantized > 0xFFFu)
        quantized = 0xFFFu;

    absval = quantized << shift;

    return negative ? -int32_t(absval) : int32_t(absval);
}

int main() {
    std::cout << "DX7 DAC Quantization Test\n";
    std::cout << "=========================\n\n";

    // Test cases: various magnitudes
    int32_t test_values[] = {
        0,                  // Zero
        1,                  // Very small
        100,                // Small
        1000,               // Medium
        10000,              // Larger
        100000,             // Large
        1000000,            // Very large
        0x7FFFFFFF,         // Max positive
        -1,                 // Negative small
        -1000,              // Negative medium
        -1000000,           // Negative large
    };

    std::cout << std::left << std::setw(15) << "Input"
              << std::setw(15) << "Output"
              << std::setw(15) << "Difference"
              << std::setw(15) << "Exponent"
              << std::setw(15) << "Shift\n";
    std::cout << std::string(60, '-') << "\n";

    for (int32_t test_val : test_values) {
        int32_t output = d7DacQuantize(test_val);
        int32_t diff = test_val - output;

        // Calculate what exponent was used
        uint32_t absval = test_val < 0 ? uint32_t(-test_val) : uint32_t(test_val);
        int bits = test_val == 0 ? 0 : (31 - __builtin_clz(absval));
        int exponent = bits > 15 ? 15 : bits;
        int shift = exponent - 11;
        if (shift < 0) shift = 0;

        std::cout << std::left
                  << std::setw(15) << test_val
                  << std::setw(15) << output
                  << std::setw(15) << diff
                  << std::setw(15) << exponent
                  << std::setw(15) << shift
                  << "\n";
    }

    std::cout << "\nInterpretation:\n";
    std::cout << "- Output: Quantized value (should have reduced precision)\n";
    std::cout << "- Difference: Quantization error introduced\n";
    std::cout << "- Exponent: Bit position used to determine shift\n";
    std::cout << "- Shift: How many positions shifted for quantization\n";
    std::cout << "\nQuantization is working correctly if:\n";
    std::cout << "1. Non-zero inputs produce quantization errors (Difference != 0)\n";
    std::cout << "2. Larger values have higher exponents\n";
    std::cout << "3. Positive and negative values have symmetric errors\n";

    return 0;
}
