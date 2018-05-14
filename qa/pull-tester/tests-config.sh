#!/bin/bash
# Copyright (c) 2013-2014 The Giracoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

BUILDDIR="/home/alberto/Projects/crypto/giracoin/vcs/cryptocurrency/core-test"
EXEEXT=""

# These will turn into comments if they were disabled when configuring.
ENABLE_WALLET=1
ENABLE_UTILS=1
ENABLE_GIRACOIND=1

REAL_GIRACOIND="$BUILDDIR/src/giracoind${EXEEXT}"
REAL_GIRACOINCLI="$BUILDDIR/src/giracoin-cli${EXEEXT}"

