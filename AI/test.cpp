//
//  main.cpp
//  study-ai
//
//  Created by Victor Xiong on 15/7/16.
//  Copyright (c) 2015年 Victor Xiong. All rights reserved.
//
#define TEST_WITH_MAIN_FOR_GEP_HPP 1
#define main GEP_main
#include "GEP.hpp"
#undef main

#define TEST_WITH_MAIN_FOR_ANN_HPP 1
#define main ANN_main
#include "ANN.hpp"
#undef main

#define TEST_WITH_MAIN_FOR_FSM_HPP 1
#define main FSM_main
#include "FSM.hpp"
#undef main

#define TEST_WITH_MAIN_FOR_AST_HPP 1
#define main AST_main
#include "AST.hpp"
#undef main

#define TEST_WITH_MAIN_FOR_TSP_HPP 1
#define main TSP_main
#include "TSP.hpp"
#undef main

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
#if TEST_WITH_MAIN_FOR_GEP_HPP
    GEP_main(argc, argv);
#endif//TEST_WITH_MAIN_FOR_GEP_HPP
    
#if TEST_WITH_MAIN_FOR_ANN_HPP
    ANN_main(argc, argv);
#endif//TEST_WITH_MAIN_FOR_ANN_HPP
    
#if TEST_WITH_MAIN_FOR_FSM_HPP
    FSM_main(argc, argv);
#endif//TEST_WITH_MAIN_FOR_FSM_HPP
    
#if TEST_WITH_MAIN_FOR_AST_HPP
    AST_main(argc, argv);
#endif//TEST_WITH_MAIN_FOR_AST_HPP
    
#if TEST_WITH_MAIN_FOR_TSP_HPP
    TSP_main(argc, argv);
#endif//TEST_WITH_MAIN_FOR_TSP_HPP
    
    return 0;
}
