// Generated by the protocol buffer compiler.  DO NOT EDIT!

#ifndef PROTOBUF_MsgHangars_2eproto__INCLUDED
#define PROTOBUF_MsgHangars_2eproto__INCLUDED

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
void  protobuf_AddDesc_MsgHangars_2eproto();
void protobuf_AssignDesc_MsgHangars_2eproto();
void protobuf_ShutdownFile_MsgHangars_2eproto();

class MsgHangarUnit;
class MsgHangars;

// ===================================================================

class MsgHangarUnit : public ::google::protobuf::Message {
 public:
  MsgHangarUnit();
  virtual ~MsgHangarUnit();
  
  MsgHangarUnit(const MsgHangarUnit& from);
  
  inline MsgHangarUnit& operator=(const MsgHangarUnit& from) {
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
  static const MsgHangarUnit& default_instance();
  void Swap(MsgHangarUnit* other);
  
  // implements Message ----------------------------------------------
  
  MsgHangarUnit* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MsgHangarUnit& from);
  void MergeFrom(const MsgHangarUnit& from);
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
  
  // optional string sku = 1;
  inline bool has_sku() const;
  inline void clear_sku();
  static const int kSkuFieldNumber = 1;
  inline const ::std::string& sku() const;
  inline void set_sku(const ::std::string& value);
  inline void set_sku(const char* value);
  inline void set_sku(const char* value, size_t size);
  inline ::std::string* mutable_sku();
  
  // optional int32 num = 2;
  inline bool has_num() const;
  inline void clear_num();
  static const int kNumFieldNumber = 2;
  inline ::google::protobuf::int32 num() const;
  inline void set_num(::google::protobuf::int32 value);
  
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::std::string* sku_;
  static const ::std::string _default_sku_;
  ::google::protobuf::int32 num_;
  friend void  protobuf_AddDesc_MsgHangars_2eproto();
  friend void protobuf_AssignDesc_MsgHangars_2eproto();
  friend void protobuf_ShutdownFile_MsgHangars_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
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
  static MsgHangarUnit* default_instance_;
};
// -------------------------------------------------------------------

class MsgHangars : public ::google::protobuf::Message {
 public:
  MsgHangars();
  virtual ~MsgHangars();
  
  MsgHangars(const MsgHangars& from);
  
  inline MsgHangars& operator=(const MsgHangars& from) {
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
  static const MsgHangars& default_instance();
  void Swap(MsgHangars* other);
  
  // implements Message ----------------------------------------------
  
  MsgHangars* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MsgHangars& from);
  void MergeFrom(const MsgHangars& from);
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
  
  // optional int32 sid = 1;
  inline bool has_sid() const;
  inline void clear_sid();
  static const int kSidFieldNumber = 1;
  inline ::google::protobuf::int32 sid() const;
  inline void set_sid(::google::protobuf::int32 value);
  
  // repeated .MsgHangarUnit Hangar = 2;
  inline int hangar_size() const;
  inline void clear_hangar();
  static const int kHangarFieldNumber = 2;
  inline const ::google::protobuf::RepeatedPtrField< ::MsgHangarUnit >& hangar() const;
  inline ::google::protobuf::RepeatedPtrField< ::MsgHangarUnit >* mutable_hangar();
  inline const ::MsgHangarUnit& hangar(int index) const;
  inline ::MsgHangarUnit* mutable_hangar(int index);
  inline ::MsgHangarUnit* add_hangar();
  
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::int32 sid_;
  ::google::protobuf::RepeatedPtrField< ::MsgHangarUnit > hangar_;
  friend void  protobuf_AddDesc_MsgHangars_2eproto();
  friend void protobuf_AssignDesc_MsgHangars_2eproto();
  friend void protobuf_ShutdownFile_MsgHangars_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
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
  static MsgHangars* default_instance_;
};
// ===================================================================


// ===================================================================


// ===================================================================

// MsgHangarUnit

// optional string sku = 1;
inline bool MsgHangarUnit::has_sku() const {
  return _has_bit(0);
}
inline void MsgHangarUnit::clear_sku() {
  if (sku_ != &_default_sku_) {
    sku_->clear();
  }
  _clear_bit(0);
}
inline const ::std::string& MsgHangarUnit::sku() const {
  return *sku_;
}
inline void MsgHangarUnit::set_sku(const ::std::string& value) {
  _set_bit(0);
  if (sku_ == &_default_sku_) {
    sku_ = new ::std::string;
  }
  sku_->assign(value);
}
inline void MsgHangarUnit::set_sku(const char* value) {
  _set_bit(0);
  if (sku_ == &_default_sku_) {
    sku_ = new ::std::string;
  }
  sku_->assign(value);
}
inline void MsgHangarUnit::set_sku(const char* value, size_t size) {
  _set_bit(0);
  if (sku_ == &_default_sku_) {
    sku_ = new ::std::string;
  }
  sku_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* MsgHangarUnit::mutable_sku() {
  _set_bit(0);
  if (sku_ == &_default_sku_) {
    sku_ = new ::std::string;
  }
  return sku_;
}

// optional int32 num = 2;
inline bool MsgHangarUnit::has_num() const {
  return _has_bit(1);
}
inline void MsgHangarUnit::clear_num() {
  num_ = 0;
  _clear_bit(1);
}
inline ::google::protobuf::int32 MsgHangarUnit::num() const {
  return num_;
}
inline void MsgHangarUnit::set_num(::google::protobuf::int32 value) {
  _set_bit(1);
  num_ = value;
}

// -------------------------------------------------------------------

// MsgHangars

// optional int32 sid = 1;
inline bool MsgHangars::has_sid() const {
  return _has_bit(0);
}
inline void MsgHangars::clear_sid() {
  sid_ = 0;
  _clear_bit(0);
}
inline ::google::protobuf::int32 MsgHangars::sid() const {
  return sid_;
}
inline void MsgHangars::set_sid(::google::protobuf::int32 value) {
  _set_bit(0);
  sid_ = value;
}

// repeated .MsgHangarUnit Hangar = 2;
inline int MsgHangars::hangar_size() const {
  return hangar_.size();
}
inline void MsgHangars::clear_hangar() {
  hangar_.Clear();
}
inline const ::google::protobuf::RepeatedPtrField< ::MsgHangarUnit >&
MsgHangars::hangar() const {
  return hangar_;
}
inline ::google::protobuf::RepeatedPtrField< ::MsgHangarUnit >*
MsgHangars::mutable_hangar() {
  return &hangar_;
}
inline const ::MsgHangarUnit& MsgHangars::hangar(int index) const {
  return hangar_.Get(index);
}
inline ::MsgHangarUnit* MsgHangars::mutable_hangar(int index) {
  return hangar_.Mutable(index);
}
inline ::MsgHangarUnit* MsgHangars::add_hangar() {
  return hangar_.Add();
}


#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

#endif  // PROTOBUF_MsgHangars_2eproto__INCLUDED