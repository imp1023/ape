// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "RceObtainBattleReplay.pb.h"
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>

namespace {

const ::google::protobuf::Descriptor* RceObtainBattleReplay_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  RceObtainBattleReplay_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_RceObtainBattleReplay_2eproto() {
  protobuf_AddDesc_RceObtainBattleReplay_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "RceObtainBattleReplay.proto");
  GOOGLE_CHECK(file != NULL);
  RceObtainBattleReplay_descriptor_ = file->message_type(0);
  static const int RceObtainBattleReplay_offsets_[1] = {
  };
  RceObtainBattleReplay_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      RceObtainBattleReplay_descriptor_,
      RceObtainBattleReplay::default_instance_,
      RceObtainBattleReplay_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RceObtainBattleReplay, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RceObtainBattleReplay, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(RceObtainBattleReplay));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_RceObtainBattleReplay_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    RceObtainBattleReplay_descriptor_, &RceObtainBattleReplay::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_RceObtainBattleReplay_2eproto() {
  delete RceObtainBattleReplay::default_instance_;
  delete RceObtainBattleReplay_reflection_;
}

void protobuf_AddDesc_RceObtainBattleReplay_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\033RceObtainBattleReplay.proto\"\027\n\025RceObta"
    "inBattleReplay", 54);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "RceObtainBattleReplay.proto", &protobuf_RegisterTypes);
  RceObtainBattleReplay::default_instance_ = new RceObtainBattleReplay();
  RceObtainBattleReplay::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_RceObtainBattleReplay_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_RceObtainBattleReplay_2eproto {
  StaticDescriptorInitializer_RceObtainBattleReplay_2eproto() {
    protobuf_AddDesc_RceObtainBattleReplay_2eproto();
  }
} static_descriptor_initializer_RceObtainBattleReplay_2eproto_;


// ===================================================================

#ifndef _MSC_VER
#endif  // !_MSC_VER

RceObtainBattleReplay::RceObtainBattleReplay() {
  SharedCtor();
}

void RceObtainBattleReplay::InitAsDefaultInstance() {
}

RceObtainBattleReplay::RceObtainBattleReplay(const RceObtainBattleReplay& from) {
  SharedCtor();
  MergeFrom(from);
}

void RceObtainBattleReplay::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

RceObtainBattleReplay::~RceObtainBattleReplay() {
  SharedDtor();
}

void RceObtainBattleReplay::SharedDtor() {
  if (this != default_instance_) {
  }
}

const ::google::protobuf::Descriptor* RceObtainBattleReplay::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return RceObtainBattleReplay_descriptor_;
}

const RceObtainBattleReplay& RceObtainBattleReplay::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_RceObtainBattleReplay_2eproto();  return *default_instance_;
}

RceObtainBattleReplay* RceObtainBattleReplay::default_instance_ = NULL;

RceObtainBattleReplay* RceObtainBattleReplay::New() const {
  return new RceObtainBattleReplay;
}

void RceObtainBattleReplay::Clear() {
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool RceObtainBattleReplay::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
        ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
      return true;
    }
    DO_(::google::protobuf::internal::WireFormat::SkipField(
          input, tag, mutable_unknown_fields()));
  }
  return true;
#undef DO_
}

void RceObtainBattleReplay::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  ::google::protobuf::uint8* raw_buffer = output->GetDirectBufferForNBytesAndAdvance(_cached_size_);
  if (raw_buffer != NULL) {
    RceObtainBattleReplay::SerializeWithCachedSizesToArray(raw_buffer);
    return;
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* RceObtainBattleReplay::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int RceObtainBattleReplay::ByteSize() const {
  int total_size = 0;
  
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  _cached_size_ = total_size;
  return total_size;
}

void RceObtainBattleReplay::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const RceObtainBattleReplay* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const RceObtainBattleReplay*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void RceObtainBattleReplay::MergeFrom(const RceObtainBattleReplay& from) {
  GOOGLE_CHECK_NE(&from, this);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void RceObtainBattleReplay::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void RceObtainBattleReplay::CopyFrom(const RceObtainBattleReplay& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RceObtainBattleReplay::IsInitialized() const {
  
  return true;
}

void RceObtainBattleReplay::Swap(RceObtainBattleReplay* other) {
  if (other != this) {
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata RceObtainBattleReplay::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = RceObtainBattleReplay_descriptor_;
  metadata.reflection = RceObtainBattleReplay_reflection_;
  return metadata;
}
