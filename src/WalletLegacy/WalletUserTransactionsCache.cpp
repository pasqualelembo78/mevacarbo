// Copyright (c) 2012-2016, The MevaCoin developers, The Bytecoin developers
// Copyright (c) 2018, Karbo developers
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

#include "IWalletLegacy.h"
#include "Wallet/WalletErrors.h"
#include "MevaCoinCore/TransactionExtra.h"
#include "WalletLegacy/WalletUserTransactionsCache.h"
#include "WalletLegacy/WalletLegacySerialization.h"
#include "WalletLegacy/WalletUtils.h"

#include "Serialization/ISerializer.h"
#include "Serialization/SerializationOverloads.h"
#include <algorithm>

using namespace Crypto;

namespace MevaCoin {

WalletUserTransactionsCache::WalletUserTransactionsCache(uint64_t mempoolTxLiveTime) : m_unconfirmedTransactions(mempoolTxLiveTime) {
}

bool WalletUserTransactionsCache::serialize(MevaCoin::ISerializer& s) {
  if (s.type() == MevaCoin::ISerializer::INPUT) {
    s(m_transactions, "transactions");
    s(m_transfers, "transfers");
    s(m_unconfirmedTransactions, "unconfirmed");

    updateUnconfirmedTransactions();
    deleteOutdatedTransactions();
	rebuildPaymentsIndex();
  } else {
    UserTransactions txsToSave;
    UserTransfers transfersToSave;

    getGoodItems(txsToSave, transfersToSave);
    s(txsToSave, "transactions");
    s(transfersToSave, "transfers");
    s(m_unconfirmedTransactions, "unconfirmed");
  }

  return true;
}

bool paymentIdIsSet(const PaymentId& paymentId) {
  return paymentId != NULL_HASH;
}

bool canInsertTransactionToIndex(const WalletLegacyTransaction& info) {
  return info.state == WalletLegacyTransactionState::Active && info.blockHeight != WALLET_LEGACY_UNCONFIRMED_TRANSACTION_HEIGHT &&
      info.totalAmount > 0 && !info.extra.empty();
}

void WalletUserTransactionsCache::pushToPaymentsIndex(const PaymentId& paymentId, Offset distance) {
  m_paymentsIndex[paymentId].push_back(distance);
}

void WalletUserTransactionsCache::popFromPaymentsIndex(const PaymentId& paymentId, Offset distance) {
  auto it = m_paymentsIndex.find(paymentId);
  if (it == m_paymentsIndex.end()) {
    return;
  }

  auto toErase = std::lower_bound(it->second.begin(), it->second.end(), distance);
  if (toErase == it->second.end() || *toErase != distance) {
    return;
  }

  it->second.erase(toErase);
}

void WalletUserTransactionsCache::rebuildPaymentsIndex() {
  auto begin = std::begin(m_transactions);
  auto end = std::end(m_transactions);
  std::vector<uint8_t> extra;
  for (auto it = begin; it != end; ++it) {
    PaymentId paymentId;
    extra.insert(extra.begin(), it->extra.begin(), it->extra.end());
    if (canInsertTransactionToIndex(*it) && getPaymentIdFromTxExtra(extra, paymentId)) {
      pushToPaymentsIndex(paymentId, std::distance(begin, it));
    }
    extra.clear();
  }
}

uint64_t WalletUserTransactionsCache::unconfirmedTransactionsAmount() const {
  return m_unconfirmedTransactions.countUnconfirmedTransactionsAmount();
}

uint64_t WalletUserTransactionsCache::unconfrimedOutsAmount() const {
  return m_unconfirmedTransactions.countUnconfirmedOutsAmount();
}

size_t WalletUserTransactionsCache::getTransactionCount() const {
  return m_transactions.size();
}

size_t WalletUserTransactionsCache::getTransferCount() const {
  return m_transfers.size();
}

TransactionId WalletUserTransactionsCache::addNewTransaction(
  uint64_t amount, uint64_t fee, const std::string& extra, const std::vector<WalletLegacyTransfer>& transfers, uint64_t unlockTime) {
  
  WalletLegacyTransaction transaction;

  transaction.firstTransferId = insertTransfers(transfers);
  transaction.transferCount = transfers.size();
  transaction.totalAmount = -static_cast<int64_t>(amount);
  transaction.fee = fee;
  transaction.sentTime = time(nullptr);
  transaction.isCoinbase = false;
  transaction.timestamp = 0;
  transaction.extra = extra;
  transaction.blockHeight = WALLET_LEGACY_UNCONFIRMED_TRANSACTION_HEIGHT;
  transaction.state = WalletLegacyTransactionState::Sending;
  transaction.unlockTime = unlockTime;
  transaction.secretKey = NULL_SECRET_KEY;

  return insertTransaction(std::move(transaction));
}

void WalletUserTransactionsCache::updateTransaction(
  TransactionId transactionId, const MevaCoin::Transaction& tx, uint64_t amount, const std::list<TransactionOutputInformation>& usedOutputs, Crypto::SecretKey& tx_key) {
  // update extra field from created transaction
  auto& txInfo = m_transactions.at(transactionId);
  txInfo.extra.assign(tx.extra.begin(), tx.extra.end());
  txInfo.secretKey = tx_key;
  m_unconfirmedTransactions.add(tx, transactionId, amount, usedOutputs, tx_key);
}

void WalletUserTransactionsCache::updateTransactionSendingState(TransactionId transactionId, std::error_code ec) {
  auto& txInfo = m_transactions.at(transactionId);
  if (ec) {
    txInfo.state = ec.value() == error::TX_CANCELLED ? WalletLegacyTransactionState::Cancelled : WalletLegacyTransactionState::Failed;
    m_unconfirmedTransactions.erase(txInfo.hash);
  } else {
    txInfo.sentTime = time(nullptr); // update sending time
    txInfo.state = WalletLegacyTransactionState::Active;
  }
}

std::shared_ptr<WalletLegacyEvent> WalletUserTransactionsCache::onTransactionUpdated(const TransactionInformation& txInfo, int64_t txBalance) {
  std::shared_ptr<WalletLegacyEvent> event;

  TransactionId id = MevaCoin::WALLET_LEGACY_INVALID_TRANSACTION_ID;

  if (!m_unconfirmedTransactions.findTransactionId(txInfo.transactionHash, id)) {
    id = findTransactionByHash(txInfo.transactionHash);
  }
  m_unconfirmedTransactions.erase(txInfo.transactionHash);

  bool isCoinbase = txInfo.totalAmountIn == 0;

  if (id == MevaCoin::WALLET_LEGACY_INVALID_TRANSACTION_ID) {
    WalletLegacyTransaction transaction;
    transaction.firstTransferId = WALLET_LEGACY_INVALID_TRANSFER_ID;
    transaction.transferCount = 0;
    transaction.totalAmount = txBalance;
    transaction.fee = isCoinbase ? 0 : txInfo.totalAmountIn - txInfo.totalAmountOut;
    transaction.sentTime = 0;
    transaction.hash = txInfo.transactionHash;
    transaction.blockHeight = txInfo.blockHeight;
    transaction.isCoinbase = isCoinbase;
    transaction.timestamp = txInfo.timestamp;
    transaction.extra.assign(txInfo.extra.begin(), txInfo.extra.end());
    transaction.state = WalletLegacyTransactionState::Active;
    transaction.unlockTime = txInfo.unlockTime;
    transaction.secretKey = NULL_SECRET_KEY;

    id = insertTransaction(std::move(transaction));
    // notification event
    event = std::make_shared<WalletExternalTransactionCreatedEvent>(id);
  } else {
    WalletLegacyTransaction& tr = getTransaction(id);
    tr.blockHeight = txInfo.blockHeight;
    tr.timestamp = txInfo.timestamp;
    tr.state = WalletLegacyTransactionState::Active;
    // notification event
    event = std::make_shared<WalletTransactionUpdatedEvent>(id);
  }

  return event;
}

std::shared_ptr<WalletLegacyEvent> WalletUserTransactionsCache::onTransactionDeleted(const Hash& transactionHash) {
  TransactionId id = MevaCoin::WALLET_LEGACY_INVALID_TRANSACTION_ID;
  if (m_unconfirmedTransactions.findTransactionId(transactionHash, id)) {
    m_unconfirmedTransactions.erase(transactionHash);
    // LOG_ERROR("Unconfirmed transaction is deleted: id = " << id << ", hash = " << transactionHash);
    assert(false);
  } else {
    id = findTransactionByHash(transactionHash);
  }

  std::shared_ptr<WalletLegacyEvent> event;
  if (id != MevaCoin::WALLET_LEGACY_INVALID_TRANSACTION_ID) {
    WalletLegacyTransaction& tr = getTransaction(id);
	std::vector<uint8_t> extra(tr.extra.begin(), tr.extra.end());
    PaymentId paymentId;
    if (getPaymentIdFromTxExtra(extra, paymentId)) {
      popFromPaymentsIndex(paymentId, id);
    }

    tr.blockHeight = WALLET_LEGACY_UNCONFIRMED_TRANSACTION_HEIGHT;
    tr.timestamp = 0;
    tr.state = WalletLegacyTransactionState::Deleted;

    event = std::make_shared<WalletTransactionUpdatedEvent>(id);
  } else {
    // LOG_ERROR("Transaction wasn't found: " << transactionHash);
    assert(false);
  }

  return event;
}

TransactionId WalletUserTransactionsCache::findTransactionByTransferId(TransferId transferId) const
{
  TransactionId id;
  for (id = 0; id < m_transactions.size(); ++id) {
    const WalletLegacyTransaction& tx = m_transactions[id];

    if (tx.firstTransferId == WALLET_LEGACY_INVALID_TRANSFER_ID || tx.transferCount == 0)
      continue;

    if (transferId >= tx.firstTransferId && transferId < (tx.firstTransferId + tx.transferCount))
      break;
  }

  if (id == m_transactions.size())
    return WALLET_LEGACY_INVALID_TRANSACTION_ID;

  return id;
}

std::vector<Payments> WalletUserTransactionsCache::getTransactionsByPaymentIds(const std::vector<PaymentId>& paymentIds) const {
  std::vector<Payments> payments(paymentIds.size());
  auto payment = payments.begin();
  for (auto& key : paymentIds) {
    payment->paymentId = key;
    auto it = m_paymentsIndex.find(key);
    if (it != m_paymentsIndex.end()) {
      std::transform(it->second.begin(), it->second.end(), std::back_inserter(payment->transactions),
      [this](decltype(it->second)::value_type val) {
        assert(val < m_transactions.size());
        return m_transactions[val];
      });
    }

    ++payment;
  }

  return payments;
}

bool WalletUserTransactionsCache::getTransaction(TransactionId transactionId, WalletLegacyTransaction& transaction) const
{
  if (transactionId >= m_transactions.size())
    return false;

  transaction = m_transactions[transactionId];

  return true;
}

bool WalletUserTransactionsCache::getTransfer(TransferId transferId, WalletLegacyTransfer& transfer) const
{
  if (transferId >= m_transfers.size())
    return false;

  transfer = m_transfers[transferId];

  return true;
}

TransactionId WalletUserTransactionsCache::insertTransaction(WalletLegacyTransaction&& Transaction) {
  m_transactions.emplace_back(std::move(Transaction));
  return m_transactions.size() - 1;
}

TransactionId WalletUserTransactionsCache::findTransactionByHash(const Hash& hash) {
  auto it = std::find_if(m_transactions.begin(), m_transactions.end(), [&hash](const WalletLegacyTransaction& tx) { return tx.hash == hash; });

  if (it == m_transactions.end())
    return MevaCoin::WALLET_LEGACY_INVALID_TRANSACTION_ID;

  return std::distance(m_transactions.begin(), it);
}

bool WalletUserTransactionsCache::isUsed(const TransactionOutputInformation& out) const {
  return m_unconfirmedTransactions.isUsed(out);
}

WalletLegacyTransaction& WalletUserTransactionsCache::getTransaction(TransactionId transactionId) {
  return m_transactions.at(transactionId);
}

void WalletUserTransactionsCache::getGoodItems(UserTransactions& transactions, UserTransfers& transfers) {
  size_t offset = 0;

  for (size_t txId = 0; txId < m_transactions.size(); ++txId) {
    bool isGood =
      m_transactions[txId].state != WalletLegacyTransactionState::Cancelled &&
      m_transactions[txId].state != WalletLegacyTransactionState::Failed;

    if (isGood) {
      getGoodTransaction(txId, offset, transactions, transfers);
    } else {
      const WalletLegacyTransaction& t = m_transactions[txId];
      offset += t.firstTransferId != WALLET_LEGACY_INVALID_TRANSFER_ID ? t.transferCount : 0;
    }
  }
}

void WalletUserTransactionsCache::getGoodTransaction(TransactionId txId, size_t offset, UserTransactions& transactions, UserTransfers& transfers) {
  transactions.push_back(m_transactions[txId]);
  WalletLegacyTransaction& tx = transactions.back();

  if (tx.firstTransferId == WALLET_LEGACY_INVALID_TRANSFER_ID) {
    return;
  }

  UserTransfers::const_iterator first = m_transfers.begin() + tx.firstTransferId;
  UserTransfers::const_iterator last = first + tx.transferCount;

  tx.firstTransferId -= offset;

  std::copy(first, last, std::back_inserter(transfers));
}

void WalletUserTransactionsCache::getTransfersByTx(TransactionId id, UserTransfers& transfers) {
  const WalletLegacyTransaction& tx = m_transactions[id];

  if (tx.firstTransferId != WALLET_LEGACY_INVALID_TRANSFER_ID) {
    UserTransfers::const_iterator first = m_transfers.begin() + tx.firstTransferId;
    UserTransfers::const_iterator last = first + tx.transferCount;
    std::copy(first, last, std::back_inserter(transfers));
  }
}

TransferId WalletUserTransactionsCache::insertTransfers(const std::vector<WalletLegacyTransfer>& transfers) {
  std::copy(transfers.begin(), transfers.end(), std::back_inserter(m_transfers));
  return m_transfers.size() - transfers.size();
}

void WalletUserTransactionsCache::updateUnconfirmedTransactions() {
  for (TransactionId id = 0; id < m_transactions.size(); ++id) {
    if (m_transactions[id].blockHeight == WALLET_LEGACY_UNCONFIRMED_TRANSACTION_HEIGHT) {
      m_unconfirmedTransactions.updateTransactionId(m_transactions[id].hash, id);
    }
  }
}

WalletLegacyTransfer& WalletUserTransactionsCache::getTransfer(TransferId transferId) {
  return m_transfers.at(transferId);
}
  
void WalletUserTransactionsCache::reset() {
  m_transactions.clear();
  m_transfers.clear();
  m_unconfirmedTransactions.reset();
}

std::vector<TransactionId> WalletUserTransactionsCache::deleteOutdatedTransactions() {
  auto deletedTransactions = m_unconfirmedTransactions.deleteOutdatedTransactions();

  for (auto id: deletedTransactions) {
    assert(id < m_transactions.size());
    m_transactions[id].state = WalletLegacyTransactionState::Deleted;
  }

  return deletedTransactions;
}

} //namespace MevaCoin
