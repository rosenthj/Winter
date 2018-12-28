/*
 *  Winter is a UCI chess engine.
 *
 *  Copyright (C) 2016 Jonas Kuratli, Jonathan Maurer, Jonathan Rosenthal
 *  Copyright (C) 2017-2018 Jonathan Rosenthal
 *
 *  Winter is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Winter is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * magic.h
 *
 *  Created on: Dec 16, 2016
 *      Author: Jonathan Rosenthal
 */

#ifndef GENERAL_MAGIC_H_
#define GENERAL_MAGIC_H_

#include "types.h"

namespace magic {

template<PieceType pt>
BitBoard GetAttackMap(const int &index, BitBoard AllPieces);
BitBoard GetAttackVectors(BitBoard src, BitBoard des);
BitBoard GetAttackMap(PieceType piece_type, Square square, BitBoard all_pieces);
int GetSquareDistance(const Square a, const Square b);
BitBoard GetKingArea(const Square square);
BitBoard GetSquareFile(const Square square);

}

#endif /* GENERAL_MAGIC_H_ */
