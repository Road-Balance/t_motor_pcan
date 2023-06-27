/* SPDX-License-Identifier: LGPL-2.1-only */
/*
 * RunExample.cpp - Entry point of the 04_ManualWrite PCANBasic example
 *
 * Copyright (C) 2001-2020  PEAK System-Technik GmbH <www.peak-system.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Contact:    <linux@peak-system.com>
 * Maintainer:  Fabrice Vergnaud <f.vergnaud@peak-system.com>
 * 	    	    Romain Tissier <r.tissier@peak-system.com>
 */
#include <iostream>
#include "ManualWrite.h"

int main()
{
	ManualWrite start;
	
	uint8_t id = 0x01;
	float pos = 0.0;

	std::cout << "Controller ID: " << std::hex << (int)id << std::endl;
	std::cout << "Position: " << pos << std::endl;

	start.comm_can_set_pos(id, pos);

	return 0;
}