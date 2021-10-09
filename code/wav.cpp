
#pragma pack( push, 1 )
struct WAV_HEADER {
	uint32 ckID;
	uint32 ckSize;
	uint32 waveID;
};

struct BLOCK_HEADER {
	uint32 marker;
	uint32 size;
};

struct FMT_BLOCK {
	uint16 format;
	uint16 channels;
	uint32 sampleRate;
	uint32 avgDataRate;
	uint16 blockAlign;
	uint16 bitsPerSample;
	uint16 extensionSize;
};
#pragma pack( pop )

internal AUDIO_DATA
WAV_readFile( PLATFORM * Platform, char * filename ) {
	AUDIO_DATA result = {};
	FILE_DATA file = Platform->readFile( &Platform->TempMemory, 0, filename, "wav" );
	if( file.contents ) {
		uint32 riffTag = *( ( uint32 * )&"RIFF" );
		uint32 waveTag = *( ( uint32 * )&"WAVE" );
		uint32  fmtTag = *( ( uint32 * )&"fmt " );
		uint32 dataTag = *( ( uint32 * )&"data" );
		
		uint8 * start = ( uint8 * )file.contents;
		uint8 * at = start;
		
		WAV_HEADER * header = _addr( at, WAV_HEADER );
		Assert( header->ckID == riffTag );
		Assert( header->waveID == waveTag );
		
		BLOCK_HEADER * blockA = _addr( at, BLOCK_HEADER );
		Assert( blockA->marker == fmtTag );
		
		FMT_BLOCK * fmt = _addr( at, FMT_BLOCK );
		Assert( fmt->format == 1 ); // PCM format
		// Assert( fmt->channels == 1 );
		Assert( fmt->channels == 2 );
		Assert( fmt->sampleRate == 48000 );
		Assert( fmt->bitsPerSample == 16 );
		
		BLOCK_HEADER * blockB = _addr( at, BLOCK_HEADER );
		Assert( blockB->marker == dataTag );
		
		uint8 * src = at;
		uint8 * dest = ( uint8 * )_pushSize( &Platform->permMemory, blockB->size );
		memcpy( dest, src, blockB->size );
		
		Assert( blockB->size % ( ( fmt->bitsPerSample / 8 ) * fmt->channels ) == 0 );
		uint32 samples = blockB->size / ( ( fmt->bitsPerSample / 8 ) * fmt->channels );
		
		result = AudioData( dest, samples );
	}
	return result;
}