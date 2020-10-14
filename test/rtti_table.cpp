#include "demo_classes/rtti_classes.s11n.h"
#include <me/s11n/rtti_type_coder.h>
namespace me {
namespace s11n {
CoderPickerImpl<::Base> _coder_picker_1;
CoderPickerImpl<DerivedOne> _coder_picker_2;
CoderPickerImpl<DerivedTwo> _coder_picker_3;

std::unordered_map<std::size_t, CoderPicker *> CoderPicker::IdToCoderMap{
    {1, &_coder_picker_1},
    {2, &_coder_picker_2},
    {3, &_coder_picker_3},};
std::unordered_map<std::type_index, std::size_t> CoderPicker::CppIdToIdMap{
    {typeid(Base), 1},
    {typeid(DerivedOne), 2},
    {typeid(DerivedTwo), 3}};

} // namespace s11n
} // namespace me