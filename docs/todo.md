# TODO

| Component  | Symbol                            | Work                                                                                                     |
| ---------- | --------------------------------- | -------------------------------------------------------------------------------------------------------- |
| -          | -                                 | add changelog                                                                                            |
| -          | -                                 | add support for msvc                                                                                     |
| -          | -                                 | cleanup cmake                                                                                            |
| -          | -                                 | cleanup run tool                                                                                         |
| -          | -                                 | consider include guards instead of pragmas                                                               |
| -          | -                                 | consider more flexible error handling (than throw/abort)                                                 |
| -          | -                                 | decide how to handle clang-tidy                                                                          |
| -          | -                                 | general documentation                                                                                    |
| -          | -                                 | let workflow check license in each file                                                                  |
| -          | -                                 | reconsider license                                                                                       |
| any        | -                                 | document                                                                                                 |
| any        | -                                 | test                                                                                                     |
| any        | copyable_any                      | implement                                                                                                |
| any        | details::any::cast                | consider only using RTTI when that's available                                                           |
| any        | move_only_any                     | consider allow conversions (copyable_any -> move_only_any)                                               |
| any        | move_only_any                     | consider allow conversions (move_only_any<N> -> move_only_any<M>, if M > N)                              |
| any        | move_only_any                     | consider relaxing nothrow requirements                                                                   |
| any        | move_only_any                     | constexpr support, if possible                                                                           |
| any        | move_only_any::swap               | optimize                                                                                                 |
| any        | no_move_any                       | consider implementing                                                                                    |
| functional | -                                 | document                                                                                                 |
| functional | -                                 | test                                                                                                     |
| functional | copyable_function                 | implement                                                                                                |
| functional | function::swap                    | optimize (like stdlibc++ any::swap)                                                                      |
| functional | move_only_function                | implement                                                                                                |
| hive       | -                                 | everything                                                                                               |
| map        | -                                 | everything                                                                                               |
| set        | -                                 | everything                                                                                               |
| string     | -                                 | check if there is an N where only copying size_ chars is faster than default memcpy of the entire object |
| string     | -                                 | consider adding try_* alternatives just like std::inplace_vector has                                     |
| string     | -                                 | document                                                                                                 |
| string     | -                                 | test                                                                                                     |
| string     | erase                             | implement                                                                                                |
| string     | erase_if                          | implement                                                                                                |
| string     | operator+                         | implement                                                                                                |
| string     | operator<=>                       | implement                                                                                                |
| string     | operator==                        | implement                                                                                                |
| string     | string::append                    | implement                                                                                                |
| string     | string::append_range              | implement                                                                                                |
| string     | string::append_range              | implement                                                                                                |
| string     | string::compare                   | implement                                                                                                |
| string     | string::copy                      | implement                                                                                                |
| string     | string::erase                     | implement                                                                                                |
| string     | string::insert                    | implement                                                                                                |
| string     | string::insert_range              | implement                                                                                                |
| string     | string::operator+=                | implement                                                                                                |
| string     | string::replace                   | implement                                                                                                |
| string     | string::replace_with_range        | implement                                                                                                |
| string     | string::resize                    | implement                                                                                                |
| string     | string::resize_and_overwrite      | implement                                                                                                |
| string     | swap                              | implement                                                                                                |
| vector     | -                                 | document                                                                                                 |
| vector     | -                                 | test                                                                                                     |
| vector     | comparison operators              | implement                                                                                                |
| vector     | deduction guides                  | implement                                                                                                |
| vector     | erase                             | implement                                                                                                |
| vector     | erase_if                          | implement                                                                                                |
| vector     | swap                              | implement                                                                                                |
| vector     | vector::append_range              | implement                                                                                                |
| vector     | vector::assign                    | implement                                                                                                |
| vector     | vector::assign_range              | implement                                                                                                |
| vector     | vector::emplace                   | implement                                                                                                |
| vector     | vector::erase                     | implement                                                                                                |
| vector     | vector::insert                    | implement                                                                                                |
| vector     | vector::insert_range              | implement                                                                                                |
| vector     | vector::resize                    | implement                                                                                                |
| vector     | vector::swap                      | implement                                                                                                |
| vector     | vector::try_append_range          | implement                                                                                                |
| vector     | vector::vector(from_range_t, R&&) | implement                                                                                                |
| vector     | vector<T, 0>                      | implement specialization                                                                                 |
