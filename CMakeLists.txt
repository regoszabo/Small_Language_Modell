cmake_minimum_required(VERSION 3.14)
project(Word_Predictor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Szigorú fordítási flagek a hibamentes kódért
add_compile_options(-Wall -Wextra -Werror -pedantic)

# A header fájlok mappája
include_directories(include)

# Célfájl (executable) létrehozása a CSAK .cpp forrásfájlokkal
add_executable(Word_Predictor
        src/main.cpp
        src/CoOccurrenceMatrix.cpp
        src/PPMIMatrix.cpp
)