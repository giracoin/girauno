# GiraUno
GiraUno is a fork of bitcoin-core code. For any information or instruction not listed here, please refer to the [Official Bitcoin Documentation](https://github.com/bitcoin/bitcoin/tree/master/doc).

### Deploy

The `deploy` folder contains all builds that are necessary for running Giracoin in a Server Environment:

  - `giracoin-cli` is a command line utilities for interfacing with Giracoin Blockchain
  - `giracoin-tx` is a command line set of utilities for working with Giracoin Transactions
  - `giracoind` is the core implementation of Giracoin Blockchain. Mining and Staking are disabled. It is meant to be used on Girapay and any other web wallet.
  - `giracoind-staking` is the core implementation yet with staking abilities. Performing staking on a wallet that makes intensive usage of the "account" feature is strongly discouraged.
  - `giracoind-mining` is the core implementation yet with mining abilities. It is meant to be used only by Main Wallet or any other generating server.
  - `giracoin.conf` is the default configuration file that you want to put into the default data directory

### Build
For installing dependencies and for build configuration, please refer to the [Official Bitcoin Building Documentation](https://github.com/bitcoin/bitcoin/blob/master/doc/build-unix.md). There are only two build configs that differ from Bitcoin:

- `--disable-staking` will not build the staking code base. The default configuration is to enable staking.
- `--enable-mining` will build the mining code base. The default configuration is to disable mining.

*Note:* it is strongly discouraged to build with both staking and mining abilities. Use either the first or the latter, or none of them (for web wallets).


### License
Copyright 2016-2021 Gira Financial Group AG
