
internal void
UploadTextureFromFile( PLATFORM * Platform, TEXTURE_ID TextureID, char * filename ) {
    TEXTURE_DATA Texture = BMP_readFile( Platform, "../../art", filename );
    UploadTexture( &Platform->Renderer, TextureID, Texture.width, Texture.Height, Texture.texelData );
}