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

#include <vector>
#include <unordered_map>

#include "MevaCoinCore/Account.h"
#include "MevaCoinCore/MevaCoinBasic.h"
#include "MevaCoinCore/Currency.h"
#include "MevaCoinCore/BlockchainIndices.h"
#include "crypto/hash.h"

#include "../TestGenerator/TestGenerator.h"

class TestBlockchainGenerator
{
public:
  TestBlockchainGenerator(const MevaCoin::Currency& currency);

  //TODO: get rid of this method
  std::vector<MevaCoin::Block>& getBlockchain();
  std::vector<MevaCoin::Block> getBlockchainCopy();
  void generateEmptyBlocks(size_t count);
  bool getBlockRewardForAddress(const MevaCoin::AccountPublicAddress& address);
  bool generateTransactionsInOneBlock(const MevaCoin::AccountPublicAddress& address, size_t n);
  bool getSingleOutputTransaction(const MevaCoin::AccountPublicAddress& address, uint64_t amount);
  void addTxToBlockchain(const MevaCoin::Transaction& transaction);
  bool getTransactionByHash(const Crypto::Hash& hash, MevaCoin::Transaction& tx, bool checkTxPool = false);
  const MevaCoin::AccountBase& getMinerAccount() const;
  bool generateFromBaseTx(const MevaCoin::AccountBase& address);

  void putTxToPool(const MevaCoin::Transaction& tx);
  void getPoolSymmetricDifference(std::vector<Crypto::Hash>&& known_pool_tx_ids, Crypto::Hash known_block_id, bool& is_bc_actual,
    std::vector<MevaCoin::Transaction>& new_txs, std::vector<Crypto::Hash>& deleted_tx_ids);
  void putTxPoolToBlockchain();
  void clearTxPool();

  void cutBlockchain(uint32_t height);

  bool addOrphan(const Crypto::Hash& hash, uint32_t height);
  bool getGeneratedTransactionsNumber(uint32_t height, uint64_t& generatedTransactions);
  bool getOrphanBlockIdsByHeight(uint32_t height, std::vector<Crypto::Hash>& blockHashes);
  bool getBlockIdsByTimestamp(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t blocksNumberLimit, std::vector<Crypto::Hash>& hashes, uint32_t& blocksNumberWithinTimestamps);
  bool getPoolTransactionIdsByTimestamp(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t transactionsNumberLimit, std::vector<Crypto::Hash>& hashes, uint64_t& transactionsNumberWithinTimestamps);
  bool getTransactionIdsByPaymentId(const Crypto::Hash& paymentId, std::vector<Crypto::Hash>& transactionHashes);

  bool getTransactionGlobalIndexesByHash(const Crypto::Hash& transactionHash, std::vector<uint32_t>& globalIndexes);
  bool getMultisignatureOutputByGlobalIndex(uint64_t amount, uint32_t globalIndex, MevaCoin::MultisignatureOutput& out);
  void setMinerAccount(const MevaCoin::AccountBase& account);

private:
  struct MultisignatureOutEntry {
    Crypto::Hash transactionHash;
    uint16_t indexOut;
  };

  struct KeyOutEntry {
    Crypto::Hash transactionHash;
    uint16_t indexOut;
  };
  
  void addGenesisBlock();
  void addMiningBlock();

  const MevaCoin::Currency& m_currency;
  test_generator generator;
  MevaCoin::AccountBase miner_acc;
  std::vector<MevaCoin::Block> m_blockchain;
  std::unordered_map<Crypto::Hash, MevaCoin::Transaction> m_txs;
  std::unordered_map<Crypto::Hash, std::vector<uint32_t>> transactionGlobalOuts;
  std::unordered_map<uint64_t, std::vector<MultisignatureOutEntry>> multisignatureOutsIndex;
  std::unordered_map<uint64_t, std::vector<KeyOutEntry>> keyOutsIndex;

  std::unordered_map<Crypto::Hash, MevaCoin::Transaction> m_txPool;
  mutable std::mutex m_mutex;

  MevaCoin::PaymentIdIndex m_paymentIdIndex;
  MevaCoin::TimestampTransactionsIndex m_timestampIndex;
  MevaCoin::GeneratedTransactionsIndex m_generatedTransactionsIndex;
  MevaCoin::OrphanBlocksIndex m_orthanBlocksIndex;

  void addToBlockchain(const MevaCoin::Transaction& tx);
  void addToBlockchain(const std::vector<MevaCoin::Transaction>& txs);
  void addToBlockchain(const std::vector<MevaCoin::Transaction>& txs, const MevaCoin::AccountBase& minerAddress);
  void addTx(const MevaCoin::Transaction& tx);

  bool doGenerateTransactionsInOneBlock(MevaCoin::AccountPublicAddress const &address, size_t n);
};
