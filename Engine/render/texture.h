#ifndef INC_RENDER_TEXTURE_H_
#define INC_RENDER_TEXTURE_H_

class CTexture : public IResource {
	std::string               name;
	ID3D11Resource*           resource;
	ID3D11ShaderResourceView* res_view;
public:

	void renderUIDebug();

	CTexture() : resource(nullptr), res_view(nullptr)
	{ }

	void setName(const char* new_name) { name = new_name; }
	const std::string& getName() const override {
		return name;
	}
	bool isValid() const override {
		return res_view != nullptr;
	}
	void destroy() override {
		SAFE_RELEASE(resource);
		SAFE_RELEASE(res_view);
	}
	eType getType() const override { return TEXTURE; }

	bool load(const char* filename);

	void activate(int slot) const;
};

#endif
