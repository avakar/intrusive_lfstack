#include <avakar/intrusive/lfstack.h>
#include <avakar/mutest.h>

#include <type_traits>
#include <vector>

namespace ns = avakar::intrusive;

namespace {

struct X
{
	ns::lfstack_node node;
};

}

mutest_case("it should start empty")
{
	ns::lfstack<X, &X::node> st;
	chk st.pop() == nullptr;
}

mutest_case("it supportes is_lock_free()")
{
	ns::lfstack<X, &X::node> st;
	(void)st.is_lock_free();
}

#if __cplusplus >= 201703
mutest_case("it supportes is_always_lock_free")
{
	if (ns::lfstack<X, &X::node>::is_always_lock_free)
	{
		ns::lfstack<X, &X::node> st;
		chk st.is_lock_free();
	}
}
#endif

mutest_case("nodes should start detached")
{
	X x;
	chk !x.node.attached();
}

mutest_case("pushing into stack should attach the node")
{
	X x;
	ns::lfstack<X, &X::node> st;

	st.push(x);
	chk x.node.attached();
}

mutest_case("popping an item should detach it")
{
	X x;
	ns::lfstack<X, &X::node> st;

	st.push(x);
	chk st.pop() == &x;

	chk !x.node.attached();
}

mutest_case("popping sole item should leave stack empty")
{
	X x;
	ns::lfstack<X, &X::node> st;

	st.push(x);

	chk st.pop() == &x;
	chk st.pop() == nullptr;
}

mutest_case("items should be popped in LIFO order")
{
	X x[3];
	ns::lfstack<X, &X::node> st;

	st.push(x[0]);
	st.push(x[1]);
	st.push(x[2]);

	chk st.pop() == &x[2];
	chk st.pop() == &x[1];
	chk st.pop() == &x[0];
	chk st.pop() == nullptr;
}

mutest_case("clearing stack should leave it empty")
{
	X x[2];
	ns::lfstack<X, &X::node> st;

	st.push(x[0]);
	st.push(x[1]);

	st.clear();
	chk st.pop() == nullptr;
}

mutest_case("clearing stack should detach all nodes")
{
	X x[2];
	ns::lfstack<X, &X::node> st;

	st.push(x[0]);
	st.push(x[1]);

	st.clear();

	chk !x[0].node.attached();
	chk !x[1].node.attached();
}
