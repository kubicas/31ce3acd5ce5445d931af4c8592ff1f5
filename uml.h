#ifndef INTF_UML__UML_H
#define INTF_UML__UML_H

#include "eps/eps.h"
#include "eps/eps_basic_shapes.h"
#include <vector>
#include <stack>
#include <string>

#ifdef EPS
#define EPS_API __declspec(dllexport)
#else
#define EPS_API __declspec(dllimport)
#endif

// http://www.omg.org/spec/UML/2.4.1/Superstructure/PDF
namespace uml
{

struct simple_class_t;

struct lane_t
{
    lane_t()
        : m_x(0)
        , m_pclass(nullptr)
        , m_plifeline(nullptr)
    {}
    float m_x;
    simple_class_t* m_pclass;
    eps::line_t* m_plifeline;
    std::stack<eps::rectangle_t*> m_context_stack;
};

static float const grid_space = static_cast<float>(9._bp);
static float const solid_whiteness = static_cast<float>(0.95); // (0,1)
static float const line_width = static_cast<float>(0.25_bp);

EPS_API eps::lineending_t* lineending_comments();
EPS_API eps::lineending_t* lineending_asynchronous_message();
EPS_API eps::lineending_t* lineending_synchronous_message();
EPS_API eps::linestyle_t* linestyle_dash();

inline eps::point_t get_ref(eps::ref_t ref,
    eps::point_t tr,
    eps::point_t tl,
    eps::point_t bl,
    eps::point_t br)
{
    switch (ref)
    {
    case eps::ref_t::bl:
        return bl;
    case eps::ref_t::cl:
        return eps::mid(bl, tl);
    case eps::ref_t::tl:
        return tl;
    case eps::ref_t::bc:
        return eps::mid(bl, br);
    case eps::ref_t::cc:
        return eps::mid(bl, tr);
    case eps::ref_t::tc:
        return eps::mid(tl, tr);
    case eps::ref_t::br:
        return br;
    case eps::ref_t::cr:
        return eps::mid(br, tr);
    case eps::ref_t::tr:
        return tr;
    default:
        THROW(std::logic_error, "E0202", << "Illegal enum value '" << static_cast<int>(ref) << "'");
    }
}

struct simple_class_t
    : public eps::group_t
{
    EPS_API simple_class_t(
        eps::iproperties_t const& parent_properties,
        std::string const& text);
    EPS_API eps::point_t get_ref(eps::ref_t ref);
    eps::rectangle_t& m_rectangle;
    eps::text_t& m_text;
    static float const m_default_height;
    static float const m_default_width;
};
float const simple_class_t::m_default_width = grid_space * 8;
float const simple_class_t::m_default_height = grid_space * 4;

struct sequence_diagram_t
    : public eps::group_t
{
    enum class sync_t
    {
        sync,
        async
    };
    EPS_API sequence_diagram_t(
        eps::iproperties_t const& parent_properties,
        int lanes);
    EPS_API void add_simple_class(int lane, std::string const& text);
    EPS_API void start_lifeline(int lane);
    EPS_API void lifeline_space(int lane, float space);
    EPS_API void end_lifeline(int lane, bool destroy = false);
    EPS_API void start_context(int lane);
    EPS_API void shift_context(int lane, int shift);
    EPS_API void end_context(int lane);
    EPS_API void found_async_message(int from, int to, std::string const& event_text = "");
    EPS_API void found_sync_message(int from, int to, std::string const& event_text = "");
    EPS_API void async_message(int from, int to, std::string const& event_text = "");
    EPS_API void sync_message(int from, int to, std::string const& event_text = "");
    EPS_API void return_message(int from, int to, std::string const& event_text = "");
    EPS_API void create(int from, int to, std::string const& event_text = R"(\tiny\textit{\guillemotleft create\guillemotright})");
    EPS_API void destroy(int from, int to, std::string const& event_text = R"(\tiny\textit{\guillemotleft destroy\guillemotright})");
    void time_advance(int step=1) { m_time -= m_time_advance*step; }
    std::vector<lane_t> m_lanes;
    float m_time;
    float m_time_advance;
    float m_context_width;
    float const m_default_lane_space;
private:
    void calculate_from_to(int from, int to, float& x_from, float& x_to);
    void text_above_line(eps::line_t* pline, std::string const& text);

};

}; // namespace uml

#endif // INTF_UML__UML_H