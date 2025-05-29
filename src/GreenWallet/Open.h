// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Mevacoin Developers
// 
// Please see the included LICENSE file for more information.

#pragma once

#include <GreenWallet/Types.h>

std::shared_ptr<WalletInfo> importFromKeys(MevaCoin::WalletGreen &wallet, 
                                           Crypto::SecretKey privateSpendKey,
                                           Crypto::SecretKey privateViewKey);

std::shared_ptr<WalletInfo> openWallet(MevaCoin::WalletGreen &wallet,
                                       Config &config);

std::shared_ptr<WalletInfo> createViewWallet(MevaCoin::WalletGreen &wallet);

std::shared_ptr<WalletInfo> importWallet(MevaCoin::WalletGreen &wallet);

std::shared_ptr<WalletInfo> createViewWallet(MevaCoin::WalletGreen &wallet);

std::shared_ptr<WalletInfo> mnemonicImportWallet(MevaCoin::WalletGreen 
                                                 &wallet);

std::shared_ptr<WalletInfo> generateWallet(MevaCoin::WalletGreen &wallet);

std::shared_ptr<WalletInfo> importGUIWallet(MevaCoin::WalletGreen &wallet);

Crypto::SecretKey getPrivateKey(std::string outputMsg);

std::string getNewWalletFileName();

std::string getExistingWalletFileName(Config &config);

std::string getWalletPassword(bool verifyPwd, std::string msg);

bool isValidMnemonic(std::string &mnemonic_phrase, 
                     Crypto::SecretKey &private_spend_key);

void logIncorrectMnemonicWords(std::vector<std::string> words);

void viewWalletMsg();

void connectingMsg();

void promptSaveKeys(MevaCoin::WalletGreen &wallet);
