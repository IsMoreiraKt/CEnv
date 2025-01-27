#ifndef CENV_H
#define CENV_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define ENV_NEWLINE "\r\n" ///< Windows newline
#else
#define ENV_NEWLINE "\n" ///< Linux/macOS newline
#endif

/**
 * @struct env_var
 * @brief Structure representing an environment variable.
 *
 * Contains a key-value pair for an environment variable.
 */
typedef struct {
  char *key;   ///< The key of the environment variable.
  char *value; ///< The value associated with the key.
} env_var;

/**
 * @struct dotenv_context
 * @brief Internal structure to manage environment variables.
 *
 * Holds the loaded variables, their count, and the allocated capacity.
 */
typedef struct {
  env_var *vars;         ///< Dynamic array of environment variables.
  int var_count;         ///< Number of currently loaded variables.
  int capacity;          ///< Capacity of the dynamic array.
  pthread_mutex_t mutex; ///< Mutex to ensure thread-safe access.
} dotenv_context;

/// Internal context to manage the loaded variables (hidden from the user).
static dotenv_context ctx = {NULL, 0, 0, PTHREAD_MUTEX_INITIALIZER};

/**
 * @brief Removes leading and trailing whitespace from a string.
 *
 * Modifies the string in place to remove any spaces, tabs, or newline
 * characters from the beginning and the end.
 *
 * @param str The string to trim.
 * @return A pointer to the trimmed string.
 */
static char *trim_whitespace(char *str) {
  char *end;

  // Trim leading spaces
  while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') {
    str++;
  }

  // If the string is now empty, return it
  if (*str == '\0') {
    return str;
  }

  // Trim trailing spaces
  end = str + strlen(str) - 1;

  while (end > str &&
         (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
    end--;
  }

  // Null-terminate the string
  *(end + 1) = '\0';

  return str;
}

/**
 * @brief Initializes the internal dotenv context.
 *
 * Ensures the internal context is ready for use.
 *
 * @param initial_capacity Initial capacity for the environment variable array.
 * @return 0 on success, -1 if memory allocation fails.
 */
static int dotenv_init(int initial_capacity) {
  pthread_mutex_lock(&ctx.mutex);

  if (ctx.vars == NULL) { // Only initialize if not already done
    ctx.vars = malloc(sizeof(env_var) * initial_capacity);

    if (!ctx.vars) {
      perror("Failed to allocate memory for environment variables.");
      pthread_mutex_unlock(&ctx.mutex);
      return -1;
    }

    ctx.var_count = 0;
    ctx.capacity = initial_capacity;
  }

  pthread_mutex_unlock(&ctx.mutex);
  return 0;
}

/**
 * @brief Resizes the internal array of environment variables.
 *
 * Doubles the capacity to accommodate more variables when needed.
 *
 * @return 0 on success, -1 if memory allocation fails.
 */
static int dotenv_resize() {
  pthread_mutex_lock(&ctx.mutex);

  int new_capacity = ctx.capacity * 2;
  env_var *new_vars = realloc(ctx.vars, sizeof(env_var) * new_capacity);

  if (!new_vars) {
    perror("Failed to resize environment variable array.");
    pthread_mutex_unlock(&ctx.mutex);
    return -1;
  }

  ctx.vars = new_vars;
  ctx.capacity = new_capacity;

  pthread_mutex_unlock(&ctx.mutex);
  return 0;
}

/// Maximum length of a line in the .env file.
#define MAX_LINE_LENGTH 1024

/**
 * @brief Loads environment variables from a `.env` file using a stream
 * approach.
 *
 * Processes the file line by line, storing key-value pairs without loading the
 * entire file into memory.
 *
 * @param filename Path to the `.env` file.
 * @return 0 if the file is successfully loaded, -1 if the file cannot be
 * opened.
 */
int dotenv_load(const char *filename) {
  if (dotenv_init(10) == -1)
    return -1;

  FILE *file = fopen(filename, "r");

  if (!file) {
    perror("Failed to open .env file.");
    return -1;
  }

  char line[MAX_LINE_LENGTH];

  while (fgets(line, sizeof(line), file)) {
    char *newline_pos = strstr(line, ENV_NEWLINE);

    if (newline_pos) {
      *newline_pos = '\0';
    } else {
      char *single_newline = strchr(line, '\n');

      if (single_newline) {
        *single_newline = '\0';
      }
    }

    if (line[0] == '#' || line[0] == '\0')
      continue;

    char *delimiter = strchr(line, '=');
    if (!delimiter)
      continue;

    *delimiter = '\0';
    char *key = trim_whitespace(line);
    char *value = trim_whitespace(delimiter + 1);

    if (key[0] == '\0')
      continue;

    pthread_mutex_lock(&ctx.mutex);

    if (ctx.var_count >= ctx.capacity) {
      if (dotenv_resize() == -1) {
        pthread_mutex_unlock(&ctx.mutex);
        fclose(file);
        return -1;
      }
    }

    ctx.vars[ctx.var_count].key = strdup(key);
    ctx.vars[ctx.var_count].value = strdup(value);

    if (!ctx.vars[ctx.var_count].key || !ctx.vars[ctx.var_count].value) {
      perror("Failed to allocate memory for key or value.");
      pthread_mutex_unlock(&ctx.mutex);
      fclose(file);
      return -1;
    }

    ctx.var_count++;
    pthread_mutex_unlock(&ctx.mutex);
  }

  fclose(file);
  return 0;
}

/**
 * @brief Retrieves the value associated with a specific key.
 *
 * Searches for the value of a key previously loaded from the `.env` file.
 *
 * @param key The key of the variable to search for.
 * @return The value associated with the key, or `NULL` if the key is not found.
 */
const char *dotenv_get(const char *key) {
  pthread_mutex_lock(&ctx.mutex);

  for (int i = 0; i < ctx.var_count; i++) {
    if (strcmp(ctx.vars[i].key, key) == 0) {
      pthread_mutex_unlock(&ctx.mutex);
      return ctx.vars[i].value;
    }
  }

  pthread_mutex_unlock(&ctx.mutex);
  return NULL;
}

/**
 * @brief Frees the memory allocated for loaded environment variables.
 *
 * Releases the memory allocated for keys, values, and the internal context.
 */
void dotenv_free() {
  pthread_mutex_lock(&ctx.mutex);

  if (ctx.vars) {
    for (int i = 0; i < ctx.var_count; i++) {
      free(ctx.vars[i].key);
      free(ctx.vars[i].value);
    }

    free(ctx.vars);

    ctx.vars = NULL;
    ctx.var_count = 0;
    ctx.capacity = 0;
  }

  pthread_mutex_unlock(&ctx.mutex);
}

#endif // CENV_H