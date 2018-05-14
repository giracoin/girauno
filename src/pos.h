// Copyright (c) 20016-2021 Alberto Dallaporta (alberto@novalab.io)
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GIRACOIN_POS_H
#define GIRACOIN_POS_H

#include "chain.h"

unsigned int GetNextStakeTargetRequired(const CBlockIndex* pindexLast, const Consensus::Params consensusParams);

/** Check kernel hash target and coinstake signature - sets hashProofOfStake on success return */
bool CheckProofOfStake(CBlockIndex* pindexPrev, const CTransaction& tx, unsigned int nBits, uint256& hashProofOfStake, int64_t blockTime,
                       const Consensus::Params consensusParams);

bool CheckStakeSimple(uint256 hash, unsigned int nBits, const Consensus::Params& params);

bool CheckStakeKernelHash(unsigned int nBits, int64_t nTimeBlockFrom, const CTransaction& txPrev, const COutPoint& prevout, unsigned int nTimeTx,
                          uint256& hashProofOfStake, const Consensus::Params consensusParams, bool fPrintProofOfStake = false);

/** Wrapper around CheckStakeKernelHash()
 *
 * Also checks existence of kernel input and min age
 * Convenient for searching a kernel
 */
bool CheckKernel(CBlockIndex* pindexPrev, unsigned int nBits, int64_t nTime, const COutPoint& prevout, const Consensus::Params consensusParams, int64_t* pBlockTime = NULL);

#endif //GIRACOIN_POS_H
