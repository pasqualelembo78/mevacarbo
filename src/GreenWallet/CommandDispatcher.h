// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Mevacoin Developers
// 
// Please see the included LICENSE file for more information.

#pragma once

#include <GreenWallet/Types.h>

bool handleCommand(const std::string command,
                   std::shared_ptr<WalletInfo> walletInfo,
                   MevaCoin::INode &node);

std::shared_ptr<WalletInfo> handleLaunchCommand(MevaCoin::WalletGreen &wallet,
                                                std::string launchCommand,
                                                Config &config);
