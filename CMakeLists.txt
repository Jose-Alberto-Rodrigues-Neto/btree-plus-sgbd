cmake_minimum_required(VERSION 3.10)
project(BPlusIndex)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Adiciona os arquivos-fonte do projeto
add_executable(BPlusIndex
    main.cpp
    BPlusTree.cpp
    StorageManager.cpp
)

# Inclui os diretórios de headers
target_include_directories(BPlusIndex PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
