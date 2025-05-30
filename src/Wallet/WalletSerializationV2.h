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

#include "Common/IInputStream.h"
#include "Common/IOutputStream.h"
#include "Serialization/ISerializer.h"
#include "Transfers/TransfersSynchronizer.h"
#include "Wallet/WalletIndices.h"

namespace MevaCoin {

class WalletSerializerV2 {
public:
  WalletSerializerV2(
    ITransfersObserver& transfersObserver,
    Crypto::PublicKey& viewPublicKey,
    Crypto::SecretKey& viewSecretKey,
    uint64_t& actualBalance,
    uint64_t& pendingBalance,
    WalletsContainer& walletsContainer,
    TransfersSyncronizer& synchronizer,
    UnlockTransactionJobs& unlockTransactions,
    WalletTransactions& transactions,
    WalletTransfers& transfers,
    UncommitedTransactions& uncommitedTransactions,
    std::string& extra,
    uint32_t transactionSoftLockTime
  );

  void load(Common::IInputStream& source, uint8_t version);
  void save(Common::IOutputStream& destination, WalletSaveLevel saveLevel);

  std::unordered_set<Crypto::PublicKey>& addedKeys();
  std::unordered_set<Crypto::PublicKey>& deletedKeys();

  static const uint8_t MIN_VERSION = 6;
  static const uint8_t SERIALIZATION_VERSION = 6;

private:
  void loadKeyListAndBalances(MevaCoin::ISerializer& serializer, bool saveCache);
  void saveKeyListAndBalances(MevaCoin::ISerializer& serializer, bool saveCache);
    
  void loadTransactions(MevaCoin::ISerializer& serializer);
  void saveTransactions(MevaCoin::ISerializer& serializer);

  void loadTransfers(MevaCoin::ISerializer& serializer);
  void saveTransfers(MevaCoin::ISerializer& serializer);

  void loadTransfersSynchronizer(MevaCoin::ISerializer& serializer);
  void saveTransfersSynchronizer(MevaCoin::ISerializer& serializer);

  void loadUnlockTransactionsJobs(MevaCoin::ISerializer& serializer);
  void saveUnlockTransactionsJobs(MevaCoin::ISerializer& serializer);

  ITransfersObserver& m_transfersObserver;
  uint64_t& m_actualBalance;
  uint64_t& m_pendingBalance;
  WalletsContainer& m_walletsContainer;
  TransfersSyncronizer& m_synchronizer;
  UnlockTransactionJobs& m_unlockTransactions;
  WalletTransactions& m_transactions;
  WalletTransfers& m_transfers;
  UncommitedTransactions& m_uncommitedTransactions;
  std::string& m_extra;
  uint32_t m_transactionSoftLockTime;

  std::unordered_set<Crypto::PublicKey> m_addedKeys;
  std::unordered_set<Crypto::PublicKey> m_deletedKeys;
};

} //namespace MevaCoin
