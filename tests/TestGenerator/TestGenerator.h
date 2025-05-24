// Copyright (c) 2012-2016, The MevaCoin developers, The Bytecoin developers
//
// This file is part of Karbo.
//
// Karbo is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Karbo is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Karbo.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <cstdint>
#include <list>
#include <vector>
#include <unordered_map>

#include "crypto/hash.h"
#include "MevaCoinCore/MevaCoinBasic.h"
#include "MevaCoinCore/MevaCoinBasicImpl.h"
#include "MevaCoinCore/MevaCoinFormatUtils.h"
#include "MevaCoinCore/Currency.h"
#include "MevaCoinCore/Difficulty.h"


class test_generator
{
public:
  struct BlockInfo {
    BlockInfo()
      : previousBlockHash()
      , alreadyGeneratedCoins(0)
      , blockSize(0) {
    }

    BlockInfo(Crypto::Hash aPrevId, uint64_t anAlreadyGeneratedCoins, size_t aBlockSize)
      : previousBlockHash(aPrevId)
      , alreadyGeneratedCoins(anAlreadyGeneratedCoins)
      , blockSize(aBlockSize) {
    }

    Crypto::Hash previousBlockHash;
    uint64_t alreadyGeneratedCoins;
    size_t blockSize;
  };

  enum BlockFields {
    bf_none      = 0,
    bf_major_ver = 1 << 0,
    bf_minor_ver = 1 << 1,
    bf_timestamp = 1 << 2,
    bf_prev_id   = 1 << 3,
    bf_miner_tx  = 1 << 4,
    bf_tx_hashes = 1 << 5,
    bf_diffic    = 1 << 6
  };

  test_generator(const MevaCoin::Currency& currency, uint8_t majorVersion = MevaCoin::BLOCK_MAJOR_VERSION_1,
                 uint8_t minorVersion = MevaCoin::BLOCK_MINOR_VERSION_0)
    : m_currency(currency), defaultMajorVersion(majorVersion), defaultMinorVersion(minorVersion) {
  }


  uint8_t defaultMajorVersion;
  uint8_t defaultMinorVersion;

  const MevaCoin::Currency& currency() const { return m_currency; }

  void getBlockchain(std::vector<BlockInfo>& blockchain, const Crypto::Hash& head, size_t n) const;
  void getLastNBlockSizes(std::vector<size_t>& blockSizes, const Crypto::Hash& head, size_t n) const;
  uint64_t getAlreadyGeneratedCoins(const Crypto::Hash& blockId) const;
  uint64_t getAlreadyGeneratedCoins(const MevaCoin::Block& blk) const;

  void addBlock(const MevaCoin::Block& blk, size_t tsxSize, uint64_t fee, std::vector<size_t>& blockSizes,
    uint64_t alreadyGeneratedCoins);
  bool constructBlock(MevaCoin::Block& blk, uint32_t height, const Crypto::Hash& previousBlockHash,
    const MevaCoin::AccountBase& minerAcc, uint64_t timestamp, uint64_t alreadyGeneratedCoins,
    std::vector<size_t>& blockSizes, const std::list<MevaCoin::Transaction>& txList);
  bool constructBlock(MevaCoin::Block& blk, const MevaCoin::AccountBase& minerAcc, uint64_t timestamp);
  bool constructBlock(MevaCoin::Block& blk, const MevaCoin::Block& blkPrev, const MevaCoin::AccountBase& minerAcc,
    const std::list<MevaCoin::Transaction>& txList = std::list<MevaCoin::Transaction>());

  bool constructBlockManually(MevaCoin::Block& blk, const MevaCoin::Block& prevBlock,
    const MevaCoin::AccountBase& minerAcc, int actualParams = bf_none, uint8_t majorVer = 0,
    uint8_t minorVer = 0, uint64_t timestamp = 0, const Crypto::Hash& previousBlockHash = Crypto::Hash(),
    const MevaCoin::difficulty_type& diffic = 1, const MevaCoin::Transaction& baseTransaction = MevaCoin::Transaction(),
    const std::vector<Crypto::Hash>& transactionHashes = std::vector<Crypto::Hash>(), size_t txsSizes = 0, uint64_t fee = 0);
  bool constructBlockManuallyTx(MevaCoin::Block& blk, const MevaCoin::Block& prevBlock,
    const MevaCoin::AccountBase& minerAcc, const std::vector<Crypto::Hash>& transactionHashes, size_t txsSize);
  bool constructMaxSizeBlock(MevaCoin::Block& blk, const MevaCoin::Block& blkPrev,
    const MevaCoin::AccountBase& minerAccount, size_t medianBlockCount = 0,
    const std::list<MevaCoin::Transaction>& txList = std::list<MevaCoin::Transaction>());

private:
  const MevaCoin::Currency& m_currency;
  std::unordered_map<Crypto::Hash, BlockInfo> m_blocksInfo;
};

inline MevaCoin::difficulty_type getTestDifficulty() { return 1; }
void fillNonce(MevaCoin::Block& blk, const MevaCoin::difficulty_type& diffic);

bool constructMinerTxManually(const MevaCoin::Currency& currency, uint8_t blockMajorVersion, uint32_t height, uint64_t alreadyGeneratedCoins,
  const MevaCoin::AccountPublicAddress& minerAddress, MevaCoin::Transaction& tx, uint64_t fee, MevaCoin::KeyPair* pTxKey = 0);
bool constructMinerTxBySize(const MevaCoin::Currency& currency, MevaCoin::Transaction& baseTransaction, uint8_t blockMajorVersion, uint32_t height,
  uint64_t alreadyGeneratedCoins, const MevaCoin::AccountPublicAddress& minerAddress,
  std::vector<size_t>& blockSizes, size_t targetTxSize, size_t targetBlockSize, uint64_t fee = 0);
