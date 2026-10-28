#include "option_pricer/pricing.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace option_pricer {
namespace {
bool valid(const OptionParameters& p) {
    return (p.type == OptionType::call || p.type == OptionType::put) &&
           std::isfinite(p.spot) && p.spot > 0 && std::isfinite(p.strike) && p.strike > 0 &&
           std::isfinite(p.time) && p.time >= 0 && std::isfinite(p.rate) &&
           std::isfinite(p.dividend) && std::isfinite(p.volatility) && p.volatility >= 0;
}
double cdf(double x) { return 0.5 * std::erfc(-x / std::numbers::sqrt2); }
struct Terms {
    double a, b, root_t, v, d1, d2;
    bool finite;
};
Terms terms(const OptionParameters& p) {
    const double a = p.spot * std::exp(-p.dividend * p.time);
    const double b = p.strike * std::exp(-p.rate * p.time);
    const double root_t = std::sqrt(p.time);
    const double v = p.volatility * root_t;
    const bool finite = std::isfinite(a) && a > 0 && std::isfinite(b) && b > 0 &&
                        std::isfinite(v);
    const double d1 = v > 0 && finite ? (std::log(a) - std::log(b)) / v + v / 2 : 0;
    return {a, b, root_t, v, d1, d1 - v, finite && std::isfinite(d1)};
}
}  // namespace

PricingResult price(const OptionParameters& p) noexcept {
    if (!valid(p)) return {Status::invalid_input};
    if (p.time == 0) {
        const double payoff = p.type == OptionType::call ? p.spot - p.strike : p.strike - p.spot;
        return {Status::ok, std::max(payoff, 0.0)};
    }
    const auto t = terms(p);
    if (!t.finite) return {Status::numerical_error};
    if (t.v == 0) {
        if (p.volatility != 0) return {Status::numerical_error};
        return {Status::ok, std::max(p.type == OptionType::call ? t.a - t.b : t.b - t.a, 0.0)};
    }
    double value = 0;
    if (t.a <= t.b) {
        const double call = std::max(t.a * cdf(t.d1) - t.b * cdf(t.d2), 0.0);
        value = p.type == OptionType::call ? call : call + (t.b - t.a);
    } else {
        const double put = std::max(t.b * cdf(-t.d2) - t.a * cdf(-t.d1), 0.0);
        value = p.type == OptionType::put ? put : put + (t.a - t.b);
    }
    return std::isfinite(value) ? PricingResult{Status::ok, value}
                                : PricingResult{Status::numerical_error};
}

}  // namespace option_pricer
