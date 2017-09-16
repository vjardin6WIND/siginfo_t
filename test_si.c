/*
 * SIGCHLD tutorial using sigaction with siginfo_t
 *
 * cat test.sh
 *  echo ">>>foo"
 *  echo PID=$$
 *  echo "<<<bar"
 *  exit 7
 *
 * gcc -Wall -o test_si test_si.c
 * ./test_si &
 */

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

static void
handler(int sig, siginfo_t *si, void *uap)
{
  printf("signal %d received with siginfo_t:\n", sig);
  printf("\tsignal number: si_signo=%d\n", si->si_signo);
  printf("\terror number: si_errno=%d\n", si->si_errno);
  printf("\tsignal code: si_code=%d\n", si->si_code);
  printf("\tsignal value union: si_value, sival_int=%d\n", si->si_value.sival_int);
  printf("\tsignal value union: si_value, sival_ptr=%p\n", si->si_value.sival_ptr);
  printf("\tsending process ID: si_pid=%d\n", si->si_pid);
  printf("\tsending process user's ID: si_uid=%d\n", si->si_uid);
  printf("\tfaulting instruction at: si_addr=%p\n", si->si_addr);
  printf("\texit value or signal: si_status=%d\n", si->si_status);
  printf("\tband event for SIGPOLL: si_band=%ld\n", si->si_band);
  printf("\ttimer ID: si_timerid=%d\n", si->si_timerid);
  printf("\toverrun count: si_overrun=%d\n", si->si_overrun);
  printf("\n");

  waitpid(si->si_pid, NULL, 0);
}

int
main(void)
{
  struct sigaction sa;
  int pid;

  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = handler;
  sa.sa_flags = SA_SIGINFO | SA_RESTART | SA_NOCLDSTOP;

  sigaction(SIGCHLD, &sa, NULL);

restart:

  switch(pid = fork()) {
    case -1: return -1; // error
    case 0: /* children */
       sleep(2);
       return execlp("./test.sh", "nothing",NULL);
     default: /* parent */
       sleep(10);
       printf("test\n");
       goto restart;
       return 0;
  }

  return 0;
}
