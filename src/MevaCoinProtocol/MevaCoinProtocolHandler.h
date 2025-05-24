// Copyright (c) 2012-2016, The MevaCoin developers, The Bytecoin developers
// Copyright (c) 2016-2022, The Karbo developers
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

#include <atomic>

#include <Common/ObserverManager.h>

#include "MevaCoinCore/ICore.h"
#include "MevaCoinCore/OnceInInterval.h"

#include "MevaCoinProtocol/MevaCoinProtocolDefinitions.h"
#include "MevaCoinProtocol/MevaCoinProtocolHandlerCommon.h"
#include "MevaCoinProtocol/IMevaCoinProtocolObserver.h"
#include "MevaCoinProtocol/IMevaCoinProtocolQuery.h"

#include "P2p/P2pProtocolDefinitions.h"
#include "P2p/NetNodeCommon.h"
#include "P2p/ConnectionContext.h"

#include <Logging/LoggerRef.h>

#define CURRENCY_PROTOCOL_MAX_OBJECT_REQUEST_COUNT 500

namespace System {
  class Dispatcher;
}

namespace MevaCoin
{
  class Currency;

  class StemPool {
  public:

    size_t getTransactionsCount() {
      std::lock_guard<std::recursive_mutex> lk(m_stempool_mutex);
      return m_stempool.size();
    }

    bool hasTransactions() {
      std::lock_guard<std::recursive_mutex> lk(m_stempool_mutex);
      return m_stempool.empty();
    }

    bool hasTransaction(const Crypto::Hash& txid) {
      std::lock_guard<std::recursive_mutex> lk(m_stempool_mutex);
      return m_stempool.find(txid) != m_stempool.end();
    }

    bool addTransaction(const Crypto::Hash& txid, std::string tx_blob) {
      std::lock_guard<std::recursive_mutex> lk(m_stempool_mutex);
      auto r = m_stempool.insert(tx_blob_by_hash::value_type(txid, tx_blob));

      return r.second;
    }

    bool removeTransaction(const Crypto::Hash& txid) {
      std::lock_guard<std::recursive_mutex> lk(m_stempool_mutex);

      if (m_stempool.find(txid) != m_stempool.end()) {
        m_stempool.erase(txid);
        return true;
      }

      return false;
    }

    std::vector<std::pair<Crypto::Hash, std::string>> getTransactions() {
      std::lock_guard<std::recursive_mutex> lk(m_stempool_mutex);
      std::vector<std::pair<Crypto::Hash, std::string>> txs;
      for (const auto & s : m_stempool) {
        txs.push_back(std::make_pair(s.first, s.second));
      }

      return txs;
    }

    void clearStemPool() {
      std::lock_guard<std::recursive_mutex> lk(m_stempool_mutex);

      m_stempool.clear();
    }

  private:
    typedef std::unordered_map<Crypto::Hash, std::string> tx_blob_by_hash;
    tx_blob_by_hash m_stempool;
    std::recursive_mutex m_stempool_mutex;
  };

  class MevaCoinProtocolHandler : 
    public i_mevacoin_protocol, 
    public IMevaCoinProtocolQuery
  {
  public:


    struct parsed_block_entry
    {
      Block block;
      std::vector<BinaryArray> txs;

      void serialize(ISerializer& s) {
        KV_MEMBER(block);
        KV_MEMBER(txs);
      }
    };

    MevaCoinProtocolHandler(const Currency& currency, System::Dispatcher& dispatcher, ICore& rcore, IP2pEndpoint* p_net_layout, Logging::ILogger& log);

    virtual bool addObserver(IMevaCoinProtocolObserver* observer) override;
    virtual bool removeObserver(IMevaCoinProtocolObserver* observer) override;

    void set_p2p_endpoint(IP2pEndpoint* p2p);
    // ICore& get_core() { return m_core; }
    virtual bool isSynchronized() const override { return m_synchronized; }
    void log_connections();
    virtual bool getConnections(std::vector<MevaCoinConnectionContext>& connections) const override;

    // Interface t_payload_net_handler, where t_payload_net_handler is template argument of nodetool::node_server
    void stop();
    bool start_sync(MevaCoinConnectionContext& context);
    bool on_idle();
    void onConnectionOpened(MevaCoinConnectionContext& context);
    void onConnectionClosed(MevaCoinConnectionContext& context);
    bool get_stat_info(core_stat_info& stat_inf);
    bool get_payload_sync_data(CORE_SYNC_DATA& hshd);
    bool process_payload_sync_data(const CORE_SYNC_DATA& hshd, MevaCoinConnectionContext& context, bool is_inital);
    int handleCommand(bool is_notify, int command, const BinaryArray& in_buff, BinaryArray& buff_out, MevaCoinConnectionContext& context, bool& handled);
    virtual size_t getPeerCount() const override;
    virtual uint32_t getObservedHeight() const override;
    void requestMissingPoolTransactions(const MevaCoinConnectionContext& context);
    bool select_dandelion_stem();
    bool fluffStemPool();
    void printDandelions() const override;

    std::atomic<bool> m_init_select_dandelion_called;

  private:
    //----------------- commands handlers ----------------------------------------------
    int handle_notify_new_block(int command, NOTIFY_NEW_BLOCK::request& arg, MevaCoinConnectionContext& context);
    int handle_notify_new_transactions(int command, NOTIFY_NEW_TRANSACTIONS::request& arg, MevaCoinConnectionContext& context);
    int handle_request_get_objects(int command, NOTIFY_REQUEST_GET_OBJECTS::request& arg, MevaCoinConnectionContext& context);
    int handle_response_get_objects(int command, NOTIFY_RESPONSE_GET_OBJECTS::request& arg, MevaCoinConnectionContext& context);
    int handle_request_chain(int command, NOTIFY_REQUEST_CHAIN::request& arg, MevaCoinConnectionContext& context);
    int handle_response_chain_entry(int command, NOTIFY_RESPONSE_CHAIN_ENTRY::request& arg, MevaCoinConnectionContext& context);
    int handle_request_tx_pool(int command, NOTIFY_REQUEST_TX_POOL::request& arg, MevaCoinConnectionContext& context);
    int handle_notify_new_lite_block(int command, NOTIFY_NEW_LITE_BLOCK::request &arg, MevaCoinConnectionContext &context);
    int handle_notify_missing_txs(int command, NOTIFY_MISSING_TXS::request &arg, MevaCoinConnectionContext &context);

    //----------------- i_mevacoin_protocol ----------------------------------
    virtual void relay_block(NOTIFY_NEW_BLOCK::request& arg) override;
    virtual void relay_transactions(NOTIFY_NEW_TRANSACTIONS::request& arg) override;

    //----------------------------------------------------------------------------------
    uint32_t get_current_blockchain_height();
    bool request_missing_objects(MevaCoinConnectionContext& context, bool check_having_blocks);
    bool on_connection_synchronized();
    void updateObservedHeight(uint32_t peerHeight, const MevaCoinConnectionContext& context);
    void recalculateMaxObservedHeight(const MevaCoinConnectionContext& context);
    int processObjects(MevaCoinConnectionContext& context, const std::vector<parsed_block_entry>& blocks);
    Logging::LoggerRef logger;

  private:
    int doPushLiteBlock(NOTIFY_NEW_LITE_BLOCK::request block, MevaCoinConnectionContext &context, std::vector<BinaryArray> missingTxs);

    System::Dispatcher& m_dispatcher;
    ICore& m_core;
    const Currency& m_currency;

    p2p_endpoint_stub m_p2p_stub;
    IP2pEndpoint* m_p2p;
    std::atomic<bool> m_synchronized;
    std::atomic<bool> m_stop;
    std::recursive_mutex m_sync_lock;

    mutable std::mutex m_observedHeightMutex;
    uint32_t m_observedHeight;

    std::atomic<size_t> m_peersCount;
    Tools::ObserverManager<IMevaCoinProtocolObserver> m_observerManager;

    OnceInInterval m_dandelionStemSelectInterval;
    OnceInInterval m_dandelionStemFluffInterval;
    std::vector<MevaCoinConnectionContext> m_dandelion_stem;

    StemPool m_stemPool;
  };
}
