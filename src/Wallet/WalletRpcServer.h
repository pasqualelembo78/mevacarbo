// Copyright (c) 2012-2016, The MevaCoin developers, The Bytecoin developers
// Copyright (c) 2014-2016, XDN developers
// Copyright (c) 2014-2016, The Monero Project
// Copyright (c) 2016-2023, Karbo developers
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

#pragma  once

#include <future>
#include <thread>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

#include "Common/CommandLine.h"
#include "HTTP/httplib.h"
#include "Rpc/JsonRpc.h"
#include "Logging/LoggerRef.h"
#include "WalletRpcServerCommandsDefinitions.h"
#include "WalletLegacy/WalletLegacy.h"

namespace Tools
{
class wallet_rpc_server
{
public:
  wallet_rpc_server(
    Logging::ILogger& log,
    MevaCoin::IWalletLegacy &w, 
    MevaCoin::INode &n, 
    MevaCoin::Currency& currency,
    const std::string& walletFilename);

  ~wallet_rpc_server();

  static const command_line::arg_descriptor<uint16_t>    arg_rpc_bind_port;
  static const command_line::arg_descriptor<uint16_t>    arg_rpc_bind_ssl_port;
  static const command_line::arg_descriptor<bool>        arg_rpc_bind_ssl_enable;
  static const command_line::arg_descriptor<std::string> arg_rpc_bind_ip;
  static const command_line::arg_descriptor<std::string> arg_rpc_user;
  static const command_line::arg_descriptor<std::string> arg_rpc_password;
  static const command_line::arg_descriptor<std::string> arg_chain_file;
  static const command_line::arg_descriptor<std::string> arg_key_file;

  static void init_options(boost::program_options::options_description& desc);
  bool init(const boost::program_options::variables_map& vm);
  void getServerConf(std::string &bind_address, std::string &bind_address_ssl, bool &enable_ssl);
    
  bool run();
  void stop();

private:
  void processRequest(const httplib::Request& request, httplib::Response& response);

  //json_rpc
  bool on_get_balance(const wallet_rpc::COMMAND_RPC_GET_BALANCE::request& req, wallet_rpc::COMMAND_RPC_GET_BALANCE::response& res);
  bool on_transfer(const wallet_rpc::COMMAND_RPC_TRANSFER::request& req, wallet_rpc::COMMAND_RPC_TRANSFER::response& res);
  bool on_store(const wallet_rpc::COMMAND_RPC_STORE::request& req, wallet_rpc::COMMAND_RPC_STORE::response& res);
  bool on_stop_wallet(const wallet_rpc::COMMAND_RPC_STOP::request& req, wallet_rpc::COMMAND_RPC_STOP::response& res);
  bool on_get_payments(const wallet_rpc::COMMAND_RPC_GET_PAYMENTS::request& req, wallet_rpc::COMMAND_RPC_GET_PAYMENTS::response& res);
  bool on_get_transfers(const wallet_rpc::COMMAND_RPC_GET_TRANSFERS::request& req, wallet_rpc::COMMAND_RPC_GET_TRANSFERS::response& res);
  bool on_get_last_transfers(const wallet_rpc::COMMAND_RPC_GET_LAST_TRANSFERS::request& req, wallet_rpc::COMMAND_RPC_GET_LAST_TRANSFERS::response& res);
  bool on_get_transaction(const wallet_rpc::COMMAND_RPC_GET_TRANSACTION::request& req, wallet_rpc::COMMAND_RPC_GET_TRANSACTION::response& res);
  bool on_get_height(const wallet_rpc::COMMAND_RPC_GET_HEIGHT::request& req, wallet_rpc::COMMAND_RPC_GET_HEIGHT::response& res);
  bool on_get_address(const wallet_rpc::COMMAND_RPC_GET_ADDRESS::request& req, wallet_rpc::COMMAND_RPC_GET_ADDRESS::response& res);
  bool on_query_key(const wallet_rpc::COMMAND_RPC_QUERY_KEY::request& req, wallet_rpc::COMMAND_RPC_QUERY_KEY::response& res);
  bool on_get_tx_key(const wallet_rpc::COMMAND_RPC_GET_TX_KEY::request& req, wallet_rpc::COMMAND_RPC_GET_TX_KEY::response& res);
  bool on_get_tx_proof(const wallet_rpc::COMMAND_RPC_GET_TX_PROOF::request& req, wallet_rpc::COMMAND_RPC_GET_TX_PROOF::response& res);
  bool on_get_reserve_proof(const wallet_rpc::COMMAND_RPC_GET_BALANCE_PROOF::request& req, wallet_rpc::COMMAND_RPC_GET_BALANCE_PROOF::response& res);
  bool on_sign_message(const wallet_rpc::COMMAND_RPC_SIGN_MESSAGE::request& req, wallet_rpc::COMMAND_RPC_SIGN_MESSAGE::response& res);
  bool on_verify_message(const wallet_rpc::COMMAND_RPC_VERIFY_MESSAGE::request& req, wallet_rpc::COMMAND_RPC_VERIFY_MESSAGE::response& res);
  bool on_change_password(const wallet_rpc::COMMAND_RPC_CHANGE_PASSWORD::request& req, wallet_rpc::COMMAND_RPC_CHANGE_PASSWORD::response& res);
  bool on_estimate_fusion(const wallet_rpc::COMMAND_RPC_ESTIMATE_FUSION::request& req, wallet_rpc::COMMAND_RPC_ESTIMATE_FUSION::response& res);
  bool on_send_fusion(const wallet_rpc::COMMAND_RPC_SEND_FUSION::request& req, wallet_rpc::COMMAND_RPC_SEND_FUSION::response& res);
  bool on_gen_paymentid(const wallet_rpc::COMMAND_RPC_GET_ADDRESS::request& req, wallet_rpc::COMMAND_RPC_GEN_PAYMENT_ID::response& res);
  bool on_validate_address(const wallet_rpc::COMMAND_RPC_VALIDATE_ADDRESS::request& req, wallet_rpc::COMMAND_RPC_VALIDATE_ADDRESS::response& res);
  bool on_reset(const wallet_rpc::COMMAND_RPC_RESET::request& req, wallet_rpc::COMMAND_RPC_RESET::response& res);

  bool handle_command_line(const boost::program_options::variables_map& vm);
  void listen(const std::string address, const uint16_t port);
  void listen_ssl(const std::string address, const uint16_t port);
  bool authenticate(const httplib::Request& request) const;

  MevaCoin::Currency& m_currency;
  MevaCoin::IWalletLegacy& m_wallet;
  MevaCoin::INode& m_node;
  httplib::Server* http;
  httplib::SSLServer* https;
  Logging::LoggerRef logger;
  std::list<std::thread> m_workers;

  bool m_enable_ssl;
  bool m_run_ssl;
  uint16_t m_port;
  uint16_t m_port_ssl;
  std::string m_bind_ip;
  std::string m_rpcUser;
  std::string m_rpcPassword;
  std::string m_chain_file;
  std::string m_key_file;
  std::string m_credentials;
  const std::string m_walletFilename;
};
} //Tools
