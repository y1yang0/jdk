/*
 * Copyright (c) 2018, 2025, Oracle and/or its affiliates. All rights reserved.
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
 */


/*
 * @test
 *
 * @summary converted from VM Testbase vm/mlvm/cp/stress/classfmt/correctBootstrap.
 * VM Testbase keywords: [feature_mlvm]
 * VM Testbase readme:
 * DESCRIPTION
 *     Create a constant pool, which contains maximum number of invokedynamic constants and corresponding
 *     invokedynamic commands that refer to correct bootstrap method types. Verify that bootstrap/target
 *     methods are invoked. The maximum number of invokedynamic calls is determined dynamically.
 *
 * @library /testlibrary/asm
 * @library /vmTestbase
 *          /test/lib
 *          /vmTestbase/vm/mlvm/patches
 *
 * @comment patch for java.base
 * @build java.base/*
 *
 * @comment build generator
 * @build vm.mlvm.cp.share.GenManyIndyCorrectBootstrap
 *
 *
 * @comment build test class and indify classes
 * @build vm.mlvm.share.ClassfileGeneratorTest
 * @run driver vm.mlvm.share.IndifiedClassesBuilder
 * @ignore 8194951
 *
 * @run main/othervm/timeout=300
 *      vm.mlvm.share.ClassfileGeneratorTest
 *      -generator vm.mlvm.cp.share.GenManyIndyCorrectBootstrap
 */

