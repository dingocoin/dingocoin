# Dingocoin Core [DINGO, Ð]

![Dingocoin](https://dingocoin.com/img/logos/dingocoin/logo/dingo_coin256.png)

[![Build Status](https://travis-ci.com/dingocoin/dingocoin.svg?branch=master)](https://travis-ci.com/dingocoin/dingocoin)

Dingocoin is a cryptocurrency like Bitcoin, although it does not use SHA256 as
its proof of work (POW). Taking development cues from Tenebrix and Litecoin,
Dingocoin currently employs a simplified variant of scrypt.
- **Website:** [dingocoin.com](https://dingocoin.com)

## License – Much license ⚖️
Dingocoin Core is released under the terms of the MIT license. See
[COPYING](COPYING) for more information or see
[opensource.org](https://opensource.org/licenses/MIT)

## Development and contributions – omg developers
Development is ongoing, and the development team, as well as other volunteers,
can freely work in their own trees and submit pull requests when features or
bug fixes are ready.

#### Version strategy
Version numbers are following ```major.minor.patch``` semantics.

### Branches
There are 4 types of branches in this repository:

- **master:** Unstable, contains the latest code under development.
- **maintenance:** Stable, contains the latest version of previous releases,
  which are still under active maintenance. Format: ```<version>-maint```
- **development:** Unstable, contains new code for upcoming releases. Format: ```<version>-dev```
- **archive:** Stable, immutable branches for old versions that no longer change
  because they are no longer maintained.

***Submit your pull requests against `master`***

*Maintenance branches are exclusively mutable by release. When a release is*
*planned, a development branch will be created and commits from master will*
*be cherry-picked into these by maintainers.*

## Contributing 🤝

If you find a bug or experience issues with this software, please report it
using the [issue system](https://github.com/dingocoin/dingocoin/issues/new?assignees=&labels=bug&template=bug_report.md&title=%5Bbug%5D+).

Please see [the contribution guide](CONTRIBUTING.md) to see how you can
participate in the development of Dingocoin Core. There are often
[topics seeking help](https://github.com/dingocoin/dingocoin/labels/help%20wanted)
where your contributions will have high impact and get very appreciation. wow.

## Very Much Frequently Asked Questions ❓

### How much dingo can exist? – So many puppies! 🐕
Mid 2022 (approximately a year and a half after release) there will be
approximately 100,000,000,000 coins.
Each subsequent block will grant 10,000 coins to encourage miners to continue to
secure the network and make up for lost wallets on hard drives/phones/lost
encryption passwords/etc.


### Such mining information ⛏

Dingocoin uses a simplified variant of the scrypt key derivation function as its
proof of work with a target time of one minute per block and difficulty
readjustment after every block. The block rewards are fixed and halve every
100,000 blocks. Starting with the 600,000th block, a permanent reward of
10,000 Dingocoin per block will be issued.  

Originally, a different payout scheme was envisioned with block rewards being
determined by taking the maximum reward as per the block schedule and applying
the result of a Mersenne Twister pseudo-random number generator to arrive at a
number between 0 and the maximum reward.

This was changed starting with block 5,000, to prevent large pools from gaming
the system and mining only high reward blocks. At the same time, the difficulty
retargeting was also changed from four hours to once per block (every minute),
implementing an algorithm courtesy of the DigiByte Coin development team, to
lessen the impact of sudden increases and decreases of network hashing rate.

**The current block reward schedule:**

1–4,999: 0–1,000,000 Dingocoin

5,000–99,999: 500,000 Dingocoin

100,000–199,999: 250,000 Dingocoin

200,000–299,999: 125,000 Dingocoin

300,000–399,999: 62,500 Dingocoin

400,000–499,999: 31,250 Dingocoin

500,000–599,999: 15,625 Dingocoin

600,000+: 10,000 Dingocoin

**The original block reward schedule, with one-minute block targets and four-hour difficulty readjustment:**

1–99,999: 0–1,000,000 Dingocoin

100,000–199,999: 0–500,000 Dingocoin

200,000–299,999: 0–250,000 Dingocoin

300,000–399,999: 0–125,000 Dingocoin

400,000–499,999: 0–62,500 Dingocoin

500,000–599,999: 0–31,250 Dingocoin

600,000+: 10,000 Dingocoin

### Wow plz make dingocoind/dingocoin-cli/dingocoin-qt

  The following are developer notes on how to build Dingocoin on your native platform. They are not complete guides, but include notes on the necessary libraries, compile flags, etc.

  - [OSX Build Notes](doc/build-osx.md)
  - [Unix Build Notes](doc/build-unix.md)
  - [Windows Build Notes](doc/build-windows.md)

### Such ports

- RPC 34646
- P2P 33117

## Development tips and tricks

**compiling for debugging**

Run `configure` with the `--enable-debug` option, then `make`. Or run `configure` with
`CXXFLAGS="-g -ggdb -O0"` or whatever debug flags you need.

**debug.log**

If the code is behaving strangely, take a look in the debug.log file in the data directory;
error and debugging messages are written there.

The `-debug=...` command-line option controls debugging; running with just `-debug` will turn
on all categories (and give you a very large debug.log file).

The Qt code routes `qDebug()` output to debug.log under category "qt": run with `-debug=qt`
to see it.

**testnet and regtest modes**

Run with the `-testnet` option to run with "play dingocoins" on the test network, if you
are testing multi-machine code that needs to operate across the internet.

If you are testing something that can run on one machine, run with the `-regtest` option.
In regression test mode, blocks can be created on-demand; see qa/rpc-tests/ for tests
that run in `-regtest` mode.

**DEBUG_LOCKORDER**

Dingocoin Core is a multithreaded application, and deadlocks or other multithreading bugs
can be very difficult to track down. Compiling with `-DDEBUG_LOCKORDER` (`configure
CXXFLAGS="-DDEBUG_LOCKORDER -g"`) inserts run-time checks to keep track of which locks
are held, and adds warnings to the debug.log file if inconsistencies are detected.
