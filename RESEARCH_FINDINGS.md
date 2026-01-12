# Research Findings: Other ESP32 DOOM Ports

## Analysis Date
2025-01-XX

## Primary Source: jkirsons/doom-espidf
**Repository:** https://github.com/jkirsons/doom-espidf (83 stars, 23 forks)  
**Engine:** PrBoom (enhanced DOOM)  
**Target:** ESP32 with PSRAM  

### CRITICAL LIMITATION DISCOVERED
**jkirsons uses external PSRAM (SPIRAM) for all allocations:**
```c
// z_zone.c:392-418
while (!(block = (heap_caps_malloc)(size + HEADER_SIZE, MALLOC_CAP_SPIRAM))) {
```

**Impact:** This approach is NOT applicable to M5StickC Plus 1.1, which has **NO PSRAM**. Their port succeeds by using external RAM that we don't have.

---

## Applicable Techniques (Can Be Used Without PSRAM)

### 1. Dynamic Vissprite Allocation
**Location:** `r_things.c:294-310`  
**Description:** Instead of static MAXVISSPRITES array, use dynamic growth

```c
static vissprite_t *R_NewVisSprite(void)
{
  if (num_vissprite >= num_vissprite_alloc) {
    size_t num_vissprite_alloc_prev = num_vissprite_alloc;
    num_vissprite_alloc = num_vissprite_alloc ? num_vissprite_alloc*2 : 128;
    vissprites = realloc(vissprites, num_vissprite_alloc*sizeof(*vissprites));
    memset(vissprites + num_vissprite_alloc_prev, 0,
      (num_vissprite_alloc - num_vissprite_alloc_prev)*sizeof(*vissprites));
  }
  return vissprites + num_vissprite++;
}
```

**Benefits:**
- Start with 128 sprites (vs our current static MAXVISSPRITES)
- Only grow when needed
- Can save 10-20KB if scene complexity is low
- **Estimated savings:** 15KB

**Implementation Complexity:** Medium (requires modifying vissprite array handling)

---

### 2. Block Memory Allocator (z_bmalloc)
**Location:** `z_bmalloc.h`, `z_bmalloc.c`, `p_map.c:2002-2070`  
**Description:** Custom pool allocator for frequently-used small structures

```c
// Example usage for sector nodes
IMPLEMENT_BLOCK_MEMORY_ALLOC_ZONE(secnodezone, sizeof(msecnode_t), PU_LEVEL, 32, "SecNodes");

inline static msecnode_t* P_GetSecnode(void) {
  return Z_BMalloc(&secnodezone);
}

inline static void P_PutSecnode(msecnode_t* node) {
  Z_BFree(&secnodezone, node);
}
```

**Benefits:**
- Reduces malloc fragmentation (critical on ESP32)
- Faster allocation/deallocation
- Pooled allocation = better memory locality
- Can reduce effective overhead by 5-10KB
- **Estimated savings:** 5-8KB

**Implementation Complexity:** Medium (requires integrating z_bmalloc system)

---

### 3. Memory-Mapped File Caching System
**Location:** `i_system.c:340-407`  
**Description:** Cache mmap handles with LRU eviction

```c
#define NO_MMAP_HANDLES 16

static struct {
  void *addr;
  int ifd;
  off_t offset;
  size_t len;
  int used;  // Reference count
} mmapHandle[NO_MMAP_HANDLES];

void *I_Mmap(void *addr, size_t length, int prot, int flags, int ifd, off_t offset) {
  // Check cache first
  for (i=0; i<NO_MMAP_HANDLES; i++) {
    if (mmapHandle[i].offset==offset && mmapHandle[i].len==length && mmapHandle[i].ifd==ifd) {
      mmapHandle[i].used++;
      return mmapHandle[i].addr;  // HIT!
    }
  }
  
  // Cache miss - allocate and store
  i = getFreeHandle();
  retaddr = malloc(length);
  if (!retaddr) {
    freeUnusedMmaps();  // Evict unused
    retaddr = malloc(length);
  }
  // ... store in cache ...
}

void freeUnusedMmaps(void) {
  for (int i=0; i<NO_MMAP_HANDLES; i++) {
    if (mmapHandle[i].used==0 && mmapHandle[i].addr!=NULL) {
      free(mmapHandle[i].addr);
      mmapHandle[i].addr=NULL;
    }
  }
}
```

**Benefits:**
- Avoid repeated malloc/free cycles for WAD lumps
- Reduces fragmentation dramatically
- Emergency memory reclaim via `freeUnusedMmaps()`
- **Estimated savings:** 10-15KB (via reduced fragmentation overhead)

**Implementation Complexity:** High (requires modifying WAD loading system)

---

### 4. Disable Zone Heap Entirely
**Location:** `z_zone.c:275-302`  
**Description:** Use direct malloc instead of zone allocator

```c
void Z_Init(void) {
#ifdef HAVE_MMAP
  return; /* cphipps - if we have mmap, we don't need our own heap */
#endif
  // ... normal zone init ...
}
```

**Benefits:**
- Eliminates zone heap overhead (headers, tracking)
- Direct malloc can be more efficient on FreeRTOS
- Saves zone management memory
- **Estimated savings:** 2-4KB (zone metadata overhead)

**Risks:**
- Requires all Z_Malloc calls to work directly with malloc
- May increase fragmentation without proper pooling

**Implementation Complexity:** Medium-High

---

### 5. Screen Buffer Optimization
**Location:** `i_video.c:163-171`  
**Description:** Framebuffer allocation strategy

```c
#ifdef INTERNAL_MEM_FB
  screenbuf=heap_caps_malloc(320*240, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#endif

// In I_SetRes():
screens[0].not_on_heap=true;
screens[0].data=screenbuf;
```

**Their approach:**
- 320x240 = 76,800 bytes framebuffer in internal memory
- Marked as `not_on_heap` to prevent double-free
- Single allocation, no per-screen arrays

**Our situation:**
- 135x240 = 32,400 bytes (already optimal)
- We're already doing this correctly

**No additional savings available here.**

---

## Techniques NOT Applicable (Require PSRAM)

### 1. SPIRAM Allocation (z_zone.c:392)
```c
while (!(block = (heap_caps_malloc)(size + HEADER_SIZE, MALLOC_CAP_SPIRAM))) {
```
**Why not applicable:** M5StickC Plus 1.1 has NO PSRAM

### 2. Large Zone Heap (z_zone.c:114)
```c
static int memory_size = 0;  // Unlimited (relies on PSRAM)
```
**Why not applicable:** They can afford large heaps in external PSRAM

---

## Optimization Roadmap Based on Research

### Phase 1: Quick Wins (Estimated 20-25KB savings)
1. **Dynamic vissprite allocation** → 15KB
2. **Mmap caching system** → 10KB (fragmentation reduction)

### Phase 2: Advanced Optimizations (Estimated 10-15KB savings)
3. **Block memory allocator** → 5-8KB
4. **Disable zone heap** → 2-4KB
5. **Additional buffer reductions** → 3-5KB

### Phase 3: Nuclear Options (If Still Short)
- Move IRAM data back to PROGMEM (slower but frees 65KB DRAM)
- Implement texture streaming from flash
- Reduce rendering resolution to 120x200
- Port to dual-core architecture (Core 0: render, Core 1: logic)

---

## Critical Insight

**The jkirsons port succeeds because they have 4-8MB of external PSRAM.** Without PSRAM, we need fundamentally different approaches:

1. **Aggressive buffer reduction** (MAXVISPLANES, VISSPRITES, DRAWSEGS)
2. **Streaming from flash** (textures, sprites loaded on-demand)
3. **Dynamic allocation** (grow arrays only when needed)
4. **Memory pooling** (reduce fragmentation)
5. **Hybrid IRAM/DRAM** (we already did this)

**Next steps:**
1. Implement dynamic vissprite allocation (15KB)
2. Add mmap caching system (10KB)
3. Reduce rendering buffers to minimal values (20-30KB)
4. If still short, move to texture streaming

**Total potential savings from research: 40-50KB**  
**Combined with buffer reductions: 60-80KB total**  
**Current need: 119KB DRAM + 2.6KB IRAM**

We're getting close, but need to stack multiple optimizations to succeed.

---

## References

- jkirsons/doom-espidf: https://github.com/jkirsons/doom-espidf
- PrBoom source code analysis
- ESP32 heap_caps_malloc documentation
- FreeRTOS memory management

---

## Conclusion

The main lesson from this research: **PSRAM is the easy solution, but without it we must be far more creative.** The techniques above (dynamic allocation, memory pooling, mmap caching) are applicable and valuable, but won't magically solve 119KB overflow. We need a **combination of all techniques** plus aggressive buffer reductions.

**Recommended next action:** Implement dynamic vissprite allocation + mmap caching, then measure actual runtime memory usage to find remaining bottlenecks.
