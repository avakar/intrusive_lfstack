#include <avakar/container_of.h>
#include <atomic>
#include <cstdint>

namespace avakar {
namespace intrusive {

struct lfstack_node
{
	lfstack_node() noexcept = default;

	bool attached() const noexcept
	{
		return _pincount.load(std::memory_order_relaxed) != 0;
	}

	lfstack_node(lfstack_node const &) = delete;
	lfstack_node & operator=(lfstack_node const &) = delete;

private:
	template <typename T, lfstack_node T::* mp>
	friend struct lfstack;

	struct _next_t
	{
		lfstack_node * node;
		std::uintptr_t pincount;
	};

	_next_t _next;
	std::atomic<uintptr_t> _pincount{ 0 };
};

template <typename T, lfstack_node T::* mp>
struct lfstack
{
#if __cpp_lib_atomic_is_always_lock_free
	static constexpr bool is_always_lock_free = std::atomic<lfstack_node::_next_t>::is_always_lock_free;
#endif

	lfstack() noexcept
		: _top(_next_t{ nullptr, 0 })
	{
	}

	bool is_lock_free() const noexcept
	{
		return _top.is_lock_free();
	}

	void push(T & v) noexcept
	{
		lfstack_node & n = v.*mp;
		n._pincount.store(1, std::memory_order_relaxed);
		
		_next_t top = _top.load(std::memory_order_relaxed);
		do {
			n._next = top;
		}
		while (!_top.compare_exchange_weak(top, _next_t{ &n, 0 }, std::memory_order_release, std::memory_order_relaxed));
	}

	T * pop() noexcept
	{
		_next_t top = _top.load(std::memory_order_relaxed);

		for (;;)
		{
			for (;;)
			{
				if (top.node == nullptr)
					return nullptr;
				if (_top.compare_exchange_weak(top, _next_t{ top.node, top.pincount + 2 }, std::memory_order_acquire, std::memory_order_relaxed))
					break;
			}

			top.pincount += 2;

			lfstack_node * n = top.node;
			uintptr_t acquired_pins = 2;

			while (top.node == n)
			{
				if (_top.compare_exchange_weak(top, n->_next, std::memory_order_relaxed))
				{
					acquired_pins -= top.pincount - 1;
					break;
				}
			}

			if (n->_pincount.fetch_sub(acquired_pins, std::memory_order_relaxed) == acquired_pins)
				return avakar::container_of(n, mp);
		}
	}

	void clear() noexcept
	{
		while (this->pop())
		{
		}
	}

	~lfstack()
	{
		this->clear();
	}

	lfstack(lfstack const &) = delete;
	lfstack & operator=(lfstack const &) = delete;

private:
	using _next_t = lfstack_node::_next_t;

	std::atomic<_next_t> _top;
};

}
}

#pragma once
