// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Mevacoin Developers
// 
// Please see the included LICENSE file for more information.

#pragma once

#include <GreenWallet/Types.h>

#include <Wallet/WalletGreen.h>

bool handleCommand(const std::string command,
                   std::shared_ptr<WalletInfo> walletInfo,
                   MevaCoin::INode &node);

void changePassword(std::shared_ptr<WalletInfo> walletInfo);

void printPrivateKeys(MevaCoin::WalletGreen &wallet, bool viewWallet);

void reset(MevaCoin::INode &node, std::shared_ptr<WalletInfo> walletInfo);

void status(MevaCoin::INode &node, MevaCoin::WalletGreen &wallet);

void printHeights(uint32_t localHeight, uint32_t remoteHeight,
	uint32_t walletHeight);

void printSyncStatus(uint32_t localHeight, uint32_t remoteHeight,
	uint32_t walletHeight);

void printSyncSummary(uint32_t localHeight, uint32_t remoteHeight,
	uint32_t walletHeight);

void printPeerCount(size_t peerCount);

void printHashrate(uint64_t difficulty);

void blockchainHeight(MevaCoin::INode &node, MevaCoin::WalletGreen &wallet);

void balance(MevaCoin::INode &node, MevaCoin::WalletGreen &wallet,
             bool viewWallet);

void exportKeys(std::shared_ptr<WalletInfo> walletInfo);

void saveCSV(MevaCoin::WalletGreen &wallet, MevaCoin::INode &node);

void save(MevaCoin::WalletGreen &wallet);

void listTransfers(bool incoming, bool outgoing, 
                   MevaCoin::WalletGreen &wallet, MevaCoin::INode &node);

void printOutgoingTransfer(MevaCoin::WalletTransaction t,
                           MevaCoin::INode &node);

void printIncomingTransfer(MevaCoin::WalletTransaction t,
                           MevaCoin::INode &node);

std::string getGUIPrivateKey(MevaCoin::WalletGreen &wallet);

void help(std::shared_ptr<WalletInfo> wallet);

void advanced(std::shared_ptr<WalletInfo> wallet);

void reserveProof(std::shared_ptr<WalletInfo> walletInfo, bool viewWallet);

void txSecretKey(MevaCoin::WalletGreen &wallet);

void txProof(MevaCoin::WalletGreen &wallet);

void signMessage(std::shared_ptr<WalletInfo> walletInfo, bool viewWallet);

void verifyMessage(MevaCoin::WalletGreen &wallet);
