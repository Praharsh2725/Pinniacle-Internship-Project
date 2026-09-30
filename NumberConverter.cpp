#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <limits>

using namespace std;

typedef unsigned long long ull;

string baseName(int base) {
    switch (base) {
        case 2:  return "Binary";
        case 8:  return "Octal";
        case 10: return "Decimal";
        case 16: return "Hexadecimal";
    }
    return "Unknown";
}

int digitValue(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    c = toupper(static_cast<unsigned char>(c));
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

bool parseNumber(const string& text, int base, ull& result) {
    if (text.empty()) return false;
    result = 0;
    for (size_t i = 0; i < text.length(); ++i) {
        int d = digitValue(text[i]);
        if (d < 0 || d >= base) return false;                  // invalid digit for this base
        if (result > (numeric_limits<ull>::max() - d) / base)  // overflow check
            return false;
        result = result * base + d;
    }
    return true;
}

// Converts a number into a string in the given base
string toBase(ull number, int base) {
    if (number == 0) return "0";
    const string digits = "0123456789ABCDEF";
    string out;
    while (number > 0) {
        out += digits[number % base];
        number /= base;
    }
    reverse(out.begin(), out.end());
    return out;
}

int chooseBase(const string& prompt) {
    cout << "\n" << prompt << "\n"
         << "  1. Decimal\n"
         << "  2. Binary\n"
         << "  3. Octal\n"
         << "  4. Hexadecimal\n"
         << "Choose: ";
    int c;
    while (!(cin >> c) || c < 1 || c > 4) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter a number from 1 to 4: ";
    }
    const int bases[] = {10, 2, 8, 16};
    return bases[c - 1];
}

int main() {
    cout << "===== NUMBER CONVERTER =====\n";
    char again;

    do {
        int from = chooseBase("Convert FROM:");
        int to   = chooseBase("Convert TO:");

        cout << "\nEnter the " << baseName(from) << " number: ";
        string input;
        cin >> input;

        // Allow common prefixes such as 0b1010 or 0xFF
        if (input.size() > 2 && input[0] == '0') {
            char p = tolower(static_cast<unsigned char>(input[1]));
            if ((p == 'b' && from == 2) || (p == 'x' && from == 16))
                input = input.substr(2);
        }

        ull value;
        if (!parseNumber(input, from, value)) {
            cout << "Invalid " << baseName(from) << " number (or too large).\n";
        } else {
            cout << "\n" << baseName(from) << ": " << input << "\n"
                 << baseName(to) << ": " << toBase(value, to) << "\n";

            cout << "\n--- All bases ---\n"
                 << "Decimal:     " << toBase(value, 10) << "\n"
                 << "Binary:      " << toBase(value, 2)  << "\n"
                 << "Octal:       " << toBase(value, 8)  << "\n"
                 << "Hexadecimal: " << toBase(value, 16) << "\n";
        }

        cout << "\nConvert another number? (y/n): ";
        cin >> again;
    } while (again == 'y' || again == 'Y');

    cout << "Goodbye!\n";
    return 0;
}