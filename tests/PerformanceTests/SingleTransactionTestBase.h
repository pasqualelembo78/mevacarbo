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
#include "MevaCoinCore/MevaCoinBasic.h"
#include "MevaCoinCore/MevaCoinFormatUtils.h"

#include <Logging/LoggerGroup.h>

class single_tx_test_base
{
public:
  bool init()
  {
    using namespace MevaCoin;

    Currency currency = CurrencyBuilder(m_nullLog).currency();
    m_bob.generate();

    if (!currency.constructMinerTx(BLOCK_MAJOR_VERSION_1, 0, 0, 0, 2, 0, m_bob.getAccountKeys().address, m_tx))
      return false;

    m_tx_pub_key = getTransactionPublicKeyFromExtra(m_tx.extra);
    return true;
  }

protected:

  Logging::LoggerGroup m_nullLog;
  MevaCoin::AccountBase m_bob;
  MevaCoin::Transaction m_tx;
  Crypto::PublicKey m_tx_pub_key;
};
