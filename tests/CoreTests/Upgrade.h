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

struct gen_upgrade : public test_chain_unit_base
{
  gen_upgrade();

  bool generate(std::vector<test_event_entry>& events) const;

  bool check_block_verification_context(const MevaCoin::block_verification_context& bvc, size_t eventIdx, const MevaCoin::Block& blk);

  bool markInvalidBlock(MevaCoin::core& c, size_t evIndex, const std::vector<test_event_entry>& events);
  bool checkBlockTemplateVersionIsV1(MevaCoin::core& c, size_t evIndex, const std::vector<test_event_entry>& events);
  bool checkBlockTemplateVersionIsV2(MevaCoin::core& c, size_t evIndex, const std::vector<test_event_entry>& events);
  bool checkBlockRewardEqFee(MevaCoin::core& c, size_t evIndex, const std::vector<test_event_entry>& events);
  bool checkBlockRewardIsZero(MevaCoin::core& c, size_t evIndex, const std::vector<test_event_entry>& events);
  bool rememberCoinsInCirculationBeforeUpgrade(MevaCoin::core& c, size_t evIndex, const std::vector<test_event_entry>& events);
  bool rememberCoinsInCirculationAfterUpgrade(MevaCoin::core& c, size_t evIndex, const std::vector<test_event_entry>& events);

private:
  bool checkBeforeUpgrade(std::vector<test_event_entry>& events, test_generator& generator,
                          const MevaCoin::Block& parentBlock, const MevaCoin::AccountBase& minerAcc, bool checkReward) const;
  bool checkAfterUpgrade(std::vector<test_event_entry>& events, test_generator& generator,
                         const MevaCoin::Block& parentBlock, const MevaCoin::AccountBase& minerAcc) const;
  bool checkBlockTemplateVersion(MevaCoin::core& c, uint8_t expectedMajorVersion, uint8_t expectedMinorVersion);

private:
  size_t m_invalidBlockIndex;
  size_t m_checkBlockTemplateVersionCallCounter;
  uint64_t m_coinsInCirculationBeforeUpgrade;
  uint64_t m_coinsInCirculationAfterUpgrade;
};
