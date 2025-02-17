#pragma once
#include "Sprite.h"
#include <unordered_map>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>

namespace Lumos::Graphics
{
    class AnimatedSprite : public Sprite
    {
    public:
        enum class PlayMode
        {
            Loop = 0,
            PingPong
        };

        struct AnimationState
        {
            PlayMode Mode;
            std::vector<glm::vec2> Frames;
            float FrameDuration = 1.0f;

            template <typename Archive>
            void serialize(Archive& archive)
            {
                archive(cereal::make_nvp("PlayMode", Mode),
                        cereal::make_nvp("Frames", Frames),
                        cereal::make_nvp("FrameDuration", FrameDuration));
            }
        };

        AnimatedSprite();

        AnimatedSprite(const SharedPtr<Texture2D>& texture, const glm::vec2& position, const glm::vec2& scale, const std::vector<glm::vec2>& frames, float frameDuration, const std::string& stateName);
        virtual ~AnimatedSprite() = default;

        void OnUpdate(float dt);

        const std::array<glm::vec2, 4>& GetAnimatedUVs();

        void AddState(const std::vector<glm::vec2>& frames, float frameDuration, const std::string& stateName);
        void SetState(const std::string& state);
        const std::string& GetState() const { return m_State; }
        std::unordered_map<std::string, AnimationState>& GetAnimationStates() { return m_AnimationStates; }

        template <typename Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("TexturePath", m_Texture ? m_Texture->GetFilepath() : ""),
                    cereal::make_nvp("Position", m_Position),
                    cereal::make_nvp("Scale", m_Scale),
                    cereal::make_nvp("Colour", m_Colour),
                    cereal::make_nvp("AnimationFrames", m_AnimationStates),
                    cereal::make_nvp("State", m_State));
            archive(SpriteSheetTileSize);
        }

        template <typename Archive>
        void load(Archive& archive)
        {
            std::string textureFilePath;
            archive(cereal::make_nvp("TexturePath", textureFilePath),
                    cereal::make_nvp("Position", m_Position),
                    cereal::make_nvp("Scale", m_Scale),
                    cereal::make_nvp("Colour", m_Colour),
                    cereal::make_nvp("AnimationFrames", m_AnimationStates),
                    cereal::make_nvp("State", m_State));

            if(!textureFilePath.empty())
                m_Texture = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile("sprite", textureFilePath));

            if(Serialisation::CurrentSceneVersion > 21)
                archive(SpriteSheetTileSize);

            SetState(m_State);
        }

        std::unordered_map<std::string, AnimationState> m_AnimationStates;
        uint32_t m_CurrentFrame = 0;
        float m_FrameTimer      = 0.0f;
        std::string m_State;
        bool m_Forward = true;
    };
}
