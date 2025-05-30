// Copyright (c) 2012-2017, The MevaCoin developers, The Bytecoin developers
// Copyright (c) 2018, The Karbo developers
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

#include "BlockchainExplorerDataSerialization.h"

#include <stdexcept>

#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>

#include "MevaCoinCore/MevaCoinSerialization.h"

#include "Serialization/SerializationOverloads.h"

namespace MevaCoin {

enum class SerializationTag : uint8_t { Base = 0xff, Key = 0x2, Multisignature = 0x3, Transaction = 0xcc, Block = 0xbb };

namespace {

struct BinaryVariantTagGetter: boost::static_visitor<uint8_t> {
  uint8_t operator()(const MevaCoin::BaseInputDetails) { return static_cast<uint8_t>(SerializationTag::Base); }
  uint8_t operator()(const MevaCoin::KeyInputDetails) { return static_cast<uint8_t>(SerializationTag::Key); }
  uint8_t operator()(const MevaCoin::MultisignatureInputDetails) { return static_cast<uint8_t>(SerializationTag::Multisignature); }
};

struct VariantSerializer : boost::static_visitor<> {
  VariantSerializer(MevaCoin::ISerializer& serializer, const std::string& name) : s(serializer), name(name) {}

  template <typename T>
  void operator() (T& param) { s(param, name); }

  MevaCoin::ISerializer& s;
  const std::string name;
};

void getVariantValue(MevaCoin::ISerializer& serializer, uint8_t tag, boost::variant<MevaCoin::BaseInputDetails,
                                                                                      MevaCoin::KeyInputDetails,
                                                                                      MevaCoin::MultisignatureInputDetails> in) {
  switch (static_cast<SerializationTag>(tag)) {
  case SerializationTag::Base: {
    MevaCoin::BaseInputDetails v;
    serializer(v, "data");
    in = v;
    break;
  }
  case SerializationTag::Key: {
    MevaCoin::KeyInputDetails v;
    serializer(v, "data");
    in = v;
    break;
  }
  case SerializationTag::Multisignature: {
    MevaCoin::MultisignatureInputDetails v;
    serializer(v, "data");
    in = v;
    break;
  }
  default:
    throw std::runtime_error("Unknown variant tag");
  }
}

template <typename T>
bool serializePod(T& v, Common::StringView name, MevaCoin::ISerializer& serializer) {
  return serializer.binary(&v, sizeof(v), name);
}

} //namespace

//namespace MevaCoin {

void serialize(transactionOutputDetails2& output, ISerializer& serializer) {
  serializer(output.output, "output");
  serializer(output.globalIndex, "globalIndex");
}

void serialize(TransactionOutputReferenceDetails& outputReference, ISerializer& serializer) {
  serializePod(outputReference.transactionHash, "transactionHash", serializer);
  serializer(outputReference.number, "number");
}

void serialize(BaseInputDetails& inputBase, ISerializer& serializer) {
  serializer(inputBase.input, "input");
  serializer(inputBase.amount, "amount");
}

void serialize(KeyInputDetails& inputToKey, ISerializer& serializer) {
  serializer(inputToKey.input, "input");
  serializer(inputToKey.mixin, "mixin");
  serializer(inputToKey.outputs, "outputs");
}

void serialize(MultisignatureInputDetails& inputMultisig, ISerializer& serializer) {
  serializer(inputMultisig.input, "input");
  serializer(inputMultisig.output, "output");
}

void serialize(transactionInputDetails2& input, ISerializer& serializer) {
  if (serializer.type() == ISerializer::OUTPUT) {
    BinaryVariantTagGetter tagGetter;
    uint8_t tag = boost::apply_visitor(tagGetter, input);
    serializer.binary(&tag, sizeof(tag), "type");

    VariantSerializer visitor(serializer, "data");
    boost::apply_visitor(visitor, input);
  } else {
    uint8_t tag;
    serializer.binary(&tag, sizeof(tag), "type");

    getVariantValue(serializer, tag, input);
  }
}

void serialize(TransactionExtraDetails& extra, ISerializer& serializer) {
  serializePod(extra.publicKey, "publicKey", serializer);
  serializer(extra.nonce, "nonce");
  serializeAsBinary(extra.raw, "raw", serializer);
}

void serialize(TransactionExtraDetails2& extra, ISerializer& serializer) {
  serializePod(extra.publicKey, "publicKey", serializer);
  serializer(extra.nonce, "nonce");
  serializeAsBinary(extra.raw, "raw", serializer);
  serializer(extra.size, "size");
}

void serialize(TransactionDetails& transaction, ISerializer& serializer) {
  serializePod(transaction.hash, "hash", serializer);
  serializer(transaction.size, "size");
  serializer(transaction.fee, "fee");
  serializer(transaction.totalInputsAmount, "totalInputsAmount");
  serializer(transaction.totalOutputsAmount, "totalOutputsAmount");
  serializer(transaction.mixin, "mixin");
  serializer(transaction.unlockTime, "unlockTime");
  serializer(transaction.timestamp, "timestamp");
  serializer(transaction.version, "version");
  serializePod(transaction.paymentId, "paymentId", serializer);
  serializer(transaction.inBlockchain, "inBlockchain");
  serializePod(transaction.blockHash, "blockHash", serializer);
  serializer(transaction.blockHeight, "blockIndex");
  serializer(transaction.extra, "extra");
  serializer(transaction.inputs, "inputs");
  serializer(transaction.outputs, "outputs");

  //serializer(transaction.signatures, "signatures");
  if (serializer.type() == ISerializer::OUTPUT) {
    std::vector<std::pair<size_t, Crypto::Signature>> signaturesForSerialization;
    signaturesForSerialization.reserve(transaction.signatures.size());
    size_t ctr = 0;
    for (const auto& signaturesV : transaction.signatures) {
      for (auto signature : signaturesV) {
        signaturesForSerialization.emplace_back(ctr, std::move(signature));
      }
      ++ctr;
    }
    size_t size = transaction.signatures.size();
    serializer(size, "signaturesSize");
    serializer(signaturesForSerialization, "signatures");
  } else {
    size_t size = 0;
    serializer(size, "signaturesSize");
    transaction.signatures.resize(size);

    std::vector<std::pair<size_t, Crypto::Signature>> signaturesForSerialization;
    serializer(signaturesForSerialization, "signatures");

    for (const auto& signatureWithIndex : signaturesForSerialization) {
      transaction.signatures[signatureWithIndex.first].push_back(signatureWithIndex.second);
    }
  }
}

void serialize(BlockDetails& block, ISerializer& serializer) {
  serializer(block.majorVersion, "majorVersion");
  serializer(block.minorVersion, "minorVersion");
  serializer(block.timestamp, "timestamp");
  serializePod(block.prevBlockHash, "prevBlockHash", serializer);
  serializePod(block.proofOfWork, "proofOfWork", serializer);
  serializer(block.nonce, "nonce");
  serializer(block.isOrphaned, "isOrphaned");
  serializer(block.height, "index");
  serializer(block.depth, "depth");
  serializePod(block.hash, "hash", serializer);
  serializer(block.difficulty, "difficulty");
  serializer(block.cumulativeDifficulty, "cumulativeDifficulty");
  serializer(block.reward, "reward");
  serializer(block.baseReward, "baseReward");
  serializer(block.blockSize, "blockSize");
  serializer(block.transactionsCumulativeSize, "transactionsCumulativeSize");
  serializer(block.alreadyGeneratedCoins, "alreadyGeneratedCoins");
  serializer(block.alreadyGeneratedTransactions, "alreadyGeneratedTransactions");
  serializer(block.sizeMedian, "sizeMedian");
  serializer(block.effectiveSizeMedian, "effectiveSizeMedian");
  serializer(block.penalty, "penalty");
  serializer(block.totalFeeAmount, "totalFeeAmount");
  serializer(block.minerSignature, "minerSignature");
  serializer(block.transactions, "transactions");
}

} //namespace MevaCoin
