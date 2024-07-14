#pragma once
#pragma warning (disable : 4251)
#pragma warning (disable : 4819)
// FEngine Warnings
#pragma warning (disable : 4146)
#pragma warning (disable : 4068)
#pragma warning (disable : 4267)
#pragma warning (disable : 4244)

#define _ITERATOR_DEBUG_LEVEL 0 // this is for using STL containers as our standard parameters

#define __dojostatic extern "C" __declspec(dllexport)
#define __dojoclass class __declspec(dllexport)
#define __dojostruct struct __declspec(dllexport)

#define __FP (float*)&
#define VZRESULT int
#define VZ_OK 0
#define VZ_FAIL 1
#define VZ_JOB_WAIT 2
#define VZ_WARNNING 3

// std
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <set>
#include <any>
#include <list>
#include <memory>
#include <algorithm>
#include <chrono>
#include <functional>

using VID = uint32_t;
inline constexpr VID INVALID_VID = 0;
using TimeStamp = std::chrono::high_resolution_clock::time_point;

constexpr float VZ_PI = 3.141592654f;
constexpr float VZ_2PI = 6.283185307f;
constexpr float VZ_1DIVPI = 0.318309886f;
constexpr float VZ_1DIV2PI = 0.159154943f;
constexpr float VZ_PIDIV2 = 1.570796327f;
constexpr float VZ_PIDIV4 = 0.785398163f;

using uint = uint32_t;

//TO DO
//3. Light Cube 확인...
//4. gltfio
//5. Shared Source 정리

namespace vzm
{
    class VzRenderPath;

    template <typename ID> struct ParamMap {
    private:
        std::string __PM_VERSION = "LIBI_1.4";
        std::unordered_map<ID, std::any> __params;
    public:
        bool FindParam(const ID& param_name) {
            auto it = __params.find(param_name);
            return !(it == __params.end());
        }
        template <typename SRCV> bool GetParamCheck(const ID& key, SRCV& param) {
            auto it = __params.find(key);
            if (it == __params.end()) return false;
            param = std::any_cast<SRCV&>(it->second);
            return true;
        }
        template <typename SRCV> SRCV GetParam(const ID& key, const SRCV& init_v) const {
            auto it = __params.find(key);
            if (it == __params.end()) return init_v;
            return std::any_cast<const SRCV&>(it->second);
        }
        template <typename SRCV> SRCV* GetParamPtr(const ID& key) {
            auto it = __params.find(key);
            if (it == __params.end()) return NULL;
            return (SRCV*)&std::any_cast<SRCV&>(it->second);
        }
        template <typename SRCV, typename DSTV> bool GetParamCastingCheck(const ID& key, DSTV& param) {
            auto it = __params.find(key);
            if (it == __params.end()) return false;
            param = (DSTV)std::any_cast<SRCV&>(it->second);
            return true;
        }
        template <typename SRCV, typename DSTV> DSTV GetParamCasting(const ID& key, const DSTV& init_v) {
            auto it = __params.find(key);
            if (it == __params.end()) return init_v;
            return (DSTV)std::any_cast<SRCV&>(it->second);
        }
        void SetParam(const ID& key, const std::any& param) {
            __params[key] = param;
        }
        void RemoveParam(const ID& key) {
            auto it = __params.find(key);
            if (it != __params.end()) {
                __params.erase(it);
            }
        }
        void RemoveAll() {
            __params.clear();
        }
        size_t Size() {
            return __params.size();
        }
        std::string GetPMapVersion() {
            return __PM_VERSION;
        }

        typedef std::unordered_map<ID, std::any> MapType;
        typename typedef MapType::iterator iterator;
        typename typedef MapType::const_iterator const_iterator;
        typename typedef MapType::reference reference;
        iterator begin() { return __params.begin(); }
        const_iterator begin() const { return __params.begin(); }
        iterator end() { return __params.end(); }
        const_iterator end() const { return __params.end(); }
    };

    enum class SCENE_COMPONENT_TYPE // every component involves a transform and a name
    {
        // camera, light, actor component can have renderable resources technically
        SCENEBASE = 0,  // empty (only transform and name)
        CAMERA,
        LIGHT,
        ACTOR,
    };

    enum class RES_COMPONENT_TYPE
    {
        RESOURCE = 0,
        GEOMATRY,
        MATERIAL,
        MATERIALINSTANCE,
    };

    __dojostruct VzBaseComp
    {
        // DO NOT SET
        VID componentVID = INVALID_VID;
        TimeStamp timeStamp = {}; // will be automatically set 
        std::string originFrom;
        std::string type;

        // User data
        ParamMap<std::string> attributes;

        std::string GetName();
        void SetName(const std::string& name);
    };
    __dojostruct VzScene : VzBaseComp
    {
        bool LoadIBL(const std::string& iblPath);
        void SetSkyboxVisibleLayerMask(const uint8_t layerBits = 0x7, const uint8_t maskBits = 0x4);
        void SetLightmapVisibleLayerMask(const uint8_t layerBits = 0x3, const uint8_t maskBits = 0x2); // check where to set
    };
    __dojostruct VzSceneComp : VzBaseComp
    {
        SCENE_COMPONENT_TYPE compType = SCENE_COMPONENT_TYPE::SCENEBASE;

        void GetWorldPosition(float v[3]);
        void GetWorldForward(float v[3]);
        void GetWorldRight(float v[3]);
        void GetWorldUp(float v[3]);
        void GetWorldTransform(float mat[16], const bool rowMajor = false);
        void GetWorldInvTransform(float mat[16], const bool rowMajor = false);

        void GetLocalTransform(float mat[16], const bool rowMajor = false);
        void GetLocalInvTransform(float mat[16], const bool rowMajor = false);

        // local transforms
        void SetTransform(const float s[3] = nullptr, const float q[4] = nullptr, const float t[3] = nullptr, const bool additiveTransform = false);
        void SetMatrix(const float value[16], const bool additiveTransform = false, const bool rowMajor = false);

        VID GetParentVid();
        VID GetSceneVid();
    };
    __dojostruct VzCamera : VzSceneComp
    {
        // Pose parameters are defined in WS (not local space)
        void SetWorldPose(const float pos[3], const float view[3], const float up[3]);
        void SetPerspectiveProjection(const float zNearP, const float zFarP, const float fovInDegree, const float aspectRatio, const bool isVertical = true);
        void GetWorldPose(float pos[3], float view[3], float up[3]);
        void GetPerspectiveProjection(float* zNearP, float* zFarP, float* fovInDegree, float* aspectRatio, bool isVertical = true);

        void SetCameraCubeVisibleLayerMask(const uint8_t layerBits = 0x3, const uint8_t maskBits = 0x2); // helper object

        __dojostruct Controller {
            // Configuration parameters //
            // Common properties
            float targetPosition[3] = {0, 0, 0};        //! World-space position of interest, defaults to (0,0,0)
            float upVector[3] = { 0, 1.f, 0 };            //! Orientation for the home position, defaults to (0,1,0)
            float zoomSpeed = 0.01f;                    //! Multiplied with scroll delta, defaults to 0.01
            // Orbit mode properties
            float orbitHomePosition[3] = {0, 0, 1.f};   //! Initial eye position in world space, defaults to (0,0,1)
            float orbitSpeed[2] = {0.01f, 0.01f};       //! Multiplied with viewport delta, defaults to 0.01
            // Map mode properties
            bool isVerticalFov = true;                  //! The axis that's held constant when viewport changes
            float fovDegrees = 90.f;                    //! The full FOV (not the half-angle)
            float farPlane = 1000.f;                    //! The distance to the far plane
            float mapExtent[2] = {10.f, 10.f};          //! The ground size for computing home position
            float mapMinDistance = 0.01f;               //! Constrains the zoom-in level
            // Free flight properties
            float flightStartPosition[3];               //! Initial eye position in world space, defaults to (0,0,0)
            float flightStartPitch = 0;
            float flightStartYaw = 0;
            float flightMaxSpeed = 10.f;                //! The maximum camera speed in world units per second, defaults to 10
            int flightSpeedSteps = 80;              //! The number of speed steps adjustable with scroll wheel, defaults to 80
            float flightPanSpeed[2] = {0.01f, 0.01f};   //! Multiplied with viewport delta, defaults to 0.01,0.01
            float flightMoveDamping = 15.f;             //! Applies a deceleration to camera movement, 0 (no damping), default is 15
            // Raycast properties
            float groundPlane[4] = {0, 1.f, 0, 0};  //! Plane equation used as a raycast fallback
            void* raycastCallback = nullptr;        //! Raycast function for accurate grab-and-pan
            void* raycastUserdata = nullptr;
            bool panning = true;                    //! Sets whether panning is enabled
            enum class Mode { ORBIT, MAP, FREE_FLIGHT };
            Mode mode = Mode::ORBIT;

            void UpdateControllerSettings();            // call this to apply current configuration 

            enum class Key { FORWARD, LEFT, BACKWARD, RIGHT, UP, DOWN, COUNT };
            void KeyDown(const Key key);
            void KeyUp(const Key key);
            void Scroll(const int x, const int y, const float scrollDelta);
            void GrabBegin(const int x, const int y, const bool strafe);
            void GrabDrag(const int x, const int y);
            void GrabEnd();
            void SetViewport(const int w, const int h);
            void UpdateCamera(const float deltaTime); // this is for final sync to the camera

            VID vidCam = INVALID_VID;   // DO NOT SET
        };
        Controller* GetController();              // this activates the camera manipulator
    };
    __dojostruct VzLight : VzSceneComp
    {
        enum class Type : uint8_t {
            SUN,            //!< Directional light that also draws a sun's disk in the sky.
            DIRECTIONAL,    //!< Directional light, emits light in a given direction.
            POINT,          //!< Point light, emits light from a position, in all directions.
            FOCUSED_SPOT,   //!< Physically correct spot light.
            SPOT,           //!< Spot light with coupling of outer cone and illumination disabled.
        };
        void SetIntensity(const float intensity = 110000);
        float GetIntensity() const;
    };
    __dojostruct VzActor : VzSceneComp
    {
        void SetVisibleLayerMask(const uint8_t layerBits, const uint8_t maskBits);

        void SetRenderableRes(const VID vidGeo, const std::vector<VID>& vidMIs);
        void SetMI(const VID vidMI, const int slot = 0);

        std::vector<VID> GetMIs();
        VID GetMI(const int slot = 0);
        VID GetMaterial(const int slot = 0);
        VID GetGeometry();
    };

    __dojostruct VzResource : VzBaseComp
    {
        RES_COMPONENT_TYPE compType = RES_COMPONENT_TYPE::RESOURCE;
    };
    __dojostruct VzGeometry : VzResource
    {
        // 
    };
    __dojostruct VzMaterial : VzResource
    {
        enum class MaterialType : uint8_t {
            STANDARD = 0,
            CUSTOM
        };
        enum class LightingModel : uint8_t {
            UNLIT,                  //!< no lighting applied, emissive possible
            LIT,                    //!< default, standard lighting
            SUBSURFACE,             //!< subsurface lighting model
            CLOTH,                  //!< cloth lighting model
            SPECULAR_GLOSSINESS,    //!< legacy lighting model
        };
        void SetMaterialType(const MaterialType type);
        MaterialType GetMaterialType() const;

        void SetLightingModel(const LightingModel model);
        LightingModel GetLightingModel() const;
    };
    __dojostruct VzMI : VzMaterial
    {
        static constexpr size_t MATERIAL_PROPERTIES_COUNT = 29;
        enum class MProp : uint8_t {
            BASE_COLOR = 0,              //!< float4, all shading models
            ROUGHNESS,               //!< float,  lit shading models only
            METALLIC,                //!< float,  all shading models, except unlit and cloth
            REFLECTANCE,             //!< float,  all shading models, except unlit and cloth
            AMBIENT_OCCLUSION,       //!< float,  lit shading models only, except subsurface and cloth
            CLEAR_COAT,              //!< float,  lit shading models only, except subsurface and cloth
            CLEAR_COAT_ROUGHNESS,    //!< float,  lit shading models only, except subsurface and cloth
            CLEAR_COAT_NORMAL,       //!< float,  lit shading models only, except subsurface and cloth
            ANISOTROPY,              //!< float,  lit shading models only, except subsurface and cloth
            ANISOTROPY_DIRECTION,    //!< float3, lit shading models only, except subsurface and cloth
            THICKNESS,               //!< float,  subsurface shading model only
            SUBSURFACE_POWER,        //!< float,  subsurface shading model only
            SUBSURFACE_COLOR,        //!< float3, subsurface and cloth shading models only
            SHEEN_COLOR,             //!< float3, lit shading models only, except subsurface
            SHEEN_ROUGHNESS,         //!< float3, lit shading models only, except subsurface and cloth
            SPECULAR_COLOR,          //!< float3, specular-glossiness shading model only
            GLOSSINESS,              //!< float,  specular-glossiness shading model only
            EMISSIVE,                //!< float4, all shading models
            NORMAL,                  //!< float3, all shading models only, except unlit
            POST_LIGHTING_COLOR,     //!< float4, all shading models
            POST_LIGHTING_MIX_FACTOR,//!< float, all shading models
            CLIP_SPACE_TRANSFORM,    //!< mat4,   vertex shader only
            ABSORPTION,              //!< float3, how much light is absorbed by the material
            TRANSMISSION,            //!< float,  how much light is refracted through the material
            IOR,                     //!< float,  material's index of refraction
            MICRO_THICKNESS,         //!< float, thickness of the thin layer
            BENT_NORMAL,             //!< float3, all shading models only, except unlit
            SPECULAR_FACTOR,         //!< float, lit shading models only, except subsurface and cloth
            SPECULAR_COLOR_FACTOR,   //!< float3, lit shading models only, except subsurface and cloth

            // when adding new Properties, make sure to update MATERIAL_PROPERTIES_COUNT
        };

        enum class RgbType : uint8_t {
            /**
             * the color is defined in Rec.709-sRGB-D65 (sRGB) space and the RGB values
             * have not been pre-multiplied by the alpha (for instance, a 50%
             * transparent red is <1,0,0,0.5>)
             */
            sRGB,
            /**
             * the color is defined in Rec.709-Linear-D65 ("linear sRGB") space and the
             * RGB values have not been pre-multiplied by the alpha (for instance, a 50%
             * transparent red is <1,0,0,0.5>)
             */
            LINEAR,
            /**
             * the color is defined in Rec.709-sRGB-D65 (sRGB) space and the RGB values
             * have been pre-multiplied by the alpha (for instance, a 50%
             * transparent red is <0.5,0,0,0.5>)
             */
            PREMULTIPLIED_sRGB,
            /**
             * the color is defined in Rec.709-Linear-D65 ("linear sRGB") space and the
             * RGB values have been pre-multiplied by the alpha (for instance, a 50%
             * transparent red is <0.5,0,0,0.5>)
             */
            PREMULTIPLIED_LINEAR
        } rgbType = RgbType::LINEAR;

        enum class TransparencyMode : uint8_t {
            DEFAULT = 0, // following the rasterizer state
            TWO_PASSES_ONE_SIDE,
            TWO_PASSES_TWO_SIDES
        };

        void SetTransparencyMode(const TransparencyMode tMode);
        void SetMaterialProperty(const MProp mProp, const std::vector<float>& v);

        // use attributes
        //void Update();
    };

    __dojostruct VzAsset : VzBaseComp
    {
        std::vector<VID> GetGLTFRootVIDs();
        VID GetSkeleton();

        // Animator //
        __dojostruct Animator{
            void GrabBegin(const int x, const int y, const bool strafe);
            void GrabDrag(const int x, const int y);
            void GrabEnd();
            void SetViewport(const int w, const int h);
            void UpdateCamera(const float deltaTime); // this is for final sync to the camera

            VID vidCam = INVALID_VID;   // DO NOT SET
        };
        Animator* GetAnimator();              // this activates the camera manipulator
    };

    __dojostruct VzSkeleton : VzBaseComp
    {
        using BoneVID = VID;

        // componentVID refers to the root bone
        std::vector<BoneVID> GetBones(); // including this
        std::vector<BoneVID> GetBoneChildren(const BoneVID vidBone);
        std::string GetBoneName();

        void SetTransformTRS(const BoneVID vidBone, const float t[3], const float r[4], const float s[3]);
        void UpdateBoneMatrices();

        // future work... skinning...
    };

    __dojostruct VzRenderer : VzBaseComp
    {
        void SetCanvas(const uint32_t w, const uint32_t h, const float dpi, void* window = nullptr);
        void GetCanvas(uint32_t* w, uint32_t* h, float* dpi, void** window = nullptr);

        void SetViewport(const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h);
        void GetViewport(uint32_t* x, uint32_t* y, uint32_t* w, uint32_t* h);

        void SetVisibleLayerMask(const uint8_t layerBits, const uint8_t maskBits);
        // setters and getters of rendering options
        VZRESULT Render(const VID vidScene, const VID vidCam);
    };
}
