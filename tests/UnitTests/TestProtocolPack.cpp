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

#include <boost/lexical_cast.hpp>

#include "gtest/gtest.h"
#include "MevaCoinProtocol/MevaCoinProtocolDefinitions.h"
#include "Serialization/SerializationTools.h"

TEST(protocol_pack, protocol_pack_command) 
{
  std::string buff;
  MevaCoin::NOTIFY_RESPONSE_CHAIN_ENTRY::request r;
  r.start_height = 1;
  r.total_height = 3;
  for(int i = 1; i < 10000; i += i*10) {
    r.m_block_ids.resize(i, MevaCoin::NULL_HASH);
    buff = MevaCoin::storeToBinaryKeyValue(r);

    MevaCoin::NOTIFY_RESPONSE_CHAIN_ENTRY::request r2;
    MevaCoin::loadFromBinaryKeyValue(r2, buff);
    ASSERT_TRUE(r.m_block_ids.size() == i);
    ASSERT_TRUE(r.start_height == 1);
    ASSERT_TRUE(r.total_height == 3);
  }
}
