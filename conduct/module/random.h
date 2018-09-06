#ifndef RANDOM_H
#define RANDOM_H

class Random{
public:
    const static int MAXIMUM;
    static bool percent(double);
    static int pick(int, int);

private:
    static bool setSeed;
};


#endif // RANDOM_H
