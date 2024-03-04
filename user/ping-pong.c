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

char* str_reverse(const char *s, int len) {
  char *rv = malloc(len + 1);
  if (rv == 0) {
    fprintf(1, "Couldn't allocate\n");
    return 0;
  }
  for (int i = 0; i < len; i++) {
    rv[i] = s[len-i-1];
  }
  rv[len] = 0;
  return rv;
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
      char digits[100];
      if ((now - start) > 1000000000L) {
        printf("sent %d times, now: ", count);
        start = now;
        count = 0;

        // When printing uint64, neither %llu nor PRIu64 worked.
        // So, manually converting to a string, then print here
        int i = 0;
        while (now > 0ULL) {
          digits[i++] = (now % 10) + '0';
          now /= 10;
        }
        digits[i] = 0;
        const char *timestamp = str_reverse(digits, i);
        if (timestamp != 0) {
          printf("%s ns\n", timestamp);
        }
      }
    }
  }
}


