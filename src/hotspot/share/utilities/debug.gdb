# Copyright (c) 2021, Alibaba Group Holding Limited. All rights reserved.
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
#
# This code is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 only, as
# published by the Free Software Foundation.
#
# This code is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# version 2 for more details (a copy is included in the LICENSE file that
# accompanied this code).
#
# You should have received a copy of the GNU General Public License version
# 2 along with this work; if not, write to the Free Software Foundation,
# Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
#
# Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
# or visit www.oracle.com if you need additional information or have any
# questions.
#

printf "Loading predefined GDB functions for HotSpot VM...\n"


# Print Symbol
define print_s
    if $arg0 == 0
        printf "invalid argument\n"
    else
        set $i = 0
        set $len = ((Symbol*)$arg0)->_length
        while $i < $len 
            printf "%c", ((Symbol*)$arg0)->_body[$i]
            set $i = $i + 1
        end
        printf "\n"
    end
end

# Print InstanceKlass
define print_ik
    set $ik_name = ((InstanceKlass*)$arg0)->_name
    printf "name: "
    print_symbol $ik_name

    set $ik_cld = ((InstanceKlass*)$arg0)->_class_loader_data
    if $ik_cld != 0
        printf "cld:  "
        if $ik_cld->_class_loader_klass == 0
            printf "bootstrap\n"
        else
            if $ik_cld->_name_and_id != 0
                print_symbol $ik_cld->_name_and_id
            else
                print_symbol $ik_cld->_class_loader_klass->_name
            end
        end
    end 
end

# Print Method
define print_m()
end