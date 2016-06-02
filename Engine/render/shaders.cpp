#include "mcv_platform.h"
#include "shaders.h"
#include "vertex_declarations.h"
#include <d3dcompiler.h>
#include "resources/resources_manager.h"
#include "DirectXHelpers.h"

#pragma comment(lib, "D3DCompiler.lib" )

// ----------------------------------
// name = solid_colored.vs
template<> IResource::eType getTypeOfResource<CVertexShader>() { return IResource::VERTEX_SHADER; }
template<>
IResource* createObjFromName<CVertexShader>(const std::string& name) {
  fatal("Should not create vertex shaders directly. Use a tech");
  return nullptr;
}

// ----------------------------------
// name = solid_colored.ps
template<> IResource::eType getTypeOfResource<CPixelShader>() { return IResource::PIXEL_SHADER; }
template<>
IResource* createObjFromName<CPixelShader>(const std::string& name) {
  fatal("Should not create pixel shaders directly. Use a tech");
  return nullptr;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
bool compileShaderFromFileReal(
    const char* szFileName
  , const char* szEntryPoint
  , const char* szShaderModel
  , ID3DBlob** ppBlobOut)
{
  HRESULT hr = S_OK;

  DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
  // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
  // Setting this flag improves the shader debugging experience, but still allows 
  // the shaders to be optimized and to run exactly the way they will run in 
  // the release configuration of this program.
  dwShaderFlags |= D3DCOMPILE_DEBUG;
  
#endif
  // To avoid having to upload the matrix as transposed matrixs
  dwShaderFlags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

  WCHAR wFilename[MAX_PATH];
  ::mbstowcs(wFilename, szFileName, MAX_PATH);

  ID3DBlob* pErrorBlob = nullptr;
  hr = D3DCompileFromFile(
    wFilename
    , nullptr
    , D3D_COMPILE_STANDARD_FILE_INCLUDE
    , szEntryPoint
    , szShaderModel
    , dwShaderFlags
    , 0
    , ppBlobOut
    , &pErrorBlob);
  if (FAILED(hr))
  {
    const char* err = "unknown";
    if (pErrorBlob != NULL) {
      err = (char*)pErrorBlob->GetBufferPointer();
    }
    else if (hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND)) {
      err = "File not found.";
    }
    dbg("Error compiling shader %s @ %s Profile %s:\n%s\n"
      , szEntryPoint
      , szFileName
      , szShaderModel
      , err );
    MessageBox(NULL, err, szEntryPoint, MB_OK);
    if (pErrorBlob) pErrorBlob->Release();
    return false;
  }
  if (pErrorBlob) pErrorBlob->Release();
  return true;
}

bool compileShaderFromFile(
  const char* szFileName
  , const char* szEntryPoint
  , const char* szShaderModel
  , ID3DBlob** ppBlobOut)
{
  while (true) {
    bool is_ok = compileShaderFromFileReal(szFileName, szEntryPoint, szShaderModel, ppBlobOut);
    if (is_ok)
      return true;
  }
  return false;
}




void CVertexShader::destroy() {
  SAFE_RELEASE(vs);
  SAFE_RELEASE(vertex_layout);
}

bool CVertexShader::create(
    const char* fx_filename
  , const char* new_entry_point
  , const CVertexDeclaration* new_vtx_decl
  ) {
  src_fx_filename = fx_filename;
  entry_point = new_entry_point;
  vtx_decl = new_vtx_decl;

  HRESULT hr;

  assert(fx_filename);
  assert(!entry_point.empty());
  assert(vtx_decl);
  
  // Compilar shader
  ID3DBlob* pBlob = NULL;
  if( !compileShaderFromFile(fx_filename, entry_point.c_str(), "vs_4_0", &pBlob))
    return false;

  // In case we are reloading....
  destroy();

  // Create the vertex shader
  hr = Render.device->CreateVertexShader(
    pBlob->GetBufferPointer()
    , pBlob->GetBufferSize()
    , NULL
    , &vs);
  if (FAILED(hr))
  {
    SAFE_RELEASE(pBlob);
    return false;
  }

  // Create the input layout
  hr = Render.device->CreateInputLayout(vtx_decl->elems, vtx_decl->nelems, pBlob->GetBufferPointer(),
    pBlob->GetBufferSize(), &vertex_layout);
  SAFE_RELEASE(pBlob);
  if (FAILED(hr)) {
    fatal("Vertex declaration %s given for vtx shader %s@%s is not compatible."
      , vtx_decl->name, entry_point.c_str(), fx_filename);
    return false;
  }
  setDXName(vertex_layout, vtx_decl->name);

  std::string dx_name = entry_point + std::string("@") + fx_filename;
  setDXName(vs, dx_name.c_str());

  return true;
}

void CVertexShader::activate() const {
  assert(vs);
  assert(vertex_layout);
  Render.ctx->VSSetShader(vs, NULL, 0);
  Render.ctx->IASetInputLayout(vertex_layout);
}

// --------------------------------------------
bool CPixelShader::create(
  const char* fx_filename
  , const char* new_entry_point
  ) {
  src_fx_filename = fx_filename;
  entry_point = new_entry_point;

  HRESULT hr;

  // Compilar shader
  ID3DBlob* pBlob = NULL;
  if (!compileShaderFromFile(fx_filename, entry_point.c_str(), "ps_4_0", &pBlob))
    return false;

  ID3D11PixelShader *new_ps = nullptr;

  // Create the vertex shader
  hr = Render.device->CreatePixelShader(
    pBlob->GetBufferPointer()
    , pBlob->GetBufferSize()
    , NULL
    , &new_ps);
  SAFE_RELEASE(pBlob);
  if (FAILED(hr)) {
    dbg("CreatePixelShader failed\n");
    return false;
  }

  // In case we are reloading....
  destroy();
  
  ps = new_ps;

  std::string dx_name = entry_point + std::string("@") + fx_filename;
  setDXName(ps, dx_name.c_str());

  return true;
}

void CPixelShader::destroy() {
  SAFE_RELEASE(ps);
}

void CPixelShader::activate() const {
  assert(ps);
  Render.ctx->PSSetShader(ps, NULL, 0);
}

void CPixelShader::onFileChanged(const std::string& filename) {
  if (filename == src_fx_filename) {
    create(src_fx_filename.c_str(), entry_point.c_str());
  }
}

void CVertexShader::onFileChanged(const std::string& filename) {
  if (filename == src_fx_filename) {
    create(src_fx_filename.c_str(), entry_point.c_str(), vtx_decl);
  }
}