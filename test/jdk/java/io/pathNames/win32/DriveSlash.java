/*
 * Copyright (c) 1998, 2025, Oracle and/or its affiliates. All rights reserved.
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

/* @test
   @bug 4065189
   @summary Check that win32 pathnames of the form "C:\\"
            can be listed by the File.list method
   @requires (os.family == "windows")
   @author Mark Reinhold
 */

import java.io.*;


public class DriveSlash {

    public static void main(String[] args) throws Exception {

        File f = new File("c:\\");
        System.err.println(f.getCanonicalPath());
        String[] fs = f.list();
        if (fs == null) {
            throw new Exception("File.list returned null");
        }
        for (int i = 0; i < fs.length; i++) {
            System.err.print(" " + fs[i]);
        }
        System.err.println();
    }

}
