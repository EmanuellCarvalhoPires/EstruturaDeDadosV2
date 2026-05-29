cmake_minimum_required(VERSION 4.2)
project(contacts_app C)

set(CMAKE_C_STANDARD 99)

add_executable(contacts_app
        src/include/contact_types.h
        src/model.c
        src/include/model.h
)
