#!/usr/bin/env python3
# Copyright (c) 2021 The Dingocoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""PayTxFee QA test.

# Tests wallet behavior of -paytxfee in relation to -mintxfee
"""

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import *
from decimal import Decimal

class PayTxFeeTest(BitcoinTestFramework):

    def __init__(self):
        super().__init__()
        self.setup_clean_chain = True
        self.num_nodes = 4

    # -paytxfee must be at least the node's own -minrelaytxfee or the daemon
    # refuses to start, and the "-paytxfee lowers -mintxfee" interaction in
    # CWallet only fires when -paytxfee is below minTxFee (1 DINGO/kB by
    # default). With the default relay floor of 1 DINGO/kB those two
    # conditions cannot both hold, so the interaction is unreachable. Lower
    # the floor so this test can exercise it at all.
    RELAY = "-minrelaytxfee=0.01"

    def setup_nodes(self, split=False):
        nodes = []

        # node 0 has txindex to track txs
        nodes.append(start_node(0, self.options.tmpdir,
            ["-debug", "-txindex", self.RELAY]))

        # node 1 pays 0.1 DINGO/kB: no explicit -mintxfee, so the lower
        # -paytxfee overrides the default minTxFee.
        nodes.append(start_node(1, self.options.tmpdir,
            ["-paytxfee=0.1", "-debug", self.RELAY]))

        # node 2 pays 1 DINGO/kB: an explicit -mintxfee above -paytxfee wins.
        nodes.append(start_node(2, self.options.tmpdir,
            ["-mintxfee=1", "-paytxfee=0.1", "-debug", self.RELAY]))

        # node 3 pays 0.01 DINGO/kB: an explicit -mintxfee *below* -paytxfee
        # also wins, because CWallet::GetMinimumFee discards payTxFee and
        # returns GetRequiredFee(), which is driven by minTxFee alone. So
        # -paytxfee never raises the fee -- it can only lower it, and only
        # when -mintxfee is unset. Asserted here as observed behaviour; whether
        # -paytxfee ought to act as a floor is a separate question.
        nodes.append(start_node(3, self.options.tmpdir,
            ["-mintxfee=0.01", "-paytxfee=0.1", "-debug", self.RELAY]))

        return nodes

    def run_test(self):

        seed = 1000 # the amount to seed wallets with
        amount = 995 # the amount to send back
        targetAddress = self.nodes[0].getnewaddress()

        # mine some blocks and prepare some coins
        self.nodes[0].generate(102)
        self.nodes[0].sendtoaddress(self.nodes[1].getnewaddress(), seed)
        self.nodes[0].sendtoaddress(self.nodes[2].getnewaddress(), seed)
        self.nodes[0].sendtoaddress(self.nodes[3].getnewaddress(), seed)
        self.nodes[0].generate(1)
        self.sync_all()

        # create transactions
        txid1 = self.nodes[1].sendtoaddress(targetAddress, amount)
        txid2 = self.nodes[2].sendtoaddress(targetAddress, amount)
        txid3 = self.nodes[3].sendtoaddress(targetAddress, amount)
        self.sync_all()

        # Each wallet spent exactly the one `seed`-sized output it was given,
        # so the fee is whatever did not come back out.
        def fee_of(txid):
            tx = self.nodes[0].getrawtransaction(txid, True)
            return Decimal(seed) - sum(v['value'] for v in tx['vout'])

        fee1 = fee_of(txid1)
        fee2 = fee_of(txid2)
        fee3 = fee_of(txid3)

        # Assert the configured rates in relation to each other rather than as
        # absolute amounts: the three transactions are the same shape, so the
        # ratios hold whatever rule converts size into a fee, and the test does
        # not have to be rewritten whenever the fee schedule moves.
        assert fee1 > 0 and fee2 > 0 and fee3 > 0
        assert_equal(fee2, fee1 * 10)   # -mintxfee 1 vs 0.1
        assert_equal(fee1, fee3 * 10)   # -paytxfee 0.1 vs -mintxfee 0.01

        # Sanity bound: a sub-kilobyte transaction can never owe more than one
        # kilobyte's worth of its own rate.
        assert fee1 <= Decimal("0.1")

        # mine a block
        self.nodes[0].generate(1);
        self.sync_all()

        # every fee paid above should now be in the coinbase
        block = self.nodes[0].getblock(self.nodes[0].getbestblockhash())
        coinbaseTx = self.nodes[0].getrawtransaction(block['tx'][0], True)

        assert_equal(coinbaseTx['vout'][0]['value'],
                     Decimal("500000") + fee1 + fee2 + fee3)

if __name__ == '__main__':
    PayTxFeeTest().main()
