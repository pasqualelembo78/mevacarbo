// Copyright (c) 2012-2016, The MevaCoin developers, The Bytecoin developers
// Copyright (c) 2016-2019, Karbo developers
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
#include <ostream>
#include <istream>

#include "crypto/hash.h"
#include "crypto/chacha8.h"

namespace MevaCoin {
class AccountBase;
class ISerializer;
}

namespace MevaCoin {

class WalletUserTransactionsCache;

class WalletLegacySerializer {
public:
  WalletLegacySerializer(MevaCoin::AccountBase& account, WalletUserTransactionsCache& transactionsCache);

  void serialize(std::ostream& stream, const std::string& password, bool saveDetailed, const std::string& cache);
  void deserialize(std::istream& stream, const std::string& password, std::string& cache);
  bool deserialize(std::istream& stream, const std::string& password);

private:
  void saveKeys(MevaCoin::ISerializer& serializer);
  void loadKeys(MevaCoin::ISerializer& serializer);

  Crypto::chacha8_iv encrypt(const std::string& plain, const std::string& password, std::string& cipher);
  void decrypt(const std::string& cipher, std::string& plain, Crypto::chacha8_iv iv, const std::string& password);

  MevaCoin::AccountBase& account;
  WalletUserTransactionsCache& transactionsCache;
  const uint32_t walletSerializationVersion;
};

extern uint32_t WALLET_LEGACY_SERIALIZATION_VERSION;

} //namespace MevaCoin
