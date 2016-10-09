#ifndef INC_MESH_FORMAT_H_
#define INC_MESH_FORMAT_H_

namespace MeshFormat {
	struct TRiff {
		uint32_t magic;
		uint32_t num_bytes;
	};

	static const uint32_t magic_header = 0x44221100;
	static const uint32_t magic_vtxs = 0x44221101;
	static const uint32_t magic_idxs = 0x44221102;
	static const uint32_t magic_groups = 0x44221103;
	static const uint32_t magic_mesh_end = 0x44221144;
	static const uint32_t magic_terminator = 0x44222200;

	struct THeader {
		uint32_t version;
		uint32_t num_vtxs;
		uint32_t num_idxs;
		uint32_t primitive_type;
		uint32_t vertex_type;
		uint32_t bytes_per_vtx;
		uint32_t bytes_per_idx;
		uint32_t num_groups;
		uint32_t the_magic_terminator;
		static const uint32_t current_version = 1;
		bool isValid() const {
			return version == current_version && (the_magic_terminator == magic_terminator);
		}
	};
};

#endif
