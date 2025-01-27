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

#endif // CENV_H