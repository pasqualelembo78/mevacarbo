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

#include "ITransaction.h"

#include <numeric>
#include <system_error>

#include "MevaCoinCore/MevaCoinBasic.h"
#include "MevaCoinCore/TransactionApiExtra.h"
#include "TransactionUtils.h"
#include "MevaCoinCore/MevaCoinTools.h"

using namespace Crypto;

namespace MevaCoin {

class TransactionPrefixImpl : public ITransactionReader {
public:
  TransactionPrefixImpl();
  TransactionPrefixImpl(const TransactionPrefix& prefix, const Hash& transactionHash);

  virtual ~TransactionPrefixImpl() { }

  virtual Hash getTransactionHash() const override;
  virtual Hash getTransactionPrefixHash() const override;
  virtual Hash getTransactionInputsHash() const override;
  virtual PublicKey getTransactionPublicKey() const override;
  virtual uint64_t getUnlockTime() const override;

  // extra
  virtual bool getPaymentId(Hash& paymentId) const override;
  virtual bool getExtraNonce(BinaryArray& nonce) const override;
  virtual BinaryArray getExtra() const override;

  // inputs
  virtual size_t getInputCount() const override;
  virtual uint64_t getInputTotalAmount() const override;
  virtual TransactionTypes::InputType getInputType(size_t index) const override;
  virtual void getInput(size_t index, KeyInput& input) const override;
  virtual void getInput(size_t index, MultisignatureInput& input) const override;
  virtual std::vector<TransactionInput> getInputs() const override;

  // outputs
  virtual size_t getOutputCount() const override;
  virtual uint64_t getOutputTotalAmount() const override;
  virtual TransactionTypes::OutputType getOutputType(size_t index) const override;
  virtual void getOutput(size_t index, KeyOutput& output, uint64_t& amount) const override;
  virtual void getOutput(size_t index, MultisignatureOutput& output, uint64_t& amount) const override;

  // signatures
  virtual size_t getRequiredSignaturesCount(size_t inputIndex) const override;
  virtual bool findOutputsToAccount(const AccountPublicAddress& addr, const SecretKey& viewSecretKey, std::vector<uint32_t>& outs, uint64_t& outputAmount) const override;

  // various checks
  virtual bool validateInputs() const override;
  virtual bool validateOutputs() const override;
  virtual bool validateSignatures() const override;

  // serialized transaction
  virtual BinaryArray getTransactionData() const override;

  virtual TransactionPrefix getTransactionPrefix() const override;

  virtual bool getTransactionSecretKey(SecretKey& key) const override;

private:
  TransactionPrefix m_txPrefix;
  TransactionExtra m_extra;
  Hash m_txHash;
};

TransactionPrefixImpl::TransactionPrefixImpl() {
}

TransactionPrefixImpl::TransactionPrefixImpl(const TransactionPrefix& prefix, const Hash& transactionHash) {
  m_extra.parse(prefix.extra);

  m_txPrefix = prefix;
  m_txHash = transactionHash;
}

Hash TransactionPrefixImpl::getTransactionHash() const {
  return m_txHash;
}

Hash TransactionPrefixImpl::getTransactionPrefixHash() const {
  return getObjectHash(m_txPrefix);
}

Hash TransactionPrefixImpl::getTransactionInputsHash() const {
  return getObjectHash(m_txPrefix.inputs);
}

PublicKey TransactionPrefixImpl::getTransactionPublicKey() const {
  Crypto::PublicKey pk(NULL_PUBLIC_KEY);
  m_extra.getPublicKey(pk);
  return pk;
}

uint64_t TransactionPrefixImpl::getUnlockTime() const {
  return m_txPrefix.unlockTime;
}

bool TransactionPrefixImpl::getPaymentId(Hash& hash) const {
  BinaryArray nonce;

  if (getExtraNonce(nonce)) {
    Crypto::Hash paymentId;
    if (getPaymentIdFromTransactionExtraNonce(nonce, paymentId)) {
      hash = reinterpret_cast<const Hash&>(paymentId);
      return true;
    }
  }

  return false;
}

bool TransactionPrefixImpl::getExtraNonce(BinaryArray& nonce) const {
  TransactionExtraNonce extraNonce;

  if (m_extra.get(extraNonce)) {
    nonce = extraNonce.nonce;
    return true;
  }

  return false;
}

BinaryArray TransactionPrefixImpl::getExtra() const {
  return m_txPrefix.extra;
}

size_t TransactionPrefixImpl::getInputCount() const {
  return m_txPrefix.inputs.size();
}

uint64_t TransactionPrefixImpl::getInputTotalAmount() const {
  return std::accumulate(m_txPrefix.inputs.begin(), m_txPrefix.inputs.end(), 0ULL, [](uint64_t val, const TransactionInput& in) {
    return val + getTransactionInputAmount(in); });
}

TransactionTypes::InputType TransactionPrefixImpl::getInputType(size_t index) const {
  return getTransactionInputType(getInputChecked(m_txPrefix, index));
}

void TransactionPrefixImpl::getInput(size_t index, KeyInput& input) const {
  input = boost::get<KeyInput>(getInputChecked(m_txPrefix, index, TransactionTypes::InputType::Key));
}

void TransactionPrefixImpl::getInput(size_t index, MultisignatureInput& input) const {
  input = boost::get<MultisignatureInput>(getInputChecked(m_txPrefix, index, TransactionTypes::InputType::Multisignature));
}

std::vector<TransactionInput> TransactionPrefixImpl::getInputs() const {
  return m_txPrefix.inputs;
}

size_t TransactionPrefixImpl::getOutputCount() const {
  return m_txPrefix.outputs.size();
}

uint64_t TransactionPrefixImpl::getOutputTotalAmount() const {
  return std::accumulate(m_txPrefix.outputs.begin(), m_txPrefix.outputs.end(), 0ULL, [](uint64_t val, const TransactionOutput& out) {
    return val + out.amount; });
}

TransactionTypes::OutputType TransactionPrefixImpl::getOutputType(size_t index) const {
  return getTransactionOutputType(getOutputChecked(m_txPrefix, index).target);
}

void TransactionPrefixImpl::getOutput(size_t index, KeyOutput& output, uint64_t& amount) const {
  const auto& out = getOutputChecked(m_txPrefix, index, TransactionTypes::OutputType::Key);
  output = boost::get<KeyOutput>(out.target);
  amount = out.amount;
}

void TransactionPrefixImpl::getOutput(size_t index, MultisignatureOutput& output, uint64_t& amount) const {
  const auto& out = getOutputChecked(m_txPrefix, index, TransactionTypes::OutputType::Multisignature);
  output = boost::get<MultisignatureOutput>(out.target);
  amount = out.amount;
}

size_t TransactionPrefixImpl::getRequiredSignaturesCount(size_t inputIndex) const {
  return ::MevaCoin::getRequiredSignaturesCount(getInputChecked(m_txPrefix, inputIndex));
}

bool TransactionPrefixImpl::findOutputsToAccount(const AccountPublicAddress& addr, const SecretKey& viewSecretKey, std::vector<uint32_t>& outs, uint64_t& outputAmount) const {
  return ::MevaCoin::findOutputsToAccount(m_txPrefix, addr, viewSecretKey, outs, outputAmount);
}

bool TransactionPrefixImpl::validateInputs() const {
  return check_inputs_types_supported(m_txPrefix) &&
          check_inputs_overflow(m_txPrefix) &&
          checkInputsKeyimagesDiff(m_txPrefix) &&
          checkMultisignatureInputsDiff(m_txPrefix);
}

bool TransactionPrefixImpl::validateOutputs() const {
  return check_outs_valid(m_txPrefix) &&
          check_outs_overflow(m_txPrefix);
}

bool TransactionPrefixImpl::validateSignatures() const {
  throw std::system_error(std::make_error_code(std::errc::function_not_supported), "Validating signatures is not supported for transaction prefix");
}

BinaryArray TransactionPrefixImpl::getTransactionData() const {
  return toBinaryArray(m_txPrefix);
}

TransactionPrefix TransactionPrefixImpl::getTransactionPrefix() const {
   return m_txPrefix;
}

bool TransactionPrefixImpl::getTransactionSecretKey(SecretKey& key) const {
  return false;
}


std::unique_ptr<ITransactionReader> createTransactionPrefix(const TransactionPrefix& prefix, const Hash& transactionHash) {
  return std::unique_ptr<ITransactionReader> (new TransactionPrefixImpl(prefix, transactionHash));
}

std::unique_ptr<ITransactionReader> createTransactionPrefix(const Transaction& fullTransaction) {
  return std::unique_ptr<ITransactionReader> (new TransactionPrefixImpl(fullTransaction, getObjectHash(fullTransaction)));
}

}
