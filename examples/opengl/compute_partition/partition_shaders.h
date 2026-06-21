#ifndef __PARTITION_SHADERS_H__
#define __PARTITION_SHADERS_H__

namespace partition_flags_shaders {

static const char* kComputeSource = R"(
#version 430 core

layout(local_size_x = 256) in;

layout (std430, binding = 0) readonly buffer SourceColors { vec4 sourceColors[]; };
layout (std430, binding = 1) writeonly buffer Flags { uint flags[]; };

uniform uint uCount;

void main()
{
	uint i = gl_GlobalInvocationID.x;
	if (i >= uCount)
		return;
	// stable partition keeps opaque items first
	bool opaque = sourceColors[i].a >= 1.0;
	flags[i] = opaque ? 1u : 0u;
}
)";

} // namespace partition_flags_shaders

namespace partition_scan_shaders {

static const char* kComputeSource = R"(
#version 430 core

layout(local_size_x = 1024) in;            // == branching factor B

layout(std430, binding = 0) buffer Data           { uint gData[]; };   // in-place
layout(std430, binding = 1) writeonly buffer Sums { uint gSums[]; };

uniform uint uCount;

shared uint s[1024];

void main()
{
	uint tid = gl_LocalInvocationID.x;
	uint gid = gl_GlobalInvocationID.x;

	uint v = (gid < uCount) ? gData[gid] : 0u;
	s[tid] = v;
	barrier();

	// Hillis-Steele inclusive scan within the block
	for (uint off = 1u; off < 1024u; off <<= 1u)
	{
		uint t = (tid >= off) ? s[tid - off] : 0u;
		barrier();                         // all reads done before writes
		s[tid] += t;
		barrier();                         // all writes visible before next read
	}

	uint inclusive = s[tid];
	if (gid < uCount)
		gData[gid] = inclusive - v;        // exclusive prefix within the block
	if (tid == 1023u)
		gSums[gl_WorkGroupID.x] = inclusive; // block total (padded lanes contribute 0)
}
)";

} // namespace partition_scan_shaders

namespace partition_add_shaders {

static const char* kComputeSource = R"(
#version 430 core
layout(local_size_x = 1024) in;            // must match scan block size

layout(std430, binding = 0) buffer Data            { uint gData[]; };   // level prefixes, in-place
layout(std430, binding = 1) readonly buffer Parent { uint gParent[]; }; // scanned block totals

uniform uint uCount;

void main()
{
	uint gid = gl_GlobalInvocationID.x;
	if (gid >= uCount)
		return;
	gData[gid] += gParent[gl_WorkGroupID.x]; // workgroup id == gid / 1024
}
)";

} // namespace partition_add_shaders

namespace partition_scatter_shaders {

static const char* kComputeSource = R"(
#version 430 core
layout(local_size_x = 256) in;

layout(std430, binding = 0) readonly buffer OpaquePrefix        { uint opaquePrefix[]; };   // exclusive scan
layout(std430, binding = 1) readonly buffer GrandTotal          { uint grandTotal[]; };     // [0] = numOpaque
layout(std430, binding = 2) readonly buffer SourceIndices       { uint sourceIndices[]; };
layout(std430, binding = 3) writeonly buffer FilteredIndices    { uint filteredIndices[]; };
layout(std430, binding = 4) readonly buffer SourceColors        { vec4 sourceColors[]; };
layout(std430, binding = 5) writeonly buffer FilteredColors     { vec4 filteredColors[]; };

uniform uint uCount;

void main()
{
    uint i = gl_GlobalInvocationID.x;
    if (i >= uCount)
        return;

    bool opaque = sourceColors[i].a >= 1.0;
    uint numOpaque = grandTotal[0];
    uint before  = opaquePrefix[i];                          // opaque count before i

    // stable: opaque -> [0,numOpaque), transparent -> [numOpaque,N), both in source order
    uint pos = opaque ? before : (numOpaque + (i - before));

    filteredIndices[pos] = sourceIndices[i];
    filteredColors[pos] = sourceColors[i];
}
)";

} // namespace partition_scatter_shaders

#endif