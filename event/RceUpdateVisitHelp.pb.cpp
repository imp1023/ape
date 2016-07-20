// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "RceUpdateVisitHelp.pb.h"
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>

namespace {

const ::google::protobuf::Descriptor* RceUpdateVisitHelp_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  RceUpdateVisitHelp_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_RceUpdateVisitHelp_2eproto() {
  protobuf_AddDesc_RceUpdateVisitHelp_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "RceUpdateVisitHelp.proto");
  GOOGLE_CHECK(file != NULL);
  RceUpdateVisitHelp_descriptor_ = file->message_type(0);
  static const int RceUpdateVisitHelp_offsets_[3] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RceUpdateVisitHelp, planetid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RceUpdateVisitHelp, action_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RceUpdateVisitHelp, coins_),
  };
  RceUpdateVisitHelp_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      RceUpdateVisitHelp_descriptor_,
      RceUpdateVisitHelp::default_instance_,
      RceUpdateVisitHelp_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RceUpdateVisitHelp, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RceUpdateVisitHelp, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(RceUpdateVisitHelp));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_RceUpdateVisitHelp_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    RceUpdateVisitHelp_descriptor_, &RceUpdateVisitHelp::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_RceUpdateVisitHelp_2eproto() {
  delete RceUpdateVisitHelp::default_instance_;
  delete RceUpdateVisitHelp_reflection_;
}

void protobuf_AddDesc_RceUpdateVisitHelp_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\030RceUpdateVisitHelp.proto\"E\n\022RceUpdateV"
    "isitHelp\022\020\n\010planetId\030\001 \001(\t\022\016\n\006action\030\002 \001"
    "(\t\022\r\n\005coins\030\003 \001(\005", 97);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "RceUpdateVisitHelp.proto", &protobuf_RegisterTypes);
  RceUpdateVisitHelp::default_instance_ = new RceUpdateVisitHelp();
  RceUpdateVisitHelp::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_RceUpdateVisitHelp_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_RceUpdateVisitHelp_2eproto {
  StaticDescriptorInitializer_RceUpdateVisitHelp_2eproto() {
    protobuf_AddDesc_RceUpdateVisitHelp_2eproto();
  }
} static_descriptor_initializer_RceUpdateVisitHelp_2eproto_;


// ===================================================================

const ::std::string RceUpdateVisitHelp::_default_planetid_;
const ::std::string RceUpdateVisitHelp::_default_action_;
#ifndef _MSC_VER
const int RceUpdateVisitHelp::kPlanetIdFieldNumber;
const int RceUpdateVisitHelp::kActionFieldNumber;
const int RceUpdateVisitHelp::kCoinsFieldNumber;
#endif  // !_MSC_VER

RceUpdateVisitHelp::RceUpdateVisitHelp() {
  SharedCtor();
}

void RceUpdateVisitHelp::InitAsDefaultInstance() {
}

RceUpdateVisitHelp::RceUpdateVisitHelp(const RceUpdateVisitHelp& from) {
  SharedCtor();
  MergeFrom(from);
}

void RceUpdateVisitHelp::SharedCtor() {
  _cached_size_ = 0;
  planetid_ = const_cast< ::std::string*>(&_default_planetid_);
  action_ = const_cast< ::std::string*>(&_default_action_);
  coins_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

RceUpdateVisitHelp::~RceUpdateVisitHelp() {
  SharedDtor();
}

void RceUpdateVisitHelp::SharedDtor() {
  if (planetid_ != &_default_planetid_) {
    delete planetid_;
  }
  if (action_ != &_default_action_) {
    delete action_;
  }
  if (this != default_instance_) {
  }
}

const ::google::protobuf::Descriptor* RceUpdateVisitHelp::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return RceUpdateVisitHelp_descriptor_;
}

const RceUpdateVisitHelp& RceUpdateVisitHelp::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_RceUpdateVisitHelp_2eproto();  return *default_instance_;
}

RceUpdateVisitHelp* RceUpdateVisitHelp::default_instance_ = NULL;

RceUpdateVisitHelp* RceUpdateVisitHelp::New() const {
  return new RceUpdateVisitHelp;
}

void RceUpdateVisitHelp::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (_has_bit(0)) {
      if (planetid_ != &_default_planetid_) {
        planetid_->clear();
      }
    }
    if (_has_bit(1)) {
      if (action_ != &_default_action_) {
        action_->clear();
      }
    }
    coins_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool RceUpdateVisitHelp::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional string planetId = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
        DO_(::google::protobuf::internal::WireFormatLite::ReadString(
              input, this->mutable_planetid()));
        ::google::protobuf::internal::WireFormat::VerifyUTF8String(
          this->planetid().data(), this->planetid().length(),
          ::google::protobuf::internal::WireFormat::PARSE);
        if (input->ExpectTag(18)) goto parse_action;
        break;
      }
      
      // optional string action = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
       parse_action:
        DO_(::google::protobuf::internal::WireFormatLite::ReadString(
              input, this->mutable_action()));
        ::google::protobuf::internal::WireFormat::VerifyUTF8String(
          this->action().data(), this->action().length(),
          ::google::protobuf::internal::WireFormat::PARSE);
        if (input->ExpectTag(24)) goto parse_coins;
        break;
      }
      
      // optional int32 coins = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          goto handle_uninterpreted;
        }
       parse_coins:
        DO_(::google::protobuf::internal::WireFormatLite::ReadInt32(
              input, &coins_));
        _set_bit(2);
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

void RceUpdateVisitHelp::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  ::google::protobuf::uint8* raw_buffer = output->GetDirectBufferForNBytesAndAdvance(_cached_size_);
  if (raw_buffer != NULL) {
    RceUpdateVisitHelp::SerializeWithCachedSizesToArray(raw_buffer);
    return;
  }
  
  // optional string planetId = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->planetid().data(), this->planetid().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->planetid(), output);
  }
  
  // optional string action = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->action().data(), this->action().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->action(), output);
  }
  
  // optional int32 coins = 3;
  if (_has_bit(2)) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(3, this->coins(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* RceUpdateVisitHelp::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional string planetId = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->planetid().data(), this->planetid().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->planetid(), target);
  }
  
  // optional string action = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->action().data(), this->action().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->action(), target);
  }
  
  // optional int32 coins = 3;
  if (_has_bit(2)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(3, this->coins(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int RceUpdateVisitHelp::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional string planetId = 1;
    if (has_planetid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->planetid());
    }
    
    // optional string action = 2;
    if (has_action()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->action());
    }
    
    // optional int32 coins = 3;
    if (has_coins()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->coins());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  _cached_size_ = total_size;
  return total_size;
}

void RceUpdateVisitHelp::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const RceUpdateVisitHelp* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const RceUpdateVisitHelp*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void RceUpdateVisitHelp::MergeFrom(const RceUpdateVisitHelp& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_planetid(from.planetid());
    }
    if (from._has_bit(1)) {
      set_action(from.action());
    }
    if (from._has_bit(2)) {
      set_coins(from.coins());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void RceUpdateVisitHelp::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void RceUpdateVisitHelp::CopyFrom(const RceUpdateVisitHelp& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RceUpdateVisitHelp::IsInitialized() const {
  
  return true;
}

void RceUpdateVisitHelp::Swap(RceUpdateVisitHelp* other) {
  if (other != this) {
    std::swap(planetid_, other->planetid_);
    std::swap(action_, other->action_);
    std::swap(coins_, other->coins_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata RceUpdateVisitHelp::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = RceUpdateVisitHelp_descriptor_;
  metadata.reflection = RceUpdateVisitHelp_reflection_;
  return metadata;
}
