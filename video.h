
struct Video
{
	void loadFromImage(const String &filename);
	void loadFromDirectory(const String &directory);

	//UINT Width() const  { return frames[0].width();  }
	//UINT Height() const { return frames[0].height(); }

	//Vector<Vec3f> ComputePaletteKMeans(UINT paletteSize) const;
	//Vector<Vec3f> ComputeFrame0Palette( const Vector<UINT>& frameids, const Vector<String> &filename ) const;

	UINT width, height;
	Vector<Bitmap> frames;
};