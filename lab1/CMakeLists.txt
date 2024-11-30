cmake_minimum_required(VERSION 3.10)
project(lr11 C)

# Устанавливаем стандарт языка C
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Компилируем предупреждения как ошибки для неинициализированных переменных
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=maybe-uninitialized")

# Создаем исполняемый файл для родительского процесса
add_executable(CreateProcess_exe CreateProcess.c)

# Создаем исполняемый файл для дочернего процесса
add_executable(ChildProcess_exe ChildProcess.c)