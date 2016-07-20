// Generated by the protocol buffer compiler.  DO NOT EDIT!

#ifndef PROTOBUF_RceObtainAskForHelp_2eproto__INCLUDED
#define PROTOBUF_RceObtainAskForHelp_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2002000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2002000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_RceObtainAskForHelp_2eproto();
void protobuf_AssignDesc_RceObtainAskForHelp_2eproto();
void protobuf_ShutdownFile_RceObtainAskForHelp_2eproto();

class RceObtainAskForHelp;

// ===================================================================

class RceObtainAskForHelp : public ::google::protobuf::Message {
 public:
  RceObtainAskForHelp();
  virtual ~RceObtainAskForHelp();
  
  RceObtainAskForHelp(const RceObtainAskForHelp& from);
  
  inline RceObtainAskForHelp& operator=(const RceObtainAskForHelp& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const RceObtainAskForHelp& default_instance();
  void Swap(RceObtainAskForHelp* other);
  
  // implements Message ----------------------------------------------
  
  RceObtainAskForHelp* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RceObtainAskForHelp& from);
  void MergeFrom(const RceObtainAskForHelp& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const { _cached_size_ = size; }
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // optional int32 ret = 1;
  inline bool has_ret() const;
  inline void clear_ret();
  static const int kRetFieldNumber = 1;
  inline ::google::protobuf::int32 ret() const;
  inline void set_ret(::google::protobuf::int32 value);
  
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::int32 ret_;
  friend void  protobuf_AddDesc_RceObtainAskForHelp_2eproto();
  friend void protobuf_AssignDesc_RceObtainAskForHelp_2eproto();
  friend void protobuf_ShutdownFile_RceObtainAskForHelp_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static RceObtainAskForHelp* default_instance_;
};
// ===================================================================


// ===================================================================


// ===================================================================

// RceObtainAskForHelp

// optional int32 ret = 1;
inline bool RceObtainAskForHelp::has_ret() const {
  return _has_bit(0);
}
inline void RceObtainAskForHelp::clear_ret() {
  ret_ = 0;
  _clear_bit(0);
}
inline ::google::protobuf::int32 RceObtainAskForHelp::ret() const {
  return ret_;
}
inline void RceObtainAskForHelp::set_ret(::google::protobuf::int32 value) {
  _set_bit(0);
  ret_ = value;
}


#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

#endif  // PROTOBUF_RceObtainAskForHelp_2eproto__INCLUDED