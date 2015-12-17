/*
 * readperfctrs -- helper functions to read/update performance counters
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
 * perfctr_scan - scan and update performance ctrs of all processes
 */
void perfctr_scan(struct proc *procs);

/**
 * perfctr_cleanup - clean up when a process dies
 */
void perfctr_cleanup(struct proc *proc);

/**
 * Initialize the proc struct with defaults and start counting
 */
void perfctr_init(struct proc *procs);
