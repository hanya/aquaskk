/* -*- C++ -*-

   MacOS X implementation of the SKK input method.

   Copyright (C) 2007 Tomotaka SUWA <t.suwa@mac.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef MessageId_h
#define MessageId_h

enum {
    IMP_COMP_MINIMUM_LENGTH = 'Icml',
    IMP_COMP_DYNAMIC_RANGE = 'Icdr',
    
    IMP_CONV_MAX_INLINE_CANDIDATE = 'Icmi',

    IMP_DICT_USER_LOCATION = 'Idul',
    IMP_DICT_LOCATION = 'Idlc',
    IMP_DICT_ADD = 'Idad',
    IMP_DICT_REMOVE = 'Idrm',
    IMP_DICT_UP = 'Idup',
    IMP_DICT_DOWN = 'Iddn',
    IMP_DICT_DOWNLOAD = 'Iddw',
    
    IMP_DISPLAY_CUSTOM_FONT = 'Idcf',
};

#endif
