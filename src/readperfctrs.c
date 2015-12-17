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

#include <asm/unistd.h>
#include <linux/perf_event.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include "hexpads.h"

/**
 * perf_event_open -- open a performance counter fd
 * (unfortunately, there's no libc wrapper available for this system call)
 */
long perf_event_open(struct perf_event_attr* event_attr, pid_t pid, int cpu, 
                     int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, event_attr, pid, cpu, group_fd, flags);
}


/**
 * perfctr_init -- initialize perf ctrs for a given proc
 * This function assumes that proc is initialized and in STATUS_PROC state
 */
void perfctr_init(struct proc *prc) {
  struct perf_event_attr pe;

  memset(&pe, 0, sizeof(struct perf_event_attr));
  pe.size = sizeof(struct perf_event_attr);
  pe.disabled = 1;  // start disabled, start all together
  pe.exclude_kernel = 1;
  pe.exclude_hv = 1;

  prc->pfd_instr = -1;
  prc->pfd_cache_access = -1;
  prc->pfd_cache_miss = -1;

  // activate performance counters
  // FIXME for production we must trace all processes!
  // for now, restrict tracing to uid '1000' and programs that start with pmontest
  if (prc->uid == 1000) { // && !strncmp(prc->cmd, "pmontest", 8)) {
    prc->psample = (struct p_sample*)malloc(sizeof(struct p_sample));
    bzero(prc->psample, sizeof(struct p_sample));

    pe.type = PERF_TYPE_HARDWARE;
    pe.config = PERF_COUNT_HW_INSTRUCTIONS;
    prc->pfd_instr = perf_event_open(&pe, prc->pid, -1 /*any cpu*/, -1 /*new group*/, 0 /*no flags*/);
    if (prc->pfd_instr == -1) {
      fatal("[fatal] Could not initialize perf monitoring for pid %d\n", prc->pid);
    }

    pe.config = PERF_COUNT_HW_CACHE_REFERENCES;
    //pe.type = PERF_TYPE_HW_CACHE;
    // when working with HW caches, we can only select one each, so either READ, WRITE or PREFETCH
    //pe.config = (PERF_COUNT_HW_CACHE_LL) | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_OP_WRITE <<8) | (PERF_COUNT_HW_CACHE_OP_PREFETCH << 8) | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16);
    prc->pfd_cache_access = perf_event_open(&pe, prc->pid, -1 /*any cpu*/, prc->pfd_instr /*this proc's group*/, 0 /*no flags*/);
    if (prc->pfd_cache_access == -1) {
      fatal("[fatal] Could not initialize perf monitoring for pid %d (cache access)\n", prc->pid);
    }

    pe.config = PERF_COUNT_HW_CACHE_MISSES;
    prc->pfd_cache_miss = perf_event_open(&pe, prc->pid, -1 /*any cpu*/, prc->pfd_instr /*this proc's group*/, 0 /*no flags*/);
    if (prc->pfd_cache_miss == -1) {
      fatal("[fatal] Could not initialize perf monitoring for pid %d (cache miss)\n", prc->pid);
    }

    // start counting
    ioctl(prc->pfd_instr, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
    ioctl(prc->pfd_instr, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);

    // start cracking and counting
    prc->status = STATUS_PERFCTR;
  } else {
    // we don't care about this process
    prc->status = STATUS_IGNORE;
  }
}


/**
 * perfctr_cleanup -- cleanup perf ctrs for this proc
 * Closes all open fd's and releases the sample's memory.
 */
void perfctr_cleanup(struct proc *proc) {
  if (proc->pfd_instr != -1)
    close(proc->pfd_instr);
  if (proc->pfd_cache_access != -1)
    close(proc->pfd_cache_access);
  if (proc->pfd_cache_miss != -1)
    close(proc->pfd_cache_miss);
  if (proc->psample != NULL)
    free(proc->psample);
  proc->pfd_instr = -1;
  proc->pfd_cache_access = -1;
  proc->pfd_cache_miss = -1;
  proc->psample = NULL;
}


/**
 * perfctr_scan -- scan all processes and read in new perfctr data
 */
void perfctr_scan(struct proc *procs) {
  struct proc *loc = procs;
  struct sysinfo info;

  sysinfo(&info);

  // iterate over all running processes
  while (loc != NULL) {
    if (loc->status == STATUS_READY || loc->status == STATUS_PERFCTR) {
      long long icount, cmiss, caccess;
      long oldringloc;

      // read samples
      read(loc->pfd_instr, &icount, sizeof(long long));
      read(loc->pfd_cache_access, &caccess, sizeof(long long));
      read(loc->pfd_cache_miss, &cmiss, sizeof(long long));

      // reset all counters
      ioctl(loc->pfd_instr, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);

      // store samples
      oldringloc = loc->psample->ringloc;
      loc->psample->ringloc = (loc->psample->ringloc + 1) % NR_SAMPLES;

      loc->psample->instr[loc->psample->ringloc] = icount;
      loc->psample->cache_access[loc->psample->ringloc] = caccess;
      loc->psample->cache_miss[loc->psample->ringloc] = cmiss;
      loc->psample->minflt[loc->psample->ringloc] = loc->minflt - loc->lstminflt;
      loc->lstminflt = loc->minflt;

      // figure out for how many seconds a process has been running
      double time = info.uptime - (double)(loc->starttime/sysconf(_SC_CLK_TCK));
      loc->uptime = time;

      // if this was the first collection, propagate first measurement
      if (loc->status == STATUS_PERFCTR) {
        // for longer running processes, adjust time and "copy" measurements
        if (time > 1.0) {  //time = 1.0;
          // calculate average number of page faults per scan iteration
          loc->minflt = loc->minflt / (time/SLEEP_TIME);
          //printf("%d - %ld %ld %f\n", loc->pid, loc->lstminflt, loc->minflt, time);
          for (int i = 0; i<NR_SAMPLES; ++i) {
            loc->psample->instr[i] = icount;
            loc->psample->cache_access[i] = caccess;
            loc->psample->cache_miss[i] = cmiss;
            loc->psample->minflt[i] = loc->minflt;
          }
        }
        loc->status = STATUS_READY;
      }
    }

    loc = loc->next;
  }
}

