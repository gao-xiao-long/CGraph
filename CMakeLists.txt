
message("* * * * * * * * * * * * * * * * * * * * * * * * * * * * *")
message("*     ______   ______                             __    *")
message("*    / ____/  / ____/   _____  ____ _    ____    / /_   *")
message("*   / /      / / __    / ___/ / __ `/   / __ \\  / __ \\  *")
message("*  / /___   / /_/ /   / /    / /_/ /   / /_/ / / / / /  *")
message("*  \\____/   \\____/   /_/     \\__,_/   / .___/ /_/ /_/   *")
message("*                                    /_/ by Chunel Feng *")
message("* * * * * * * * * * * * * * * * * * * * * * * * * * * * *")

cmake_minimum_required(VERSION 3.2.5)

project(CGraph VERSION 2.5.0)

# CGraph默认使用C++11版本，推荐使用C++17版本。暂不支持C++11以下版本
set(CMAKE_CXX_STANDARD 11)

# 如果开启此宏定义，则CGraph执行过程中，不会在控制台打印任何信息
# add_definitions(-D_CGRAPH_SILENCE_)

# 对应tutorial中的内容
set(CGRAPH_TUTORIAL_LIST
        T00-HelloCGraph
        T01-Simple
        T02-Cluster
        T03-Region
        T04-Complex
        T05-Param
        T06-Condition
        T07-MultiPipeline
        T08-Template
        T09-Aspect
        T10-AspectParam
        T11-Singleton
        T12-Function
        T13-Daemon
        T14-Hold
        T15-ElementParam
        T16-MessageSendRecv
        T17-MessagePubSub
        T18-Event
        T19-Cancel
        T20-YieldResume
        T21-MultiCondition
        T22-Some
        T23-Timeout

        # 以下为工具类tutorial
        TU01-ThreadPool
        # TU02-Lru
        # TU03-Trie
        # TU04-Timer
        # TU05-Distance
        )

# add CGraph environment info
include(cmake/CGraph-env-include.cmake)

foreach(tut ${CGRAPH_TUTORIAL_LIST})
            add_executable(${tut}
            # 在自己的工程中引入CGraph功能，仅需引入 CGraph-env-include.cmake 后，加入这一句话即可
            $<TARGET_OBJECTS:CGraph>
            tutorial/${tut}.cpp
            )
endforeach()
