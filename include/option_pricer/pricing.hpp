#pragma once

/** @file pricing.hpp Minimal European option pricing API for the homework. */
namespace option_pricer {

/** European option payoff type. */
enum class OptionType { call, put };
/** Only ok permits using the returned price. */
enum class Status { ok, invalid_input, numerical_error };

/** BSM inputs: time in years; rates and volatility as fractions (0.2 = 20%). */
struct OptionParameters {
    OptionType type = OptionType::call;
    double spot = 100;
    double strike = 100;
    double time = 1;
    double rate = 0.05;
    double dividend = 0;
    double volatility = 0.2;
};

/** Price per underlying unit, in the same currency as spot and strike. */
struct PricingResult {
    Status status = Status::ok;
    double price = 0;
};

/**
 * Price a European call/put using Black–Scholes–Merton.
 * Supports expiry (time = 0) and deterministic payoff (volatility = 0).
 * Rejects nonfinite inputs, nonpositive spot/strike, negative time/volatility.
 * Returns numerical_error if intermediate values cannot be represented.
 * @param parameters Option parameters with continuous rates and dividend yield.
 * @return Status and price; the price is meaningful only when status is ok.
 */
PricingResult price(const OptionParameters& parameters) noexcept;

}  // namespace option_pricer
