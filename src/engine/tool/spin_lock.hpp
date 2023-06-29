//
// Created by Anya on 2023/6/29.
//

#ifndef ANYA_RENDERER_SPIN_LOCK_HPP
#define ANYA_RENDERER_SPIN_LOCK_HPP

#include <atomic>

namespace anya {

class spin_lock {
private:
    std::atomic_flag flag;

public:
    spin_lock() : flag(ATOMIC_FLAG_INIT) {}

public:
    // 尝试获取自旋锁，直到返回的旧值为false时，说明当前线程已经成功获取锁，将flag设置为true了
    // test_and_set 先测试旧值是否是false，是的话则设置为true并返回旧值false，否则将一直返回true
    void
    lock() { while(flag.test_and_set(std::memory_order_acquire)); }

    // 放弃自旋锁，当前线程把flag设置为false
    void
    unlock() { flag.clear(std::memory_order_acquire); }
};

}

#endif //ANYA_RENDERER_SPIN_LOCK_HPP
