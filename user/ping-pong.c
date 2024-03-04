#include "kernel/types.h"
#include "user/user.h"

void
panic(char *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

int
fork1(void)
{
  int pid;

  pid = fork();
  if(pid == -1)
    panic("fork");
  return pid;
}


int
main() {
  int parent_pipe[2] = {0};
  int child_pipe[2] = {0};
  pipe(parent_pipe);
  pipe(child_pipe);

  if (fork1() == 0)  {
    // child
    close(child_pipe[0]);

    char byte;
    while (1) {
      if (read(parent_pipe[0], &byte, 1) < 1) {
        fprintf(1, "error in child: can't read from parent\n");
        exit(1);
      }

      if (write(child_pipe[1], &byte, 1) < 1) {
        fprintf(1, "error in child: can't write to parent\n");
        exit(1);
      }
    }
  } else {
    // parent
    close(parent_pipe[0]);

    char byte = 22;
    time_t start = time();
    int count = 0;
    while (1) {
      if (write(parent_pipe[1], &byte, 1) < 1) {
        fprintf(1, "error in child: can't write to parent\n");
        exit(1);
      }

      if (read(child_pipe[0], &byte, 1) < 1) {
        fprintf(1, "error in child: can't read from parent\n");
        exit(1);
      }

      count++;
      time_t now = time();
      if ((now - start) > 1000000000L) {
        printf("sent %d times\n", count);
        count = 0;
        start = now;
      }
    }
  }
}

