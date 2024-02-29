// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1386325540, nBits=0x1e0ffff0, nNonce=99943, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Nintondo";
    const CScript genesisOutputScript = CScript() << ParseHex("040184710fa689ad5023690c80f3a49c8f13f8d45b8c857fbcbc8bc4a8e4d3eb4b10f4d4604fa08dce601aaf0f470216fe1b51850b4acf21b179c45070ac7b03a9") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
private:
    Consensus::Params digishieldConsensus;
    Consensus::Params auxpowConsensus;
    Consensus::Params aux2Consensus;

public:
    CMainParams() {
        strNetworkID = "main";

        genesis = CreateGenesisBlock(1386325540, 99943, 0x1e0ffff0, 1, 88 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x1a91e3dace36e2be3bf030a65679fe821aa1d6ef92e7c9902eb318182c355691"));
        assert(genesis.hashMerkleRoot == uint256S("0x5b2a3f53f605d62c53e62932dac6925e3d74afa5a4b459745c36d42d0ed26a69"));

        // Blocks 0 - 4999 are conventional difficulty calculation
        consensus.nSubsidyHalvingInterval = 100000;
        consensus.nMajorityEnforceBlockUpgrade = 1500;
        consensus.nMajorityRejectBlockOutdated = 1900;
        consensus.nMajorityWindow = 2000;
        // BIP34 is never enforced in Dingocoin v2 blocks, so we enforce from v3
        consensus.BIP34Height = 129999;
        consensus.BIP34Hash = uint256S("0x801f73ac28d2c05e33868a6e751e21f967a0ba88b03395696b97194e04f8e39b");
        consensus.BIP65Height = 197999;
        consensus.BIP66Height = 198999;
        consensus.newMinProtoHeight = 969730;
        consensus.powLimit = uint256S("0x00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20;
        consensus.nPowTargetTimespan = 4 * 60 * 60; // pre-digishield: 4 hours
        consensus.nPowTargetSpacing = 60; // 1 minute
        consensus.fDigishieldDifficultyCalculation = false;
        consensus.nCoinbaseMaturity = 30;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowAllowDigishieldMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 9576; // 95% of 10,080
        consensus.nMinerConfirmationWindow = 10080; // 60 * 24 * 7 = 10,080 blocks, or one week
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        // XXX: BIP heights and hashes all need to be updated to Dingocoin values
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1651494979; // May 5th, 2022
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 0; // Disabled

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1651494979; // May 5th, 2022
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 0; // Disabled

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000144662338111b2add0");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x9d53b93b7e1158f3c4b04a45d9bd08ae1c56fe6787786da6ffc20ed7768f10aa"); // 1,420,369

        // AuxPoW parameters
        consensus.nAuxpowChainIds = {0x0062, 0x0032}; // All future chain IDs. Used for permissive block header checks.
        consensus.nAuxpowChainId = 0x0062; // 98 - Josh Wise!
        consensus.fStrictChainId = true;
        consensus.fAllowLegacyBlocks = true;
        consensus.nHeightEffective = 0;

        // Blocks 5000 - 7500 are Digishield without AuxPoW
        digishieldConsensus = consensus;
        digishieldConsensus.nHeightEffective = 5000;
        digishieldConsensus.fSimplifiedRewards = true;
        digishieldConsensus.fDigishieldDifficultyCalculation = true;
        digishieldConsensus.nPowTargetTimespan = 60; // post-digishield: 1 minute
        digishieldConsensus.nCoinbaseMaturity = 240;

        // Blocks 7501+ are AuxPoW
        auxpowConsensus = digishieldConsensus;
        auxpowConsensus.nHeightEffective = 7500;
        auxpowConsensus.fAllowLegacyBlocks = false;

        // AuxPow2
        aux2Consensus = auxpowConsensus;
        aux2Consensus.nHeightEffective = 265000; // At 265,000 blocks we switch to new Chain ID of 50
        aux2Consensus.nAuxpowChainId = 0x0032; // Chain ID of 50
        aux2Consensus.fStrictChainId = true;
        aux2Consensus.fAllowLegacyBlocks = false;

        // Assemble the binary search tree of consensus parameters
        pConsensusRoot = &digishieldConsensus; // 5000
        pConsensusRoot->InsertConsensus(&consensus); // 0
        pConsensusRoot->InsertConsensus(&auxpowConsensus); // 7500
        pConsensusRoot->InsertConsensus(&aux2Consensus); // 265000

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xc1;
        pchMessageStart[1] = 0xc1;
        pchMessageStart[2] = 0xc1;
        pchMessageStart[3] = 0xc1;
        nDefaultPort = 33117;
        nPruneAfterHeight = 100000;

        // Note that of those with the service bits flag, most only support a subset of possible options
        vSeeds.push_back(CDNSSeedData("vps.dingocoin.com", "dnsseed.dingocoin.com", true));
        vSeeds.push_back(CDNSSeedData("australiacash.org", "seed.australiacash.org", true));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,30);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,22);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,158);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x02)(0xfa)(0xca)(0xfd).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x02)(0xfa)(0xc3)(0x98).convert_to_container<std::vector<unsigned char> >();

        //TODO: fix this for dingocoin -- plddr
        //vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));
        vFixedSeeds.clear();

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (       0, uint256S("0x1a91e3dace36e2be3bf030a65679fe821aa1d6ef92e7c9902eb318182c355691")) // Dogecoins original genesis block for shits and giggles
            (       1, uint256S("0x594a42d8fe16382085dc982135df72cf8fcea12d34e6efd566e2f9e442e2136f")) // First block mined within the Dingocoin Blockchain 03/04/2021
            (    4999, uint256S("0x2a17ca5b715be5cb5322b5c4e13fa2509744728fde377eeb3dc2eff69fec79e4")) // Final block before Digishield activation
            (    6250, uint256S("0xd2fe4ee046feaa1d175ff7120cd2e3d1ea07088745b5f52bc13df98cf8eb5094"))
            (   37486, uint256S("0x58c8b4c5c07efafaeb602b93ca0a46f4c442871964d43b26b76a281883f9d2e2"))
            (   65694, uint256S("0x71d55f53676a85567e96020194d66736751a4330814142c86a4bac9972109c3d"))
            (  159010, uint256S("0x4223941e5b442f6413921b4ee376aa75e01b8522b42d4cb0d293914e628eb81f"))
            (  205022, uint256S("0x62ad9c9bc2baefa68d6c3ed0e7d1f19cce817b21b1211f5beaa8952696ce2fec"))
            (  222333, uint256S("0xc99682f70c4243247d1fffc282134e10067769ce36bb3f53e9c90c8ff7e398e9"))
            (  300009, uint256S("0xcb18be1476083dd7e50783f71c0412ea36c2cc92b1713bf6e5e6f091d25f78dc"))
            (  487649, uint256S("0x4de898291b44f64a4e9349adda2e98556d2568a82585c4bc5b84c59636b40d24"))
            (  497420, uint256S("0x83c341127a4dfabd720c36e7f3f9d56b35dd1c30b71dc353fa646c014744cc9f"))
            (  517371, uint256S("0x5cd1462506d2d2637cd190489265b8adec9045d656a65d57f0dbf705fc447b64"))
            (  651369, uint256S("0xfc192842e08f010334e278ca02e64396458c9f0a284858ae5342db400b09d393"))
            (  883514, uint256S("0x7feb22bada667e99b60fc432a28acbc6ed7b769db6c11981c4b1855020bf2d54"))
            (  999369, uint256S("0x119faca615a876a03f72d83ef5e1f627aa7ed53ea7ce3ffc813818721ba51d9c"))
            ( 1000369, uint256S("0x1deef4ba5e669fdb233301516bce5cccc4a02b2f12ed07b392fe5b6237618aaf"))
            ( 1369420, uint256S("0xcbdb6f2b3b0ab81aa025ae58864491728a6c030455d870fca8754ce2efe67dcb"))
            ( 1420369, uint256S("0x9d53b93b7e1158f3c4b04a45d9bd08ae1c56fe6787786da6ffc20ed7768f10aa"))
        };

        chainTxData = ChainTxData{
            // Data as of block 594a42d8fe16382085dc982135df72cf8fcea12d34e6efd566e2f9e442e2136f (height 1).
            // Tx estimate based on average of year 2021 (~40k transactions per day)
            1706044525, // * UNIX timestamp of last checkpoint block
            1986348,   // * total number of transactions between genesis and last checkpoint
                        //   (the tx=... number in the SetBestChain debug.log lines)
            0.13        // * estimated number of transactions per second after checkpoint
        };
    }
};
static CMainParams mainParams;

/**
 * Testnet (v1)
 */
class CTestNetParams : public CChainParams {
private:
    Consensus::Params digishieldConsensus;
    Consensus::Params auxpowConsensus;
    Consensus::Params minDifficultyConsensus;
    Consensus::Params aux2Consensus;

public:
    CTestNetParams() {
        strNetworkID = "test";

        genesis = CreateGenesisBlock(1386325540, 99943, 0x1e0ffff0, 1, 88 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x1a91e3dace36e2be3bf030a65679fe821aa1d6ef92e7c9902eb318182c355691"));
        assert(genesis.hashMerkleRoot == uint256S("0x5b2a3f53f605d62c53e62932dac6925e3d74afa5a4b459745c36d42d0ed26a69"));

        // Blocks 0 - 4999 are conventional difficulty calculation
        consensus.nSubsidyHalvingInterval = 100000;
        consensus.nMajorityEnforceBlockUpgrade = 1500;
        consensus.nMajorityRejectBlockOutdated = 1900;
        consensus.nMajorityWindow = 2000;
        // BIP34 is never enforced in Dingocoin v2 blocks, so we enforce from v3
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256S("0x000");
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.newMinProtoHeight = 15000;
        consensus.powLimit = uint256S("0x00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20;
        consensus.nPowTargetTimespan = 4 * 60 * 60; // pre-digishield: 4 hours
        consensus.nPowTargetSpacing = 60; // 1 minute
        consensus.fDigishieldDifficultyCalculation = false;
        consensus.nCoinbaseMaturity = 30;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowAllowDigishieldMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 9576; // 95% of 10,080
        consensus.nMinerConfirmationWindow = 10080; // 60 * 24 * 7 = 10,080 blocks, or one week
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        // XXX: BIP heights and hashes all need to be updated to Dingocoin values
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1651494979; // May 5th, 2022
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 0; // Disabled

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1651494979; // May 5th, 2022
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 0; // Disabled

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00"); // 900,000

        // AuxPoW parameters
        consensus.nAuxpowChainIds = {0x0062, 0x0099}; // All future chain IDs. Used for permissive block header checks.
        consensus.nAuxpowChainId = 0x0062; // 98 - Josh Wise!
        consensus.fStrictChainId = true;
        consensus.fAllowLegacyBlocks = true;
        consensus.nHeightEffective = 0;

        // Blocks 5000 - 7500 are Digishield without AuxPoW
        digishieldConsensus = consensus;
        digishieldConsensus.nHeightEffective = 5000;
        digishieldConsensus.fSimplifiedRewards = true;
        digishieldConsensus.fDigishieldDifficultyCalculation = true;
        digishieldConsensus.nPowTargetTimespan = 60; // post-digishield: 1 minute
        digishieldConsensus.nCoinbaseMaturity = 240;

        // Blocks 7501+ are AuxPoW
        auxpowConsensus = digishieldConsensus;
        auxpowConsensus.nHeightEffective = 7500;
        auxpowConsensus.fAllowLegacyBlocks = false;

        // AuxPow2
        aux2Consensus = auxpowConsensus;
        aux2Consensus.nHeightEffective = 9999999; // At 50,000 blocks we switch to new Chain ID of 153
        aux2Consensus.nAuxpowChainId = 0x0032; // Chain ID of 50
        aux2Consensus.fStrictChainId = true;
        aux2Consensus.fAllowLegacyBlocks = false;

        // Assemble the binary search tree of consensus parameters
        pConsensusRoot = &digishieldConsensus; // 5000
        pConsensusRoot->InsertConsensus(&consensus); // 0
        pConsensusRoot->InsertConsensus(&auxpowConsensus); // 7500
        pConsensusRoot->InsertConsensus(&aux2Consensus); // 265000

        pchMessageStart[0] = 0xdc;
        pchMessageStart[1] = 0xb1;
        pchMessageStart[2] = 0xd7;
        pchMessageStart[3] = 0xda;
        nDefaultPort = 43117;
        nPruneAfterHeight = 1000;

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.push_back(CDNSSeedData("testnet", "146.19.215.43"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,113); // 0x71
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196); // 0xc4
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,241); // 0xf1
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xcf).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        //TODO: fix this for dingocoin -- plddr
        //vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            ( 0, uint256S("0x1a91e3dace36e2be3bf030a65679fe821aa1d6ef92e7c9902eb318182c355691"))
        };

        chainTxData = ChainTxData{
            // Data as of block 0xad784961be215b7872b0b1b846cd61654f7376866e7978d63fb25076be26f426 (height 1)
            1672759804, // * UNIX timestamp of last checkpoint block
            2,    // * total number of transactions between genesis and last checkpoint
            0.01        // * estimated number of transactions per second after that timestamp
        };

    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
private:
    Consensus::Params digishieldConsensus;
    Consensus::Params auxpowConsensus;
    Consensus::Params aux2Consensus;

public:
    CRegTestParams() {
        strNetworkID = "regtest";

        genesis = CreateGenesisBlock(1296688602, 2, 0x207fffff, 1, 88 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x3d2160a3b5dc4a9d62e7e66a295f70313ac808440ef7400d6c0772171ce973a5"));
        assert(genesis.hashMerkleRoot == uint256S("0x5b2a3f53f605d62c53e62932dac6925e3d74afa5a4b459745c36d42d0ed26a69"));

        consensus.hashGenesisBlock = genesis.GetHash();
        consensus.nSubsidyHalvingInterval = 150;
        consensus.nMajorityEnforceBlockUpgrade = 12;
        consensus.nMajorityRejectBlockOutdated = 13;
        consensus.nMajorityWindow = 1000;
        consensus.BIP34Height = 100000000; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1; // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 1; // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.powLimit = uint256S("0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1;
        consensus.nPowTargetTimespan = 4 * 60 * 60; // pre-digishield: 4 hours
        consensus.nPowTargetSpacing = 1; // regtest: 1 second blocks
        consensus.fDigishieldDifficultyCalculation = false;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 13; // 75% for testchains
        consensus.nMinerConfirmationWindow = 15; // Faster than normal for regtest (2,520 instead of 10,080)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 999999999999ULL;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        // AuxPow parameters
        consensus.nAuxpowChainIds = {0x0062, 0x0032}; // All future chain IDs. Used for permissive block header checks.
        consensus.nAuxpowChainId = 0x0062; // 98 - Josh Wise!
        consensus.fStrictChainId = true;
        consensus.fAllowLegacyBlocks = true;
        consensus.nHeightEffective = 0;

        // Dingocoin parameters
        consensus.fSimplifiedRewards = true;
        consensus.nCoinbaseMaturity = 60; // For easier testability in RPC tests

        digishieldConsensus = consensus;
        digishieldConsensus.nHeightEffective = 10;
        digishieldConsensus.nPowTargetTimespan = 1; // regtest: also retarget every second in digishield mode, for conformity
        digishieldConsensus.fDigishieldDifficultyCalculation = true;

        auxpowConsensus = digishieldConsensus;
        auxpowConsensus.fAllowLegacyBlocks = false;
        auxpowConsensus.nHeightEffective = 20;

        // AuxPow2
        aux2Consensus = auxpowConsensus;
        aux2Consensus.nHeightEffective = 25;
        aux2Consensus.nAuxpowChainId = 0x0032;
        aux2Consensus.fStrictChainId = true;
        aux2Consensus.fAllowLegacyBlocks = false;

        // Assemble the binary search tree of parameters
        pConsensusRoot = &digishieldConsensus; // 10
        pConsensusRoot->InsertConsensus(&consensus); // 0
        pConsensusRoot->InsertConsensus(&auxpowConsensus); // 20
        pConsensusRoot->InsertConsensus(&aux2Consensus); // 25

        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xda;
        nDefaultPort = 18444;
        nPruneAfterHeight = 1000;

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("0x3d2160a3b5dc4a9d62e7e66a295f70313ac808440ef7400d6c0772171ce973a5"))
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);  // 0x6f
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);  // 0xc4
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);  // 0xef
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
    }

    void UpdateBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

const Consensus::Params *Consensus::Params::GetConsensus(uint32_t nTargetHeight) const {
    if (nTargetHeight < this->nHeightEffective && this->pLeft != NULL) {
        return this->pLeft->GetConsensus(nTargetHeight);
    } else if (nTargetHeight > this -> nHeightEffective && this -> pRight != NULL) {
        const Consensus::Params *pCandidate = this -> pRight -> GetConsensus(nTargetHeight);
        if (pCandidate->nHeightEffective <= nTargetHeight) {
            return pCandidate;
        }
    }

    // No better match below the target height
    return this;
}

void Consensus::Params::InsertConsensus(Consensus::Params* item) {
  if (item->nHeightEffective < this->nHeightEffective) {
    if (this->pLeft == NULL) {
      this->pLeft = item;
    } else {
      this->pLeft->InsertConsensus(item);
    }
  } else if (item->nHeightEffective > this->nHeightEffective) {
    if (this->pRight == NULL) {
      this->pRight = item;
    } else {
      this->pRight->InsertConsensus(item);
    }
  } else {
    throw std::runtime_error(strprintf("Duplicate consensus.nHeightEffective: %d", item->nHeightEffective));
  }
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
            return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
            return testNetParams;
    else if (chain == CBaseChainParams::REGTEST)
            return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

void UpdateRegtestBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    regTestParams.UpdateBIP9Parameters(d, nStartTime, nTimeout);
}
