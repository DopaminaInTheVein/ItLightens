#ifndef INC_COMP_VIDEO_H_
#define	INC_COMP_VIDEO_H_

#include "comp_base.h"
#include <dshow.h>

class TCompVideo : public TCompBase {
	IGraphBuilder *pGraph;
	IMediaControl *pControl;
	IMediaEvent   *pEvent;
	//IVideoWindow  *pVidWin;
	IMediaSeeking *pSeek;
	IBaseFilter* pVmr;
	IVMRWindowlessControl* pWc;
	IVMRFilterConfig* pConfig;
	HDC         hdc;

	std::string file;
	bool loaded = false;

public:
	bool load(MKeyValue& atts);
	void update(float dt);
	void init();
	void ReleaseAll();
};

#endif