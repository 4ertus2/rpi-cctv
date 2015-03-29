#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <string>
#include <vector>
#include <iostream>

#include <bcm_host.h>

#include <interface/vcos/vcos_semaphore.h>
#include <interface/vmcs_host/vchost.h>

#include <IL/OMX_Core.h>
#include <IL/OMX_Component.h>
#include <IL/OMX_Video.h>
#include <IL/OMX_Broadcom.h>


namespace
{
    static const char * format2str(OMX_VIDEO_CODINGTYPE c)
    {
        switch (c)
        {
            case OMX_VIDEO_CodingUnused:        return "not used";
            case OMX_VIDEO_CodingAutoDetect:    return "autodetect";
            case OMX_VIDEO_CodingMPEG2:         return "MPEG2";
            case OMX_VIDEO_CodingH263:          return "H.263";
            case OMX_VIDEO_CodingMPEG4:         return "MPEG4";
            case OMX_VIDEO_CodingWMV:           return "Windows Media Video";
            case OMX_VIDEO_CodingRV:            return "RealVideo";
            case OMX_VIDEO_CodingAVC:           return "H.264/AVC";
            case OMX_VIDEO_CodingMJPEG:         return "Motion JPEG";
            case OMX_VIDEO_CodingVP6:           return "VP6";
            case OMX_VIDEO_CodingVP7:           return "VP7";
            case OMX_VIDEO_CodingVP8:           return "VP8";
            case OMX_VIDEO_CodingYUV:           return "Raw YUV video";
            case OMX_VIDEO_CodingSorenson:      return "Sorenson";
            case OMX_VIDEO_CodingTheora:        return "OGG Theora";
            case OMX_VIDEO_CodingMVC:           return "H.264/MVC";
            default:
                std::cerr << "unknown OMX_VIDEO_CODINGTYPE: " << c << std::endl;
                return "unknown";
        }
    }

    static const char * format2str(OMX_COLOR_FORMATTYPE c)
    {
        switch (c)
        {
            case OMX_COLOR_FormatUnused:                 return "OMX_COLOR_FormatUnused: not used";
            case OMX_COLOR_FormatMonochrome:             return "OMX_COLOR_FormatMonochrome";
            case OMX_COLOR_Format8bitRGB332:             return "OMX_COLOR_Format8bitRGB332";
            case OMX_COLOR_Format12bitRGB444:            return "OMX_COLOR_Format12bitRGB444";
            case OMX_COLOR_Format16bitARGB4444:          return "OMX_COLOR_Format16bitARGB4444";
            case OMX_COLOR_Format16bitARGB1555:          return "OMX_COLOR_Format16bitARGB1555";
            case OMX_COLOR_Format16bitRGB565:            return "OMX_COLOR_Format16bitRGB565";
            case OMX_COLOR_Format16bitBGR565:            return "OMX_COLOR_Format16bitBGR565";
            case OMX_COLOR_Format18bitRGB666:            return "OMX_COLOR_Format18bitRGB666";
            case OMX_COLOR_Format18bitARGB1665:          return "OMX_COLOR_Format18bitARGB1665";
            case OMX_COLOR_Format19bitARGB1666:          return "OMX_COLOR_Format19bitARGB1666";
            case OMX_COLOR_Format24bitRGB888:            return "OMX_COLOR_Format24bitRGB888";
            case OMX_COLOR_Format24bitBGR888:            return "OMX_COLOR_Format24bitBGR888";
            case OMX_COLOR_Format24bitARGB1887:          return "OMX_COLOR_Format24bitARGB1887";
            case OMX_COLOR_Format25bitARGB1888:          return "OMX_COLOR_Format25bitARGB1888";
            case OMX_COLOR_Format32bitBGRA8888:          return "OMX_COLOR_Format32bitBGRA8888";
            case OMX_COLOR_Format32bitARGB8888:          return "OMX_COLOR_Format32bitARGB8888";
            case OMX_COLOR_FormatYUV411Planar:           return "OMX_COLOR_FormatYUV411Planar";
            case OMX_COLOR_FormatYUV411PackedPlanar:     return "OMX_COLOR_FormatYUV411PackedPlanar: Planes fragmented when a frame is split in multiple buffers";
            case OMX_COLOR_FormatYUV420Planar:           return "OMX_COLOR_FormatYUV420Planar: Planar YUV, 4:2:0 (I420)";
            case OMX_COLOR_FormatYUV420PackedPlanar:     return "OMX_COLOR_FormatYUV420PackedPlanar: Planar YUV, 4:2:0 (I420), planes fragmented when a frame is split in multiple buffers";
            case OMX_COLOR_FormatYUV420SemiPlanar:       return "OMX_COLOR_FormatYUV420SemiPlanar, Planar YUV, 4:2:0 (NV12), U and V planes interleaved with first U value";
            case OMX_COLOR_FormatYUV422Planar:           return "OMX_COLOR_FormatYUV422Planar";
            case OMX_COLOR_FormatYUV422PackedPlanar:     return "OMX_COLOR_FormatYUV422PackedPlanar: Planes fragmented when a frame is split in multiple buffers";
            case OMX_COLOR_FormatYUV422SemiPlanar:       return "OMX_COLOR_FormatYUV422SemiPlanar";
            case OMX_COLOR_FormatYCbYCr:                 return "OMX_COLOR_FormatYCbYCr";
            case OMX_COLOR_FormatYCrYCb:                 return "OMX_COLOR_FormatYCrYCb";
            case OMX_COLOR_FormatCbYCrY:                 return "OMX_COLOR_FormatCbYCrY";
            case OMX_COLOR_FormatCrYCbY:                 return "OMX_COLOR_FormatCrYCbY";
            case OMX_COLOR_FormatYUV444Interleaved:      return "OMX_COLOR_FormatYUV444Interleaved";
            case OMX_COLOR_FormatRawBayer8bit:           return "OMX_COLOR_FormatRawBayer8bit";
            case OMX_COLOR_FormatRawBayer10bit:          return "OMX_COLOR_FormatRawBayer10bit";
            case OMX_COLOR_FormatRawBayer8bitcompressed: return "OMX_COLOR_FormatRawBayer8bitcompressed";
            case OMX_COLOR_FormatL2:                     return "OMX_COLOR_FormatL2";
            case OMX_COLOR_FormatL4:                     return "OMX_COLOR_FormatL4";
            case OMX_COLOR_FormatL8:                     return "OMX_COLOR_FormatL8";
            case OMX_COLOR_FormatL16:                    return "OMX_COLOR_FormatL16";
            case OMX_COLOR_FormatL24:                    return "OMX_COLOR_FormatL24";
            case OMX_COLOR_FormatL32:                    return "OMX_COLOR_FormatL32";
            case OMX_COLOR_FormatYUV420PackedSemiPlanar: return "OMX_COLOR_FormatYUV420PackedSemiPlanar: Planar YUV, 4:2:0 (NV12), planes fragmented when a frame is split in multiple buffers, U and V planes interleaved with first U value";
            case OMX_COLOR_FormatYUV422PackedSemiPlanar: return "OMX_COLOR_FormatYUV422PackedSemiPlanar: Planes fragmented when a frame is split in multiple buffers";
            case OMX_COLOR_Format18BitBGR666:            return "OMX_COLOR_Format18BitBGR666";
            case OMX_COLOR_Format24BitARGB6666:          return "OMX_COLOR_Format24BitARGB6666";
            case OMX_COLOR_Format24BitABGR6666:          return "OMX_COLOR_Format24BitABGR6666";
            case OMX_COLOR_Format32bitABGR8888:          return "OMX_COLOR_Format32bitABGR8888";
            case OMX_COLOR_Format8bitPalette:            return "OMX_COLOR_Format8bitPalette";
            case OMX_COLOR_FormatYUVUV128:               return "OMX_COLOR_FormatYUVUV128";
            case OMX_COLOR_FormatRawBayer12bit:          return "OMX_COLOR_FormatRawBayer12bit";
            case OMX_COLOR_FormatBRCMEGL:                return "OMX_COLOR_FormatBRCMEGL";
            case OMX_COLOR_FormatBRCMOpaque:             return "OMX_COLOR_FormatBRCMOpaque";
            case OMX_COLOR_FormatYVU420PackedPlanar:     return "OMX_COLOR_FormatYVU420PackedPlanar";
            case OMX_COLOR_FormatYVU420PackedSemiPlanar: return "OMX_COLOR_FormatYVU420PackedSemiPlanar";
            default:
                std::cerr << "unknown OMX_COLOR_FORMATTYPE: " << c << std::endl;
                return "unknown";
        }
    }

    static void dump_portdef(OMX_PARAM_PORTDEFINITIONTYPE* portdef)
    {
        fprintf(stderr, "Port %d is %s, %s, buffers wants:%d needs:%d, size:%d, pop:%d, aligned:%d\n",
            portdef->nPortIndex,
            (portdef->eDir ==  OMX_DirInput ? "input" : "output"),
            (portdef->bEnabled == OMX_TRUE ? "enabled" : "disabled"),
            portdef->nBufferCountActual,
            portdef->nBufferCountMin,
            portdef->nBufferSize,
            portdef->bPopulated,
            portdef->nBufferAlignment);

        OMX_VIDEO_PORTDEFINITIONTYPE *viddef = &portdef->format.video;
        OMX_IMAGE_PORTDEFINITIONTYPE *imgdef = &portdef->format.image;

        switch (portdef->eDomain)
        {
            case OMX_PortDomainVideo:
                fprintf(stderr, "Video type:\n"
                    "\tWidth:\t\t%d\n"
                    "\tHeight:\t\t%d\n"
                    "\tStride:\t\t%d\n"
                    "\tSliceHeight:\t%d\n"
                    "\tBitrate:\t%d\n"
                    "\tFramerate:\t%.02f\n"
                    "\tError hiding:\t%s\n"
                    "\tCodec:\t\t%s\n"
                    "\tColor:\t\t%s\n",
                    viddef->nFrameWidth,
                    viddef->nFrameHeight,
                    viddef->nStride,
                    viddef->nSliceHeight,
                    viddef->nBitrate,
                    ((float)viddef->xFramerate / (float)65536),
                    (viddef->bFlagErrorConcealment == OMX_TRUE ? "yes" : "no"),
                    format2str(viddef->eCompressionFormat),
                    format2str(viddef->eColorFormat));
                break;
            case OMX_PortDomainImage:
                fprintf(stderr, "Image type:\n"
                    "\tWidth:\t\t%d\n"
                    "\tHeight:\t\t%d\n"
                    "\tStride:\t\t%d\n"
                    "\tSliceHeight:\t%d\n"
                    "\tError hiding:\t%s\n"
                    "\tCodec:\t\t%s\n"
                    "\tColor:\t\t%s\n",
                    imgdef->nFrameWidth,
                    imgdef->nFrameHeight,
                    imgdef->nStride,
                    imgdef->nSliceHeight,
                    (imgdef->bFlagErrorConcealment == OMX_TRUE ? "yes" : "no"),
                    format2str( (OMX_VIDEO_CODINGTYPE) imgdef->eCompressionFormat ),
                    format2str(imgdef->eColorFormat));
                break;
            default:
                break;
        }
    }

    const char * eventType2Str(OMX_EVENTTYPE eEvent)
    {
        switch (eEvent)
        {
            case OMX_EventCmdComplete:          return "OMX_EventCmdComplete";
            case OMX_EventError:                return "OMX_EventError";
            case OMX_EventMark:                 return "OMX_EventMark";
            case OMX_EventPortSettingsChanged:  return "OMX_EventPortSettingsChanged";
            case OMX_EventBufferFlag:           return "OMX_EventBufferFlag";
            case OMX_EventResourcesAcquired:    return "OMX_EventResourcesAcquired";
            case OMX_EventComponentResumed:     return "OMX_EventComponentResumed";
            case OMX_EventDynamicResourcesAvailable: return "OMX_EventDynamicResourcesAvailable";
            case OMX_EventPortFormatDetected:   return "OMX_EventPortFormatDetected";
            case OMX_EventKhronosExtensions:    return "OMX_EventKhronosExtensions";
            case OMX_EventVendorStartUnused:    return "OMX_EventVendorStartUnused";
            case OMX_EventParamOrConfigChanged: return "OMX_EventParamOrConfigChanged";
            default:
                break;
        };

        return nullptr;
    }

    static void printEvent(const char * compName, OMX_HANDLETYPE hComponent, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2)
    {
        const char * strEvent = eventType2Str(eEvent);
        if (strEvent)
            fprintf(stderr, "%s (%p) %s, data: %d, %d\n", compName, hComponent, strEvent, nData1, nData2);
        else
            fprintf(stderr, "%s (%p) 0x%08x, data: %d, %d\n", compName, hComponent, eEvent, nData1, nData2);
    }
}

namespace broadcom
{
    // TODO: add all

    typedef enum
    {
        VIDEO_SCHEDULER = 10,
        SOURCE = 20,
        RESIZER = 60,
        CAMERA = 70,
        CLOCK = 80,
        VIDEO_RENDER = 90,
        VIDEO_DECODER = 130,
        VIDEO_ENCODER = 200,
        EGL_RENDER = 220,
        NULL_SINK = 240,
        VIDEO_SPLITTER = 250
    } ComponentType;

    static const char * componentType2name(ComponentType type)
    {
        switch (type)
        {
            case VIDEO_SCHEDULER:   return "OMX.broadcom.video_scheduler";
            case SOURCE:            return "OMX.broadcom.source";
            case RESIZER:           return "OMX.broadcom.resize";
            case CAMERA:            return "OMX.broadcom.camera";
            case CLOCK:             return "OMX.broadcom.clock";
            case VIDEO_RENDER:      return "OMX.broadcom.video_render";
            case VIDEO_DECODER:     return "OMX.broadcom.video_decode";
            case VIDEO_ENCODER:     return "OMX.broadcom.video_encode";
            case EGL_RENDER:        return "OMX.broadcom.egl_render";
            case NULL_SINK:         return "OMX.broadcom.null_sink";
            case VIDEO_SPLITTER:    return "OMX.broadcom.video_splitter";
        }

        return nullptr;
    }

    static unsigned componentPortsCount(ComponentType type)
    {
        switch (type)
        {
            case VIDEO_SCHEDULER:   return 3;
            case SOURCE:            return 1;
            case RESIZER:           return 2;
            case CAMERA:            return 4;
            case CLOCK:             return 6;
            case VIDEO_RENDER:      return 1;
            case VIDEO_DECODER:     return 2;
            case VIDEO_ENCODER:     return 2;
            case EGL_RENDER:        return 2;
            case NULL_SINK:         return 3;
            case VIDEO_SPLITTER:    return 5;
        }

        return 0;
    }

    struct VcosSemaphore
    {
        VcosSemaphore(const char * name)
        {
            if (vcos_semaphore_create(&sem_, name, 1) != VCOS_SUCCESS)
                throw "Failed to create handler lock semaphore";
        }

        ~VcosSemaphore()
        {
            vcos_semaphore_delete(&sem_);
        }

        VCOS_STATUS_T wait() { return vcos_semaphore_wait(&sem_); }
        VCOS_STATUS_T post() { return vcos_semaphore_post(&sem_); }

    private:
        VCOS_SEMAPHORE_T sem_;
    };

    class VcosLock
    {
    public:
        VcosLock(VcosSemaphore * sem)
        :   sem_(sem)
        {
            sem_->wait();
        }

        ~VcosLock()
        {
            sem_->post();
        }

    private:
        VcosSemaphore * sem_;
    };
}

namespace rpi_omx
{
    typedef broadcom::ComponentType ComponentType;
    using broadcom::componentType2name;
    using broadcom::componentPortsCount;

    using broadcom::VcosSemaphore;
    using Lock = broadcom::VcosLock;

    VcosSemaphore * pSemaphore;

    //

    static OMX_ERRORTYPE callback_EventHandler(
        OMX_HANDLETYPE hComponent,
        OMX_PTR pAppData,
        OMX_EVENTTYPE eEvent,
        OMX_U32 nData1,
        OMX_U32 nData2,
        OMX_PTR pEventData);

    static OMX_ERRORTYPE callback_EmptyBufferDone(
        OMX_HANDLETYPE hComponent,
        OMX_PTR pAppData,
        OMX_BUFFERHEADERTYPE * pBuffer);

    static OMX_ERRORTYPE callback_FillBufferDone(
        OMX_HANDLETYPE hComponent,
        OMX_PTR pAppData,
        OMX_BUFFERHEADERTYPE * pBuffer);

    OMX_CALLBACKTYPE cbsEvents = {
        .EventHandler = callback_EventHandler,
        .EmptyBufferDone = callback_EmptyBufferDone,
        .FillBufferDone = callback_FillBufferDone
    };

    //

    ///
    class OMXExeption
    {
    public:
        static const unsigned MAX_LEN = 512;

        OMXExeption(OMX_ERRORTYPE errCode, const char * file, unsigned line, const char * msg = nullptr)
        :   errCode_(errCode)
        {
            if (msg && msg[0])
                snprintf(msg_, MAX_LEN, "%s:%d OpenMAX IL error: 0x%08x. %s", file, line, errCode, msg);
            else
                snprintf(msg_, MAX_LEN, "%s:%d OpenMAX IL error: 0x%08x", file, line, errCode);
        }

        OMX_ERRORTYPE code() const { return errCode_; }
        const char * what() const { return msg_; }

        static void die(OMX_ERRORTYPE error, const char * str)
        {
            const char * errStr = omxErr2str(error);
            fprintf(stderr, "OMX error: %s: 0x%08x %s\n", str, error, errStr);
            exit(1);
        }

    private:
        OMX_ERRORTYPE errCode_;
        char msg_[MAX_LEN];

        static const char * omxErr2str(OMX_ERRORTYPE error)
        {
            switch (error)
            {
                case OMX_ErrorNone:                     return "OMX_ErrorNone";
                case OMX_ErrorBadParameter:             return "OMX_ErrorBadParameter";
                case OMX_ErrorIncorrectStateOperation:  return "OMX_ErrorIncorrectStateOperation";
                case OMX_ErrorIncorrectStateTransition: return "OMX_ErrorIncorrectStateTransition";
                case OMX_ErrorInsufficientResources:    return "OMX_ErrorInsufficientResources";
                case OMX_ErrorBadPortIndex:             return "OMX_ErrorBadPortIndex";
                case OMX_ErrorHardware:                 return "OMX_ErrorHardware";
                // ...

                default:
                    break;
            }

            return "";
        }
    };

#define ERR_OMX(err, msg) if ((err) != OMX_ErrorNone) throw OMXExeption(err, __FILE__, __LINE__, msg)

    ///
    struct VideoFromat
    {
        typedef enum
        {
            RATIO_4x3,
            RATIO_16x9
        } Ratio;

        unsigned width;
        unsigned height;
        unsigned framerate;
        Ratio ratio;
        bool fov;
    };

    /// #   Resolution  Aspect Ratio    Framerates  Video   Image   FoV     Binning
    /// 1   1920x1080   16:9            1-30fps     x               Partial None
    /// 2   2592x1944   4:3             1-15fps     x       x       Full    None
    /// 3   2592x1944   4:3             0.1666-1fps x       x       Full    None
    /// 4   1296x972    4:3             1-42fps     x               Full    2x2
    /// 5   1296x730    16:9            1-49fps     x               Full    2x2
    /// 6   640x480     4:3             42.1-60fps  x               Full    4x4
    /// 7   640x480     4:3             60.1-90fps  x               Full    4x4
    ///
    static const VideoFromat VF_1920x1080 = { 1920, 1080, 30, VideoFromat::RATIO_16x9, false };
    //static const VideoFromat VF_2560x1920 = { 2560, 1920, 0, VideoFromat::RATIO_4x3, true };
    static const VideoFromat VF_1280x960 = { 1280, 960, 30, VideoFromat::RATIO_4x3, true };
    static const VideoFromat VF_1280x720 = { 1280, 720, 30, VideoFromat::RATIO_16x9, true };
    static const VideoFromat VF_640x480 = { 640, 480, 30, VideoFromat::RATIO_4x3, true };

    static const VideoFromat VF_RESIZED_640x480 = VF_640x480;
    static const VideoFromat VF_RESIZED_320x240 = { 320, 240, 30, VideoFromat::RATIO_4x3, true };
    static const VideoFromat VF_RESIZED_256x192 = { 256, 192, 30, VideoFromat::RATIO_4x3, true };
    static const VideoFromat VF_RESIZED_160x120 = { 160, 120, 30, VideoFromat::RATIO_4x3, true };
    static const VideoFromat VF_RESIZED_128x96 = { 128, 96, 30, VideoFromat::RATIO_4x3, true };

    static const VideoFromat VF_RESIZED_960x540 = { 960, 540, 30, VideoFromat::RATIO_16x9, false };
    static const VideoFromat VF_RESIZED_640x360 = { 640, 360, 30, VideoFromat::RATIO_16x9, false };
    static const VideoFromat VF_RESIZED_480x270 = { 480, 270, 30, VideoFromat::RATIO_16x9, false };
    static const VideoFromat VF_RESIZED_384x216 = { 384, 216, 30, VideoFromat::RATIO_16x9, false };
    static const VideoFromat VF_RESIZED_320x180 = { 320, 180, 30, VideoFromat::RATIO_16x9, false };
    static const VideoFromat VF_RESIZED_240x135 = { 240, 135, 30, VideoFromat::RATIO_16x9, false };

    ///
    template <typename T>
    class Parameter
    {
    public:
        Parameter()
        {
            init();
        }

        void init()
        {
            memset(&param_, 0, sizeof(param_));
            param_.nSize = sizeof(param_);
            param_.nVersion.nVersion = OMX_VERSION;
            param_.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
            param_.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
            param_.nVersion.s.nRevision = OMX_VERSION_REVISION;
            param_.nVersion.s.nStep = OMX_VERSION_STEP;
        }

        T& operator * () { return param_; }
        T* operator & () { return &param_; }
        T* operator -> () { return &param_; }

        const T& operator * () const { return param_; }
        const T* operator & () const { return &param_; }
        const T* operator -> () const { return &param_; }

    private:
        T param_;
    };

    ///
    class OMXInit
    {
    public:
        OMXInit()
        {
            ERR_OMX( OMX_Init(), "OMX initalization failed" );
        }

        ~OMXInit()
        {
            try
            {
                ERR_OMX( OMX_Deinit(), "OMX de-initalization failed" );
            }
            catch (const OMXExeption& )
            {
                // TODO
            }
        }
    };

    ///
    class Buffer
    {
    public:
        Buffer()
        :   ppBuffer_(nullptr),
            fillDone_(false)
        {}

        bool filled() const { return fillDone_; }

        void setFilled(bool val = true)
        {
            Lock lock(pSemaphore); // LOCK

            fillDone_ = val;
        }

        OMX_BUFFERHEADERTYPE ** pHeader() { return &ppBuffer_; }
        OMX_BUFFERHEADERTYPE * header() { return ppBuffer_; }
        OMX_U32 flags() const { return ppBuffer_->nFlags; }
        OMX_U32& flags() { return ppBuffer_->nFlags; }

        OMX_U8 * data() { return  ppBuffer_->pBuffer + ppBuffer_->nOffset; }
        OMX_U32 dataSize() const { return ppBuffer_->nFilledLen; }
        OMX_U32 allocSize() const { return ppBuffer_->nAllocLen; }

    private:
        OMX_BUFFERHEADERTYPE * ppBuffer_;
        bool fillDone_;
    };

    ///
    class Component
    {
    public:
        OMX_HANDLETYPE& component() { return component_; }
        ComponentType type() const { return type_; }
        const char * name() const { return componentType2name(type_); }
        unsigned numPorts() const { return componentPortsCount(type_); }

        void dumpPort(OMX_U32 nPortIndex, OMX_BOOL dumpFormats = OMX_FALSE)
        {
            Parameter<OMX_PARAM_PORTDEFINITIONTYPE> portdef;
            getPortDefinition(nPortIndex, portdef);

            dump_portdef(&portdef);

            if (dumpFormats)
            {
                Parameter<OMX_VIDEO_PARAM_PORTFORMATTYPE> portformat;
                portformat->nPortIndex = nPortIndex;
                portformat->nIndex = 0;

                std::cerr << "Port " << nPortIndex << " supports these video formats:" << std::endl;

                for (;; portformat->nIndex++)
                {
                    OMX_ERRORTYPE err = OMX_GetParameter(component_, OMX_IndexParamVideoPortFormat, &portformat);
                    if (err != OMX_ErrorNone)
                        break;

                    std::cerr << "\t" << format2str(portformat->eColorFormat)
                        << ", compression: " << format2str(portformat->eCompressionFormat) << std::endl;
                }
            }
        }

        OMX_STATETYPE state()
        {
            OMX_STATETYPE state;
            ERR_OMX( OMX_GetState(component_, &state), "OMX_GetState" );
            return state;
        }

        void switchState(OMX_STATETYPE newState)
        {
            unsigned value = eventState_;
            ERR_OMX( OMX_SendCommand(component_, OMX_CommandStateSet, newState, NULL), "switch state");

            if (! waitValue(&eventState_, value + 1))
                std::cerr << name() << " lost state changed event" << std::endl;
#if 0
            if (! waitStateChanged(newState))
                std::cerr << name() << " state wanted: " << newState << " observed: " << state() << std::endl;
#endif
        }

        unsigned waitCount(OMX_U32 nPortIndex) const { return (nPortIndex == OMX_ALL) ? numPorts() : 1; }

        void enablePort(OMX_U32 nPortIndex = OMX_ALL)
        {
            unsigned value = eventEnabled_;
            ERR_OMX( OMX_SendCommand(component_, OMX_CommandPortEnable, nPortIndex, NULL), "enable port");

            if (! waitValue(&eventEnabled_, value + waitCount(nPortIndex)))
                std::cerr << name() << " port " << nPortIndex << " lost enable port event(s)" << std::endl;
        }

        void disablePort(OMX_U32 nPortIndex = OMX_ALL)
        {
            unsigned value = eventDisabled_;
            ERR_OMX( OMX_SendCommand(component_, OMX_CommandPortDisable, nPortIndex, NULL), "disable port");

            if (! waitValue(&eventDisabled_, value + waitCount(nPortIndex)))
                std::cerr << name() << " port " << nPortIndex << " lost disable port event(s)" << std::endl;
        }

        void flushPort(OMX_U32 nPortIndex = OMX_ALL)
        {
            unsigned value = eventFlushed_;
            ERR_OMX( OMX_SendCommand(component_, OMX_CommandFlush, nPortIndex, NULL), "flush buffers");

            if (! waitValue(&eventFlushed_, value + waitCount(nPortIndex)))
                std::cerr << name() << " port " << nPortIndex << " lost flush port event(s)" << std::endl;
        }

        void getPortDefinition(OMX_U32 nPortIndex, Parameter<OMX_PARAM_PORTDEFINITIONTYPE>& portDef)
        {
            portDef->nPortIndex = nPortIndex;
            ERR_OMX( OMX_GetParameter(component_, OMX_IndexParamPortDefinition, &portDef), "get port definition");
        }

        void setPortDefinition(OMX_U32 nPortIndex, Parameter<OMX_PARAM_PORTDEFINITIONTYPE>& portDef)
        {
            portDef->nPortIndex = nPortIndex;
            ERR_OMX( OMX_SetParameter(component_, OMX_IndexParamPortDefinition, &portDef), "set port definition");
        }

        void allocBuffers(OMX_U32 nPortIndex, Buffer& buffer)
        {
            Parameter<OMX_PARAM_PORTDEFINITIONTYPE> portDef;
            getPortDefinition(nPortIndex, portDef);

            ERR_OMX( OMX_AllocateBuffer(component_, buffer.pHeader(), nPortIndex, NULL, portDef->nBufferSize), "OMX_AllocateBuffer");
        }

        void freeBuffers(OMX_U32 nPortIndex, Buffer& buffer)
        {
            ERR_OMX( OMX_FreeBuffer(component_, nPortIndex, buffer.header()), "OMX_FreeBuffer");
        }

        void callFillThisBuffer(Buffer& buffer)
        {
            ERR_OMX( OMX_FillThisBuffer(component_, buffer.header()), "OMX_FillThisBuffer");
        }

        void eventCmdComplete(OMX_U32 cmd, OMX_U32 /*nPortIndex*/)
        {
            Lock lock(pSemaphore);  // LOCK

            switch (cmd)
            {
                case OMX_CommandStateSet:
                    ++eventState_;
                    break;

                case OMX_CommandFlush:
                    ++eventFlushed_;
                    break;

                case OMX_CommandPortDisable:
                    ++eventDisabled_;
                    break;

                case OMX_CommandPortEnable:
                    ++eventEnabled_;
                    break;

                case OMX_CommandMarkBuffer:
                default:
                    break;
            }
        }

        void eventPortSettingsChanged(OMX_U32 nPortIndex)
        {
            Lock lock(pSemaphore);  // LOCK

            ++changedPorts_[n2idx(nPortIndex)];
        }

    protected:
        OMX_HANDLETYPE component_;
        ComponentType type_;

        Component(ComponentType type, OMX_PTR pAppData, OMX_CALLBACKTYPE * callbacks)
        :   type_(type),
            eventState_(0),
            eventFlushed_(0),
            eventDisabled_(0),
            eventEnabled_(0)
        {
            changedPorts_.resize(numPorts());

            OMX_STRING xName = const_cast<OMX_STRING>(name());
            ERR_OMX( OMX_GetHandle(&component_, xName, pAppData, callbacks), "OMX_GetHandle");

            disablePort();
        }

        ~Component()
        {
            try
            {
                ERR_OMX( OMX_FreeHandle(component_), "OMX_FreeHandle" );
            }
            catch (const OMXExeption& )
            {
                // TODO
            }
        }

        // type_ equals to first port number
        unsigned n2idx(OMX_U32 nPortIndex) const { return nPortIndex - type_; }
        unsigned idx2n(unsigned idx) const { return type_ + idx; }

    private:
        static const unsigned WAIT_CHANGES_US = 10000;
        static const unsigned MAX_WAIT_COUNT = 200;

        unsigned eventState_;
        unsigned eventFlushed_;
        unsigned eventDisabled_;
        unsigned eventEnabled_;
        std::vector<unsigned> changedPorts_;

        // TODO: wait for specific port changes
        bool waitValue(unsigned * pValue, unsigned wantedValue)
        {
            for (unsigned i = 0; i < MAX_WAIT_COUNT; ++i)
            {
                if (*pValue == wantedValue)
                    return true;

                usleep(WAIT_CHANGES_US);
            }

            return false;
        }
#if 0
        bool waitStateChanged(OMX_STATETYPE wantedState)
        {
            for (unsigned i=0; i < MAX_WAIT_COUNT; ++i)
            {
                if (state() == wantedState)
                    return true;

                usleep(WAIT_CHANGES_US);
            }

            return false;
        }
#endif
    };

    /// Raspberry Pi Camera Module
    class Camera : public Component
    {
    public:
        static const ComponentType cType = broadcom::CAMERA;

        static const unsigned OPORT_PREVIEW = 70;
        static const unsigned OPORT_VIDEO = 71;
        static const unsigned OPORT_STILL = 72;
        static const unsigned IPORT = 73;

        static const unsigned CAM_DEVICE_NUMBER = 0;

        static int32_t align(unsigned x, unsigned y)
        {
            return (x + y - 1) & (~(y - 1));
        }

        // The recommended initialisation sequence:
        // 1. Create component.
        // 2. Use OMX_IndexConfigRequestCallback to request callbacks on OMX_IndexParamCameraDeviceNumber.
        // 3. Set OMX_IndexParamISPTunerName.
        // 4. Set OMX_IndexParamCameraFlashType.
        // 5. Set OMX_IndexParamCameraDeviceNumber.
        // 6. Wait for the callback that OMX_IndexParamCameraDeviceNumber has changed.
        //      At this point, all the drivers have been loaded. Other settings can be applied whilst waiting for this event.
        // 7. Query for OMX_IndexConfigCameraSensorModes as required.
        // 8. Change state to IDLE, and proceed as required.
        Camera()
        :   Component(cType, (OMX_PTR) this, &cbsEvents),
            ready_(false)
        {
            requestCallback();
            setDeviceNumber(CAM_DEVICE_NUMBER);
        }

        void requestCallback()
        {
            Parameter<OMX_CONFIG_REQUESTCALLBACKTYPE> cbtype;
            cbtype->nPortIndex = OMX_ALL;
            cbtype->nIndex     = OMX_IndexParamCameraDeviceNumber;
            cbtype->bEnable    = OMX_TRUE;

            ERR_OMX( OMX_SetConfig(component_, OMX_IndexConfigRequestCallback, &cbtype), "request callbacks");
        }

        void setDeviceNumber(unsigned camNumber)
        {
            Parameter<OMX_PARAM_U32TYPE> device;
            device->nPortIndex = OMX_ALL;
            device->nU32 = camNumber;

            ERR_OMX( OMX_SetParameter(component_, OMX_IndexParamCameraDeviceNumber, &device), "set camera device number");
        }

        void setVideoFromat(const VideoFromat& videoFormat)
        {
            Parameter<OMX_PARAM_PORTDEFINITIONTYPE> portDef;
            getPortDefinition(OPORT_VIDEO, portDef);

            portDef->format.video.nFrameWidth  = videoFormat.width;
            portDef->format.video.nFrameHeight = videoFormat.height;
            portDef->format.video.xFramerate   = videoFormat.framerate << 16;
            portDef->format.video.nStride      = align(portDef->format.video.nFrameWidth, portDef->nBufferAlignment);
            portDef->format.video.eColorFormat = OMX_COLOR_FormatYUV420PackedPlanar;

            setPortDefinition(OPORT_VIDEO, portDef);
            //setFramerate(videoFormat.framerate);
        }

        void setFramerate(unsigned fps)
        {
            Parameter<OMX_CONFIG_FRAMERATETYPE> framerate;
            framerate->xEncodeFramerate = fps << 16;
            framerate->nPortIndex = OPORT_VIDEO;

            ERR_OMX( OMX_SetConfig(component_, OMX_IndexConfigVideoFramerate, &framerate), "set framerate");
        }

        // -100 .. 100
        void setSharpness(OMX_U32 nPortIndex = OMX_ALL, OMX_S32 nSharpness = 0)
        {
            Parameter<OMX_CONFIG_SHARPNESSTYPE> sharpness;
            sharpness->nPortIndex = nPortIndex;
            sharpness->nSharpness = nSharpness;

            ERR_OMX( OMX_SetConfig(component_, OMX_IndexConfigCommonSharpness, &sharpness), "set camera sharpness");
        }

        // -100 .. 100
        void setContrast(OMX_U32 nPortIndex = OMX_ALL, OMX_S32 nContrast = 0)
        {
            Parameter<OMX_CONFIG_CONTRASTTYPE> contrast;
            contrast->nPortIndex = nPortIndex;
            contrast->nContrast = nContrast;

            ERR_OMX( OMX_SetConfig(component_, OMX_IndexConfigCommonContrast, &contrast), "set camera contrast");
        }

        // -100 .. 100
        void setSaturation(OMX_U32 nPortIndex = OMX_ALL, OMX_S32 nSaturation = 0)
        {
            Parameter<OMX_CONFIG_SATURATIONTYPE> saturation;
            saturation->nPortIndex = nPortIndex;
            saturation->nSaturation = nSaturation;

            ERR_OMX( OMX_SetConfig(component_, OMX_IndexConfigCommonSaturation, &saturation), "set camera saturation");
        }

        // 0 .. 100
        void setBrightness(OMX_U32 nPortIndex = OMX_ALL, OMX_U32 nBrightness = 50)
        {
            Parameter<OMX_CONFIG_BRIGHTNESSTYPE> brightness;
            brightness->nPortIndex = nPortIndex;
            brightness->nBrightness = nBrightness;

            ERR_OMX( OMX_SetConfig(component_, OMX_IndexConfigCommonBrightness, &brightness), "set camera brightness");
        }

        void setExposureValue(OMX_U32 nPortIndex = OMX_ALL,
                OMX_S32 xEVCompensation = 0, OMX_U32 nSensitivity = 100, OMX_BOOL bAutoSensitivity = OMX_FALSE)
        {
            Parameter<OMX_CONFIG_EXPOSUREVALUETYPE> exposure_value;
            exposure_value->nPortIndex = nPortIndex;
            exposure_value->xEVCompensation = xEVCompensation;
            exposure_value->nSensitivity = nSensitivity;
            exposure_value->bAutoSensitivity = bAutoSensitivity;

            ERR_OMX( OMX_SetConfig(component_, OMX_IndexConfigCommonExposureValue, &exposure_value), "set camera exposure value");
        }

        void setFrameStabilisation(OMX_U32 nPortIndex = OMX_ALL, OMX_BOOL bStab = OMX_TRUE)
        {
            Parameter<OMX_CONFIG_FRAMESTABTYPE> frame_stabilisation_control;
            frame_stabilisation_control->nPortIndex = nPortIndex;
            frame_stabilisation_control->bStab = bStab;

            ERR_OMX( OMX_SetConfig(component_, OMX_IndexConfigCommonFrameStabilisation, &frame_stabilisation_control),
                     "set camera frame stabilisation");
        }

        void setWhiteBalanceControl(OMX_U32 nPortIndex = OMX_ALL, OMX_WHITEBALCONTROLTYPE eWhiteBalControl = OMX_WhiteBalControlAuto)
        {
            Parameter<OMX_CONFIG_WHITEBALCONTROLTYPE> white_balance_control;
            white_balance_control->nPortIndex = nPortIndex;
            white_balance_control->eWhiteBalControl = eWhiteBalControl;

            ERR_OMX( OMX_SetConfig(component_, OMX_IndexConfigCommonWhiteBalance, &white_balance_control),
                     "set camera frame white balance");
        }

        void setImageFilter(OMX_U32 nPortIndex = OMX_ALL, OMX_IMAGEFILTERTYPE eImageFilter = OMX_ImageFilterNone)
        {
            Parameter<OMX_CONFIG_IMAGEFILTERTYPE> image_filter;
            image_filter->nPortIndex = nPortIndex;
            image_filter->eImageFilter = eImageFilter;

            ERR_OMX( OMX_SetConfig(component_, OMX_IndexConfigCommonImageFilter, &image_filter), "set camera image filter");
        }

        // OMX_MirrorHorizontal | OMX_MirrorVertical | OMX_MirrorBoth
        void setMirror(OMX_U32 nPortIndex, OMX_MIRRORTYPE eMirror = OMX_MirrorNone)
        {
            Parameter<OMX_CONFIG_MIRRORTYPE> mirror;
            mirror->nPortIndex = nPortIndex;
            mirror->eMirror = eMirror;

            ERR_OMX( OMX_SetConfig(component_, OMX_IndexConfigCommonMirror, &mirror), "set cammera mirror");
        }

        void setImageDefaults()
        {
            setSharpness();
            setContrast();
            setSaturation();
            setBrightness();
            setExposureValue();
            setFrameStabilisation();
            setWhiteBalanceControl();
            setImageFilter();
            //setMirror();
        }

        void capture(OMX_U32 nPortIndex, OMX_BOOL bEnabled)
        {
            Parameter<OMX_CONFIG_PORTBOOLEANTYPE> capture;
            capture->nPortIndex = nPortIndex;
            capture->bEnabled = bEnabled;

            ERR_OMX( OMX_SetParameter(component_, OMX_IndexConfigPortCapturing, &capture), "switch capture on port");
        }

        void allocBuffers()
        {
            Component::allocBuffers(IPORT, bufferIn_);
        }

        void freeBuffers()
        {
            Component::freeBuffers(IPORT, bufferIn_);
        }

        bool ready() const { return ready_; }

        void eventReady()
        {
            Lock lock(pSemaphore); // LOCK

            ready_ = true;
        }

    private:
        Buffer bufferIn_;
        bool ready_;
    };

    ///
    class Encoder : public Component
    {
    public:
        static const ComponentType cType = broadcom::VIDEO_ENCODER;

        static const unsigned IPORT = 200;
        static const unsigned OPORT = 201;

        Encoder()
        :   Component(cType, (OMX_PTR) this, &cbsEvents)
        {
        }

        void setupOutputPort(const Parameter<OMX_PARAM_PORTDEFINITIONTYPE>& cameraPortDef, unsigned bitrate, unsigned framerate = 0)
        {
            Parameter<OMX_PARAM_PORTDEFINITIONTYPE> portDef;
            getPortDefinition(OPORT, portDef);

            portDef->format.video.nFrameWidth  = cameraPortDef->format.video.nFrameWidth;
            portDef->format.video.nFrameHeight = cameraPortDef->format.video.nFrameHeight;
            portDef->format.video.xFramerate   = cameraPortDef->format.video.xFramerate;
            portDef->format.video.nStride      = cameraPortDef->format.video.nStride;
            portDef->format.video.nBitrate     = bitrate;

            if (framerate)
                portDef->format.video.xFramerate = framerate;

            setPortDefinition(OPORT, portDef);
        }

        void setCodec(OMX_VIDEO_CODINGTYPE codec)
        {
            Parameter<OMX_VIDEO_PARAM_PORTFORMATTYPE> format;
            format->nPortIndex = OPORT;
            format->eCompressionFormat = codec;

            ERR_OMX( OMX_SetParameter(component_, OMX_IndexParamVideoPortFormat, &format), "set video format");
        }

        void setBitrate(OMX_U32 bitrate, OMX_VIDEO_CONTROLRATETYPE type = OMX_Video_ControlRateVariable)
        {
            Parameter<OMX_VIDEO_PARAM_BITRATETYPE> brate;
            brate->nPortIndex = OPORT;
            brate->eControlRate = type;
            brate->nTargetBitrate = bitrate;

            ERR_OMX( OMX_SetParameter(component_, OMX_IndexParamVideoBitrate, &brate), "set bitrate");
        }

        void allocBuffers()
        {
            Component::allocBuffers(OPORT, bufferOut_);
        }

        void freeBuffers()
        {
            Component::freeBuffers(OPORT, bufferOut_);
        }

        void callFillThisBuffer()
        {
            Component::callFillThisBuffer(bufferOut_);
        }

        Buffer& outBuffer() { return bufferOut_; }

    private:
        Parameter<OMX_PARAM_PORTDEFINITIONTYPE> encoderPortDef_;
        Buffer bufferOut_;
    };

    ///
    class NullSink : public Component
    {
    public:
        static const ComponentType cType = broadcom::NULL_SINK;

        static const unsigned IPORT = 240;

        NullSink()
        :   Component(cType, (OMX_PTR) this, &cbsEvents)
        {
        }
    };

    ///
    class VideoSplitter : public Component
    {
    public:
        static const ComponentType cType = broadcom::VIDEO_SPLITTER;

        static const unsigned IPORT = 250;
        static const unsigned OPORT_1 = 251;
        static const unsigned OPORT_2 = 252;
        static const unsigned OPORT_3 = 253;
        static const unsigned OPORT_4 = 254;

        VideoSplitter()
        :   Component(cType, (OMX_PTR) this, &cbsEvents)
        {
        }
    };

    ///
    class Resizer : public Component
    {
    public:
        static const ComponentType cType = broadcom::RESIZER;

        static const unsigned IPORT = 60;
        static const unsigned OPORT = 61;

        Resizer()
        :   Component(cType, (OMX_PTR) this, &cbsEvents)
        {
        }

        void setupOutputPort(unsigned newWidth, unsigned newHeight)
        {
            Parameter<OMX_PARAM_PORTDEFINITIONTYPE> portDef;
            getPortDefinition(IPORT, portDef);

            portDef->format.image.nFrameWidth = newWidth;
            portDef->format.image.nFrameHeight = newHeight;
            portDef->format.image.eColorFormat = OMX_COLOR_FormatYUV420PackedPlanar;

            portDef->format.image.nSliceHeight = 0;
            portDef->format.image.nStride = 0;

            setPortDefinition(OPORT, portDef);
        }
#if 0
        void resize()
        {
            Parameter<OMX_PARAM_RESIZETYPE> resize;
            resize->nPortIndex = OPORT;
            resize->eMode = OMX_RESIZE_NONE; // OMX_RESIZE_BOX;
            resize->nMaxWidth = frameWidth;
            resize->nMaxHeight = frameHeight;
            resize->bPreserveAspectRatio = OMX_TRUE;

            ERR_OMX( OMX_SetParameter(component_, OMX_IndexParamResize, &resize), "resize");
        }
#endif
    };

    //

    static OMX_ERRORTYPE callback_EventHandler(
            OMX_HANDLETYPE hComponent,
            OMX_PTR pAppData,
            OMX_EVENTTYPE eEvent,
            OMX_U32 nData1,
            OMX_U32 nData2,
            OMX_PTR pEventData)
    {
        Component * component = static_cast<Component *>(pAppData);

        printEvent(component->name(), hComponent, eEvent, nData1, nData2);

        switch (eEvent)
        {
            case OMX_EventCmdComplete:
            {
                switch (nData1)
                {
                    case OMX_CommandFlush:
                    case OMX_CommandPortDisable:
                    case OMX_CommandPortEnable:
                    case OMX_CommandMarkBuffer:
                        // nData2 is port index
                        component->eventCmdComplete(nData1, nData2);
                        break;

                    case OMX_CommandStateSet:
                        // nData2 is state
                        component->eventCmdComplete(nData1, nData2);
                        break;

                    default:
                        break;
                }

                break;
            }

            case OMX_EventPortSettingsChanged:
            {
                // nData1 is port index
                component->eventPortSettingsChanged(nData1);
                break;
            }

            // vendor specific
            case OMX_EventParamOrConfigChanged:
            {
                if (nData2 == OMX_IndexParamCameraDeviceNumber)
                {
                    Camera * camera = static_cast<Camera *>(pAppData);
                    camera->eventReady();
                }

                break;
            }

            case OMX_EventError:
                OMXExeption::die( (OMX_ERRORTYPE) nData1, "OMX_EventError received");
                break;

            case OMX_EventMark:
            case OMX_EventResourcesAcquired:
            case OMX_EventBufferFlag:
            default:
                break;
        }

        return OMX_ErrorNone;
    }

    static OMX_ERRORTYPE callback_EmptyBufferDone(OMX_HANDLETYPE, OMX_PTR, OMX_BUFFERHEADERTYPE *)
    {
        return OMX_ErrorNone;
    }

    static OMX_ERRORTYPE callback_FillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE * /*pBuffer*/)
    {
        Component * component = static_cast<Component *>(pAppData);

        if (component->type() == Encoder::cType)
        {
            Encoder * encoder = static_cast<Encoder *>(pAppData);
            encoder->outBuffer().setFilled();
        }

        return OMX_ErrorNone;
    }
}


// Global variable used by the signal handler and capture/encoding loop
static int want_quit = 0;

#if 1
// Global signal handler for trapping SIGINT, SIGTERM, and SIGQUIT
static void signal_handler(int signal)
{
    want_quit = 1;
}
#endif

using namespace rpi_omx;

static const unsigned VIDEO_BITRATE_HIGH = 6000000;
static const unsigned VIDEO_BITRATE_LOW = 1000000;


int main(int argc, char **argv)
{
    bcm_host_init();

    try
    {
        OMXInit omx;
        VcosSemaphore sem("common semaphore");
        pSemaphore = &sem;

        Camera camera;
        VideoSplitter vsplitter;
        Resizer resizer;
        Encoder encoderHigh;
        Encoder encoderLow;

        // configuring camera
        {
            camera.setVideoFromat(VF_1280x960);
            //camera.setVideoFromat(VF_1920x1080);
            camera.setImageDefaults();
            camera.setImageFilter(OMX_ALL, OMX_ImageFilterNoise);

            while (!camera.ready())
            {
                std::cerr << "waiting for camera..." << std::endl;
                usleep(10000);
            }
        }

        // configuring encoders
        {
            VideoFromat vfResized = VF_RESIZED_640x480;
            //VideoFromat vfResized = VF_RESIZED_320x240;
            //VideoFromat vfResized = VF_RESIZED_256x192;
            //VideoFromat vfResized = VF_RESIZED_160x120;
            //VideoFromat vfResized = VF_RESIZED_128x96;

            //VideoFromat vfResized = VF_RESIZED_960x540;
            //VideoFromat vfResized = VF_RESIZED_640x360;
            //VideoFromat vfResized = VF_RESIZED_480x270;
            //VideoFromat vfResized = VF_RESIZED_384x216;
            //VideoFromat vfResized = VF_RESIZED_320x180;
            //VideoFromat vfResized = VF_RESIZED_240x135;

            Parameter<OMX_PARAM_PORTDEFINITIONTYPE> portDef;
            camera.getPortDefinition(Camera::OPORT_VIDEO, portDef);

            // high
            encoderHigh.setupOutputPort(portDef, VIDEO_BITRATE_HIGH);
            encoderHigh.setCodec(OMX_VIDEO_CodingAVC);

            // resizer
            resizer.setupOutputPort(vfResized.width, vfResized.height);

            // low
            portDef->format.video.nFrameWidth = vfResized.width;
            portDef->format.video.nFrameHeight = vfResized.height;

            encoderLow.setupOutputPort(portDef, VIDEO_BITRATE_LOW);
            encoderLow.setCodec(OMX_VIDEO_CodingAVC);
        }

        // disable proprietary
        {
            Parameter<OMX_PARAM_BRCMDISABLEPROPRIETARYTUNNELSTYPE> dpt;
            dpt->bUseBuffers = OMX_TRUE;
#if 0
            dpt->nPortIndex = Camera::OPORT_VIDEO;
            ERR_OMX( OMX_SetParameter(camera.component(), OMX_IndexParamBrcmDisableProprietaryTunnels, &dpt),
                     "camera DisableProprietaryTunnels");
#endif
#if 0
            dpt->nPortIndex = Resizer::OPORT;
            ERR_OMX( OMX_SetParameter(resizer.component(), OMX_IndexParamBrcmDisableProprietaryTunnels, &dpt),
                     "resizer DisableProprietaryTunnels");
#endif
#if 0
            dpt->nPortIndex = VideoSplitter::OPORT_1;
            ERR_OMX( OMX_SetParameter(vsplitter.component(), OMX_IndexParamBrcmDisableProprietaryTunnels, &dpt),
                     "splitter DisableProprietaryTunnels");
            dpt->nPortIndex = VideoSplitter::OPORT_2;
            ERR_OMX( OMX_SetParameter(vsplitter.component(), OMX_IndexParamBrcmDisableProprietaryTunnels, &dpt),
                     "splitter DisableProprietaryTunnels");
            dpt->nPortIndex = VideoSplitter::OPORT_3;
            ERR_OMX( OMX_SetParameter(vsplitter.component(), OMX_IndexParamBrcmDisableProprietaryTunnels, &dpt),
                     "splitter DisableProprietaryTunnels");
            dpt->nPortIndex = VideoSplitter::OPORT_4;
            ERR_OMX( OMX_SetParameter(vsplitter.component(), OMX_IndexParamBrcmDisableProprietaryTunnels, &dpt),
                     "splitter DisableProprietaryTunnels");
#endif
        }

        // tunneling ports
        {
#if 0
            ERR_OMX( OMX_SetupTunnel(camera.component(), Camera::OPORT_VIDEO, encoderHigh.component(), Encoder::IPORT),
                "tunnel camera.video -> encoder.input");
#else
            ERR_OMX( OMX_SetupTunnel(camera.component(), Camera::OPORT_VIDEO, vsplitter.component(), VideoSplitter::IPORT),
                "tunnel camera.video -> splitter.input");

            ERR_OMX( OMX_SetupTunnel(vsplitter.component(), VideoSplitter::OPORT_1, encoderHigh.component(), Encoder::IPORT),
                "tunnel splitter.output -> encoder.input (high)");

            //ERR_OMX( OMX_SetupTunnel(vsplitter.component(), VideoSplitter::OPORT_2, encoderLow.component(), Encoder::IPORT),
            //    "tunnel splitter.output -> encoder.input (low)");

            ERR_OMX( OMX_SetupTunnel(vsplitter.component(), VideoSplitter::OPORT_2, resizer.component(), Resizer::IPORT),
                "tunnel splitter.output -> resizer.input");

            ERR_OMX( OMX_SetupTunnel(resizer.component(), Resizer::OPORT, encoderLow.component(), Encoder::IPORT),
                "tunnel resizer.output -> encoder.input (low)");
#endif
        }

        // switch components to idle state
        {
            camera.switchState(OMX_StateIdle);

            vsplitter.switchState(OMX_StateIdle);
            resizer.switchState(OMX_StateIdle);

            encoderHigh.switchState(OMX_StateIdle);
            encoderLow.switchState(OMX_StateIdle);
        }

        // enable ports
        {
            camera.enablePort(Camera::IPORT);
            camera.enablePort(Camera::OPORT_VIDEO);

            vsplitter.enablePort(VideoSplitter::IPORT);
            vsplitter.enablePort(VideoSplitter::OPORT_1);
            vsplitter.enablePort(VideoSplitter::OPORT_2);

            resizer.enablePort();       // all
            encoderHigh.enablePort();   // all
            encoderLow.enablePort();    // all
        }

        // allocate buffers
        {
            camera.allocBuffers();
            encoderHigh.allocBuffers();
            encoderLow.allocBuffers();
        }

        // switch state of the components prior to starting
        {
            camera.switchState(OMX_StateExecuting);
            vsplitter.switchState(OMX_StateExecuting);
            resizer.switchState(OMX_StateExecuting);
            encoderLow.switchState(OMX_StateExecuting);
            encoderHigh.switchState(OMX_StateExecuting);
        }

        // start capturing video with the camera
        {
            camera.capture(Camera::OPORT_VIDEO, OMX_TRUE);
        }

#if 0
        // dump (counfigured) ports
        {
            camera.dumpPort(Camera::IPORT, OMX_FALSE);
            camera.dumpPort(Camera::OPORT_VIDEO, OMX_FALSE);

            vsplitter.dumpPort(VideoSplitter::IPORT, OMX_FALSE);
            vsplitter.dumpPort(VideoSplitter::OPORT_1, OMX_FALSE);
            vsplitter.dumpPort(VideoSplitter::OPORT_2, OMX_FALSE);

            resizer.dumpPort(Resizer::IPORT);
            resizer.dumpPort(Resizer::OPORT);

            encoderHigh.dumpPort(Encoder::IPORT, OMX_FALSE);
            encoderHigh.dumpPort(Encoder::OPORT, OMX_FALSE);

            encoderLow.dumpPort(Encoder::IPORT, OMX_FALSE);
            encoderLow.dumpPort(Encoder::OPORT, OMX_FALSE);
        }
#endif

#if 1
        signal(SIGINT,  signal_handler);
        signal(SIGTERM, signal_handler);
        signal(SIGQUIT, signal_handler);
#endif

#if 1
        FILE * outHigh = fopen("x.h264", "w+");
        FILE * outLow = fopen("y.h264", "w+");
        if (!outHigh || !outLow)
            throw "Can't open output file";
#endif

        std::cerr << "Enter capture and encode loop, press Ctrl-C to quit..." << std::endl;

        unsigned highCount = 0;
        unsigned lowCount = 0;
        unsigned noDataCount = 0;
        Buffer& encBufferHigh = encoderHigh.outBuffer();
        Buffer& encBufferLow = encoderLow.outBuffer();

        encoderHigh.callFillThisBuffer();
        encoderLow.callFillThisBuffer();

        while (1)
        {
            bool aval = false;
            if (encBufferHigh.filled())
            {
                aval = true;
                noDataCount = 0;

                // Don't exit the loop until we are certain that we have processed
                // a full frame till end of the frame, i.e. we're at the end
                // of the current key frame if processing one or until
                // the next key frame is detected. This way we should always
                // avoid corruption of the last encoded at the expense of
                // small delay in exiting.
                if (want_quit && (encBufferHigh.flags() & OMX_BUFFERFLAG_SYNCFRAME))
                {
                    std::cerr << "Key frame boundry reached, exiting loop..." << std::endl;
                    break;
                }

                // Flush buffer to output file
                unsigned toWrite = encBufferHigh.dataSize();
                if (toWrite)
                {
                    ++highCount;
#if 1
                    size_t outWritten = fwrite(encBufferHigh.data(), 1, toWrite, outHigh);
                    if (outWritten != toWrite)
                    {
                        std::cerr << "Failed to write to output file: " << strerror(errno) << std::endl;
                        break;
                    }
#endif
                    std::cerr << "[High] buffer -> out file: "
                        << encBufferHigh.dataSize() << "/" << encBufferHigh.allocSize() << std::endl;
                }

                // Buffer flushed, request a new buffer to be filled by the encoder component
                encBufferHigh.setFilled(false);
                encoderHigh.callFillThisBuffer();
            }

            if (encBufferLow.filled())
            {
                aval = true;
                noDataCount = 0;

                // Don't exit the loop until we are certain that we have processed
                // a full frame till end of the frame, i.e. we're at the end
                // of the current key frame if processing one or until
                // the next key frame is detected. This way we should always
                // avoid corruption of the last encoded at the expense of
                // small delay in exiting.
                if (want_quit && (encBufferLow.flags() & OMX_BUFFERFLAG_SYNCFRAME))
                {
                    std::cerr << "Key frame boundry reached, exiting loop..." << std::endl;
                    break;
                }

                // Flush buffer to output file
                unsigned toWrite = encBufferLow.dataSize();
                if (toWrite)
                {
                    ++lowCount;
#if 1
                    size_t outWritten = fwrite(encBufferLow.data(), 1, toWrite, outLow);
                    if (outWritten != toWrite)
                    {
                        std::cerr << "Failed to write to output file: " << strerror(errno) << std::endl;
                        break;
                    }
#endif
                    std::cerr << "[Low] buffer -> out file: "
                        << encBufferLow.dataSize() << "/" << encBufferLow.allocSize() << std::endl;
                }

                // Buffer flushed, request a new buffer to be filled by the encoder component
                encBufferLow.setFilled(false);
                encoderLow.callFillThisBuffer();
            }

            if (!aval)
            {
                if (want_quit)
                {
                    ++noDataCount;
                    if (noDataCount > 1000)
                    {
                        std::cerr << "" << std::endl;
                        break;
                    }
                }

                usleep(1000);
            }
        }

        std::cerr << "high: " << highCount << " low: " << lowCount << std::endl;
#if 1
        if (outHigh)
            fclose(outHigh);
        if (outLow)
            fclose(outLow);
#endif

#if 1
        // Restore signal handlers
        signal(SIGINT,  SIG_DFL);
        signal(SIGTERM, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
#endif

        // stop capturing video with the camera
        {
            camera.capture(Camera::OPORT_VIDEO, OMX_FALSE);
        }

        // return the last full buffer back to the encoder component
        {
            encoderHigh.outBuffer().flags() &= OMX_BUFFERFLAG_EOS;
            encoderLow.outBuffer().flags() &= OMX_BUFFERFLAG_EOS;

            encoderHigh.callFillThisBuffer();
            encoderLow.callFillThisBuffer();
        }

        // flush the buffers on each component
        {
            camera.flushPort();

            vsplitter.flushPort();
            resizer.flushPort();

            encoderHigh.flushPort();
            encoderLow.flushPort();
        }

        // disable all the ports
        {
            camera.disablePort();

            vsplitter.disablePort();
            resizer.disablePort();

            encoderHigh.disablePort();
            encoderLow.disablePort();
        }

        // free all the buffers
        {
            camera.freeBuffers();

            encoderHigh.freeBuffers();
            encoderLow.freeBuffers();
        }

        // transition all the components to idle states
        {
            camera.switchState(OMX_StateIdle);

            vsplitter.switchState(OMX_StateIdle);
            resizer.switchState(OMX_StateIdle);

            encoderHigh.switchState(OMX_StateIdle);
            encoderLow.switchState(OMX_StateIdle);
        }

        // transition all the components to loaded states
        {
            camera.switchState(OMX_StateLoaded);

            vsplitter.switchState(OMX_StateLoaded);
            resizer.switchState(OMX_StateLoaded);

            encoderHigh.switchState(OMX_StateLoaded);
            encoderLow.switchState(OMX_StateLoaded);
        }
    }
    catch (const OMXExeption& e)
    {
        OMXExeption::die(e.code(), e.what());
    }
    catch (const char * msg)
    {
        std::cerr << msg;
        return 1;
    }

    return 0;
}
