#include <assert.h>
#include <jni.h>
#include <string.h>
#include <pthread.h>
#include <vector>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#define LOG_TAG "OpenSLExample"

#define LOG_PRINT(LEVEL_, LOG_TAG_, ...) __android_log_print(LEVEL_, LOG_TAG_, __VA_ARGS__)

#define LOGV(...) LOG_PRINT(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) LOG_PRINT(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) LOG_PRINT(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) LOG_PRINT(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) LOG_PRINT(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGF(...) LOG_PRINT(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)

#define PREFETCHSTATUS_NONE ((SLuint32)0)
#define BUFFER_BLOCK_SIZE   (1024)


struct Metadata {
    SLuint32 numChannels;
    SLuint32 samplesPerSec;
    SLuint32 bitsPerSample;
    SLuint32 containerSize;
    SLuint32 channelMask;
    SLuint32 endianness;
    SLmillisecond duration;

    Metadata() :
            numChannels(0), samplesPerSec(0), bitsPerSample(0),
            containerSize(0), channelMask(0), endianness(0), duration(0) {
    }
};

struct BufferBlockInfo {
    void *buffer;
    SLuint32 containerSizeInBytes;
    SLuint32 bufferSizeInBytes;
    SLuint32 numChannels;
    SLuint32 count;
};

class SLAppContext {
    const SLuint32 NUM_BLOCKS = 2;

public:
    SLAppContext() :
            engineObj_(nullptr),
            engineItf_(nullptr),
            playerObj_(nullptr),
            playItf_(nullptr),
            seekItf_(nullptr),
            prefetchStatusItf_(nullptr),
            bufferQueueItf_(nullptr),
            metadataExtractionItf_(nullptr),
            prefetch_level_(0),
            prefetch_status_(0),
            decode_finished_(false),
            decoded_block_count_(0),
            bufferPool_(),
            bufferBlocks_()
    {
        ::pthread_mutex_init(&mutex_, nullptr);
        ::pthread_cond_init(&cond_, nullptr);
    }

    ~SLAppContext() {
        destroyObjects();
        ::pthread_mutex_destroy(&mutex_);
        ::pthread_cond_destroy(&cond_);
    }

    void decode(int fd, off_t start, off_t length) {
        LOGD("createEngine()");
        createEngine();

        LOGD("createPlayerForDecodingFd()");
        createPlayerForDecodingFd(fd, start, length);

        LOGD("setupPlayerCallbacks()");
        setupPlayerCallbacks();

        LOGD("startPrefetch()");
        startPrefetch();

        LOGD("waitForPrefetchCompletion()");
        waitForPrefetchCompletion();

        LOGD("extractMetadata()");
        Metadata metadata = extractMetadata();

        LOGD("prepareBufferQueue()");
        prepareBufferQueue(metadata);

        LOGD("startDecoding()");
        startDecoding();

        LOGD("waitForDecodeCompletion()");
        waitForDecodeCompletion();

        LOGD("stopDecoding()");
        stopDecoding();

        LOGD("printDecodeResults");
        printDecodeResults(metadata);

        LOGD("destroyObjects()");
        destroyObjects();
    }


private:
    void destroyObjects() {
        if (playerObj_) {
            (*playerObj_)->Destroy(playerObj_);
            playerObj_ = nullptr;
            playItf_ = nullptr;
            seekItf_ = nullptr;
            bufferQueueItf_ = nullptr;
            prefetchStatusItf_ = nullptr;
            metadataExtractionItf_ = nullptr;
        }

        if (engineObj_) {
            (*engineObj_)->Destroy(engineObj_);
            engineObj_ = nullptr;
            engineItf_ = nullptr;
        }
    }

    void createEngine() {
        SLresult result;

        SLObjectItf engineObj = nullptr;
        SLEngineItf engineItf = nullptr;

        // create engine
        result = ::slCreateEngine(&engineObj, 0, nullptr, 0, nullptr, nullptr);
        assert(SL_RESULT_SUCCESS == result);

        // realize the engine
        result = (*engineObj)->Realize(engineObj, SL_BOOLEAN_FALSE);
        assert(SL_RESULT_SUCCESS == result);

        // get the engine interface, which is needed in order to create other objects
        result = (*engineObj)->GetInterface(engineObj, SL_IID_ENGINE, &engineItf);
        assert(SL_RESULT_SUCCESS == result);

        this->engineObj_ = engineObj;
        this->engineItf_ = engineItf;
    }

    void createPlayerForDecodingFd(int fd, off_t start, off_t length) {
        SLresult result;

        SLObjectItf playerObj = nullptr;
        SLPlayItf playItf = nullptr;
        SLSeekItf seekItf = nullptr;
        SLAndroidSimpleBufferQueueItf bufferQueueItf = nullptr;
        SLPrefetchStatusItf prefetchStatusItf = nullptr;
        SLMetadataExtractionItf metadataExtractionItf = nullptr;

        // configure audio source
        SLDataLocator_AndroidFD srcLoc = {SL_DATALOCATOR_ANDROIDFD, fd, start, length};
        SLDataFormat_MIME srcFmtMime = {SL_DATAFORMAT_MIME, nullptr, SL_CONTAINERTYPE_UNSPECIFIED};
        SLDataSource audioSrc = {&srcLoc, &srcFmtMime};

        // configure audio sink
        SLDataLocator_AndroidSimpleBufferQueue sinkLoc = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                          NUM_BLOCKS};
        SLDataFormat_PCM sinkFmtPcm = {};
        SLDataSink audioSink = {&sinkLoc, &sinkFmtPcm};

        sinkFmtPcm.formatType = SL_DATAFORMAT_PCM;
        sinkFmtPcm.numChannels = 2;
        sinkFmtPcm.samplesPerSec = SL_SAMPLINGRATE_44_1;
        sinkFmtPcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
        sinkFmtPcm.containerSize = 16;
        sinkFmtPcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
        sinkFmtPcm.endianness = SL_BYTEORDER_LITTLEENDIAN;


        // create audio player
        const SLInterfaceID ids[] = {SL_IID_SEEK, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                     SL_IID_PREFETCHSTATUS, SL_IID_METADATAEXTRACTION};
        const SLboolean req[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
                                 SL_BOOLEAN_TRUE};

        result = (*engineItf_)->CreateAudioPlayer(
                engineItf_, &playerObj, &audioSrc, &audioSink,
                sizeof(ids) / sizeof(ids[0]), ids, req);

        assert(SL_RESULT_SUCCESS == result);

        // realize the player
        result = (*playerObj)->Realize(playerObj, SL_BOOLEAN_FALSE);
        assert(SL_RESULT_SUCCESS == result);

        // get the play interface
        result = (*playerObj)->GetInterface(playerObj, SL_IID_PLAY, &playItf);
        assert(SL_RESULT_SUCCESS == result);

        // get the seek interface
        result = (*playerObj)->GetInterface(playerObj, SL_IID_SEEK, &seekItf);
        assert(SL_RESULT_SUCCESS == result);

        // get the simple buffer queue interface
        result = (*playerObj)->GetInterface(playerObj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                            &bufferQueueItf);
        assert(SL_RESULT_SUCCESS == result);

        // get the prefetch status interface
        result = (*playerObj)->GetInterface(playerObj, SL_IID_PREFETCHSTATUS, &prefetchStatusItf);
        assert(SL_RESULT_SUCCESS == result);

        // get the metadata extraction interface
        result = (*playerObj)->GetInterface(playerObj, SL_IID_METADATAEXTRACTION,
                                            &metadataExtractionItf);
        assert(SL_RESULT_SUCCESS == result);

        this->playerObj_ = playerObj;
        this->playItf_ = playItf;
        this->seekItf_ = seekItf;
        this->bufferQueueItf_ = bufferQueueItf;
        this->prefetchStatusItf_ = prefetchStatusItf;
        this->metadataExtractionItf_ = metadataExtractionItf;
    }

    void setupPlayerCallbacks() {
        SLresult result;

        result = (*playItf_)->SetCallbackEventsMask(
                playItf_,
                SL_PLAYEVENT_HEADATEND |
                SL_PLAYEVENT_HEADATMARKER |
                SL_PLAYEVENT_HEADATNEWPOS |
                SL_PLAYEVENT_HEADMOVING |
                SL_PLAYEVENT_HEADSTALLED);
        assert(SL_RESULT_SUCCESS == result);

        result = (*playItf_)->RegisterCallback(
                playItf_, playInterfaceCallback, this);
        assert(SL_RESULT_SUCCESS == result);

        result = (*bufferQueueItf_)->RegisterCallback(bufferQueueItf_, bufferQueueCallback, this);
        assert(SL_RESULT_SUCCESS == result);

        result = (*prefetchStatusItf_)->SetCallbackEventsMask(
                prefetchStatusItf_,
                SL_PREFETCHEVENT_STATUSCHANGE | SL_PREFETCHEVENT_FILLLEVELCHANGE);
        assert(SL_RESULT_SUCCESS == result);

        result = (*prefetchStatusItf_)->RegisterCallback(prefetchStatusItf_, prefetchStatusCallback,
                                                         this);
        assert(SL_RESULT_SUCCESS == result);
    }

    void startPrefetch() {
        SLresult result;

        prefetch_level_ = 0;
        prefetch_status_ = PREFETCHSTATUS_NONE;

        // start prefetching
        result = (*playItf_)->SetPlayState(playItf_, SL_PLAYSTATE_PAUSED);
        assert(SL_RESULT_SUCCESS == result);
    }

    void waitForPrefetchCompletion() {
        int pt_result;

        // wait (ref. prefetchStatusCallback())
        pt_result = ::pthread_mutex_lock(&mutex_);
        assert(0 == pt_result);
        while (prefetch_status_ == PREFETCHSTATUS_NONE) {
            pt_result = ::pthread_cond_wait(&cond_, &mutex_);
            assert(0 == pt_result);
        }
        pt_result = ::pthread_mutex_unlock(&mutex_);
        assert(0 == pt_result);
    }

    void printDecodeResults(const Metadata &metadata) {
        LOGD("numChannels     = %d [channels]", metadata.numChannels);
        LOGD("samplesPerSec   = %d [samples/sec.]", metadata.samplesPerSec);
        LOGD("bitsPerSample   = %d [bits]", metadata.bitsPerSample);
        LOGD("containerSize   = %d [bits]", metadata.containerSize);
        LOGD("channelMask     = %d", metadata.channelMask);
        LOGD("endianness      = %d", metadata.endianness);
        LOGD("duration        = %d [ms]", metadata.duration);
        LOGD("decoded samples = %d [samples]", decoded_block_count_ / (BUFFER_BLOCK_SIZE * metadata.numChannels));
    }

    Metadata extractMetadata() {
        SLuint32 itemCount = 0;
        SLresult result;
        Metadata metadata = Metadata();

        std::vector<uint32_t> keyBuff(16);
        std::vector<uint32_t> valueBuff(16);

        // get duration from player
        result = (*playItf_)->GetDuration(playItf_, &(metadata.duration));
        assert(SL_RESULT_SUCCESS == result);

        // get metadata
        result = (*metadataExtractionItf_)->GetItemCount(metadataExtractionItf_, &itemCount);
        assert(SL_RESULT_SUCCESS == result);

        for (SLuint32 i = 0; i < itemCount; ++i) {
            SLuint32 keySize = 0;
            SLuint32 valueSize = 0;

            // get key & value sizes
            result = (*metadataExtractionItf_)->GetKeySize(metadataExtractionItf_, i, &keySize);
            assert(SL_RESULT_SUCCESS == result);

            result = (*metadataExtractionItf_)->GetValueSize(metadataExtractionItf_, i, &valueSize);
            assert(SL_RESULT_SUCCESS == result);

            // resize buffers
            if (keyBuff.size() * sizeof(uint32_t) < keySize) {
                keyBuff.resize((keySize + sizeof(uint32_t) - 1) / sizeof(uint32_t));
            }

            if (valueBuff.size() * sizeof(uint32_t) < valueSize) {
                valueBuff.resize((valueSize + sizeof(uint32_t) - 1) / sizeof(uint32_t));
            }

            // get key & value
            SLMetadataInfo *key = reinterpret_cast<SLMetadataInfo *>(&keyBuff[0]);
            SLMetadataInfo *value = reinterpret_cast<SLMetadataInfo *>(&valueBuff[0]);

            result = (*metadataExtractionItf_)->GetKey(metadataExtractionItf_, i, keySize, key);
            assert(SL_RESULT_SUCCESS == result);

            result = (*metadataExtractionItf_)->GetValue(metadataExtractionItf_, i, valueSize,
                                                         value);
            assert(SL_RESULT_SUCCESS == result);

            // update keySize & valueSize variables
            keySize = key->size;
            valueSize = value->size;

            const char *keyStr = reinterpret_cast<const char *>(key->data);

            const bool sizeIsUint32 = (valueSize) == sizeof(SLuint32);
            union slu32u8_t {
                SLuint32 u32;
                SLuint8 u8[4];
            };
            const slu32u8_t *u32value = reinterpret_cast<const slu32u8_t *>(&(value->data[0]));

            if (sizeIsUint32 && ::strncmp(keyStr, ANDROID_KEY_PCMFORMAT_NUMCHANNELS, keySize) == 0) {
                metadata.numChannels = u32value->u32;
            } else if (sizeIsUint32 && ::strncmp(keyStr, ANDROID_KEY_PCMFORMAT_SAMPLERATE, keySize) == 0) {
                metadata.samplesPerSec = u32value->u32;
            } else if (sizeIsUint32 && ::strncmp(keyStr, ANDROID_KEY_PCMFORMAT_BITSPERSAMPLE, keySize) == 0) {
                metadata.bitsPerSample = u32value->u32;
            } else if (sizeIsUint32 && ::strncmp(keyStr, ANDROID_KEY_PCMFORMAT_CONTAINERSIZE, keySize) == 0) {
                metadata.containerSize = u32value->u32;
            } else if (sizeIsUint32 && ::strncmp(keyStr, ANDROID_KEY_PCMFORMAT_CHANNELMASK, keySize) == 0) {
                metadata.channelMask = u32value->u32;
            } else if (sizeIsUint32 && ::strncmp(keyStr, ANDROID_KEY_PCMFORMAT_ENDIANNESS, keySize) == 0) {
                metadata.endianness = u32value->u32;
            }
        }

        return metadata;
    }

    void prepareBufferQueue(const Metadata &metadata) {
        assert(metadata.endianness == SL_BYTEORDER_LITTLEENDIAN);
        assert(metadata.bitsPerSample == SL_PCMSAMPLEFORMAT_FIXED_16);
        assert(metadata.containerSize == 16);

        const int block_size = BUFFER_BLOCK_SIZE;
        const SLuint32 numChannels = metadata.numChannels;
        SLresult result;

        bufferPool_.resize(block_size * numChannels * NUM_BLOCKS);

        for (int i = 0; i < NUM_BLOCKS; ++i) {
            BufferBlockInfo block;

            block.containerSizeInBytes = (metadata.containerSize / 8);
            block.count = block_size;
            block.numChannels = numChannels;
            block.bufferSizeInBytes = block.containerSizeInBytes * block.numChannels * block.count;
            block.buffer = &(bufferPool_[block_size * numChannels * i]);

            bufferBlocks_.push_back(block);
        }

        for (int i = 0; i < NUM_BLOCKS; ++i) {
            const BufferBlockInfo &block = bufferBlocks_[i];

            result = (*bufferQueueItf_)->Enqueue(
                    bufferQueueItf_, block.buffer, block.bufferSizeInBytes);

            assert(SL_RESULT_SUCCESS == result);
        }
    }

    void startDecoding() {
        SLresult result;

        decode_finished_ = false;
        decoded_block_count_ = 0;

        result = (*playItf_)->SetPlayState(playItf_, SL_PLAYSTATE_PLAYING);
        assert(SL_RESULT_SUCCESS == result);
    }

    void waitForDecodeCompletion() {
        int pt_result;

        // wait (ref. playInterfaceCallback())
        pt_result = ::pthread_mutex_lock(&mutex_);
        assert(0 == pt_result);
        while (!decode_finished_) {
            struct timeval tv;
            struct timespec timeout;

            gettimeofday(&tv, NULL);

            timeout.tv_sec = tv.tv_sec + 3;
            timeout.tv_nsec = 0;

            pt_result = ::pthread_cond_timedwait(&cond_, &mutex_, &timeout);
            assert(0 == pt_result);  // !!! Android 7+ fails on this assertion with OGG media
        }
        pt_result = ::pthread_mutex_unlock(&mutex_);
        assert(0 == pt_result);
    }

    void stopDecoding() {
        SLresult result;

        result = (*playItf_)->SetPlayState(playItf_, SL_PLAYSTATE_STOPPED);
        assert(SL_RESULT_SUCCESS == result);
    }

    static void playInterfaceCallback(SLPlayItf caller, void *pContext, SLuint32 event) {
        SLAppContext *thiz = static_cast<SLAppContext *>(pContext);

        if (event & SL_PLAYEVENT_HEADATEND) {
            LOGD("playInterfaceCallback(event = HEADATEND)");
        }
        if (event & SL_PLAYEVENT_HEADATMARKER) {
            LOGD("playInterfaceCallback(event = HEADATMARKER)");
        }
        if (event & SL_PLAYEVENT_HEADATNEWPOS) {
            LOGD("playInterfaceCallback(event = HEADATNEWPOS)");
        }
        if (event & SL_PLAYEVENT_HEADMOVING) {
            LOGD("playInterfaceCallback(event = HEADMOVING)");
        }
        if (event & SL_PLAYEVENT_HEADSTALLED) {
            LOGD("playInterfaceCallback(event = HEADSTALLED)");
        }

        if (event & SL_PLAYEVENT_HEADATEND) {
            SLAppContext *thiz = static_cast<SLAppContext *>(pContext);
            int pt_result;

            // notify to waiting thread (= waitForPrefetchCompletion())
            pt_result = ::pthread_mutex_lock(&(thiz->mutex_));
            assert(0 == pt_result);

            thiz->decode_finished_ = true;

            pt_result = ::pthread_cond_signal(&(thiz->cond_));
            assert(0 == pt_result);

            pt_result = ::pthread_mutex_unlock(&(thiz->mutex_));
            assert(0 == pt_result);
        }
    }

    static void bufferQueueCallback(SLAndroidSimpleBufferQueueItf caller, void *pContext) {
        SLAppContext *thiz = static_cast<SLAppContext *>(pContext);

        SLresult result;
        SLAndroidSimpleBufferQueueState state;

        result = (*caller)->GetState(caller, &state);
        assert(result == SL_RESULT_SUCCESS);

        BufferBlockInfo &block = thiz->bufferBlocks_[state.index % thiz->bufferBlocks_.size()];

        // re-enqueue the empty buffer
        (*caller)->Enqueue(caller, block.buffer, block.bufferSizeInBytes);

        {
            // notify to waiting thread (= waitForPrefetchCompletion())
            int pt_result;

            pt_result = ::pthread_mutex_lock(&(thiz->mutex_));
            assert(0 == pt_result);

            thiz->decoded_block_count_ += 1;

            LOGD("bufferQueueCallback(decoded_block_count = %d)", thiz->decoded_block_count_);

            pt_result = ::pthread_cond_signal(&(thiz->cond_));
            assert(0 == pt_result);

            pt_result = ::pthread_mutex_unlock(&(thiz->mutex_));
            assert(0 == pt_result);
        }
    }

    static void prefetchStatusCallback(SLPrefetchStatusItf caller, void *pContext, SLuint32 event) {
        SLAppContext *thiz = static_cast<SLAppContext *>(pContext);

        SLresult result;

        SLpermille level = 0;
        SLuint32 status = 0;

        // Get fill level and prefetch status
        result = (*caller)->GetFillLevel(caller, &level);
        assert(SL_RESULT_SUCCESS == result);

        result = (*caller)->GetPrefetchStatus(caller, &status);
        assert(SL_RESULT_SUCCESS == result);

        // notify to waiting thread (= waitForPrefetchCompletion())
        {
            int pt_result;
            pt_result = ::pthread_mutex_lock(&(thiz->mutex_));
            assert(0 == pt_result);

            thiz->prefetch_level_ = level;
            thiz->prefetch_status_ = status;

            pt_result = ::pthread_cond_signal(&(thiz->cond_));
            assert(0 == pt_result);

            pt_result = ::pthread_mutex_unlock(&(thiz->mutex_));
            assert(0 == pt_result);
        }
    }

private:
    SLObjectItf engineObj_;
    SLEngineItf engineItf_;

    SLObjectItf playerObj_;
    SLPlayItf playItf_;
    SLSeekItf seekItf_;
    SLAndroidSimpleBufferQueueItf bufferQueueItf_;
    SLPrefetchStatusItf prefetchStatusItf_;
    SLMetadataExtractionItf metadataExtractionItf_;

    pthread_cond_t cond_;
    pthread_mutex_t mutex_;
    SLpermille prefetch_level_;
    SLuint32 prefetch_status_;
    bool decode_finished_;
    SLuint32 decoded_block_count_;

    std::vector<SLint16> bufferPool_;
    std::vector<BufferBlockInfo> bufferBlocks_;
};


extern "C" {

jboolean Java_com_h6ah4i_android_example_decodemp3ogg_MainActivity_decodeAssetAudio(
        JNIEnv *env, jclass clazz, jobject assetManager, jstring filename) {

    const char *utf8 = env->GetStringUTFChars(filename, nullptr);
    AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
    assert(NULL != mgr);
    AAsset *asset = AAssetManager_open(mgr, utf8, AASSET_MODE_UNKNOWN);

    // release the Java string and UTF-8
    env->ReleaseStringUTFChars(filename, utf8);

    // the asset might not be found
    if (NULL == asset) {
        return JNI_FALSE;
    }

    // open asset as file descriptor
    off_t start, length;
    int fd = AAsset_openFileDescriptor(asset, &start, &length);
    assert(0 <= fd);
    AAsset_close(asset);

    // perform decoding
    SLAppContext context;
    context.decode(fd, start, length);

    return JNI_TRUE;
}

};
