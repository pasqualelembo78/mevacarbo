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

#include "Chaingen.h"

struct GetRandomOutputs : public test_chain_unit_base
{
  GetRandomOutputs();

  // bool check_tx_verification_context(const MevaCoin::tx_verification_context& tvc, bool tx_added, size_t event_idx, const MevaCoin::Transaction& tx);
  // bool check_block_verification_context(const MevaCoin::block_verification_context& bvc, size_t event_idx, const MevaCoin::Block& block);
  // bool mark_last_valid_block(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry>& events);

  bool generate(std::vector<test_event_entry>& events) const;


private:

  bool checkHalfUnlocked(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry>& events);
  bool checkFullyUnlocked(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry>& events);

  bool request(MevaCoin::core& c, uint64_t amount, size_t mixin, MevaCoin::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_response& resp);

};
