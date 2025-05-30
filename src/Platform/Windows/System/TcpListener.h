// Copyright (c) 2012-2017, The MevaCoin developers, The Bytecoin developers
// Copyright (c) 2016-2019, The Karbo developers
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
#include <string>
#include <stdexcept>

namespace System {

class Dispatcher;
class Ipv4Address;
class TcpConnection;

class TcpListener {
public:
  TcpListener();
  TcpListener(Dispatcher& dispatcher, const Ipv4Address& address, uint16_t port);
  TcpListener(const TcpListener&) = delete;
  TcpListener(TcpListener&& other);
  ~TcpListener();
  TcpListener& operator=(const TcpListener&) = delete;
  TcpListener& operator=(TcpListener&& other);
  TcpConnection accept();

private:
  Dispatcher* dispatcher;
  size_t listener;
  void* context;
};

}
