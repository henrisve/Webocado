#ifndef MYRAND_H
#define MYRAND_H

#include <random>

extern std::mt19937 engine;
extern std::normal_distribution<double> distribution;

class myrand{
public:
    double value;
    myrand(){
        value = distribution(engine);
    }

};

#endif // MYRAND_H
