//#include "mcv_platform.h"
//#include "profiling.h"
//
//// use chrome with url: chrome://tracing/
//
//CProfiler profiler;
//
//bool CProfiler::TEntry::isBegin() const {
//	return (time_stamp & 1ULL) == 0;
//}
//
//void CProfiler::setNFramesToCapture(uint32_t new_n) {
//	nframes_to_capture = new_n;
//	is_capturing = false;
//}
//
//void CProfiler::beginFrame() {
//
//	if (max_entries == 0)
//		create(1 << 16);
//
//	if (!is_capturing) {
//		if (nframes_to_capture) {
//			is_capturing = true;
//		}
//		else {
//			// el profiler no esta activo
//		}
//		nentries = 0;
//	}
//	else {
//		// Estamos acabando el �ltimo frame de los 
//		// que queria calcular
//		if (nframes_to_capture == 1) {
//			is_capturing = false;
//			saveResults();
//			nentries = 0;
//		}
//		--nframes_to_capture;
//	}
//}
//
//void CProfiler::saveResults() {
//	const char* ofilename = "results.cpuprofile";
//	FILE* f = fopen(ofilename, "wb");
//	if (!f)
//		return;
//
//	// Query the freq to convert cycles to seconds
//	LARGE_INTEGER freq;
//	QueryPerformanceFrequency(&freq);
//
//	// The minimum time of all records
//	uint64_t t0 = entries[0].time_stamp;
//
//	fprintf(f, "{\"traceEvents\": [");
//
//	uint32_t pid = 1234;    // Invented process id
//	uint32_t thread_id = 5588;
//	const char* thread_name = "main_thread";
//	int n = 0;
//	for (uint32_t i = 0; i < nentries; ++i, ++n) {
//		const TEntry* e = entries + i;
//		if (n)
//			fprintf(f, ",");
//
//		// Convert cycles to s
//		assert(e->time_stamp >= t0);
//		uint64_t ts = e->time_stamp - t0;
//		ts *= 1000000;
//		ts /= freq.QuadPart;
//
//		if (e->isBegin()) {
//			fprintf(f, "{\"name\":\"%s\", \"cat\":\"c++\"", e->name);
//			fprintf(f, ",\"ph\":\"B\",\"ts\": %lld, \"pid\":%d, \"tid\" : %d }\n", ts, pid, thread_id);
//		}
//		else {
//			fprintf(f, "{\"ph\":\"E\",\"ts\": %lld, \"pid\":%d, \"tid\" : %d }\n", ts, pid, thread_id);
//		}
//	}
//	if (nentries) {
//		fprintf(f, ",{\"name\": \"thread_name\", \"ph\": \"M\", \"pid\": %d, \"tid\": %d, \"args\": {\"name\":\"%s\" }}\n"
//			, pid, thread_id, thread_name);
//	}
//	fprintf(f, "]}");
//
//	fclose(f);
//}




//--- Cristian autocapture some bug
#include "mcv_platform.h"
#include "profiling.h"

#ifdef PROFILING_ENABLED

CProfiler profiler;

bool CProfiler::TEntry::isBegin() const {
	return (time_stamp & 1ULL) == 0;
}

void CProfiler::setNFramesToCapture(uint32_t new_n) {
	nframes_to_capture = new_n;
	is_capturing = auto_capture = false;
}

void CProfiler::setAutoCapture(uint32_t new_n, float t_threshold)
{
	nframes_to_capture = new_n;
	is_capturing = false;
	auto_capture = true;
	time_threshold = t_threshold;
}

void CProfiler::beginFrame() {
	if (max_entries == 0)
		create(1 << 16);

	if (!is_capturing) {
		if (nframes_to_capture) {
			is_capturing = true;
			if (auto_capture) {
				times = std::vector<float>(nframes_to_capture);
			}
		}
		else {
			// profiler manual desactivado
		}
		nentries = 0;
	}
	else {
		if (auto_capture) times[nframes_to_capture - 1] = getDeltaTime(true);
		// Estamos acabando el último frame de los
		// que queria calcular
		if (nframes_to_capture == 1) {
			if (hasToSave()) {
				saveResults();
				is_capturing = auto_capture = false;
			}
			else {
				nframes_to_capture = times.size() + 1;
			}
			nentries = 0;
		}
		--nframes_to_capture;
	}
}

bool CProfiler::hasToSave() {
	if (!auto_capture) return true;
	float min = FLT_MAX;
	float max = FLT_MIN;
	for (auto t : times) {
		if (t < min) min = t;
		if (t > max) max = t;
	}
	dbg("Min: %f, Max: %f\n", min, max);
	return max / min > time_threshold;
}

bool CProfiler::isAutoCapture() {
	return auto_capture;
}

void CProfiler::saveResults() {
	const char* ofilename = auto_capture ? "auto_capture.cpuprofile" : "results.cpuprofile";
	FILE* f = fopen(ofilename, "wb");
	if (!f)
		return;

	// Query the freq to convert cycles to seconds
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	// The minimum time of all records
	uint64_t t0 = entries[0].time_stamp;

	fprintf(f, "{\"traceEvents\": [");

	uint32_t pid = 1234;    // Invented process id
	uint32_t thread_id = 5588;
	const char* thread_name = "main_thread";
	int n = 0;
	for (uint32_t i = 0; i < nentries; ++i, ++n) {
		const TEntry* e = entries + i;
		if (n)
			fprintf(f, ",");

		// Convert cycles to s
		assert(e->time_stamp >= t0);
		uint64_t ts = e->time_stamp - t0;
		ts *= 1000000;
		ts /= freq.QuadPart;

		if (e->isBegin()) {
			fprintf(f, "{\"name\":\"%s\", \"cat\":\"c++\"", e->name);
			fprintf(f, ",\"ph\":\"B\",\"ts\": %lld, \"pid\":%d, \"tid\" : %d }\n", ts, pid, thread_id);
		}
		else {
			fprintf(f, "{\"ph\":\"E\",\"ts\": %lld, \"pid\":%d, \"tid\" : %d }\n", ts, pid, thread_id);
		}
	}
	if (nentries) {
		fprintf(f, ",{\"name\": \"thread_name\", \"ph\": \"M\", \"pid\": %d, \"tid\": %d, \"args\": {\"name\":\"%s\" }}\n"
			, pid, thread_id, thread_name);
	}
	fprintf(f, "]}");

	fclose(f);
}

#endif