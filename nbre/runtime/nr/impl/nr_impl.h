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

#include "common/math/softfloat.hpp"
#include "runtime/stdrt.h"
#include <string>
#include <vector>

namespace neb {
namespace rt {
namespace nr {

using nr_float_t = float32;
std::string entry_point_nr_impl(uint64_t start_block, uint64_t end_block,
                                nr_float_t a, nr_float_t b, nr_float_t c,
                                nr_float_t d, int64_t mu, int64_t lambda);
} // namespace nr
} // namespace rt
} // namespace neb