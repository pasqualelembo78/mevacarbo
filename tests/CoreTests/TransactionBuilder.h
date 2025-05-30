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

#include "MevaCoinCore/Account.h"
#include "MevaCoinCore/MevaCoinFormatUtils.h"
#include "MevaCoinCore/Currency.h"

class TransactionBuilder {
public:

  typedef std::vector<MevaCoin::AccountKeys> KeysVector;
  typedef std::vector<Crypto::Signature> SignatureVector;
  typedef std::vector<SignatureVector> SignatureMultivector;

  struct MultisignatureSource {
    MevaCoin::MultisignatureInput input;
    KeysVector keys;
    Crypto::PublicKey srcTxPubKey;
    size_t srcOutputIndex;
  };

  TransactionBuilder(const MevaCoin::Currency& currency, uint64_t unlockTime = 0);

  // regenerate transaction keys
  TransactionBuilder& newTxKeys();
  TransactionBuilder& setTxKeys(const MevaCoin::KeyPair& txKeys);

  // inputs
  TransactionBuilder& setInput(const std::vector<MevaCoin::TransactionSourceEntry>& sources, const MevaCoin::AccountKeys& senderKeys);
  TransactionBuilder& addMultisignatureInput(const MultisignatureSource& source);

  // outputs
  TransactionBuilder& setOutput(const std::vector<MevaCoin::TransactionDestinationEntry>& destinations);
  TransactionBuilder& addOutput(const MevaCoin::TransactionDestinationEntry& dest);
  TransactionBuilder& addMultisignatureOut(uint64_t amount, const KeysVector& keys, uint32_t required);

  MevaCoin::Transaction build() const;

  std::vector<MevaCoin::TransactionSourceEntry> m_sources;
  std::vector<MevaCoin::TransactionDestinationEntry> m_destinations;

private:

  void fillInputs(MevaCoin::Transaction& tx, std::vector<MevaCoin::KeyPair>& contexts) const;
  void fillOutputs(MevaCoin::Transaction& tx) const;
  void signSources(const Crypto::Hash& prefixHash, const std::vector<MevaCoin::KeyPair>& contexts, MevaCoin::Transaction& tx) const;

  struct MultisignatureDestination {
    uint64_t amount;
    uint32_t requiredSignatures;
    KeysVector keys;
  };

  MevaCoin::AccountKeys m_senderKeys;

  std::vector<MultisignatureSource> m_msigSources;
  std::vector<MultisignatureDestination> m_msigDestinations;

  size_t m_version;
  uint64_t m_unlockTime;
  MevaCoin::KeyPair m_txKey;
  const MevaCoin::Currency& m_currency;
};
