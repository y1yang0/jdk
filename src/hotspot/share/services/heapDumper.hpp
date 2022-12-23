/*
 * Copyright (c) 2005, 2021, Oracle and/or its affiliates. All rights reserved.
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

#ifndef SHARE_SERVICES_HEAPDUMPER_HPP
#define SHARE_SERVICES_HEAPDUMPER_HPP

#include "memory/allocation.hpp"
#include "oops/oop.hpp"
#include "runtime/os.hpp"
#include "services/heapDumperCompression.hpp"

// HeapDumper is used to dump the java heap to file in HPROF binary format:
//
//  { HeapDumper dumper(true /* full GC before heap dump */);
//    if (dumper.dump("/export/java.hprof")) {
//      ResourceMark rm;
//      tty->print_cr("Dump failed: %s", dumper.error_as_C_string());
//    } else {
//      // dump succeeded
//    }
//  }
//

class outputStream;
// HPROF tags

enum hprofTag : u1 {
  // top-level records
  HPROF_UTF8                    = 0x01,
  HPROF_LOAD_CLASS              = 0x02,
  HPROF_UNLOAD_CLASS            = 0x03,
  HPROF_FRAME                   = 0x04,
  HPROF_TRACE                   = 0x05,
  HPROF_ALLOC_SITES             = 0x06,
  HPROF_HEAP_SUMMARY            = 0x07,
  HPROF_START_THREAD            = 0x0A,
  HPROF_END_THREAD              = 0x0B,
  HPROF_HEAP_DUMP               = 0x0C,
  HPROF_CPU_SAMPLES             = 0x0D,
  HPROF_CONTROL_SETTINGS        = 0x0E,
  HPROF_INSTANCE_KLASS          = 0x0F,
  HPROF_ARRAY_KLASS             = 0x10,
  HPROF_METHOD                  = 0x11,
  HPROF_KLASS_LOADER            = 0x12,

  // 1.0.2 record types
  HPROF_HEAP_DUMP_SEGMENT       = 0x1C,
  HPROF_HEAP_DUMP_END           = 0x2C,

  // field types
  HPROF_ARRAY_OBJECT            = 0x01,
  HPROF_NORMAL_OBJECT           = 0x02,
  HPROF_BOOLEAN                 = 0x04,
  HPROF_CHAR                    = 0x05,
  HPROF_FLOAT                   = 0x06,
  HPROF_DOUBLE                  = 0x07,
  HPROF_BYTE                    = 0x08,
  HPROF_SHORT                   = 0x09,
  HPROF_INT                     = 0x0A,
  HPROF_LONG                    = 0x0B,

  // data-dump sub-records
  HPROF_GC_ROOT_UNKNOWN         = 0xFF,
  HPROF_GC_ROOT_JNI_GLOBAL      = 0x01,
  HPROF_GC_ROOT_JNI_LOCAL       = 0x02,
  HPROF_GC_ROOT_JAVA_FRAME      = 0x03,
  HPROF_GC_ROOT_NATIVE_STACK    = 0x04,
  HPROF_GC_ROOT_STICKY_CLASS    = 0x05,
  HPROF_GC_ROOT_THREAD_BLOCK    = 0x06,
  HPROF_GC_ROOT_MONITOR_USED    = 0x07,
  HPROF_GC_ROOT_THREAD_OBJ      = 0x08,
  HPROF_GC_CLASS_DUMP           = 0x20,
  HPROF_GC_INSTANCE_DUMP        = 0x21,
  HPROF_GC_OBJ_ARRAY_DUMP       = 0x22,
  HPROF_GC_PRIM_ARRAY_DUMP      = 0x23
};

// Supports I/O operations for a dump
// Base class for dump and parallel dump
class AbstractDumpWriter : public StackObj {
 protected:
  enum {
    io_buffer_max_size = 1*M,
    io_buffer_max_waste = 10*K,
    dump_segment_header_size = 9
  };

  char* _buffer;    // internal buffer
  size_t _size;
  size_t _pos;

  bool _in_dump_segment; // Are we currently in a dump segment?
  bool _is_huge_sub_record; // Are we writing a sub-record larger than the buffer size?
  DEBUG_ONLY(size_t _sub_record_left;) // The bytes not written for the current sub-record.
  DEBUG_ONLY(bool _sub_record_ended;) // True if we have called the end_sub_record().

  virtual void flush(bool force = false) = 0;

  char* buffer() const                          { return _buffer; }
  size_t buffer_size() const                    { return _size; }
  void set_position(size_t pos)                 { _pos = pos; }

  // Can be called if we have enough room in the buffer.
  void write_fast(const void* s, size_t len);

  // Returns true if we have enough room in the buffer for 'len' bytes.
  bool can_write_fast(size_t len);

  void write_address(address a);

 public:
  AbstractDumpWriter() :
    _buffer(NULL),
    _size(io_buffer_max_size),
    _pos(0),
    _in_dump_segment(false) { }

  // total number of bytes written to the disk
  virtual julong bytes_written() const = 0;
  virtual char const* error() const = 0;

  size_t position() const                       { return _pos; }
  // writer functions
  virtual void write_raw(const void* s, size_t len);
  void write_u1(u1 x);
  void write_u2(u2 x);
  void write_u4(u4 x);
  void write_u8(u8 x);
  void write_objectID(oop o);
  void write_rootID(oop* p);
  void write_symbolID(Symbol* o);
  void write_classID(Klass* k);
  void write_id(u4 x);

  // Start a new sub-record. Starts a new heap dump segment if needed.
  void start_sub_record(u1 tag, u4 len);
  // Ends the current sub-record.
  void end_sub_record();
  // Finishes the current dump segment if not already finished.
  void finish_dump_segment(bool force_flush = false);
  // Refresh to get new buffer
  void refresh() {
    assert (_in_dump_segment ==false, "Sanity check");
    _buffer = NULL;
    _size = io_buffer_max_size;
    _pos = 0;
    // Force flush to guarantee data from parallel dumper are written.
    flush(true);
  }
  // Called when finished to release the threads.
  virtual void deactivate() = 0;
};

class SymbolTableDumper : public SymbolClosure {
 private:
  AbstractDumpWriter* _writer;
  AbstractDumpWriter* writer() const                { return _writer; }
 public:
  SymbolTableDumper(AbstractDumpWriter* writer)     { _writer = writer; }
  void do_symbol(Symbol** p);
};

// Supports I/O operations for a dump

class DumpWriter : public AbstractDumpWriter {
 private:
  CompressionBackend _backend; // Does the actual writing.
 protected:
  void flush(bool force = false) override;

 public:
  // Takes ownership of the writer and compressor.
  DumpWriter(AbstractWriter* writer, AbstractCompressor* compressor);

  // total number of bytes written to the disk
  julong bytes_written() const override { return (julong) _backend.get_written(); }

  char const* error() const override    { return _backend.error(); }

  // Called by threads used for parallel writing.
  void writer_loop()                    { _backend.thread_loop(); }
  // Called when finish to release the threads.
  void deactivate() override            { flush(); _backend.deactivate(); }
  // Get the backend pointer, used by parallel dump writer.
  CompressionBackend* backend_ptr()     { return &_backend; }

};

// Support class with a collection of functions used when dumping the heap

class DumperSupport : AllStatic {
 public:

  // write a header of the given type
  static void write_header(AbstractDumpWriter* writer, u1 tag, u4 len);

  // returns hprof tag for the given type signature
  static hprofTag sig2tag(Symbol* sig);
  // returns hprof tag for the given basic type
  static hprofTag type2tag(BasicType type);
  // Returns the size of the data to write.
  static u4 sig2size(Symbol* sig);

  // returns the size of the instance of the given class
  static u4 instance_size(Klass* k);

  // dump a jfloat
  static void dump_float(AbstractDumpWriter* writer, jfloat f);
  // dump a jdouble
  static void dump_double(AbstractDumpWriter* writer, jdouble d);
  // dumps the raw value of the given field
  static void dump_field_value(AbstractDumpWriter* writer, char type, oop obj, int offset);
  // returns the size of the static fields; also counts the static fields
  static u4 get_static_fields_size(InstanceKlass* ik, u2& field_count);
  // dumps static fields of the given class
  static void dump_static_fields(AbstractDumpWriter* writer, Klass* k);
  // dump the raw values of the instance fields of the given object
  static void dump_instance_fields(AbstractDumpWriter* writer, oop o);
  // get the count of the instance fields for a given class
  static u2 get_instance_fields_count(InstanceKlass* ik);
  // dumps the definition of the instance fields for a given class
  static void dump_instance_field_descriptors(AbstractDumpWriter* writer, Klass* k);
  // creates HPROF_GC_INSTANCE_DUMP record for the given object
  static void dump_instance(AbstractDumpWriter* writer, oop o);
  // creates HPROF_GC_CLASS_DUMP record for the given instance class
  static void dump_instance_class(AbstractDumpWriter* writer, Klass* k);
  // creates HPROF_GC_CLASS_DUMP record for a given array class
  static void dump_array_class(AbstractDumpWriter* writer, Klass* k);

  // creates HPROF_GC_OBJ_ARRAY_DUMP record for the given object array
  static void dump_object_array(AbstractDumpWriter* writer, objArrayOop array);
  // creates HPROF_GC_PRIM_ARRAY_DUMP record for the given type array
  static void dump_prim_array(AbstractDumpWriter* writer, typeArrayOop array);
  // create HPROF_FRAME record for the given method and bci
  static void dump_stack_frame(AbstractDumpWriter* writer, int frame_serial_num, int class_serial_num, Method* m, int bci);

  // check if we need to truncate an array
  static int calculate_array_max_length(AbstractDumpWriter* writer, arrayOop array, short header_size);

  // fixes up the current dump record and writes HPROF_HEAP_DUMP_END record
  static void end_of_dump(AbstractDumpWriter* writer);

  static oop mask_dormant_archived_object(oop o) {
    if (o != NULL && o->klass()->java_mirror() == NULL) {
      // Ignore this object since the corresponding java mirror is not loaded.
      // Might be a dormant archive object.
      return NULL;
    } else {
      return o;
    }
  }
};

class HeapDumper : public StackObj {
 private:
  char* _error;
  bool _gc_before_heap_dump;
  bool _oome;
  elapsedTimer _t;

  HeapDumper(bool gc_before_heap_dump, bool oome) :
    _error(NULL), _gc_before_heap_dump(gc_before_heap_dump), _oome(oome) { }

  // string representation of error
  char* error() const                   { return _error; }
  void set_error(char const* error);

  // internal timer.
  elapsedTimer* timer()                 { return &_t; }

  static void dump_heap(bool oome);

 public:
  HeapDumper(bool gc_before_heap_dump) :
    _error(NULL), _gc_before_heap_dump(gc_before_heap_dump), _oome(false) { }

  ~HeapDumper();

  // dumps the heap to the specified file, returns 0 if success.
  // additional info is written to out if not NULL.
  // compression >= 0 creates a gzipped file with the given compression level.
  // parallel_thread_num >= 0 indicates thread numbers of parallel object dump
  int dump(const char* path, outputStream* out = NULL, int compression = -1, bool overwrite = false, uint parallel_thread_num = 1);

  // returns error message (resource allocated), or NULL if no error
  char* error_as_C_string() const;

  static void dump_heap()    NOT_SERVICES_RETURN;

  static void dump_heap_from_oome()    NOT_SERVICES_RETURN;
};

#endif // SHARE_SERVICES_HEAPDUMPER_HPP
