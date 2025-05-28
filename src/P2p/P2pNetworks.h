// Copyright (c) 2012-2016, The MevaCoin developers, The Bytecoin developers, The Karbowanec developers
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

#include <boost/uuid/uuid.hpp>

namespace MevaCoin
{
    // UUID fisso per la rete MevaCoin
    const static boost::uuids::uuid MEVACOIN_NETWORK = { {
        0xcf, 0x74, 0x21, 0x8a,
        0x75, 0x61, 0x41, 0x0b,
        0x8e, 0xa7, 0xa3, 0xa9,
        0xc0, 0x12, 0x4b, 0x92
    } };
}
