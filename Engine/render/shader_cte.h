#ifndef INC_SHADER_CTES_H_
#define INC_SHADER_CTES_H_

template< typename TObjParams >
class CShaderCte : public TObjParams {
	ID3D11Buffer*  constant_buffer;
public:
	CShaderCte()
		: constant_buffer(nullptr) { }

	void destroy() {
		SAFE_RELEASE(constant_buffer);
	}

	bool create(const char* name) {
		D3D11_BUFFER_DESC bd;
		memset(&bd, 0x00, sizeof(bd));
		// Create the constant buffer
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(TObjParams);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		HRESULT hr = Render.device->CreateBuffer(&bd, NULL, &constant_buffer);
		if (FAILED(hr))
			return false;
		setDXName(constant_buffer, name);
		return true;
	}

	void activate(int slot) {
		assert(constant_buffer);
		Render.ctx->VSSetConstantBuffers(slot, 1, &constant_buffer);
		Render.ctx->PSSetConstantBuffers(slot, 1, &constant_buffer);
	}

	void uploadToGPU() {
		//PROFILE_FUNCTION("upload GPU: upload gpu");
		Render.ctx->UpdateSubresource(constant_buffer, 0, NULL, (TObjParams*)this, 0, 0);
	}
};

#endif
