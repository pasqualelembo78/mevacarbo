// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Mevacoin Developers
// 
// Please see the included LICENSE file for more information.

#pragma once

#include <NodeRpcProxy/NodeRpcProxy.h>
#include <GreenWallet/Types.h>

int main(int argc, char **argv);

void run(MevaCoin::WalletGreen &wallet, MevaCoin::INode &node,
         Config &config);
