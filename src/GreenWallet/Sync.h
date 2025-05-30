// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Mevacoin Developers
// 
// Please see the included LICENSE file for more information.

#pragma once

#include <GreenWallet/Types.h>

void syncWallet(MevaCoin::INode &node,
                std::shared_ptr<WalletInfo> walletInfo);

void checkForNewTransactions(std::shared_ptr<WalletInfo> walletInfo);
