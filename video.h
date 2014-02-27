
struct Video
{
	void loadFromImage(const std::string &filename);
	void loadFromDirectory(const std::string &directory);

	//UINT Width() const  { return frames[0].width();  }
	//UINT Height() const { return frames[0].height(); }

	//Vector<Vec3f> ComputePaletteKMeans(UINT paletteSize) const;
	//Vector<Vec3f> ComputeFrame0Palette( const Vector<UINT>& frameids, const Vector<std::string> &filename ) const;

	UINT width, height;
	Vector<Bitmap> frames;
};