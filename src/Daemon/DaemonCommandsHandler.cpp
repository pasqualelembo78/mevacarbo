// Copyright (c) 2012-2016, The MevaCoin developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero project
// Copyright (c) 2014-2018, The Forknote developers
// Copyright (c) 2018, The TurtleCoin developers
// Copyright (c) 2016-2020, The Karbo developers
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

#include "DaemonCommandsHandler.h"

#include <ctime>
#include "P2p/NetNode.h"
#include <Common/ColouredMsg.h>
#include "MevaCoinCore/Miner.h"
#include "MevaCoinCore/Core.h"
#include "MevaCoinProtocol/MevaCoinProtocolHandler.h"
#include "Serialization/SerializationTools.h"
#include "version.h"
#include <boost/format.hpp>
#include "math.h"
#include "MevaCoin.h"

#if defined(WIN32)
#undef ERROR
#endif

namespace {
  template <typename T>
  static bool print_as_json(const T& obj) {
    std::cout << MevaCoin::storeToJson(obj) << ENDL;
    return true;
  }
}


DaemonCommandsHandler::DaemonCommandsHandler(MevaCoin::Core& core, MevaCoin::NodeServer& srv, Logging::LoggerManager& log, const MevaCoin::IMevaCoinProtocolQuery& protocol, MevaCoin::RpcServer* prpc_server) :
  m_core(core), m_srv(srv), logger(log, "daemon"), m_logManager(log), protocolQuery(protocol), m_prpc_server(prpc_server) {
  m_consoleHandler.setHandler("exit", std::bind(&DaemonCommandsHandler::exit, this, std::placeholders::_1), "Shutdown the daemon");
  m_consoleHandler.setHandler("help", std::bind(&DaemonCommandsHandler::help, this, std::placeholders::_1), "Show this help");
  m_consoleHandler.setHandler("print_pl", std::bind(&DaemonCommandsHandler::print_pl, this, std::placeholders::_1), "Print peer list");
  m_consoleHandler.setHandler("print_cn", std::bind(&DaemonCommandsHandler::print_cn, this, std::placeholders::_1), "Print connections");
  m_consoleHandler.setHandler("print_dandelion", std::bind(&DaemonCommandsHandler::print_dand, this, std::placeholders::_1), "Print current dandelion connections");
  m_consoleHandler.setHandler("print_bc", std::bind(&DaemonCommandsHandler::print_bc, this, std::placeholders::_1), "Print blockchain info in a given blocks range, print_bc <begin_height> [<end_height>]");
  m_consoleHandler.setHandler("height", std::bind(&DaemonCommandsHandler::print_height, this, std::placeholders::_1), "Print blockchain height");
  //m_consoleHandler.setHandler("print_bci", std::bind(&DaemonCommandsHandler::print_bci, this, std::placeholders::_1));
  //m_consoleHandler.setHandler("print_bc_outs", std::bind(&DaemonCommandsHandler::print_bc_outs, this, std::placeholders::_1));
  m_consoleHandler.setHandler("print_block", std::bind(&DaemonCommandsHandler::print_block, this, std::placeholders::_1), "Print block, print_block <block_hash> | <block_height>");
  m_consoleHandler.setHandler("print_tx", std::bind(&DaemonCommandsHandler::print_tx, this, std::placeholders::_1), "Print transaction, print_tx <transaction_hash>");
  m_consoleHandler.setHandler("start_mining", std::bind(&DaemonCommandsHandler::start_mining, this, std::placeholders::_1), "Start mining with keys, start_mining <spend key> <view key> [threads=1]");
  m_consoleHandler.setHandler("stop_mining", std::bind(&DaemonCommandsHandler::stop_mining, this, std::placeholders::_1), "Stop mining");
  m_consoleHandler.setHandler("print_pool", std::bind(&DaemonCommandsHandler::print_pool, this, std::placeholders::_1), "Print transaction pool (long format)");
  m_consoleHandler.setHandler("print_pool_sh", std::bind(&DaemonCommandsHandler::print_pool_sh, this, std::placeholders::_1), "Print transaction pool (short format)");
  m_consoleHandler.setHandler("print_mp", std::bind(&DaemonCommandsHandler::print_pool_count, this, std::placeholders::_1), "Print number of transactions in memory pool");
  m_consoleHandler.setHandler("show_hr", std::bind(&DaemonCommandsHandler::show_hr, this, std::placeholders::_1), "Start showing hash rate");
  m_consoleHandler.setHandler("hide_hr", std::bind(&DaemonCommandsHandler::hide_hr, this, std::placeholders::_1), "Stop showing hash rate");
  m_consoleHandler.setHandler("set_log", std::bind(&DaemonCommandsHandler::set_log, this, std::placeholders::_1), "set_log <level> - Change current log level, <level> is a number 0-4");
  m_consoleHandler.setHandler("print_diff", std::bind(&DaemonCommandsHandler::print_diff, this, std::placeholders::_1), "Difficulty for next block");
  m_consoleHandler.setHandler("print_ban", std::bind(&DaemonCommandsHandler::print_ban, this, std::placeholders::_1), "Print banned nodes");
  m_consoleHandler.setHandler("ban", std::bind(&DaemonCommandsHandler::ban, this, std::placeholders::_1), "Ban a given <IP> for [<seconds>] or permanently if no duration provided, ban <IP> [<seconds>]");
  m_consoleHandler.setHandler("unban", std::bind(&DaemonCommandsHandler::unban, this, std::placeholders::_1), "Unban a given <IP>, unban <IP>");
  m_consoleHandler.setHandler("status", std::bind(&DaemonCommandsHandler::status, this, std::placeholders::_1), "Show daemon status");
  m_consoleHandler.setHandler("save", std::bind(&DaemonCommandsHandler::save, this, std::placeholders::_1), "Store blockchain");
}

//--------------------------------------------------------------------------------
std::string DaemonCommandsHandler::get_commands_str()
{
  std::stringstream ss;
  ss << MevaCoin::MEVACOIN_NAME << " v" << PROJECT_VERSION_LONG << ENDL;
  ss << "Commands: " << ENDL;
  std::string usage = m_consoleHandler.getUsage();
  boost::replace_all(usage, "\n", "\n  ");
  usage.insert(0, "  ");
  ss << usage << ENDL;
  return ss.str();
}

//--------------------------------------------------------------------------------
std::string DaemonCommandsHandler::get_mining_speed(uint64_t hr) {
  if (hr > 1e12) return (boost::format("%.2f TH/s") % (hr / 1e12)).str();
  if (hr > 1e9) return (boost::format("%.2f GH/s") % (hr / 1e9)).str();
  if (hr > 1e6) return (boost::format("%.2f MH/s") % (hr / 1e6)).str();
  if (hr > 1e3) return (boost::format("%.2f kH/s") % (hr / 1e3)).str();
  return (boost::format("%.0f H/s") % hr).str();
}

//--------------------------------------------------------------------------------
float DaemonCommandsHandler::get_sync_percentage(uint64_t height, uint64_t target_height) {
  target_height = target_height ? target_height < height ? height : target_height : height;
  float pc = 100.0f * height / target_height;
  if (height < target_height && pc > 99.9f){
    return 99.9f; // to avoid 100% when not fully synced
  }
    return pc;
}

//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::exit(const std::vector<std::string>& args) {
  m_consoleHandler.requestStop();
  m_srv.sendStopSignal();
  return true;
}

//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::help(const std::vector<std::string>& args) {
  std::cout << get_commands_str() << ENDL;
  return true;
}

//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::status(const std::vector<std::string>& args) {
  uint32_t height = m_core.getCurrentBlockchainHeight() - 1;
  uint64_t difficulty = m_core.getNextBlockDifficulty();
  size_t tx_pool_size = m_core.getPoolTransactionsCount();
  size_t alt_blocks_count = m_core.getAlternativeBlocksCount();
  uint32_t last_known_block_index = std::max(static_cast<uint32_t>(1), protocolQuery.getObservedHeight()) - 1;
  Crypto::Hash last_block_hash = m_core.getBlockIdByHeight(height);
  size_t total_conn = m_srv.get_connections_count();
  size_t rpc_conn = m_prpc_server->getRpcConnectionsCount();
  size_t outgoing_connections_count = m_srv.get_outgoing_connections_count();
  size_t incoming_connections_count = total_conn - outgoing_connections_count;
  size_t white_peerlist_size = m_srv.getPeerlistManager().get_white_peers_count();
  size_t grey_peerlist_size = m_srv.getPeerlistManager().get_gray_peers_count();
  uint64_t hashrate = (uint64_t) round(difficulty / MevaCoin::parameters::DIFFICULTY_TARGET);
  std::time_t uptime = std::time(nullptr) - m_core.getStartTime();
  uint8_t majorVersion = m_core.getBlockMajorVersionForHeight(height);
  bool synced = ((uint32_t)height == (uint32_t)last_known_block_index);

  std::cout << std::endl
    << (synced ? ColouredMsg("Synced ", Common::Console::Color::BrightGreen) : ColouredMsg("Syncing ", Common::Console::Color::BrightYellow)) 
    << ColouredMsg(std::to_string(height), Common::Console::Color::BrightWhite) << "/" << ColouredMsg(std::to_string(last_known_block_index), Common::Console::Color::BrightWhite)
    << " (" << ColouredMsg(std::to_string(get_sync_percentage(height, last_known_block_index)).substr(0, 5) + "%", Common::Console::Color::BrightWhite) << ") "
    << "on " << ColouredMsg((m_core.currency().isTestnet() ? "testnet" : "mainnet"), Common::Console::Color::BrightWhite) << ", "
    << "block v. " << ColouredMsg(std::to_string((int)majorVersion), Common::Console::Color::BrightWhite) << ", "
    << "last block: " << "\n"
    << ColouredMsg(Common::podToHex(last_block_hash), Common::Console::Color::BrightWhite) << ",\n"
    << "next difficulty: " << ColouredMsg(std::to_string(difficulty), Common::Console::Color::BrightWhite) << ", "
    << "est. network hashrate: " << ColouredMsg(get_mining_speed(hashrate), Common::Console::Color::BrightWhite) << ",\n"
    << "connections: " << ColouredMsg(std::to_string(outgoing_connections_count), Common::Console::Color::BrightWhite) << " OUT "
    << ColouredMsg(std::to_string(incoming_connections_count), Common::Console::Color::BrightWhite) << " INC "
    << ColouredMsg(std::to_string(rpc_conn), Common::Console::Color::BrightWhite) << " RPC, "
    << "peers: " << ColouredMsg(std::to_string(white_peerlist_size), Common::Console::Color::BrightWhite) << " white / "
    << ColouredMsg(std::to_string(grey_peerlist_size), Common::Console::Color::BrightWhite) << " grey,\n"
    << "mempool: " << ColouredMsg(std::to_string(tx_pool_size), Common::Console::Color::BrightWhite) << ", "
    << "alt. blocks: " << ColouredMsg(std::to_string(alt_blocks_count), Common::Console::Color::BrightWhite) << ", "
    << "uptime: " << ColouredMsg(Common::timeIntervalToString(uptime), Common::Console::Color::BrightWhite) << "\n"
    << std::endl << std::endl;
  
  return true;
}

//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_pl(const std::vector<std::string>& args) {
  m_srv.log_peerlist();
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::show_hr(const std::vector<std::string>& args)
{
  if (!m_core.get_miner().is_mining())
  {
    std::cout << "Mining is not started. You need to start mining before you can see hash rate." << ENDL;
  } else
  {
    m_core.get_miner().do_print_hashrate(true);
  }
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::hide_hr(const std::vector<std::string>& args)
{
  m_core.get_miner().do_print_hashrate(false);
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_bc_outs(const std::vector<std::string>& args)
{
  if (args.size() != 1)
  {
    std::cout << "need file path as parameter" << ENDL;
    return true;
  }
  m_core.print_blockchain_outs(args[0]);
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_cn(const std::vector<std::string>& args)
{
  m_srv.get_payload_object().log_connections();
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_dand(const std::vector<std::string>& args)
{
  protocolQuery.printDandelions();
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_bc(const std::vector<std::string> &args) {
  if (!args.size()) {
    std::cout << "need block index parameter" << ENDL;
    return false;
  }

  uint32_t start_index = 0;
  uint32_t end_index = 0;
  uint32_t end_block_parametr = m_core.getCurrentBlockchainHeight();
  if (!Common::fromString(args[0], start_index)) {
    std::cout << "wrong starter block index parameter" << ENDL;
    return false;
  }

  if (args.size() > 1 && !Common::fromString(args[1], end_index)) {
    std::cout << "wrong end block index parameter" << ENDL;
    return false;
  }

  if (end_index == 0) {
    end_index = end_block_parametr;
  }

  if (end_index > end_block_parametr) {
    std::cout << "end block index parameter shouldn't be greater than " << end_block_parametr << ENDL;
    return false;
  }

  if (end_index <= start_index) {
    std::cout << "end block index should be greater than starter block index" << ENDL;
    return false;
  }

  m_core.print_blockchain(start_index, end_index);
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_height(const std::vector<std::string> &args) {
  logger(Logging::INFO) << "Height: " << m_core.getCurrentBlockchainHeight() << std::endl;
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_bci(const std::vector<std::string>& args)
{
  m_core.print_blockchain_index();
  return true;
}

bool DaemonCommandsHandler::set_log(const std::vector<std::string>& args)
{
  if (args.size() != 1) {
    std::cout << "use: set_log <log_level_number_0-4>" << ENDL;
    return true;
  }

  uint16_t l = 0;
  if (!Common::fromString(args[0], l)) {
    std::cout << "wrong number format, use: set_log <log_level_number_0-4>" << ENDL;
    return true;
  }

  ++l;

  if (l > Logging::TRACE) {
    std::cout << "wrong number range, use: set_log <log_level_number_0-4>" << ENDL;
    return true;
  }

  m_logManager.setMaxLevel(static_cast<Logging::Level>(l));
  return true;
}

//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_block_by_height(uint32_t height)
{
  std::list<MevaCoin::Block> blocks;
  m_core.get_blocks(height, 1, blocks);

  if (1 == blocks.size()) {
    std::cout << "block_id: " << get_block_hash(blocks.front()) << ENDL;
    print_as_json(blocks.front());
  } else {
    uint32_t current_height;
    Crypto::Hash top_id;
    m_core.get_blockchain_top(current_height, top_id);
    std::cout << "block wasn't found. Current block chain height: " << current_height << ", requested: " << height << std::endl;
    return false;
  }

  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_block_by_hash(const std::string& arg)
{
  Crypto::Hash block_hash;
  if (!parse_hash256(arg, block_hash)) {
    return false;
  }

  std::list<Crypto::Hash> block_ids;
  block_ids.push_back(block_hash);
  std::list<MevaCoin::Block> blocks;
  std::list<Crypto::Hash> missed_ids;
  m_core.get_blocks(block_ids, blocks, missed_ids);

  if (1 == blocks.size())
  {
    print_as_json(blocks.front());
  } else
  {
    std::cout << "block wasn't found: " << arg << std::endl;
    return false;
  }

  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_block(const std::vector<std::string> &args) {
  if (args.empty()) {
    std::cout << "expected: print_block (<block_hash> | <block_height>)" << std::endl;
    return true;
  }

  const std::string &arg = args.front();
  try {
    uint32_t height = boost::lexical_cast<uint32_t>(arg);
    print_block_by_height(height);
  } catch (boost::bad_lexical_cast &) {
    print_block_by_hash(arg);
  }

  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_tx(const std::vector<std::string>& args)
{
  if (args.empty()) {
    std::cout << "expected: print_tx <transaction hash>" << std::endl;
    return true;
  }

  const std::string &str_hash = args.front();
  Crypto::Hash tx_hash;
  if (!parse_hash256(str_hash, tx_hash)) {
    return true;
  }

  std::vector<Crypto::Hash> tx_ids;
  tx_ids.push_back(tx_hash);
  std::list<MevaCoin::Transaction> txs;
  std::list<Crypto::Hash> missed_ids;
  m_core.getTransactions(tx_ids, txs, missed_ids, true);

  if (1 == txs.size()) {
    print_as_json(txs.front());
  } else {
    std::cout << "transaction wasn't found: <" << str_hash << '>' << std::endl;
  }

  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_pool(const std::vector<std::string>& args)
{
  logger(Logging::INFO) << "Pool state: " << ENDL << m_core.print_pool(false);
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_pool_sh(const std::vector<std::string>& args)
{
  logger(Logging::INFO) << "Pool state: " << ENDL << m_core.print_pool(true);
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_diff(const std::vector<std::string>& args)
{
  logger(Logging::INFO) << "Difficulty for next block: " << m_core.getNextBlockDifficulty() << std::endl;
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_pool_count(const std::vector<std::string>& args)
{
  logger(Logging::INFO) << "Pending transactions in mempool: " << m_core.getPoolTransactionsCount() << std::endl;
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::start_mining(const std::vector<std::string> &args) {
  if (!args.size()) {
    std::cout << "Please, specify wallet address to mine for: start_mining <spend key> <view key> [threads=1]" << std::endl;
    return true;
  }

  MevaCoin::AccountKeys keys = boost::value_initialized<MevaCoin::AccountKeys>();

  Crypto::Hash private_key_hash;
  size_t size;
  if (!Common::fromHex(args.front(), &private_key_hash, sizeof(private_key_hash), size) || size != sizeof(private_key_hash)) {
    logger(Logging::INFO) << "could not parse private spend key";
    return false;
  }
  keys.spendSecretKey = *(struct Crypto::SecretKey *) &private_key_hash;

  if (!Common::fromHex(args[1], &private_key_hash, sizeof(private_key_hash), size) || size != sizeof(private_key_hash)) {
    logger(Logging::INFO) << "could not parse private view key";
    return false;
  }
  keys.viewSecretKey = *(struct Crypto::SecretKey *) &private_key_hash;

  Crypto::secret_key_to_public_key(keys.spendSecretKey, keys.address.spendPublicKey);
  Crypto::secret_key_to_public_key(keys.viewSecretKey, keys.address.viewPublicKey);

  size_t threads_count = 1;
  if (args.size() > 2) {
    bool ok = Common::fromString(args[2], threads_count);
    threads_count = (ok && 0 < threads_count) ? threads_count : 1;
  }

  m_core.get_miner().start(keys, threads_count);
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::stop_mining(const std::vector<std::string>& args) {
  m_core.get_miner().stop();
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_ban(const std::vector<std::string>& args) {
  m_srv.log_banlist();
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::ban(const std::vector<std::string>& args)
{
  if (args.size() != 1 && args.size() != 2) return false;
  std::string addr = args[0];
  uint32_t ip;
  time_t seconds = std::numeric_limits<time_t>::max();
  try {
    if (args.size() > 1) {
      seconds = std::stoi(args[1]);
      if (seconds == 0) {
        logger(Logging::ERROR) << "Invalid ban duration. Should be greater than zero.";
        return false;
      }
    }
    ip = Common::stringToIpAddress(addr);
    if (!ip) {
      logger(Logging::ERROR) << "Invalid IP address: " << addr;
      return false;
    }
  }
  catch (const std::exception &e) {
    logger(Logging::ERROR) << "Failed to parse ban parameters: " << e.what();
    return false;
  }
  return m_srv.ban_host(ip, seconds);
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::unban(const std::vector<std::string>& args)
{
  if (args.size() != 1) return false;
  std::string addr = args[0];
  uint32_t ip = Common::stringToIpAddress(addr);
  if (!ip) {
    logger(Logging::ERROR) << "Invalid IP address: " << addr;
    return false;
  }
  return m_srv.unban_host(ip);
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::save(const std::vector<std::string>& args) {
  return m_core.saveBlockchain();
}

