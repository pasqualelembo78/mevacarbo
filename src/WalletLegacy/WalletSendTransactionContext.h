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

#pragma once

#include <list>
#include <vector>

#include "MevaCoinCore/MevaCoinBasic.h"
#include "IWalletLegacy.h"
#include "ITransfersContainer.h"

namespace MevaCoin {

struct TxDustPolicy
{
  uint64_t dustThreshold;
  bool addToFee;
  MevaCoin::AccountPublicAddress addrForDust;

  TxDustPolicy(uint64_t a_dust_threshold = 0, bool an_add_to_fee = true, MevaCoin::AccountPublicAddress an_addr_for_dust = MevaCoin::AccountPublicAddress())
    : dustThreshold(a_dust_threshold), addToFee(an_add_to_fee), addrForDust(an_addr_for_dust) {}
};

struct SendTransactionContext
{
  TransactionId transactionId;
  std::vector<MevaCoin::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount> outs;
  uint64_t foundMoney;
  std::list<TransactionOutputInformation> selectedTransfers;
  TxDustPolicy dustPolicy;
  uint64_t mixIn;
  Crypto::SecretKey tx_key = NULL_SECRET_KEY;
};

} //namespace MevaCoin
