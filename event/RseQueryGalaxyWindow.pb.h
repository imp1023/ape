// Generated by the protocol buffer compiler.  DO NOT EDIT!

#ifndef PROTOBUF_RseQueryGalaxyWindow_2eproto__INCLUDED
#define PROTOBUF_RseQueryGalaxyWindow_2eproto__INCLUDED

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
void  protobuf_AddDesc_RseQueryGalaxyWindow_2eproto();
void protobuf_AssignDesc_RseQueryGalaxyWindow_2eproto();
void protobuf_ShutdownFile_RseQueryGalaxyWindow_2eproto();

class RseQueryGalaxyWindow;
class GalaxyWindow;
class SpaceStar;

// ===================================================================

class RseQueryGalaxyWindow : public ::google::protobuf::Message {
 public:
  RseQueryGalaxyWindow();
  virtual ~RseQueryGalaxyWindow();
  
  RseQueryGalaxyWindow(const RseQueryGalaxyWindow& from);
  
  inline RseQueryGalaxyWindow& operator=(const RseQueryGalaxyWindow& from) {
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
  static const RseQueryGalaxyWindow& default_instance();
  void Swap(RseQueryGalaxyWindow* other);
  
  // implements Message ----------------------------------------------
  
  RseQueryGalaxyWindow* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RseQueryGalaxyWindow& from);
  void MergeFrom(const RseQueryGalaxyWindow& from);
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
  
  // repeated .GalaxyWindow galaxyWindow = 1;
  inline int galaxywindow_size() const;
  inline void clear_galaxywindow();
  static const int kGalaxyWindowFieldNumber = 1;
  inline const ::google::protobuf::RepeatedPtrField< ::GalaxyWindow >& galaxywindow() const;
  inline ::google::protobuf::RepeatedPtrField< ::GalaxyWindow >* mutable_galaxywindow();
  inline const ::GalaxyWindow& galaxywindow(int index) const;
  inline ::GalaxyWindow* mutable_galaxywindow(int index);
  inline ::GalaxyWindow* add_galaxywindow();
  
  // optional int32 topLeftCoorX = 2;
  inline bool has_topleftcoorx() const;
  inline void clear_topleftcoorx();
  static const int kTopLeftCoorXFieldNumber = 2;
  inline ::google::protobuf::int32 topleftcoorx() const;
  inline void set_topleftcoorx(::google::protobuf::int32 value);
  
  // optional int32 topLeftCoorY = 3;
  inline bool has_topleftcoory() const;
  inline void clear_topleftcoory();
  static const int kTopLeftCoorYFieldNumber = 3;
  inline ::google::protobuf::int32 topleftcoory() const;
  inline void set_topleftcoory(::google::protobuf::int32 value);
  
  // optional int32 bottomRightCoorX = 4;
  inline bool has_bottomrightcoorx() const;
  inline void clear_bottomrightcoorx();
  static const int kBottomRightCoorXFieldNumber = 4;
  inline ::google::protobuf::int32 bottomrightcoorx() const;
  inline void set_bottomrightcoorx(::google::protobuf::int32 value);
  
  // optional int32 bottomRightCoorY = 5;
  inline bool has_bottomrightcoory() const;
  inline void clear_bottomrightcoory();
  static const int kBottomRightCoorYFieldNumber = 5;
  inline ::google::protobuf::int32 bottomrightcoory() const;
  inline void set_bottomrightcoory(::google::protobuf::int32 value);
  
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::RepeatedPtrField< ::GalaxyWindow > galaxywindow_;
  ::google::protobuf::int32 topleftcoorx_;
  ::google::protobuf::int32 topleftcoory_;
  ::google::protobuf::int32 bottomrightcoorx_;
  ::google::protobuf::int32 bottomrightcoory_;
  friend void  protobuf_AddDesc_RseQueryGalaxyWindow_2eproto();
  friend void protobuf_AssignDesc_RseQueryGalaxyWindow_2eproto();
  friend void protobuf_ShutdownFile_RseQueryGalaxyWindow_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];
  
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
  static RseQueryGalaxyWindow* default_instance_;
};
// -------------------------------------------------------------------

class GalaxyWindow : public ::google::protobuf::Message {
 public:
  GalaxyWindow();
  virtual ~GalaxyWindow();
  
  GalaxyWindow(const GalaxyWindow& from);
  
  inline GalaxyWindow& operator=(const GalaxyWindow& from) {
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
  static const GalaxyWindow& default_instance();
  void Swap(GalaxyWindow* other);
  
  // implements Message ----------------------------------------------
  
  GalaxyWindow* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const GalaxyWindow& from);
  void MergeFrom(const GalaxyWindow& from);
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
  
  // repeated .SpaceStar SpaceStar = 1;
  inline int spacestar_size() const;
  inline void clear_spacestar();
  static const int kSpaceStarFieldNumber = 1;
  inline const ::google::protobuf::RepeatedPtrField< ::SpaceStar >& spacestar() const;
  inline ::google::protobuf::RepeatedPtrField< ::SpaceStar >* mutable_spacestar();
  inline const ::SpaceStar& spacestar(int index) const;
  inline ::SpaceStar* mutable_spacestar(int index);
  inline ::SpaceStar* add_spacestar();
  
  // optional int32 planetsFree = 2;
  inline bool has_planetsfree() const;
  inline void clear_planetsfree();
  static const int kPlanetsFreeFieldNumber = 2;
  inline ::google::protobuf::int32 planetsfree() const;
  inline void set_planetsfree(::google::protobuf::int32 value);
  
  // optional int32 planetsOccupied = 3;
  inline bool has_planetsoccupied() const;
  inline void clear_planetsoccupied();
  static const int kPlanetsOccupiedFieldNumber = 3;
  inline ::google::protobuf::int32 planetsoccupied() const;
  inline void set_planetsoccupied(::google::protobuf::int32 value);
  
  // optional string sku = 4;
  inline bool has_sku() const;
  inline void clear_sku();
  static const int kSkuFieldNumber = 4;
  inline const ::std::string& sku() const;
  inline void set_sku(const ::std::string& value);
  inline void set_sku(const char* value);
  inline void set_sku(const char* value, size_t size);
  inline ::std::string* mutable_sku();
  
  // optional int32 starId = 5;
  inline bool has_starid() const;
  inline void clear_starid();
  static const int kStarIdFieldNumber = 5;
  inline ::google::protobuf::int32 starid() const;
  inline void set_starid(::google::protobuf::int32 value);
  
  // optional int32 type = 6;
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 6;
  inline ::google::protobuf::int32 type() const;
  inline void set_type(::google::protobuf::int32 value);
  
  // optional string name = 7;
  inline bool has_name() const;
  inline void clear_name();
  static const int kNameFieldNumber = 7;
  inline const ::std::string& name() const;
  inline void set_name(const ::std::string& value);
  inline void set_name(const char* value);
  inline void set_name(const char* value, size_t size);
  inline ::std::string* mutable_name();
  
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::RepeatedPtrField< ::SpaceStar > spacestar_;
  ::google::protobuf::int32 planetsfree_;
  ::google::protobuf::int32 planetsoccupied_;
  ::std::string* sku_;
  static const ::std::string _default_sku_;
  ::google::protobuf::int32 starid_;
  ::google::protobuf::int32 type_;
  ::std::string* name_;
  static const ::std::string _default_name_;
  friend void  protobuf_AddDesc_RseQueryGalaxyWindow_2eproto();
  friend void protobuf_AssignDesc_RseQueryGalaxyWindow_2eproto();
  friend void protobuf_ShutdownFile_RseQueryGalaxyWindow_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(7 + 31) / 32];
  
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
  static GalaxyWindow* default_instance_;
};
// -------------------------------------------------------------------

class SpaceStar : public ::google::protobuf::Message {
 public:
  SpaceStar();
  virtual ~SpaceStar();
  
  SpaceStar(const SpaceStar& from);
  
  inline SpaceStar& operator=(const SpaceStar& from) {
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
  static const SpaceStar& default_instance();
  void Swap(SpaceStar* other);
  
  // implements Message ----------------------------------------------
  
  SpaceStar* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const SpaceStar& from);
  void MergeFrom(const SpaceStar& from);
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
  friend void  protobuf_AddDesc_RseQueryGalaxyWindow_2eproto();
  friend void protobuf_AssignDesc_RseQueryGalaxyWindow_2eproto();
  friend void protobuf_ShutdownFile_RseQueryGalaxyWindow_2eproto();
  
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
  static SpaceStar* default_instance_;
};
// ===================================================================


// ===================================================================


// ===================================================================

// RseQueryGalaxyWindow

// repeated .GalaxyWindow galaxyWindow = 1;
inline int RseQueryGalaxyWindow::galaxywindow_size() const {
  return galaxywindow_.size();
}
inline void RseQueryGalaxyWindow::clear_galaxywindow() {
  galaxywindow_.Clear();
}
inline const ::google::protobuf::RepeatedPtrField< ::GalaxyWindow >&
RseQueryGalaxyWindow::galaxywindow() const {
  return galaxywindow_;
}
inline ::google::protobuf::RepeatedPtrField< ::GalaxyWindow >*
RseQueryGalaxyWindow::mutable_galaxywindow() {
  return &galaxywindow_;
}
inline const ::GalaxyWindow& RseQueryGalaxyWindow::galaxywindow(int index) const {
  return galaxywindow_.Get(index);
}
inline ::GalaxyWindow* RseQueryGalaxyWindow::mutable_galaxywindow(int index) {
  return galaxywindow_.Mutable(index);
}
inline ::GalaxyWindow* RseQueryGalaxyWindow::add_galaxywindow() {
  return galaxywindow_.Add();
}

// optional int32 topLeftCoorX = 2;
inline bool RseQueryGalaxyWindow::has_topleftcoorx() const {
  return _has_bit(1);
}
inline void RseQueryGalaxyWindow::clear_topleftcoorx() {
  topleftcoorx_ = 0;
  _clear_bit(1);
}
inline ::google::protobuf::int32 RseQueryGalaxyWindow::topleftcoorx() const {
  return topleftcoorx_;
}
inline void RseQueryGalaxyWindow::set_topleftcoorx(::google::protobuf::int32 value) {
  _set_bit(1);
  topleftcoorx_ = value;
}

// optional int32 topLeftCoorY = 3;
inline bool RseQueryGalaxyWindow::has_topleftcoory() const {
  return _has_bit(2);
}
inline void RseQueryGalaxyWindow::clear_topleftcoory() {
  topleftcoory_ = 0;
  _clear_bit(2);
}
inline ::google::protobuf::int32 RseQueryGalaxyWindow::topleftcoory() const {
  return topleftcoory_;
}
inline void RseQueryGalaxyWindow::set_topleftcoory(::google::protobuf::int32 value) {
  _set_bit(2);
  topleftcoory_ = value;
}

// optional int32 bottomRightCoorX = 4;
inline bool RseQueryGalaxyWindow::has_bottomrightcoorx() const {
  return _has_bit(3);
}
inline void RseQueryGalaxyWindow::clear_bottomrightcoorx() {
  bottomrightcoorx_ = 0;
  _clear_bit(3);
}
inline ::google::protobuf::int32 RseQueryGalaxyWindow::bottomrightcoorx() const {
  return bottomrightcoorx_;
}
inline void RseQueryGalaxyWindow::set_bottomrightcoorx(::google::protobuf::int32 value) {
  _set_bit(3);
  bottomrightcoorx_ = value;
}

// optional int32 bottomRightCoorY = 5;
inline bool RseQueryGalaxyWindow::has_bottomrightcoory() const {
  return _has_bit(4);
}
inline void RseQueryGalaxyWindow::clear_bottomrightcoory() {
  bottomrightcoory_ = 0;
  _clear_bit(4);
}
inline ::google::protobuf::int32 RseQueryGalaxyWindow::bottomrightcoory() const {
  return bottomrightcoory_;
}
inline void RseQueryGalaxyWindow::set_bottomrightcoory(::google::protobuf::int32 value) {
  _set_bit(4);
  bottomrightcoory_ = value;
}

// -------------------------------------------------------------------

// GalaxyWindow

// repeated .SpaceStar SpaceStar = 1;
inline int GalaxyWindow::spacestar_size() const {
  return spacestar_.size();
}
inline void GalaxyWindow::clear_spacestar() {
  spacestar_.Clear();
}
inline const ::google::protobuf::RepeatedPtrField< ::SpaceStar >&
GalaxyWindow::spacestar() const {
  return spacestar_;
}
inline ::google::protobuf::RepeatedPtrField< ::SpaceStar >*
GalaxyWindow::mutable_spacestar() {
  return &spacestar_;
}
inline const ::SpaceStar& GalaxyWindow::spacestar(int index) const {
  return spacestar_.Get(index);
}
inline ::SpaceStar* GalaxyWindow::mutable_spacestar(int index) {
  return spacestar_.Mutable(index);
}
inline ::SpaceStar* GalaxyWindow::add_spacestar() {
  return spacestar_.Add();
}

// optional int32 planetsFree = 2;
inline bool GalaxyWindow::has_planetsfree() const {
  return _has_bit(1);
}
inline void GalaxyWindow::clear_planetsfree() {
  planetsfree_ = 0;
  _clear_bit(1);
}
inline ::google::protobuf::int32 GalaxyWindow::planetsfree() const {
  return planetsfree_;
}
inline void GalaxyWindow::set_planetsfree(::google::protobuf::int32 value) {
  _set_bit(1);
  planetsfree_ = value;
}

// optional int32 planetsOccupied = 3;
inline bool GalaxyWindow::has_planetsoccupied() const {
  return _has_bit(2);
}
inline void GalaxyWindow::clear_planetsoccupied() {
  planetsoccupied_ = 0;
  _clear_bit(2);
}
inline ::google::protobuf::int32 GalaxyWindow::planetsoccupied() const {
  return planetsoccupied_;
}
inline void GalaxyWindow::set_planetsoccupied(::google::protobuf::int32 value) {
  _set_bit(2);
  planetsoccupied_ = value;
}

// optional string sku = 4;
inline bool GalaxyWindow::has_sku() const {
  return _has_bit(3);
}
inline void GalaxyWindow::clear_sku() {
  if (sku_ != &_default_sku_) {
    sku_->clear();
  }
  _clear_bit(3);
}
inline const ::std::string& GalaxyWindow::sku() const {
  return *sku_;
}
inline void GalaxyWindow::set_sku(const ::std::string& value) {
  _set_bit(3);
  if (sku_ == &_default_sku_) {
    sku_ = new ::std::string;
  }
  sku_->assign(value);
}
inline void GalaxyWindow::set_sku(const char* value) {
  _set_bit(3);
  if (sku_ == &_default_sku_) {
    sku_ = new ::std::string;
  }
  sku_->assign(value);
}
inline void GalaxyWindow::set_sku(const char* value, size_t size) {
  _set_bit(3);
  if (sku_ == &_default_sku_) {
    sku_ = new ::std::string;
  }
  sku_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* GalaxyWindow::mutable_sku() {
  _set_bit(3);
  if (sku_ == &_default_sku_) {
    sku_ = new ::std::string;
  }
  return sku_;
}

// optional int32 starId = 5;
inline bool GalaxyWindow::has_starid() const {
  return _has_bit(4);
}
inline void GalaxyWindow::clear_starid() {
  starid_ = 0;
  _clear_bit(4);
}
inline ::google::protobuf::int32 GalaxyWindow::starid() const {
  return starid_;
}
inline void GalaxyWindow::set_starid(::google::protobuf::int32 value) {
  _set_bit(4);
  starid_ = value;
}

// optional int32 type = 6;
inline bool GalaxyWindow::has_type() const {
  return _has_bit(5);
}
inline void GalaxyWindow::clear_type() {
  type_ = 0;
  _clear_bit(5);
}
inline ::google::protobuf::int32 GalaxyWindow::type() const {
  return type_;
}
inline void GalaxyWindow::set_type(::google::protobuf::int32 value) {
  _set_bit(5);
  type_ = value;
}

// optional string name = 7;
inline bool GalaxyWindow::has_name() const {
  return _has_bit(6);
}
inline void GalaxyWindow::clear_name() {
  if (name_ != &_default_name_) {
    name_->clear();
  }
  _clear_bit(6);
}
inline const ::std::string& GalaxyWindow::name() const {
  return *name_;
}
inline void GalaxyWindow::set_name(const ::std::string& value) {
  _set_bit(6);
  if (name_ == &_default_name_) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void GalaxyWindow::set_name(const char* value) {
  _set_bit(6);
  if (name_ == &_default_name_) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void GalaxyWindow::set_name(const char* value, size_t size) {
  _set_bit(6);
  if (name_ == &_default_name_) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* GalaxyWindow::mutable_name() {
  _set_bit(6);
  if (name_ == &_default_name_) {
    name_ = new ::std::string;
  }
  return name_;
}

// -------------------------------------------------------------------

// SpaceStar

// optional int32 ret = 1;
inline bool SpaceStar::has_ret() const {
  return _has_bit(0);
}
inline void SpaceStar::clear_ret() {
  ret_ = 0;
  _clear_bit(0);
}
inline ::google::protobuf::int32 SpaceStar::ret() const {
  return ret_;
}
inline void SpaceStar::set_ret(::google::protobuf::int32 value) {
  _set_bit(0);
  ret_ = value;
}


#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

#endif  // PROTOBUF_RseQueryGalaxyWindow_2eproto__INCLUDED