cmake_minimum_required(VERSION 4.2)
project(contacts_app C)

set(CMAKE_C_STANDARD 99)

add_executable(contacts_app
        include/contact_types.h
        src/model.c
        src/view.c
        src/controller.c
        src/cJSON.c
        include/model.h
        include/controller.h
        include/cJSON.h
        include/nuklear.h)
