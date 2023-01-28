//
// Created by Anya on 2023/1/12.
//

#ifndef ANYA_RENDERER_PROGRESS_HPP
#define ANYA_RENDERER_PROGRESS_HPP

#include "tool/vec.hpp"

namespace anya {

class Progress {
private:
    int barWidth = 30;
public:
    void
    update(numberType progress) const {
        std::cout << "[";
        int pos = static_cast<int>(barWidth * progress);
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos)
                std::cout << "=";
            else if (i == pos)
                std::cout << ">";
            else
                std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(progress * 100.0) << " %" << '\r';
        std::cout.flush();
    }
};

}

#endif //ANYA_RENDERER_PROGRESS_HPP
