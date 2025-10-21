#include <limits>

template <typename T>
struct minmax {
    static_assert(std::numeric_limits<T>::is_specialized);

    minmax() : minmax(
        std::numeric_limits<T>::min(),
        std::numeric_limits<T>::max()
    ) { }

    // Set default of min, max
    minmax(T d_max, T d_min) { set_default(d_max, d_min); clear(); }

    T m_min;
    T m_max;

private:
    T m_dmin, m_dmax;

public:
    void set_default(T d_max, T d_min) { m_dmin = d_min; m_dmax = d_max; }
    void clear() { m_min = m_dmin; m_max = m_dmax; }

    T diff() const { return m_max - m_min; }

    // Set min and max both
    minmax& operator|=(T value) {
        m_min = std::min(m_min, value);
        m_max = std::max(m_max, value);
        return *this;
    }

    // Set max
    minmax& operator+=(T value) { m_max = std::max(m_max, value); }
    // Set min
    minmax& operator-=(T value) { m_min = std::min(m_min, value); }
};