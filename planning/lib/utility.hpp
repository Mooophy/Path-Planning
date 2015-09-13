#pragma once

#include <iostream>
#include <string>

template<typename Item>
std::ostream& print(Item const& item)
{
    return std::cout << item << std::endl;
}

template<typename Item, typename... Args>
std::ostream& print(Item const& item, Args const&... rest)
{
    print(item);
    return print(rest...);
}