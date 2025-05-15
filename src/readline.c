#include <readline/history.h>
#include <readline/readline.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prim_fn.h"

#ifndef READLIN_HISTORY_MAX
#define READLIN_HISTORY_MAX 100
#endif

char *prim_fn_completion_generator(const char *text, int state) {
  const char *word = is_in_gperf_keyword_table(text, state);

  if (word) {
    return strdup(word);
  }

  return NULL;
}

static const char *get_history_path(void) {
  const char *home = getenv("HOME");
  static char path[PATH_MAX];
  snprintf(path, sizeof(path), "%s/.myrepl_history", home ? home : ".");
  return path;
}

static char **_attempted_completion_function(const char *text, int start,
                                             int end) {
  (void)start;
  (void)end;
  rl_attempted_completion_over = 1;
  return rl_completion_matches(text, prim_fn_completion_generator);
}

static void rl_cleanup(void) {
  const char *hist_path = get_history_path();
  write_history(hist_path);
}

void rl_init(void) {
  rl_attempted_completion_function = _attempted_completion_function;
  const char *hist_path            = get_history_path();
  read_history(hist_path);
  stifle_history(READLIN_HISTORY_MAX);
  rl_variable_bind("blink-matching-paren", "on");
  atexit(rl_cleanup);
}

int rl_readline(char *full_input, size_t n) {
  size_t len = 0;
  char *line = NULL;

  full_input[0] = '\0';

  do {
    if (line) {
      free(line);
      line = NULL;
    }

    line = readline(len == 0 ? "clsr> " : "... ");

    if (!line)
      return -1;

    size_t line_len = strlen(line);

    if (len + line_len + 2 >= n) {
      free(line);
      return -1;
    }

    if (len > 0) {
      full_input[len++] = '\n';
    }

    strcpy(&full_input[len], line);
    len += line_len;
  } while (len == 0 || full_input[len - 1] != '\n');

  if (full_input[0]) {
    add_history(full_input);
  }

  free(line);
  return len;
}
