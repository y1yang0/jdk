/*
 * Copyright (c) 1997, 2023, Oracle and/or its affiliates. All rights reserved.
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

#ifndef SHARE_OPTO_DIVNODE_HPP
#define SHARE_OPTO_DIVNODE_HPP

#include "opto/multnode.hpp"
#include "opto/node.hpp"
#include "opto/opcodes.hpp"
#include "opto/type.hpp"

// Portions of code courtesy of Clifford Click

// Optimization - Graph Style

// Integer division
// Note: this is division as defined by JVMS, i.e., MinInt/-1 == MinInt.
// On processors which don't naturally support this special case (e.g., x86),
// the matcher or runtime system must take care of this.
class DivINode : public Node {
public:
  DivINode( Node *c, Node *dividend, Node *divisor ) : Node(c, dividend, divisor ) {}
  virtual int Opcode() const;
  virtual Node* Identity(PhaseGVN* phase);
  virtual Node *Ideal(PhaseGVN *phase, bool can_reshape);
  virtual const Type* Value(PhaseGVN* phase) const;
  virtual const Type *bottom_type() const { return TypeInt::INT; }
  virtual uint ideal_reg() const { return Op_RegI; }
};

// Long division
class DivLNode : public Node {
public:
  DivLNode( Node *c, Node *dividend, Node *divisor ) : Node(c, dividend, divisor ) {}
  virtual int Opcode() const;
  virtual Node* Identity(PhaseGVN* phase);
  virtual Node *Ideal(PhaseGVN *phase, bool can_reshape);
  virtual const Type* Value(PhaseGVN* phase) const;
  virtual const Type *bottom_type() const { return TypeLong::LONG; }
  virtual uint ideal_reg() const { return Op_RegL; }
};

// Float division
class DivFNode : public Node {
public:
  DivFNode( Node *c, Node *dividend, Node *divisor ) : Node(c, dividend, divisor) {}
  virtual int Opcode() const;
  virtual Node* Identity(PhaseGVN* phase);
  virtual Node *Ideal(PhaseGVN *phase, bool can_reshape);
  virtual const Type* Value(PhaseGVN* phase) const;
  virtual const Type *bottom_type() const { return Type::FLOAT; }
  virtual uint ideal_reg() const { return Op_RegF; }
};

// Double division
class DivDNode : public Node {
public:
  DivDNode( Node *c, Node *dividend, Node *divisor ) : Node(c,dividend, divisor) {}
  virtual int Opcode() const;
  virtual Node* Identity(PhaseGVN* phase);
  virtual Node *Ideal(PhaseGVN *phase, bool can_reshape);
  virtual const Type* Value(PhaseGVN* phase) const;
  virtual const Type *bottom_type() const { return Type::DOUBLE; }
  virtual uint ideal_reg() const { return Op_RegD; }
};

// Unsigned integer division
class UDivINode : public Node {
public:
  UDivINode( Node *c, Node *dividend, Node *divisor ) : Node(c, dividend, divisor ) {}
  virtual int Opcode() const;
  virtual Node* Identity(PhaseGVN* phase);
  virtual const Type* Value(PhaseGVN* phase) const;
  virtual Node *Ideal(PhaseGVN *phase, bool can_reshape);
  virtual const Type *bottom_type() const { return TypeInt::INT; }
  virtual uint ideal_reg() const { return Op_RegI; }
};

// Unsigned long division
class UDivLNode : public Node {
public:
  UDivLNode( Node *c, Node *dividend, Node *divisor ) : Node(c, dividend, divisor ) {}
  virtual int Opcode() const;
  virtual Node* Identity(PhaseGVN* phase);
  virtual const Type* Value(PhaseGVN* phase) const;
  virtual Node *Ideal(PhaseGVN *phase, bool can_reshape);
  virtual const Type *bottom_type() const { return TypeLong::LONG; }
  virtual uint ideal_reg() const { return Op_RegL; }
};

// Integer modulus
class ModINode : public Node {
public:
  ModINode( Node *c, Node *in1, Node *in2 ) : Node(c,in1, in2) {}
  virtual int Opcode() const;
  virtual const Type* Value(PhaseGVN* phase) const;
  virtual Node *Ideal(PhaseGVN *phase, bool can_reshape);
  virtual const Type *bottom_type() const { return TypeInt::INT; }
  virtual uint ideal_reg() const { return Op_RegI; }
};

// Long modulus
class ModLNode : public Node {
public:
  ModLNode( Node *c, Node *in1, Node *in2 ) : Node(c,in1, in2) {}
  virtual int Opcode() const;
  virtual const Type* Value(PhaseGVN* phase) const;
  virtual Node *Ideal(PhaseGVN *phase, bool can_reshape);
  virtual const Type *bottom_type() const { return TypeLong::LONG; }
  virtual uint ideal_reg() const { return Op_RegL; }
};

// Float Modulus
class ModFNode : public Node {
public:
  ModFNode( Node *c, Node *in1, Node *in2 ) : Node(c,in1, in2) {}
  virtual int Opcode() const;
  virtual const Type* Value(PhaseGVN* phase) const;
  virtual const Type *bottom_type() const { return Type::FLOAT; }
  virtual uint ideal_reg() const { return Op_RegF; }
};

// Double Modulus
class ModDNode : public Node {
public:
  ModDNode( Node *c, Node *in1, Node *in2 ) : Node(c, in1, in2) {}
  virtual int Opcode() const;
  virtual const Type* Value(PhaseGVN* phase) const;
  virtual const Type *bottom_type() const { return Type::DOUBLE; }
  virtual uint ideal_reg() const { return Op_RegD; }
};

// Unsigned integer modulus
class UModINode : public Node {
public:
  UModINode( Node *c, Node *in1, Node *in2 ) : Node(c,in1, in2) {}
  virtual int Opcode() const;
  virtual Node *Ideal(PhaseGVN *phase, bool can_reshape);
  virtual const Type *bottom_type() const { return TypeInt::INT; }
  virtual uint ideal_reg() const { return Op_RegI; }
};

// Unsigned long modulus
class UModLNode : public Node {
public:
  UModLNode( Node *c, Node *in1, Node *in2 ) : Node(c,in1, in2) {}
  virtual int Opcode() const;
  virtual Node *Ideal(PhaseGVN *phase, bool can_reshape);
  virtual const Type *bottom_type() const { return TypeLong::LONG; }
  virtual uint ideal_reg() const { return Op_RegL; }
};

// Division with remainder result.
class DivModNode : public MultiNode {
protected:
  DivModNode( Node *c, Node *dividend, Node *divisor );
public:
  enum {
    div_proj_num =  0,      // quotient
    mod_proj_num =  1       // remainder
  };
  virtual int Opcode() const;
  virtual Node* Identity(PhaseGVN* phase) { return this; }
  virtual Node *Ideal(PhaseGVN *phase, bool can_reshape) { return nullptr; }
  virtual const Type* Value(PhaseGVN* phase)  const { return bottom_type(); }
  virtual uint hash() const { return Node::hash(); }
  virtual bool is_CFG() const  { return false; }
  virtual uint ideal_reg() const { return NotAMachineReg; }

  ProjNode* div_proj() { return proj_out_or_null(div_proj_num); }
  ProjNode* mod_proj() { return proj_out_or_null(mod_proj_num); }
};

// Integer division with remainder result.
class DivModINode : public DivModNode {
public:
  DivModINode( Node *c, Node *dividend, Node *divisor ) : DivModNode(c, dividend, divisor) {}
  virtual int Opcode() const;
  virtual const Type *bottom_type() const { return TypeTuple::INT_PAIR; }
  virtual Node *match( const ProjNode *proj, const Matcher *m );

  // Make a divmod and associated projections from a div or mod.
  static DivModINode* make(Node* div_or_mod);
};

// Long division with remainder result.
class DivModLNode : public DivModNode {
public:
  DivModLNode( Node *c, Node *dividend, Node *divisor ) : DivModNode(c, dividend, divisor) {}
  virtual int Opcode() const;
  virtual const Type *bottom_type() const { return TypeTuple::LONG_PAIR; }
  virtual Node *match( const ProjNode *proj, const Matcher *m );

  // Make a divmod and associated projections from a div or mod.
  static DivModLNode* make(Node* div_or_mod);
};

// Unsigend integer division with remainder result.
class UDivModINode : public DivModNode {
public:
  UDivModINode( Node *c, Node *dividend, Node *divisor ) : DivModNode(c, dividend, divisor) {}
  virtual int Opcode() const;
  virtual const Type *bottom_type() const { return TypeTuple::INT_PAIR; }
  virtual Node *match( const ProjNode *proj, const Matcher *m );

  // Make a divmod and associated projections from a div or mod.
  static UDivModINode* make(Node* div_or_mod);
};

// Unsigned long division with remainder result.
class UDivModLNode : public DivModNode {
public:
  UDivModLNode( Node *c, Node *dividend, Node *divisor ) : DivModNode(c, dividend, divisor) {}
  virtual int Opcode() const;
  virtual const Type *bottom_type() const { return TypeTuple::LONG_PAIR; }
  virtual Node *match( const ProjNode *proj, const Matcher *m );

  // Make a divmod and associated projections from a div or mod.
  static UDivModLNode* make(Node* div_or_mod);
};

#endif // SHARE_OPTO_DIVNODE_HPP
