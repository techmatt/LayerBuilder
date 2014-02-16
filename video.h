
struct Bitmap
{
	// this needs to be moved into mLib
};

struct RGBColor
{
	// this needs to be moved into mLib
};

struct Video
{
	void loadFromDirectory(const String &s);
	void loadFromImage(const String &s);

	//UINT Width() const  { return frames[0].width();  }
	//UINT Height() const { return frames[0].height(); }

	//Vector<Vec3f> ComputePaletteKMeans(UINT paletteSize) const;
	//Vector<Vec3f> ComputeFrame0Palette( const Vector<UINT>& frameids, const Vector<String> &filename ) const;

	Vector<Bitmap> frames;
};