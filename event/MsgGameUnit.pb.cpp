// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "MsgGameUnit.pb.h"
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>

namespace {

const ::google::protobuf::Descriptor* MsgGameUnit_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  MsgGameUnit_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_MsgGameUnit_2eproto() {
  protobuf_AddDesc_MsgGameUnit_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "MsgGameUnit.proto");
  GOOGLE_CHECK(file != NULL);
  MsgGameUnit_descriptor_ = file->message_type(0);
  static const int MsgGameUnit_offsets_[6] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MsgGameUnit, updatedat_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MsgGameUnit, unlocked_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MsgGameUnit, timeleft_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MsgGameUnit, gameunit_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MsgGameUnit, sku_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MsgGameUnit, upgradeid_),
  };
  MsgGameUnit_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      MsgGameUnit_descriptor_,
      MsgGameUnit::default_instance_,
      MsgGameUnit_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MsgGameUnit, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MsgGameUnit, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(MsgGameUnit));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_MsgGameUnit_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    MsgGameUnit_descriptor_, &MsgGameUnit::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_MsgGameUnit_2eproto() {
  delete MsgGameUnit::default_instance_;
  delete MsgGameUnit_reflection_;
}

void protobuf_AddDesc_MsgGameUnit_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\021MsgGameUnit.proto\"v\n\013MsgGameUnit\022\021\n\tup"
    "datedAt\030\001 \001(\005\022\020\n\010unlocked\030\002 \001(\005\022\020\n\010timeL"
    "eft\030\003 \001(\005\022\020\n\010GameUnit\030\004 \003(\005\022\013\n\003sku\030\005 \001(\t"
    "\022\021\n\tupgradeId\030\006 \001(\005", 139);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "MsgGameUnit.proto", &protobuf_RegisterTypes);
  MsgGameUnit::default_instance_ = new MsgGameUnit();
  MsgGameUnit::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_MsgGameUnit_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_MsgGameUnit_2eproto {
  StaticDescriptorInitializer_MsgGameUnit_2eproto() {
    protobuf_AddDesc_MsgGameUnit_2eproto();
  }
} static_descriptor_initializer_MsgGameUnit_2eproto_;


// ===================================================================

const ::std::string MsgGameUnit::_default_sku_;
#ifndef _MSC_VER
const int MsgGameUnit::kUpdatedAtFieldNumber;
const int MsgGameUnit::kUnlockedFieldNumber;
const int MsgGameUnit::kTimeLeftFieldNumber;
const int MsgGameUnit::kGameUnitFieldNumber;
const int MsgGameUnit::kSkuFieldNumber;
const int MsgGameUnit::kUpgradeIdFieldNumber;
#endif  // !_MSC_VER

MsgGameUnit::MsgGameUnit() {
  SharedCtor();
}

void MsgGameUnit::InitAsDefaultInstance() {
}

MsgGameUnit::MsgGameUnit(const MsgGameUnit& from) {
  SharedCtor();
  MergeFrom(from);
}

void MsgGameUnit::SharedCtor() {
  _cached_size_ = 0;
  updatedat_ = 0;
  unlocked_ = 0;
  timeleft_ = 0;
  sku_ = const_cast< ::std::string*>(&_default_sku_);
  upgradeid_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

MsgGameUnit::~MsgGameUnit() {
  SharedDtor();
}

void MsgGameUnit::SharedDtor() {
  if (sku_ != &_default_sku_) {
    delete sku_;
  }
  if (this != default_instance_) {
  }
}

const ::google::protobuf::Descriptor* MsgGameUnit::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return MsgGameUnit_descriptor_;
}

const MsgGameUnit& MsgGameUnit::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_MsgGameUnit_2eproto();  return *default_instance_;
}

MsgGameUnit* MsgGameUnit::default_instance_ = NULL;

MsgGameUnit* MsgGameUnit::New() const {
  return new MsgGameUnit;
}

void MsgGameUnit::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    updatedat_ = 0;
    unlocked_ = 0;
    timeleft_ = 0;
    if (_has_bit(4)) {
      if (sku_ != &_default_sku_) {
        sku_->clear();
      }
    }
    upgradeid_ = 0;
  }
  gameunit_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool MsgGameUnit::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional int32 updatedAt = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          goto handle_uninterpreted;
        }
        DO_(::google::protobuf::internal::WireFormatLite::ReadInt32(
              input, &updatedat_));
        _set_bit(0);
        if (input->ExpectTag(16)) goto parse_unlocked;
        break;
      }
      
      // optional int32 unlocked = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          goto handle_uninterpreted;
        }
       parse_unlocked:
        DO_(::google::protobuf::internal::WireFormatLite::ReadInt32(
              input, &unlocked_));
        _set_bit(1);
        if (input->ExpectTag(24)) goto parse_timeLeft;
        break;
      }
      
      // optional int32 timeLeft = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          goto handle_uninterpreted;
        }
       parse_timeLeft:
        DO_(::google::protobuf::internal::WireFormatLite::ReadInt32(
              input, &timeleft_));
        _set_bit(2);
        if (input->ExpectTag(32)) goto parse_GameUnit;
        break;
      }
      
      // repeated int32 GameUnit = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          goto handle_uninterpreted;
        }
       parse_GameUnit:
        ::google::protobuf::int32 value;
        DO_(::google::protobuf::internal::WireFormatLite::ReadInt32(
              input, &value));
        add_gameunit(value);
        if (input->ExpectTag(32)) goto parse_GameUnit;
        if (input->ExpectTag(42)) goto parse_sku;
        break;
      }
      
      // optional string sku = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
       parse_sku:
        DO_(::google::protobuf::internal::WireFormatLite::ReadString(
              input, this->mutable_sku()));
        ::google::protobuf::internal::WireFormat::VerifyUTF8String(
          this->sku().data(), this->sku().length(),
          ::google::protobuf::internal::WireFormat::PARSE);
        if (input->ExpectTag(48)) goto parse_upgradeId;
        break;
      }
      
      // optional int32 upgradeId = 6;
      case 6: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          goto handle_uninterpreted;
        }
       parse_upgradeId:
        DO_(::google::protobuf::internal::WireFormatLite::ReadInt32(
              input, &upgradeid_));
        _set_bit(5);
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void MsgGameUnit::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  ::google::protobuf::uint8* raw_buffer = output->GetDirectBufferForNBytesAndAdvance(_cached_size_);
  if (raw_buffer != NULL) {
    MsgGameUnit::SerializeWithCachedSizesToArray(raw_buffer);
    return;
  }
  
  // optional int32 updatedAt = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->updatedat(), output);
  }
  
  // optional int32 unlocked = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->unlocked(), output);
  }
  
  // optional int32 timeLeft = 3;
  if (_has_bit(2)) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(3, this->timeleft(), output);
  }
  
  // repeated int32 GameUnit = 4;
  for (int i = 0; i < this->gameunit_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      4, this->gameunit(i), output);
  }
  
  // optional string sku = 5;
  if (_has_bit(4)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->sku().data(), this->sku().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      5, this->sku(), output);
  }
  
  // optional int32 upgradeId = 6;
  if (_has_bit(5)) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(6, this->upgradeid(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* MsgGameUnit::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional int32 updatedAt = 1;
  if (_has_bit(0)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->updatedat(), target);
  }
  
  // optional int32 unlocked = 2;
  if (_has_bit(1)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->unlocked(), target);
  }
  
  // optional int32 timeLeft = 3;
  if (_has_bit(2)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(3, this->timeleft(), target);
  }
  
  // repeated int32 GameUnit = 4;
  for (int i = 0; i < this->gameunit_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(4, this->gameunit(i), target);
  }
  
  // optional string sku = 5;
  if (_has_bit(4)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->sku().data(), this->sku().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        5, this->sku(), target);
  }
  
  // optional int32 upgradeId = 6;
  if (_has_bit(5)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(6, this->upgradeid(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int MsgGameUnit::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional int32 updatedAt = 1;
    if (has_updatedat()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->updatedat());
    }
    
    // optional int32 unlocked = 2;
    if (has_unlocked()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->unlocked());
    }
    
    // optional int32 timeLeft = 3;
    if (has_timeleft()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->timeleft());
    }
    
    // optional string sku = 5;
    if (has_sku()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->sku());
    }
    
    // optional int32 upgradeId = 6;
    if (has_upgradeid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->upgradeid());
    }
    
  }
  // repeated int32 GameUnit = 4;
  {
    int data_size = 0;
    for (int i = 0; i < this->gameunit_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->gameunit(i));
    }
    total_size += 1 * this->gameunit_size() + data_size;
  }
  
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  _cached_size_ = total_size;
  return total_size;
}

void MsgGameUnit::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const MsgGameUnit* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const MsgGameUnit*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void MsgGameUnit::MergeFrom(const MsgGameUnit& from) {
  GOOGLE_CHECK_NE(&from, this);
  gameunit_.MergeFrom(from.gameunit_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_updatedat(from.updatedat());
    }
    if (from._has_bit(1)) {
      set_unlocked(from.unlocked());
    }
    if (from._has_bit(2)) {
      set_timeleft(from.timeleft());
    }
    if (from._has_bit(4)) {
      set_sku(from.sku());
    }
    if (from._has_bit(5)) {
      set_upgradeid(from.upgradeid());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void MsgGameUnit::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void MsgGameUnit::CopyFrom(const MsgGameUnit& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool MsgGameUnit::IsInitialized() const {
  
  return true;
}

void MsgGameUnit::Swap(MsgGameUnit* other) {
  if (other != this) {
    std::swap(updatedat_, other->updatedat_);
    std::swap(unlocked_, other->unlocked_);
    std::swap(timeleft_, other->timeleft_);
    gameunit_.Swap(&other->gameunit_);
    std::swap(sku_, other->sku_);
    std::swap(upgradeid_, other->upgradeid_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata MsgGameUnit::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = MsgGameUnit_descriptor_;
  metadata.reflection = MsgGameUnit_reflection_;
  return metadata;
}

