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

#include "crypto/crypto.h"
#include "MevaCoinCore/MevaCoinBasic.h"

#include "SingleTransactionTestBase.h"

class test_generate_key_derivation : public single_tx_test_base
{
public:
  static const size_t loop_count = 1000;

  bool test()
  {
    Crypto::KeyDerivation recv_derivation;
    Crypto::generate_key_derivation(m_tx_pub_key, m_bob.getAccountKeys().viewSecretKey, recv_derivation);
    return true;
  }
};
