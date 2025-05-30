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

#include "LegacyKeysImporter.h"

#include <vector>
#include <system_error>

#include "Common/StringTools.h"

#include "MevaCoinCore/Currency.h"
#include "MevaCoinCore/Account.h"
#include "MevaCoinCore/MevaCoinTools.h"

#include "Serialization/SerializationTools.h"

#include "WalletLegacy/WalletLegacySerializer.h"
#include "WalletLegacy/WalletUserTransactionsCache.h"
#include "Wallet/WalletUtils.h"
#include "Wallet/WalletErrors.h"

using namespace Crypto;

namespace {

struct keys_file_data {
  chacha8_iv iv;
  std::string account_data;

  void serialize(MevaCoin::ISerializer& s) {
    s(iv, "iv");
    s(account_data, "account_data");
  }
};

void loadKeysFromFile(const std::string& filename, const std::string& password, MevaCoin::AccountBase& account) {
  keys_file_data keys_file_data;
  std::string buf;

  if (!Common::loadFileToString(filename, buf)) {
    throw std::system_error(make_error_code(MevaCoin::error::INTERNAL_WALLET_ERROR), "failed to load \"" + filename + '\"');
  }

  if (!MevaCoin::fromBinaryArray(keys_file_data, Common::asBinaryArray(buf))) {
    throw std::system_error(make_error_code(MevaCoin::error::INTERNAL_WALLET_ERROR), "failed to deserialize \"" + filename + '\"');
  }

  chacha8_key key;
  cn_context cn_context;
  generate_chacha8_key(cn_context, password, key);
  std::string account_data;
  account_data.resize(keys_file_data.account_data.size());
  chacha8(keys_file_data.account_data.data(), keys_file_data.account_data.size(), key, keys_file_data.iv, &account_data[0]);

  if (!MevaCoin::loadFromBinaryKeyValue(account, account_data)) {
    throw std::system_error(make_error_code(MevaCoin::error::WRONG_PASSWORD));
  }

  const MevaCoin::AccountKeys& keys = account.getAccountKeys();
  MevaCoin::throwIfKeysMissmatch(keys.viewSecretKey, keys.address.viewPublicKey);
  MevaCoin::throwIfKeysMissmatch(keys.spendSecretKey, keys.address.spendPublicKey);
}

}

namespace MevaCoin {

void importLegacyKeys(const std::string& legacyKeysFilename, const std::string& password, std::ostream& destination) {
  MevaCoin::AccountBase account;

  loadKeysFromFile(legacyKeysFilename, password, account);

  MevaCoin::WalletUserTransactionsCache transactionsCache;
  std::string cache;
  MevaCoin::WalletLegacySerializer importer(account, transactionsCache);
  importer.serialize(destination, password, false, cache);
}

} //namespace MevaCoin
