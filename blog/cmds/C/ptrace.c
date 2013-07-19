PTRACE(2)                                         Linux Programmer's Manual                                         PTRACE(2)



NAME
       ptrace - process trace

SYNOPSIS
       #include <sys/ptrace.h>

       long ptrace(enum __ptrace_request request, pid_t pid,
                   void *addr, void *data);

DESCRIPTION
       The ptrace() system call provides a means by which one process (the "tracer") may observe and control the execution of
       another process (the "tracee"), and examine and change the tracee's memory and registers.  It  is  primarily  used  to
       implement breakpoint debugging and system call tracing.

       A  tracee  first  needs  to be attached to the tracer.  Attachment and subsequent commands are per thread: in a multi‐
       threaded process, every thread can be individually attached to a (potentially different) tracer, or left not  attached
       and  thus  not debugged.  Therefore, "tracee" always means "(one) thread", never "a (possibly multithreaded) process".
       Ptrace commands are always sent to a specific tracee using a call of the form

           ptrace(PTRACE_foo, pid, ...)

       where pid is the thread ID of the corresponding Linux thread.

       (Note that in this page, a "multithreaded process" means a thread  group  consisting  of  threads  created  using  the
       clone(2) CLONE_THREAD flag.)

       A  process can initiate a trace by calling fork(2) and having the resulting child do a PTRACE_TRACEME, followed (typi‐
       cally) by an execve(2).  Alternatively, one process may commence tracing another process using PTRACE_ATTACH.

       While being traced, the tracee will stop each time a signal is delivered, even if the signal is  being  ignored.   (An
       exception is SIGKILL, which has its usual effect.)  The tracer will be notified at its next call to waitpid(2) (or one
       of the related "wait" system calls); that call will return a status value containing information  that  indicates  the
       cause  of  the stop in the tracee.  While the tracee is stopped, the tracer can use various ptrace requests to inspect
       and modify the tracee.  The tracer then causes the tracee to continue, optionally ignoring the  delivered  signal  (or
       even delivering a different signal instead).

       If  the PTRACE_O_TRACEEXEC option is not in effect, all successful calls to execve(2) by the traced process will cause
       it to be sent a SIGTRAP signal, giving the parent a chance to gain control before the new program begins execution.

       When the tracer is finished tracing, it can cause the tracee to continue executing in  a  normal,  untraced  mode  via
       PTRACE_DETACH.

       The value of request determines the action to be performed:

       PTRACE_TRACEME
              Indicate  that  this  process is to be traced by its parent.  A process probably shouldn't make this request if
              its parent isn't expecting to trace it.  (pid, addr, and data are ignored.)

       The PTRACE_TRACEME request is used only by the tracee; the remaining requests are used only by  the  tracer.   In  the
       following  requests,  pid specifies the thread ID of the tracee to be acted on.  For requests other than PTRACE_ATTACH
       and PTRACE_KILL, the tracee must be stopped.

       PTRACE_PEEKTEXT, PTRACE_PEEKDATA
              Read a word at the address addr in the tracee's memory, returning the word as the result of the ptrace()  call.
              Linux  does  not  have  separate  text and data address spaces, so these two requests are currently equivalent.
              (data is ignored.)

       PTRACE_PEEKUSER
              Read a word at offset addr in the tracee's USER area, which holds the registers and other information about the
              process  (see  <sys/user.h>).   The word is returned as the result of the ptrace() call.  Typically, the offset
              must be word-aligned, though this might vary by architecture.  See NOTES.  (data is ignored.)

       PTRACE_POKETEXT, PTRACE_POKEDATA
              Copy the word data to the address addr in the tracee's memory.  As  for  PTRACE_PEEKTEXT  and  PTRACE_PEEKDATA,
              these two requests are currently equivalent.

       PTRACE_POKEUSER
              Copy the word data to offset addr in the tracee's USER area.  As for PTRACE_PEEKUSER, the offset must typically
              be word-aligned.  In order to maintain the integrity of the kernel, some modifications to  the  USER  area  are
              disallowed.

       PTRACE_GETREGS, PTRACE_GETFPREGS
              Copy the tracee's general-purpose or floating-point registers, respectively, to the address data in the tracer.
              See <sys/user.h> for information on the format of this data.  (addr is ignored.)  Note that SPARC systems  have
              the  meaning  of  data  and addr reversed; that is, data is ignored and the registers are copied to the address
              addr.  PTRACE_GETREGS and PTRACE_GETFPREGS are not present on all architectures.

       PTRACE_GETSIGINFO (since Linux 2.3.99-pre6)
              Retrieve information about the signal that caused the stop.  Copy a siginfo_t structure (see sigaction(2)) from
              the tracee to the address data in the tracer.  (addr is ignored.)

       PTRACE_SETREGS, PTRACE_SETFPREGS
              Copy  the  tracee's  general-purpose  or  floating-point  registers, respectively, from the address data in the
              tracer.  As for PTRACE_POKEUSER, some general-purpose register  modifications  may  be  disallowed.   (addr  is
              ignored.)  Note that SPARC systems have the meaning of data and addr reversed; that is, data is ignored and the
              registers are copied from the address addr.  PTRACE_SETREGS and PTRACE_SETFPREGS are not present on all  archi‐
              tectures.

       PTRACE_SETSIGINFO (since Linux 2.3.99-pre6)
              Set  signal  information:  copy  a siginfo_t structure from the address data in the tracer to the tracee.  This
              will affect only signals that would normally be delivered to the tracee and were caught by the tracer.  It  may
              be  difficult  to  tell  these  normal  signals  from synthetic signals generated by ptrace() itself.  (addr is
              ignored.)

       PTRACE_SETOPTIONS (since Linux 2.4.6; see BUGS for caveats)
              Set ptrace options from data.  (addr is ignored.)  data is interpreted as a bit  mask  of  options,  which  are
              specified by the following flags:

              PTRACE_O_TRACESYSGOOD (since Linux 2.4.6)
                     When  delivering  system  call traps, set bit 7 in the signal number (i.e., deliver SIGTRAP|0x80).  This
                     makes it easy for the  tracer  to  distinguish  normal  traps  from  those  caused  by  a  system  call.
                     (PTRACE_O_TRACESYSGOOD may not work on all architectures.)

              PTRACE_O_TRACEFORK (since Linux 2.5.46)
                     Stop the tracee at the next fork(2) and automatically start tracing the newly forked process, which will
                     start with a SIGSTOP.  A waitpid(2) by the tracer will return a status value such that

                       status>>8 == (SIGTRAP | (PTRACE_EVENT_FORK<<8))

                     The PID of the new process can be retrieved with PTRACE_GETEVENTMSG.

              PTRACE_O_TRACEVFORK (since Linux 2.5.46)
                     Stop the tracee at the next vfork(2) and automatically start tracing the newly  vforked  process,  which
                     will start with a SIGSTOP.  A waitpid(2) by the tracer will return a status value such that

                       status>>8 == (SIGTRAP | (PTRACE_EVENT_VFORK<<8))

                     The PID of the new process can be retrieved with PTRACE_GETEVENTMSG.

              PTRACE_O_TRACECLONE (since Linux 2.5.46)
                     Stop  the  tracee  at  the next clone(2) and automatically start tracing the newly cloned process, which
                     will start with a SIGSTOP.  A waitpid(2) by the tracer will return a status value such that

                       status>>8 == (SIGTRAP | (PTRACE_EVENT_CLONE<<8))

                     The PID of the new process can be retrieved with PTRACE_GETEVENTMSG.

                     This option may not catch clone(2)  calls  in  all  cases.   If  the  tracee  calls  clone(2)  with  the
                     CLONE_VFORK  flag, PTRACE_EVENT_VFORK will be delivered instead if PTRACE_O_TRACEVFORK is set; otherwise
                     if the tracee calls clone(2) with the exit signal set to SIGCHLD, PTRACE_EVENT_FORK will be delivered if
                     PTRACE_O_TRACEFORK is set.

              PTRACE_O_TRACEEXEC (since Linux 2.5.46)
                     Stop the tracee at the next execve(2).  A waitpid(2) by the tracer will return a status value such that

                       status>>8 == (SIGTRAP | (PTRACE_EVENT_EXEC<<8))

                     If  the  execing thread is not a thread group leader, the thread ID is reset to thread group leader's ID
                     before this stop.  Since Linux 3.0, the former thread ID can be retrieved with PTRACE_GETEVENTMSG.

              PTRACE_O_TRACEVFORKDONE (since Linux 2.5.60)
                     Stop the tracee at the completion of the next vfork(2).  A waitpid(2) by the tracer will return a status
                     value such that

                       status>>8 == (SIGTRAP | (PTRACE_EVENT_VFORK_DONE<<8))

                     The PID of the new process can (since Linux 2.6.18) be retrieved with PTRACE_GETEVENTMSG.

              PTRACE_O_TRACEEXIT (since Linux 2.5.60)
                     Stop the tracee at exit.  A waitpid(2) by the tracer will return a status value such that

                       status>>8 == (SIGTRAP | (PTRACE_EVENT_EXIT<<8))

                     The tracee's exit status can be retrieved with PTRACE_GETEVENTMSG.

                     The tracee is stopped early during process exit, when registers are still available, allowing the tracer
                     to see where the exit occurred, whereas the normal exit notification is done after the process  is  fin‐
                     ished  exiting.   Even though context is available, the tracer cannot prevent the exit from happening at
                     this point.

       PTRACE_GETEVENTMSG (since Linux 2.5.46)
              Retrieve a message (as an unsigned long) about the ptrace event that just happened, placing it at  the  address
              data  in  the  tracer.   For  PTRACE_EVENT_EXIT,  this  is  the  tracee's  exit status.  For PTRACE_EVENT_FORK,
              PTRACE_EVENT_VFORK, PTRACE_EVENT_VFORK_DONE, and PTRACE_EVENT_CLONE, this is the PID of the new process.  (addr
              is ignored.)

       PTRACE_CONT
              Restart  the  stopped  tracee  process.   If data is nonzero, it is interpreted as the number of a signal to be
              delivered to the tracee; otherwise, no signal is delivered.  Thus, for example, the tracer can control  whether
              a signal sent to the tracee is delivered or not.  (addr is ignored.)

       PTRACE_SYSCALL, PTRACE_SINGLESTEP
              Restart the stopped tracee as for PTRACE_CONT, but arrange for the tracee to be stopped at the next entry to or
              exit from a system call, or after execution of a single instruction, respectively.  (The tracee will  also,  as
              usual,  be  stopped  upon  receipt of a signal.)  From the tracer's perspective, the tracee will appear to have
              been stopped by receipt of a SIGTRAP.  So, for PTRACE_SYSCALL, for example, the idea is to  inspect  the  argu‐
              ments  to the system call at the first stop, then do another PTRACE_SYSCALL and inspect the return value of the
              system call at the second stop.  The data argument is treated as for PTRACE_CONT.  (addr is ignored.)

       PTRACE_SYSEMU, PTRACE_SYSEMU_SINGLESTEP (since Linux 2.6.14)
              For PTRACE_SYSEMU, continue and stop on entry to the next  system  call,  which  will  not  be  executed.   For
              PTRACE_SYSEMU_SINGLESTEP,  do the same but also singlestep if not a system call.  This call is used by programs
              like User Mode Linux that want to emulate all the tracee's system calls.  The data argument is treated  as  for
              PTRACE_CONT.  The addr argument is ignored.  These requests are currently supported only on x89.

       PTRACE_KILL
              Send the tracee a SIGKILL to terminate it.  (addr and data are ignored.)

              This operation is deprecated; do not use it!  Instead, send a SIGKILL directly using kill(2) or tgkill(2).  The
              problem with PTRACE_KILL is that it requires the tracee to be in signal-delivery-stop,  otherwise  it  may  not
              work  (i.e., may complete successfully but won't kill the tracee).  By contrast, sending a SIGKILL directly has
              no such limitation.

       PTRACE_ATTACH
              Attach to the process specified in pid, making it a tracee of the  calling  process.   The  tracee  is  sent  a
              SIGSTOP,  but  will not necessarily have stopped by the completion of this call; use waitpid(2) to wait for the
              tracee to stop.  See the "Attaching and detaching" subsection for additional information.  (addr and  data  are
              ignored.)

       PTRACE_DETACH
              Restart the stopped tracee as for PTRACE_CONT, but first detach from it.  Under Linux, a tracee can be detached
              in this way regardless of which method was used to initiate tracing.  (addr is ignored.)

   Death under ptrace
       When a (possibly multithreaded) process receives a killing signal (one whose disposition is set to SIG_DFL  and  whose
       default  action  is to kill the process), all threads exit.  Tracees report their death to their tracer(s).  Notifica‐
       tion of this event is delivered via waitpid(2).

       Note that the killing signal will first cause signal-delivery-stop (on one tracee only), and only after it is injected
       by  the  tracer  (or after it was dispatched to a thread which isn't traced), will death from the signal happen on all
       tracees within a multithreaded process.  (The term "signal-delivery-stop" is explained below.)

       SIGKILL does not generate signal-delivery-stop and therefore the tracer can't suppress it.  SIGKILL kills even  within
       system  calls  (syscall-exit-stop  is not generated prior to death by SIGKILL).  The net effect is that SIGKILL always
       kills the process (all its threads), even if some threads of the process are ptraced.

       When the tracee calls _exit(2), it reports its death to its tracer.  Other threads are not affected.

       When any thread executes exit_group(2), every tracee in its thread group reports its death to its tracer.

       If the PTRACE_O_TRACEEXIT option is on, PTRACE_EVENT_EXIT will happen before actual death.  This applies to exits  via
       exit(2),  exit_group(2),  and  signal deaths (except SIGKILL), and when threads are torn down on execve(2) in a multi‐
       threaded process.

       The tracer cannot assume that the ptrace-stopped tracee exists.  There are many scenarios  when  the  tracee  may  die
       while stopped (such as SIGKILL).  Therefore, the tracer must be prepared to handle an ESRCH error on any ptrace opera‐
       tion.  Unfortunately, the same error is returned if the tracee exists but is not ptrace-stopped  (for  commands  which
       require  a  stopped  tracee), or if it is not traced by the process which issued the ptrace call.  The tracer needs to
       keep track of the stopped/running state of the tracee, and interpret ESRCH as "tracee died unexpectedly"  only  if  it
       knows  that  the tracee has been observed to enter ptrace-stop.  Note that there is no guarantee that waitpid(WNOHANG)
       will reliably report the tracee's death status if a ptrace operation returned ESRCH.  waitpid(WNOHANG)  may  return  0
       instead.  In other words, the tracee may be "not yet fully dead", but already refusing ptrace requests.

       The  tracer  can't  assume that the tracee always ends its life by reporting WIFEXITED(status) or WIFSIGNALED(status);
       there are cases where this does not occur.  For example, if a thread other than thread group leader does an execve(2),
       it  disappears;  its  PID  will never be seen again, and any subsequent ptrace stops will be reported under the thread
       group leader's PID.

   Stopped states
       A tracee can be in two states: running or stopped.

       There are many kinds of states when the tracee is stopped, and in ptrace discussions they are often conflated.  There‐
       fore, it is important to use precise terms.

       In  this  manual  page,  any  stopped  state in which the tracee is ready to accept ptrace commands from the tracer is
       called ptrace-stop.  Ptrace-stops can be further subdivided into signal-delivery-stop, group-stop,  syscall-stop,  and
       so on.  These stopped states are described in detail below.

       When the running tracee enters ptrace-stop, it notifies its tracer using waitpid(2) (or one of the other "wait" system
       calls).  Most of this manual page assumes that the tracer waits with:

           pid = waitpid(pid_or_minus_1, &status, __WALL);

       Ptrace-stopped tracees are reported as returns with pid greater than 0 and WIFSTOPPED(status) true.

       The __WALL flag does not include the WSTOPPED and WEXITED flags, but implies their functionality.

       Setting the WCONTINUED flag when calling waitpid(2) is not recommended: the "continued" state is per-process and  con‐
       suming it can confuse the real parent of the tracee.

       Use  of  the  WNOHANG flag may cause waitpid(2) to return 0 ("no wait results available yet") even if the tracer knows
       there should be a notification.  Example:

           errno = 0;
           ptrace(PTRACE_CONT, pid, 0L, 0L);
           if (errno == ESRCH) {
               /* tracee is dead */
               r = waitpid(tracee, &status, __WALL | WNOHANG);
               /* r can still be 0 here! */
           }

       The following kinds of ptrace-stops exist:  signal-delivery-stops,  group-stops,  PTRACE_EVENT  stops,  syscall-stops.
       They  all  are reported by waitpid(2) with WIFSTOPPED(status) true.  They may be differentiated by examining the value
       status>>8, and if there is ambiguity in that value, by querying PTRACE_GETSIGINFO.  (Note: the WSTOPSIG(status)  macro
       can't be used to perform this examination, because it returns the value (status>>8) & 0xff.)

   Signal-delivery-stop
       When  a  (possibly  multithreaded)  process receives any signal except SIGKILL, the kernel selects an arbitrary thread
       which handles the signal.  (If the signal is generated with tgkill(2), the target thread can be explicitly selected by
       the  caller.)  If the selected thread is traced, it enters signal-delivery-stop.  At this point, the signal is not yet
       delivered to the process, and can be suppressed by the tracer.  If the tracer doesn't suppress the signal,  it  passes
       the  signal  to  the  tracee in the next ptrace restart request.  This second step of signal delivery is called signal
       injection in this manual page.  Note that if the signal is blocked, signal-delivery-stop doesn't happen until the sig‐
       nal is unblocked, with the usual exception that SIGSTOP can't be blocked.

       Signal-delivery-stop  is  observed by the tracer as waitpid(2) returning with WIFSTOPPED(status) true, with the signal
       returned by WSTOPSIG(status).  If the signal is SIGTRAP, this  may  be  a  different  kind  of  ptrace-stop;  see  the
       "Syscall-stops" and "execve" sections below for details.  If WSTOPSIG(status) returns a stopping signal, this may be a
       group-stop; see below.

   Signal injection and suppression
       After signal-delivery-stop is observed by the tracer, the tracer should restart the tracee with the call

           ptrace(PTRACE_restart, pid, 0, sig)

       where PTRACE_restart is one of the restarting ptrace requests.  If sig is 0, then a signal is not  delivered.   Other‐
       wise,  the  signal sig is delivered.  This operation is called signal injection in this manual page, to distinguish it
       from signal-delivery-stop.

       The sig value may be different from the WSTOPSIG(status) value:  the  tracer  can  cause  a  different  signal  to  be
       injected.

       Note  that  a  suppressed  signal  still causes system calls to return prematurely.  In this case system calls will be
       restarted: the tracer will observe the tracee to reexecute the interrupted system call (or  restart_syscall(2)  system
       call  for a few syscalls which use a different mechanism for restarting) if the tracer uses PTRACE_SYSCALL.  Even sys‐
       tem calls (such as poll(2)) which are not restartable after signal are restarted after signal is suppressed;  however,
       kernel  bugs  exist  which  cause some syscalls to fail with EINTR even though no observable signal is injected to the
       tracee.

       Restarting ptrace commands issued in ptrace-stops other than signal-delivery-stop are not guaranteed to inject a  sig‐
       nal,  even if sig is nonzero.  No error is reported; a nonzero sig may simply be ignored.  Ptrace users should not try
       to "create a new signal" this way: use tgkill(2) instead.

       The fact that signal injection requests may be ignored when restarting the tracee after ptrace stops that are not sig‐
       nal-delivery-stops  is  a  cause  of  confusion  among ptrace users.  One typical scenario is that the tracer observes
       group-stop, mistakes it for signal-delivery-stop, restarts the tracee with

           ptrace(PTRACE_rest, pid, 0, stopsig)

       with the intention of injecting stopsig, but stopsig gets ignored and the tracee continues to run.

       The SIGCONT signal has a side effect of waking up (all threads of) a group-stopped process.  This side effect  happens
       before signal-delivery-stop.  The tracer can't suppress this side effect (it can only suppress signal injection, which
       only causes the SIGCONT handler to not be executed in the tracee, if such a handler is installed).  In fact, waking up
       from  group-stop  may  be followed by signal-delivery-stop for signal(s) other than SIGCONT, if they were pending when
       SIGCONT was delivered.  In other words, SIGCONT may be not the first signal observed by the tracee after it was sent.

       Stopping signals cause (all threads of) a process to enter group-stop.  This side effect happens after  signal  injec‐
       tion, and therefore can be suppressed by the tracer.

       In Linux 2.4 and earlier, the SIGSTOP signal can't be injected.

       PTRACE_GETSIGINFO  can  be  used  to  retrieve  a  siginfo_t  structure  which  corresponds  to  the delivered signal.
       PTRACE_SETSIGINFO may be used to modify it.  If PTRACE_SETSIGINFO has been used to alter siginfo_t, the si_signo field
       and the sig parameter in the restarting command must match, otherwise the result is undefined.

   Group-stop
       When  a  (possibly  multithreaded)  process receives a stopping signal, all threads stop.  If some threads are traced,
       they enter a group-stop.  Note that the stopping signal will first cause signal-delivery-stop (on  one  tracee  only),
       and  only  after it is injected by the tracer (or after it was dispatched to a thread which isn't traced), will group-
       stop be initiated on all tracees within the multithreaded process.  As usual, every tracee reports its group-stop sep‐
       arately to the corresponding tracer.

       Group-stop  is  observed  by the tracer as waitpid(2) returning with WIFSTOPPED(status) true, with the stopping signal
       available via WSTOPSIG(status).  The same result is returned by some other classes of ptrace-stops, therefore the rec‐
       ommended practice is to perform the call

           ptrace(PTRACE_GETSIGINFO, pid, 0, &siginfo)

       The  call can be avoided if the signal is not SIGSTOP, SIGTSTP, SIGTTIN, or SIGTTOU; only these four signals are stop‐
       ping signals.  If the tracer sees something else, it can't be a group-stop.   Otherwise,  the  tracer  needs  to  call
       PTRACE_GETSIGINFO.   If PTRACE_GETSIGINFO fails with EINVAL, then it is definitely a group-stop.  (Other failure codes
       are possible, such as ESRCH ("no such process") if a SIGKILL killed the tracee.)

       As of kernel 2.6.38, after the tracer sees the tracee ptrace-stop and until it restarts or kills it, the  tracee  will
       not  run, and will not send notifications (except SIGKILL death) to the tracer, even if the tracer enters into another
       waitpid(2) call.

       The kernel behavior described in the previous paragraph causes a problem with transparent handling  of  stopping  sig‐
       nals.   If  the  tracer  restarts  the  tracee after group-stop, the stopping signal is effectively ignored—the tracee
       doesn't remain stopped, it runs.  If the tracer doesn't restart the tracee before entering into the  next  waitpid(2),
       future  SIGCONT  signals will not be reported to the tracer; this would cause the SIGCONT signals to have no effect on
       the tracee.

   PTRACE_EVENT stops
       If the tracer sets PTRACE_O_TRACE_* options, the tracee will enter ptrace-stops called PTRACE_EVENT stops.

       PTRACE_EVENT stops are observed by the tracer as waitpid(2) returning with  WIFSTOPPED(status),  and  WSTOPSIG(status)
       returns SIGTRAP.  An additional bit is set in the higher byte of the status word: the value status>>8 will be

           (SIGTRAP | PTRACE_EVENT_foo << 8).

       The following events exist:

       PTRACE_EVENT_VFORK
              Stop  before  return  from  vfork(2) or clone(2) with the CLONE_VFORK flag.  When the tracee is continued after
              this stop, it will wait for child to exit/exec before continuing its  execution  (in  other  words,  the  usual
              behavior on vfork(2)).

       PTRACE_EVENT_FORK
              Stop before return from fork(2) or clone(2) with the exit signal set to SIGCHLD.

       PTRACE_EVENT_CLONE
              Stop before return from clone(2).

       PTRACE_EVENT_VFORK_DONE
              Stop  before  return  from  vfork(2)  or clone(2) with the CLONE_VFORK flag, but after the child unblocked this
              tracee by exiting or execing.

       For all four stops described above, the stop occurs in the parent (i.e., the tracee), not in the newly created thread.
       PTRACE_GETEVENTMSG can be used to retrieve the new thread's ID.

       PTRACE_EVENT_EXEC
              Stop before return from execve(2).  Since Linux 3.0, PTRACE_GETEVENTMSG returns the former thread ID.

       PTRACE_EVENT_EXIT
              Stop  before  exit  (including death from exit_group(2)), signal death, or exit caused by execve(2) in a multi‐
              threaded process.  PTRACE_GETEVENTMSG returns the exit status.  Registers can be examined (unlike  when  "real"
              exit happens).  The tracee is still alive; it needs to be PTRACE_CONTed or PTRACE_DETACHed to finish exiting.

       PTRACE_GETSIGINFO on PTRACE_EVENT stops returns SIGTRAP in si_signo, with si_code set to (event<<8) | SIGTRAP.

   Syscall-stops
       If  the tracee was restarted by PTRACE_SYSCALL, the tracee enters syscall-enter-stop just prior to entering any system
       call.  If the tracer restarts the tracee with PTRACE_SYSCALL, the tracee enters syscall-exit-stop when the system call
       is finished, or if it is interrupted by a signal.  (That is, signal-delivery-stop never happens between syscall-enter-
       stop and syscall-exit-stop; it happens after syscall-exit-stop.)

       Other possibilities are that  the  tracee  may  stop  in  a  PTRACE_EVENT  stop,  exit  (if  it  entered  _exit(2)  or
       exit_group(2)),  be  killed  by  SIGKILL,  or  die silently (if it is a thread group leader, the execve(2) happened in
       another thread, and that thread is not traced by the same tracer; this situation is discussed later).

       Syscall-enter-stop and syscall-exit-stop are observed by the tracer as waitpid(2)  returning  with  WIFSTOPPED(status)
       true,  and  WSTOPSIG(status)  giving  SIGTRAP.  If the PTRACE_O_TRACESYSGOOD option was set by the tracer, then WSTOP‐
       SIG(status) will give the value (SIGTRAP | 0x80).

       Syscall-stops can be distinguished from signal-delivery-stop with SIGTRAP by querying PTRACE_GETSIGINFO for  the  fol‐
       lowing cases:

       si_code <= 0
              SIGTRAP  was  delivered  as  a  result  of a user-space action, for example, a system call (tgkill(2), kill(2),
              sigqueue(3), etc.), expiration of a POSIX timer, change of state on a POSIX message queue, or completion of  an
              asynchronous I/O request.

       si_code == SI_KERNEL (0x80)
              SIGTRAP was sent by the kernel.

       si_code == SIGTRAP or si_code == (SIGTRAP|0x80)
              This is a syscall-stop.

       However,  syscall-stops happen very often (twice per system call), and performing PTRACE_GETSIGINFO for every syscall-
       stop may be somewhat expensive.

       Some architectures allow the cases to be distinguished by examining registers.  For example, on x86, rax == -ENOSYS in
       syscall-enter-stop.   Since  SIGTRAP (like any other signal) always happens after syscall-exit-stop, and at this point
       rax almost never contains -ENOSYS, the SIGTRAP looks like "syscall-stop which is  not  syscall-enter-stop";  in  other
       words,  it  looks like a "stray syscall-exit-stop" and can be detected this way.  But such detection is fragile and is
       best avoided.

       Using the PTRACE_O_TRACESYSGOOD option is the recommended method to distinguish  syscall-stops  from  other  kinds  of
       ptrace-stops, since it is reliable and does not incur a performance penalty.

       Syscall-enter-stop  and  syscall-exit-stop  are  indistinguishable from each other by the tracer.  The tracer needs to
       keep track of the sequence of ptrace-stops in order to not misinterpret  syscall-enter-stop  as  syscall-exit-stop  or
       vice  versa.   The  rule  is that syscall-enter-stop is always followed by syscall-exit-stop, PTRACE_EVENT stop or the
       tracee's death; no other kinds of ptrace-stop can occur in between.

       If after syscall-enter-stop, the tracer uses a restarting command other than PTRACE_SYSCALL, syscall-exit-stop is  not
       generated.

       PTRACE_GETSIGINFO on syscall-stops returns SIGTRAP in si_signo, with si_code set to SIGTRAP or (SIGTRAP|0x80).

   PTRACE_SINGLESTEP, PTRACE_SYSEMU, PTRACE_SYSEMU_SINGLESTEP stops
       [Details of these kinds of stops are yet to be documented.]

   Informational and restarting ptrace commands
       Most ptrace commands (all except PTRACE_ATTACH, PTRACE_TRACEME, and PTRACE_KILL) require the tracee to be in a ptrace-
       stop, otherwise they fail with ESRCH.

       When the tracee is in ptrace-stop, the tracer can read and write data to  the  tracee  using  informational  commands.
       These commands leave the tracee in ptrace-stopped state:

           ptrace(PTRACE_PEEKTEXT/PEEKDATA/PEEKUSER, pid, addr, 0);
           ptrace(PTRACE_POKETEXT/POKEDATA/POKEUSER, pid, addr, long_val);
           ptrace(PTRACE_GETREGS/GETFPREGS, pid, 0, &struct);
           ptrace(PTRACE_SETREGS/SETFPREGS, pid, 0, &struct);
           ptrace(PTRACE_GETSIGINFO, pid, 0, &siginfo);
           ptrace(PTRACE_SETSIGINFO, pid, 0, &siginfo);
           ptrace(PTRACE_GETEVENTMSG, pid, 0, &long_var);
           ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_flags);

       Note  that some errors are not reported.  For example, setting signal information (siginfo) may have no effect in some
       ptrace-stops, yet the call may succeed (return 0 and not set  errno);  querying  PTRACE_GETEVENTMSG  may  succeed  and
       return some random value if current ptrace-stop is not documented as returning a meaningful event message.

       The call

           ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_flags);

       affects  one  tracee.  The tracee's current flags are replaced.  Flags are inherited by new tracees created and "auto-
       attached" via active PTRACE_O_TRACEFORK, PTRACE_O_TRACEVFORK, or PTRACE_O_TRACECLONE options.

       Another group of commands makes the ptrace-stopped tracee run.  They have the form:

           ptrace(cmd, pid, 0, sig);

       where cmd is PTRACE_CONT,  PTRACE_DETACH,  PTRACE_SYSCALL,  PTRACE_SINGLESTEP,  PTRACE_SYSEMU,  or  PTRACE_SYSEMU_SIN‐
       GLESTEP.   If  the tracee is in signal-delivery-stop, sig is the signal to be injected (if it is nonzero).  Otherwise,
       sig may be ignored.  (When restarting a tracee from a ptrace-stop other than signal-delivery-stop,  recommended  prac‐
       tice is to always pass 0 in sig.)

   Attaching and detaching
       A thread can be attached to the tracer using the call

           ptrace(PTRACE_ATTACH, pid, 0, 0);

       This  also sends SIGSTOP to this thread.  If the tracer wants this SIGSTOP to have no effect, it needs to suppress it.
       Note that if other signals are concurrently sent to this thread during attach, the tracer may  see  the  tracee  enter
       signal-delivery-stop  with  other  signal(s)  first!  The usual practice is to reinject these signals until SIGSTOP is
       seen, then suppress SIGSTOP injection.  The design bug here is that a  ptrace  attach  and  a  concurrently  delivered
       SIGSTOP may race and the concurrent SIGSTOP may be lost.

       Since  attaching sends SIGSTOP and the tracer usually suppresses it, this may cause a stray EINTR return from the cur‐
       rently executing system call in the tracee, as described in the "Signal injection and suppression" section.

       The request

           ptrace(PTRACE_TRACEME, 0, 0, 0);

       turns the calling thread into a tracee.  The thread continues to run (doesn't enter ptrace-stop).  A  common  practice
       is to follow the PTRACE_TRACEME with

           raise(SIGSTOP);

       and allow the parent (which is our tracer now) to observe our signal-delivery-stop.

       If  the  PTRACE_O_TRACEFORK,  PTRACE_O_TRACEVFORK, or PTRACE_O_TRACECLONE options are in effect, then children created
       by, respectively, vfork(2) or clone(2) with the CLONE_VFORK flag, fork(2) or clone(2) with  the  exit  signal  set  to
       SIGCHLD,  and  other  kinds  of  clone(2),  are  automatically  attached to the same tracer which traced their parent.
       SIGSTOP is delivered to the children, causing them to enter signal-delivery-stop after they exit the system call which
       created them.

       Detaching of the tracee is performed by:

           ptrace(PTRACE_DETACH, pid, 0, sig);

       PTRACE_DETACH  is  a restarting operation; therefore it requires the tracee to be in ptrace-stop.  If the tracee is in
       signal-delivery-stop, a signal can be injected.  Otherwise, the sig parameter may be silently ignored.

       If the tracee is running when the tracer wants to detach it, the usual solution is to send SIGSTOP  (using  tgkill(2),
       to  make sure it goes to the correct thread), wait for the tracee to stop in signal-delivery-stop for SIGSTOP and then
       detach it (suppressing SIGSTOP injection).  A design bug is that this can race with concurrent SIGSTOPs.  Another com‐
       plication  is  that  the  tracee  may  enter  other ptrace-stops and needs to be restarted and waited for again, until
       SIGSTOP is seen.  Yet another complication is to be sure that the tracee is not  already  ptrace-stopped,  because  no
       signal delivery happens while it is—not even SIGSTOP.

       If the tracer dies, all tracees are automatically detached and restarted, unless they were in group-stop.  Handling of
       restart from group-stop is currently buggy, but the "as planned" behavior is to leave tracee stopped and  waiting  for
       SIGCONT.  If the tracee is restarted from signal-delivery-stop, the pending signal is injected.

   execve(2) under ptrace
       When  one thread in a multithreaded process calls execve(2), the kernel destroys all other threads in the process, and
       resets the thread ID of the execing thread to the thread group ID (process ID).  (Or, to put things another way,  when
       a  multithreaded  process does an execve(2), at completion of the call, it appears as though the execve(2) occurred in
       the thread group leader, regardless of which thread did the execve(2).)  This resetting of the thread  ID  looks  very
       confusing to tracers:

       *  All  other  threads stop in PTRACE_EVENT_EXIT stop, if the PTRACE_O_TRACEEXIT option was turned on.  Then all other
          threads except the thread group leader report death as if they exited via _exit(2) with exit code 0.

       *  The execing tracee changes its thread ID while it is in the execve(2).  (Remember, under ptrace, the "pid" returned
          from waitpid(2), or fed into ptrace calls, is the tracee's thread ID.)  That is, the tracee's thread ID is reset to
          be the same as its process ID, which is the same as the thread group leader's thread ID.

       *  Then a PTRACE_EVENT_EXEC stop happens, if the PTRACE_O_TRACEEXEC option was turned on.

       *  If the thread group leader has reported its PTRACE_EVENT_EXIT stop by this time, it appears to the tracer that  the
          dead  thread  leader  "reappears  from  nowhere".   (Note: the thread group leader does not report death via WIFEX‐
          ITED(status) until there is at least one other live thread.  This eliminates the possibility that the  tracer  will
          see it dying and then reappearing.)  If the thread group leader was still alive, for the tracer this may look as if
          thread group leader returns from a different system call than it entered, or even "returned from a system call even
          though  it  was  not  in any system call".  If the thread group leader was not traced (or was traced by a different
          tracer), then during execve(2) it will appear as if it has become a tracee of the tracer of the execing tracee.

       All of the above effects are the artifacts of the thread ID change in the tracee.

       The PTRACE_O_TRACEEXEC  option  is  the  recommended  tool  for  dealing  with  this  situation.   First,  it  enables
       PTRACE_EVENT_EXEC stop, which occurs before execve(2) returns.  In this stop, the tracer can use PTRACE_GETEVENTMSG to
       retrieve the tracee's former thread ID.  (This feature was introduced in Linux 3.0).  Second,  the  PTRACE_O_TRACEEXEC
       option disables legacy SIGTRAP generation on execve(2).

       When  the tracer receives PTRACE_EVENT_EXEC stop notification, it is guaranteed that except this tracee and the thread
       group leader, no other threads from the process are alive.

       On receiving the PTRACE_EVENT_EXEC stop notification, the tracer should clean up  all  its  internal  data  structures
       describing  the  threads of this process, and retain only one data structure—one which describes the single still run‐
       ning tracee, with

           thread ID == thread group ID == process ID.

       Example: two threads call execve(2) at the same time:

       *** we get syscall-enter-stop in thread 1: **
       PID1 execve("/bin/foo", "foo" <unfinished ...>
       *** we issue PTRACE_SYSCALL for thread 1 **
       *** we get syscall-enter-stop in thread 2: **
       PID2 execve("/bin/bar", "bar" <unfinished ...>
       *** we issue PTRACE_SYSCALL for thread 2 **
       *** we get PTRACE_EVENT_EXEC for PID0, we issue PTRACE_SYSCALL **
       *** we get syscall-exit-stop for PID0: **
       PID0 <... execve resumed> )             = 0

       If the PTRACE_O_TRACEEXEC option is not in effect for the execing tracee, the kernel delivers an extra SIGTRAP to  the
       tracee after execve(2) returns.  This is an ordinary signal (similar to one which can be generated by kill -TRAP), not
       a special kind of ptrace-stop.  Employing PTRACE_GETSIGINFO for this signal returns si_code set to 0 (SI_USER).   This
       signal may be blocked by signal mask, and thus may be delivered (much) later.

       Usually, the tracer (for example, strace(1)) would not want to show this extra post-execve SIGTRAP signal to the user,
       and would suppress its delivery to the tracee (if SIGTRAP is set to SIG_DFL, it is a killing signal).  However, deter‐
       mining  which  SIGTRAP to suppress is not easy.  Setting the PTRACE_O_TRACEEXEC option and thus suppressing this extra
       SIGTRAP is the recommended approach.

   Real parent
       The ptrace API (ab)uses the standard UNIX parent/child signaling over waitpid(2).  This used to cause the real  parent
       of  the  process  to stop receiving several kinds of waitpid(2) notifications when the child process is traced by some
       other process.

       Many of these bugs have been fixed, but as of Linux 2.6.38 several still exist; see BUGS below.

       As of Linux 2.6.38, the following is believed to work correctly:

       *  exit/death by signal is reported first to the tracer, then, when the tracer consumes the waitpid(2) result, to  the
          real  parent (to the real parent only when the whole multithreaded process exits).  If the tracer and the real par‐
          ent are the same process, the report is sent only once.

RETURN VALUE
       On success, PTRACE_PEEK* requests return the requested data, while other requests return zero.  On error, all requests
       return  -1,  and errno is set appropriately.  Since the value returned by a successful PTRACE_PEEK* request may be -1,
       the caller must clear errno before the call, and then check  it  afterward  to  determine  whether  or  not  an  error
       occurred.

ERRORS
       EBUSY  (i386 only) There was an error with allocating or freeing a debug register.

       EFAULT There  was an attempt to read from or write to an invalid area in the tracer's or the tracee's memory, probably
              because the area wasn't mapped or accessible.  Unfortunately, under Linux, different variations of  this  fault
              will return EIO or EFAULT more or less arbitrarily.

       EINVAL An attempt was made to set an invalid option.

       EIO    request  is  invalid,  or  an  attempt was made to read from or write to an invalid area in the tracer's or the
              tracee's memory, or there was a word-alignment violation, or an invalid signal was specified during  a  restart
              request.

       EPERM  The  specified  process  cannot  be  traced.  This could be because the tracer has insufficient privileges (the
              required capability is CAP_SYS_PTRACE); unprivileged processes cannot trace processes  that  they  cannot  send
              signals to or those running set-user-ID/set-group-ID programs, for obvious reasons.  Alternatively, the process
              may already be being traced, or (on kernels before 2.6.26) be init(8) (PID 1).

       ESRCH  The specified process does not exist, or is not currently being traced by the caller, or is  not  stopped  (for
              requests that require a stopped tracee).

CONFORMING TO
       SVr4, 4.3BSD.

NOTES
       Although  arguments to ptrace() are interpreted according to the prototype given, glibc currently declares ptrace() as
       a variadic function with only the request argument fixed.  It is recommended to always supply four arguments, even  if
       the requested operation does not use them, setting unused/ignored arguments to 0L or (void *) 0.

       In Linux kernels before 2.6.26, init(8), the process with PID 1, may not be traced.

       The  layout  of  the  contents of memory and the USER area are quite operating-system- and architecture-specific.  The
       offset supplied, and the data returned, might not entirely match with the definition of struct user.

       The size of a "word" is determined by the operating-system variant (e.g., for 32-bit Linux it is 32 bits).

       This page documents the way the ptrace() call works currently in Linux.  Its behavior differs noticeably on other fla‐
       vors of UNIX.  In any case, use of ptrace() is highly specific to the operating system and architecture.

BUGS
       On  hosts  with  2.6  kernel headers, PTRACE_SETOPTIONS is declared with a different value than the one for 2.4.  This
       leads to applications compiled with 2.6 kernel headers failing when run on 2.4 kernels.  This can be worked around  by
       redefining PTRACE_SETOPTIONS to PTRACE_OLDSETOPTIONS, if that is defined.

       Group-stop notifications are sent to the tracer, but not to real parent.  Last confirmed on 2.6.38.6.

       If  a  thread  group  leader  is traced and exits by calling _exit(2), a PTRACE_EVENT_EXIT stop will happen for it (if
       requested), but the subsequent WIFEXITED notification will  not  be  delivered  until  all  other  threads  exit.   As
       explained above, if one of other threads calls execve(2), the death of the thread group leader will never be reported.
       If the execed thread is not traced by this tracer, the tracer will never know that execve(2) happened.   One  possible
       workaround  is  to  PTRACE_DETACH  the  thread  group leader instead of restarting it in this case.  Last confirmed on
       2.6.38.6.

       A SIGKILL signal may still cause a PTRACE_EVENT_EXIT stop before actual signal death.  This  may  be  changed  in  the
       future; SIGKILL is meant to always immediately kill tasks even under ptrace.  Last confirmed on 2.6.38.6.

       Some  system  calls  return  with  EINTR  if a signal was sent to a tracee, but delivery was suppressed by the tracer.
       (This is very typical operation: it is usually done by debuggers on every attach, in order to not  introduce  a  bogus
       SIGSTOP).  As of Linux 3.2.9, the following system calls are affected (this list is likely incomplete): epoll_wait(2),
       and read(2) from an inotify(7) file descriptor.  The usual symptom of this bug is that when you attach to a  quiescent
       process with the command

           strace -p <process-ID>

       then, instead of the usual and expected one-line output such as

           restart_syscall(<... resuming interrupted call ...>_

       or

           select(6, [5], NULL, [5], NULL_

       ('_' denotes the cursor position), you observe more than one line.  For example:

           clock_gettime(CLOCK_MONOTONIC, {15370, 690928118}) = 0
           epoll_wait(4,_

       What  is  not  visible  here  is  that  the  process was blocked in epoll_wait(2) before strace(1) has attached to it.
       Attaching caused epoll_wait(2) to return to user space with the error EINTR.  In this  particular  case,  the  program
       reacted  to EINTR by checking the current time, and then executing epoll_wait(2) again.  (Programs which do not expect
       such "stray" EINTR errors may behave in an unintended way upon an strace(1) attach.)

SEE ALSO
       gdb(1), strace(1), clone(2), execve(2), fork(2), gettid(2), sigaction(2), tgkill(2),  vfork(2),  waitpid(2),  exec(3),
       capabilities(7), signal(7)

COLOPHON
       This page is part of release 3.44 of the Linux man-pages project.  A description of the project, and information about
       reporting bugs, can be found at http://www.kernel.org/doc/man-pages/.



Linux                                                     2012-10-25                                                PTRACE(2)




Playing with ptrace

Using ptrace allows you to set up system call interception and modification at the user level.

Have you ever wondered how system calls can be intercepted? Have you ever tried fooling the kernel by changing system call arguments? Have you ever wondered how debuggers stop a running process and let you take control of the process?

If you are thinking of using complex kernel programming to accomplish tasks, think again. Linux provides an elegant mechanism to achieve all of these things: the ptrace (Process Trace) system call. ptrace provides a mechanism by which a parent process may observe and control the execution of another process. It can examine and change its core image and registers and is used primarily to implement breakpoint debugging and system call tracing.

In this article, we learn how to intercept a system call and change its arguments. In Part II of the article we will study advanced techniques—setting breakpoints and injecting code into a running program. We will peek into the child process' registers and data segment and modify the contents. We will also describe a way to inject code so the process can be stopped and execute arbitrary instructions.
Basics

Operating systems offer services through a standard mechanism called system calls. They provide a standard API for accessing the underlying hardware and low-level services, such as the filesystems. When a process wants to invoke a system call, it puts the arguments to system calls in registers and calls soft interrupt 0x80. This soft interrupt is like a gate to the kernel mode, and the kernel will execute the system call after examining the arguments.

On the i386 architecture (all the code in this article is i386-specific), the system call number is put in the register %eax. The arguments to this system call are put into registers %ebx, %ecx, %edx, %esi and %edi, in that order. For example, the call:

write(2, "Hello", 5)

roughly would translate into

movl   $4, %eax
movl   $2, %ebx
movl   $hello,%ecx
movl   $5, %edx
int    $0x80

where $hello points to a literal string “Hello”.

So where does ptrace come into picture? Before executing the system call, the kernel checks whether the process is being traced. If it is, the kernel stops the process and gives control to the tracking process so it can examine and modify the traced process' registers.

Let's clarify this explanation with an example of how the process works:

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/user.h>   /* For constants
                                   ORIG_EAX etc */
int main()
{   pid_t child;
    long orig_eax;
    child = fork();
    if(child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    }
    else {
        wait(NULL);
        orig_eax = ptrace(PTRACE_PEEKUSER,
                          child, 4 * ORIG_EAX,
                          NULL);
        printf("The child made a "
               "system call %ld\n", orig_eax);
        ptrace(PTRACE_CONT, child, NULL, NULL);
    }
    return 0;
}

When run, this program prints:

The child made a system call 11

along with the output of ls. System call number 11 is execve, and it's the first system call executed by the child. For reference, system call numbers can be found in /usr/include/asm/unistd.h.

As you can see in the example, a process forks a child and the child executes the process we want to trace. Before running exec, the child calls ptrace with the first argument, equal to PTRACE_TRACEME. This tells the kernel that the process is being traced, and when the child executes the execve system call, it hands over control to its parent. The parent waits for notification from the kernel with a wait() call. Then the parent can check the arguments of the system call or do other things, such as looking into the registers.

When the system call occurs, the kernel saves the original contents of the eax register, which contains the system call number. We can read this value from child's USER segment by calling ptrace with the first argument PTRACE_PEEKUSER, shown as above.

After we are done examining the system call, the child can continue with a call to ptrace with the first argument PTRACE_CONT, which lets the system call continue.
ptrace Parameters

ptrace is called with four arguments:

long ptrace(enum __ptrace_request request,
            pid_t pid,
            void *addr,
            void *data);

The first argument determines the behaviour of ptrace and how other arguments are used. The value of request should be one of PTRACE_TRACEME, PTRACE_PEEKTEXT, PTRACE_PEEKDATA, PTRACE_PEEKUSER, PTRACE_POKETEXT, PTRACE_POKEDATA, PTRACE_POKEUSER, PTRACE_GETREGS, PTRACE_GETFPREGS, PTRACE_SETREGS, PTRACE_SETFPREGS, PTRACE_CONT, PTRACE_SYSCALL, PTRACE_SINGLESTEP, PTRACE_DETACH. The significance of each of these requests will be explained in the rest of the article.
Reading System Call Parameters

By calling ptrace with PTRACE_PEEKUSER as the first argument, we can examine the contents of the USER area where register contents and other information is stored. The kernel stores the contents of registers in this area for the parent process to examine through ptrace.

Let's show this with an example:

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/user.h>
#include <sys/syscall.h>   /* For SYS_write etc */
int main()
{   pid_t child;
    long orig_eax, eax;
    long params[3];
    int status;
    int insyscall = 0;
    child = fork();
    if(child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    }
    else {
       while(1) {
          wait(&status);
          if(WIFEXITED(status))
              break;
          orig_eax = ptrace(PTRACE_PEEKUSER,
                     child, 4 * ORIG_EAX, NULL);
          if(orig_eax == SYS_write) {
             if(insyscall == 0) {
                /* Syscall entry */
                insyscall = 1;
                params[0] = ptrace(PTRACE_PEEKUSER,
                                   child, 4 * EBX,
                                   NULL);
                params[1] = ptrace(PTRACE_PEEKUSER,
                                   child, 4 * ECX,
                                   NULL);
                params[2] = ptrace(PTRACE_PEEKUSER,
                                   child, 4 * EDX,
                                   NULL);
                printf("Write called with "
                       "%ld, %ld, %ld\n",
                       params[0], params[1],
                       params[2]);
                }
          else { /* Syscall exit */
                eax = ptrace(PTRACE_PEEKUSER,
                             child, 4 * EAX, NULL);
                    printf("Write returned "
                           "with %ld\n", eax);
                    insyscall = 0;
                }
            }
            ptrace(PTRACE_SYSCALL,
                   child, NULL, NULL);
        }
    }
    return 0;
}

This program should print an output similar to the following:

ppadala@linux:~/ptrace > ls
a.out        dummy.s      ptrace.txt
libgpm.html  registers.c  syscallparams.c
dummy        ptrace.html  simple.c
ppadala@linux:~/ptrace > ./a.out
Write called with 1, 1075154944, 48
a.out        dummy.s      ptrace.txt
Write returned with 48
Write called with 1, 1075154944, 59
libgpm.html  registers.c  syscallparams.c
Write returned with 59
Write called with 1, 1075154944, 30
dummy        ptrace.html  simple.c
Write returned with 30

Here we are tracing the write system calls, and ls makes three write system calls. The call to ptrace, with a first argument of PTRACE_SYSCALL, makes the kernel stop the child process whenever a system call entry or exit is made. It's equivalent to doing a PTRACE_CONT and stopping at the next system call entry/exit.

In the previous example, we used PTRACE_PEEKUSER to look into the arguments of the write system call. When a system call returns, the return value is placed in %eax, and it can be read as shown in that example.

The status variable in the wait call is used to check whether the child has exited. This is the typical way to check whether the child has been stopped by ptrace or was able to exit. For more details on macros like WIFEXITED, see the wait(2) man page.
Reading Register Values

If you want to read register values at the time of a syscall entry or exit, the procedure shown above can be cumbersome. Calling ptrace with a first argument of PTRACE_GETREGS will place all the registers in a single call.

The code to fetch register values looks like this:

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/user.h>
#include <sys/syscall.h>
int main()
{   pid_t child;
    long orig_eax, eax;
    long params[3];
    int status;
    int insyscall = 0;
    struct user_regs_struct regs;
    child = fork();
    if(child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    }
    else {
       while(1) {
          wait(&status);
          if(WIFEXITED(status))
              break;
          orig_eax = ptrace(PTRACE_PEEKUSER,
                            child, 4 * ORIG_EAX,
                            NULL);
          if(orig_eax == SYS_write) {
              if(insyscall == 0) {
                 /* Syscall entry */
                 insyscall = 1;
                 ptrace(PTRACE_GETREGS, child,
                        NULL, &regs);
                 printf("Write called with "
                        "%ld, %ld, %ld\n",
                        regs.ebx, regs.ecx,
                        regs.edx);
             }
             else { /* Syscall exit */
                 eax = ptrace(PTRACE_PEEKUSER,
                              child, 4 * EAX,
                              NULL);
                 printf("Write returned "
                        "with %ld\n", eax);
                 insyscall = 0;
             }
          }
          ptrace(PTRACE_SYSCALL, child,
                 NULL, NULL);
       }
   }
   return 0;
}

This code is similar to the previous example except for the call to ptrace with PTRACE_GETREGS. Here we have made use of the user_regs_struct defined in <linux/user.h> to read the register values.
Doing Funny Things

Now it's time for some fun. In the following example, we will reverse the string passed to the write system call:

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/user.h>
#include <sys/syscall.h>
const int long_size = sizeof(long);
void reverse(char *str)
{   int i, j;
    char temp;
    for(i = 0, j = strlen(str) - 2;
        i <= j; ++i, --j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}
void getdata(pid_t child, long addr,
             char *str, int len)
{   char *laddr;
    int i, j;
    union u {
            long val;
            char chars[long_size];
    }data;
    i = 0;
    j = len / long_size;
    laddr = str;
    while(i < j) {
        data.val = ptrace(PTRACE_PEEKDATA,
                          child, addr + i * 4,
                          NULL);
        memcpy(laddr, data.chars, long_size);
        ++i;
        laddr += long_size;
    }
    j = len % long_size;
    if(j != 0) {
        data.val = ptrace(PTRACE_PEEKDATA,
                          child, addr + i * 4,
                          NULL);
        memcpy(laddr, data.chars, j);
    }
    str[len] = '\0';
}
void putdata(pid_t child, long addr,
             char *str, int len)
{   char *laddr;
    int i, j;
    union u {
            long val;
            char chars[long_size];
    }data;
    i = 0;
    j = len / long_size;
    laddr = str;
    while(i < j) {
        memcpy(data.chars, laddr, long_size);
        ptrace(PTRACE_POKEDATA, child,
               addr + i * 4, data.val);
        ++i;
        laddr += long_size;
    }
    j = len % long_size;
    if(j != 0) {
        memcpy(data.chars, laddr, j);
        ptrace(PTRACE_POKEDATA, child,
               addr + i * 4, data.val);
    }
}
int main()
{
   pid_t child;
   child = fork();
   if(child == 0) {
      ptrace(PTRACE_TRACEME, 0, NULL, NULL);
      execl("/bin/ls", "ls", NULL);
   }
   else {
      long orig_eax;
      long params[3];
      int status;
      char *str, *laddr;
      int toggle = 0;
      while(1) {
         wait(&status);
         if(WIFEXITED(status))
             break;
         orig_eax = ptrace(PTRACE_PEEKUSER,
                           child, 4 * ORIG_EAX,
                           NULL);
         if(orig_eax == SYS_write) {
            if(toggle == 0) {
               toggle = 1;
               params[0] = ptrace(PTRACE_PEEKUSER,
                                  child, 4 * EBX,
                                  NULL);
               params[1] = ptrace(PTRACE_PEEKUSER,
                                  child, 4 * ECX,
                                  NULL);
               params[2] = ptrace(PTRACE_PEEKUSER,
                                  child, 4 * EDX,
                                  NULL);
               str = (char *)calloc((params[2]+1)
                                 * sizeof(char));
               getdata(child, params[1], str,
                       params[2]);
               reverse(str);
               putdata(child, params[1], str,
                       params[2]);
            }
            else {
               toggle = 0;
            }
         }
      ptrace(PTRACE_SYSCALL, child, NULL, NULL);
      }
   }
   return 0;
}

The output looks like this:

ppadala@linux:~/ptrace > ls
a.out        dummy.s      ptrace.txt
libgpm.html  registers.c  syscallparams.c
dummy        ptrace.html  simple.c
ppadala@linux:~/ptrace > ./a.out
txt.ecartp      s.ymmud      tuo.a
c.sretsiger     lmth.mpgbil  c.llacys_egnahc
c.elpmis        lmth.ecartp  ymmud

This example makes use of all the concepts previously discussed, plus a few more. In it, we use calls to ptrace with PTRACE_POKEDATA to change the data values. It works exactly the same way as PTRACE_PEEKDATA, except it both reads and writes the data thatt the child passes in arguments to the system call whereas PEEKDATA only reads the data.

Single-Stepping

ptrace provides features to single-step through the child's code. The call to ptrace(PTRACE_SINGLESTEP,..) tells the kernel to stop the child at each instruction and let the parent take control. The following example shows a way of reading the instruction being executed when a system call is executed. I have created a small dummy executable for you to understand what is happening instead of bothering with the calls made by libc.

Here's the listing for dummy1.s. It's written in assembly language and compiled as gcc -o dummy1 dummy1.s:

.data
hello:
    .string "hello world\n"
.globl  main
main:
    movl    $4, %eax
    movl    $2, %ebx
    movl    $hello, %ecx
    movl    $12, %edx
    int     $0x80
    movl    $1, %eax
    xorl    %ebx, %ebx
    int     $0x80
    ret

The example program that single-steps through the above code is:

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/user.h>
#include <sys/syscall.h>
int main()
{   pid_t child;
    const int long_size = sizeof(long);
    child = fork();
    if(child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("./dummy1", "dummy1", NULL);
    }
    else {
        int status;
        union u {
            long val;
            char chars[long_size];
        }data;
        struct user_regs_struct regs;
        int start = 0;
        long ins;
        while(1) {
            wait(&status);
            if(WIFEXITED(status))
                break;
            ptrace(PTRACE_GETREGS,
                   child, NULL, &regs);
            if(start == 1) {
                ins = ptrace(PTRACE_PEEKTEXT,
                             child, regs.eip,
                             NULL);
                printf("EIP: %lx Instruction "
                       "executed: %lx\n",
                       regs.eip, ins);
            }
            if(regs.orig_eax == SYS_write) {
                start = 1;
                ptrace(PTRACE_SINGLESTEP, child,
                       NULL, NULL);
            }
            else
                ptrace(PTRACE_SYSCALL, child,
                       NULL, NULL);
        }
    }
    return 0;
}

This program prints:

hello world
EIP: 8049478 Instruction executed: 80cddb31
EIP: 804947c Instruction executed: c3

You might have to look at Intel's manuals to make sense out of those instruction bytes. Using single stepping for more complex processes, such as setting breakpoints, requires careful design and more complex code.

Attaching to a Running Process

In Part I, we ran the process to be traced as a child after calling ptrace(PTRACE_TRACEME, ..). If you simply wanted to see how the process is making system calls and trace the program, this would be sufficient. If you want to trace or debug a process already running, then ptrace(PTRACE_ATTACH, ..) should be used.

When a ptrace(PTRACE_ATTACH, ..) is called with the pid to be traced, it is roughly equivalent to the process calling ptrace(PTRACE_TRACEME, ..) and becoming a child of the tracing process. The traced process is sent a SIGSTOP, so we can examine and modify the process as usual. After we are done with modifications or tracing, we can let the traced process continue on its own by calling ptrace(PTRACE_DETACH, ..).

The following is the code for a small example tracing program:

int main()
{   int i;
    for(i = 0;i < 10; ++i) {
        printf("My counter: %d\n", i);
        sleep(2);
    }
    return 0;
}

Save the program as dummy2.c. Compile and run it:

gcc -o dummy2 dummy2.c
./dummy2 &

Now, we can attach to dummy2 by using the code below:

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/user.h>   /* For user_regs_struct
                             etc. */
int main(int argc, char *argv[])
{   pid_t traced_process;
    struct user_regs_struct regs;
    long ins;
    if(argc != 2) {
        printf("Usage: %s <pid to be traced>\n",
               argv[0], argv[1]);
        exit(1);
    }
    traced_process = atoi(argv[1]);
    ptrace(PTRACE_ATTACH, traced_process,
           NULL, NULL);
    wait(NULL);
    ptrace(PTRACE_GETREGS, traced_process,
           NULL, &regs);
    ins = ptrace(PTRACE_PEEKTEXT, traced_process,
                 regs.eip, NULL);
    printf("EIP: %lx Instruction executed: %lx\n",
           regs.eip, ins);
    ptrace(PTRACE_DETACH, traced_process,
           NULL, NULL);
    return 0;
}

The above program simply attaches to a process, waits for it to stop, examines its eip (instruction pointer) and detaches.

To inject code use ptrace(PTRACE_POKETEXT, ..) and ptrace(PTRACE_POKEDATA, ..) after the traced process has stopped.
Setting Breakpoints

How do debuggers set breakpoints? Generally, they replace the instruction to be executed with a trap instruction, so that when the traced program stops, the tracing program, the debugger, can examine it. It will replace the original instruction once the tracing program continues the traced process. Here's an example:

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/user.h>
const int long_size = sizeof(long);
void getdata(pid_t child, long addr,
             char *str, int len)
{   char *laddr;
    int i, j;
    union u {
            long val;
            char chars[long_size];
    }data;
    i = 0;
    j = len / long_size;
    laddr = str;
    while(i < j) {
        data.val = ptrace(PTRACE_PEEKDATA, child,
                          addr + i * 4, NULL);
        memcpy(laddr, data.chars, long_size);
        ++i;
        laddr += long_size;
    }
    j = len % long_size;
    if(j != 0) {
        data.val = ptrace(PTRACE_PEEKDATA, child,
                          addr + i * 4, NULL);
        memcpy(laddr, data.chars, j);
    }
    str[len] = '\0';
}
void putdata(pid_t child, long addr,
             char *str, int len)
{   char *laddr;
    int i, j;
    union u {
            long val;
            char chars[long_size];
    }data;
    i = 0;
    j = len / long_size;
    laddr = str;
    while(i < j) {
        memcpy(data.chars, laddr, long_size);
        ptrace(PTRACE_POKEDATA, child,
               addr + i * 4, data.val);
        ++i;
        laddr += long_size;
    }
    j = len % long_size;
    if(j != 0) {
        memcpy(data.chars, laddr, j);
        ptrace(PTRACE_POKEDATA, child,
               addr + i * 4, data.val);
    }
}
int main(int argc, char *argv[])
{   pid_t traced_process;
    struct user_regs_struct regs, newregs;
    long ins;
    /* int 0x80, int3 */
    char code[] = {0xcd,0x80,0xcc,0};
    char backup[4];
    if(argc != 2) {
        printf("Usage: %s <pid to be traced>\n",
               argv[0], argv[1]);
        exit(1);
    }
    traced_process = atoi(argv[1]);
    ptrace(PTRACE_ATTACH, traced_process,
           NULL, NULL);
    wait(NULL);
    ptrace(PTRACE_GETREGS, traced_process,
           NULL, &regs);
    /* Copy instructions into a backup variable */
    getdata(traced_process, regs.eip, backup, 3);
    /* Put the breakpoint */
    putdata(traced_process, regs.eip, code, 3);
    /* Let the process continue and execute
       the int 3 instruction */
    ptrace(PTRACE_CONT, traced_process, NULL, NULL);
    wait(NULL);
    printf("The process stopped, putting back "
           "the original instructions\n");
    printf("Press <enter> to continue\n");
    getchar();
    putdata(traced_process, regs.eip, backup, 3);
    /* Setting the eip back to the original
       instruction to let the process continue */
    ptrace(PTRACE_SETREGS, traced_process,
           NULL, &regs);
    ptrace(PTRACE_DETACH, traced_process,
           NULL, NULL);
    return 0;
}

Here we replace the three bytes with the code for a trap instruction, and when the process stops, we replace the original instructions and reset the eip to original location. Figures 1-4 clarify how the instruction stream looks when above program is executed.



Figure 1. After the Process Is Stopped


Figure 2. After the Trap Instruction Bytes Are Set



Figure 3. Trap Is Hit and Control Is Given to the Tracing Program


Figure 4. After the Original Instructions Are Replaced and eip Is Reset to the Original Location

Now that we have a clear idea of how breakpoints are set, let's inject some code bytes into a running program. These code bytes will print “hello world”.

The following program is a simple “hello world” program with modifications to fit our needs. Compile the following program with:

gcc -o hello hello.c
void main()
{
__asm__("
         jmp forward
backward:
         popl   %esi      # Get the address of
                          # hello world string
         movl   $4, %eax  # Do write system call
         movl   $2, %ebx
         movl   %esi, %ecx
         movl   $12, %edx
         int    $0x80
         int3             # Breakpoint. Here the
                          # program will stop and
                          # give control back to
                          # the parent
forward:
         call   backward
         .string \"Hello World\\n\""
       );
}

The jumping backward and forward here is required to find the address of the “hello world” string.

We can get the machine code for the above assembly from GDB. Fire up GDB and disassemble the program:

(gdb) disassemble main
Dump of assembler code for function main:
0x80483e0 <main>:       push   %ebp
0x80483e1 <main+1>:     mov    %esp,%ebp
0x80483e3 <main+3>:     jmp    0x80483fa <forward>
End of assembler dump.
(gdb) disassemble forward
Dump of assembler code for function forward:
0x80483fa <forward>:    call   0x80483e5 <backward>
0x80483ff <forward+5>:  dec    %eax
0x8048400 <forward+6>:  gs
0x8048401 <forward+7>:  insb   (%dx),%es:(%edi)
0x8048402 <forward+8>:  insb   (%dx),%es:(%edi)
0x8048403 <forward+9>:  outsl  %ds:(%esi),(%dx)
0x8048404 <forward+10>: and    %dl,0x6f(%edi)
0x8048407 <forward+13>: jb     0x8048475
0x8048409 <forward+15>: or     %fs:(%eax),%al
0x804840c <forward+18>: mov    %ebp,%esp
0x804840e <forward+20>: pop    %ebp
0x804840f <forward+21>: ret
End of assembler dump.
(gdb) disassemble backward
Dump of assembler code for function backward:
0x80483e5 <backward>:   pop    %esi
0x80483e6 <backward+1>: mov    $0x4,%eax
0x80483eb <backward+6>: mov    $0x2,%ebx
0x80483f0 <backward+11>:        mov    %esi,%ecx
0x80483f2 <backward+13>:        mov    $0xc,%edx
0x80483f7 <backward+18>:        int    $0x80
0x80483f9 <backward+20>:        int3
End of assembler dump.

We need to take the machine code bytes from main+3 to backward+20, which is a total of 41 bytes. The machine code can be seen with the x command in GDB:

(gdb) x/40bx main+3
<main+3>: eb 15 5e b8 04 00 00 00
<backward+6>: bb 02 00 00 00 89 f1 ba
<backward+14>: 0c 00 00 00 cd 80 cc
<forward+1>: e6 ff ff ff 48 65 6c 6c
<forward+9>: 6f 20 57 6f 72 6c 64 0a

Now we have the instruction bytes to be executed. Why wait? We can inject them using the same method as in the previous example. The following is the source code; only the main function is given here:

int main(int argc, char *argv[])
{   pid_t traced_process;
    struct user_regs_struct regs, newregs;
    long ins;
    int len = 41;
    char insertcode[] =
"\xeb\x15\x5e\xb8\x04\x00"
        "\x00\x00\xbb\x02\x00\x00\x00\x89\xf1\xba"
        "\x0c\x00\x00\x00\xcd\x80\xcc\xe8\xe6\xff"
        "\xff\xff\x48\x65\x6c\x6c\x6f\x20\x57\x6f"
        "\x72\x6c\x64\x0a\x00";
    char backup[len];
    if(argc != 2) {
        printf("Usage: %s <pid to be traced>\n",
               argv[0], argv[1]);
        exit(1);
    }
    traced_process = atoi(argv[1]);
    ptrace(PTRACE_ATTACH, traced_process,
           NULL, NULL);
    wait(NULL);
    ptrace(PTRACE_GETREGS, traced_process,
           NULL, &regs);
    getdata(traced_process, regs.eip, backup, len);
    putdata(traced_process, regs.eip,
            insertcode, len);
    ptrace(PTRACE_SETREGS, traced_process,
           NULL, &regs);
    ptrace(PTRACE_CONT, traced_process,
           NULL, NULL);
    wait(NULL);
    printf("The process stopped, Putting back "
           "the original instructions\n");
    putdata(traced_process, regs.eip, backup, len);
    ptrace(PTRACE_SETREGS, traced_process,
           NULL, &regs);
    printf("Letting it continue with "
           "original flow\n");
    ptrace(PTRACE_DETACH, traced_process,
           NULL, NULL);
    return 0;
}

njecting the Code into Free Space

In the previous example we injected the code directly into the executing instruction stream. However, debuggers can get confused with this kind of behaviour, so let's find the free space in the process and inject the code there. We can find free space by examining the /proc/pid/maps file of the traced process. The following function will find the starting address of this map:

long freespaceaddr(pid_t pid)
{
    FILE *fp;
    char filename[30];
    char line[85];
    long addr;
    char str[20];
    sprintf(filename, "/proc/%d/maps", pid);
    fp = fopen(filename, "r");
    if(fp == NULL)
        exit(1);
    while(fgets(line, 85, fp) != NULL) {
        sscanf(line, "%lx-%*lx %*s %*s %s", &addr,
               str, str, str, str);
        if(strcmp(str, "00:00") == 0)
            break;
    }
    fclose(fp);
    return addr;
}

Each line in /proc/pid/maps represents a mapped region of the process. An entry in /proc/pid/maps looks like this:

map start-mapend    protection  offset     device
inode      process file
08048000-0804d000   r-xp        00000000   03:08
66111      /opt/kde2/bin/kdeinit

The following program injects code into free space. It's similar to the previous injection program except the free space address is used for keeping our new code. Here is the source code for the main function:

int main(int argc, char *argv[])
{   pid_t traced_process;
    struct user_regs_struct oldregs, regs;
    long ins;
    int len = 41;
    char insertcode[] =
"\xeb\x15\x5e\xb8\x04\x00"
        "\x00\x00\xbb\x02\x00\x00\x00\x89\xf1\xba"
        "\x0c\x00\x00\x00\xcd\x80\xcc\xe8\xe6\xff"
        "\xff\xff\x48\x65\x6c\x6c\x6f\x20\x57\x6f"
        "\x72\x6c\x64\x0a\x00";
    char backup[len];
    long addr;
    if(argc != 2) {
        printf("Usage: %s <pid to be traced>\n",
               argv[0], argv[1]);
        exit(1);
    }
    traced_process = atoi(argv[1]);
    ptrace(PTRACE_ATTACH, traced_process,
           NULL, NULL);
    wait(NULL);
    ptrace(PTRACE_GETREGS, traced_process,
           NULL, &regs);
    addr = freespaceaddr(traced_process);
    getdata(traced_process, addr, backup, len);
    putdata(traced_process, addr, insertcode, len);
    memcpy(&oldregs, &regs, sizeof(regs));
    regs.eip = addr;
    ptrace(PTRACE_SETREGS, traced_process,
           NULL, &regs);
    ptrace(PTRACE_CONT, traced_process,
           NULL, NULL);
    wait(NULL);
    printf("The process stopped, Putting back "
           "the original instructions\n");
    putdata(traced_process, addr, backup, len);
    ptrace(PTRACE_SETREGS, traced_process,
           NULL, &oldregs);
    printf("Letting it continue with "
           "original flow\n");
    ptrace(PTRACE_DETACH, traced_process,
           NULL, NULL);
    return 0;
}

Behind the Scenes

So what happens within the kernel now? How is ptrace implemented? This section could be an article on its own; however, here's a brief description of what happens.

When a process calls ptrace with PTRACE_TRACEME, the kernel sets up the process flags to reflect that it is being traced:

Source: arch/i386/kernel/ptrace.c
if (request == PTRACE_TRACEME) {
    /* are we already being traced? */
    if (current->ptrace & PT_PTRACED)
        goto out;
    /* set the ptrace bit in the process flags. */
    current->ptrace |= PT_PTRACED;
    ret = 0;
    goto out;
}

When a system call entry is done, the kernel checks this flag and calls the trace system call if the process is being traced. The gory assembly details can be found in arch/i386/kernel/entry.S.

Now, we are in the sys_trace() function as defined in arch/i386/kernel/ptrace.c. It stops the child and sends a signal to the parent notifying that the child is stopped. This wakes up the waiting parent, and it does the ptrace magic. Once the parent is done, and it calls ptrace(PTRACE_CONT, ..) or ptrace(PTRACE_SYSCALL, ..), it wakes up the child by calling the scheduler function wake_up_process(). Some other architectures can implement this by sending a SIGCHLD to child.
Conclusion

ptrace may appear to be magic to some people, because it can examine and modify a running program. It is generally used by debuggers and system call tracing programs, such as ptrace. It opens up interesting possibilities for doing user-mode extensions as well. There have been a lot of attempts to extend the operating system on the user level. See Resources to read about UFO, a user-level extension to filesystems. ptrace also is used to employ security mechanisms.

All example code from this article and from Part I is available as a tar archive on the Linux Journal FTP site [

Site
http://www.linuxjournal.com/article/6100
http://www.linuxjournal.com/article/6210

All example code
ftp://ftp.linuxjournal.com/pub/lj/listings/issue103/6011.tgz
ftp://ftp.linuxjournal.com/pub/lj/listings/issue104/6210.tgz



你想过怎么实现对系统调用的拦截吗？你尝试过通过改变系统调用的参数来愚弄你的系统kernel吗？你想过调试器是如何使运行中的进程暂停并且控制它吗？

你可能会开始考虑怎么使用复杂的kernel编程来达到目的，那么，你错了。实际上Linux提供了一种优雅的机制来完成这些：ptrace系统函数。 ptrace提供了一种使父进程得以监视和控制其它进程的方式，它还能够改变子进程中的寄存器和内核映像，因而可以实现断点调试和系统调用的跟踪。

使用ptrace，你可以在用户层拦截和修改系统调用(sys call)

在这篇文章中，我们将学习如何拦截一个系统调用，然后修改它的参数。在本文的第二部分我们将学习更先进的技术：设置断点，插入代码到一个正在运行的程序中；我们将潜入到机器内部，偷窥和纂改进程的寄存器和数据段。



基本知识

操作系统提供了一种标准的服务来让程序员实现对底层硬件和服务的控制（比如文件系统），叫做系统调用(system calls)。当一个程序需要作系统调用的时候，它将相关参数放进系统调用相关的寄存器，然后调用软中断0x80，这个中断就像一个让程序得以接触到内核模式的窗口，程序将参数和系统调用号交给内核，内核来完成系统调用的执行。



在i386体系中(本文中所有的代码都是面向i386体系)，系统调用号将放入%eax,它的参数则依次放入%ebx, %ecx, %edx, %esi 和 %edi。 比如，在以下的调用



       Write(2, “Hello”, 5)



的汇编形式大概是这样的



    movl   $4, %eax

    movl   $2, %ebx

    movl   $hello, %ecx

    movl   $5, %edx

    int    $0x80



这里的$hello指向的是标准字符串”Hello”。



那么，ptrace会在什么时候出现呢？在执行系统调用之前，内核会先检查当前进程是否处于被“跟踪”(traced)的状态。如果是的话，内核暂停当前进程并将控制权交给跟踪进程，使跟踪进程得以察看或者修改被跟踪进程的寄存器。



让我们来看一个例子，演示这个跟踪程序的过程


#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/user.h>   /* For constants
                                   ORIG_EAX etc */
int main()
{
   pid_t child;
    long orig_eax;
    child = fork();
    if(child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    }
    else {
        wait(NULL);
        orig_eax = ptrace(PTRACE_PEEKUSER,
                          child, 4 * ORIG_EAX,
                          NULL);
        printf("The child made a "
               "system call %ld ", orig_eax);
        ptrace(PTRACE_CONT, child, NULL, NULL);
    }
    return 0;
}

运行这个程序，将会在输出ls命令的结果的同时，输出:



The child made a system call 11



说明：11是execve的系统调用号，这是该程序调用的第一个系统调用。

想知道系统调用号的详细内容，察看 /usr/include/asm/unistd.h。



在以上的示例中，父进程fork出了一个子进程，然后跟踪它。在调用exec函数之前，子进程用PTRACE_TRACEME作为第一个参数调用了ptrace函数，它告诉内核：让别人跟踪我吧！然后，在子进程调用了execve()之后，它将控制权交还给父进程。当时父进程正使用wait()函数来等待来自内核的通知，现在它得到了通知，于是它可以开始察看子进程都作了些什么，比如看看寄存器的值之类。



出现系统调用之后，内核会将eax中的值（此时存的是系统调用号）保存起来，我们可以使用PTRACE_PEEKUSER作为ptrace的第一个参数来读到这个值。

我们察看完系统调用的信息后，可以使用PTRACE_CONT作为ptrace的第一个参数，调用ptrace使子进程继续系统调用的过程。



ptrace函数的参数



Ptrace有四个参数

long ptrace(enum __ptrace_request request,

            pid_t pid,

            void *addr,

            void *data);



第一个参数决定了ptrace的行为与其它参数的使用方法，可取的值有:

PTRACE_ME

PTRACE_PEEKTEXT

PTRACE_PEEKDATA

PTRACE_PEEKUSER

PTRACE_POKETEXT

PTRACE_POKEDATA

PTRACE_POKEUSER

PTRACE_GETREGS

PTRACE_GETFPREGS,

PTRACE_SETREGS

PTRACE_SETFPREGS

PTRACE_CONT

PTRACE_SYSCALL,

PTRACE_SINGLESTEP

PTRACE_DETACH

在下文中将对这些常量的用法进行说明。



读取系统调用的参数



通过将PTRACE_PEEKUSER作为ptrace 的第一个参数进行调用，可以取得与子进程相关的寄存器值。



先看下面这个例子

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/user.h>
#include <sys/syscall.h>   /* For SYS_write etc */

int main()
{
    pid_t child;
    long orig_eax, eax;
    long params[3];
    int status;
    int insyscall = 0;
    child = fork();
    if(child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    }
    else {
       while(1) {
          wait(&status);
          if(WIFEXITED(status))
              break;
          orig_eax = ptrace(PTRACE_PEEKUSER,
                     child, 4 * ORIG_EAX, NULL);
          if(orig_eax == SYS_write) {
             if(insyscall == 0) {
                /* Syscall entry */
                insyscall = 1;
                params[0] = ptrace(PTRACE_PEEKUSER,
                                   child, 4 * EBX,
                                   NULL);
                params[1] = ptrace(PTRACE_PEEKUSER,
                                   child, 4 * ECX,
                                   NULL);
                params[2] = ptrace(PTRACE_PEEKUSER,
                                   child, 4 * EDX,
                                   NULL);
                printf("Write called with "
                       "%ld, %ld, %ld ",
                       params[0], params[1],
                       params[2]);
                }
          else { /* Syscall exit */
                eax = ptrace(PTRACE_PEEKUSER,
                             child, 4 * EAX, NULL);
                    printf("Write returned "
                           "with %ld ", eax);
                    insyscall = 0;
                }
            }
            ptrace(PTRACE_SYSCALL,
                   child, NULL, NULL);
        }
    }
    return 0;
}

这个程序的输出是这样的



ppadala@linux:~/ptrace > ls

a.out        dummy.s      ptrace.txt

libgpm.html  registers.c  syscallparams.c

dummy        ptrace.html  simple.c

ppadala@linux:~/ptrace > ./a.out

Write called with 1, 1075154944, 48

a.out        dummy.s      ptrace.txt

Write returned with 48

Write called with 1, 1075154944, 59

libgpm.html  registers.c  syscallparams.c

Write returned with 59

Write called with 1, 1075154944, 30

dummy        ptrace.html  simple.c

Write returned with 30





以上的例子中我们跟踪了write系统调用，而ls命令的执行将产生三个write系统调用。使用PTRACE_SYSCALL作为ptrace的第一个参数，使内核在子进程做出系统调用或者准备退出的时候暂停它。这种行为与使用PTRACE_CONT，然后在下一个系统调用/进程退出时暂停它是等价的。



在前一个例子中，我们用PTRACE_PEEKUSER来察看write系统调用的参数。系统调用的返回值会被放入%eax。



wait函数使用status变量来检查子进程是否已退出。它是用来判断子进程是被ptrace暂停掉还是已经运行结束并退出。有一组宏可以通过status的值来判断进程的状态，比如WIFEXITED等，详情可以察看wait(2) man。



读取寄存器的值



如果你想在系统调用或者进程终止的时候读取它的寄存器，使用前面那个例子的方法是可以的，但是这是笨拙的方法。使用PRACE_GETREGS作为ptrace的第一个参数来调用，可以只需一次函数调用就取得所有的相关寄存器值。

获得寄存器值得例子如下：
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/user.h>
#include <sys/syscall.h>

int main()
{
    pid_t child;
    long orig_eax, eax;
    long params[3];
    int status;
    int insyscall = 0;
    struct user_regs_struct regs;
    child = fork();
    if(child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    }
    else {
       while(1) {
          wait(&status);
          if(WIFEXITED(status))
              break;
          orig_eax = ptrace(PTRACE_PEEKUSER,
                            child, 4 * ORIG_EAX,
                            NULL);
          if(orig_eax == SYS_write) {
              if(insyscall == 0) {
                 /* Syscall entry */
                 insyscall = 1;
                 ptrace(PTRACE_GETREGS, child,
                        NULL, &regs);
                 printf("Write called with "
                        "%ld, %ld, %ld ",
                        regs.ebx, regs.ecx,
                        regs.edx);
             }
             else { /* Syscall exit */
                 eax = ptrace(PTRACE_PEEKUSER,
                              child, 4 * EAX,
                              NULL);
                 printf("Write returned "
                        "with %ld ", eax);
                 insyscall = 0;
             }
          }
          ptrace(PTRACE_SYSCALL, child,
                 NULL, NULL);
       }
   }
   return 0;
}

这段代码与前面的例子是比较相似的，不同的是它使用了PTRACE_GETREGS。 其中的user_regs_struct结构是在<linux/user.h>中定义的。





来点好玩的



现在该做点有意思的事情了，我们将要把传给write系统调用的字符串给反转。

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/user.h>
#include <sys/syscall.h>

const int long_size = sizeof(long);

void reverse(char *str)
{
    int i, j;
    char temp;
    for(i = 0, j = strlen(str) - 2;
        i <= j; ++i, --j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

void getdata(pid_t child, long addr,
             char *str, int len)
{
    char *laddr;
    int i, j;
    union u {
            long val;
            char chars[long_size];
    }data;

    i = 0;
    j = len / long_size;
    laddr = str;
    while(i < j) {
        data.val = ptrace(PTRACE_PEEKDATA,
                          child, addr + i * 4,
                          NULL);
        memcpy(laddr, data.chars, long_size);
        ++i;
        laddr += long_size;
    }
    j = len % long_size;
    if(j != 0) {
        data.val = ptrace(PTRACE_PEEKDATA,
                          child, addr + i * 4,
                          NULL);
        memcpy(laddr, data.chars, j);
    }
    str[len] = '';
}

void putdata(pid_t child, long addr,
             char *str, int len)
{
    char *laddr;
    int i, j;
    union u {
            long val;
            char chars[long_size];
    }data;

    i = 0;
    j = len / long_size;
    laddr = str;
    while(i < j) {
        memcpy(data.chars, laddr, long_size);
        ptrace(PTRACE_POKEDATA, child,
               addr + i * 4, data.val);
        ++i;
        laddr += long_size;
    }
    j = len % long_size;
    if(j != 0) {
        memcpy(data.chars, laddr, j);
        ptrace(PTRACE_POKEDATA, child,
               addr + i * 4, data.val);
    }
}

int main()
{
   pid_t child;
   child = fork();
   if(child == 0) {
      ptrace(PTRACE_TRACEME, 0, NULL, NULL);
      execl("/bin/ls", "ls", NULL);
   }
   else {
      long orig_eax;
      long params[3];
      int status;
      char *str, *laddr;
      int toggle = 0;
      while(1) {
         wait(&status);
         if(WIFEXITED(status))
             break;
         orig_eax = ptrace(PTRACE_PEEKUSER,
                           child, 4 * ORIG_EAX,
                           NULL);
         if(orig_eax == SYS_write) {
            if(toggle == 0) {
               toggle = 1;
               params[0] = ptrace(PTRACE_PEEKUSER,
                                  child, 4 * EBX,
                                  NULL);
               params[1] = ptrace(PTRACE_PEEKUSER,
                                  child, 4 * ECX,
                                  NULL);
               params[2] = ptrace(PTRACE_PEEKUSER,
                                  child, 4 * EDX,
                                  NULL);
               str = (char *)calloc((params[2]+1)
                                 * sizeof(char));
               getdata(child, params[1], str,
                       params[2]);
               reverse(str);
               putdata(child, params[1], str,
                       params[2]);
            }
            else {
               toggle = 0;
            }
         }
      ptrace(PTRACE_SYSCALL, child, NULL, NULL);
      }
   }
   return 0;
}

输出是这样的：



ppadala@linux:~/ptrace > ls

a.out        dummy.s      ptrace.txt

libgpm.html  registers.c  syscallparams.c

dummy        ptrace.html  simple.c

ppadala@linux:~/ptrace > ./a.out

txt.ecartp      s.ymmud      tuo.a

c.sretsiger     lmth.mpgbil  c.llacys_egnahc

c.elpmis        lmth.ecartp  ymmud





这个例子中涵盖了前面讨论过的所有知识点，当然还有些新的内容。这里我们用PTRACE_POKEDATA作为第一个参数，以此来改变子进程中的变量值。它以与PTRACE_PEEKDATA相似的方式工作，当然，它不只是偷窥变量的值了，它可以修改它们。



单步



ptrace提供了对子进程进行单步的功能。 ptrace(PTRACE_SINGLESTEP, …) 会使内核在子进程的每一条指令执行前先将其阻塞，然后将控制权交给父进程。下面的例子可以查出子进程当前将要执行的指令。为了便于理解，我用汇编写了这个受控程序，而不是让你为c的库函数到底会作那些系统调用而头痛。



以下是被控程序的代码 dummy1.s，使用gcc  –o dummy1 dummy1.s来编译



.data

hello:

    .string "hello world/n"

.globl  main

main:

    movl    $4, %eax

    movl    $2, %ebx

    movl    $hello, %ecx

    movl    $12, %edx

    int     $0x80

    movl    $1, %eax

    xorl    %ebx, %ebx

    int     $0x80

    ret





以下的程序则用来完成单步：

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/user.h>
#include <sys/syscall.h>
int main()
{
    pid_t child;
    const int long_size = sizeof(long);
    child = fork();
    if(child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("./dummy1", "dummy1", NULL);
    }
    else {
        int status;
        union u {
            long val;
            char chars[long_size];
        }data;
        struct user_regs_struct regs;
        int start = 0;
        long ins;
        while(1) {
            wait(&status);
            if(WIFEXITED(status))
                break;
            ptrace(PTRACE_GETREGS,
                   child, NULL, &regs);
            if(start == 1) {
                ins = ptrace(PTRACE_PEEKTEXT,
                             child, regs.eip,
                             NULL);
                printf("EIP: %lx Instruction "
                       "executed: %lx ",
                       regs.eip, ins);
            }
            if(regs.orig_eax == SYS_write) {
                start = 1;
                ptrace(PTRACE_SINGLESTEP, child,
                       NULL, NULL);
            }
            else
                ptrace(PTRACE_SYSCALL, child,
                       NULL, NULL);
        }
    }
    return 0;
}

程序的输出是这样的：

你可能需要察看Intel的用户手册来了解这些指令代码的意思。

更复杂的单步，比如设置断点，则需要很仔细的设计和更复杂的代码才可以实现。



在第一部分中我们已经看到ptrace怎么获取子进程的系统调用以及改变系统调用的参数。在这篇文章中，我们将要研究如何在子进程中设置断点和往运行中的程序里插入代码。实际上调试器就是用这种方法来设置断点和执行调试句柄。与前面一样，这里的所有代码都是针对i386平台的。



附着在进程上



在第一部分钟，我们使用ptrace(PTRACE_TRACEME, …)来跟踪一个子进程，如果你只是想要看进程是怎么进行系统调用和跟踪程序的，这个做法是不错的。但如果你要对运行中的进程进行调试，则需要使用 ptrace( PTRACE_ATTACH, ….)



当 ptrace( PTRACE_ATTACH, …)在被调用的时候传入了子进程的pid时， 它大体是与ptrace( PTRACE_TRACEME, …)的行为相同的，它会向子进程发送SIGSTOP信号，于是我们可以察看和修改子进程，然后使用 ptrace( PTRACE_DETACH, …)来使子进程继续运行下去。



下面是调试程序的一个简单例子


int main()
{
   int i;
    for(i = 0;i < 10; ++i) {
        printf("My counter: %d ", i);
        sleep(2);
    }
    return 0;
}



将上面的代码保存为dummy2.c。按下面的方法编译运行：

gcc -o dummy2 dummy2.c

./dummy2 &



现在我们可以用下面的代码来附着到dummy2上。


#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/user.h>   /* For user_regs_struct
                             etc. */
int main(int argc, char *argv[])
{
    pid_t traced_process;
    struct user_regs_struct regs;
    long ins;
    if(argc != 2) {
        printf("Usage: %s <pid to be traced> ",
               argv[0], argv[1]);
        exit(1);
    }
    traced_process = atoi(argv[1]);
    ptrace(PTRACE_ATTACH, traced_process,
           NULL, NULL);
    wait(NULL);
    ptrace(PTRACE_GETREGS, traced_process,
           NULL, &regs);
    ins = ptrace(PTRACE_PEEKTEXT, traced_process,
                 regs.eip, NULL);
    printf("EIP: %lx Instruction executed: %lx ",
           regs.eip, ins);
    ptrace(PTRACE_DETACH, traced_process,
           NULL, NULL);
    return 0;
}



上面的程序仅仅是附着在子进程上，等待它结束，并测量它的eip( 指令指针)然后释放子进程。



设置断点



调试器是怎么设置断点的呢？通常是将当前将要执行的指令替换成trap指令，于是被调试的程序就会在这里停滞，这时调试器就可以察看被调试程序的信息了。被调试程序恢复运行以后调试器会把原指令再放回来。这里是一个例子：


#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/user.h>

const int long_size = sizeof(long);

void getdata(pid_t child, long addr,
             char *str, int len)
{
    char *laddr;
    int i, j;
    union u {
            long val;
            char chars[long_size];
    }data;

    i = 0;
    j = len / long_size;
    laddr = str;

    while(i < j) {
        data.val = ptrace(PTRACE_PEEKDATA, child,
                          addr + i * 4, NULL);
        memcpy(laddr, data.chars, long_size);
        ++i;
        laddr += long_size;
    }
    j = len % long_size;
    if(j != 0) {
        data.val = ptrace(PTRACE_PEEKDATA, child,
                          addr + i * 4, NULL);
        memcpy(laddr, data.chars, j);
    }
    str[len] = '';
}

void putdata(pid_t child, long addr,
             char *str, int len)
{
    char *laddr;
    int i, j;
    union u {
            long val;
            char chars[long_size];
    }data;

    i = 0;
    j = len / long_size;
    laddr = str;
    while(i < j) {
        memcpy(data.chars, laddr, long_size);
        ptrace(PTRACE_POKEDATA, child,
               addr + i * 4, data.val);
        ++i;
        laddr += long_size;
    }
    j = len % long_size;
    if(j != 0) {
        memcpy(data.chars, laddr, j);
        ptrace(PTRACE_POKEDATA, child,
               addr + i * 4, data.val);
    }
}

int main(int argc, char *argv[])
{
    pid_t traced_process;
    struct user_regs_struct regs, newregs;
    long ins;
    /* int 0x80, int3 */
    char code[] = {0xcd,0x80,0xcc,0};
    char backup[4];
    if(argc != 2) {
        printf("Usage: %s <pid to be traced> ",
               argv[0], argv[1]);
        exit(1);
    }
    traced_process = atoi(argv[1]);
    ptrace(PTRACE_ATTACH, traced_process,
           NULL, NULL);
    wait(NULL);
    ptrace(PTRACE_GETREGS, traced_process,
           NULL, &regs);
    /* Copy instructions into a backup variable */
    getdata(traced_process, regs.eip, backup, 3);
    /* Put the breakpoint */
    putdata(traced_process, regs.eip, code, 3);
    /* Let the process continue and execute
       the int 3 instruction */
    ptrace(PTRACE_CONT, traced_process, NULL, NULL);
    wait(NULL);
    printf("The process stopped, putting back "
           "the original instructions ");
    printf("Press <enter> to continue ");
    getchar();
    putdata(traced_process, regs.eip, backup, 3);
    /* Setting the eip back to the original
       instruction to let the process continue */
    ptrace(PTRACE_SETREGS, traced_process,
           NULL, &regs);
    ptrace(PTRACE_DETACH, traced_process,
           NULL, NULL);
    return 0;

}



上面的程序将把三个byte的内容进行替换以执行trap指令，等被调试进程停滞以后，我们把原指令再替换回来并把eip修改为原来的值。下面的图中演示了指令的执行过程








1. 进程停滞后


2. 替换入trap指令






3.断点成功，控制权交给了调试器


4. 继续运行，将原指令替换回来并将eip复原





在了解了断点的机制以后，往运行中的程序里面添加指令也不再是难事了，下面的代码会使原程序多出一个”hello world”的输出



这时一个简单的”hello world”程序，当然为了我们的特殊需要作了点修改：


void main()
{
__asm__("
         jmp forward
backward:
         popl   %esi      # Get the address of
                          # hello world string
         movl   $4, %eax  # Do write system call
         movl   $2, %ebx
         movl   %esi, %ecx
         movl   $12, %edx
         int    $0x80
         int3             # Breakpoint. Here the
                          # program will stop and
                          # give control back to
                          # the parent
forward:
         call   backward
         .string "Hello World/n""
       );
}



使用 gcc –o hello hello.c来编译它。



在backward和forward之间的跳转是为了使程序能够找到”hello world” 字符串的地址。



使用GDB我们可以得到上面那段程序的机器码。启动GDB,然后对程序进行反汇编：



(gdb) disassemble main

Dump of assembler code for function main:

0x80483e0 <main>:       push   %ebp

0x80483e1 <main+1>:     mov    %esp,%ebp

0x80483e3 <main+3>:     jmp    0x80483fa <forward>

End of assembler dump.

(gdb) disassemble forward

Dump of assembler code for function forward:

0x80483fa <forward>:    call   0x80483e5 <backward>

0x80483ff <forward+5>:  dec    %eax

0x8048400 <forward+6>:  gs

0x8048401 <forward+7>:  insb   (%dx),%es:(%edi)

0x8048402 <forward+8>:  insb   (%dx),%es:(%edi)

0x8048403 <forward+9>:  outsl  %ds:(%esi),(%dx)

0x8048404 <forward+10>: and    %dl,0x6f(%edi)

0x8048407 <forward+13>: jb     0x8048475

0x8048409 <forward+15>: or     %fs:(%eax),%al

0x804840c <forward+18>: mov    %ebp,%esp

0x804840e <forward+20>: pop    %ebp

0x804840f <forward+21>: ret

End of assembler dump.

(gdb) disassemble backward

Dump of assembler code for function backward:

0x80483e5 <backward>:   pop    %esi

0x80483e6 <backward+1>: mov    $0x4,%eax

0x80483eb <backward+6>: mov    $0x2,%ebx

0x80483f0 <backward+11>:        mov    %esi,%ecx

0x80483f2 <backward+13>:        mov    $0xc,%edx

0x80483f7 <backward+18>:        int    $0x80

0x80483f9 <backward+20>:        int3

End of assembler dump.





我们需要使用从man+3到backward+20之间的字节码，总共41字节。使用GDB中的x命令来察看机器码。



(gdb) x/40bx main+3

<main+3>: eb 15 5e b8 04 00 00 00

<backward+6>: bb 02 00 00 00 89 f1 ba

<backward+14>: 0c 00 00 00 cd 80 cc

<forward+1>: e6 ff ff ff 48 65 6c 6c

<forward+9>: 6f 20 57 6f 72 6c 64 0a



已经有了我们想要执行的指令，还等什么呢？只管把它们根前面那个例子一样插入到被调试程序中去！



代码：

int main(int argc, char *argv[])
{
pid_t traced_process;
    struct user_regs_struct regs, newregs;
    long ins;
    int len = 41;
    char insertcode[] =
        "/xeb/x15/x5e/xb8/x04/x00"
        "/x00/x00/xbb/x02/x00/x00/x00/x89/xf1/xba"
        "/x0c/x00/x00/x00/xcd/x80/xcc/xe8/xe6/xff"
        "/xff/xff/x48/x65/x6c/x6c/x6f/x20/x57/x6f"
        "/x72/x6c/x64/x0a/x00";
    char backup[len];
    if(argc != 2) {
        printf("Usage: %s <pid to be traced> ",
               argv[0], argv[1]);
        exit(1);
    }
    traced_process = atoi(argv[1]);
    ptrace(PTRACE_ATTACH, traced_process,
           NULL, NULL);
    wait(NULL);
    ptrace(PTRACE_GETREGS, traced_process,
           NULL, &regs);
    getdata(traced_process, regs.eip, backup, len);
    putdata(traced_process, regs.eip,
            insertcode, len);
    ptrace(PTRACE_SETREGS, traced_process,
           NULL, &regs);
    ptrace(PTRACE_CONT, traced_process,
           NULL, NULL);
    wait(NULL);
    printf("The process stopped, Putting back "
           "the original instructions ");
    putdata(traced_process, regs.eip, backup, len);
    ptrace(PTRACE_SETREGS, traced_process,
           NULL, &regs);
    printf("Letting it continue with "
           "original flow ");
    ptrace(PTRACE_DETACH, traced_process,
           NULL, NULL);
    return 0;
}

将代码插入到自由空间



在前面的例子中我们将代码直接插入到了正在执行的指令流中，然而，调试器可能会被这种行为弄糊涂，所以我们决定把指令插入到进程中的自由空间中去。通过察看/proc/pid/maps可以知道这个进程中自由空间的分布。接下来这个函数可以找到这个内存映射的起始点：

long freespaceaddr(pid_t pid)
{
    FILE *fp;
    char filename[30];
    char line[85];
    long addr;
    char str[20];
    sprintf(filename, "/proc/%d/maps", pid);
    fp = fopen(filename, "r");
    if(fp == NULL)
        exit(1);
    while(fgets(line, 85, fp) != NULL) {
        sscanf(line, "%lx-%*lx %*s %*s %s", &addr,
               str, str, str, str);
        if(strcmp(str, "00:00") == 0)
            break;
    }
    fclose(fp);
    return addr;
}

在/proc/pid/maps中的每一行都对应了进程中一段内存区域。主函数的代码如下：

int main(int argc, char *argv[])
{
    pid_t traced_process;
    struct user_regs_struct oldregs, regs;
    long ins;
    int len = 41;
    char insertcode[] =
        "/xeb/x15/x5e/xb8/x04/x00"
        "/x00/x00/xbb/x02/x00/x00/x00/x89/xf1/xba"
        "/x0c/x00/x00/x00/xcd/x80/xcc/xe8/xe6/xff"
        "/xff/xff/x48/x65/x6c/x6c/x6f/x20/x57/x6f"
        "/x72/x6c/x64/x0a/x00";
    char backup[len];
    long addr;
    if(argc != 2) {
        printf("Usage: %s <pid to be traced> ",
               argv[0], argv[1]);
        exit(1);
    }
    traced_process = atoi(argv[1]);
    ptrace(PTRACE_ATTACH, traced_process,
           NULL, NULL);
    wait(NULL);
    ptrace(PTRACE_GETREGS, traced_process,
           NULL, &regs);
    addr = freespaceaddr(traced_process);
    getdata(traced_process, addr, backup, len);
    putdata(traced_process, addr, insertcode, len);
    memcpy(&oldregs, &regs, sizeof(regs));
    regs.eip = addr;
    ptrace(PTRACE_SETREGS, traced_process,
           NULL, &regs);
    ptrace(PTRACE_CONT, traced_process,
           NULL, NULL);
    wait(NULL);
    printf("The process stopped, Putting back "
           "the original instructions ");
    putdata(traced_process, addr, backup, len);
    ptrace(PTRACE_SETREGS, traced_process,
           NULL, &oldregs);
    printf("Letting it continue with "
           "original flow ");
    ptrace(PTRACE_DETACH, traced_process,
           NULL, NULL);
    return 0;
}

ptrace的幕后工作

那么，在使用ptrace的时候，内核里发生了声么呢？这里有一段简要的说明：



当一个进程调用了 ptrace( PTRACE_TRACEME, …)之后，内核为该进程设置了一个标记，注明该进程将被跟踪。内核中的相关原代码如下：



Source: arch/i386/kernel/ptrace.c

if (request == PTRACE_TRACEME) {

    /* are we already being traced? */

    if (current->ptrace & PT_PTRACED)

        goto out;

    /* set the ptrace bit in the process flags. */

    current->ptrace |= PT_PTRACED;

    ret = 0;

    goto out;

}



一次系统调用完成之后，内核察看那个标记，然后执行trace系统调用（如果这个进程正处于被跟踪状态的话）。其汇编的细节可以在 arh/i386/kernel/entry.S中找到。



现在让我们来看看这个sys_trace()函数（位于 arch/i386/kernel/ptrace.c ）。它停止子进程，然后发送一个信号给父进程，告诉它子进程已经停滞，这个信号会激活正处于等待状态的父进程，让父进程进行相关处理。父进程在完成相关操作以后就调用ptrace( PTRACE_CONT, …)或者 ptrace( PTRACE_SYSCALL, …), 这将唤醒子进程，内核此时所作的是调用一个叫wake_up_process() 的进程调度函数。其他的一些系统架构可能会通过发送SIGCHLD给子进程来达到这个目的。



小结：

ptrace函数可能会让人们觉得很奇特，因为它居然可以检测和修改一个运行中的程序。这种技术主要是在调试器和系统调用跟踪程序中使用。它使程序员可以在用户级别做更多有意思的事情。已经有过很多在用户级别下扩展操作系统得尝试，比如UFO,一个用户级别的文件系统扩展，它使用ptrace来实现一些安全机制。





Ptrace 提供了一种父进程可以控制子进程运行，并可以检查和改变它的核心image。它主要用于实现断点调试。一个被跟踪的进程运行中，直到发生一个信号。则进程被中止，并且通知其父进程。在进程中止的状态下，进程的内存空间可以被读写。父进程还可以使子进程继续执行，并选择是否是否忽略引起中止的信号。

Request参数决定了系统调用的功能：

PTRACE_TRACEME

本进程被其父进程所跟踪。其父进程应该希望跟踪子进程。

PTRACE_PEEKTEXT, PTRACE_PEEKDATA

从内存地址中读取一个字节，内存地址由addr给出。

PTRACE_PEEKUSR

从USER区域中读取一个字节，偏移量为addr。

PTRACE_POKETEXT, PTRACE_POKEDATA

往内存地址中写入一个字节。内存地址由addr给出。

PTRACE_POKEUSR

往USER区域中写入一个字节。偏移量为addr。

PTRACE_SYSCALL, PTRACE_CONT

重新运行。

PTRACE_KILL

杀掉子进程，使它退出。

PTRACE_SINGLESTEP

设置单步执行标志

PTRACE_ATTACH

跟踪指定pid 进程。

PTRACE_DETACH

结束跟踪

Intel386特有：

PTRACE_GETREGS

读取寄存器

PTRACE_SETREGS

设置寄存器

PTRACE_GETFPREGS

读取浮点寄存器

PTRACE_SETFPREGS

设置浮点寄存器

init进程不可以使用此函数


返回值

成功返回0。错误返回-1。errno被设置。


错误

EPERM

特殊进程不可以被跟踪或进程已经被跟踪。

ESRCH

指定的进程不存在

EIO

请求非法
ptrace系统函数。 ptrace提供了一种使父进程得以监视和控制其它进程的方式，它还能够改变子进程中的寄存器和内核映像，因而可以实现断点调试和系统调用的跟踪。使用ptrace，你可以在用户层拦截和修改系统调用(sys call).
功能详细描述
1)   PTRACE_TRACEME

形式：ptrace(PTRACE_TRACEME,0 ,0 ,0)

描述：本进程被其父进程所跟踪。其父进程应该希望跟踪子进程。


2)  PTRACE_PEEKTEXT, PTRACE_PEEKDATA

形式：ptrace(PTRACE_PEEKTEXT, pid, addr, data)

         ptrace(PTRACE_PEEKDATA, pid, addr, data)

描述：从内存地址中读取一个字节，pid表示被跟踪的子进程，内存地址由addr给出，data为用户变量地址用于返回读到的数据。在Linux（i386）中用户代码段与用户数据段重合所以读取代码段和数据段数据处理是一样的。


3)  PTRACE_POKETEXT, PTRACE_POKEDATA

形式：ptrace(PTRACE_POKETEXT, pid, addr, data)

         ptrace(PTRACE_POKEDATA, pid, addr, data)

描述：往内存地址中写入一个字节。pid表示被跟踪的子进程，内存地址由addr给出，data为所要写入的数据。


4)  TRACE_PEEKUSR

形式：ptrace(PTRACE_PEEKUSR, pid, addr, data)

描述：从USER区域中读取一个字节，pid表示被跟踪的子进程，USER区域地址由addr给出，data为用户变量地址用于返回读到的数据。USER结构为core文件的前面一部分，它描述了进程中止时的一些状态，如：寄存器值，代码、数据段大小，代码、数据段开始地址等。在Linux（i386）中通过PTRACE_PEEKUSER和PTRACE_POKEUSR可以访问USER结构的数据有寄存器和调试寄存器。


5)  PTRACE_POKEUSR

形式：ptrace(PTRACE_POKEUSR, pid, addr, data)

描述：往USER区域中写入一个字节，pid表示被跟踪的子进程，USER区域地址由addr给出，data为需写入的数据。


6)   PTRACE_CONT

形式：ptrace(PTRACE_CONT, pid, 0, signal)

描述：继续执行。pid表示被跟踪的子进程，signal为0则忽略引起调试进程中止的信号，若不为0则继续处理信号signal。


7)  PTRACE_SYSCALL

形式：ptrace(PTRACE_SYS, pid, 0, signal)

描述：继续执行。pid表示被跟踪的子进程，signal为0则忽略引起调试进程中止的信号，若不为0则继续处理信号signal。与PTRACE_CONT不同的是进行系统调用跟踪。在被跟踪进程继续运行直到调用系统调用开始或结束时，被跟踪进程被中止，并通知父进程。


8)   PTRACE_KILL

形式：ptrace(PTRACE_KILL,pid)

描述：杀掉子进程，使它退出。pid表示被跟踪的子进程。


9)   PTRACE_SINGLESTEP

形式：ptrace(PTRACE_KILL, pid, 0, signle)

描述：设置单步执行标志，单步执行一条指令。pid表示被跟踪的子进程。signal为0则忽略引起调试进程中止的信号，若不为0则继续处理信号signal。当被跟踪进程单步执行完一个指令后，被跟踪进程被中止，并通知父进程。


10)  PTRACE_ATTACH

形式：ptrace(PTRACE_ATTACH,pid)

描述：跟踪指定pid 进程。pid表示被跟踪进程。被跟踪进程将成为当前进程的子进程，并进入中止状态。


11)  PTRACE_DETACH

形式：ptrace(PTRACE_DETACH,pid)

描述：结束跟踪。 pid表示被跟踪的子进程。结束跟踪后被跟踪进程将继续执行。


12)  PTRACE_GETREGS

形式：ptrace(PTRACE_GETREGS, pid, 0, data)

描述：读取寄存器值，pid表示被跟踪的子进程，data为用户变量地址用于返回读到的数据。此功能将读取所有17个基本寄存器的值。


13)  PTRACE_SETREGS

形式：ptrace(PTRACE_SETREGS, pid, 0, data)

描述：设置寄存器值，pid表示被跟踪的子进程，data为用户数据地址。此功能将设置所有17个基本寄存器的值。


14)  PTRACE_GETFPREGS

形式：ptrace(PTRACE_GETFPREGS, pid, 0, data)

描述：读取浮点寄存器值，pid表示被跟踪的子进程，data为用户变量地址用于返回读到的数据。此功能将读取所有浮点协处理器387的所有寄存器的值。


15)  PTRACE_SETFPREGS

形式：ptrace(PTRACE_SETREGS, pid, 0, data)

描述：设置浮点寄存器值，pid表示被跟踪的子进程，data为用户数据地址。此功能将设置所有浮点协处理器387的所有寄存器的值。

 vim:tw=78:ts=8:ft=man:norl:
