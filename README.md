# intrusive_lfstack

Lock-free, allocation-free, header-only, composition-based stack for C++11.

## Getting started

Add `lfstack_node` as a member to whatever struct you want to keep in a stack.
Then, you can push and pop _references_ to that struct.

```cpp
#include <avakar/intrusive/lfstack.h>
using namespace avakar::intrusive;

struct X {
    // A node maintains a membership in at most one lfstack.
    lfstack_node node;
};

int main()
{
    // You must specify the node to be used by each stack.
    lfstack<X, &X::node> st;

    X x;
    st.push(x);

    // You can check if a node is pushed in a stack.
    assert(x.node.attached());

    // If the stack is non-empty, `pop` removes the most recently inserted
    // element and returns a pointer to it. Otherwise, it returns `nullptr`.
    X * px = st.pop();
    assert(px == &x);

    px = st.pop();
    assert(px == nullptr);
}
```

Both stacks and nodes are immovable.
If a node is destroyed while attached, the behavior is undefined.
Clearing or destroying a stack will detach all its nodes.

## Thread-safety

Member functions `lfstack::push`, `lfstack::pop` and `lfstack_node::attached`
can be invoked simultaneously without data races. The same holds for
simultaneous invocations of `lfstack_node::attached` and the `lfstack`
destructor.

A push to `lfstack` synchronizes-with (and therefore happens-before) a pop
of the corresponding node.

All operations are lock-free as long as your implementation of `std::atomic`
has lock-free DCAS. On amd64 platforms, this requires cmpxchg16b instruction,
which early 64-bit AMD processors didn't have.
As of today, MSVC's 64-bit implementation does not support lock-free
DCAS regardless of your actual processor.

You can check if lfstack is actually lock-free by checking
`lfstack::is_lock_free()` or
`lfstack::is_always_lock_free` (C++17 or newer only).

In either case, lfstack is thread-safe.

## CMake integration

Copy this repo into yours, add it as a submodule, or `FetchContent` it.
Either way, make sure this repo is added via `add_subdirectory` or
`FetchContent_MakeAvailable`, then link against `avakar::intrusive_lfstack`.

    FetchContent(avakar.intrusive_lfstack SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/lfstack")
    FetchContent_MakeAvailable(avakar.intrusive_lfstack)

    # ...
    target_link_libraries(mytarget PUBLIC avakar::intrusive_lfstack)
