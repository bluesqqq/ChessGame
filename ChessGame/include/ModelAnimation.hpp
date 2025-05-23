#ifndef RAYLIB_CPP_INCLUDE_MODELANIMATION_HPP_
#define RAYLIB_CPP_INCLUDE_MODELANIMATION_HPP_

#include <string>
#include <vector>

#include "./Mesh.hpp"
#include "./raylib-cpp-utils.hpp"
#include "./raylib.hpp"

namespace raylib {
/**
 * Model animation
 */
class ModelAnimation : public ::ModelAnimation {
public:
    ModelAnimation(const ::ModelAnimation& model) { set(model); }

    ModelAnimation(const ModelAnimation&) = delete;

    ModelAnimation(ModelAnimation&& other) noexcept {
        set(other);

        other.boneCount = 0;
        other.frameCount = 0;
        other.bones = nullptr;
        other.framePoses = nullptr;
    }

    ~ModelAnimation() { Unload(); }

    /**
     * Load model animations from file
     */
    static std::vector<ModelAnimation> Load(const std::string& fileName) {
        int count = 0;
        ::ModelAnimation* modelAnimations = ::LoadModelAnimations(fileName.c_str(), &count);
        std::vector<ModelAnimation> mats(modelAnimations, modelAnimations + count);

        RL_FREE(modelAnimations);

        return mats;
    }

    GETTERSETTER(int, BoneCount, boneCount)
    GETTERSETTER(::BoneInfo*, Bones, bones)
    GETTERSETTER(int, FrameCount, frameCount)
    GETTERSETTER(::Transform**, FramePoses, framePoses)

    ModelAnimation& operator=(const ::ModelAnimation& model) {
        set(model);
        return *this;
    }

    ModelAnimation& operator=(const ModelAnimation&) = delete;

    ModelAnimation& operator=(ModelAnimation&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        Unload();
        set(other);

        other.boneCount = 0;
        other.frameCount = 0;
        other.bones = nullptr;
        other.framePoses = nullptr;

        return *this;
    }

    /**
     * Unload animation data
     */
    void Unload() { ::UnloadModelAnimation(*this); }

    /**
     * Update model animation pose
     */
    ModelAnimation& Update(const ::Model& model, int frame) {
        ::UpdateModelAnimation(model, *this, frame);
        return *this;
    }

    /**
     * Update model animation mesh bone matrices (GPU skinning)
     */
    ModelAnimation& UpdateBones(const ::Model& model, int frame) {
        ::UpdateModelAnimationBones(model, *this, frame);
        return *this;
    }

    /**
     * Check model animation skeleton match
     */
    [[nodiscard]] bool IsValid(const ::Model& model) const { return ::IsModelAnimationValid(model, *this); }
protected:
    void set(const ::ModelAnimation& model) {
        boneCount = model.boneCount;
        frameCount = model.frameCount;
        bones = model.bones;
        framePoses = model.framePoses;

        // Duplicate the name. TextCopy() uses the null terminator, which we ignore here.
        for (int i = 0; i < 32; i++) {
            name[i] = model.name[i];
        }
    }
};
} // namespace raylib

using RModelAnimation = raylib::ModelAnimation;

#endif // RAYLIB_CPP_INCLUDE_MODELANIMATION_HPP_
