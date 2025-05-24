// Copyright (c) 2018-2019, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include "MevaCoinProtocol/MevaCoinProtocolDefinitions.h"

#include <unordered_set>

namespace MevaCoin
{
    struct PendingLiteBlock
    {
        NOTIFY_NEW_LITE_BLOCK_request request;
        std::unordered_set<Crypto::Hash> missed_transactions;
    };
} // namespace MevaCoin
