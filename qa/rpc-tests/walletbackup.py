#!/usr/bin/env python3
# Copyright (c) 2014-2016 The Bitcoin Core developers
# Copyright (c) 2022 The Dingocoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

"""
Exercise the wallet backup code.  Ported from walletbackup.sh.

Test case is:
4 nodes. 1 2 and 3 send transactions between each other,
fourth node is a miner.
1 2 3 each mine a block to start, then
Miner creates 100 blocks so 1 2 3 each have 50 mature
coins to spend.
Then 5 iterations of 1/2/3 sending coins amongst
themselves to get transactions in the wallets,
and the miner mining one block.

Wallets are backed up using dumpwallet/backupwallet.
Then 5 more iterations of transactions and mining a block.

Miner then generates 101 more blocks, so any
transaction fees paid mature.

Sanity check:
  Sum(1,2,3,4 balances) == 114*50

1/2/3 are shutdown, and their wallets erased.
Then restore using wallet.dat backup. And
confirm 1/2/3/4 balances are same as before.

Shutdown again, restore using importwallet,
and confirm again balances are correct.
"""

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import *
from random import randint
import logging
logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO, stream=sys.stdout)

class WalletBackupTest(BitcoinTestFramework):

    def __init__(self):
        super().__init__()
        self.setup_clean_chain = True
        self.num_nodes = 4
        # nodes 1, 2,3 are spenders, let's give them a keypool=100
        self.extra_args = [["-keypool=100"], ["-keypool=100"], ["-keypool=100"], []]

    # This mirrors how the network was setup in the bash test
    def setup_network(self, split=False):
        self.nodes = start_nodes(self.num_nodes, self.options.tmpdir, self.extra_args)
        connect_nodes(self.nodes[0], 3)
        connect_nodes(self.nodes[1], 3)
        connect_nodes(self.nodes[2], 3)
        connect_nodes(self.nodes[2], 0)
        self.is_network_split=False
        self.sync_all()

    def one_send(self, from_node, to_address):
        if (randint(1,2) == 1):
            amount = Decimal(randint(1,10))
            self.nodes[from_node].sendtoaddress(to_address, amount)

    def do_one_round(self):
        a0 = self.nodes[0].getnewaddress()
        a1 = self.nodes[1].getnewaddress()
        a2 = self.nodes[2].getnewaddress()

        self.one_send(0, a1)
        self.one_send(0, a2)
        self.one_send(1, a0)
        self.one_send(1, a2)
        self.one_send(2, a0)
        self.one_send(2, a1)

        # Have the miner (node3) mine a block.
        # Must sync mempools before mining.
        sync_mempools(self.nodes)
        self.nodes[3].generate(1)
        sync_blocks(self.nodes)

    # As above, this mirrors the original bash test.
    def start_three(self):
        self.nodes[0] = start_node(0, self.options.tmpdir)
        self.nodes[1] = start_node(1, self.options.tmpdir)
        self.nodes[2] = start_node(2, self.options.tmpdir)
        connect_nodes(self.nodes[0], 3)
        connect_nodes(self.nodes[1], 3)
        connect_nodes(self.nodes[2], 3)
        connect_nodes(self.nodes[2], 0)

    def stop_three(self):
        stop_node(self.nodes[0], 0)
        stop_node(self.nodes[1], 1)
        stop_node(self.nodes[2], 2)

    def erase_three(self):
        os.remove(self.options.tmpdir + "/node0/regtest/wallet.dat")
        os.remove(self.options.tmpdir + "/node1/regtest/wallet.dat")
        os.remove(self.options.tmpdir + "/node2/regtest/wallet.dat")

    def run_test(self):
        logging.info("Generating initial blockchain")
        self.nodes[0].generate(1)
        sync_blocks(self.nodes)
        self.nodes[1].generate(1)
        sync_blocks(self.nodes)
        self.nodes[2].generate(1)
        sync_blocks(self.nodes)
        self.nodes[3].generate(60)
        sync_blocks(self.nodes)

        assert_equal(self.nodes[0].getbalance(), 500000)
        assert_equal(self.nodes[1].getbalance(), 500000)
        assert_equal(self.nodes[2].getbalance(), 500000)
        assert_equal(self.nodes[3].getbalance(), 0)

        logging.info("Creating transactions")
        # Five rounds of sending each other transactions.
        for i in range(5):
            self.do_one_round()

        logging.info("Backing up")
        tmpdir = self.options.tmpdir
        self.nodes[0].backupwallet(tmpdir + "/node0/wallet0.bak")
        self.nodes[0].dumpwallet(tmpdir + "/node0/wallet.dump")
        self.nodes[1].backupwallet(tmpdir + "/node1/wallet1.bak")
        self.nodes[1].dumpwallet(tmpdir + "/node1/wallet.dump")
        self.nodes[2].backupwallet(tmpdir + "/node2/wallet2.bak")
        self.nodes[2].dumpwallet(tmpdir + "/node2/wallet.dump")

        logging.info("More transactions")
        for i in range(5):
            self.do_one_round()

        # Generate 61 more blocks, so any fees paid mature
        self.nodes[3].generate(61)
        self.sync_all()

        balance0 = self.nodes[0].getbalance()
        balance1 = self.nodes[1].getbalance()
        balance2 = self.nodes[2].getbalance()
        balance3 = self.nodes[3].getbalance()
        total = balance0 + balance1 + balance2 + balance3

        # At this point, there are 134 blocks (63 for setup, then 10 rounds, then 61.)
        # 74 are mature, so the sum of all wallets should be 74 * 500000 = 37000000.
        assert_equal(total, 37000000)

        ##
        # Test restoring spender wallets from backups
        ##
        logging.info("Restoring using wallet.dat")
        self.stop_three()
        self.erase_three()

        # Start node2 with no chain
        shutil.rmtree(self.options.tmpdir + "/node2/regtest/blocks")
        shutil.rmtree(self.options.tmpdir + "/node2/regtest/chainstate")

        # Restore wallets from backup
        shutil.copyfile(tmpdir + "/node0/regtest/backups/wallet0.bak", tmpdir + "/node0/regtest/wallet.dat")
        shutil.copyfile(tmpdir + "/node1/regtest/backups/wallet1.bak", tmpdir + "/node1/regtest/wallet.dat")
        shutil.copyfile(tmpdir + "/node2/regtest/backups/wallet2.bak", tmpdir + "/node2/regtest/wallet.dat")

        logging.info("Re-starting nodes")
        self.start_three()
        sync_blocks(self.nodes)

        assert_equal(self.nodes[0].getbalance(), balance0)
        assert_equal(self.nodes[1].getbalance(), balance1)
        assert_equal(self.nodes[2].getbalance(), balance2)

        logging.info("Restoring using dumped wallet")
        self.stop_three()
        self.erase_three()

        #start node2 with no chain
        shutil.rmtree(self.options.tmpdir + "/node2/regtest/blocks")
        shutil.rmtree(self.options.tmpdir + "/node2/regtest/chainstate")

        self.start_three()

        assert_equal(self.nodes[0].getbalance(), 0)
        assert_equal(self.nodes[1].getbalance(), 0)
        assert_equal(self.nodes[2].getbalance(), 0)

        self.nodes[0].importwallet(tmpdir + "/node0/regtest/backups/wallet.dump")
        self.nodes[1].importwallet(tmpdir + "/node1/regtest/backups/wallet.dump")
        self.nodes[2].importwallet(tmpdir + "/node2/regtest/backups/wallet.dump")

        sync_blocks(self.nodes)

        assert_equal(self.nodes[0].getbalance(), balance0)
        assert_equal(self.nodes[1].getbalance(), balance1)
        assert_equal(self.nodes[2].getbalance(), balance2)

        # start a node that writes backups with a -backupdir
        initialize_datadir(tmpdir, 4)
        backupdir = tmpdir + "/onebackup"
        os.makedirs(backupdir)
        backupdirnode = start_node(4, tmpdir, extra_args=["-backupdir=" + backupdir])
        backupdirnode.dumpwallet('backupwallet.dump')
        backupdirnode.importwallet(tmpdir + "/onebackup/backupwallet.dump")
        stop_node(backupdirnode, 4)
        assert(os.path.exists(tmpdir + "/onebackup/backupwallet.dump"))



if __name__ == '__main__':
    WalletBackupTest().main()
