
MP3 decode extremely slow and OGG decode fails on Android 7+
====

This repository is a demonstration of OpenSL audio decoding feature regression on Android 7.x.
**In shortly, the audio decoding performance is extremely worse on new Android OS, moreover it stopped working with OGG format!**

I discovered these issues while debugging my another project [OpenSLMediaPlayer](https://github.com/h6ah4i/android-openslmediaplayer), but its code base is very large and so complicated. I created this project to report the issue to AOSP bug tracker, so it contains minimal code to reproduce the issue.

- [h6ah4i/android-openslmediaplayer --- Can't play OGG files on Android 7+ #28](https://github.com/h6ah4i/android-openslmediaplayer/issues/28)

## Result summary

| Android OS:       | 4.4.4      | 5.1.0      | 6.0.0      | 7.0.0      | 7.1.1      |
|-------------------|------------|------------|------------|------------|------------|
| MP3 decode        | OK         | OK         | OK         | **SLOW**   | **SLOW**   |
| -> estimated time | 2 sec      | 2 sec      | 2 sec      | **36 sec** | **40 sec** |
| OGG decode        | OK         | OK         | OK         | **FAIL**   | **FAIL**   |
| -> estimated time | 2 sec      | 2 sec      | 2 sec      | ---        | ---        |


*(maybe, I'll add more format test results later.)*

---

**SLOW** : It takes 20x time to decode the whole audio data

**FAIL** : The `bufferQueueCallback` function never be called. Also the following error message is printed on logcat;

```logcat
E SoftVorbis: onQueueFilled, input header has nFilledLen 1, expected 4
E ACodec  : [OMX.google.vorbis.decoder] ERROR(0x80001005)
E ACodec  : signalError(omxError 0x80001005, internalError -2147483648)
E MediaCodec: Codec reported err 0x80001005, actionCode 0, while in state 6
W         : [OMX.google.vorbis.decoder] could not get input buffer #1
E libOpenSLES: MediaSource::read returned error -2147483648
```


## Logcat outputs

### Android 4.4.4 (Genymotion)

<details><summary>Logcat - MP3 (click here to expand)</summary><p>

```logcat
D/OpenSLExample( 1329): createEngine()
D/OpenSLExample( 1329): createPlayerForDecodingFd()
D/OpenSLExample( 1329): setupPlayerCallbacks()
D/OpenSLExample( 1329): startPrefetch()
D/OpenSLExample( 1329): waitForPrefetchCompletion()
E/WVMExtractor( 1329): Failed to open libwvm.so
V/FFmpegExtractor( 1329): SniffFFMPEG
I/FFmpegExtractor( 1329): android-source:0xb7d48ee8
D/FFMPEG  ( 1329): android source begin open
D/FFMPEG  ( 1329): android open, url: android-source:0xb7d48ee8
D/FFMPEG  ( 1329): ffmpeg open android data source success, source ptr: 0xb7d48ee8
D/FFMPEG  ( 1329): android source open success
I/AudioMixer(  256): found effect "Multichannel Downmix To Stereo" from The Android Open Source Project
I/FFMPEG  ( 1329): Input #0, mp3, from 'android-source:0xb7d48ee8':
I/FFMPEG  ( 1329):   Metadata:
I/FFMPEG  ( 1329):     artist          : Glenn Kasten
I/FFMPEG  ( 1329):     album           : Unknown
I/FFMPEG  ( 1329):     title           : Background for talking but don't
I/FFMPEG  ( 1329):     track           : 9
I/FFMPEG  ( 1329):     genre           : Soundtrack
I/FFMPEG  ( 1329):     date            : 2008
I/FFMPEG  ( 1329):   Duration: 00:01:46.32, start: 0.000000, bitrate: 193 kb/s
I/FFMPEG  ( 1329):     Stream #0:0: Audio: mp3, 44100 Hz, stereo, s16p, 193 kb/s
I/FFmpegExtractor( 1329): FFmpegExtrator, url: android-source:0xb7d48ee8, format_name: mp3, format_long_name: MP2/3 (MPEG audio layer 2/3)
I/FFmpegExtractor( 1329): list the formats suppoted by ffmpeg:
I/FFmpegExtractor( 1329): ========================================
V/FFmpegExtractor( 1329): format_names[00]: mpeg
V/FFmpegExtractor( 1329): format_names[01]: mpegts
V/FFmpegExtractor( 1329): format_names[02]: mov,mp4,m4a,3gp,3g2,mj2
V/FFmpegExtractor( 1329): format_names[03]: matroska,webm
V/FFmpegExtractor( 1329): format_names[04]: asf
V/FFmpegExtractor( 1329): format_names[05]: rm
V/FFmpegExtractor( 1329): format_names[06]: flv
V/FFmpegExtractor( 1329): format_names[07]: swf
V/FFmpegExtractor( 1329): format_names[08]: avi
V/FFmpegExtractor( 1329): format_names[09]: ape
V/FFmpegExtractor( 1329): format_names[10]: dts
V/FFmpegExtractor( 1329): format_names[11]: flac
V/FFmpegExtractor( 1329): format_names[12]: ac3
V/FFmpegExtractor( 1329): format_names[13]: wav
V/FFmpegExtractor( 1329): format_names[14]: ogg
V/FFmpegExtractor( 1329): format_names[15]: vc1
V/FFmpegExtractor( 1329): format_names[16]: hevc
I/FFmpegExtractor( 1329): ========================================
D/FFMPEG  ( 1329): android source close
W/FFmpegExtractor( 1329): sniff through BetterSniffFFMPEG failed, try LegacySniffFFMPEG
I/FFmpegExtractor( 1329): source url:/data/app/com.h6ah4i.android.example.decodemp3ogg-2.apk
D/FFMPEG  ( 1329): android source begin open
D/FFMPEG  ( 1329): android open, url: android-source:0xb7d48ee8|file:/data/app/com.h6ah4i.android.example.decodemp3ogg-2.apk
D/FFMPEG  ( 1329): ffmpeg open android data source success, source ptr: 0xb7d48ee8
D/FFMPEG  ( 1329): android source open success
W/genymotion_audio(  256): out_write() limiting sleep time 1841900441 to 39909
I/FFMPEG  ( 1329): Input #0, mp3, from 'android-source:0xb7d48ee8|file:/data/app/com.h6ah4i.android.example.decodemp3ogg-2.apk':
I/FFMPEG  ( 1329):   Metadata:
I/FFMPEG  ( 1329):     artist          : Glenn Kasten
I/FFMPEG  ( 1329):     album           : Unknown
I/FFMPEG  ( 1329):     title           : Background for talking but don't
I/FFMPEG  ( 1329):     track           : 9
I/FFMPEG  ( 1329):     genre           : Soundtrack
I/FFMPEG  ( 1329):     date            : 2008
I/FFMPEG  ( 1329):   Duration: 00:01:46.32, start: 0.000000, bitrate: 193 kb/s
I/FFMPEG  ( 1329):     Stream #0:0: Audio: mp3, 44100 Hz, stereo, s16p, 193 kb/s
I/FFmpegExtractor( 1329): FFmpegExtrator, url: android-source:0xb7d48ee8|file:/data/app/com.h6ah4i.android.example.decodemp3ogg-2.apk, format_name: mp3, format_long_name: MP2/3 (MPEG audio layer 2/3)
I/FFmpegExtractor( 1329): list the formats suppoted by ffmpeg:
I/FFmpegExtractor( 1329): ========================================
V/FFmpegExtractor( 1329): format_names[00]: mpeg
V/FFmpegExtractor( 1329): format_names[01]: mpegts
V/FFmpegExtractor( 1329): format_names[02]: mov,mp4,m4a,3gp,3g2,mj2
V/FFmpegExtractor( 1329): format_names[03]: matroska,webm
V/FFmpegExtractor( 1329): format_names[04]: asf
V/FFmpegExtractor( 1329): format_names[05]: rm
V/FFmpegExtractor( 1329): format_names[06]: flv
V/FFmpegExtractor( 1329): format_names[07]: swf
V/FFmpegExtractor( 1329): format_names[08]: avi
V/FFmpegExtractor( 1329): format_names[09]: ape
V/FFmpegExtractor( 1329): format_names[10]: dts
V/FFmpegExtractor( 1329): format_names[11]: flac
V/FFmpegExtractor( 1329): format_names[12]: ac3
V/FFmpegExtractor( 1329): format_names[13]: wav
V/FFmpegExtractor( 1329): format_names[14]: ogg
V/FFmpegExtractor( 1329): format_names[15]: vc1
V/FFmpegExtractor( 1329): format_names[16]: hevc
I/FFmpegExtractor( 1329): ========================================
D/FFMPEG  ( 1329): android source close
D/FFmpegExtractor( 1329): SniffFFMPEG failed to sniff this source
I/OMXClient( 1329): Using client-side OMX mux.
W/genymotion_audio(  256): out_write() limiting sleep time 1841885475 to 39909
W/genymotion_audio(  256): out_write() limiting sleep time 1841870509 to 39909
D/OpenSLExample( 1329): extractMetadata()
D/OpenSLExample( 1329): prepareBufferQueue()
D/OpenSLExample( 1329): startDecoding()
D/OpenSLExample( 1329): waitForDecodeCompletion()
D/OpenSLExample( 1329): bufferQueueCallback(decoded_block_count = 1)
D/OpenSLExample( 1329): bufferQueueCallback(decoded_block_count = 2)
D/OpenSLExample( 1329): bufferQueueCallback(decoded_block_count = 3)

...

D/OpenSLExample( 1329): bufferQueueCallback(decoded_block_count = 4573)
D/OpenSLExample( 1329): bufferQueueCallback(decoded_block_count = 4574)
D/OpenSLExample( 1329): bufferQueueCallback(decoded_block_count = 4575)
D/OpenSLExample( 1329): bufferQueueCallback(decoded_block_count = 4576)
D/OpenSLExample( 1329): playInterfaceCallback(event = HEADATEND)
D/OpenSLExample( 1329): stopDecoding()
D/OpenSLExample( 1329): printDecodeResults
D/OpenSLExample( 1329): numChannels     = 2 [channels]
D/OpenSLExample( 1329): samplesPerSec   = 44100 [samples/sec.]
D/OpenSLExample( 1329): bitsPerSample   = 16 [bits]
D/OpenSLExample( 1329): containerSize   = 16 [bits]
D/OpenSLExample( 1329): channelMask     = 3
D/OpenSLExample( 1329): endianness      = 2
D/OpenSLExample( 1329): duration        = 106318 [ms]
D/OpenSLExample( 1329): decoded samples = 2 [samples]
D/OpenSLExample( 1329): destroyObjects()
---

</p></details>

<details><summary>Logcat - OGG (click here to expand)</summary><p>

```logcat
D/OpenSLExample( 1284): createEngine()
D/OpenSLExample( 1284): createPlayerForDecodingFd()
D/OpenSLExample( 1284): setupPlayerCallbacks()
D/OpenSLExample( 1284): startPrefetch()
D/OpenSLExample( 1284): waitForPrefetchCompletion()
V/FFmpegExtractor( 1284): SniffFFMPEG
I/FFmpegExtractor( 1284): android-source:0xb936dce0
D/FFMPEG  ( 1284): android source begin open
D/FFMPEG  ( 1284): android open, url: android-source:0xb936dce0
D/FFMPEG  ( 1284): ffmpeg open android data source success, source ptr: 0xb936dce0
D/FFMPEG  ( 1284): android source open success
I/FFMPEG  ( 1284): Input #0, ogg, from 'android-source:0xb936dce0':
I/FFMPEG  ( 1284):   Duration: 00:01:46.34, start: 0.000000, bitrate: 147 kb/s
I/FFMPEG  ( 1284):     Stream #0:0: Audio: vorbis, 44100 Hz, stereo, fltp, 160 kb/s
I/FFMPEG  ( 1284):     Metadata:
I/FFMPEG  ( 1284):       ALBUM           : Unknown
I/FFMPEG  ( 1284):       TITLE           : Background for talking but don't
I/FFMPEG  ( 1284):       track           : 9
I/FFMPEG  ( 1284):       COMMENTS        : Copyright 2008 Glenn Kasten, license CC:BY
I/FFMPEG  ( 1284):       ARTIST          : Glenn Kasten
I/FFMPEG  ( 1284):       DATE            : 2008
I/FFMPEG  ( 1284):       GENRE           : Soundtrack
I/FFmpegExtractor( 1284): FFmpegExtrator, url: android-source:0xb936dce0, format_name: ogg, format_long_name: Ogg
I/FFmpegExtractor( 1284): list the formats suppoted by ffmpeg:
I/FFmpegExtractor( 1284): ========================================
V/FFmpegExtractor( 1284): format_names[00]: mpeg
V/FFmpegExtractor( 1284): format_names[01]: mpegts
V/FFmpegExtractor( 1284): format_names[02]: mov,mp4,m4a,3gp,3g2,mj2
V/FFmpegExtractor( 1284): format_names[03]: matroska,webm
V/FFmpegExtractor( 1284): format_names[04]: asf
V/FFmpegExtractor( 1284): format_names[05]: rm
V/FFmpegExtractor( 1284): format_names[06]: flv
V/FFmpegExtractor( 1284): format_names[07]: swf
V/FFmpegExtractor( 1284): format_names[08]: avi
V/FFmpegExtractor( 1284): format_names[09]: ape
V/FFmpegExtractor( 1284): format_names[10]: dts
V/FFmpegExtractor( 1284): format_names[11]: flac
V/FFmpegExtractor( 1284): format_names[12]: ac3
V/FFmpegExtractor( 1284): format_names[13]: wav
V/FFmpegExtractor( 1284): format_names[14]: ogg
V/FFmpegExtractor( 1284): format_names[15]: vc1
V/FFmpegExtractor( 1284): format_names[16]: hevc
I/FFmpegExtractor( 1284): ========================================
I/FFmpegExtractor( 1284): adjust mime(application/ogg -> audio/vorbis)
D/FFmpegExtractor( 1284): suppoted codec(vorbis) by official Stagefright
D/FFMPEG  ( 1284): android source close
I/FFmpegExtractor( 1284): sniff through BetterSniffFFMPEG success
D/FFmpegExtractor( 1284): ffmpeg detected media content as 'audio/vorbis' with confidence 0.08
I/OMXClient( 1284): Using client-side OMX mux.
D/OpenSLExample( 1284): extractMetadata()
D/OpenSLExample( 1284): prepareBufferQueue()
D/OpenSLExample( 1284): startDecoding()
D/OpenSLExample( 1284): waitForDecodeCompletion()
D/OpenSLExample( 1284): bufferQueueCallback(decoded_block_count = 1)
D/OpenSLExample( 1284): bufferQueueCallback(decoded_block_count = 2)
D/OpenSLExample( 1284): bufferQueueCallback(decoded_block_count = 3)

...


D/OpenSLExample( 1284): bufferQueueCallback(decoded_block_count = 4575)
D/OpenSLExample( 1284): bufferQueueCallback(decoded_block_count = 4576)
D/OpenSLExample( 1284): bufferQueueCallback(decoded_block_count = 4577)
D/OpenSLExample( 1284): bufferQueueCallback(decoded_block_count = 4578)
D/OpenSLExample( 1284): bufferQueueCallback(decoded_block_count = 4579)
D/OpenSLExample( 1284): playInterfaceCallback(event = HEADATEND)
D/OpenSLExample( 1284): stopDecoding()
D/OpenSLExample( 1284): printDecodeResults
D/OpenSLExample( 1284): numChannels     = 2 [channels]
D/OpenSLExample( 1284): samplesPerSec   = 44100 [samples/sec.]
D/OpenSLExample( 1284): bitsPerSample   = 16 [bits]
D/OpenSLExample( 1284): containerSize   = 16 [bits]
D/OpenSLExample( 1284): channelMask     = 3
D/OpenSLExample( 1284): endianness      = 2
D/OpenSLExample( 1284): duration        = 106344 [ms]
D/OpenSLExample( 1284): decoded samples = 2 [samples]
D/OpenSLExample( 1284): destroyObjects()
```
</p></details>


### Android 5.1.0 (Genymotion)

<details><summary>Logcat - MP3 (click here to expand)</summary><p>

```logcat
D/OpenSLExample( 3306): createEngine()
D/OpenSLExample( 3306): createPlayerForDecodingFd()
D/OpenSLExample( 3306): setupPlayerCallbacks()
D/OpenSLExample( 3306): startPrefetch()
D/OpenSLExample( 3306): waitForPrefetchCompletion()
W/AudioTrack(  634): AUDIO_OUTPUT_FLAG_FAST denied by client
V/FFmpegExtractor( 3306): SniffFFMPEG
I/FFmpegExtractor( 3306): android-source:0xeec5b680
D/FFMPEG  ( 3306): android source begin open
D/FFMPEG  ( 3306): android open, url: android-source:0xeec5b680
D/FFMPEG  ( 3306): ffmpeg open android data source success, source ptr: 0xeec5b680
D/FFMPEG  ( 3306): android source open success
I/FFMPEG  ( 3306): Input #0, mp3, from 'android-source:0xeec5b680':
I/FFMPEG  ( 3306):   Metadata:
I/FFMPEG  ( 3306):     artist          : Glenn Kasten
I/FFMPEG  ( 3306):     album           : Unknown
I/FFMPEG  ( 3306):     title           : Background for talking but don't
I/FFMPEG  ( 3306):     track           : 9
I/FFMPEG  ( 3306):     genre           : Soundtrack
I/FFMPEG  ( 3306):     date            : 2008
I/FFMPEG  ( 3306):   Duration: 00:01:46.32, start: 0.025057, bitrate: 193 kb/s
I/FFMPEG  ( 3306):     Stream #0:0: Audio: mp3, 44100 Hz, stereo, s16p, 193 kb/s
I/FFMPEG  ( 3306):     Metadata:
I/FFMPEG  ( 3306):       encoder         : LAME3.98r
I/FFmpegExtractor( 3306): FFmpegExtrator, url: android-source:0xeec5b680, format_name: mp3, format_long_name: MP2/3 (MPEG audio layer 2/3)
I/FFmpegExtractor( 3306): list the formats suppoted by ffmpeg:
I/FFmpegExtractor( 3306): ========================================
V/FFmpegExtractor( 3306): format_names[00]: mpeg
V/FFmpegExtractor( 3306): format_names[01]: mpegts
V/FFmpegExtractor( 3306): format_names[02]: mov,mp4,m4a,3gp,3g2,mj2
V/FFmpegExtractor( 3306): format_names[03]: matroska,webm
V/FFmpegExtractor( 3306): format_names[04]: asf
V/FFmpegExtractor( 3306): format_names[05]: rm
V/FFmpegExtractor( 3306): format_names[06]: flv
V/FFmpegExtractor( 3306): format_names[07]: swf
V/FFmpegExtractor( 3306): format_names[08]: avi
V/FFmpegExtractor( 3306): format_names[09]: ape
V/FFmpegExtractor( 3306): format_names[10]: dts
V/FFmpegExtractor( 3306): format_names[11]: flac
V/FFmpegExtractor( 3306): format_names[12]: ac3
V/FFmpegExtractor( 3306): format_names[13]: wav
V/FFmpegExtractor( 3306): format_names[14]: ogg
V/FFmpegExtractor( 3306): format_names[15]: vc1
V/FFmpegExtractor( 3306): format_names[16]: hevc
I/FFmpegExtractor( 3306): ========================================
D/FFMPEG  ( 3306): android source close
W/FFmpegExtractor( 3306): sniff through BetterSniffFFMPEG failed, try LegacySniffFFMPEG
I/FFmpegExtractor( 3306): source url:/data/app/com.h6ah4i.android.example.decodemp3ogg-2/base.apk
D/FFMPEG  ( 3306): android source begin open
D/FFMPEG  ( 3306): android open, url: android-source:0xeec5b680|file:/data/app/com.h6ah4i.android.example.decodemp3ogg-2/base.apk
D/FFMPEG  ( 3306): ffmpeg open android data source success, source ptr: 0xeec5b680
D/FFMPEG  ( 3306): android source open success
I/FFMPEG  ( 3306): Input #0, mp3, from 'android-source:0xeec5b680|file:/data/app/com.h6ah4i.android.example.decodemp3ogg-2/base.apk':
I/FFMPEG  ( 3306):   Metadata:
I/FFMPEG  ( 3306):     artist          : Glenn Kasten
I/FFMPEG  ( 3306):     album           : Unknown
I/FFMPEG  ( 3306):     title           : Background for talking but don't
I/FFMPEG  ( 3306):     track           : 9
I/FFMPEG  ( 3306):     genre           : Soundtrack
I/FFMPEG  ( 3306):     date            : 2008
I/FFMPEG  ( 3306):   Duration: 00:01:46.32, start: 0.025057, bitrate: 193 kb/s
I/FFMPEG  ( 3306):     Stream #0:0: Audio: mp3, 44100 Hz, stereo, s16p, 193 kb/s
I/FFMPEG  ( 3306):     Metadata:
I/FFMPEG  ( 3306):       encoder         : LAME3.98r
I/FFmpegExtractor( 3306): FFmpegExtrator, url: android-source:0xeec5b680|file:/data/app/com.h6ah4i.android.example.decodemp3ogg-2/base.apk, format_name: mp3, format_long_name: MP2/3 (MPEG audio layer 2/3)
I/FFmpegExtractor( 3306): list the formats suppoted by ffmpeg:
I/FFmpegExtractor( 3306): ========================================
V/FFmpegExtractor( 3306): format_names[00]: mpeg
V/FFmpegExtractor( 3306): format_names[01]: mpegts
V/FFmpegExtractor( 3306): format_names[02]: mov,mp4,m4a,3gp,3g2,mj2
V/FFmpegExtractor( 3306): format_names[03]: matroska,webm
V/FFmpegExtractor( 3306): format_names[04]: asf
V/FFmpegExtractor( 3306): format_names[05]: rm
V/FFmpegExtractor( 3306): format_names[06]: flv
V/FFmpegExtractor( 3306): format_names[07]: swf
V/FFmpegExtractor( 3306): format_names[08]: avi
V/FFmpegExtractor( 3306): format_names[09]: ape
V/FFmpegExtractor( 3306): format_names[10]: dts
V/FFmpegExtractor( 3306): format_names[11]: flac
V/FFmpegExtractor( 3306): format_names[12]: ac3
V/FFmpegExtractor( 3306): format_names[13]: wav
V/FFmpegExtractor( 3306): format_names[14]: ogg
V/FFmpegExtractor( 3306): format_names[15]: vc1
V/FFmpegExtractor( 3306): format_names[16]: hevc
I/FFmpegExtractor( 3306): ========================================
D/FFMPEG  ( 3306): android source close
D/FFmpegExtractor( 3306): SniffFFMPEG failed to sniff this source
I/OMXClient( 3306): Using client-side OMX mux.
D/OpenSLExample( 3306): extractMetadata()
D/OpenSLExample( 3306): prepareBufferQueue()
D/OpenSLExample( 3306): startDecoding()
D/OpenSLExample( 3306): waitForDecodeCompletion()
D/OpenSLExample( 3306): bufferQueueCallback(decoded_block_count = 1)
D/OpenSLExample( 3306): bufferQueueCallback(decoded_block_count = 2)
D/OpenSLExample( 3306): bufferQueueCallback(decoded_block_count = 3)

...

D/OpenSLExample( 3306): bufferQueueCallback(decoded_block_count = 4570)
E/MP3Extractor( 3306): Unable to resync. Signalling end of stream.
D/OpenSLExample( 3306): bufferQueueCallback(decoded_block_count = 4571)
D/OpenSLExample( 3306): bufferQueueCallback(decoded_block_count = 4572)
D/OpenSLExample( 3306): bufferQueueCallback(decoded_block_count = 4573)
D/OpenSLExample( 3306): bufferQueueCallback(decoded_block_count = 4574)
D/OpenSLExample( 3306): bufferQueueCallback(decoded_block_count = 4575)
D/OpenSLExample( 3306): bufferQueueCallback(decoded_block_count = 4576)
D/OpenSLExample( 3306): playInterfaceCallback(event = HEADATEND)
D/OpenSLExample( 3306): stopDecoding()
D/OpenSLExample( 3306): printDecodeResults
D/OpenSLExample( 3306): numChannels     = 2 [channels]
D/OpenSLExample( 3306): samplesPerSec   = 44100 [samples/sec.]
D/OpenSLExample( 3306): bitsPerSample   = 16 [bits]
D/OpenSLExample( 3306): containerSize   = 16 [bits]
D/OpenSLExample( 3306): channelMask     = 3
D/OpenSLExample( 3306): endianness      = 2
D/OpenSLExample( 3306): duration        = 106318 [ms]
D/OpenSLExample( 3306): decoded samples = 2 [samples]
D/OpenSLExample( 3306): destroyObjects()
```
</p></details>

<details><summary>Logcat - OGG (click here to expand)</summary><p>

```logcat
D/OpenSLExample( 3306): createEngine()
D/OpenSLExample( 3306): createPlayerForDecodingFd()
D/OpenSLExample( 3306): setupPlayerCallbacks()
D/OpenSLExample( 3306): startPrefetch()
D/OpenSLExample( 3306): waitForPrefetchCompletion()
W/AudioTrack(  634): AUDIO_OUTPUT_FLAG_FAST denied by client
V/FFmpegExtractor( 3306): SniffFFMPEG
I/FFmpegExtractor( 3306): android-source:0xeec5b300
D/FFMPEG  ( 3306): android source begin open
D/FFMPEG  ( 3306): android open, url: android-source:0xeec5b300
D/FFMPEG  ( 3306): ffmpeg open android data source success, source ptr: 0xeec5b300
D/FFMPEG  ( 3306): android source open success
I/FFMPEG  ( 3306): Input #0, ogg, from 'android-source:0xeec5b300':
I/FFMPEG  ( 3306):   Duration: 00:01:46.34, start: 0.000000, bitrate: 147 kb/s
I/FFMPEG  ( 3306):     Stream #0:0: Audio: vorbis, 44100 Hz, stereo, fltp, 160 kb/s
I/FFMPEG  ( 3306):     Metadata:
I/FFMPEG  ( 3306):       ALBUM           : Unknown
I/FFMPEG  ( 3306):       TITLE           : Background for talking but don't
I/FFMPEG  ( 3306):       track           : 9
I/FFMPEG  ( 3306):       COMMENTS        : Copyright 2008 Glenn Kasten, license CC:BY
I/FFMPEG  ( 3306):       ARTIST          : Glenn Kasten
I/FFMPEG  ( 3306):       DATE            : 2008
I/FFMPEG  ( 3306):       GENRE           : Soundtrack
I/FFmpegExtractor( 3306): FFmpegExtrator, url: android-source:0xeec5b300, format_name: ogg, format_long_name: Ogg
I/FFmpegExtractor( 3306): list the formats suppoted by ffmpeg:
I/FFmpegExtractor( 3306): ========================================
V/FFmpegExtractor( 3306): format_names[00]: mpeg
V/FFmpegExtractor( 3306): format_names[01]: mpegts
V/FFmpegExtractor( 3306): format_names[02]: mov,mp4,m4a,3gp,3g2,mj2
V/FFmpegExtractor( 3306): format_names[03]: matroska,webm
V/FFmpegExtractor( 3306): format_names[04]: asf
V/FFmpegExtractor( 3306): format_names[05]: rm
V/FFmpegExtractor( 3306): format_names[06]: flv
V/FFmpegExtractor( 3306): format_names[07]: swf
V/FFmpegExtractor( 3306): format_names[08]: avi
V/FFmpegExtractor( 3306): format_names[09]: ape
V/FFmpegExtractor( 3306): format_names[10]: dts
V/FFmpegExtractor( 3306): format_names[11]: flac
V/FFmpegExtractor( 3306): format_names[12]: ac3
V/FFmpegExtractor( 3306): format_names[13]: wav
V/FFmpegExtractor( 3306): format_names[14]: ogg
V/FFmpegExtractor( 3306): format_names[15]: vc1
V/FFmpegExtractor( 3306): format_names[16]: hevc
I/FFmpegExtractor( 3306): ========================================
I/FFmpegExtractor( 3306): adjust mime(application/ogg -> audio/vorbis)
D/FFmpegExtractor( 3306): suppoted codec(vorbis) by official Stagefright
D/FFMPEG  ( 3306): android source close
I/FFmpegExtractor( 3306): sniff through BetterSniffFFMPEG success
D/FFmpegExtractor( 3306): ffmpeg detected media content as 'audio/vorbis' with confidence 0.08
I/OMXClient( 3306): Using client-side OMX mux.
D/OpenSLExample( 3306): extractMetadata()
D/OpenSLExample( 3306): prepareBufferQueue()
D/OpenSLExample( 3306): startDecoding()
D/OpenSLExample( 3306): waitForDecodeCompletion()
D/OpenSLExample( 3306): bufferQueueCallback(decoded_block_count = 1)
D/OpenSLExample( 3306): bufferQueueCallback(decoded_block_count = 2)
D/OpenSLExample( 3306): bufferQueueCallback(decoded_block_count = 3)

...

D/OpenSLExample( 3306): bufferQueueCallback(decoded_block_count = 4577)
D/OpenSLExample( 3306): bufferQueueCallback(decoded_block_count = 4578)
D/OpenSLExample( 3306): bufferQueueCallback(decoded_block_count = 4579)
D/OpenSLExample( 3306): playInterfaceCallback(event = HEADATEND)
D/OpenSLExample( 3306): stopDecoding()
D/OpenSLExample( 3306): printDecodeResults
D/OpenSLExample( 3306): numChannels     = 2 [channels]
D/OpenSLExample( 3306): samplesPerSec   = 44100 [samples/sec.]
D/OpenSLExample( 3306): bitsPerSample   = 16 [bits]
D/OpenSLExample( 3306): containerSize   = 16 [bits]
D/OpenSLExample( 3306): channelMask     = 3
D/OpenSLExample( 3306): endianness      = 2
D/OpenSLExample( 3306): duration        = 106344 [ms]
D/OpenSLExample( 3306): decoded samples = 2 [samples]
D/OpenSLExample( 3306): destroyObjects()
```
</p></details>

### Android 6.0.0 (Genymotion)

<details><summary>Logcat - MP3 (click here to expand)</summary><p>

```logcat
D OpenSLExample: createEngine()
D OpenSLExample: createPlayerForDecodingFd()
D OpenSLExample: setupPlayerCallbacks()
D OpenSLExample: startPrefetch()
D OpenSLExample: waitForPrefetchCompletion()
V FFmpegExtractor: SniffFFMPEG
I FFmpegExtractor: android-source:0xe813e0c0
D FFMPEG  : android source begin open
D FFMPEG  : android open, url: android-source:0xe813e0c0
D FFMPEG  : ffmpeg open android data source success, source ptr: 0xe813e0c0
D FFMPEG  : android source open success
I FFMPEG  : Input #0, mp3, from 'android-source:0xe813e0c0':
I FFMPEG  :   Metadata:
I FFMPEG  :     artist          : Glenn Kasten
I FFMPEG  :     album           : Unknown
I FFMPEG  :     title           : Background for talking but don't
I FFMPEG  :     track           : 9
I FFMPEG  :     genre           : Soundtrack
I FFMPEG  :     date            : 2008
I FFMPEG  :   Duration: 00:01:46.32, start: 0.025057, bitrate: 193 kb/s
I FFMPEG  :     Stream #0:0: Audio: mp3, 44100 Hz, stereo, s16p, 193 kb/s
I FFMPEG  :     Metadata:
I FFMPEG  :       encoder         : LAME3.98r
I FFmpegExtractor: FFmpegExtrator, url: android-source:0xe813e0c0, format_name: mp3, format_long_name: MP2/3 (MPEG audio layer 2/3)
I FFmpegExtractor: list the formats suppoted by ffmpeg:
I FFmpegExtractor: ========================================
V FFmpegExtractor: format_names[00]: mpeg
V FFmpegExtractor: format_names[01]: mpegts
V FFmpegExtractor: format_names[02]: mov,mp4,m4a,3gp,3g2,mj2
V FFmpegExtractor: format_names[03]: matroska,webm
V FFmpegExtractor: format_names[04]: asf
V FFmpegExtractor: format_names[05]: rm
V FFmpegExtractor: format_names[06]: flv
V FFmpegExtractor: format_names[07]: swf
V FFmpegExtractor: format_names[08]: avi
V FFmpegExtractor: format_names[09]: ape
V FFmpegExtractor: format_names[10]: dts
V FFmpegExtractor: format_names[11]: flac
V FFmpegExtractor: format_names[12]: ac3
V FFmpegExtractor: format_names[13]: wav
V FFmpegExtractor: format_names[14]: ogg
V FFmpegExtractor: format_names[15]: vc1
V FFmpegExtractor: format_names[16]: hevc
I FFmpegExtractor: ========================================
D FFMPEG  : android source close
W FFmpegExtractor: sniff through BetterSniffFFMPEG failed, try LegacySniffFFMPEG
I FFmpegExtractor: source url:/data/app/com.h6ah4i.android.example.decodemp3ogg-1/base.apk
D FFMPEG  : android source begin open
D FFMPEG  : android open, url: android-source:0xe813e0c0|file:/data/app/com.h6ah4i.android.example.decodemp3ogg-1/base.apk
D FFMPEG  : ffmpeg open android data source success, source ptr: 0xe813e0c0
D FFMPEG  : android source open success
I FFMPEG  : Input #0, mp3, from 'android-source:0xe813e0c0|file:/data/app/com.h6ah4i.android.example.decodemp3ogg-1/base.apk':
I FFMPEG  :   Metadata:
I FFMPEG  :     artist          : Glenn Kasten
I FFMPEG  :     album           : Unknown
I FFMPEG  :     title           : Background for talking but don't
I FFMPEG  :     track           : 9
I FFMPEG  :     genre           : Soundtrack
I FFMPEG  :     date            : 2008
I FFMPEG  :   Duration: 00:01:46.32, start: 0.025057, bitrate: 193 kb/s
I FFMPEG  :     Stream #0:0: Audio: mp3, 44100 Hz, stereo, s16p, 193 kb/s
I FFMPEG  :     Metadata:
I FFMPEG  :       encoder         : LAME3.98r
I FFmpegExtractor: FFmpegExtrator, url: android-source:0xe813e0c0|file:/data/app/com.h6ah4i.android.example.decodemp3ogg-1/base.apk, format_name: mp3, format_long_name: MP2/3 (MPEG audio layer 2/3)
I FFmpegExtractor: list the formats suppoted by ffmpeg:
I FFmpegExtractor: ========================================
V FFmpegExtractor: format_names[00]: mpeg
V FFmpegExtractor: format_names[01]: mpegts
V FFmpegExtractor: format_names[02]: mov,mp4,m4a,3gp,3g2,mj2
V FFmpegExtractor: format_names[03]: matroska,webm
V FFmpegExtractor: format_names[04]: asf
V FFmpegExtractor: format_names[05]: rm
V FFmpegExtractor: format_names[06]: flv
V FFmpegExtractor: format_names[07]: swf
V FFmpegExtractor: format_names[08]: avi
V FFmpegExtractor: format_names[09]: ape
V FFmpegExtractor: format_names[10]: dts
V FFmpegExtractor: format_names[11]: flac
V FFmpegExtractor: format_names[12]: ac3
V FFmpegExtractor: format_names[13]: wav
V FFmpegExtractor: format_names[14]: ogg
V FFmpegExtractor: format_names[15]: vc1
V FFmpegExtractor: format_names[16]: hevc
I FFmpegExtractor: ========================================
D FFMPEG  : android source close
D FFmpegExtractor: SniffFFMPEG failed to sniff this source
I OMXClient: Using client-side OMX mux.
D AudioFlinger: mixer(0xf1dc0000) throttle end: throttle time(11)
D OpenSLExample: extractMetadata()
D OpenSLExample: prepareBufferQueue()
D OpenSLExample: startDecoding()
D OpenSLExample: waitForDecodeCompletion()
D OpenSLExample: bufferQueueCallback(decoded_block_count = 1)
D OpenSLExample: bufferQueueCallback(decoded_block_count = 2)
D OpenSLExample: bufferQueueCallback(decoded_block_count = 3)

...


D OpenSLExample: bufferQueueCallback(decoded_block_count = 4573)
D OpenSLExample: bufferQueueCallback(decoded_block_count = 4574)
D OpenSLExample: bufferQueueCallback(decoded_block_count = 4575)
D OpenSLExample: bufferQueueCallback(decoded_block_count = 4576)
D OpenSLExample: playInterfaceCallback(event = HEADATEND)
D OpenSLExample: stopDecoding()
D OpenSLExample: printDecodeResults
D OpenSLExample: numChannels     = 2 [channels]
D OpenSLExample: samplesPerSec   = 44100 [samples/sec.]
D OpenSLExample: bitsPerSample   = 16 [bits]
D OpenSLExample: containerSize   = 16 [bits]
D OpenSLExample: channelMask     = 3
D OpenSLExample: endianness      = 2
D OpenSLExample: duration        = 106318 [ms]
D OpenSLExample: decoded samples = 2 [samples]
D OpenSLExample: destroyObjects()
```
</p></details>


<details><summary>Logcat - OGG (click here to expand)</summary><p>

```logcat
02-14 18:17:51.030  1937  1937 D OpenSLExample: createEngine()
02-14 18:17:51.030  1937  1937 D OpenSLExample: createPlayerForDecodingFd()
02-14 18:17:51.031  1937  1937 D OpenSLExample: setupPlayerCallbacks()
02-14 18:17:51.031  1937  1937 D OpenSLExample: startPrefetch()
02-14 18:17:51.031  1937  1937 D OpenSLExample: waitForPrefetchCompletion()
02-14 18:17:51.035  1937  2035 V FFmpegExtractor: SniffFFMPEG
02-14 18:17:51.035  1937  2035 I FFmpegExtractor: android-source:0xe813e300
02-14 18:17:51.035  1937  2035 D FFMPEG  : android source begin open
02-14 18:17:51.035  1937  2035 D FFMPEG  : android open, url: android-source:0xe813e300
02-14 18:17:51.035  1937  2035 D FFMPEG  : ffmpeg open android data source success, source ptr: 0xe813e300
02-14 18:17:51.035  1937  2035 D FFMPEG  : android source open success
02-14 18:17:51.037  1937  2035 I FFMPEG  : Input #0, ogg, from 'android-source:0xe813e300':
02-14 18:17:51.037  1937  2035 I FFMPEG  :   Duration: 00:01:46.34, start: 0.000000, bitrate: 147 kb/s
02-14 18:17:51.037  1937  2035 I FFMPEG  :     Stream #0:0: Audio: vorbis, 44100 Hz, stereo, fltp, 160 kb/s
02-14 18:17:51.037  1937  2035 I FFMPEG  :     Metadata:
02-14 18:17:51.037  1937  2035 I FFMPEG  :       ALBUM           : Unknown
02-14 18:17:51.037  1937  2035 I FFMPEG  :       TITLE           : Background for talking but don't
02-14 18:17:51.037  1937  2035 I FFMPEG  :       track           : 9
02-14 18:17:51.037  1937  2035 I FFMPEG  :       COMMENTS        : Copyright 2008 Glenn Kasten, license CC:BY
02-14 18:17:51.037  1937  2035 I FFMPEG  :       ARTIST          : Glenn Kasten
02-14 18:17:51.037  1937  2035 I FFMPEG  :       DATE            : 2008
02-14 18:17:51.037  1937  2035 I FFMPEG  :       GENRE           : Soundtrack
02-14 18:17:51.037  1937  2035 I FFmpegExtractor: FFmpegExtrator, url: android-source:0xe813e300, format_name: ogg, format_long_name: Ogg
02-14 18:17:51.037  1937  2035 I FFmpegExtractor: list the formats suppoted by ffmpeg:
02-14 18:17:51.037  1937  2035 I FFmpegExtractor: ========================================
02-14 18:17:51.037  1937  2035 V FFmpegExtractor: format_names[00]: mpeg
02-14 18:17:51.037  1937  2035 V FFmpegExtractor: format_names[01]: mpegts
02-14 18:17:51.037  1937  2035 V FFmpegExtractor: format_names[02]: mov,mp4,m4a,3gp,3g2,mj2
02-14 18:17:51.037  1937  2035 V FFmpegExtractor: format_names[03]: matroska,webm
02-14 18:17:51.037  1937  2035 V FFmpegExtractor: format_names[04]: asf
02-14 18:17:51.037  1937  2035 V FFmpegExtractor: format_names[05]: rm
02-14 18:17:51.037  1937  2035 V FFmpegExtractor: format_names[06]: flv
02-14 18:17:51.037  1937  2035 V FFmpegExtractor: format_names[07]: swf
02-14 18:17:51.037  1937  2035 V FFmpegExtractor: format_names[08]: avi
02-14 18:17:51.037  1937  2035 V FFmpegExtractor: format_names[09]: ape
02-14 18:17:51.037  1937  2035 V FFmpegExtractor: format_names[10]: dts
02-14 18:17:51.037  1937  2035 V FFmpegExtractor: format_names[11]: flac
02-14 18:17:51.037  1937  2035 V FFmpegExtractor: format_names[12]: ac3
02-14 18:17:51.037  1937  2035 V FFmpegExtractor: format_names[13]: wav
02-14 18:17:51.037  1937  2035 V FFmpegExtractor: format_names[14]: ogg
02-14 18:17:51.037  1937  2035 V FFmpegExtractor: format_names[15]: vc1
02-14 18:17:51.037  1937  2035 V FFmpegExtractor: format_names[16]: hevc
02-14 18:17:51.037  1937  2035 I FFmpegExtractor: ========================================
02-14 18:17:51.037  1937  2035 I FFmpegExtractor: adjust mime(application/ogg -> audio/vorbis)
02-14 18:17:51.037  1937  2035 D FFmpegExtractor: suppoted codec(vorbis) by official Stagefright
02-14 18:17:51.037  1937  2035 D FFMPEG  : android source close
02-14 18:17:51.037  1937  2035 I FFmpegExtractor: sniff through BetterSniffFFMPEG success
02-14 18:17:51.037  1937  2035 D FFmpegExtractor: ffmpeg detected media content as 'audio/vorbis' with confidence 0.08
02-14 18:17:51.041  1937  2035 I OMXClient: Using client-side OMX mux.
02-14 18:17:51.254   435   812 D AudioFlinger: mixer(0xf1dc0000) throttle end: throttle time(11)
02-14 18:17:51.321  1937  1937 D OpenSLExample: extractMetadata()
02-14 18:17:51.321  1937  1937 D OpenSLExample: prepareBufferQueue()
02-14 18:17:51.321  1937  1937 D OpenSLExample: startDecoding()
02-14 18:17:51.321  1937  1937 D OpenSLExample: waitForDecodeCompletion()
02-14 18:17:51.322  1937  2035 D OpenSLExample: bufferQueueCallback(decoded_block_count = 1)
02-14 18:17:51.323  1937  2035 D OpenSLExample: bufferQueueCallback(decoded_block_count = 2)
02-14 18:17:51.323  1937  2035 D OpenSLExample: bufferQueueCallback(decoded_block_count = 3)

...

02-14 18:17:54.307  1937  2035 D OpenSLExample: bufferQueueCallback(decoded_block_count = 4577)
02-14 18:17:54.307  1937  2035 D OpenSLExample: bufferQueueCallback(decoded_block_count = 4578)
02-14 18:17:54.308  1937  2035 D OpenSLExample: bufferQueueCallback(decoded_block_count = 4579)
02-14 18:17:54.308  1937  2035 D OpenSLExample: playInterfaceCallback(event = HEADATEND)
02-14 18:17:54.309  1937  1937 D OpenSLExample: stopDecoding()
02-14 18:17:54.309  1937  1937 D OpenSLExample: printDecodeResults
02-14 18:17:54.309  1937  1937 D OpenSLExample: numChannels     = 2 [channels]
02-14 18:17:54.309  1937  1937 D OpenSLExample: samplesPerSec   = 44100 [samples/sec.]
02-14 18:17:54.309  1937  1937 D OpenSLExample: bitsPerSample   = 16 [bits]
02-14 18:17:54.309  1937  1937 D OpenSLExample: containerSize   = 16 [bits]
02-14 18:17:54.309  1937  1937 D OpenSLExample: channelMask     = 3
02-14 18:17:54.309  1937  1937 D OpenSLExample: endianness      = 2
02-14 18:17:54.309  1937  1937 D OpenSLExample: duration        = 106344 [ms]
02-14 18:17:54.309  1937  1937 D OpenSLExample: decoded samples = 2 [samples]
02-14 18:17:54.309  1937  1937 D OpenSLExample: destroyObjects()
```
</p></details>


### Android 7.0.0 (Genymotion)

<details><summary>Logcat - MP3 (click here to expand)</summary><p>

```logcat
D OpenSLExample: createEngine()
D OpenSLExample: createPlayerForDecodingFd()
D OpenSLExample: setupPlayerCallbacks()
D OpenSLExample: startPrefetch()
D OpenSLExample: waitForPrefetchCompletion()
E WVMExtractor: Failed to open libwvm.so: dlopen failed: library "libwvm.so" not found
V FFmpegExtractor: SniffFFMPEG
I FFmpegExtractor: android-source:0xe3986c00
D FFMPEG  : android source begin open
D FFMPEG  : android open, url: android-source:0xe3986c00
D FFMPEG  : ffmpeg open android data source success, source ptr: 0xe3986c00
D FFMPEG  : android source open success
I FFMPEG  : Input #0, mp3, from 'android-source:0xe3986c00':
I FFMPEG  :   Metadata:
I FFMPEG  :     artist          : Glenn Kasten
I FFMPEG  :     album           : Unknown
I FFMPEG  :     title           : Background for talking but don't
I FFMPEG  :     track           : 9
I FFMPEG  :     genre           : Soundtrack
I FFMPEG  :     date            : 2008
I FFMPEG  :   Duration: 00:01:46.32, start: 0.025057, bitrate: 193 kb/s
I FFMPEG  :     Stream #0:0: Audio: mp3, 44100 Hz, stereo, s16p, 193 kb/s
I FFMPEG  :     Metadata:
I FFMPEG  :       encoder         : LAME3.98r
I FFmpegExtractor: FFmpegExtrator, url: android-source:0xe3986c00, format_name: mp3, format_long_name: MP2/3 (MPEG audio layer 2/3)
I FFmpegExtractor: list the formats suppoted by ffmpeg:
I FFmpegExtractor: ========================================
V FFmpegExtractor: format_names[00]: mpeg
V FFmpegExtractor: format_names[01]: mpegts
V FFmpegExtractor: format_names[02]: mov,mp4,m4a,3gp,3g2,mj2
V FFmpegExtractor: format_names[03]: matroska,webm
V FFmpegExtractor: format_names[04]: asf
V FFmpegExtractor: format_names[05]: rm
V FFmpegExtractor: format_names[06]: flv
V FFmpegExtractor: format_names[07]: swf
V FFmpegExtractor: format_names[08]: avi
V FFmpegExtractor: format_names[09]: ape
V FFmpegExtractor: format_names[10]: dts
V FFmpegExtractor: format_names[11]: flac
V FFmpegExtractor: format_names[12]: ac3
V FFmpegExtractor: format_names[13]: wav
V FFmpegExtractor: format_names[14]: ogg
V FFmpegExtractor: format_names[15]: vc1
V FFmpegExtractor: format_names[16]: hevc
I FFmpegExtractor: ========================================
D FFMPEG  : android source close
W FFmpegExtractor: sniff through BetterSniffFFMPEG failed, try LegacySniffFFMPEG
I FFmpegExtractor: source url:
D FFMPEG  : android source begin open
D FFMPEG  : android open, url: android-source:0xe3986c00|file:
D FFMPEG  : ffmpeg open android data source success, source ptr: 0xe3986c00
D FFMPEG  : android source open success
I FFMPEG  : Input #0, mp3, from 'android-source:0xe3986c00|file:':
I FFMPEG  :   Metadata:
I FFMPEG  :     artist          : Glenn Kasten
I FFMPEG  :     album           : Unknown
I FFMPEG  :     title           : Background for talking but don't
I FFMPEG  :     track           : 9
I FFMPEG  :     genre           : Soundtrack
I FFMPEG  :     date            : 2008
I FFMPEG  :   Duration: 00:01:46.32, start: 0.025057, bitrate: 193 kb/s
I FFMPEG  :     Stream #0:0: Audio: mp3, 44100 Hz, stereo, s16p, 193 kb/s
I FFMPEG  :     Metadata:
I FFMPEG  :       encoder         : LAME3.98r
I FFmpegExtractor: FFmpegExtrator, url: android-source:0xe3986c00|file:, format_name: mp3, format_long_name: MP2/3 (MPEG audio layer 2/3)
I FFmpegExtractor: list the formats suppoted by ffmpeg:
I FFmpegExtractor: ========================================
V FFmpegExtractor: format_names[00]: mpeg
V FFmpegExtractor: format_names[01]: mpegts
V FFmpegExtractor: format_names[02]: mov,mp4,m4a,3gp,3g2,mj2
V FFmpegExtractor: format_names[03]: matroska,webm
V FFmpegExtractor: format_names[04]: asf
V FFmpegExtractor: format_names[05]: rm
V FFmpegExtractor: format_names[06]: flv
V FFmpegExtractor: format_names[07]: swf
V FFmpegExtractor: format_names[08]: avi
V FFmpegExtractor: format_names[09]: ape
V FFmpegExtractor: format_names[10]: dts
V FFmpegExtractor: format_names[11]: flac
V FFmpegExtractor: format_names[12]: ac3
V FFmpegExtractor: format_names[13]: wav
V FFmpegExtractor: format_names[14]: ogg
V FFmpegExtractor: format_names[15]: vc1
V FFmpegExtractor: format_names[16]: hevc
I FFmpegExtractor: ========================================
D FFMPEG  : android source close
D FFmpegExtractor: SniffFFMPEG failed to sniff this source
I MediaPlayerService: MediaPlayerService::getOMX
I OMXClient: MuxOMX ctor
I OMXMaster: makeComponentInstance(OMX.google.mp3.decoder) in mediacodec process
I         : Successfully allocated codec 'OMX.google.mp3.decoder'
D OpenSLExample: extractMetadata()
D OpenSLExample: prepareBufferQueue()
D OpenSLExample: startDecoding()
D OpenSLExample: waitForDecodeCompletion()
D OpenSLExample: bufferQueueCallback(decoded_block_count = 1)
D OpenSLExample: bufferQueueCallback(decoded_block_count = 2)
D OpenSLExample: bufferQueueCallback(decoded_block_count = 3)

...

02-13 18:40:33.068  2031  2397 D OpenSLExample: bufferQueueCallback(decoded_block_count = 4573)
02-13 18:40:33.068  2031  2397 D OpenSLExample: bufferQueueCallback(decoded_block_count = 4574)
02-13 18:40:33.069  2031  2397 D OpenSLExample: bufferQueueCallback(decoded_block_count = 4575)
02-13 18:40:33.069  2031  2397 D OpenSLExample: bufferQueueCallback(decoded_block_count = 4576)
02-13 18:40:33.071  2031  2397 D OpenSLExample: playInterfaceCallback(event = HEADATEND)
02-13 18:40:33.071  2031  2031 D OpenSLExample: stopDecoding()
02-13 18:40:33.071  2031  2031 D OpenSLExample: printDecodeResults
02-13 18:40:33.071  2031  2031 D OpenSLExample: numChannels     = 2 [channels]
02-13 18:40:33.071  2031  2031 D OpenSLExample: samplesPerSec   = 44100 [samples/sec.]
02-13 18:40:33.071  2031  2031 D OpenSLExample: bitsPerSample   = 16 [bits]
02-13 18:40:33.071  2031  2031 D OpenSLExample: containerSize   = 16 [bits]
02-13 18:40:33.071  2031  2031 D OpenSLExample: channelMask     = 3
02-13 18:40:33.071  2031  2031 D OpenSLExample: endianness      = 2
02-13 18:40:33.071  2031  2031 D OpenSLExample: duration        = 106318 [ms]
02-13 18:40:33.071  2031  2031 D OpenSLExample: decoded samples = 2 [samples]
02-13 18:40:33.071  2031  2031 D OpenSLExample: destroyObjects()
```
</p></details>

<details><summary>Logcat - OGG (click here to expand)</summary><p>

```logcat
D OpenSLExample: createEngine()
D OpenSLExample: createPlayerForDecodingFd()
D OpenSLExample: setupPlayerCallbacks()
D OpenSLExample: startPrefetch()
D OpenSLExample: waitForPrefetchCompletion()
V FFmpegExtractor: SniffFFMPEG
I FFmpegExtractor: android-source:0xe3983a00
D FFMPEG  : android source begin open
D FFMPEG  : android open, url: android-source:0xe3983a00
D FFMPEG  : ffmpeg open android data source success, source ptr: 0xe3983a00
D FFMPEG  : android source open success
I FFMPEG  : Input #0, ogg, from 'android-source:0xe3983a00':
I FFMPEG  :   Duration: 00:01:46.34, start: 0.000000, bitrate: 147 kb/s
I FFMPEG  :     Stream #0:0: Audio: vorbis, 44100 Hz, stereo, fltp, 160 kb/s
I FFMPEG  :     Metadata:
I FFMPEG  :       ALBUM           : Unknown
I FFMPEG  :       TITLE           : Background for talking but don't
I FFMPEG  :       track           : 9
I FFMPEG  :       COMMENTS        : Copyright 2008 Glenn Kasten, license CC:BY
I FFMPEG  :       ARTIST          : Glenn Kasten
I FFMPEG  :       DATE            : 2008
I FFMPEG  :       GENRE           : Soundtrack
I FFmpegExtractor: FFmpegExtrator, url: android-source:0xe3983a00, format_name: ogg, format_long_name: Ogg
I FFmpegExtractor: list the formats suppoted by ffmpeg:
I FFmpegExtractor: ========================================
V FFmpegExtractor: format_names[00]: mpeg
V FFmpegExtractor: format_names[01]: mpegts
V FFmpegExtractor: format_names[02]: mov,mp4,m4a,3gp,3g2,mj2
V FFmpegExtractor: format_names[03]: matroska,webm
V FFmpegExtractor: format_names[04]: asf
V FFmpegExtractor: format_names[05]: rm
V FFmpegExtractor: format_names[06]: flv
V FFmpegExtractor: format_names[07]: swf
V FFmpegExtractor: format_names[08]: avi
V FFmpegExtractor: format_names[09]: ape
V FFmpegExtractor: format_names[10]: dts
V FFmpegExtractor: format_names[11]: flac
V FFmpegExtractor: format_names[12]: ac3
V FFmpegExtractor: format_names[13]: wav
V FFmpegExtractor: format_names[14]: ogg
V FFmpegExtractor: format_names[15]: vc1
V FFmpegExtractor: format_names[16]: hevc
I FFmpegExtractor: ========================================
I FFmpegExtractor: adjust mime(application/ogg -> audio/vorbis)
D FFmpegExtractor: suppoted codec(vorbis) by official Stagefright
D FFMPEG  : android source close
I FFmpegExtractor: sniff through BetterSniffFFMPEG success
D FFmpegExtractor: ffmpeg detected media content as 'audio/vorbis' with confidence 0.08
D AudioFlinger: mixer(0xeec835c0) throttle end: throttle time(11)
I MediaPlayerService: MediaPlayerService::getOMX
I OMXClient: MuxOMX ctor
I OMXMaster: makeComponentInstance(OMX.google.vorbis.decoder) in mediacodec process
I         : Successfully allocated codec 'OMX.google.vorbis.decoder'
D OpenSLExample: extractMetadata()
D OpenSLExample: prepareBufferQueue()
D OpenSLExample: startDecoding()
D OpenSLExample: waitForDecodeCompletion()
E SoftVorbis: onQueueFilled, input header has nFilledLen 1, expected 4
E ACodec  : [OMX.google.vorbis.decoder] ERROR(0x80001005)
E ACodec  : signalError(omxError 0x80001005, internalError -2147483648)
E MediaCodec: Codec reported err 0x80001005, actionCode 0, while in state 6
W         : [OMX.google.vorbis.decoder] could not get input buffer #1
E libOpenSLES: MediaSource::read returned error -2147483648
F libc    : /Users/hasegawa/git/android-opensl-mp3-ogg-decode-test/app/src/main/cpp/decode-mp3-ogg-jni.cpp:439: void SLAppContext::waitForDecodeCompletion(): assertion "0 == pt_result" failed
```
</p></details>

### Android 7.1.1 (Nexus 5X)

<details><summary>Logcat - MP3 (click here to expand)</summary><p>

```logcat
D OpenSLExample: createEngine()
D OpenSLExample: createPlayerForDecodingFd()
D OpenSLExample: setupPlayerCallbacks()
D OpenSLExample: startPrefetch()
D OpenSLExample: waitForPrefetchCompletion()
E WVMExtractor: Failed to open libwvm.so: dlopen failed: library "libwvm.so" not found
I MediaPlayerService: MediaPlayerService::getOMX
I OMXClient: MuxOMX ctor
I OMXMaster: makeComponentInstance(OMX.google.mp3.decoder) in mediacodec process
I         : Successfully allocated codec 'OMX.google.mp3.decoder'
D OpenSLExample: extractMetadata()
D OpenSLExample: prepareBufferQueue()
D OpenSLExample: startDecoding()
D OpenSLExample: waitForDecodeCompletion()
D OpenSLExample: bufferQueueCallback(decoded_block_count = 1)
D OpenSLExample: bufferQueueCallback(decoded_block_count = 2)
D OpenSLExample: bufferQueueCallback(decoded_block_count = 3)
D OpenSLExample: bufferQueueCallback(decoded_block_count = 4)

...

D OpenSLExample: bufferQueueCallback(decoded_block_count = 4573)
D OpenSLExample: bufferQueueCallback(decoded_block_count = 4574)
D OpenSLExample: bufferQueueCallback(decoded_block_count = 4575)
D OpenSLExample: bufferQueueCallback(decoded_block_count = 4576)
D OpenSLExample: playInterfaceCallback(event = HEADATEND)
D OpenSLExample: stopDecoding()
D OpenSLExample: printDecodeResults
D OpenSLExample: numChannels     = 2 [channels]
D OpenSLExample: samplesPerSec   = 44100 [samples/sec.]
D OpenSLExample: bitsPerSample   = 16 [bits]
D OpenSLExample: containerSize   = 16 [bits]
D OpenSLExample: channelMask     = 3
D OpenSLExample: endianness      = 2
D OpenSLExample: duration        = 106318 [ms]
D OpenSLExample: decoded samples = 2 [samples]
D OpenSLExample: destroyObjects()
```
</p></details>

<details><summary>Logcat - OGG (click here to expand)</summary><p>

```logcat
D OpenSLExample: createEngine()
D OpenSLExample: createPlayerForDecodingFd()
D OpenSLExample: setupPlayerCallbacks()
D OpenSLExample: startPrefetch()
D OpenSLExample: waitForPrefetchCompletion()
I MediaPlayerService: MediaPlayerService::getOMX
I OMXClient: MuxOMX ctor
I OMXMaster: makeComponentInstance(OMX.google.vorbis.decoder) in mediacodec process
I         : Successfully allocated codec 'OMX.google.vorbis.decoder'
D OpenSLExample: extractMetadata()
D OpenSLExample: prepareBufferQueue()
D OpenSLExample: startDecoding()
D OpenSLExample: waitForDecodeCompletion()
E SoftVorbis: onQueueFilled, input header has nFilledLen 1, expected 4
E ACodec  : [OMX.google.vorbis.decoder] ERROR(0x80001005)
E ACodec  : signalError(omxError 0x80001005, internalError -2147483648)
E MediaCodec: Codec reported err 0x80001005, actionCode 0, while in state 6
W         : [OMX.google.vorbis.decoder] could not get input buffer #1
E libOpenSLES: MediaSource::read returned error -2147483648
F libc    : /Users/hasegawa/git/android-opensl-mp3-ogg-decode-test/app/src/main/cpp/decode-mp3-ogg-jni.cpp:439: void SLAppContext::waitForDecodeCompletion(): assertion "0 == pt_result" failed
```
</p></details>
