/*
 * opencog/atomspace/TLB.cc
 *
 * Copyright (C) 2002-2007 Novamente LLC
 * All Rights Reserved
 *
 * Written by Thiago Maia <thiago@vettatech.com>
 *            Andre Senna <senna@vettalabs.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "TLB.h"

#include <opencog/util/platform.h>
#include <opencog/atomspace/type_codes.h>

#ifdef USE_TLB_MAP

using namespace opencog;

// Low-lying values are reserved for "non-real" atoms.
// Leave a large gap between the highest "non-real" atom,
// and the first "real" handle.  This allows new atom types
// to be added, while still not overlapping with handles
// in older persistent storage.
unsigned long TLB::uuid = NOTYPE + 1000;

std::map<Handle, Atom *> TLB::handle_map;
std::map<Atom *, Handle> TLB::atom_map;

#endif
