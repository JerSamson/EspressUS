#pragma once

#include <memory>
#include <array>
#include <type_traits>
#include <typeindex>
#include <typeinfo>

#include "esp_gatt_defs.h"

namespace GATT {

#define MAX_LEN 20

/* wrapper to get rid of c pointer in original esp struct */
struct attr_desc_wrapper_t
{
    esp_bt_uuid_t uuid;
    esp_gatt_perm_t perm;
    uint16_t max_length;
    uint16_t length;
    std::unique_ptr<uint8_t[]> value;
    std::type_index ti = typeid(void);

    esp_attr_desc_t get(void) const noexcept
    {
        return {
            uuid.len,
            const_cast<uint8_t*>(uuid.uuid.uuid128),
            perm,
            max_length,
            length,
            value.get()
        };
    }
};

template <size_t n_elements>
using gatt_table_t = std::array<attr_desc_wrapper_t, n_elements>

template <size_t n_elements, class... Attrs>
gatt_table_t<n_elements> make_table(Attrs&&... attrs)
{
    return {(std::move(attrs), ...)}
}

/* Strip ptr, arrays or ref to get underlying type */
template <class T>
using strip_ptr_type = std::remove_pointer_t<std::std::remove_reference_t<T>>;

/* strips arrays once, would not work with 2D arrays */
template <class T>
using strip_arr_type = 
    std::remove_pointer_t<std::remove_extent_t<std::std::remove_reference_t<T>>>; 
 


/* takes any type, array or pointer and create a unique smart pointer and converts value into byte array */
template <class T, size_t n_elements = 1>
struct attr_value_t
{
    static constexpr size_t len         = n_elements;
    static constexpr size_t len_type    = sizeof(strip_ptr_type<T>);
    static constexpr size_t len_bytes   = len * len_type;
    static_assert(len_bytes);

    std::unique_ptr<uint8_t[]> val;
    std::type_index ti = typeid(strip_arr_type<T>); /* get the underlying type */

    attr_value_t(const T value) :
        val{new uint8_t[len_bytes]}
    {
        const uint8_t* p = nullptr;

        if constexpr (std::is_pointer_v<T> || std::is_array_v<T>)
            p = reinterpret_cast<const uint8_t*>(value);
        else
            p = reinterpret_cast<const uint8_t*>(&value);
            
        std::array<uint_8_t, len_bytes> arr;
        for (auto& elem : arr) elem = *(p++);

        std::copy(arr.begin(), arr.end(), val.get());
    
    }


    template <typename... Ts>
    gatt_table_t<sizeof...(Ts)> make_read_only_table(Ts&&... values)
    {
        gatt_table_t<sizeof...(Ts)> ret;

        esp_bt_uuid_t uuid = {ESP_UUID_LEN_16, 123}; // TODO

        size_t idx = 0;
        auto fill_arr_one = [&ret, &uuid, &idx](const auto& arg)
        {
          attr_value_t<std::remove_reference_t<arg>> arg_heap;

          ret[idx++] = {    uuid,
                            ESP_GATT_PERM_READ,
                            MAX_LEN,
                            arg_heap.len_bytes,
                            std::move(arg_heap.val),
                            arg_heap.ti
            };  
        };

        (fill_arr_one(values), ...);

        return ret;
    }

    auto my_service_table = make_read_only_table("Hello world", 69, 3.14159, "Hello Sherb");
};

} // namespace GATT