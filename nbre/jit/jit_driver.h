// Copyright (C) 2018 go-nebulas authors
//
// This file is part of the go-nebulas library.
//
// the go-nebulas library is free software: you can redistribute it and/or
// modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the go-nebulas library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the go-nebulas library.  If not, see
// <http://www.gnu.org/licenses/>.
//
#pragma once

#include "common/common.h"
#include "common/util/singleton.h"
#include "core/ir_warden.h"
#include "fs/proto/ir.pb.h"
#include "jit/jit_engine.h"

namespace neb {
namespace internal {
class jit_driver_impl;
}
namespace core {
class driver;
}
class jit_driver : public ::neb::util::singleton<jit_driver> {
public:
  jit_driver();
  ~jit_driver();

  template <typename RT, typename... ARGS>
  RT run_ir(const std::string &name, uint64_t height,
            const std::string &func_name, ARGS... args) {
    auto irs =
        core::ir_warden::instance().get_ir_by_name_height(name, height, false);
    if (irs.size() != 1) {
      throw std::invalid_argument("no such ir");
    }
    std::string key = gen_key(irs, func_name);
    auto ret = run_if_exists<RT>(irs.back(), func_name, args...);
    if (ret.first) {
      return ret.second;
    }

    irs = core::ir_warden::instance().get_ir_by_name_height(name, height, true);
    return run<RT>(key, irs, func_name, args...);
  }

  template <typename RT, typename... ARGS>
  std::pair<bool, RT> run_if_exists(const std::shared_ptr<nbre::NBREIR> &ir,
                                    const std::string &func_name,
                                    ARGS... args) {

    std::vector<std::shared_ptr<nbre::NBREIR>> irs;
    irs.push_back(ir);
    std::string key = gen_key(irs, func_name);
    std::unique_lock<std::mutex> _l(m_mutex);
    auto it = m_jit_instances.find(key);
    if (it == m_jit_instances.end())
      return std::make_pair(false, RT());

    auto &context = it->second;
    context->m_time_counter = 30 * 60;
    _l.unlock();
    return std::make_pair(true, context->m_jit.run<RT>(args...));
  }

  template <typename RT, typename... ARGS>
  RT run(const std::string &ir_key,
         const std::vector<std::shared_ptr<nbre::NBREIR>> &irs,
         const std::string &func_name, ARGS... args) {
    std::string key = ir_key;
    m_mutex.lock();
    auto it = m_jit_instances.find(key);
    if (it == m_jit_instances.end()) {
      shrink_instances();
      m_jit_instances.insert(std::make_pair(key, make_context(irs, func_name)));
      it = m_jit_instances.find(key);
    }
    auto &context = it->second;
    context->m_time_counter = 30 * 60;
    m_mutex.unlock();
    return context->m_jit.run<RT>(args...);
  }

  void timer_callback();

protected:
  void shrink_instances();

  std::string gen_key(const std::vector<std::shared_ptr<nbre::NBREIR>> &irs,
                      const std::string &func_name);

  struct jit_context {
    llvm::LLVMContext m_context;
    jit::jit_engine m_jit;
    int32_t m_time_counter;
  };

  std::unique_ptr<jit_driver::jit_context>
  make_context(const std::vector<std::shared_ptr<nbre::NBREIR>> &irs,
               const std::string &func_name);

protected:
  std::mutex m_mutex;
  std::unordered_map<std::string, std::unique_ptr<jit_context>> m_jit_instances;
}; // end class jit_driver;
} // end namespace neb
