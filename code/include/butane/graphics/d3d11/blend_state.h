// This file is part of Butane. See README.md and LICENSE.md for details.
// Copyright (c) 2012 Michael Williams <devbug@bitbyte.ca>

#ifndef _BUTANE_D3D11_BLEND_STATE_H_
#define _BUTANE_D3D11_BLEND_STATE_H_

#include <butane/graphics/d3d11/d3d11.h>
#include <butane/graphics/blend_state.h>

namespace butane {
  class BUTANE_EXPORT D3D11BlendState final
    : public BlendState
  {
    __foundation_trait(D3D11BlendState, non_copyable);

    private:
      friend class BlendState;

    protected:
      D3D11BlendState();
      ~D3D11BlendState();

    public:
      void destroy() override;

    public:
      FOUNDATION_INLINE ID3D11BlendState* interface()
      { return _interface; }

    private:
      ID3D11BlendState* _interface;
  };
} // butane

#endif // _BUTANE_D3D11_BLEND_STATE_H_
