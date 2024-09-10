#pragma once

#include "go.h"
#include <iostream>
#include <string>
#include <utility>

namespace minizero::env::atarigo {

const std::string kAtariGoName = "atarigo";
const int kAtariGoNumPlayer = 2;

typedef go::GoAction AtariGoAction;

class AtariGoEnv : public go::GoEnv {
public:
    AtariGoEnv() : go::GoEnv() {}

    void reset() override
    {
        GoEnv::reset();

        num_passes_.get(Player::kPlayer1) = 0;
        num_passes_.get(Player::kPlayer2) = 0;

        // int center_block_id = -1;
        // if (board_size_ % 2 == 0) {
        //     center_block_id = ((board_size_ + 1) * ((board_size_ / 2) - 1));
        // } else {
        //     center_block_id = (((board_size_ * board_size_) - 1) / 2);
        // }

        // act(AtariGoAction(center_block_id, Player::kPlayer1));
        // act(AtariGoAction(center_block_id + 1, Player::kPlayer2));
        // act(AtariGoAction(center_block_id + board_size_ + 1, Player::kPlayer1));
        // act(AtariGoAction(center_block_id + board_size_, Player::kPlayer2));
    }

    bool act(const AtariGoAction& action) override
    {
        bool result = GoEnv::act(action);
        if (result && isPassAction(action)) {
            (action.getPlayer() == Player::kPlayer1) ? num_passes_.get(Player::kPlayer1)++ : num_passes_.get(Player::kPlayer2)++;
        }

        return result;
    }

    bool isTerminal() const override
    {
        if (isEatStone()) { return true; }

        return go::GoEnv::isTerminal();
    }

    float getEvalScore(bool is_resign = false) const override
    {
        if (!isEatStone() && !is_resign) { return 0.0f; }

        if (turn_ == Player::kPlayer1)
            return -1.0f; // white win
        else if (turn_ == Player::kPlayer2)
            return 1.0f; // black win

        return 0.0f;
    }

    inline std::string name() const override { return kAtariGoName + "_" + std::to_string(board_size_) + "x" + std::to_string(board_size_); }
    inline int getNumPlayer() const override { return kAtariGoNumPlayer; }

private:
    GamePair<int> num_passes_;

    // std::pair<int, int> getNumPasses() const
    // {
    //     int num_black_pass = 0;
    //     int num_white_pass = 0;
    //     for (const auto& action : actions_) {
    //         if (!isPassAction(action)) { continue; }
    //         if (action.getPlayer() == Player::kPlayer1) {
    //             num_black_pass++;
    //         } else if (action.getPlayer() == Player::kPlayer2) {
    //             num_white_pass++;
    //         }
    //     }
    //     return {num_black_pass, num_white_pass};
    // }

    bool isEatStone() const
    {
        // auto [num_black_pass, num_white_pass] = getNumPasses();
        std::size_t black_count = getStoneBitboard().get(Player::kPlayer1).count() + num_passes_.get(Player::kPlayer1);
        std::size_t white_count = getStoneBitboard().get(Player::kPlayer2).count() + num_passes_.get(Player::kPlayer2);
        if (turn_ == Player::kPlayer1 && black_count != white_count) { return true; }
        if (turn_ == Player::kPlayer2 && black_count != (white_count + 1)) { return true; }

        return false;
    }
};

class AtariGoEnvLoader : public go::GoEnvLoader {
public:
    inline std::string name() const override { return kAtariGoName + "_" + std::to_string(getBoardSize()) + "x" + std::to_string(getBoardSize()); }
};

} // namespace minizero::env::atarigo
