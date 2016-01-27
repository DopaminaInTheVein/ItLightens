#include "mcv_platform.h"
#include "shaders.h"
#include "vertex_declarations.h"
#include <d3dcompiler.h>

#pragma comment(lib, "D3DCompiler.lib" )

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
bool compileShaderFromFile(
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
    if (pErrorBlob) pErrorBlob->Release();
    return false;
  }
  if (pErrorBlob) pErrorBlob->Release();
  return true;
}

void CVertexShader::destroy() {
  SAFE_RELEASE(vs);
  SAFE_RELEASE(vertex_layout);
}

bool CVertexShader::create(
    const char* fx_filename
  , const char* entry_point
  , const CVertexDeclaration* vtx_decl
  ) {
  HRESULT hr;

  assert(fx_filename);
  assert(entry_point);
  assert(vtx_decl);
  
  // Compilar shader
  ID3DBlob* pBlob = NULL;
  if( !compileShaderFromFile(fx_filename, entry_point, "vs_4_0", &pBlob))
    return false;

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
      , vtx_decl->name, entry_point, fx_filename);
    return false;
  }

  return true;
}

void CVertexShader::activate() {
  assert(vs);
  assert(vertex_layout);
  Render.ctx->VSSetShader(vs, NULL, 0);
  Render.ctx->IASetInputLayout(vertex_layout);
}



// --------------------------------------------
bool CPixelShader::create(
  const char* fx_filename
  , const char* entry_point
  ) {
  HRESULT hr;

  // Compilar shader
  ID3DBlob* pBlob = NULL;
  if (!compileShaderFromFile(fx_filename, entry_point, "ps_4_0", &pBlob))
    return false;

  // Create the vertex shader
  hr = Render.device->CreatePixelShader(
    pBlob->GetBufferPointer()
    , pBlob->GetBufferSize()
    , NULL
    , &ps);
  SAFE_RELEASE(pBlob);
  if (FAILED(hr))
    return false;

  return true;
}

void CPixelShader::destroy() {
  SAFE_RELEASE(ps);
}

void CPixelShader::activate() {
  assert(ps);
  Render.ctx->PSSetShader(ps, NULL, 0);
}

