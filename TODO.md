# TODO

## Crashes / Safety

### `AnimationClipController` holds a dangling raw pointer after layout changes
**File:** `include/moth_ui/animation/animation_clip_controller.h:21`

The controller stores a raw `AnimationClip*` pointing into a `unique_ptr`-owned object
inside `LayoutEntityGroup::m_clips`. If that vector is modified while the controller is
running (e.g. via undo in the editor, or a reload), the pointer dangles. The next
`Update()` call will dereference it.

**Fix:** Store a clip name and look it up each frame, or store the index — either option
survives vector reallocation. A `weak_ptr` alternative requires changing clip ownership
to `shared_ptr`.

---

### `LayoutEntityRef::CopyLayout()` — ref instances share child `shared_ptr`s
**File:** `src/layout/layout_entity_ref.cpp:100`

`CopyLayout` pushes children as shared pointers to the *same* child objects from the
source layout, then immediately mutates them: `child->m_parent = this` and
`child->m_hardReference = child->Clone(...)`. When two `LayoutEntityRef` instances point
to the same source layout, both hold a reference to the same child objects, and the
second `CopyLayout` call overwrites `m_parent` and `m_hardReference` set by the first —
silently breaking the first ref.

**Fix:** Deep-clone each child during copy:
```cpp
auto cloned = std::static_pointer_cast<LayoutEntity>(child->Clone(CloneType::Deep));
cloned->m_parent = this;
m_children.push_back(cloned);
```

---

## Bugs

### Loop wrap broken by `FrameCount()` fix
**File:** `src/animation/animation_clip_controller.cpp:57`

`FrameCount()` was corrected to return `m_endFrame - m_startFrame + 1` (inclusive count).
However the loop wrap at line 57 still subtracts `FrameCount()`:

```cpp
m_frame -= static_cast<float>(m_clip->FrameCount());
```

The wrap triggers when `m_frame >= m_endFrame` (e.g. `m_frame = 10.3` for a 0–10 clip).
The correct amount to subtract is the loop *period* — `endFrame - startFrame` — so that
`10.3` wraps to `0.3`. With the new `FrameCount()` returning 11, it subtracts 11 and
gives `-0.7`, landing before `startFrame`.

**Fix:** Change the wrap line to subtract the period directly:
```cpp
m_frame -= static_cast<float>(m_clip->m_endFrame - m_clip->m_startFrame);
```

---

## Medium / Safety

### `AnimationTrackController` stores a direct `float&` into node member data
**File:** `src/animation/animation_controller.cpp` — `AnimationController` constructor

Track controllers hold a reference to a `float` inside a node's layout rect or colour
data (via `GetTargetReference`). This is safe under the current ownership model but would
silently corrupt if nodes were ever stored in a `std::vector` that reallocates, or if the
referenced node is moved in memory.

**Fix:** Store a setter callback or a `Node*` + offset rather than a raw reference.
Lower priority while nodes remain heap-allocated via `shared_ptr`.

---

## Downstream

### `BroadcastEvent` renamed to `FireEvent` — call sites need updating
**File:** `include/moth_ui/layers/layer_stack.h`

`LayerStack::BroadcastEvent` has been renamed to `FireEvent`. The following call sites
in other projects still use the old name and will fail to compile:

- `VanishingPoint/src/main.cpp:78`
- `moth_editor/example/src/example_layer.cpp:91`
- `moth_editor/src/editor/editor_layer.cpp:167, 784`
- `TinCan/src/game_layer.cpp:86, 93`

**Fix:** Replace all `m_layerStack->BroadcastEvent(...)` with
`m_layerStack->FireEvent(...)` in each project.

