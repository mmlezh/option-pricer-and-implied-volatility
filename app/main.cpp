#include "option_pricer/pricing.hpp"
#include <iomanip>
#include <iostream>

int main() {
    option_pricer::OptionParameters parameters;
    std::cout << std::setprecision(12);
    for (auto type : {option_pricer::OptionType::call, option_pricer::OptionType::put}) {
        parameters.type = type;
        const auto result = option_pricer::price(parameters);
        if (result.status != option_pricer::Status::ok) {
            std::cerr << "Pricing failed\n";
            return 1;
        }
        std::cout << (type == option_pricer::OptionType::call ? "call: " : "put: ")
                  << result.price << '\n';
    }
}
