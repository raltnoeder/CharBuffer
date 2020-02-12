#ifndef CHARBUFFER_H
#define CHARBUFFER_H

#include <new>
#include <memory>

class CharBuffer
{
  public:
    static const size_t MAX_CAPACITY;
    static const size_t NPOS;

    // @throws std::bad_alloc
    explicit CharBuffer(size_t capacity);
    explicit CharBuffer(const char* text);
    explicit CharBuffer(size_t capacity, const char* text);
    virtual ~CharBuffer() noexcept;
    explicit CharBuffer(const CharBuffer& orig);
    explicit CharBuffer(CharBuffer&& orig);

    virtual CharBuffer& operator=(const CharBuffer& orig);
    virtual CharBuffer& operator=(CharBuffer&& orig);
    // @throws RangeException
    virtual CharBuffer& operator=(const char* text);

    virtual bool operator==(const CharBuffer& other) const noexcept;
    virtual bool operator==(const char* text) const noexcept;

    virtual bool operator<(const CharBuffer& other) const noexcept;
    virtual bool operator>(const CharBuffer& other) const noexcept;
    virtual bool operator<=(const CharBuffer& other) const noexcept;
    virtual bool operator>=(const CharBuffer& other) const noexcept;
    virtual bool operator<(const char* const text) const noexcept;
    virtual bool operator>(const char* const text) const noexcept;
    virtual bool operator<=(const char* const text) const noexcept;
    virtual bool operator>=(const char* const text) const noexcept;

    // @throws RangeException
    virtual void operator+=(const CharBuffer& other);

    // @throws RangeException
    virtual void operator+=(const char* text);

    // @throws RangeException
    virtual void operator+=(char in_char);

    // @throws RangeException
    virtual char& operator[](size_t index);

    // @throws RangeException
    virtual const char& operator[](size_t index) const;

    virtual bool is_empty() const noexcept;
    virtual size_t length() const noexcept;
    virtual size_t capacity() const noexcept;
    virtual void clear() noexcept;
    virtual void wipe() noexcept;
    virtual void truncate(size_t new_length) noexcept;

    // @throws RangeException
    virtual void substring(size_t start, size_t end);

    // @throws RangeException
    virtual void substring(const CharBuffer& other, size_t start, size_t end);

    // @throws RangeException
    virtual void append(const CharBuffer& other, size_t start, size_t end);

    // @throws RangeException
    virtual void overwrite(size_t dst_start, const CharBuffer& other);

    // @throws RangeException
    virtual void overwrite(size_t dst_start, const CharBuffer& other, size_t src_start, size_t src_end);

    // @throws RangeException
    virtual void overwrite(size_t dst_start, const char* text);

    // @throws RangeException
    virtual void overwrite(size_t dst_start, const char* text, size_t src_start, size_t src_end);

    virtual void fill(const char fill_char) noexcept;
    virtual void fill(const char fill_char, size_t target_length);

    virtual int compare_to(const CharBuffer& other) const noexcept;

    // @throws RangeException
    virtual int compare_to(const char* text) const noexcept;

    virtual bool starts_with(const CharBuffer& other) const noexcept;

    // @throws RangeException
    virtual bool starts_with(const char* text) const noexcept;

    virtual bool ends_with(const CharBuffer& other) const noexcept;

    // @throws RangeException
    virtual bool ends_with(const char* text) const noexcept;

    virtual size_t index_of(const CharBuffer& other) const noexcept;

    // @throws RangeException
    virtual size_t index_of(const char* text) const noexcept;

    // @throws RangeException
    virtual size_t index_of(const CharBuffer& other, size_t start) const;

    // @throws RangeException
    virtual size_t index_of(const char* text, size_t start) const;

    virtual const char* c_str() const;

  private:
    size_t bfr_capacity;
    size_t bfr_length;
    std::unique_ptr<char[]> buffer_mgr;
    char* buffer;

    // @throws RangeException
    inline void overwrite_impl(
        size_t dst_start,
        const char* text,
        size_t text_length,
        size_t src_start,
        size_t src_end
    );
};

#endif /* CHARBUFFER_H */
