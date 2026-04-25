Dingocoin Core version 1.18.1.0 is now available from:

  <https://github.com/dingocoin/dingocoin/releases/tag/v1.18.1.0/>

This is a maintenance release of Dingocoin Core. It updates the client
version to 1.18.1.0, adds DNS seeds for peer discovery, and includes fixes
for continuous integration and developer-facing tests on current GitHub
runners and toolchains.

Please report bugs using the issue tracker at github:

  <https://github.com/dingocoin/dingocoin/issues>

To receive security and update notifications, please watch reddit or Twitter:

  * https://www.reddit.com/r/dingocoin/
  * @Dingocoin on Twitter for announcements and development updates

Compatibility
==============

Dingocoin Core is extensively tested on Ubuntu Server LTS, Mac OS X and Windows 10/11.

Notable changes from 1.18.0.0
===========================

Network
-------

- Additional DNS seeds for improved peer discovery on first start.

Continuous integration
----------------------

- **Linux (GitHub Actions):** More reliable `apt-get update` / install steps
  (retries, timeouts, non-interactive installs) to reduce failures when
  Ubuntu archive mirrors are slow or flaky.
- **macOS (main branch workflow):** Ensure a writable `~/.ccache` exists before
  building with depends' ccache; cap parallel `make` jobs on hosted runners to
  reduce out-of-memory failures; print `config.log` and a verbose `make` retry
  when configure or the build fails for easier diagnosis.

Tests
-----

- **`bitcoin-util-test`:** JSON output from `dingocoin-tx` is parsed after
  stripping a UTF-8 BOM and any leading bytes before the first JSON object or
  array. This avoids spurious failures under Wine / Windows-style environments
  where unrelated text can appear before the JSON payload.

Developer / code quality
------------------------

- **`uint256::GetHex`:** Use `snprintf` instead of `sprintf` to avoid deprecated
  `sprintf` warnings on recent Apple SDKs (Xcode 16).
