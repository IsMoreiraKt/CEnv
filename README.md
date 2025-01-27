# CEnv

<img 
    src="./assets/logo.png" 
    alt="logo"
    align="left"
/>

CEnv is a C library that makes it easy to load environment variables from .env files and resolve variables within other values. It is ideal for configuring applications dynamically and securely, especially in development environments.

<br>

## Installing CEnv
To install CEnv, clone the repository and use the Makefile to copy the header file to the includes directory.

1. Clone the repository:
```bash
git clone https://github.com/IsMoreiraKt/CEnv
```

2. Go to the project directory:
```bash
cd CEnv
```

3. Run make to install the library:
```bash
sudo make install
```

This will copy the cenv.h file to the system's includes directory (by default, /usr/local/include on Unix Like and C:\Program Files\cenv\include on Windows).

## How to Use
### Loading Environment Variables
The library offers a simple function for loading environment variables from an .env file.

```c
#include <cenv.h>

if (dotenv_load(".env") == -1) {
  fprintf(stderr, "Error when loading .env file\n");
  return 1;
}
```

### Accessing Environment Variables
After loading the variables, you can access the value of any environment variable using the corresponding key.

```c
const char *value = dotenv_get(‘VARIABLE_NAME’);

if (value) {
    printf("Variable value: %s\n", value);
} else {
    printf("Variable not found.\n");
}
```

### .env file
Your .env file must contain variables in the format key=value. Variables can also contain placeholders in the format ${VARIABLE}, which are automatically resolved:

```dotenv
DB_HOST=localhost
DB_PORT=5432
DB_URL=jdbc://${DB_HOST}:${DB_PORT}
```

### Cleaning up resources
After using the library, it is important to free the memory allocated for the loaded variables:

```c
dotenv_free();
```

### Complete Example
Here is an example of how you can use the library in a simple programme:

```c
#include <stdio.h>
#include <cenv.h>

int main() {
	if (dotenv_load(".env") == -1) {
		fprintf(stderr, "Error when loading .env file\n");
		return 1;
	}

  const char *db_host = dotenv_get("DB_HOST");
  
	if (db_host) {
    printf("DB_HOST: %s\n", db_host);
  } else {
    printf("DB_HOST variable not found.\n");
  }

  dotenv_free();

  return 0;
}
```

## How to Compile
Once installed, simply add the include directory to your compiler's flags, and you'll be ready to compile:

```bash
gcc -I/usr/local/include -o program main.c
```

## Licence
This project is licensed under the LGPL-2.1 license. See the [LICENSE](./LICENSE) file for more details.