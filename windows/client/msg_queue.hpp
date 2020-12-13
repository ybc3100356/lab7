#pragma once

#include <queue>
#include <mutex>
#include "../protocol/packet.hpp"

template<typename T>
class msg_queue
{
public:
	msg_queue();
	~msg_queue();

	void push(T&& data);
	T pop(bool& success);

private:
	std::queue<T> q;
	std::mutex m;

};

template<typename T>
inline msg_queue<T>::msg_queue()
{
}

template<typename T>
inline msg_queue<T>::~msg_queue()
{
}

template<typename T>
inline void msg_queue<T>::push(T&& data)
{
	std::lock_guard<std::mutex> lock(m);
	q.push(std::forward<T>(data));
}

template<typename T>
inline T msg_queue<T>::pop(bool & success)
{
	std::lock_guard<std::mutex> lock(m);
	auto retval = T();

	success = !q.empty();
	if (success)
	{
		retval = std::move(q.front());
		q.pop();
	}

	return retval;
}