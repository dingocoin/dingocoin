#!/usr/bin/env python3
# Copyright (c) 2026 The Dingocoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Pure-Python fallback for the ltc_scrypt C extension.

The C extension from github.com/dingocoin/ltc-scrypt is preferred and is what
install-deps.sh installs. It needs a compiler, Python development headers and
a network fetch, none of which are always available -- and when it is missing,
importing test_framework.mininode raises ImportError and takes every test that
imports it down with it. (Note that the ltc_scrypt 1.0 release on PyPI is not
a substitute: it is Python 2 only and will not compile against Python 3
headers, since it references PyStringObject.)

Litecoin-style scrypt proof-of-work is scrypt(N=1024, r=1, p=1, dklen=32)
over the 80-byte block header, used as both password and salt. hashlib
exposes exactly that on Python 3.6+ when linked against OpenSSL 1.1+, so no
external module is needed. This is slower than the C extension, produces
byte-identical results, and is only used when the extension is absent.
"""
import hashlib

# N * r * 128 = 1024 * 1 * 128 = 128 KiB of scratchpad; give OpenSSL headroom.
_MAXMEM = 1024 * 1024


def getPoWHash(data):
    """Return the 32-byte scrypt PoW hash of a block header."""
    return hashlib.scrypt(data, salt=data, n=1024, r=1, p=1, dklen=32,
                          maxmem=_MAXMEM)
