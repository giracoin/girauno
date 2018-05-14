// Copyright (c) 20016-2021 Alberto Dallaporta (alberto@novalab.io)
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pos.h"
#include "main.h"
#include "script/interpreter.h"

unsigned int GetNextStakeTargetRequired(const CBlockIndex *pindexLast, const Consensus::Params consensusParams)
{
    const arith_uint256 bnProofOfStakeLimit = UintToArith256(consensusParams.posLimit);

    if (pindexLast == NULL || pindexLast->nHeight == 0)
        return bnProofOfStakeLimit.GetCompact(); // genesis block

    const CBlockIndex* pindexPrev = GetLastBlockIndex(pindexLast, true);
    if (pindexPrev == NULL || pindexPrev->nHeight == 0)
        return bnProofOfStakeLimit.GetCompact(); // first block

    const CBlockIndex* pindexPrevPrev = GetLastBlockIndex(pindexPrev->pprev, true);
    if (pindexPrevPrev == NULL || pindexPrevPrev->nHeight  == 0)
        return bnProofOfStakeLimit.GetCompact(); // second block

    if (fDebug)
        LogPrintf("GetNextStakeTargetRequired(): heights => pindexPrev=%d pindexPrevPrev=%d\n", pindexPrev->nHeight, pindexPrevPrev->nHeight);

    int64_t nActualSpacing = pindexPrev->GetBlockTime() - pindexPrevPrev->GetBlockTime();
    // Limit adjustment step
    if (nActualSpacing > consensusParams.nPosTargetSpacing * 10)
        nActualSpacing = consensusParams.nPosTargetSpacing * 10;

    // Retarget
    arith_uint256 bnNewTarget;
    bnNewTarget.SetCompact(pindexPrev->nBits);
    int64_t nInterval = consensusParams.nPosTargetTimespan / consensusParams.nPosTargetSpacing;
    bnNewTarget *= ((nInterval - 1) * consensusParams.nPosTargetSpacing + nActualSpacing * 2);
    bnNewTarget /= ((nInterval + 1) * consensusParams.nPosTargetSpacing);

    if (bnNewTarget <= 0 || bnNewTarget > bnProofOfStakeLimit)
        return bnProofOfStakeLimit.GetCompact();

    return bnNewTarget.GetCompact();
}

/** Giracoin kernel protocol: coinstake must meet hash target according to the protocol:
 *
 * kernel (input 0) must meet the formula
 *      hash(txPrev.block.nTime + txPrev.nTime + txPrev.vout.hash + txPrev.vout.n + nTime) < bnTarget * nWeight
 *
 * this ensures that the chance of getting a coinstake is proportional to the amount of coins one owns.
 *
 * The reason this hash is chosen is the following:
 * -    txPrev.nTime: slightly scrambles computation (actually it is blockprev.nTime, to match Giracoin 0.13.1 code base)
 * -    txPrev.vout.hash: hash of txPrev, to reduce the chance of nodes generating coinstake at the same time
 * -    txPrev.vout.n: output number of txPrev, to reduce the chance of nodes generating coinstake at the same time
 * -    nTime: current timestamp
 *
 * block/tx hash should not be used here as they can be generated in vast
 * quantities so as to generate blocks faster, degrading the system back into
 * a proof-of-work situation.
*/
bool CheckStakeSimple(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.posLimit))
        return false;

    // Check kernel meets target
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}

bool CheckStakeKernelHash(unsigned int nBits, int64_t nTimeBlockFrom, const CTransaction& txPrev, const COutPoint& prevout, unsigned int nTimeTx,
                          uint256& hashProofOfStake, const Consensus::Params consensusParams, bool fPrintProofOfStake)
{
    if (nTimeTx < nTimeBlockFrom)  // Transaction timestamp violation
        return error("CheckStakeKernelHash() : nTime violation");

    bool fNegative;
    bool fOverflow;

    // Base target
    arith_uint256 bnTarget;
    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    if (fNegative || bnTarget == 0 || fOverflow)
        return false;

    // weighted target
    bnTarget *= txPrev.vout[prevout.n].nValue;

    // Calculate hash
    CDataStream ss(SER_GETHASH, 0);
    ss << nTimeBlockFrom << prevout.hash << prevout.n << nTimeTx;
    hashProofOfStake = Hash(ss.begin(), ss.end());

    // Now check if proof-of-stake hash meets target protocol
    arith_uint256 kernel = UintToArith256(hashProofOfStake);
    if (kernel > bnTarget)
        return false;

    return true;
}

bool CheckProofOfStake(CBlockIndex* pindexPrev, const CTransaction& tx, unsigned int nBits, uint256& hashProofOfStake, int64_t blockTime,
                       const Consensus::Params consensusParams)
{
    if (!tx.IsCoinStake())
        return error("CheckProofOfStake():  called on non-coinstake %s", tx.GetHash().ToString());

    // mining-enabled wallets only
    if (!fTxIndex)
        return true;

    // Kernel (input 0) must match the stake hash target (nBits)
    const CTxIn& txin = tx.vin[0];

    // First try finding the previous transaction in database
    CDiskTxPos postx;

    if (!pblocktree->ReadTxIndex(txin.prevout.hash, postx))
        return error("CheckProofOfStake(): INFO: read txPrev failed"); // previous transaction not in main chain, may occur during initial download

    // Read block header
    CBlock block;
    CDiskBlockPos blockpos(postx.nFile, postx.nPos);

    if (!ReadBlockFromDisk(block, blockpos, consensusParams))
        return fDebug? error("CheckProofOfStake(): read block failed") : false; // unable to read block of previous transaction

    CTransaction txPrev;
    unsigned int i = 0;
    while (i < block.vtx.size() && txPrev.IsNull())
    {
        if (block.vtx[i].GetHash() == txin.prevout.hash)
            txPrev = block.vtx[i];
        i++;
    }

    if (txPrev.IsNull())
        return error("CheckProofOfStake(): INFO: read txPrev from block failed"); // probably a dev-approach error

    // Verify signature
    ScriptError serror = SCRIPT_ERR_OK;
    if (!VerifyScript(txin.scriptSig, txPrev.vout[txin.prevout.n].scriptPubKey, NULL, SCRIPT_VERIFY_NONE, TransactionSignatureChecker(&tx, 0, txPrev.vout[txin.prevout.n].nValue), &serror))
        return error("CheckProofOfStake(): VerifySignature failed on coinstake %s", tx.GetHash().ToString());

    // min confirms requirement
    int nDepth;
    if (IsConfirmedInNPrevBlocks(postx, pindexPrev, consensusParams.nStakeMinConfirmations - 1, nDepth))
        return error("CheckProofOfStake(): tried to stake at depth %d", nDepth + 1);

    if (!CheckStakeKernelHash(nBits, block.GetBlockTime(), txPrev, txin.prevout, blockTime, hashProofOfStake, consensusParams))
        if (!IsInitialBlockDownload())
            return error("CheckProofOfStake(): INFO: check kernel failed on coinstake %s, hashProof=%s", tx.GetHash().ToString(), hashProofOfStake.ToString()); // may occur during initial download or if behind on block chain sync

    return true;
}

bool CheckKernel(CBlockIndex* pindexPrev, unsigned int nBits, int64_t nTime, const COutPoint& prevout, const Consensus::Params consensusParams, int64_t* pBlockTime)
{
    uint256 hashProofOfStake;

    // First try finding the previous transaction in database
    CDiskTxPos postx;

    if (!pblocktree->ReadTxIndex(prevout.hash, postx))
        return false;

    // Read block header
    CBlock block;
    CDiskBlockPos blockpos(postx.nFile, postx.nPos);

    if (!ReadBlockFromDisk(block, blockpos, consensusParams))
        return false;

    CTransaction txPrev;
    unsigned int i = 0;
    while (i < block.vtx.size() && txPrev.IsNull())
    {
        if (block.vtx[i].GetHash() == prevout.hash)
            txPrev = block.vtx[i];
        i++;
    }

    if (txPrev.IsNull())
        return false;

    int nDepth;
    if (IsConfirmedInNPrevBlocks(postx, pindexPrev, consensusParams.nStakeMinConfirmations - 1, nDepth))
        return false;

    if (pBlockTime)
        *pBlockTime = block.GetBlockTime();

    return CheckStakeKernelHash(nBits, block.GetBlockTime(), txPrev, prevout, nTime, hashProofOfStake, consensusParams);
}

