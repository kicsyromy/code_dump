#include <cstdio>
#include <thread>
#include <string>
#include <iostream>

#include "application_loop.hh"
#include "event_loop.hh"
#include "event.hh"
#include "property.hh"
#include "concurrent_queue.hh"
#include "file.hh"
#include "strings.hh"

#include <type_traits>
#include <typeinfo>
#include <memory>
#include <string>
#include <cstdlib>
#include <cxxabi.h>

template <class T>
std::string type_name()
{
    typedef typename std::remove_reference<T>::type TR;
    std::unique_ptr<char, void(*)(void*)> own
           (
                abi::__cxa_demangle(typeid(TR).name(), nullptr,
                                           nullptr, nullptr),
                std::free
           );
    std::string r = own != nullptr ? own.get() : typeid(TR).name();
    if (std::is_const<TR>::value)
        r += " const";
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}

#include "signal.hh"

using namespace rml;

using event_loop_t = events::application_loop<events::event_loop<events::event, utilities::concurrent_queue<events::event>>>;
template<typename ...Args> using signal_t = events::signal<event_loop_t, Args...>;
template<typename value_t> using property_t = events::property<event_loop_t, value_t>;

struct test
{
    test(event_loop_t &event_loop)
      : test_signal{event_loop}
    {}

    signal_t<int> test_signal;

    void testslot(int a)
    {
        std::printf("test slot called %d\n", a);
    }
};

void testfreef(int a)
{
    std::printf("test free function called %d\n", a);
}


void f(const std::string &s)
{
    std::printf("test string function %s\n", s.c_str());
}

template<typename ...Args>
struct vttest
{
    void f(Args ...args)
    {
        ((std::cout << type_name<decltype(args)>() << '\n'), ...);
    }
};

int main(int argc, char *argv[])
{
    // if (file::exists("/home/code/workspace/rml/src/rml/test.rml").first)
    // {
    //     auto file_handle = file::open("/home/code/workspace/rml/src/rml/test.rml", file::open_mode_t::Read).first;

    //     const auto file_content = file_handle.read_all();

    //     std::vector<std::string_view> split_strings;
    //     strings::split(file_content, ':', std::back_inserter(split_strings));

    //     std::copy(split_strings.begin(), split_strings.end(), std::ostream_iterator<std::string_view>(std::cout, "\n"));

    //     std::cout << strings::to<std::int32_t>(split_strings.back()) << '\n';
    // }

    event_loop_t main_loop;

    // std::array<std::thread, 10> producers;
    // for (std::size_t i = 0; i < producers.size(); ++i)
    // {
    //     producers[i] = std::thread([&main_loop](){
    //         for (;;)
    //         {
    //             main_loop.post();
    //             std::this_thread::sleep_for(std::chrono::milliseconds(500));
    //         }
    //     });
    // }

    // main_loop.run();


    // test t(main_loop);
    // t.test_signal.connect(&t, &test::testslot, events::signals::connection_type::Queued);
    // t.test_signal.connect(&t, &testfreef, events::signals::connection_type::Queued);

    // auto t1 = std::thread([&t, &main_loop](){
    //     int i = 0;
    //     for (;;)
    //     {
    //         t.test_signal.emit(i++);
    //         std::this_thread::sleep_for(std::chrono::milliseconds(500));
    //     }
    // });

    // auto t2 = std::thread([&t, &main_loop](){
    //     int i = 100;
    //     for (;;)
    //     {
    //         t.test_signal.emit(i++);
    //         std::this_thread::sleep_for(std::chrono::milliseconds(300));
    //     }
    // });

    // test t(main_loop);

    // property_t<int> test_property{main_loop};
    // test_property.changed.connect(&t, &test::testslot, events::signals::connection_type::Queued);
    // test_property.changed.connect(&t, &testfreef, events::signals::connection_type::Queued);

    // auto t1 = std::thread([&test_property, &main_loop](){
    //     int i = 0;
    //     test_property = 5;
    //     for (;;)
    //     {
    //         test_property = i++;
    //         std::this_thread::sleep_for(std::chrono::milliseconds(500));
    //     }
    // });

    // auto t2 = std::thread([&test_property, &main_loop](){
    //     int i = 100;
    //     for (;;)
    //     {
    //         test_property = i++;
    //         std::this_thread::sleep_for(std::chrono::milliseconds(300));
    //     }
    // });

    property_t<const std::string &> prop{main_loop};
    prop.changed.connect((void *)nullptr, &f, events::signals::connection_type::Queued);

    std::string a_string = "bla";
    prop = a_string;

    main_loop.run();

    // vttest<std::string, std::string &, const std::string &> t;
    // std::string asd;
    // t.f("a", asd, "c");


    return 0;
}