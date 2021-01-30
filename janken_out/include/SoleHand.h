#pragma once

#include "main.h"

/**
 * @brief ソレノイドハンド
 */
class SoleHand {
public:
    enum Finger {
        Thumb,  // 親指
        Index,  // 人差し指
        Middle, // 中指
        Ring,   // 薬指
        Little, // 小指
        
        End,
    };

    SoleHand(uint32_t thumb, uint32_t index, uint32_t middle, uint32_t ring, uint32_t little);

    /// @brief 初期化
    void resetPose();
        
    /// @brief ポーズ出力
    void setPose(JANKEN_POSE pose);

private:
    static const std::array<std::array<uint32_t, Finger::End>, JANKEN_POSE::POSE_NONE> Poses;

    std::array<uint32_t, Finger::End> _solenoids;
};
