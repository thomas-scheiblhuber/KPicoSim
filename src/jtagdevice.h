/***************************************************************************
 *   Copyright (C) 2005 by Mark Six                                        *
 *   marksix@xs4all.nl                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef JTAGDEVICE_H
#define JTAGDEVICE_H

/* JTAG IDCODE's for Spartan-3 Devices */
#define XC3S50   0x0140C093
#define XC3S200  0x01414093
#define XC3S400  0x0141C093
#define XC3S1000 0x01428093
#define XC3S1500 0x01434093
#define XC3S2000 0x01440093
#define XC3S4000 0x01448093
#define XC3S5000 0x01450093


class JTAGDevice{
	public:
    	JTAGDevice();
    	~JTAGDevice();
};

#endif
