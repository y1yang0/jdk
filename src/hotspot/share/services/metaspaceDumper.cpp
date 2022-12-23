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

#include "precompiled.hpp"
#include "classfile/classLoaderData.hpp"
#include "classfile/classLoaderData.inline.hpp"
#include "classfile/classLoaderDataGraph.hpp"
#include "classfile/javaClasses.inline.hpp"
#include "classfile/symbolTable.hpp"
#include "classfile/vmClasses.hpp"
#include "classfile/vmSymbols.hpp"
#include "jvm.h"
#include "memory/allocation.inline.hpp"
#include "memory/resourceArea.hpp"
#include "memory/universe.hpp"
#include "oops/klass.inline.hpp"
#include "oops/method.hpp"
#include "oops/instanceKlass.hpp"
#include "oops/instanceKlass.inline.hpp"
#include "oops/objArrayKlass.hpp"
#include "oops/objArrayOop.inline.hpp"
#include "oops/oop.inline.hpp"
#include "oops/fieldStreams.hpp"
#include "oops/fieldStreams.inline.hpp"
#include "oops/typeArrayOop.inline.hpp"
#include "runtime/handles.inline.hpp"
#include "runtime/os.hpp"
#include "runtime/reflectionUtils.hpp"
#include "runtime/threads.hpp"
#include "runtime/vframe.hpp"
#include "services/metaspaceDumper.hpp"
#include "services/heapDumper.hpp"
#include "services/heapDumperCompression.hpp"
#include "utilities/macros.hpp"
#include "utilities/ostream.hpp"

void VM_MetaspaceDumper::doit() {
  MetaspaceDumper::dump_metaspace();
}

class DumpCLDClosure : public CLDClosure {
private:
  DumpWriter* _writer;
public:
  DumpCLDClosure(DumpWriter* writer)
    :_writer(writer){}

  void do_cld(ClassLoaderData* cld);
};


class DumpClassClosure : public KlassClosure {
private:
  DumpWriter* _writer;
  void do_instance_klass(InstanceKlass* ik);
  void do_array_klass(ArrayKlass* ak);

public:
  DumpClassClosure(DumpWriter* writer)
    :_writer(writer){}

  void do_klass(Klass* k);
};

void DumpCLDClosure::do_cld(ClassLoaderData* cld) {
  ResourceMark rm;
  //DumperSupport::write_header(_writer, HPROF_KLASS_LOADER, 100);
  // cld name
  //_writer->write_symbolID(cld->name_and_id());
  //if (cld->name_and_id() != NULL) {
  //  tty->print_cr("==> %s", cld->name_and_id()->as_C_string());
  //}
}


void DumpClassClosure::do_array_klass(ArrayKlass* ak)  {
  DumperSupport::write_header(_writer, HPROF_ARRAY_KLASS, 100);
  // access flags
  _writer->write_u4(ak->access_flags().as_int());
  // id - class name ID
  Symbol* name = ak->name();
  _writer->write_symbolID(name);
  // id -  super class name ID
  InstanceKlass* super_ik = ak->java_super();
  if (super_ik != NULL) {
    name = ak->java_super()->name();
  } else {
    name = NULL;
  }
  _writer->write_symbolID(name);
  // dimension
  _writer->write_u4(ak->dimension());
}

void DumpClassClosure::do_instance_klass(InstanceKlass* ik)  {
  DumperSupport::write_header(_writer, HPROF_INSTANCE_KLASS, 100);
  // access flags
  _writer->write_u4(ik->access_flags().as_int());
  // id - class name ID
  Symbol* name = ik->name();
  _writer->write_symbolID(name);
  // id -  super class name ID
  InstanceKlass* super_ik = ik->java_super();
  if (super_ik != NULL) {
    name = ik->java_super()->name();
  } else {
    name = NULL;
  }
  _writer->write_symbolID(name);
  // u4 - num of interfaces
  Array<InstanceKlass*>* interfaces = ik->local_interfaces();
  _writer->write_u4(interfaces->length());
  for(int i=0;i<interfaces->length();i++){
    _writer->write_symbolID(interfaces->at(i)->name());
  }
  // u4 - num of fields
  _writer->write_u4(ik->java_fields_count());
  for (JavaFieldStream fs(ik); !fs.done(); fs.next()) {
    Symbol* sig = fs.signature();
    _writer->write_symbolID(fs.name());   // name
    _writer->write_u1(DumperSupport::sig2tag(sig));       // type
  }
  // u4 - num of methods
  _writer->write_u4(ik->methods()->length());
  Array<Method*>* methods = ik->methods();
  for(int i=0;i<methods->length();i++){
    _writer->write_symbolID(methods->at(i)->name());      // name
  }
}

void DumpClassClosure::do_klass(Klass* k)  {
  if (k->is_instance_klass()) {
    do_instance_klass(InstanceKlass::cast(k));
  } else {
    assert(k->is_array_klass(), "why not");
    do_array_klass(ArrayKlass::cast(k));
  }
}

static DumpWriter* mwriter = NULL;

void dump_method(Method* m) {
  DumperSupport::write_header(mwriter, HPROF_METHOD, 100);
  // method name
  mwriter->write_symbolID(m->name());
  // signature
  mwriter->write_symbolID(m->signature());
  // access flags
  mwriter->write_u4(m->access_flags().as_int());
  // max stack
  mwriter->write_u4(m->max_stack());
  // max locals
  mwriter->write_u4(m->max_locals());
  // size of params
  mwriter->write_u4(m->size_of_parameters());
  // num of bytecodes
  mwriter->write_u4(m->code_size());
  // byte codes
  u1* code_base = m->code_base();
  for(int i=0;i<m->code_size();i++) {
    mwriter->write_u1(*(u1*)(code_base + i));
  }
}

void MetaspaceDumper::dump(char* path) {
  DumpWriter writer(new (std::nothrow) FileWriter(path, true), NULL);
  const char* header = "JAVA PROFILE 1.0.3";
  // header is few bytes long - no chance to overflow int
  writer.write_raw(header, strlen(header) + 1); // NUL terminated
  writer.write_u4(oopSize);
  // timestamp is current time in ms
  writer.write_u8(os::javaTimeMillis());
  
  // HPROF_UTF8 records
  SymbolTableDumper sym_dumper(&writer);
  SymbolTable::symbols_do(&sym_dumper);

  DumpClassClosure klass_closure(&writer);
  ClassLoaderDataGraph::classes_do(&klass_closure);

  mwriter = &writer;
  ClassLoaderDataGraph::methods_do(dump_method);
  mwriter = NULL;

  DumpCLDClosure cld_closure(&writer);
  ClassLoaderDataGraph::cld_do(&cld_closure);

  // Deactive so that compression backend can be released safely
  writer.deactivate();
}

void MetaspaceDumper::dump_metaspace() {
  static char base_path[JVM_MAXPATHLEN] = {'\0'};
  static uint dump_file_seq = 0;
  char* my_path;
  const int max_digit_chars = 20;

  const char* dump_file_name = "metaspace";
  const char* dump_file_ext  = ".bin";
  const size_t dlen = strlen(base_path);  // if heap dump dir specified

  if (MetaspaceDumpPath != NULL) {
    jio_snprintf(&base_path[dlen], sizeof(base_path)-dlen, "%s%s",
                 MetaspaceDumpPath, dump_file_ext);
  } else {
    jio_snprintf(&base_path[dlen], sizeof(base_path)-dlen, "%s%d%s",
                 dump_file_name, os::current_process_id(), dump_file_ext);
  }
  const size_t len = strlen(base_path) + 1;
  my_path = (char*)os::malloc(len, mtInternal);
  if (my_path == NULL) {
    warning("Cannot create heap dump file.  Out of system memory.");
    return;
  }
  strncpy(my_path, base_path, len);

  MetaspaceDumper dumper;
  dumper.dump(my_path);
  os::free(my_path);
}
