#include <string>
#include <tuple>
#include <cstdint>

// Clock range : 00:00:00 ~ 23:59:59
// Warning : This class does not check data is valid.
struct ps_clock {
private:
    using unit = int64_t;

    constexpr static unit second_mod = 60ull;
    constexpr static unit minute_mod = 60ull;
    constexpr static unit hour12_mod = 12ull;
    constexpr static unit hour24_mod = 24ull;

    constexpr static unit minute_multiplier = 60ull;
    constexpr static unit hour_multiplier = 3600ull;

    constexpr static unit clock_mod = 86400ull;
    constexpr static unit clock_min_time = 0ull;
    constexpr static unit clock_max_time = (clock_mod - 1ull);
    constexpr static unit time_point_min_time = 1ull << 63;
    constexpr static unit time_point_max_time = (1ull << 63) - 1;

    ps_clock(unit time) : m_time(time) { }

public:
    enum class clock_mode : int {
        clock, time_point
    };

    ps_clock() = default;
    ps_clock(unit h, unit m, unit s) { m_from_hms(h, m, s); }
    ps_clock(unit h, unit m, unit s, bool is_am) { m_from_12h(h, m, s, is_am); }

private:
    unit m_time = 0ull;
    clock_mode m_clock_mode = clock_mode::clock;

    void m_from_hms(unit h, unit m, unit s) {
        if (m_clock_mode == clock_mode::clock) h %= hour24_mod;

        m_time = s + m * minute_multiplier + h * hour_multiplier;
    }

    void m_from_12h(unit h, unit m, unit s, bool is_am) {
        if (h == hour12_mod) h = 0;
        h += !is_am * 12ull;

        m_from_hms(h, m, s);
    }

    void m_add_time(unit h, unit m, unit s) {
        if (m_clock_mode == clock_mode::clock) {
            h %= hour24_mod;
            m %= minute_mod;
            s %= second_mod;
        }

        m_time = m_time +
            h * hour_multiplier +
            m * minute_multiplier +
            s;
        
        m_normalize();
    }

    void m_sub_time(unit h, unit m, unit s) {
        if (m_clock_mode == clock_mode::clock) {
            m_time += clock_mod;
            h %= hour24_mod;
            m %= minute_mod;
            s %= second_mod;
        }

        m_time = m_time - (
            h * hour_multiplier +
            m * minute_multiplier +
            s
        );

        m_normalize();
    }

    inline void m_normalize() {
        if (m_clock_mode != clock_mode::clock) return;
        m_time %= clock_mod;
    }

public:
    static ps_clock clock_min() { return ps_clock(clock_min_time); }
    static ps_clock clock_max() { return ps_clock(clock_max_time); }
    static ps_clock time_point_min() { return ps_clock(time_point_min_time); }
    static ps_clock time_point_max() { return ps_clock(time_point_max_time); }

    inline static bool is_valid(unit h, unit m, unit s) {
        if (h >= hour24_mod) return false;
        else if (m >= minute_mod) return false;
        else if (s >= second_mod) return false;
        return true;
    }

    void add_hours(unit hour) {
        m_add_time(hour, 0, 0);
    }

    void add_minutes(unit min) {
        unit
            h = min / minute_multiplier,
            m = min % minute_multiplier;
        
        m_add_time(h, m, 0);
    }

    void add_seconds(unit sec) {
        unit
            h = sec / hour_multiplier,
            m = (sec % hour_multiplier) / minute_multiplier,
            s = sec % second_mod;
        
        m_add_time(h, m, s);
    }

    void sub_hours(unit hour) {
        m_sub_time(hour, 0, 0);
    }

    void sub_minutes(unit min) {
        unit
            h = min / minute_multiplier,
            m = min % minute_multiplier;

        m_sub_time(h, m, 0);
    }

    void sub_seconds(unit sec) {
        unit
            h = sec / hour_multiplier,
            m = (sec % hour_multiplier) / minute_multiplier,
            s = sec % second_mod;
        
        m_sub_time(h, m, s);
    }

    inline std::pair<bool, unit> get_hour12() const {
        unit
            h = m_time / hour_multiplier,
            u = h % hour12_mod;
        if (u == 0) u = hour12_mod;

        return { h < hour12_mod, u };
    }

    inline std::tuple<unit, unit, unit> get_time() const {
        return {
            m_time / hour_multiplier,
            std::abs(m_time % hour_multiplier / minute_multiplier),
            std::abs(m_time % second_mod)
        };
    }

    inline void set_time(unit h, unit m, unit s) { m_from_hms(h, m, s); }

    inline unit raw_time() const { return m_time; }
    inline unit& raw_time() { return m_time; }
    inline clock_mode mode() const { return m_clock_mode; }
    inline clock_mode& mode() { return m_clock_mode; }

    // Format :
    // %h = hour, %m = minutes, %s = seconds, %a = am/pm, %A = AM/PM
    // %u = hour (12h-base)
    // upper case (for u, h, m, s) = apply setfill('0') and setw(2);
    // %% = print %
    // else word that prefix by '%' : do as normal string.
    // Abbr (AM/PM) :
    // Use custom string of am/pm. string must be lowercase because of '%a, %A'
    std::string to_string(
        const std::string& format = "%H:%M:%S",
        const std::pair<std::string, std::string>& abbr = { "am", "pm" }
    ) const {
        std::string result; result.reserve(format.size());

        auto [h, m, s] = get_time();
        auto [is_am, u] = get_hour12();

        bool neg = h < 0;
        if (neg) {
            h = std::abs(h);
            result.push_back('-');
        }

        std::string
            hs = std::to_string(h),
            ms = std::to_string(m),
            ss = std::to_string(s),
            us = std::to_string(u);
        
        const auto& [ams, pms] = abbr;
        auto [AMS, PMS] = abbr;

        for (char& c : AMS) c = std::toupper(c);
        for (char& c : PMS) c = std::toupper(c);

        for (size_t idx = 0; idx < format.size(); idx++) {
            if (format[idx] != '%') {
                result.push_back(format[idx]);
                continue;
            }

            idx++;
            if (idx == format.size()) {
                result.push_back('%');
                break;
            }

            switch (format[idx]) {
                case 'H':
                    if (hs.size() == 1) result.push_back('0');
                    [[ fallthrough ]];
                case 'h':
                    result += hs;
                    break;
                case 'M':
                    if (ms.size() == 1) result.push_back('0');
                    [[ fallthrough ]];
                case 'm':
                    result += ms;
                    break;
                case 'S':
                    if (ss.size() == 1) result.push_back('0');
                    [[ fallthrough ]];
                case 's':
                    result += ss;
                    break;
                case 'U':
                    if (us.size() == 1) result.push_back('0');
                    [[ fallthrough ]];
                case 'u':
                    result += us;
                    break;
                case '%':
                    result.push_back('%');
                    break;
                case 'a':
                    result += (is_am ? ams : pms);
                    break;
                case 'A':
                    result += (is_am ? AMS : PMS);
                    break;
                default:
                    result.push_back('%');
                    result.push_back(format[idx]);
                    break;
            }
        }

        return result;
    }

    bool operator<(ps_clock rhs) const { return m_time < rhs.m_time; }
    bool operator>(ps_clock rhs) const { return m_time > rhs.m_time; }
    bool operator<=(ps_clock rhs) const { return m_time <= rhs.m_time; }
    bool operator>=(ps_clock rhs) const { return m_time >= rhs.m_time; }
    bool operator==(ps_clock rhs) const { return m_time == rhs.m_time; }
    bool operator!=(ps_clock rhs) const { return m_time != rhs.m_time; }

    ps_clock& operator+=(ps_clock rhs) { add_seconds(rhs.m_time); return *this; }
    ps_clock& operator-=(ps_clock rhs) { sub_seconds(rhs.m_time); return *this; }
    ps_clock operator+(ps_clock rhs) const { return rhs += *this; }
    ps_clock operator-(ps_clock rhs) const { return rhs -= *this; }

    static inline ps_clock forward_distance(ps_clock start, ps_clock end) {
        if (start.m_time > end.m_time) end.m_time += clock_mod;
        
        end.m_time -= start.m_time;
        end.m_normalize();
        
        return end;
    }

    static inline ps_clock short_distance(ps_clock clk1, ps_clock clk2) {
        return std::min(forward_distance(clk1, clk2), forward_distance(clk2, clk1));
    }
};