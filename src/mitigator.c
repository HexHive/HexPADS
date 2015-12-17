/*
 * mitigator -- mitigate ongoing attacks
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
// # include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>

#include "hexpads.h"
#include "mitigator.h"

/**
 * mitigate - mitigate an ongoing attack
 */
void mitigate(struct proc *badp) {
#ifdef MITIGATESLOW
  setpriority(PRIO_PROCESS, badp->pid, 20);
  log("Slowed down process %d\n", badp->pid);
#else
#ifdef MITIGATESTOP
  signal(badp->pid, SIGSTOP);
  log("Stopped process %d\n", badp->pid);
#else
  log("No mitigation activated. %d keeps running\n", badp->pid);
#endif  // MITIGATESTOP
#endif  // MITIGATESLOW
}
