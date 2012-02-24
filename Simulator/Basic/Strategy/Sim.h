/*
Simulator and Real World Compatibility File
Copyright (C) JL Baxter, PA Birkin 2007

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
(not included)
*/

//Simulator Positions for a Simulator Strategy

#include "Strategy.h"

// sim positions
//																				inch / mm
double halfway = (FRIGHTX+FLEFTX)/2; // halfway line							50.00/1273
double upp_half = 1662/25.4; // y coord of upper freeball pos					65.43/1662
double bott_half =462/25.4; // y coord of lower freeball pos					18.82/462
double half = (FTOP+FBOT)/2; // y coord of center spot							41.81/1061						
double topy = FTOP; // top of field												77.24/1962
double topx = FRIGHTX; // right hand side of field								93.43/2373
double boty = FBOT; // bottom of field											 6.37/162
double botx = FLEFTX; // left hand side of field								 6.81/173
double pen_top = 1462/25.4; // top y coord of pen area							57.56/1462
double pen_bot = 662/25.4; // bottom y coord of pen area						26.06/662
double charge = 200/25.4; // charging distance (goal keeper)					 7.87/200
double ball_dis = 100/25.4; // distance between ball and player (kick switch)	 3.94/100
double mid_point = 723/25.4; // x coord of defensive freeball pos				28.46/723
double penarea = 523/25.4; // x coord of pen area								20.59/523
double opp = 1823/25.4; // x coord of offensive freeball pos					71.77/1823
double goalarea = 323/25.4; // x coord of goal area								12.72/323
double goaltop = GTOPY; // top of goal area										49.68/1261
double goalbot = GBOTY; // bottom of goal area									33.93/862
double goalk_ball_dis = 70/25.4; // goal keeper version of ball_dis				 2.76/70
