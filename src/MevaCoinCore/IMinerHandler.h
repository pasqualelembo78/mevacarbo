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

#include "MevaCoinCore/MevaCoinBasic.h"
#include "MevaCoinCore/Difficulty.h"

namespace MevaCoin {
  struct IMinerHandler {
    virtual bool handle_block_found(Block& b) = 0;
    virtual bool get_block_template(Block& b, const AccountKeys& acc, difficulty_type& diffic, uint32_t& height, const BinaryArray& ex_nonce) = 0;
    virtual bool getBlockLongHash(Crypto::cn_context &context, const Block& b, Crypto::Hash& res) = 0;

  protected:
    ~IMinerHandler(){};
  };
}
