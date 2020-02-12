#include <CharBuffer.h>

#include <stdexcept>
#include <cstring>

#include <RangeException.h>

// Maximum net capacity of a CharBuffer
// This is the maximum number of characters that any CharBuffer instance can contain,
// without the trailing null character.
const size_t CharBuffer::MAX_CAPACITY = (~static_cast<size_t> (0)) - 1;

const size_t CharBuffer::NPOS = ~static_cast<size_t> (0);

// @throws RangeException
inline static char* char_at(size_t idx, char* buffer, size_t length);

// @throws RangeException
inline static size_t safe_c_str_length(const char* buffer);

inline static void copy_buffer(
    const char* src_buffer,
    size_t src_start,
    size_t src_end,
    char* dst_buffer,
    size_t dst_offset
);

inline static int compare_buffer(
    const char* buffer,
    const char* other_buffer,
    size_t compare_length
);

inline static bool match_buffer(
    const char* buffer,
    const char* other_buffer,
    size_t match_length
);

inline static size_t index_of_impl(
    const char* buffer,
    size_t length,
    const char* pattern,
    size_t pat_length,
    size_t start_offset,
    size_t end_offset
);

// @throws std::bad_alloc
CharBuffer::CharBuffer(const size_t buffer_capacity):
    bfr_capacity(buffer_capacity)
{
    if (buffer_capacity < MAX_CAPACITY)
    {
        buffer_mgr = std::unique_ptr<char[]>(new char[buffer_capacity + 1]);
        buffer = buffer_mgr.get();
        bfr_length = 0;
        buffer[bfr_length] = '\0';
    }
    else
    {
        throw std::bad_alloc();
    }
}

// @throws std::bad_alloc, RangeException
CharBuffer::CharBuffer(const char* const text)
{
    size_t text_length = safe_c_str_length(text);
    if (text_length < MAX_CAPACITY)
    {
        bfr_capacity = text_length;
        buffer_mgr = std::unique_ptr<char[]>(new char[bfr_capacity + 1]);
        buffer = buffer_mgr.get();
        copy_buffer(text, 0, text_length, buffer, 0);
        bfr_length = text_length;
        buffer[bfr_length] = '\0';
    }
    else
    {
        throw std::bad_alloc();
    }
}

// @throws std::bad_alloc, RangeException
CharBuffer::CharBuffer(const size_t buffer_capacity, const char* const text):
    bfr_capacity(buffer_capacity)
{
    if (bfr_capacity < MAX_CAPACITY)
    {
        buffer_mgr = std::unique_ptr<char[]>(new char[bfr_capacity + 1]);
        buffer = buffer_mgr.get();

        size_t text_length = safe_c_str_length(text);
        if (text_length <= bfr_capacity)
        {
            copy_buffer(text, 0, text_length, buffer, 0);
        }
        else
        {
            throw RangeException();
        }
        bfr_length = text_length;
        buffer[bfr_length] = '\0';
    }
    else
    {
        throw std::bad_alloc();
    }
}

CharBuffer::~CharBuffer() noexcept
{
}

// @throws std::bad_alloc
CharBuffer::CharBuffer(const CharBuffer& orig):
    bfr_capacity(orig.bfr_capacity),
    bfr_length(orig.bfr_length),
    buffer_mgr(new char[orig.bfr_capacity + 1])
{
    buffer = buffer_mgr.get();
    copy_buffer(orig.buffer, 0, bfr_length, buffer, 0);
}

CharBuffer::CharBuffer(CharBuffer&& orig):
    bfr_capacity(orig.bfr_capacity),
    bfr_length(orig.bfr_length),
    buffer(orig.buffer)
{
    buffer_mgr = std::move(orig.buffer_mgr);
    orig.buffer = nullptr;
    orig.bfr_length = 0;
    orig.bfr_capacity = 0;
}


// @throws RangeException
CharBuffer& CharBuffer::operator=(const CharBuffer& orig)
{
    if (this != &orig)
    {
        if (orig.bfr_length <= bfr_capacity)
        {
            bfr_length = orig.bfr_length;
            copy_buffer(orig.buffer, 0, bfr_length, buffer, 0);
        }
        else
        {
            throw RangeException();
        }
    }
    return *this;
}

CharBuffer& CharBuffer::operator=(CharBuffer&& orig)
{
    if (this != &orig)
    {
        buffer = orig.buffer;
        bfr_length = orig.bfr_length;
        bfr_capacity = orig.bfr_capacity;

        buffer_mgr = std::move(orig.buffer_mgr);
        orig.buffer = nullptr;
        orig.bfr_length = 0;
        orig.bfr_capacity = 0;
    }
    return *this;
}

// @throws RangeException
CharBuffer& CharBuffer::operator=(const char* const text)
{
    const size_t text_length = safe_c_str_length(text);
    if (text_length <= bfr_capacity)
    {
        for (size_t idx = 0; idx < text_length; ++idx)
        {
            buffer[idx] = text[idx];
        }
        bfr_length = text_length;
        buffer[bfr_length] = '\0';
    }
    else
    {
        throw RangeException();
    }
    return *this;
}

bool CharBuffer::operator==(const CharBuffer& other) const noexcept
{
    bool equal_flag = false;
    if (bfr_length == other.bfr_length)
    {
        equal_flag = match_buffer(buffer, other.buffer, bfr_length);
    }
    return equal_flag;
}

bool CharBuffer::operator==(const char* const text) const noexcept
{
    bool equal_flag = false;
    const size_t text_length = safe_c_str_length(text);
    if (bfr_length == text_length)
    {
        equal_flag = match_buffer(buffer, text, bfr_length);
    }
    return equal_flag;
}

bool CharBuffer::operator<(const CharBuffer& other) const noexcept
{
    return compare_to(other) < 0;
}

bool CharBuffer::operator>(const CharBuffer& other) const noexcept
{
    return compare_to(other) > 0;
}

bool CharBuffer::operator<=(const CharBuffer& other) const noexcept
{
    return compare_to(other) <= 0;
}

bool CharBuffer::operator>=(const CharBuffer& other) const noexcept
{
    return compare_to(other) >= 0;
}

bool CharBuffer::operator<(const char* const text) const noexcept
{
    return compare_to(text) < 0;
}

bool CharBuffer::operator>(const char* const text) const noexcept
{
    return compare_to(text) > 0;
}

bool CharBuffer::operator<=(const char* const text) const noexcept
{
    return compare_to(text) <= 0;
}

bool CharBuffer::operator>=(const char* const text) const noexcept
{
    return compare_to(text) >= 0;
}

// @throws RangeException
void CharBuffer::operator+=(const CharBuffer& other)
{
    const size_t remain = bfr_capacity - bfr_length;
    if (other.bfr_length <= remain)
    {
        copy_buffer(other.buffer, 0, other.bfr_length, buffer, bfr_length);
        bfr_length += other.bfr_length;
        buffer[bfr_length] = '\0';
    }
    else
    {
        throw RangeException();
    }
}

// @throws RangeException
void CharBuffer::operator+=(const char* const text)
{
    const size_t remain = bfr_capacity - bfr_length;
    const size_t text_length = safe_c_str_length(text);
    if (text_length <= remain)
    {
        copy_buffer(text, 0, text_length, buffer, bfr_length);
        bfr_length += text_length;
        buffer[bfr_length] = '\0';
    }
    else
    {
        throw RangeException();
    }
}

// @throws RangeException
void CharBuffer::operator+=(const char in_char)
{
    if (bfr_length < bfr_capacity)
    {
        buffer[bfr_length] = in_char;
        ++bfr_length;
        buffer[bfr_length] = '\0';
    }
    else
    {
        throw RangeException();
    }
}

// @throws RangeException
char& CharBuffer::operator[](const size_t index)
{
    return *(char_at(index, buffer, bfr_length));
}

// @throws RangeException
const char& CharBuffer::operator[](const size_t index) const
{
    return *(char_at(index, buffer, bfr_length));
}

bool CharBuffer::is_empty() const noexcept
{
    return (bfr_length == 0);
}

size_t CharBuffer::length() const noexcept
{
    return bfr_length;
}

size_t CharBuffer::capacity() const noexcept
{
    return bfr_capacity;
}

void CharBuffer::clear() noexcept
{
    bfr_length = 0;
    buffer[bfr_length] = '\0';
}

void CharBuffer::wipe() noexcept
{
    std::memset(buffer, 0, bfr_capacity + 1);
}

void CharBuffer::truncate(const size_t new_length) noexcept
{
    if (new_length < bfr_length)
    {
        bfr_length = new_length;
        buffer[bfr_length] = '\0';
    }
}

// @throws RangeException
void CharBuffer::copy_raw(const char* const data, const size_t length)
{
    if (length <= bfr_capacity)
    {
        copy_buffer(data, 0, length, buffer, 0);
        bfr_length = length;
    }
    else
    {
        throw RangeException();
    }
}

// @throws RangeException
void CharBuffer::copy_raw(const char* const data, const size_t start, const size_t end)
{
    if (start <= end)
    {
        const size_t substr_length = end - start;
        if (substr_length <= bfr_capacity)
        {
            copy_buffer(data, start, end, buffer, 0);
            bfr_length = substr_length;
        }
        else
        {
            throw RangeException();
        }
    }
    else
    {
        throw RangeException();
    }
}

// @throws RangeException
void CharBuffer::substring(const size_t start, const size_t end)
{
    if (start <= end && end <= bfr_length)
    {
        if (start > 0)
        {
            copy_buffer(buffer, start, end, buffer, 0);
            bfr_length = end - start;
        }
        else
        {
            bfr_length = end;
        }
        buffer[bfr_length] = '\0';
    }
    else
    {
        throw RangeException();
    }

}

// @throws RangeException
void CharBuffer::substring(const CharBuffer& other, const size_t start, const size_t end)
{
    if (start <= end && end <= other.bfr_length)
    {
        const size_t substr_length = end - start;
        if (substr_length <= bfr_capacity)
        {
            copy_buffer(other.buffer, start, end, buffer, 0);
            bfr_length = substr_length;
            buffer[bfr_length] = '\0';
        }
        else
        {
            throw RangeException();
        }
    }
    else
    {
        throw RangeException();
    }
}

// @throws RangeException
void CharBuffer::append(const CharBuffer& other, const size_t start, const size_t end)
{
    const size_t remain = bfr_capacity - bfr_length;
    if (start <= end && end <= other.bfr_length)
    {
        const size_t substr_length = end - start;
        if (substr_length <= remain)
        {
            copy_buffer(other.buffer, start, end, buffer, bfr_length);
            bfr_length += substr_length;
            buffer[bfr_length] = '\0';
        }
        else
        {
            throw RangeException();
        }
    }
    else
    {
        throw RangeException();
    }
}

// @throws RangeException
void CharBuffer::append_raw(const char* const data, const size_t data_length)
{
    const size_t remain = bfr_capacity - bfr_length;
    if (data_length <= remain)
    {
        copy_buffer(data, 0, data_length, buffer, bfr_length);
        bfr_length += data_length;
        buffer[bfr_length] = '\0';
    }
    else
    {
        throw RangeException();
    }
}

// @throws RangeException
void CharBuffer::append_raw(const char* const data, const size_t start, const size_t end)
{
    const size_t remain = bfr_capacity - bfr_length;
    if (start <= end)
    {
        const size_t substr_length = end - start;
        if (substr_length <= remain)
        {
            copy_buffer(data, start, end, buffer, bfr_length);
            bfr_length += substr_length;
            buffer[bfr_length] = '\0';
        }
        else
        {
            throw RangeException();
        }
    }
    else
    {
        throw RangeException();
    }
}

// @throws RangeException
void CharBuffer::overwrite(
    const size_t dst_start,
    const CharBuffer& other
)
{
    overwrite_impl(dst_start, other.buffer, other.bfr_length, 0, other.bfr_length);
}

// @throws RangeException
void CharBuffer::overwrite(
    const size_t dst_start,
    const CharBuffer& other,
    const size_t src_start,
    const size_t src_end
)
{
    overwrite_impl(dst_start, other.buffer, other.bfr_length, src_start, src_end);
}

// @throws RangeException
void CharBuffer::overwrite(
    const size_t dst_start,
    const char* const text,
    const size_t src_start,
    const size_t src_end
)
{
    size_t text_length = safe_c_str_length(text);
    overwrite_impl(dst_start, text, text_length, src_start, src_end);
}

// @throws RangeException
void CharBuffer::overwrite(
    const size_t dst_start,
    const char* const text
)
{
    size_t text_length = safe_c_str_length(text);
    overwrite_impl(dst_start, text, text_length, 0, text_length);
}

// @throws RangeException
inline void CharBuffer::overwrite_impl(
    const size_t dst_start,
    const char* const src_buffer,
    const size_t src_length,
    const size_t src_start,
    const size_t src_end
)
{
    if (dst_start > bfr_capacity || dst_start > bfr_length || src_start > src_end || src_end > src_length)
    {
        throw RangeException();
    }

    const size_t remain = bfr_capacity - dst_start;
    const size_t copy_length = src_end - src_start;
    if (remain < copy_length)
    {
        throw RangeException();
    }

    size_t dst_idx = dst_start;
    size_t src_idx = src_start;
    while (src_idx < src_end)
    {
        buffer[dst_idx] = src_buffer[src_idx];
        ++dst_idx;
        ++src_idx;
    }
    size_t new_length = dst_start + copy_length;
    if (new_length > bfr_length)
    {
        buffer[new_length] = '\0';
        bfr_length = new_length;
    }
}

void CharBuffer::fill(const char fill_char) noexcept
{
    for (size_t idx = bfr_length; idx < bfr_capacity; ++idx)
    {
        buffer[idx] = fill_char;
    }
    bfr_length = bfr_capacity;
    buffer[bfr_length] = '\0';
}

void CharBuffer::fill(const char fill_char, const size_t target_length)
{
    if (target_length <= bfr_capacity)
    {
        for (size_t idx = bfr_length; idx < target_length; ++idx)
        {
            buffer[idx] = fill_char;
        }
        bfr_length = target_length;
        buffer[bfr_length] = '\0';
    }
    else
    {
        throw RangeException();
    }
}

int CharBuffer::compare_to(const CharBuffer& other) const noexcept
{
    int result = 0;
    if (bfr_length == other.bfr_length)
    {
        result = compare_buffer(buffer, other.buffer, bfr_length);
    }
    else
    {
        const size_t cmp_length = bfr_length <= other.bfr_length ? bfr_length : other.bfr_length;
        result = compare_buffer(buffer, other.buffer, cmp_length);
        if (result == 0)
        {
            result = bfr_length < other.bfr_length ? -1 : 1;
        }
    }
    return result;
}

// @throws RangeException
int CharBuffer::compare_to(const char* const text) const noexcept
{
    int result = 0;
    const size_t text_length = safe_c_str_length(text);
    if (bfr_length == text_length)
    {
        result = compare_buffer(buffer, text, text_length);
    }
    else
    {
        const size_t cmp_length = bfr_length <= text_length ? bfr_length : text_length;
        result = compare_buffer(buffer, text, cmp_length);
        if (result == 0)
        {
            result = bfr_length < text_length ? -1 : 1;
        }
    }
    return result;
}

bool CharBuffer::starts_with(const CharBuffer& other) const noexcept
{
    bool result = false;
    if (bfr_length >= other.bfr_length)
    {
        result = compare_buffer(buffer, other.buffer, other.bfr_length) == 0;
    }
    return result;
}

// @throws RangeException
bool CharBuffer::starts_with(const char* const text) const noexcept
{
    bool result = false;
    const size_t text_length = safe_c_str_length(text);
    if (bfr_length >= text_length)
    {
        result = compare_buffer(buffer, text, text_length) == 0;
    }
    return result;
}

bool CharBuffer::ends_with(const CharBuffer& other) const noexcept
{
    bool result = false;
    if (bfr_length >= other.bfr_length)
    {
        const size_t cmp_length = bfr_length - other.bfr_length;
        result = compare_buffer(
            &(buffer[bfr_length - cmp_length]),
            other.buffer,
            cmp_length
        ) == 0;
    }
    return result;
}

// @throws RangeException
bool CharBuffer::ends_with(const char* const text) const noexcept
{
    bool result = false;
    const size_t text_length = safe_c_str_length(text);
    if (bfr_length >= text_length)
    {
        size_t cmp_length = bfr_length - text_length;
        result = compare_buffer(
            &(buffer[bfr_length - cmp_length]),
            text,
            cmp_length
        ) == 0;
    }
    return result;
}

size_t CharBuffer::index_of(const CharBuffer& other) const noexcept
{
    size_t index = NPOS;
    if (bfr_length >= other.bfr_length)
    {
        const size_t end_offset = bfr_length - other.bfr_length;
        index = index_of_impl(
            buffer, bfr_length,
            other.buffer, other.bfr_length,
            0, end_offset
        );
    }

    return index;
}

// @throws RangeException
size_t CharBuffer::index_of(const char* const text) const noexcept
{
    size_t index = NPOS;
    const size_t text_length = safe_c_str_length(text);
    if (bfr_length >= text_length)
    {
        size_t end_offset = bfr_length - text_length;
        index = index_of_impl(buffer, bfr_length, text, text_length, 0, end_offset);
    }

    return index;
}

// @throws RangeException
size_t CharBuffer::index_of(const CharBuffer& other, const size_t start) const
{
    size_t index = NPOS;
    if (start <= bfr_length)
    {
        const size_t remain = bfr_length - start;
        if (remain >= other.bfr_length)
        {
            const size_t end_offset = bfr_length - other.bfr_length;
            if (start <= end_offset)
            {
                index = index_of_impl(buffer, bfr_length, other.buffer, other.bfr_length, start, end_offset);
            }
        }
    }
    else
    {
        throw RangeException();
    }

    return index;
}

// @throws RangeException
size_t CharBuffer::index_of(const char* const text, const size_t start) const
{
    size_t index = NPOS;
    const size_t text_length = safe_c_str_length(text);
    if (start <= bfr_length)
    {
        const size_t remain = bfr_length - start;
        if (remain >= text_length)
        {
            size_t end_offset = bfr_length - text_length;
            if (start <= end_offset)
            {
                index = index_of_impl(buffer, bfr_length, text, text_length, start, end_offset);
            }
        }
    }
    else
    {
        throw RangeException();
    }

    return index;
}

const char* CharBuffer::c_str() const
{
    return buffer;
}

// @throws RangeException
inline static char* char_at(const size_t idx, char* const buffer, size_t length)
{
    char* out_char {nullptr};
    if (idx < length)
    {
        out_char = &(buffer[idx]);
    }
    else
    {
        throw RangeException();
    }
    return out_char;
}

// @throws RangeException
inline static size_t safe_c_str_length(const char* const buffer)
{
    size_t idx = 0;
    while (buffer[idx] != '\0')
    {
        if (idx < CharBuffer::MAX_CAPACITY)
        {
            ++idx;
        }
        else
        {
            throw RangeException();
        }
    }
    return idx;
}

inline static void copy_buffer(
    const char* const src_buffer,
    const size_t src_start,
    const size_t src_end,
    char* const dst_buffer,
    const size_t dst_offset
)
{
    size_t dst_idx = dst_offset;
    size_t src_idx = src_start;
    while (src_idx < src_end)
    {
        dst_buffer[dst_idx] = src_buffer[src_idx];
        ++dst_idx;
        ++src_idx;
    }
    dst_buffer[dst_idx] = '\0';
}

inline static int compare_buffer(
    const char* const buffer,
    const char* const other_buffer,
    const size_t compare_length
)
{
    int result = 0;
    for (size_t idx = 0; idx < compare_length; ++idx)
    {
        if (buffer[idx] != other_buffer[idx])
        {
            if (buffer[idx] < other_buffer[idx])
            {
                result = -1;
            }
            else
            {
                result = 1;
            }
            break;
        }
    }
    return result;
}

inline static bool match_buffer(
    const char* const buffer,
    const char* const other_buffer,
    const size_t match_length
)
{
    size_t idx = 0;
    while (idx < match_length && buffer[idx] == other_buffer[idx])
    {
        ++idx;
    }
    return idx == match_length;
}

inline static size_t index_of_impl(
    const char* const buffer,
    const size_t length,
    const char* const pattern,
    const size_t pat_length,
    const size_t start_offset,
    const size_t end_offset
)
{
    size_t index = CharBuffer::NPOS;

    if (length >= pat_length)
    {
        // Cannot access the first byte of a zero-length string.
        // The empty string always matches at the position where the
        // search started
        if (pat_length > 0)
        {
            for (size_t src_idx = start_offset; src_idx <= end_offset; ++src_idx)
            {
                // compare first character of both strings
                if (buffer[src_idx] == pattern[0])
                {
                    // first character matches, compare strings
                    size_t cmp_idx = src_idx + 1;
                    size_t pat_idx = 1;

                    while (pat_idx < pat_length &&
                           buffer[cmp_idx] == pattern[pat_idx])
                    {
                        ++cmp_idx;
                        ++pat_idx;
                    }
                    if (pat_idx == pat_length)
                    {
                        index = src_idx;
                    }
                }
            }
        }
        else
        {
            index = 0;
        }
    }

    return index;
}
