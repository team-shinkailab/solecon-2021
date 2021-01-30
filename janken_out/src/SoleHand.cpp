#include "SoleHand.h"

SoleHand::SoleHand(uint32_t thumb, uint32_t index, uint32_t middle, uint32_t ring, uint32_t little) :
    _solenoids({{ thumb, index, middle, ring, little }})
{
}

/// @brief 初期化
void SoleHand::resetPose(){
    for(auto&& sole : _solenoids){
        digitalWrite(sole, 0);
    }
}
    
/// @brief ポーズ出力
void SoleHand::setPose(JANKEN_POSE pose) {
    if(pose == JANKEN_POSE::POSE_INVALID || pose == JANKEN_POSE::POSE_NONE){
        return;
    }
    
    for(size_t i = 0; i < _solenoids.size(); i++){
        digitalWrite(_solenoids[i], Poses[pose][i]);
    }
}

const std::array<std::array<uint32_t, SoleHand::Finger::End>, JANKEN_POSE::POSE_NONE> SoleHand::Poses = { {
    { 0, 0, 0, 0, 0 },
    { 0, 1, 1, 0, 0 },
    { 1, 1, 1, 1, 1 },
    { 0, 0, 0, 0, 0 },
} };
