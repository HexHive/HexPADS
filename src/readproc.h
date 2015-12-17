/*
 * readproc -- helper functions to read/update process list
 * Copyright (C) 2015 Mathias Payer <mathias.payer@nebelwelt.net>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * stat2proc - parses a single process and updates given struct proc
 * @S string that contains the /proc/PID/stat file
 * @P process that will be filled, all stat values will be overwritten
 */
void stat2proc(const char* S, struct proc *P);

/**
 * proc_scan - scan and update all processes
 * This function is NOT thread safe!
 */
struct proc *proc_scan(struct proc *procs);
