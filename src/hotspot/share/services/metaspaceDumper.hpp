/*
 * Copyright (c) 2022, Alibaba Group Holding Limited. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#ifndef SHARE_SERVICES_METASPACEDUMPER_HPP
#define SHARE_SERVICES_METASPACEDUMPER_HPP

#include "gc/shared/gcVMOperations.hpp"
#include "memory/allocation.hpp"
#include "oops/oop.hpp"
#include "runtime/os.hpp"
#include "runtime/vmOperations.hpp"


class outputStream;

class MetaspaceDumper : public StackObj {
 private:
  void dump(char* path);

 public:
  MetaspaceDumper() { }

  ~ MetaspaceDumper(){}

  static void dump_metaspace()    NOT_SERVICES_RETURN;
};

class VM_MetaspaceDumper: public VM_GC_Operation {
 private:
  outputStream* _out;
  bool _full_gc;
  uint _parallel_thread_num;
 public:
  VM_MetaspaceDumper() :
    VM_GC_Operation(0 /* total collections,      dummy, ignored */,
                    GCCause::_heap_dump /* GC Cause */,
                    0 /* total full collections, dummy, ignored */,
                    false) {}

  ~VM_MetaspaceDumper() {}
  virtual VMOp_Type type() const { return VMOp_MetaspaceDumper; }
  virtual void doit();
};

#endif // SHARE_SERVICES_METASPACEDUMPER_HPP
