//
// Created by zhsssy
//

#ifndef COMPILER_PRINCIPLE_EXPERIMENT_EXCEPTION_H
#define COMPILER_PRINCIPLE_EXPERIMENT_EXCEPTION_H

#include <exception>

class ReduceException : std::exception {
private:
public:
    ReduceException() = default;

    ~ReduceException() override = default;
};

#endif //COMPILER_PRINCIPLE_EXPERIMENT_EXCEPTION_H
