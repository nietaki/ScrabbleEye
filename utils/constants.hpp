/*
    ScrabbleEye - live Scrabble game analysis app
    Copyright (C) 2012 nietaki@gmail.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace se {
  //measured using Euro Cent coins, in mm
  const double TILE_WIDTH = 18.70;
  const double TILE_THICKNESS = 5.0;
  const int BOARD_SIZE = 15;
  const int TILE_PIXEL_WIDTH = 40;

  const int DEFAULT_WINDOW_WIDTH = 1024;
  const int DEFAULT_WINDOW_HEIGHT = 768;
  
  const int MEDIAN_SIZE = 3;
  const int GAUSSIAN_SIZE = 3;
  const double GAUSSIAN_SIGMA = 2.0;
}

#endif // CONSTANTS_HPP
