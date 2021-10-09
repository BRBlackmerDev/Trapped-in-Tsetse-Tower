
typedef HRESULT WINAPI DIRECT_SOUND_CREATE( LPCGUID lpcGuidDevice, LPDIRECTSOUND * ppDS, LPUNKNOWN lpUnkOuter );

#define AUDIO__SAMPLES_PER_SECOND  ( 48000 )
#define AUDIO__BYTES_PER_SAMPLE    ( 2 )
#define AUDIO__CHANNEL_COUNT       ( 2 )
#define AUDIO__BUFFER_SECONDS      ( 2 )
#define AUDIO__WRITE_FRAMES        ( 3 )

struct AUDIO_DATA {
    uint8 * audio;
    uint32 samples;
};

enum AUDIO_ID {
    audioID_music_start,
    
    audioID_music,
    audioID_music_battle01,
    audioID_music_underwaterAmbience,
    
    audioID_music_end,
    
    
    
    
    audioID_bite_fail,
    audioID_bite0,
    audioID_bite1,
    audioID_bite2,
    audioID_bite3,
    audioID_bite4,
    audioID_scoreMultiplier,
    
    audioID_Player_parry01,
    audioID_Player_parry02,
    audioID_Player_parry03,
    audioID_Player_death,
    
    audioID_bite_dogfish,
    
    audioID_dogfish_biting01,
    audioID_dogfish_biting02,
    audioID_dogfish_biting03,
    audioID_dogfish_biting04,
    audioID_dogfish_biting05,
    audioID_dogfish_lunge01,
    audioID_dogfish_lunge02,
    audioID_dogfish_lunge03,
    audioID_dogfish_lunge04,
    audioID_dogfish_lunge05,
    audioID_dogfish_lunge06,
    audioID_dogfish_lunge07,
    audioID_dogfish_lunge08,
    audioID_dogfish_lunge09,
    audioID_dogfish_lunge10,
    audioID_dogfish_lunge11,
    
    audioID_swordfish_parry_lo,
    audioID_swordfish_parry_hi,
    audioID_swordfish_stab01,
    audioID_swordfish_stab02,
    audioID_swordfish_slash01,
    audioID_swordfish_slash02,
    audioID_swordfish_hitPlayer,
    audioID_swordfish_block01,
    audioID_swordfish_block02,
    audioID_swordfish_block03,
    
    audioID_pop01,
    audioID_pop02,
    audioID_spit01,
    audioID_spit02,
    audioID_spit03,
    audioID_bubble_spit01,
    audioID_bubble_spit02,
    audioID_bubble_capture,
    
    audioID_count,
};

struct AUDIO_ENTRY {
    AUDIO_ID audioID;
    
    uint8 * audio;
    uint32 nSamples;
    
    uint32 atSample;
    flo32  atSample_t;
    
    flo32 volume;
    flo32 rate;
    boo32 isLooping;
    
    flo32 targetVolume;
    flo32 targetVolume_dMax;
    flo32 targetVolume_delay;
};

struct ASSET_SYSTEM;
#define AUDIO__MAX_ENTRY_COUNT  ( 256 )
struct AUDIO_SYSTEM {
    ASSET_SYSTEM * assetSystem;
    
    flo32 mainVolume;
    
    uint32     nAudio;
    AUDIO_ENTRY audio[ AUDIO__MAX_ENTRY_COUNT ];
    
    uint8 * mixBuffer;
    int32   mixBuffer_size;
};

#if 0
struct MUSIC_SYSTEM {
    ASSET_SYSTEM * assetSystem;
    
    flo32 mainVolume;
    
    uint8 * audio;
    uint32 nSamples;
    
#define MAX_SAMPLES_TO_WRITE  ( ( AUDIO__SAMPLES_PER_SECOND / ( uint32 )APP_targetFPS ) * AUDIO__WRITE_FRAMES )
    flo32  atSample_t[ MAX_SAMPLES_TO_WRITE ];
    uint32 atSample  [ MAX_SAMPLES_TO_WRITE ];
    
    uint8 * mixBuffer;
    int32   mixBuffer_size;
    
    // TODO: AudioSystem should handle interpolation of rate over time
    flo32 rate;
    flo32 volume;
};
#endif

internal AUDIO_DATA
AudioData( void * audio, uint32 samples ) {
    AUDIO_DATA result = { ( uint8 * )audio, samples };
    return result;
}

internal AUDIO_ENTRY
AudioEntry( AUDIO_ID audioID, AUDIO_DATA audio, flo32 volume, flo32 rate, boo32 isLooping ) {
    AUDIO_ENTRY result = {};
    result.audioID   = audioID;
    result.audio     = audio.audio;
    result.nSamples  = audio.samples;
    result.volume    = volume;
    result.rate      = rate;
    result.isLooping = isLooping;
    return result;
}

internal void
advanceAudio( AUDIO_SYSTEM * audioSystem, uint32 samplesToAdvance ) {
    for( uint32 iAudio = 0; iAudio < audioSystem->nAudio; iAudio++ ) {
        audioSystem->audio[ iAudio ].atSample += samplesToAdvance;
    }
    
    int32 newAudioCount = 0;
    for( uint32 iAudio = 0; iAudio < audioSystem->nAudio; iAudio++ ) {
        AUDIO_ENTRY entry = audioSystem->audio[ iAudio ];
        if( entry.atSample < entry.nSamples ) {
            audioSystem->audio[ newAudioCount++ ] = entry;
        } else if( entry.isLooping ) {
            entry.atSample -= entry.nSamples;
            audioSystem->audio[ newAudioCount++ ] = entry;
        }
    }
    audioSystem->nAudio = newAudioCount;
}

internal void
mixAudio( AUDIO_SYSTEM * audioSystem, uint32 samplesToAdvance ) {
    int32 maxSamplesToWrite = ( AUDIO__SAMPLES_PER_SECOND / 60 ) * AUDIO__WRITE_FRAMES;
    
    for( uint32 iAudio = 0; iAudio < audioSystem->nAudio; iAudio++ ) {
        AUDIO_ENTRY * entry = audioSystem->audio + iAudio;
        
        int32 samplesToWrite = entry->nSamples - entry->atSample;
        samplesToWrite = minValue( samplesToWrite, maxSamplesToWrite );
        
        uint16 * src  = ( uint16 * )( entry->audio + ( entry->atSample * AUDIO__BYTES_PER_SAMPLE * AUDIO__CHANNEL_COUNT ) );
        uint16 * dest = ( uint16 * )audioSystem->mixBuffer;
        for( int32 sample_index = 0; sample_index < samplesToWrite; sample_index++ ) {
            int16 value0 = *src++;
            int16 value1 = *src++;
            
            value0 = ( int16 )( ( flo32 )value0 * entry->volume * audioSystem->mainVolume );
            value1 = ( int16 )( ( flo32 )value1 * entry->volume * audioSystem->mainVolume );
            
            *dest++ += value0;
            *dest++ += value1;
        }
    }
}

internal void
playAudio( AUDIO_SYSTEM * audioSystem, AUDIO_DATA * audioList, AUDIO_ID audioID, flo32 volume, flo32 rate = 1.0f, boo32 isLooping = false ) {
    Assert( audioSystem->nAudio < AUDIO__MAX_ENTRY_COUNT );
    audioSystem->audio[ audioSystem->nAudio++ ] = AudioEntry( audioID, audioList[ audioID ], volume, rate, isLooping );
}

internal void
sTopAudio( AUDIO_SYSTEM * audioSystem, AUDIO_ID audioID ) {
    uint32 newCount = 0;
    for( uint32 iAudio = 0; iAudio < audioSystem->nAudio; iAudio++ ) {
        AUDIO_ENTRY entry = audioSystem->audio[ iAudio ];
        if( entry.audioID != audioID ) {
            audioSystem->audio[ newCount++ ] = entry;
        }
    }
    audioSystem->nAudio = newCount;
}

//----------
// music functions
//----------

#if 0
internal void
advanceMusic( MUSIC_SYSTEM * musicSystem, uint32 samplesToAdvance ) {
    flo32 samplesf = ( flo32 )samplesToAdvance * musicSystem->ratePrev;
    int32 samples  = ( int32 )samplesf;
    flo32 t        = samplesf - ( flo32 )samples;
    
    musicSystem->samplesPlayed += samples;
    musicSystem->t              = ( musicSystem->t + t ) - ( flo32 )( ( int32 )musicSystem->t );
    
    if( musicSystem->samplesPlayed > musicSystem->samples ) {
        musicSystem->samplesPlayed = 0;
        musicSystem->t             = 0.0f;
    }
}
#endif

#if 0
internal void
mixMusic( MUSIC_SYSTEM * musicSystem, uint32 samplesToAdvance ) {
    uint32 atSample   = musicSystem->atSample  [ samplesToAdvance ];
    flo32  atSample_t = musicSystem->atSample_t[ samplesToAdvance ];
    
    int16 * dest = ( int16 * )musicSystem->mixBuffer;
    for( int32 iSample = 0; iSample < MAX_SAMPLES_TO_WRITE; iSample++ ) {
        musicSystem->atSample  [ iSample ] = atSample;
        musicSystem->atSample_t[ iSample ] = atSample_t;
        
        flo32  t      = atSample_t;
        uint32 index0 = atSample;
        uint32 index1 = ( index0 + 1 ) % musicSystem->nSamples;
        
        uint32 src0 = ( ( uint32 * )musicSystem->audio )[ index0 ];
        uint32 src1 = ( ( uint32 * )musicSystem->audio )[ index1 ];
        
        int16 lo0 = ( int16 )( src0 ^ 0x0000FFFF );
        int16 lo1 = ( int16 )( src1 ^ 0x0000FFFF );
        
        int16 hi0 = ( int16 )( ( src0 >> 16 ) ^ 0x0000FFFF );
        int16 hi1 = ( int16 )( ( src1 >> 16 ) ^ 0x0000FFFF );
        
        int16 lo = ( int16 )( ( 1.0f - t ) * ( flo32 )lo0 + t * ( flo32 )lo1 );
        int16 hi = ( int16 )( ( 1.0f - t ) * ( flo32 )hi0 + t * ( flo32 )hi1 );
        
        lo = ( int16 )( ( flo32 )lo * musicSystem->volume * musicSystem->mainVolume );
        hi = ( int16 )( ( flo32 )hi * musicSystem->volume * musicSystem->mainVolume );
        
        *dest++ += lo;
        *dest++ += hi;
        
        atSample_t += musicSystem->rate;
        while( atSample_t >= 1.0f ) {
            atSample_t -= 1.0f;
            atSample++;
            if( atSample >= musicSystem->nSamples ) {
                atSample -= musicSystem->nSamples;
            }
        }
    }
}

internal void
playMusic( MUSIC_SYSTEM * musicSystem, AUDIO_DATA music, flo32 volume, flo32 rate ) {
    musicSystem->audio    = music.audio;
    musicSystem->nSamples = music.samples;
    musicSystem->volume   = volume;
    musicSystem->rate     = rate;
    
    memset( musicSystem->atSample, 0, sizeof( flo32 ) * MAX_SAMPLES_TO_WRITE );
}

internal void
setMusicRate( MUSIC_SYSTEM * musicSystem, flo32 rate ) {
    musicSystem->rate = rate;
}
#endif

#define MUSIC__MIX_VOLUME  ( 1.0f )
#define AUDIO__MIX_VOLUME  ( 1.0f )

struct WIN32_AUDIO {
    LPDIRECTSOUNDBUFFER buffer;
    uint32              bufferSize;
    DWORD               prevWrite;
    int32               writeSize;
};

internal WIN32_AUDIO
initAudio( HWND window, AUDIO_SYSTEM * audioSystem, MEMORY * memory ) {         
    uint32 audioBuffer_size = AUDIO__SAMPLES_PER_SECOND * AUDIO__BYTES_PER_SAMPLE * AUDIO__CHANNEL_COUNT * AUDIO__BUFFER_SECONDS;
    LPDIRECTSOUNDBUFFER audioBuffer = {};
    HMODULE audioBuffer_lib = LoadLibrary( "dsound.dll" );
    if( audioBuffer_lib ) {
        DIRECT_SOUND_CREATE * DirectSoundCreate = ( DIRECT_SOUND_CREATE * )GetProcAddress( audioBuffer_lib, "DirectSoundCreate" );
        
        LPDIRECTSOUND DirectSound;
        if( DirectSoundCreate && SUCCEEDED( DirectSoundCreate( 0, &DirectSound, 0 ) ) ) {
            WAVEFORMATEX format = {};
            format.wFormatTag      = WAVE_FORMAT_PCM;
            format.nChannels       = ( WORD )AUDIO__CHANNEL_COUNT;
            format.nSamplesPerSec  = AUDIO__SAMPLES_PER_SECOND;
            format.wBitsPerSample  = ( WORD )( AUDIO__BYTES_PER_SAMPLE * 8 );
            format.nBlockAlign     = ( WORD )( AUDIO__CHANNEL_COUNT * AUDIO__BYTES_PER_SAMPLE );
            format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
            
            if( SUCCEEDED( DirectSound->SetCooperativeLevel( window, DSSCL_PRIORITY ) ) ) {
                DSBUFFERDESC primaryDesc = { sizeof( DSBUFFERDESC ) };
                primaryDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
                
                LPDIRECTSOUNDBUFFER primaryBuffer;
                if(   SUCCEEDED( DirectSound->CreateSoundBuffer( &primaryDesc, &primaryBuffer, 0 ) )
                   && SUCCEEDED( primaryBuffer->SetFormat( &format ) ) ) {
                    OutputDebugString( "Primary Sound Buffer was created successfully.\n" );
                }
            }
            
            DSBUFFERDESC secondaryDesc = { sizeof( DSBUFFERDESC ) };
            secondaryDesc.dwBufferBytes = audioBuffer_size;
            secondaryDesc.lpwfxFormat = &format;
            if( SUCCEEDED( DirectSound->CreateSoundBuffer( &secondaryDesc, &audioBuffer, 0 ) ) ) {
                audioBuffer->Play( 0, 0, DSBPLAY_LOOPING );
                OutputDebugString( "Secondary Sound Buffer was created successfully.\n" );
            }
        }
    }
    
    int32 audio_bytesPerFrame   = ( AUDIO__SAMPLES_PER_SECOND / 60 ) * AUDIO__BYTES_PER_SAMPLE * AUDIO__CHANNEL_COUNT;
    int32 audio_writeSize       = AUDIO__WRITE_FRAMES * audio_bytesPerFrame;
    audioSystem->mixBuffer      = ( uint8 * )_pushSize_clear( memory, audio_writeSize );
    audioSystem->mixBuffer_size = audio_writeSize;
    
    DWORD audio_ignore    = 0;
    DWORD audio_prevWrite = 0;
    audioBuffer->GetCurrentPosition( &audio_ignore, &audio_prevWrite );
    
    WIN32_AUDIO result = {};
    result.buffer     = audioBuffer;
    result.bufferSize = audioBuffer_size;
    result.prevWrite  = audio_prevWrite;
    result.writeSize  = audio_writeSize;
    return result;
}

internal void
setVolume( AUDIO_ENTRY * audio, flo32 targetVolume, flo32 targetTime, flo32 delay ) {
    audio->targetVolume       = targetVolume;
    audio->targetVolume_dMax  = fabsf( ( targetVolume - audio->volume ) / ( targetTime * 60.0f ) );
    audio->targetVolume_delay = delay;
}

internal void
setVolume( AUDIO_SYSTEM * audioSystem, AUDIO_ID audioID, flo32 targetVolume, flo32 targetTime, flo32 delay ) {
    for( uint32 iAudio = 0; iAudio < audioSystem->nAudio; iAudio++ ) {
        AUDIO_ENTRY * audio = audioSystem->audio + iAudio;
        if( audio->audioID == audioID ) {
            setVolume( audio, targetVolume, targetTime, delay );
        }
    }
}

internal void
updateVolume( AUDIO_SYSTEM * audioSystem, flo32 dt ) {
    for( uint32 iAudio = 0; iAudio < audioSystem->nAudio; iAudio++ ) {
        AUDIO_ENTRY * entry = audioSystem->audio + iAudio;
        
        if( entry->targetVolume_delay > 0.0f ) {
            entry->targetVolume_delay -= dt;
        } else {
            flo32 dVolume = entry->targetVolume - entry->volume;
            flo32 dMax    = entry->targetVolume_dMax;
            dVolume = clamp( dVolume, -dMax, dMax );
            
            entry->volume += dVolume;
        }
    }
}

internal void
updateAudio( WIN32_AUDIO * audio, AUDIO_SYSTEM * audioSystem ) {
    LPDIRECTSOUNDBUFFER buffer = audio->buffer;
    
    void * regionA   = 0;
    void * regionB   = 0;
    DWORD  sizeA     = 0;
    DWORD  sizeB     = 0;
    DWORD  currRead  = 0;
    DWORD  currWrite = 0;
    
    if( ( buffer->GetCurrentPosition( &currRead, &currWrite ) == DS_OK ) &&
       ( currWrite != audio->prevWrite ) &&
       ( buffer->Lock( currWrite, audio->writeSize, &regionA, &sizeA, &regionB, &sizeB, 0 ) == DS_OK ) ) {
        uint32 bytesToAdvance = minValue( ( ( currWrite - audio->prevWrite ) + audio->bufferSize ) % audio->bufferSize, audio->writeSize );
        uint32 samplesToAdvance = bytesToAdvance / ( AUDIO__BYTES_PER_SAMPLE * AUDIO__CHANNEL_COUNT );
        
        memset( audioSystem->mixBuffer, 0, audioSystem->mixBuffer_size );
        // #if DEBUG_BUILD
        // if( !globalVar_profileSystem.showProfile ) {
        // #endif // DEBUG_BUILD
        //advanceMusic( musicSystem, samplesToAdvance );
        advanceAudio( audioSystem, samplesToAdvance );
        mixAudio    ( audioSystem, samplesToAdvance );
        
        // #if DEBUG_BUILD
        // }
        // #endif // DEBUG_BUILD
        memcpy( regionA, audioSystem->mixBuffer,         sizeA );
        memcpy( regionB, audioSystem->mixBuffer + sizeA, sizeB );
        
        buffer->Unlock( regionA, sizeA, regionB, sizeB );
        audio->prevWrite = currWrite;
    }
}