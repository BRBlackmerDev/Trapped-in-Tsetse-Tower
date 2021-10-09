
internal flo32
minValue( flo32 a, flo32 b ) {
	flo32 result = ( ( a < b ) ? a : b );
	return result;
}

internal flo32
maxValue( flo32 a, flo32 b ) {
	flo32 result = ( ( a > b ) ? a : b );
	return result;
}

internal int32
minValue( int32 a, int32 b ) {
	int32 result = ( ( a < b ) ? a : b );
	return result;
}

internal int32
maxValue( int32 a, int32 b ) {
	int32 result = ( ( a > b ) ? a : b );
	return result;
}

internal flo32
absValue( flo32 value ) {
	flo32 result = ( value < 0.0f ? -value : value );
	return result;
}

inline flo32
squareValue( flo32 value ) {
	flo32 result = value * value;
	return result;
}