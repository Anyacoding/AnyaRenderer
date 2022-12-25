//
// Created by Anya on 2022/12/25.
//

#ifndef ANYA_ENGINE_LIGHT_HPP
#define ANYA_ENGINE_LIGHT_HPP

namespace anya {

// 光源类
struct Light {
public:
    Vector3 position;
    Vector3 intensity;
};

}

#endif //ANYA_ENGINE_LIGHT_HPP
