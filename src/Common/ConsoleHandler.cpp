// Copyright (c) 2011-2016 The Mevacoin developers
// Copyright (c) 2014-2016 XDN developers
// Copyright (c) 2006-2013 Andrey N.Sabelnikov, www.sabelnikov.net
// Copyright (c) 2020-2022, The Talleo developers
// Copyright (c) 2016-2012 The Karbowanec developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "ConsoleHandler.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#else
#include <unistd.h>
#include <stdio.h>
#endif

#include <boost/algorithm/string.hpp>

using Common::Console::Color;

namespace Common {

/////////////////////////////////////////////////////////////////////////////
// AsyncConsoleReader
/////////////////////////////////////////////////////////////////////////////
AsyncConsoleReader::AsyncConsoleReader() : m_stop(true) {
}

AsyncConsoleReader::~AsyncConsoleReader() {
  stop();
}

void AsyncConsoleReader::start() {
  m_stop = false;
  m_thread = std::thread(std::bind(&AsyncConsoleReader::consoleThread, this));
}

bool AsyncConsoleReader::getline(std::string& line) {
  return m_queue.pop(line);
}

void AsyncConsoleReader::pause() {
  if (m_stop) {
    return;
  }

  m_stop = true;

  if (m_thread.joinable()) {
    m_thread.join();
  }

  m_thread = std::thread();
}

void AsyncConsoleReader::unpause() {
  start();
} 

void AsyncConsoleReader::stop() {

  if (m_stop) {
    return; // already stopping/stopped
  }

  m_stop = true;
  m_queue.close();
#ifdef _WIN32
  ::CloseHandle(::GetStdHandle(STD_INPUT_HANDLE));
#endif

  if (m_thread.joinable()) {
    m_thread.join();
  }

  m_thread = std::thread();
}

bool AsyncConsoleReader::stopped() const {
  return m_stop;
}

void AsyncConsoleReader::consoleThread() {

  while (waitInput()) {
    std::string line;

    if (!std::getline(std::cin, line)) {
      break;
    }

    if (!m_queue.push(line)) {
      break;
    }
  }
}

bool AsyncConsoleReader::waitInput() {
#ifndef _WIN32
  #if defined(__OpenBSD__) || defined(__ANDROID__)
    int stdin_fileno = fileno(stdin);
  #else
    int stdin_fileno = ::fileno(stdin);
  #endif

  while (!m_stop) {
    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(stdin_fileno, &read_set);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100 * 1000;
 
    int retval = ::select(stdin_fileno + 1, &read_set, NULL, NULL, &tv);

    if (retval == -1 && errno == EINTR) {
      continue;
    }

    if (retval < 0) {
      return false;
    }

    if (retval > 0) {
      return true;
    }
  }
#else
  while (!m_stop.load(std::memory_order_relaxed))
  {
    DWORD retval = ::WaitForSingleObject(::GetStdHandle(STD_INPUT_HANDLE), 100);
    switch (retval)
    {
      case WAIT_FAILED:
        return false;
      case WAIT_OBJECT_0:
        return true;
      default:
        break;
    }
  }
#endif

  return !m_stop;
}

/////////////////////////////////////////////////////////////////////////////
// ConsoleHandler
/////////////////////////////////////////////////////////////////////////////
ConsoleHandler::~ConsoleHandler() {
  stop();
}

void ConsoleHandler::start(bool startThread, const std::string& prompt, Console::Color promptColor) {
  m_prompt = prompt;
  m_promptColor = promptColor;
  m_consoleReader.start();

  if (startThread) {
    m_thread = std::thread(std::bind(&ConsoleHandler::handlerThread, this));
  } else {
    handlerThread();
  }
}

void ConsoleHandler::stop() {
  requestStop();
  wait();
}

void ConsoleHandler::pause() {
  m_consoleReader.pause();
}

void ConsoleHandler::unpause() {
  m_consoleReader.unpause();
}
  
void ConsoleHandler::wait() {

  try {
    if (m_thread.joinable() && m_thread.get_id() != std::this_thread::get_id()) {
      m_thread.join();
    }
  } catch (std::exception& e) {
    std::cerr << "Exception in ConsoleHandler::wait - " << e.what() << std::endl;
  }
}

void ConsoleHandler::requestStop() {
  m_consoleReader.stop();
}

std::string ConsoleHandler::getUsage() const {

  if (m_handlers.empty()) {
    return std::string();
  }
  
  std::stringstream ss;

  size_t maxlen = std::max_element(m_handlers.begin(), m_handlers.end(), [](
    CommandHandlersMap::const_reference& a, CommandHandlersMap::const_reference& b) { 
      return a.first.size() < b.first.size(); })->first.size();

  for (auto& x : m_handlers) {
    ss << std::left << std::setw(maxlen + 3) << x.first << x.second.second << std::endl;
  }

  return ss.str();
}

void ConsoleHandler::setHandler(const std::string& command, const ConsoleCommandHandler& handler, const std::string& usage) {
  m_handlers[command] = std::make_pair(handler, usage);
}

bool ConsoleHandler::runCommand(const std::vector<std::string>& cmdAndArgs) {
  if (cmdAndArgs.size() == 0) {
    return false;
  }

  const auto& cmd = cmdAndArgs.front();
  auto hIter = m_handlers.find(cmd);

  if (hIter == m_handlers.end()) {
    std::cout << "Unknown command: " << cmd << std::endl;
    return false;
  }

  std::vector<std::string> args(cmdAndArgs.begin() + 1, cmdAndArgs.end());
  hIter->second.first(args);
  return true;
}

void ConsoleHandler::handleCommand(const std::string& cmd) {
  bool parseString = false;
  std::string arg;
  std::vector<std::string> argList;

  for (auto ch : cmd) {
    switch (ch) {
    case ' ':
      if (parseString) {
        arg += ch;
      } else if (!arg.empty()) {
        argList.emplace_back(std::move(arg));
        arg.clear();
      }
      break;

    case '"':
      if (!arg.empty()) {
        argList.emplace_back(std::move(arg));
        arg.clear();
      }

      parseString = !parseString;
      break;

    default:
      arg += ch;
    }
  }

  if (!arg.empty()) {
    argList.emplace_back(std::move(arg));
  }

  runCommand(argList);
}

void ConsoleHandler::handlerThread() {
  std::string line;

  while(!m_consoleReader.stopped()) {
    try {
      if (!m_prompt.empty()) {
        if (m_promptColor != Color::Default) {
          Console::setTextColor(m_promptColor);
        }

        std::cout << m_prompt;
        std::cout.flush();

        if (m_promptColor != Color::Default) {
          Console::setTextColor(Color::Default);
        }
      }

      if (!m_consoleReader.getline(line)) {
        break;
      }

      boost::algorithm::trim(line);
      if (!line.empty()) {
        handleCommand(line);
      }

    } catch (std::exception&) {
      // ignore errors
    }
  }
}

}
