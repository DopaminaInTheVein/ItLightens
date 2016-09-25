#ifndef INC_PROFILING_H_
#define INC_PROFILING_H_

#define PROFILING_JOHN
#define PROFILING_ENABLED

#include <cinttypes>
#ifdef PROFILING_ENABLED

#ifdef PROFILING_JOHN
// Profiling John
class CProfiler {
	struct TEntry {
		uint64_t     time_stamp;
		const char*  name;
		bool isBegin() const;
	};

	TEntry*  entries;
	uint32_t nentries;
	uint32_t max_entries;
	bool     is_capturing;
	uint32_t nframes_to_capture;

	void create(uint32_t amax_entries) {
		max_entries = amax_entries;
		nentries = 0;
		entries = new TEntry[max_entries];
	}

	void destroy() {
		delete[] entries;
		max_entries = 0;
	}

	void saveResults();

public:

	CProfiler()
		: entries(nullptr)
		, nentries(0)
		, max_entries(0)
	{ }

	__forceinline uint32_t enter(const char* name) {
		auto* e = entries + nentries;
		e->time_stamp = __rdtsc() & (~1ULL);
		e->name = name;
		nentries = (nentries + 1) & (max_entries - 1);
		return nentries - 1;
	}
	__forceinline void exit(uint32_t entrance_idx) {
		auto* e = entries + nentries;
		e->time_stamp = __rdtsc() | (1ULL);
		e->name = entries[entrance_idx].name;
		nentries = (nentries + 1) & (max_entries - 1);
	}

	void setNFramesToCapture(uint32_t new_n);
	void beginFrame();
};

extern CProfiler profiler;

// ------------------------------------
struct TCPUScoped {
	uint32_t n;
	TCPUScoped(const char* txt) {
		n = profiler.enter(txt);
	}
	~TCPUScoped() {
		profiler.exit(n);
	}
};
#else
//Profiling autocapture
class CProfiler {
	struct TEntry {
		uint64_t     time_stamp;
		const char*  name;
		bool isBegin() const;
	};

	TEntry*  entries;
	uint32_t nentries;
	uint32_t max_entries;
	bool     is_capturing;
	bool	 auto_capture;
	uint32_t nframes_to_capture;
	float time_threshold;
	std::vector<float> times = std::vector<float>();

	void create(uint32_t amax_entries) {
		max_entries = amax_entries;
		nentries = 0;
		entries = new TEntry[max_entries];
	}

	void destroy() {
		delete[] entries;
		max_entries = 0;
	}

	void saveResults();

public:

	CProfiler()
		: entries(nullptr)
		, nentries(0)
		, max_entries(0)
	{ }

	__forceinline uint32_t enter(const char* name) {
		auto* e = entries + nentries;
		e->time_stamp = __rdtsc() & (~1ULL);
		e->name = name;
		nentries = (nentries + 1) & (max_entries - 1);
		return nentries - 1;
	}
	__forceinline void exit(uint32_t entrance_idx) {
		auto* e = entries + nentries;
		e->time_stamp = __rdtsc() | (1ULL);
		e->name = entries[entrance_idx].name;
		nentries = (nentries + 1) & (max_entries - 1);
	}

	void setNFramesToCapture(uint32_t new_n);
	void setAutoCapture(uint32_t new_n, float time_threshold = 2.f);
	void beginFrame();
	bool hasToSave();
	bool isAutoCapture();
};

extern CProfiler profiler;

// ------------------------------------
struct TCPUScoped {
	uint32_t n;
	TCPUScoped(const char* txt) {
		n = profiler.enter(txt);
	}
	~TCPUScoped() {
		profiler.exit(n);
	}
};
#endif //PROFILING_JOHN


#define PROFILE_FUNCTION(txt)  TCPUScoped profiled_scoped(txt)
#define PROFILE_FRAME_BEGINS()   profiler.beginFrame()

#else
 //Profile disabled
#define PROFILE_FUNCTION(txt)  
#define PROFILE_FRAME_BEGINS()   

#endif //PROFILING_ENABLED

#endif //INC_PROFILING_H