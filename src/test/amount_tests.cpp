// Copyright (c) 2016 The Bitcoin Core developers
// Copyright (c) 2021-2022 The Dingocoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "amount.h"
#include "test/test_bitcoin.h"

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(amount_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(GetFeeTest)
{
    CFeeRate feeRate;

    feeRate = CFeeRate(0);
    // Must always return 0
    BOOST_CHECK_EQUAL(feeRate.GetFee(0), 0);
    BOOST_CHECK_EQUAL(feeRate.GetFee(1e5), 0);

    feeRate = CFeeRate(1000);
    // Wallet fees are no longer rounded up
    BOOST_CHECK_EQUAL(feeRate.GetFee(0), 0);
    BOOST_CHECK_EQUAL(feeRate.GetFee(1), 1000);
    BOOST_CHECK_EQUAL(feeRate.GetFee(121), 1000);
    BOOST_CHECK_EQUAL(feeRate.GetFee(999), 1000);
    BOOST_CHECK_EQUAL(feeRate.GetFee(1e3), 1000);
    BOOST_CHECK_EQUAL(feeRate.GetFee(9e3), 9000);

    feeRate = CFeeRate(-1000);
    // Must always just return -1 * arg
    BOOST_CHECK_EQUAL(feeRate.GetFee(0), 0);
    BOOST_CHECK_EQUAL(feeRate.GetFee(1), -1000);
    BOOST_CHECK_EQUAL(feeRate.GetFee(121), -1000);
    BOOST_CHECK_EQUAL(feeRate.GetFee(999), -1000);
    BOOST_CHECK_EQUAL(feeRate.GetFee(1e3), -1000);
    BOOST_CHECK_EQUAL(feeRate.GetFee(9e3), -9000);

    feeRate = CFeeRate(123);
    // Truncates the result, if not integer
    BOOST_CHECK_EQUAL(feeRate.GetFee(0), 0);
    BOOST_CHECK_EQUAL(feeRate.GetFee(8), 123); // Special case: returns 1 instead of 0
    BOOST_CHECK_EQUAL(feeRate.GetFee(9), 123);
    BOOST_CHECK_EQUAL(feeRate.GetFee(121), 123);
    BOOST_CHECK_EQUAL(feeRate.GetFee(122), 123);
    BOOST_CHECK_EQUAL(feeRate.GetFee(999), 123);
    BOOST_CHECK_EQUAL(feeRate.GetFee(1e3), 123);
    BOOST_CHECK_EQUAL(feeRate.GetFee(9e3), 1107);

    feeRate = CFeeRate(-123);
    // Truncates the result, if not integer
    BOOST_CHECK_EQUAL(feeRate.GetFee(0), 0);
    BOOST_CHECK_EQUAL(feeRate.GetFee(8), -123); // Special case: returns -1 instead of 0
    BOOST_CHECK_EQUAL(feeRate.GetFee(9), -123);

    // Check full constructor
    // default value
    BOOST_CHECK(CFeeRate(CAmount(-1), 1000) == CFeeRate(-1));
    BOOST_CHECK(CFeeRate(CAmount(0), 1000) == CFeeRate(0));
    BOOST_CHECK(CFeeRate(CAmount(1), 1000) == CFeeRate(1));
    // lost precision (can only resolve satoshis per kB)
    BOOST_CHECK(CFeeRate(CAmount(1), 1001) == CFeeRate(0));
    BOOST_CHECK(CFeeRate(CAmount(2), 1001) == CFeeRate(1));
    // some more integer checks
    BOOST_CHECK(CFeeRate(CAmount(26), 789) == CFeeRate(32));
    BOOST_CHECK(CFeeRate(CAmount(27), 789) == CFeeRate(34));
    // Maximum size in bytes, should not crash
    CFeeRate(MAX_MONEY, std::numeric_limits<size_t>::max() >> 1).GetFeePerK();
}

BOOST_AUTO_TEST_SUITE_END()
