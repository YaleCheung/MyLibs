/*
 * =====================================================================================
 *
 *       Filename:  threadPool_test.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/12/16 10:24:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "../threadPool.h"
#include <vector>
#include <future>
#include <iostream>
using std::cout;
using std::endl;
enum {thread_num = 8};

int main(int argc, char* argv[]) {
    ThreadPool thread_pool(3);
    std::vector<std::future<int>> results;
    
    for(int i = 0; i < thread_num; ++ i) {
        results.emplace_back(thread_pool.enqueue([i]{
            cout << "hellworld " << i << " ";
            return i * i;
                }));
    }
    for(auto& ret : results) {
        cout << ret.get() << endl;
    }
    return 0;
}
