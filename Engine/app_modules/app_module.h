#ifndef INC_APP_MODULES_H_
#define INC_APP_MODULES_H_

class IAppModule {
public:
	bool enabled;
	IAppModule() : enabled(true) { }
	virtual bool start() { return true; }
	virtual void stop() {}
	virtual void update(float dt) {}
	virtual void render() {}
	virtual bool onSysMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		return true;
	}
	//virtual void renderDebug2D();
	virtual bool forcedUpdate() { return false; }
	virtual const char* getName() const = 0;
};

#endif
