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

#include "MevaCoinFormatUtils.h"
#include "TransactionExtra.h"

namespace MevaCoin {

  class TransactionExtra {
  public:
    TransactionExtra() {}
    TransactionExtra(const std::vector<uint8_t>& extra) {
      parse(extra);        
    }

    bool parse(const std::vector<uint8_t>& extra) {
      fields.clear();
      return MevaCoin::parseTransactionExtra(extra, fields);
    }

    template <typename T>
    bool get(T& value) const {
      auto it = find(typeid(T));
      if (it == fields.end()) {
        return false;
      }
      value = boost::get<T>(*it);
      return true;
    }

    template <typename T>
    void set(const T& value) {
      auto it = find(typeid(T));
      if (it != fields.end()) {
        *it = value;
      } else {
        fields.push_back(value);
      }
    }

    template <typename T>
    void append(const T& value) {
      fields.push_back(value);
    }

    bool getPublicKey(Crypto::PublicKey& pk) const {
      MevaCoin::TransactionExtraPublicKey extraPk;
      if (!get(extraPk)) {
        return false;
      }
      pk = extraPk.publicKey;
      return true;
    }

    std::vector<uint8_t> serialize() const {
      std::vector<uint8_t> extra;
      writeTransactionExtra(extra, fields);
      return extra;
    }

  private:

    std::vector<MevaCoin::TransactionExtraField>::const_iterator find(const std::type_info& t) const {
      return std::find_if(fields.begin(), fields.end(), [&t](const MevaCoin::TransactionExtraField& f) { return t == f.type(); });
    }

    std::vector<MevaCoin::TransactionExtraField>::iterator find(const std::type_info& t) {
      return std::find_if(fields.begin(), fields.end(), [&t](const MevaCoin::TransactionExtraField& f) { return t == f.type(); });
    }

    std::vector<MevaCoin::TransactionExtraField> fields;
  };

}
