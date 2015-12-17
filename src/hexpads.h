/*
 * pmon -- main header file with data structures for pmon
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


#define DOLOG 1
#define DODEBUG 1


// Select the mitigation method to use when an attack is detected:
//#define MITIGATESLOW 1      /* Slow down offending process */
//#define MITIGATESTOP 1      /* STOP offending process */

#define STATUS_CREATED  0   /* process struct created, still in init phase */
#define STATUS_PROC     1   /* scanned and filled struct with /proc data */
#define STATUS_PERFCTR  2   /* perf counters activated, not yet collected */
#define STATUS_READY    4   /* proc is in steady state */
#define STATUS_IGNORE   8   /* ignore this process (and don't collect stats) */

#define NR_SAMPLES      60  /* how many samples do we store */
#define SLEEP_TIME      1   /* how long we wait between samples (s) */

/*
 * process struct: contains information for a single process
 * Information is taken from /proc/XXX/stat and performance counters
 * stat fields according to http://man7.org/linux/man-pages/man5/proc.5.html
 */
struct proc {
  /* data read from /proc/PID/stat */
  int pid;                // PID
  char cmd[16];           // name of executable
  char state;             // R: running, S: sleeping, ...
  int ppid;               // parent's PID
  int pgrp;               // process group
  int session;            // session ID
  int tty_nr;             // terminal
  int tpgid;              // PID of proc of the controlling terminal
  unsigned int flags;     // flags
  unsigned long minflt;   // minor page faults
  unsigned long lstminflt;; // minor page faults in last iteration
  unsigned long cminflt;  // children's minor page faults
  unsigned long majflt;   // major page faults
  unsigned long cmajflt;  // children's major page faults
  unsigned long utime;    // time scheduled in user mode
  unsigned long stime;    // time scheduled in kernel mode
  long cutime;            // time waited-for children scheduled in user
  long cstime;            // time waited-for children scheduled in kernel
  long priority;          // priority, for RT tasks, negated
  long nice;              // nice level
  long num_threads;       // number of threads for this task
  long itrealvalue;       // time to next SIGALARM
  unsigned long long starttime; // time process was started after boot
  unsigned long vsize;    // virtual memory in bytes
  long rss;               // resident set size
  unsigned long rsslim;   // soft limit for rss
  unsigned long startcode; // start address of code
  unsigned long endcode;  // end address of code
  unsigned long startstack; // start address of stack
  unsigned long kstkesp;  // current esp value
  unsigned long kstkeip;  // current eip value
  unsigned long wchan;    // channel, proc is waiting on
  int exit_signal;        // signal sent to parent when child dies
  int processor;          // CPU number last executed on
  unsigned int rt_priority; // real-time scheduling priority
  unsigned int policy;    // scheduling policy
  unsigned long long delayacct_blkio_ticks; // aggregated block I/O delays
  unsigned long guest_time; // time spent running a vcpu
  long cguest_time;       // children's guest time
  unsigned long start_data; // bss address
  unsigned long end_data; // bss end address
  unsigned long start_brk;  // brk address
  unsigned long end_brk;  // brk end address
  unsigned long arg_start;  // arg address
  unsigned long arg_end;  // arg end address
  unsigned long env_start;  // env address
  unsigned long env_end;  // env end address
  unsigned int exit_code; // exit status (as of waitpid)

  /* data read from stat syscall */
  int uid;                // user id
  int gid;                // group id

  /* file descriptors for performance counter events */
  int pfd_instr;          // instruction count
  int pfd_cache_access;   // LL cache access
  int pfd_cache_miss;     // LL cache misses
  struct p_sample  *psample; // collected samples

  /* support datastructures */
  struct proc *next;      // next proc in list
  int status;             // status of this proc

  int uptime;          // time in seconds this process has been running
};

struct p_sample {
  long ringloc;
  unsigned long long instr[NR_SAMPLES];
  unsigned long long cache_access[NR_SAMPLES];
  unsigned long long cache_miss[NR_SAMPLES];
  long minflt[NR_SAMPLES];
};

#define log(fmt, ...) \
  do { if (DOLOG) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)

#define debug(fmt, ...) \
  do { if (DODEBUG) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)

#define fatal(fmt, ...) \
  do { perror("[fatal]"); fprintf(stderr, fmt, ##__VA_ARGS__); exit(-1); } while (0)
 
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)


