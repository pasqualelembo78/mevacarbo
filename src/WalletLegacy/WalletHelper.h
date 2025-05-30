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

#include <future>
#include <map>
#include <mutex>

#include "crypto/hash.h"
#include "IWalletLegacy.h"

namespace MevaCoin {
namespace WalletHelper {

class SaveWalletResultObserver : public MevaCoin::IWalletLegacyObserver {
public:
  std::promise<std::error_code> saveResult;
  virtual void saveCompleted(std::error_code result) override { saveResult.set_value(result); }
};

class InitWalletResultObserver : public MevaCoin::IWalletLegacyObserver {
public:
  std::promise<std::error_code> initResult;
  virtual void initCompleted(std::error_code result) override { initResult.set_value(result); }
};

class SendCompleteResultObserver : public MevaCoin::IWalletLegacyObserver {
public:
  virtual void sendTransactionCompleted(MevaCoin::TransactionId transactionId, std::error_code result) override;
  std::error_code wait(MevaCoin::TransactionId transactionId);

private:
  std::mutex m_mutex;
  std::condition_variable m_condition;
  std::map<MevaCoin::TransactionId, std::error_code> m_finishedTransactions;
  std::error_code m_result;
};

class IWalletRemoveObserverGuard {
public:
  IWalletRemoveObserverGuard(MevaCoin::IWalletLegacy& wallet, MevaCoin::IWalletLegacyObserver& observer);
  ~IWalletRemoveObserverGuard();

  void removeObserver();

private:
  MevaCoin::IWalletLegacy& m_wallet;
  MevaCoin::IWalletLegacyObserver& m_observer;
  bool m_removed;
};

void prepareFileNames(const std::string& file_path, std::string& keys_file, std::string& wallet_file);
bool storeWallet(MevaCoin::IWalletLegacy& wallet, const std::string& walletFilename);

}
}
