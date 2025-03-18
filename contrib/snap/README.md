# Dingocoin Snap Packaging

Commands for building and uploading a Dingocoin Core Snap to the Snap Store. Anyone on amd64 (x86_64), arm64 (aarch64), or i386 (i686) should be able to build it themselves with these instructions. This would pull the official Dingocoin binaries from the releases page, verify them, and install them on a user's machine.

## Building Locally
```
sudo apt install snapd
sudo snap install --classic snapcraft
sudo snapcraft
```

### Installing Locally
```
snap install \*.snap --devmode
```

### To Upload to the Snap Store
```
snapcraft login
snapcraft register dingocoin-core
snapcraft upload \*.snap
sudo snap install dingocoin-core
```

### Usage
```
dingocoin-unofficial.cli # for dingocoin-cli
dingocoin-unofficial.d # for dingocoind
dingocoin-unofficial.qt # for dingocoin-qt
dingocoin-unofficial.test # for test_dingocoin
dingocoin-unofficial.tx # for dingocoin-tx
```

### Uninstalling
```
sudo snap remove dingocoin-unofficial
```