// Copyright (c) 2012-2016, The MevaCoin developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Cash2 developers
// Copyright (c) 2021-2023, The Talleo developers
// Copyright (c) 2016-2024, The Karbo developers
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

#include "PaymentServiceJsonRpcMessages.h"
#include "Serialization/SerializationOverloads.h"

namespace PaymentService {

void Save::Request::serialize(MevaCoin::ISerializer& /*serializer*/) {
}

void Save::Response::serialize(MevaCoin::ISerializer& /*serializer*/) {
}

void Reset::Request::serialize(MevaCoin::ISerializer& serializer) {
  serializer(viewSecretKey, "viewSecretKey");
  serializer(scanHeight, "scanHeight");
}

void Reset::Response::serialize(MevaCoin::ISerializer& serializer) {
}

void Export::Request::serialize(MevaCoin::ISerializer& serializer) {
  if (!serializer(fileName, "fileName")) {
    throw RequestSerializationError();
  }
}

void Export::Response::serialize(MevaCoin::ISerializer& serializer) {
}

void GetViewKey::Request::serialize(MevaCoin::ISerializer& serializer) {
}

void GetViewKey::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(viewSecretKey, "viewSecretKey");
}

void GetMnemonicSeed::Request::serialize(MevaCoin::ISerializer& serializer) {
  if (!serializer(address, "address")) {
    throw RequestSerializationError();
  }
}

void GetMnemonicSeed::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(mnemonicSeed, "mnemonicSeed");
}

void GetStatus::Request::serialize(MevaCoin::ISerializer& serializer) {
}

void GetStatus::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(blockCount, "blockCount");
  serializer(knownBlockCount, "knownBlockCount");
  serializer(localDaemonBlockCount, "localDaemonBlockCount");
  serializer(lastBlockHash, "lastBlockHash");
  serializer(peerCount, "peerCount");
  serializer(minimalFee, "minimalFee");
  serializer(version, "version");
}

void ValidateAddress::Request::serialize(MevaCoin::ISerializer& serializer) {
  serializer(address, "address");
}

void ValidateAddress::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(isValid, "isValid");
  serializer(address, "address");
  serializer(spendPublicKey, "spendPublicKey");
  serializer(viewPublicKey, "viewPublicKey");
}

void GetAddresses::Request::serialize(MevaCoin::ISerializer& serializer) {
}

void GetAddresses::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(addresses, "addresses");
}

void GetAddressesCount::Request::serialize(MevaCoin::ISerializer& serializer) {
}

void GetAddressesCount::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(addresses_count, "addressesCount");
}

void CreateAddress::Request::serialize(MevaCoin::ISerializer& serializer) {
  bool hasSecretKey = serializer(spendSecretKey, "spendSecretKey");
  bool hasPublicKey = serializer(spendPublicKey, "spendPublicKey");
  bool hasScanHeight = serializer(scanHeight, "scanHeight");
  bool hasReset = serializer(reset, "reset");
  if (!hasReset && !hasScanHeight)
     reset = true;

  if (hasSecretKey && hasPublicKey) {
    //TODO: replace it with error codes
    throw RequestSerializationError();
  }

  if (hasScanHeight && hasReset) {
    //TODO: replace it with error codes
    throw RequestSerializationError();
  }
}

void CreateAddress::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(address, "address");
}

void CreateAddressList::Request::serialize(MevaCoin::ISerializer& serializer) {
  if (!serializer(spendSecretKeys, "spendSecretKeys")) {
    //TODO: replace it with error codes
    throw RequestSerializationError();
  }
  bool hasReset = serializer(reset, "reset");
  if (!hasReset)
    reset = true;
  bool hasScanHeights = serializer(scanHeights, "scanHeights");
  if (hasScanHeights && hasReset) {
    //TODO: replace it with error codes
    throw RequestSerializationError();
  }
  if (hasScanHeights && scanHeights.size() != spendSecretKeys.size()) {
    //TODO: replace it with error codes
    throw RequestSerializationError();
  }
}

void CreateAddressList::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(addresses, "addresses");
}

void DeleteAddress::Request::serialize(MevaCoin::ISerializer& serializer) {
  if (!serializer(address, "address")) {
    throw RequestSerializationError();
  }
}

void DeleteAddress::Response::serialize(MevaCoin::ISerializer& serializer) {
}

void HasAddress::Request::serialize(MevaCoin::ISerializer& serializer) {
  if (!serializer(address, "address")) {
    throw RequestSerializationError();
  }
}

void HasAddress::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(isOurs, "isOurs");
}

void GetSpendKeys::Request::serialize(MevaCoin::ISerializer& serializer) {
  if (!serializer(address, "address")) {
    throw RequestSerializationError();
  }
}

void GetSpendKeys::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(spendSecretKey, "spendSecretKey");
  serializer(spendPublicKey, "spendPublicKey");
}

void GetBalance::Request::serialize(MevaCoin::ISerializer& serializer) {
  serializer(address, "address");
}

void GetBalance::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(availableBalance, "availableBalance");
  serializer(lockedAmount, "lockedAmount");
}

void GetBlockHashes::Request::serialize(MevaCoin::ISerializer& serializer) {
  bool r = serializer(firstBlockIndex, "firstBlockIndex");
  r &= serializer(blockCount, "blockCount");

  if (!r) {
    throw RequestSerializationError();
  }
}

void GetBlockHashes::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(blockHashes, "blockHashes");
}

void TransactionHashesInBlockRpcInfo::serialize(MevaCoin::ISerializer& serializer) {
  serializer(blockHash, "blockHash");
  serializer(transactionHashes, "transactionHashes");
}

void GetTransactionHashes::Request::serialize(MevaCoin::ISerializer& serializer) {
  serializer(addresses, "addresses");

  if (serializer(blockHash, "blockHash") == serializer(firstBlockIndex, "firstBlockIndex")) {
    throw RequestSerializationError();
  }

  if (!serializer(blockCount, "blockCount")) {
    throw RequestSerializationError();
  }

  serializer(paymentId, "paymentId");
}

void GetTransactionHashes::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(items, "items");
}

void TransferRpcInfo::serialize(MevaCoin::ISerializer& serializer) {
  serializer(type, "type");
  serializer(address, "address");
  serializer(amount, "amount");
}

void TransactionRpcInfo::serialize(MevaCoin::ISerializer& serializer) {
  serializer(state, "state");
  serializer(transactionHash, "transactionHash");
  serializer(blockIndex, "blockIndex");
  serializer(confirmations, "confirmations");
  serializer(timestamp, "timestamp");
  serializer(isBase, "isBase");
  serializer(unlockTime, "unlockTime");
  serializer(amount, "amount");
  serializer(fee, "fee");
  serializer(transfers, "transfers");
  serializer(extra, "extra");
  serializer(paymentId, "paymentId");
}

void GetTransaction::Request::serialize(MevaCoin::ISerializer& serializer) {
  if (!serializer(transactionHash, "transactionHash")) {
    throw RequestSerializationError();
  }
}

void GetTransaction::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(transaction, "transaction");
}

void TransactionsInBlockRpcInfo::serialize(MevaCoin::ISerializer& serializer) {
  serializer(blockHash, "blockHash");
  serializer(transactions, "transactions");
}

void GetTransactions::Request::serialize(MevaCoin::ISerializer& serializer) {
  serializer(addresses, "addresses");

  if (serializer(blockHash, "blockHash") == serializer(firstBlockIndex, "firstBlockIndex")) {
    throw RequestSerializationError();
  }

  if (!serializer(blockCount, "blockCount")) {
    throw RequestSerializationError();
  }

  serializer(paymentId, "paymentId");
}

void GetTransactions::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(items, "items");
}

void GetUnconfirmedTransactionHashes::Request::serialize(MevaCoin::ISerializer& serializer) {
  serializer(addresses, "addresses");
}

void GetUnconfirmedTransactionHashes::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(transactionHashes, "transactionHashes");
}

void GetTransactionSecretKey::Request::serialize(MevaCoin::ISerializer& serializer) {
  if (!serializer(transactionHash, "transactionHash")) {
    throw RequestSerializationError();
  }
}

void GetTransactionSecretKey::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(transactionSecretKey, "transactionSecretKey");
}

void GetTransactionProof::Request::serialize(MevaCoin::ISerializer& serializer) {
  if (!serializer(transactionHash, "transactionHash")) {
    throw RequestSerializationError();
  }
  if (!serializer(destinationAddress, "destinationAddress")) {
    throw RequestSerializationError();
  }
  serializer(transactionSecretKey, "transactionSecretKey");
}

void GetTransactionProof::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(transactionProof, "transactionProof");
}

void GetReserveProof::Request::serialize(MevaCoin::ISerializer& serializer) {
  if (!serializer(address, "address")) {
    throw RequestSerializationError();
  }
  serializer(amount, "amount");
  serializer(message, "message");
}

void GetReserveProof::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(reserveProof, "reserveProof");
}

void WalletRpcOrder::serialize(MevaCoin::ISerializer& serializer) {
  bool r = serializer(address, "address");
  r &= serializer(amount, "amount");

  if (!r) {
    throw RequestSerializationError();
  }
}

void SignMessage::Request::serialize(MevaCoin::ISerializer& serializer) {
  serializer(address, "address");

  if (!serializer(message, "message")) {
    throw RequestSerializationError();
  }
}

void SignMessage::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(address, "address");
  serializer(signature, "signature");
}

void VerifyMessage::Request::serialize(MevaCoin::ISerializer& serializer) {
  if (!serializer(address, "address") || !serializer(message, "message") || !serializer(signature, "signature")) {
    throw RequestSerializationError();
  }
}

void VerifyMessage::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(isValid, "isValid");
}

void SendTransaction::Request::serialize(MevaCoin::ISerializer& serializer) {
  serializer(sourceAddresses, "addresses");

  if (!serializer(transfers, "transfers")) {
    throw RequestSerializationError();
  }

  serializer(changeAddress, "changeAddress");

  if (!serializer(fee, "fee")) {
    throw RequestSerializationError();
  }

  if (!serializer(anonymity, "anonymity")) {
    throw RequestSerializationError();
  }

  bool hasExtra = serializer(extra, "extra");
  bool hasPaymentId = serializer(paymentId, "paymentId");

  if (hasExtra && hasPaymentId) {
    throw RequestSerializationError();
  }

  serializer(unlockTime, "unlockTime");
}

void SendTransaction::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(transactionHash, "transactionHash");
  serializer(transactionSecretKey, "transactionSecretKey");
}

void CreateDelayedTransaction::Request::serialize(MevaCoin::ISerializer& serializer) {
  serializer(addresses, "addresses");

  if (!serializer(transfers, "transfers")) {
    throw RequestSerializationError();
  }

  serializer(changeAddress, "changeAddress");

  if (!serializer(fee, "fee")) {
    throw RequestSerializationError();
  }

  if (!serializer(anonymity, "anonymity")) {
    throw RequestSerializationError();
  }

  bool hasExtra = serializer(extra, "extra");
  bool hasPaymentId = serializer(paymentId, "paymentId");

  if (hasExtra && hasPaymentId) {
    throw RequestSerializationError();
  }

  serializer(unlockTime, "unlockTime");
}

void CreateDelayedTransaction::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(transactionHash, "transactionHash");
}

void GetDelayedTransactionHashes::Request::serialize(MevaCoin::ISerializer& serializer) {
}

void GetDelayedTransactionHashes::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(transactionHashes, "transactionHashes");
}

void DeleteDelayedTransaction::Request::serialize(MevaCoin::ISerializer& serializer) {
  if (!serializer(transactionHash, "transactionHash")) {
    throw RequestSerializationError();
  }
}

void DeleteDelayedTransaction::Response::serialize(MevaCoin::ISerializer& serializer) {
}

void SendDelayedTransaction::Request::serialize(MevaCoin::ISerializer& serializer) {
  if (!serializer(transactionHash, "transactionHash")) {
    throw RequestSerializationError();
  }
}

void SendDelayedTransaction::Response::serialize(MevaCoin::ISerializer& serializer) {
}

void SendFusionTransaction::Request::serialize(MevaCoin::ISerializer& serializer) {
  if (!serializer(threshold, "threshold")) {
    throw RequestSerializationError();
  }

  if (!serializer(anonymity, "anonymity")) {
    throw RequestSerializationError();
  }

  serializer(addresses, "addresses");
  serializer(destinationAddress, "destinationAddress");
}

void SendFusionTransaction::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(transactionHash, "transactionHash");
}

void EstimateFusion::Request::serialize(MevaCoin::ISerializer& serializer) {
  if (!serializer(threshold, "threshold")) {
    throw RequestSerializationError();
  }

  serializer(addresses, "addresses");
}

void EstimateFusion::Response::serialize(MevaCoin::ISerializer& serializer) {
  serializer(fusionReadyCount, "fusionReadyCount");
  serializer(totalOutputCount, "totalOutputCount");
}

}
