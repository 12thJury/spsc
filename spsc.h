#include <atomic>
#include <vector>
#include <new>
#include <optional>

template <typename T>
class SPSCQueue{

public:
	explicit SPSCQueue(size_t capacity)
	: capacity_(capacity + 1),
	  buffer_(capacity + 1)
	{
		head_.store(0 , std::memory_order_relaxed);
		tail_.store(0 , std::memory_order_relaxed);
	}

	SPSCQueue(const SPSCQueue&) = delete;
	SPSCQueue& operator=(const SPSCQueue&) = delete;

	bool push(const T& item) {
		
		const size_t current_tail = tail_.load(std::memory_order_relaxed);
		const size_t next_tail = (current_tail + 1) % capacity_;
		
		if(next_tail == head_.load(std::memory_order_acquire)){
			return false;
		}
		buffer_[current_tail] = item;
		tail_.store(next_tail , std::memory_order_release);
		return true;
	}

	bool pop(T& value)
	{
		const size_t current_head = head_.load(std::memory_order_relaxed);
		const size_t next_head = (current_head + 1) % capacity_;

		if(current_head == tail_.load(std::memory_order_acquire)) {
			return false;
		}
		value = buffer_[current_head];
		head_.store(next_head , std::memory_order_release);
		return true;
	}

private:
	const size_t capacity_;
	std::vector<T> buffer_;
	static constexpr size_t cacheLine = std::hardware_destructive_interference_size;

	alignas(cacheLine) std::atomic<size_t> head_;
	alignas(cacheLine) std::atomic<size_t> head_;
};
