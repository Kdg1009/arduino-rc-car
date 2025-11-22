#ifndef BASIC_MANAGER_H
#define BASIC_MANAGER_H

class BasicManager {
  public:
    virtual void init() = 0;
    virtual void update(unsigned long now) = 0;

    bool initialized = false;

    virtual ~ BasicManager() {}
};

#endif