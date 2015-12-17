/*
 * detector -- detection unit that searches for attacks
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
#include <string.h>

#include "hexpads.h"
#include "detector.h"
#include "mitigator.h"

/**
 * detector - scan all processes and search for attacks
 */
void detector(struct proc *procs) {
  struct proc *loc = procs;

  // iterate over all running processes
  while (loc != NULL) {
    if (loc->status == STATUS_READY) {
      long long icount = 0, cmiss = 0, caccess = 0;
      long minflt;
      for (int i = 0; i<NR_SAMPLES; ++i) {
        icount += loc->psample->instr[i];
        caccess += loc->psample->cache_access[i];
        cmiss += loc->psample->cache_miss[i];
        minflt += loc->psample->minflt[i];
      }
      icount /= NR_SAMPLES;
      caccess /= NR_SAMPLES;
      cmiss /= NR_SAMPLES;
      minflt /= NR_SAMPLES;
      int ringloc = loc->psample->ringloc;
      int old = (ringloc - 1 + NR_SAMPLES) % NR_SAMPLES;

      double pmissrate = ((double)loc->psample->minflt[ringloc])/((double)loc->psample->instr[ringloc]);
      double fltrate = (double)loc->psample->minflt[ringloc] / ((double)loc->psample->minflt[old] + 1);
      int lasttwo = loc->psample->minflt[ringloc] + loc->psample->minflt[old] ;
      //printf("[i] PID %d executed %lld instructions with %lld/%lld cache misses (%f ratio), %lu page faults (on average).\n", loc->pid, icount, cmiss, caccess, (double)cmiss/(double)caccess, minflt);

      /* cache attack detection */
      double cmissrate = (double)cmiss/(double)caccess;
      if (cmissrate > 0.7 && cmiss > 100000 && fltrate < 0.01) {
        log("[i] Potential cache attack detected! PID: %d, '%s' (%f missrate, %lld misses) fltrate %f pmissrate %f\n", loc->pid, loc->cmd, cmissrate, cmiss, fltrate, pmissrate);
        mitigate(loc);
      }

      /* CAIN detection */
      if (
        (
          (fltrate > 2.0 
          && loc->psample->minflt[ringloc] > 100000
          && loc->psample->cache_miss[ringloc] > 10000
          && pmissrate > 0.001
          ) 
          ||
          ( lasttwo > 256000 )
        ) 
        ) {
        log("[i] Potential CAIN detected. PID: %d, '%s' (page misses: %ld/%ld, (%f) cache misses: %lld, cache miss rate: %f, page fault miss rate: %f).\n", loc->pid, loc->cmd, loc->psample->minflt[loc->psample->ringloc], loc->psample->minflt[old], fltrate, loc->psample->cache_miss[loc->psample->ringloc], cmissrate, pmissrate);
        mitigate(loc);
      }
      //if (strncmp(loc->cmd, "anteater", 8) == 0) {
      //  log("[i] CAIN metrics: %d - %ld/%ld (%f) %lld cmr %f pmr %f\n", loc->pid, loc->psample->minflt[ringloc], loc->psample->minflt[old], fltrate, loc->psample->cache_miss[ringloc], cmissrate, pmissrate);
      //}
    } // STATUS_READY
    loc = loc->next;
  } // loop over all processes
}
