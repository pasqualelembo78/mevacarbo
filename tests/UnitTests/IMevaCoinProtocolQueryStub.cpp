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

#include "IMevaCoinProtocolQueryStub.h"

bool IMevaCoinProtocolQueryStub::addObserver(MevaCoin::IMevaCoinProtocolObserver* observer) {
  return false;
}

bool IMevaCoinProtocolQueryStub::removeObserver(MevaCoin::IMevaCoinProtocolObserver* observer) {
  return false;
}

uint32_t IMevaCoinProtocolQueryStub::getObservedHeight() const {
  return observedHeight;
}

size_t IMevaCoinProtocolQueryStub::getPeerCount() const {
  return peers;
}

bool IMevaCoinProtocolQueryStub::isSynchronized() const {
  return synchronized;
}

void IMevaCoinProtocolQueryStub::setPeerCount(uint32_t count) {
  peers = count;
}

void IMevaCoinProtocolQueryStub::setObservedHeight(uint32_t height) {
  observedHeight = height;
}

void IMevaCoinProtocolQueryStub::setSynchronizedStatus(bool status) {
    synchronized = status;
}
