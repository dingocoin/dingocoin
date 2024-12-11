Dingocoin Core version 1.17.1.1 is now available from:

  <https://github.com/dingocoin/dingocoin/releases/tag/v1.17.1.1/>

This is a new major version release, including new rpc calls 'createauxblock' and
'submitauxblock'. Implement auxpow POW checks to combat the malformed auxpow coinbase
when there's empty array of inputs causing segmentation fault. 

Please report bugs using the issue tracker at github:

  <https://github.com/dingocoin/dingocoin/issues>

To receive security and update notifications, please watch reddit or Twitter:

  * https://www.reddit.com/r/dingocoin/
  * @Dingocoin on Twitter for high priority announcements
  * @dingocoin\_devs on Twitter for updates on development work

Compatibility
==============

Dingocoin Core is extensively tested on Ubuntu Server LTS, Mac OS X and Windows 10.

Microsoft ended support for Windows XP on [April 8th, 2014](https://www.microsoft.com/en-us/WindowsForBusiness/end-of-xp-support),
No attempt is made to prevent installing or running the software on Windows XP, you
can still do so at your own risk but be aware that there are known instabilities and issues.
Please do not report issues about Windows XP to the issue tracker.

Dingocoin Core should also work on most other Unix-like systems but is not
frequently tested on them.

Notable changes from 1.14.*
===========================

Low-level RPC changes
---------------------

- Implemented namecoins method of create and submit aux blocks:
  - `createauxblock`
  - `submitauxblock`

Performance Improvements
--------------

Manual Pruning
--------------

Dingocoin Core introduces pruning the blockchain in 1.14. Pruning
the blockchain allows for significant storage space savings as the vast majority of
the downloaded data can be discarded after processing so very little of it remains
on the disk.

Manual block pruning can now be enabled by setting `-prune=1`. Once that is set,
the RPC command `pruneblockchain` can be used to prune the blockchain up to the
specified height or timestamp.

`getinfo` Deprecated
--------------------

The `getinfo` RPC command has been deprecated. Each field in the RPC call
has been moved to another command's output with that command also giving
additional information that `getinfo` did not provide. The following table
shows where each field has been moved to:

|`getinfo` field   | Moved to                                  |
|------------------|-------------------------------------------|
`"version"`	   | `getnetworkinfo()["version"]`
`"protocolversion"`| `getnetworkinfo()["protocolversion"]`
`"walletversion"`  | `getwalletinfo()["walletversion"]`
`"balance"`	   | `getwalletinfo()["balance"]`
`"blocks"`	   | `getblockchaininfo()["blocks"]`
`"timeoffset"`	   | `getnetworkinfo()["timeoffset"]`
`"connections"`	   | `getnetworkinfo()["connections"]`
`"proxy"`	   | `getnetworkinfo()["networks"][0]["proxy"]`
`"difficulty"`	   | `getblockchaininfo()["difficulty"]`
`"testnet"`	   | `getblockchaininfo()["chain"] == "test"`
`"keypoololdest"`  | `getwalletinfo()["keypoololdest"]`
`"keypoolsize"`	   | `getwalletinfo()["keypoolsize"]`
`"unlocked_until"` | `getwalletinfo()["unlocked_until"]`
`"paytxfee"`	   | `getwalletinfo()["paytxfee"]`
`"relayfee"`	   | `getnetworkinfo()["relayfee"]`
`"errors"`	   | `getnetworkinfo()["warnings"]`

ZMQ On Windows
--------------

Previously the ZeroMQ notification system was unavailable on Windows
due to various issues with ZMQ. These have been fixed upstream and
now ZMQ can be used on Windows. Please see [this document](https://github.com/bitcoin/bitcoin/blob/master/doc/zmq.md) for
help with using ZMQ in general.

Nested RPC Commands in Debug Console
------------------------------------

The ability to nest RPC commands has been added to the debug console. This
allows users to have the output of a command become the input to another
command without running the commands separately.

The nested RPC commands use bracket syntax (i.e. `getwalletinfo()`) and can
be nested (i.e. `getblock(getblockhash(1))`). Simple queries can be
done with square brackets where object values are accessed with either an 
array index or a non-quoted string (i.e. `listunspent()[0][txid]`). Both
commas and spaces can be used to separate parameters in both the bracket syntax
and normal RPC command syntax.

Network Activity Toggle
-----------------------

A RPC command and GUI toggle have been added to enable or disable all p2p
network activity. The network status icon in the bottom right hand corner 
is now the GUI toggle. Clicking the icon will either enable or disable all
p2p network activity. If network activity is disabled, the icon will 
be grayed out with an X on top of it.

Additionally the `setnetworkactive` RPC command has been added which does
the same thing as the GUI icon. The command takes one boolean parameter,
`true` enables networking and `false` disables it.

Out-of-sync Modal Info Layer
----------------------------

When Dingocoin Core is out-of-sync on startup, a semi-transparent information
layer will be shown over top of the normal display. This layer contains
details about the current sync progress and estimates the amount of time
remaining to finish syncing. This layer can also be hidden and subsequently
unhidden by clicking on the progress bar at the bottom of the window.

