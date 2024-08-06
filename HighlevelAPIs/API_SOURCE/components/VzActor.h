#pragma once
#include "../VizComponentAPIs.h"

namespace vzm
{
    __dojostruct VzActor : VzSceneComp
    {
        VzActor(const VID vid, const std::string& originFrom, const std::string& typeName, const SCENE_COMPONENT_TYPE scenecompType)
            : VzSceneComp(vid, originFrom, typeName, scenecompType) {}

        void SetVisibleLayerMask(const uint8_t layerBits, const uint8_t maskBits);

        void SetRenderableRes(const VID vidGeo, const std::vector<VID>& vidMIs);
        void SetMI(const VID vidMI, const int slot = 0);

        std::vector<VID> GetMIs();
        VID GetMI(const int slot = 0);
        VID GetMaterial(const int slot = 0);
        VID GetGeometry();
    };

    __dojostruct VzSprite : VzSceneComp
    {
        VzSprite(const VID vid, const std::string & originFrom, const std::string & typeName, const SCENE_COMPONENT_TYPE scenecompType)
            : VzSceneComp(vid, originFrom, typeName, scenecompType) {}

        void SetVisibleLayerMask(const uint8_t layerBits, const uint8_t maskBits);

        // use internal geometry (quad) and material (optimal shader for sprite rendering)
        // 
        // The sprite's anchor point, and the point around which the sprite rotates. 
        // A value of (0.5, 0.5) corresponds to the midpoint of the sprite. 
        // A value of (0, 0) corresponds to the lower left corner of the sprite. The default is (0.5, 0.5).
        void SetAnchorPoint(const float u, const float v);
        
        // material settings
    };
}
