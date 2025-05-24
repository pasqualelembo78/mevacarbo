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
#include <unordered_map>

#include "Common/ObserverManager.h"
#include "MevaCoinCore/MevaCoinBasic.h"
#include "MevaCoinCore/ICore.h"
#include "MevaCoinCore/ICoreObserver.h"
#include "MevaCoinProtocol/MevaCoinProtocolDefinitions.h"
#include "Rpc/CoreRpcServerCommandsDefinitions.h"

class ICoreStub: public MevaCoin::ICore {
public:
  ICoreStub();
  ICoreStub(const MevaCoin::Block& genesisBlock);

  virtual bool addObserver(MevaCoin::ICoreObserver* observer) override;
  virtual bool removeObserver(MevaCoin::ICoreObserver* observer) override;
  virtual void get_blockchain_top(uint32_t& height, Crypto::Hash& top_id) override;
  virtual std::vector<Crypto::Hash> findBlockchainSupplement(const std::vector<Crypto::Hash>& remoteBlockIds, size_t maxCount,
    uint32_t& totalBlockCount, uint32_t& startBlockIndex) override;
  virtual bool get_random_outs_for_amounts(const MevaCoin::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_request& req,
      MevaCoin::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_response& res) override;
  virtual bool get_tx_outputs_gindexs(const Crypto::Hash& tx_id, std::vector<uint32_t>& indexs) override;
  virtual MevaCoin::i_mevacoin_protocol* get_protocol() override;
  virtual bool handle_incoming_tx(MevaCoin::BinaryArray const& tx_blob, MevaCoin::tx_verification_context& tvc, bool keeped_by_block) override;
  virtual std::vector<MevaCoin::Transaction> getPoolTransactions() override;
  virtual bool getPoolChanges(const Crypto::Hash& tailBlockId, const std::vector<Crypto::Hash>& knownTxsIds,
                              std::vector<MevaCoin::Transaction>& addedTxs, std::vector<Crypto::Hash>& deletedTxsIds) override;
  virtual bool getPoolChangesLite(const Crypto::Hash& tailBlockId, const std::vector<Crypto::Hash>& knownTxsIds,
          std::vector<MevaCoin::TransactionPrefixInfo>& addedTxs, std::vector<Crypto::Hash>& deletedTxsIds) override;
  virtual void getPoolChanges(const std::vector<Crypto::Hash>& knownTxsIds, std::vector<MevaCoin::Transaction>& addedTxs,
                              std::vector<Crypto::Hash>& deletedTxsIds) override;
  virtual bool queryBlocks(const std::vector<Crypto::Hash>& block_ids, uint64_t timestamp,
    uint32_t& start_height, uint32_t& current_height, uint32_t& full_offset, std::vector<MevaCoin::BlockFullInfo>& entries) override;
  virtual bool queryBlocksLite(const std::vector<Crypto::Hash>& block_ids, uint64_t timestamp,
    uint32_t& start_height, uint32_t& current_height, uint32_t& full_offset, std::vector<MevaCoin::BlockShortInfo>& entries) override;

  virtual bool have_block(const Crypto::Hash& id) override;
  std::vector<Crypto::Hash> buildSparseChain() override;
  std::vector<Crypto::Hash> buildSparseChain(const Crypto::Hash& startBlockId) override;
  virtual bool get_stat_info(MevaCoin::core_stat_info& st_inf) override { return false; }
  virtual bool on_idle() override { return false; }
  virtual void pause_mining() override {}
  virtual void update_block_template_and_resume_mining() override {}
  virtual bool handle_incoming_block_blob(const MevaCoin::BinaryArray& block_blob, MevaCoin::block_verification_context& bvc, bool control_miner, bool relay_block) override { return false; }
  virtual bool handle_get_objects(MevaCoin::NOTIFY_REQUEST_GET_OBJECTS::request& arg, MevaCoin::NOTIFY_RESPONSE_GET_OBJECTS::request& rsp) override { return false; }
  virtual void on_synchronized() override {}
  virtual bool getOutByMSigGIndex(uint64_t amount, uint64_t gindex, MevaCoin::MultisignatureOutput& out) override { return true; }
  virtual size_t addChain(const std::vector<const MevaCoin::IBlock*>& chain) override;

  virtual Crypto::Hash getBlockIdByHeight(uint32_t height) override;
  virtual bool getBlockByHash(const Crypto::Hash &h, MevaCoin::Block &blk) override;
  virtual bool getBlockHeight(const Crypto::Hash& blockId, uint32_t& blockHeight) override;
  virtual void getTransactions(const std::vector<Crypto::Hash>& txs_ids, std::list<MevaCoin::Transaction>& txs, std::list<Crypto::Hash>& missed_txs, bool checkTxPool = false) override;
  virtual bool getBackwardBlocksSizes(uint32_t fromHeight, std::vector<size_t>& sizes, size_t count) override;
  virtual bool getBlockSize(const Crypto::Hash& hash, size_t& size) override;
  virtual bool getAlreadyGeneratedCoins(const Crypto::Hash& hash, uint64_t& generatedCoins) override;
  virtual bool getBlockReward(uint8_t blockMajorVersion, size_t medianSize, size_t currentBlockSize, uint64_t alreadyGeneratedCoins, uint64_t fee,
    uint64_t& reward, int64_t& emissionChange) override;
  virtual bool scanOutputkeysForIndices(const MevaCoin::KeyInput& txInToKey, std::list<std::pair<Crypto::Hash, size_t>>& outputReferences) override;
  virtual bool getBlockDifficulty(uint32_t height, MevaCoin::difficulty_type& difficulty) override;
  virtual bool getBlockContainingTx(const Crypto::Hash& txId, Crypto::Hash& blockId, uint32_t& blockHeight) override;
  virtual bool getMultisigOutputReference(const MevaCoin::MultisignatureInput& txInMultisig, std::pair<Crypto::Hash, size_t>& outputReference) override;

  virtual bool getGeneratedTransactionsNumber(uint32_t height, uint64_t& generatedTransactions) override;
  virtual bool getOrphanBlocksByHeight(uint32_t height, std::vector<MevaCoin::Block>& blocks) override;
  virtual bool getBlocksByTimestamp(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t blocksNumberLimit, std::vector<MevaCoin::Block>& blocks, uint32_t& blocksNumberWithinTimestamps) override;
  virtual bool getPoolTransactionsByTimestamp(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t transactionsNumberLimit, std::vector<MevaCoin::Transaction>& transactions, uint64_t& transactionsNumberWithinTimestamps) override;
  virtual bool getTransactionsByPaymentId(const Crypto::Hash& paymentId, std::vector<MevaCoin::Transaction>& transactions) override;
  virtual std::unique_ptr<MevaCoin::IBlock> getBlock(const Crypto::Hash& blockId) override;
  virtual bool handleIncomingTransaction(const MevaCoin::Transaction& tx, const Crypto::Hash& txHash, size_t blobSize, MevaCoin::tx_verification_context& tvc, bool keptByBlock, uint32_t height) override;
  virtual std::error_code executeLocked(const std::function<std::error_code()>& func) override;

  virtual bool addMessageQueue(MevaCoin::MessageQueue<MevaCoin::BlockchainMessage>& messageQueuePtr) override;
  virtual bool removeMessageQueue(MevaCoin::MessageQueue<MevaCoin::BlockchainMessage>& messageQueuePtr) override;
  
  virtual uint64_t getMinimalFeeForHeight(uint32_t height) override;
  virtual uint64_t getMinimalFee() override;
  virtual uint8_t getBlockMajorVersionForHeight(uint32_t height) override;
  virtual uint8_t getCurrentBlockMajorVersion() override;

  void set_blockchain_top(uint32_t height, const Crypto::Hash& top_id);
  void set_outputs_gindexs(const std::vector<uint32_t>& indexs, bool result);
  void set_random_outs(const MevaCoin::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_response& resp, bool result);

  void addBlock(const MevaCoin::Block& block);
  void addTransaction(const MevaCoin::Transaction& tx);

  void setPoolTxVerificationResult(bool result);
  void setPoolChangesResult(bool result);

private:
  uint32_t topHeight;
  Crypto::Hash topId;

  std::vector<uint32_t> globalIndices;
  bool globalIndicesResult;

  MevaCoin::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_response randomOuts;
  bool randomOutsResult;

  std::unordered_map<Crypto::Hash, MevaCoin::Block> blocks;
  std::unordered_map<uint32_t, Crypto::Hash> blockHashByHeightIndex;
  std::unordered_map<Crypto::Hash, Crypto::Hash> blockHashByTxHashIndex;

  std::unordered_map<Crypto::Hash, MevaCoin::Transaction> transactions;
  std::unordered_map<Crypto::Hash, MevaCoin::Transaction> transactionPool;
  bool poolTxVerificationResult;
  bool poolChangesResult;
  Tools::ObserverManager<MevaCoin::ICoreObserver> m_observerManager;
};
