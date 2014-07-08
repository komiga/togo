#line 2 "togo/memory_io.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/utility.hpp>
#include <togo/array.hpp>
#include <togo/memory.hpp>
#include <togo/memory_io.hpp>

namespace togo {

// class MemoryStream implementation

MemoryStream::~MemoryStream() = default;

MemoryStream::MemoryStream(
	Allocator& allocator,
	u32 const init_capacity
)
	: _status()
	, _buffer(allocator)
	, _position(0)
{
	array::set_capacity(_buffer, init_capacity);

}

IOStatus MemoryStream::status() const {
	return _status;
}

u64 MemoryStream::position() {
	return _position;
}

u64 MemoryStream::seek_to(u64 const position) {
	_position = min(position, static_cast<u64>(array::size(_buffer)));
	return this->position();
}

u64 MemoryStream::seek_relative(s64 const offset) {
	_position = static_cast<u64>(
		clamp<s64>(static_cast<s64>(_position) + offset, 0, array::size(_buffer))
	);
	return position();
}

IOStatus MemoryStream::read(void* const data, unsigned const size) {
	if (_position + size > array::size(_buffer)) {
		return _status.assign(true, true);
	}
	std::memcpy(data, array::begin(_buffer) + _position, size);
	_position += size;
	return _status.clear();
}

IOStatus MemoryStream::write(void const* const data, unsigned const size) {
	array::resize(_buffer, array::size(_buffer) + size);
	std::memcpy(array::begin(_buffer) + _position, data, size);
	_position += size;
	return _status.clear();
}

} // namespace togo
