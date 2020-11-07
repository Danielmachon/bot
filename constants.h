/*
 * Copyright (C) 2020 Daniel Machon
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/* 
 * File:   constants.h
 * Author: Daniel Machon
 *
 * Created on 5. april 2020, 18:55
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

/*
 * These are hard-coded constants that need to be changed,
 * depending on the screen resolution.
 */
#define BG_NPC_DIALOG1_X 318
#define BG_NPC_DIALOG1_Y 664
#define BG_NPC_COORD_X 1875
#define BG_NPC_COORD_Y 543
#define BG_ICON_COORD_X 3534
#define BG_ICON_COORD_Y 407
#define BG_ICON_COLOR_TRIGGER 1260250
#define BG_POPUP_COORD_X 1860
#define BG_POPUP_COORD_Y 504
#define BG_POPUP_COLOR_TRIGGER 3295
#define BG_DEFECT_COORD_X 3326
#define BG_DEFECT_COORD_Y 381
#define BG_DEFECT_COLOR_TRIGGER 12269320
#define BG_LEAVE_CONDITION1_X 2968
#define BG_LEAVE_CONDITION1_Y 583
#define BG_LEAVE_CONDITION1_COLOR_TRIGGER 22377994
#define BG_LEAVE_CONDITION2_X 2381
#define BG_LEAVE_CONDITION2_Y 1472
#define BG_LEAVE_CONDITION2_COLOR_TRIGGER 0

#define PROFILE_QUEUE 0
#define PROFILE_JOIN 1
#define PROFILE_IN_BG 2
#define PROFILE_LEAVE 3
#define PROFILE_ANTIAFK 4

#define CLIENTCONNECTION 0x8BF1A8
#define CURMGROFFSET 0x462C 
#define FIRSTOBJECTOFFSET 0xB4 
#define NEXTOBJECTOFFSET 0x3C 
#define PLAYERGUID 0xB8 
#define GAMEOBJTYPEOFFSET 0x14
#define GAMEOBJGUIDOFFSET 0x30


#endif /* CONSTANTS_H */

