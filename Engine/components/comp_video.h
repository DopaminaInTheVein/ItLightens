#ifndef INC_COMP_VIDEO_H_
#define	INC_COMP_VIDEO_H_

#include <atlbase.h>
#include <atlcom.h>
#include <atlhost.h>
#include <atlctl.h>

#include "wmp.h"
#include "comp_base.h"

class TCompVideo : public TCompBase {
	CAxWindow  m_wndView;  // ActiveX host window class.
	CComPtr<IWMPPlayer>  m_spWMPPlayer;  // Smart pointer to IWMPPlayer interface.
	std::string file;

public:
	bool load(MKeyValue& atts);
	void update(float dt);
	void init();
};

#endif