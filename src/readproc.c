/*
 * readproc -- read information from proc and helpfer functions
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

#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "hexpads.h"
#include "readproc.h"
#include "readperfctrs.h"

void stat2proc(const char* S, struct proc *P) {
  char *tmp;
  unsigned int num;

  S = strchr(S, '(') + 1;
  tmp = strrchr(S, ')');

  num = tmp - S;
  if (unlikely(num >= sizeof P->cmd)) num = sizeof P->cmd - 1;
  memcpy(P->cmd, S, num);
  P->cmd[num] = '\0';
  S = tmp + 2;                 // skip ") "

  num = sscanf(S,
    "%c "
    "%d %d %d %d %d "
    "%u %lu %lu %lu %lu "
    "%lu %lu %ld %ld "  /* utime stime cutime cstime */
    "%ld %ld "
    "%ld "
    "%ld "
    "%llu "  /* start_time */
    "%lu "
    "%ld "
    "%lu %lu %lu %lu %lu %lu "
    "%*s %*s %*s %*s " /* discard, no RT signals & Linux 2.1 used hex */
    "%lu %*u %*u "
    "%d %d "
    "%u %u "
    "%llu "
    "%lu %lu "
    "%lu %lu %lu %lu %lu %lu %lu "
    "%d",
    &P->state,
    &P->ppid, &P->pgrp, &P->session, &P->tty_nr, &P->tpgid,
    &P->flags, &P->minflt, &P->cminflt, &P->majflt, &P->cmajflt,
    &P->utime, &P->stime, &P->cutime, &P->cstime,
    &P->priority, &P->nice,
    &P->num_threads,
    &P->itrealvalue,
    &P->starttime,
    &P->vsize,
    &P->rss,
    &P->rsslim, &P->startcode, &P->endcode, &P->startstack, &P->kstkesp, &P->kstkeip,
/*     P->signal, P->blocked, P->sigignore, P->sigcatch,   */ /* can't use
*/
    &P->wchan, /* &P->nswap, &P->cnswap, */  /* nswap and cnswap dead for 2.4.xx and up */
/* -- Linux 2.0.35 ends here -- */
    &P->exit_signal, &P->processor,  /* 2.2.1 ends with "exit_signal" */
/* -- Linux 2.2.8 to 2.5.17 end here -- */
    &P->rt_priority, &P->policy,  /* both added to 2.5.18 */
    &P->delayacct_blkio_ticks, /* since 2.6.18 */
    &P->guest_time, &P->cguest_time, /* since 2.6.24 */ 
    &P->start_data, &P->end_data, &P->start_brk, &P->arg_start, &P->arg_end, &P->env_start, &P->env_end, /* since 3.5 */
    &P->exit_code /* since 3.5 */
  );
  if (P->status == STATUS_CREATED)
    P->status = STATUS_PROC;
}

struct proc *proc_scan(struct proc *procs) {
  DIR *procfs = NULL;
  char path[64];
  int fd, pid = 0;
  struct proc *prev = NULL, *lproc = procs;
  unsigned long size;
  static char buf[2048];  // we'll reuse this buffer often, push it to the heap
  static struct stat statbuf;

  procfs = opendir("/proc");
  if (!procfs) {
    fatal("[fatal] Could not open /proc directory\n");
  }

  // Scan all pids
  struct dirent *ent;
  for (;;) {
    for (;;) {
      ent = readdir(procfs);
      if (unlikely(ent == NULL) || unlikely(ent->d_name == NULL)) {
        if (procfs != NULL) {
          closedir(procfs);
        }
        return procs;
      }
      if (likely(*ent->d_name > '0') && likely(*ent->d_name <= '9')) break;
    }

    pid = strtoul(ent->d_name, NULL, 10);
    //debug("[+] Parsing pid %d.\n", pid);

    // Read stat file and parse it
    memcpy(path, "/proc/", 6);
    strcpy(path+6, ent->d_name);  // trust /proc
    strncat(path, "/stat", 5);
    fd = open(path, O_RDONLY);
    if (fd == -1) continue; // process died while we scanned /proc/PIDs
    size = read(fd, buf, 2048);
    fstat(fd, &statbuf);
    close(fd);
    assert(size < 2048);

    // Find or create proc entry
    if (unlikely(lproc!=NULL && pid < lproc->pid)) {
      log("[e] Unordered PIDs encountered.\n");
      lproc = procs;
    }

    int count = 0;
    while (lproc != NULL && lproc->pid < pid) {
      if (count++ > 0) {
        log("[-] Process died: %d, looking for %d.\n", lproc->pid, pid);
        prev->next = lproc->next;
        perfctr_cleanup(lproc);
        // TODO: more cleanup needed? (e.g., through callback for perfmon data?)
        free(lproc);
        lproc = prev->next;
      } else {
        prev = lproc;
        lproc = lproc->next;
      }
    }

    if (lproc == NULL || lproc->pid != pid) {
      log("[+] Process created for pid %d.\n", pid);
      struct proc *loc = (struct proc*)malloc(sizeof(struct proc));
      bzero(loc, sizeof(struct proc));
      loc->pid = pid;
      loc->status = STATUS_CREATED;
      if (procs == NULL) {
        procs = loc;
      }
      if (prev != NULL) {
        loc->next = prev->next;
        prev->next = loc;
      }
      lproc = loc;
    }

    // Now that we have the stat file in a buffer and found/created our PID,
    // let's update the proc data.
    lproc->uid = statbuf.st_uid;
    lproc->gid = statbuf.st_gid;
    stat2proc(buf, lproc);  
    if (lproc->status == STATUS_PROC)
      perfctr_init(lproc);
  }

  // never reached
}
