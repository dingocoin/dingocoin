#!/bin/bash

# installs test dependencies
set -euo pipefail

# Pinned to a commit rather than refs/tags/v1.0.1: the ltc-scrypt repository has
# no tags or releases, so the tag archive 404s and the sha256sum below then runs
# against a 14-byte "Not Found" body. A commit archive is content-pinned and
# cannot move. If GitHub ever regenerates source archives the checksum may need
# refreshing -- the commit itself is the thing being pinned.
commit=1dae59bc92ac2d022e686c5bad6e82eb220d112d
file=ltc-scrypt-$commit.tar.gz
curl -fL -o "$file" "https://github.com/dingocoin/ltc-scrypt/archive/$commit.tar.gz"
echo "5a6fd987c092afa9b24c2b72c3847f4362f5d5317c1c52c35c196d8694338249  $file" | sha256sum -c
python3 -m pip install "$file" --user
rm -rf "$file"
