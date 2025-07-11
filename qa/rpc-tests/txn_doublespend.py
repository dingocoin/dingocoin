#!/usr/bin/env python3
# Copyright (c) 2014-2016 The Bitcoin Core developers
# Copyright (c) 2021-2022 The Dingocoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

#
# Test proper accounting with a double-spend conflict
#

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import *

class TxnMallTest(BitcoinTestFramework):

    def __init__(self):
        super().__init__()
        self.num_nodes = 4
        self.setup_clean_chain = False

    def add_options(self, parser):
        parser.add_option("--mineblock", dest="mine_block", default=False, action="store_true",
                          help="Test double-spend of 1-confirmed transaction")

    def setup_network(self):
        # Start with split network:
        return super(TxnMallTest, self).setup_network(True)

    def run_test(self):
        # All nodes should start with 7,500,000 DINGO:
        starting_balance = 7500000
        for i in range(4):
            assert_equal(self.nodes[i].getbalance(), starting_balance)
            self.nodes[i].getnewaddress("")  # bug workaround, coins generated assigned to first getnewaddress!
        
        # Assign coins to foo and bar accounts:
        node0_address_foo = self.nodes[0].getnewaddress("foo")
        fund_foo_txid = self.nodes[0].sendfrom("", node0_address_foo, 7314000)
        fund_foo_tx = self.nodes[0].gettransaction(fund_foo_txid)

        node0_address_bar = self.nodes[0].getnewaddress("bar")
        fund_bar_txid = self.nodes[0].sendfrom("", node0_address_bar, 174000)
        fund_bar_tx = self.nodes[0].gettransaction(fund_bar_txid)

        assert_equal(self.nodes[0].getbalance(""),
                     starting_balance - 7314000 - 174000 + fund_foo_tx["fee"] + fund_bar_tx["fee"])

        # Coins are sent to node1_address
        node1_address = self.nodes[1].getnewaddress("from0")

        # First: use raw transaction API to send 7499960 DINGO to node1_address,
        # but don't broadcast:
        doublespend_fee = Decimal('-1.20')
        rawtx_input_0 = {}
        rawtx_input_0["txid"] = fund_foo_txid
        rawtx_input_0["vout"] = find_output(self.nodes[0], fund_foo_txid, 7314000)
        rawtx_input_1 = {}
        rawtx_input_1["txid"] = fund_bar_txid
        rawtx_input_1["vout"] = find_output(self.nodes[0], fund_bar_txid, 174000)
        inputs = [rawtx_input_0, rawtx_input_1]
        change_address = self.nodes[0].getnewaddress()
        outputs = {}
        outputs[node1_address] = 7440000
        outputs[change_address] = 7488000 - 7440000 + doublespend_fee
        rawtx = self.nodes[0].createrawtransaction(inputs, outputs)
        doublespend = self.nodes[0].signrawtransaction(rawtx)
        assert_equal(doublespend["complete"], True)

        # Create two spends
        txid1 = self.nodes[0].sendfrom("foo", node1_address, 240000, 0)
        txid2 = self.nodes[0].sendfrom("bar", node1_address, 120000, 0)
        
        # Have node0 mine a block:
        if (self.options.mine_block):
            self.nodes[0].generate(1)
            sync_blocks(self.nodes[0:2])

        tx1 = self.nodes[0].gettransaction(txid1)
        tx2 = self.nodes[0].gettransaction(txid2)

        # Node0's balance should be starting balance, plus 500,000 DINGO for another
        # matured block, minus 7499960, minus 20, and minus transaction fees:
        expected = starting_balance + fund_foo_tx["fee"] + fund_bar_tx["fee"]
        if self.options.mine_block: expected += 500000
        expected += tx1["amount"] + tx1["fee"]
        expected += tx2["amount"] + tx2["fee"]
        assert_equal(self.nodes[0].getbalance(), expected)

        # foo and bar accounts should be debited:
        assert_equal(self.nodes[0].getbalance("foo", 0), 7314000+tx1["amount"]+tx1["fee"])
        assert_equal(self.nodes[0].getbalance("bar", 0), 174000+tx2["amount"]+tx2["fee"])

        if self.options.mine_block:
            assert_equal(tx1["confirmations"], 1)
            assert_equal(tx2["confirmations"], 1)
            # Node1's "from0" balance should be both transaction amounts:
            assert_equal(self.nodes[1].getbalance("from0"), -(tx1["amount"]+tx2["amount"]))
        else:
            assert_equal(tx1["confirmations"], 0)
            assert_equal(tx2["confirmations"], 0)
        
        # Now give doublespend and its parents to miner:
        self.nodes[2].sendrawtransaction(fund_foo_tx["hex"])
        self.nodes[2].sendrawtransaction(fund_bar_tx["hex"])
        doublespend_txid = self.nodes[2].sendrawtransaction(doublespend["hex"])
        # ... mine a block...
        self.nodes[2].generate(1)

        # Reconnect the split network, and sync chain:
        connect_nodes(self.nodes[1], 2)
        self.nodes[2].generate(1)  # Mine another block to make sure we sync
        sync_blocks(self.nodes)
        assert_equal(self.nodes[0].gettransaction(doublespend_txid)["confirmations"], 2)

        # Re-fetch transaction info:
        tx1 = self.nodes[0].gettransaction(txid1)
        tx2 = self.nodes[0].gettransaction(txid2)

        # Both transactions should be conflicted
        assert_equal(tx1["confirmations"], -2)
        assert_equal(tx2["confirmations"], -2)

        # Node0's total balance should be starting balance, plus 1000000 DINGO for 
        # two more matured blocks, minus 7440000 for the double-spend, plus fees (which are
        # negative):
        expected = starting_balance + 1000000 - 7440000 + fund_foo_tx["fee"] + fund_bar_tx["fee"] + doublespend_fee
        assert_equal(self.nodes[0].getbalance(), expected)
        assert_equal(self.nodes[0].getbalance("*"), expected)

        # Final "" balance is starting_balance - amount moved to accounts - doublespend + subsidies +
        # fees (which are negative)
        assert_equal(self.nodes[0].getbalance("foo"), 7314000)
        assert_equal(self.nodes[0].getbalance("bar"), 174000)
        assert_equal(self.nodes[0].getbalance(""), starting_balance
                                                              -7314000
                                                              - 174000
                                                              -7440000
                                                              + 1000000
                                                              + fund_foo_tx["fee"]
                                                              + fund_bar_tx["fee"]
                                                              + doublespend_fee)

        # Node1's "from0" account balance should be just the doublespend:
        assert_equal(self.nodes[1].getbalance("from0"), 7440000)

if __name__ == '__main__':
    TxnMallTest().main()

