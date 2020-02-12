#ifndef RANGEEXCEPTION_H
#define RANGEEXCEPTION_H

#include <stdexcept>

class RangeException : public std::exception
{
  public:
    RangeException();
    virtual ~RangeException() noexcept;

    RangeException(const RangeException& orig) = delete;
    RangeException& operator=(const RangeException& orig) = delete;
    RangeException(RangeException&& orig) = default;
    RangeException& operator=(RangeException&& orig) = default;
  private:

};

#endif	/* RANGEEXCEPTION_H */
