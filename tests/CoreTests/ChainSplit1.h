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

/************************************************************************/
/*                                                                      */
/************************************************************************/
class gen_simple_chain_split_1 : public test_chain_unit_base 
{
public: 
  gen_simple_chain_split_1();
  bool generate(std::vector<test_event_entry> &events) const; 
  bool check_split_not_switched(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry> &events); 
  bool check_split_not_switched2(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry> &events); 
  bool check_split_switched(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry> &events); 
  bool check_split_not_switched_back(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry> &events); 
  bool check_split_switched_back_1(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry> &events); 
  bool check_split_switched_back_2(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry> &events); 
  bool check_mempool_1(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry> &events); 
  bool check_mempool_2(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry> &events); 
  /*bool check_orphaned_chain_1(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry> &events); 
  bool check_orphaned_switched_to_alternative(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry> &events); 
  bool check_orphaned_chain_2(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry> &events); 
  bool check_orphaned_switched_to_main(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry> &events); 
  bool check_orphaned_chain_38(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry> &events); 
  bool check_orphaned_chain_39(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry> &events); 
  bool check_orphaned_chain_40(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry> &events); 
  bool check_orphaned_chain_41(MevaCoin::core& c, size_t ev_index, const std::vector<test_event_entry> &events); */
private:
};
