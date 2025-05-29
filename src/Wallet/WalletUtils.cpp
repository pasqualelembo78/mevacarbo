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

#include "WalletUtils.h"

#include "MevaCoin.h"
#include "crypto/crypto.h"
#include "Wallet/WalletErrors.h"

namespace MevaCoin {

void throwIfKeysMissmatch(const Crypto::SecretKey& secretKey, const Crypto::PublicKey& expectedPublicKey, const std::string& message) {
  Crypto::PublicKey pub;
  bool r = Crypto::secret_key_to_public_key(secretKey, pub);
  if (!r || expectedPublicKey != pub) {
    throw std::system_error(make_error_code(MevaCoin::error::WRONG_PASSWORD), message);
  }
}

bool validateAddress(const std::string& address, const MevaCoin::Currency& currency) {
  MevaCoin::AccountPublicAddress ignore;
  return currency.parseAccountAddressString(address, ignore);
}

std::ostream& operator<<(std::ostream& os, MevaCoin::WalletTransactionState state) {
  switch (state) {
  case MevaCoin::WalletTransactionState::SUCCEEDED:
    os << "SUCCEEDED";
    break;
  case MevaCoin::WalletTransactionState::FAILED:
    os << "FAILED";
    break;
  case MevaCoin::WalletTransactionState::CANCELLED:
    os << "CANCELLED";
    break;
  case MevaCoin::WalletTransactionState::CREATED:
    os << "CREATED";
    break;
  case MevaCoin::WalletTransactionState::DELETED:
    os << "DELETED";
    break;
  default:
    os << "<UNKNOWN>";
  }

  return os << " (" << static_cast<int>(state) << ')';
}

std::ostream& operator<<(std::ostream& os, MevaCoin::WalletTransferType type) {
  switch (type) {
  case MevaCoin::WalletTransferType::USUAL:
    os << "USUAL";
    break;
  case MevaCoin::WalletTransferType::DONATION:
    os << "DONATION";
    break;
  case MevaCoin::WalletTransferType::CHANGE:
    os << "CHANGE";
    break;
  default:
    os << "<UNKNOWN>";
  }

  return os << " (" << static_cast<int>(type) << ')';
}

std::ostream& operator<<(std::ostream& os, MevaCoin::WalletGreen::WalletState state) {
  switch (state) {
  case MevaCoin::WalletGreen::WalletState::INITIALIZED:
    os << "INITIALIZED";
    break;
  case MevaCoin::WalletGreen::WalletState::NOT_INITIALIZED:
    os << "NOT_INITIALIZED";
    break;
  default:
    os << "<UNKNOWN>";
  }

  return os << " (" << static_cast<int>(state) << ')';
}

std::ostream& operator<<(std::ostream& os, MevaCoin::WalletGreen::WalletTrackingMode mode) {
  switch (mode) {
  case MevaCoin::WalletGreen::WalletTrackingMode::TRACKING:
    os << "TRACKING";
    break;
  case MevaCoin::WalletGreen::WalletTrackingMode::NOT_TRACKING:
    os << "NOT_TRACKING";
    break;
  case MevaCoin::WalletGreen::WalletTrackingMode::NO_ADDRESSES:
    os << "NO_ADDRESSES";
    break;
  default:
    os << "<UNKNOWN>";
  }

  return os << " (" << static_cast<int>(mode) << ')';
}

TransferListFormatter::TransferListFormatter(const MevaCoin::Currency& currency, const WalletGreen::TransfersRange& range) :
  m_currency(currency),
  m_range(range) {
}

void TransferListFormatter::print(std::ostream& os) const {
  for (auto it = m_range.first; it != m_range.second; ++it) {
    os << '\n' << std::setw(21) << m_currency.formatAmount(it->second.amount) <<
      ' ' << (it->second.address.empty() ? "<UNKNOWN>" : it->second.address) <<
      ' ' << it->second.type;
  }
}

std::ostream& operator<<(std::ostream& os, const TransferListFormatter& formatter) {
  formatter.print(os);
  return os;
}

WalletOrderListFormatter::WalletOrderListFormatter(const MevaCoin::Currency& currency, const std::vector<MevaCoin::WalletOrder>& walletOrderList) :
  m_currency(currency),
  m_walletOrderList(walletOrderList) {
}

void WalletOrderListFormatter::print(std::ostream& os) const {
  os << '{';

  if (!m_walletOrderList.empty()) {
    os << '<' << m_currency.formatAmount(m_walletOrderList.front().amount) << ", " << m_walletOrderList.front().address << '>';

    for (auto it = std::next(m_walletOrderList.begin()); it != m_walletOrderList.end(); ++it) {
      os << '<' << m_currency.formatAmount(it->amount) << ", " << it->address << '>';
    }
  }

  os << '}';
}

std::ostream& operator<<(std::ostream& os, const WalletOrderListFormatter& formatter) {
  formatter.print(os);
  return os;
}

}
