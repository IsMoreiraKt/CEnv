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

#endif // CENV_H