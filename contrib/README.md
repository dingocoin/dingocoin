Repository Tools
---------------------

### [Developer tools](/contrib/devtools) ###
Specific tools for developers working on this repository.
Contains the script `github-merge.py` for merging GitHub pull requests securely and signing them using GPG.

### [Verify-Commits](/contrib/verify-commits) ###
Tool to verify that every merge commit was signed by a developer using the above `github-merge.py` script.

### [Linearize](/contrib/linearize) ###
Construct a linear, no-fork, best version of the blockchain.

### [Qos](/contrib/qos) ###

A Linux bash script that will set up traffic control (tc) to limit the outgoing bandwidth for connections to the Dingocoin network. This means one can have an always-on dingocoind instance running, and another local dingocoind/dingocoin-qt instance which connects to this node and receives blocks from it.

### [Seeds](/contrib/seeds) ###
Utility to generate the pnSeed[] array that is compiled into the client.

Build Tools and Keys
---------------------

### [Debian](/contrib/debian) ###
Contains files used to package dingocoind/dingocoin-qt
for Debian-based Linux systems. If you compile dingocoind/dingocoin-qt yourself, there are some useful files here.

### [Gitian-descriptors](/contrib/gitian-descriptors) ###
Notes on getting Gitian builds up and running using KVM.

### [Gitian-keys](/contrib/gitian-keys)
PGP keys used for signing Dingocoin Core [Gitian release](/doc/release-process.md) results.

### [MacDeploy](/contrib/macdeploy) ###
Scripts and notes for Mac builds.

### [RPM](/contrib/rpm) ###
RPM spec file for building dingocoin-core on RPM based distributions

### [Gitian-build](/contrib/gitian-build.sh) ###
Script for running full Gitian builds.

### [Snap](/contrib/snap) ###
snapcraft.yaml file for building and distributing dingocoin-core with Snapcraft.

Test and Verify Tools
---------------------

### [TestGen](/contrib/testgen) ###
Utilities to generate test vectors for the data-driven Dingocoin tests.

### [Verify Binaries](/contrib/verifybinaries) ###
This script attempts to download and verify the signature file SHA256SUMS.asc from dingocoin.com.
