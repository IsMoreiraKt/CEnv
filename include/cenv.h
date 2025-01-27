/**
 * @file cenv.h
 * @brief Library for loading environment variables from `.env` files.
 *
 * This project is part of a free software library licensed under the GNU Lesser
 * General Public License (LGPL).
 *
 * Copyright (C) 2025 Ismael Moreira <ismaelmoreirakt@gmail.com>
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, see <https://www.gnu.org/licenses/>.
 */
#ifndef CENV_H
#define CENV_H

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
  env_var *vars; ///< Dynamic array of environment variables.
  int var_count; ///< Number of currently loaded variables.
  int capacity;  ///< Capacity of the dynamic array.
} dotenv_context;

/// Internal context to manage the loaded variables (hidden from the user).
static dotenv_context ctx = {NULL, 0, 0};

/**
 * @brief Initializes the internal dotenv context.
 *
 * Ensures the internal context is ready for use.
 *
 * @param initial_capacity Initial capacity for the environment variable array.
 * @return 0 on success, -1 if memory allocation fails.
 */
static int dotenv_init(int initial_capacity) {
  if (ctx.vars == NULL) { // Only initialize if not already done
    ctx.vars = malloc(sizeof(env_var) * initial_capacity);

    if (!ctx.vars) {
      perror("Failed to allocate memory for environment variables.");
      return -1;
    }

    ctx.var_count = 0;
    ctx.capacity = initial_capacity;
  }

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
  int new_capacity = ctx.capacity * 2;
  env_var *new_vars = realloc(ctx.vars, sizeof(env_var) * new_capacity);

  if (!new_vars) {
    perror("Failed to resize environment variable array.");
    return -1;
  }

  ctx.vars = new_vars;
  ctx.capacity = new_capacity;

  return 0;
}

/// Maximum length of a line in the .env file.
#define MAX_LINE_LENGTH 1024

/**
 * @brief Loads environment variables from a `.env` file.
 *
 * Reads a `.env` file line by line, ignoring comments and empty lines,
 * and stores the variables as key-value pairs.
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
    // Skip comments and empty lines
    if (line[0] == '#' || line[0] == '\n')
      continue;

    // Remove trailing newline
    char *newline_pos = strchr(line, '\n');
    if (newline_pos)
      *newline_pos = '\0';

    // Split key and value
    char *delimiter = strchr(line, '=');
    if (!delimiter)
      continue;

    *delimiter = '\0';
    char *key = line;
    char *value = delimiter + 1;

    // Resize if capacity is exceeded
    if (ctx.var_count >= ctx.capacity) {
      if (dotenv_resize() == -1) {
        fclose(file);
        return -1;
      }
    }

    // Store the key-value pair
    ctx.vars[ctx.var_count].key = strdup(key);
    ctx.vars[ctx.var_count].value = strdup(value);

    if (!ctx.vars[ctx.var_count].key || !ctx.vars[ctx.var_count].value) {
      perror("Failed to allocate memory for key or value.");
      fclose(file);
      return -1;
    }

    ctx.var_count++;
  }

  fclose(file);
  return 0;
}

#endif // CENV_H