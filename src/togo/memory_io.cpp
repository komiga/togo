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

void MemoryStream::clear() {
	array::clear(_buffer);
	_position = 0;
}

IOStatus MemoryStream::status() const {
	return _status;
}

u64 MemoryStream::position() {
	return _position;
}

u64 MemoryStream::seek_to(u64 const position) {
	_position = min(position, static_cast<u64>(array::size(_buffer)));
	return _position;
}

u64 MemoryStream::seek_relative(s64 const offset) {
	_position = static_cast<u64>(
		clamp<s64>(static_cast<s64>(_position) + offset, 0, array::size(_buffer))
	);
	return _position;
}

IOStatus MemoryStream::read(void* const data, unsigned size, unsigned* const read_size) {
	if (_position + size > array::size(_buffer)) {
		if (read_size) {
			size = array::size(_buffer) - _position;
			_status.assign(false, true);
		} else {
			return _status.assign(false, true);
		}
	} else {
		_status.clear();
	}
	std::memcpy(data, array::begin(_buffer) + _position, size);
	_position += size;
	if (read_size) {
		*read_size = size;
	}
	return _status;
}

IOStatus MemoryStream::write(void const* const data, unsigned const size) {
	array::resize(_buffer, array::size(_buffer) + size);
	std::memcpy(array::begin(_buffer) + _position, data, size);
	_position += size;
	return _status.clear();
}

// class MemoryReader implementation

MemoryReader::~MemoryReader() = default;

MemoryReader::MemoryReader(u8 const* const buffer, u32_fast const size)
	: _status()
	, _size(size)
	, _buffer(buffer)
	, _position(0)
{
	TOGO_ASSERTE(_buffer);
}

MemoryReader::MemoryReader(StringRef const& ref)
	: _status()
	, _size(ref.size)
	, _buffer(reinterpret_cast<u8 const*>(ref.data))
	, _position(0)
{
	TOGO_ASSERTE(_buffer);
}

IOStatus MemoryReader::status() const {
	return _status;
}

u64 MemoryReader::position() {
	return _position;
}

u64 MemoryReader::seek_to(u64 const position) {
	_position = min(position, static_cast<u64>(_size));
	return _position;
}

u64 MemoryReader::seek_relative(s64 const offset) {
	_position = static_cast<u64>(
		clamp<s64>(static_cast<s64>(_position) + offset, 0, _size)
	);
	return _position;
}

IOStatus MemoryReader::read(void* const data, unsigned size, unsigned* const read_size) {
	if (_position + size > _size) {
		if (read_size) {
			size = _size - _position;
			_status.assign(false, true);
		} else {
			return _status.assign(false, true);
		}
	} else {
		_status.clear();
	}
	std::memcpy(data, _buffer + _position, size);
	_position += size;
	if (read_size) {
		*read_size = size;
	}
	return _status;
}

} // namespace togo
