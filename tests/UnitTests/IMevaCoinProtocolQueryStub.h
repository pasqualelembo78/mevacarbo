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

#include <cstdint>

#include "MevaCoinProtocol/IMevaCoinProtocolObserver.h"
#include "MevaCoinProtocol/IMevaCoinProtocolQuery.h"

class IMevaCoinProtocolQueryStub: public MevaCoin::IMevaCoinProtocolQuery {
public:
  IMevaCoinProtocolQueryStub() : peers(0), observedHeight(0), synchronized(false) {}

  virtual bool addObserver(MevaCoin::IMevaCoinProtocolObserver* observer) override;
  virtual bool removeObserver(MevaCoin::IMevaCoinProtocolObserver* observer) override;
  virtual uint32_t getObservedHeight() const override;
  virtual size_t getPeerCount() const override;
  virtual bool isSynchronized() const override;

  void setPeerCount(uint32_t count);
  void setObservedHeight(uint32_t height);

  void setSynchronizedStatus(bool status);

private:
  size_t peers;
  uint32_t observedHeight;

  bool synchronized;
};
