#include "option_pricer/pricing.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <cmath>
#include <limits>

namespace op = option_pricer;

TEST(Pricing, IndependentReferencePrices) {
    struct Case { op::OptionParameters parameters; double expected; };
    const Case cases[] = {
        {{op::OptionType::call, 100, 100, 1, .05, 0, .2}, 10.450583572185579},
        {{op::OptionType::put, 100, 100, 1, .05, 0, .2}, 5.5735260222569671},
        {{op::OptionType::call, 100, 110, .5, .03, .02, .3}, 4.8578112002748677},
        {{op::OptionType::put, 100, 110, .5, .03, .02, .3}, 14.215141181694966},
    };
    for (const auto& c : cases) {
        const auto result = op::price(c.parameters);
        ASSERT_EQ(result.status, op::Status::ok);
        EXPECT_NEAR(result.price, c.expected, 1e-10);
    }
}

TEST(Pricing, ParityWithNegativeRate) {
    op::OptionParameters p{op::OptionType::call, 100, 105, .7, -.01, .02, .3};
    const auto call = op::price(p);
    p.type = op::OptionType::put;
    const auto put = op::price(p);
    ASSERT_EQ(call.status, op::Status::ok);
    ASSERT_EQ(put.status, op::Status::ok);
    EXPECT_NEAR(call.price - put.price,
                p.spot * std::exp(-p.dividend * p.time) - p.strike * std::exp(-p.rate * p.time), 1e-10);
}

TEST(Pricing, ExpiryAndZeroVolatility) {
    for (auto type : {op::OptionType::call, op::OptionType::put}) {
        op::OptionParameters p{type, 110, 100, 0, .05, .02, .2};
        const double sign = type == op::OptionType::call ? 1 : -1;
        auto result = op::price(p);
        ASSERT_EQ(result.status, op::Status::ok);
        EXPECT_DOUBLE_EQ(result.price, std::max(sign * (p.spot - p.strike), 0.0));
        p.time = 1;
        p.volatility = 0;
        result = op::price(p);
        ASSERT_EQ(result.status, op::Status::ok);
        EXPECT_DOUBLE_EQ(result.price, std::max(sign *
            (p.spot * std::exp(-p.dividend) - p.strike * std::exp(-p.rate)), 0.0));
    }
}

TEST(Pricing, InvalidInputsAndOverflow) {
    for (double bad : {std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::infinity()}) {
        for (auto field : {&op::OptionParameters::spot, &op::OptionParameters::strike,
                           &op::OptionParameters::time, &op::OptionParameters::rate,
                           &op::OptionParameters::dividend, &op::OptionParameters::volatility}) {
            op::OptionParameters p;
            p.*field = bad;
            EXPECT_EQ(op::price(p).status, op::Status::invalid_input);
        }
    }
    for (auto field : {&op::OptionParameters::spot, &op::OptionParameters::strike,
                       &op::OptionParameters::time, &op::OptionParameters::volatility}) {
        op::OptionParameters p;
        p.*field = -1;
        EXPECT_EQ(op::price(p).status, op::Status::invalid_input);
    }
    op::OptionParameters p;
    p.spot = 0;
    EXPECT_EQ(op::price(p).status, op::Status::invalid_input);
    p = {};
    p.rate = -1000;
    EXPECT_EQ(op::price(p).status, op::Status::numerical_error);
}
