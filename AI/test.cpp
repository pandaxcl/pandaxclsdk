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

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    GEP_main(argc, argv);
    ANN_main(argc, argv);
    return 0;
}
