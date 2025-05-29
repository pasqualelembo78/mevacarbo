// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Mevacoin Developers
// 
// Please see the included LICENSE file for more information.

#pragma once

#include <Wallet/WalletGreen.h>

bool fusionTX(MevaCoin::WalletGreen &wallet, 
              MevaCoin::TransactionParameters p);

bool optimize(MevaCoin::WalletGreen &wallet, uint64_t threshold);

void fullOptimize(MevaCoin::WalletGreen &wallet);

size_t makeFusionTransaction(MevaCoin::WalletGreen &wallet, 
                             uint64_t threshold);
