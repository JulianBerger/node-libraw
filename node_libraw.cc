#include <stdio.h>
#include <iostream>
#include <fstream>
#include <v8.h>
#include <nan.h>
//#include <libraw/libraw.h>

#include "libraw/libraw.h"

namespace node_libraw {
  using v8::Exception;
  using v8::Function;
  using v8::FunctionCallbackInfo;
  using v8::FunctionTemplate;
  using v8::Isolate;
  using v8::Local;
  using v8::Number;
  using v8::Object;
  using v8::String;
  using v8::Value;
  using v8::Null;

  void free_callback(char* data, void* thumb) {
    LibRaw::dcraw_clear_mem(reinterpret_cast<libraw_processed_image_t *> (thumb));
  }

  NAN_METHOD(Extract) {
    Nan::HandleScope scope;

    LibRaw RawProcessor;

    v8::Isolate* isolate = info.GetIsolate();
    v8::String::Utf8Value filenameFromArgs(isolate, info[0]);
    std::string filename = std::string(*filenameFromArgs);
    v8::String::Utf8Value outputFromArgs(isolate, info[1]);
    std::string output = std::string(*outputFromArgs);

    Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[2]));

    std::ifstream file;
    file.open(filename.c_str(), std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);

    if (file.read(buffer.data(), size)) {
      RawProcessor.open_buffer(buffer.data(), size);
      RawProcessor.unpack();
      RawProcessor.imgdata.params.output_tiff = 1;
      RawProcessor.imgdata.params.output_bps = 16;
      RawProcessor.imgdata.params.use_camera_wb = 1;
      RawProcessor.dcraw_process();

      output = output + ".tiff";
      RawProcessor.dcraw_ppm_tiff_writer(output.c_str());
      RawProcessor.recycle();

      //info.GetReturnValue().Set(Nan::New(output).ToLocalChecked());

      Local<v8::Value> argv[2] = {
        Nan::Null(),
        Nan::New(output).ToLocalChecked()
      };

      callback->Call(2, argv);
    }

    file.close();
  }

  NAN_METHOD(ExtractBuffer) {
    Nan::HandleScope scope;

    LibRaw RawProcessor;

    v8::Isolate* isolate = info.GetIsolate();
    v8::String::Utf8Value filenameFromArgs(isolate, info[0]);
    std::string filename = std::string(*filenameFromArgs);
    
    Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[1]));

    std::ifstream file;
    file.open(filename.c_str(), std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);

    if (file.read(buffer.data(), size)) {
      RawProcessor.open_buffer(buffer.data(), size);
      RawProcessor.unpack();
      RawProcessor.imgdata.params.output_tiff = 1;
      RawProcessor.imgdata.params.output_bps = 16;
      RawProcessor.imgdata.params.use_camera_wb = 1;
      RawProcessor.dcraw_process();

      int errorCode = 0;
      libraw_processed_image_t *image = RawProcessor.dcraw_make_mem_image(&errorCode);
      
      if (errorCode != LIBRAW_SUCCESS) {
        Local<v8::Value> argv[2] = {
          Nan::New("Error processing image").ToLocalChecked(),
          Nan::Null()
        };
        callback->Call(2, argv);
      } else {
        Local<v8::Value> argv[2] = {
          Nan::Null(),
          Nan::NewBuffer((char*)(image->data), image->data_size, free_callback, image).ToLocalChecked()
        };
        callback->Call(2, argv);
      }
      RawProcessor.recycle();
    }

    file.close();
  }


  NAN_METHOD(ExtractThumb) {
    Nan::HandleScope scope;

    LibRaw RawProcessor;

    v8::Isolate* isolate = info.GetIsolate();
    v8::String::Utf8Value filenameFromArgs(isolate, info[0]);
    std::string filename = std::string(*filenameFromArgs);
    v8::String::Utf8Value outputFromArgs(isolate, info[1]);
    std::string output = std::string(*outputFromArgs);

    Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[2]));

    std::string extension = "thumb.ppm";

    RawProcessor.open_file(filename.c_str());
    RawProcessor.unpack_thumb();

    if (RawProcessor.imgdata.thumbnail.tformat == LIBRAW_THUMBNAIL_JPEG) {
      extension = "thumb.jpg";
    }

    output = output + "." + extension;
    RawProcessor.dcraw_thumb_writer(output.c_str());
    RawProcessor.recycle();

    Local<v8::Value> argv[2] = {
      Nan::Null(),
      Nan::New(output).ToLocalChecked()
    };

    callback->Call(2, argv);
  }

  NAN_METHOD(ExtractThumbBuffer) {
    Nan::HandleScope scope;

    LibRaw RawProcessor;

    v8::Isolate* isolate = info.GetIsolate();
    v8::String::Utf8Value filenameFromArgs(isolate, info[0]);
    std::string filename = std::string(*filenameFromArgs);
    
    Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[1]));

    std::string extension = "thumb.ppm";

    RawProcessor.open_file(filename.c_str());
    RawProcessor.unpack_thumb();
  
    int errorCode = 0;
    libraw_processed_image_t *thumb = RawProcessor.dcraw_make_mem_thumb(&errorCode);
    
    if (errorCode != LIBRAW_SUCCESS) {
      Local<v8::Value> argv[2] = {
        Nan::New("Error processing image").ToLocalChecked(),
        Nan::Null()
      };
      callback->Call(2, argv);
    } else {
      Local<v8::Value> argv[2] = {
        Nan::Null(),
        Nan::NewBuffer((char*)(thumb->data), thumb->data_size, free_callback, thumb).ToLocalChecked()
      };
      callback->Call(2, argv);
    }
    
    RawProcessor.recycle();
  }

  void exif_callback(void *context, int tag, int type, int len, unsigned int ord, void *ifp) {

  }

  NAN_METHOD(GetExif) {
    Nan::HandleScope scope;

    LibRaw RawProcessor;

    v8::Isolate* isolate = info.GetIsolate();
    v8::String::Utf8Value filenameFromArgs(isolate, info[0]);
    std::string filename = std::string(*filenameFromArgs);

    Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[1]));
    
    RawProcessor.open_file(filename.c_str());
    //RawProcessor.set_exifparser_handler(exif_callback, output)

    RawProcessor.recycle();
  }


  void init(Local<Object> exports) {
    Nan::Set(
      exports,
      Nan::New<String>("extractThumb").ToLocalChecked(),
      Nan::GetFunction(Nan::New<v8::FunctionTemplate>(ExtractThumb)).ToLocalChecked()
    );

    Nan::Set(
      exports,
      Nan::New<String>("extractThumbBuffer").ToLocalChecked(),
      Nan::GetFunction(Nan::New<v8::FunctionTemplate>(ExtractThumbBuffer)).ToLocalChecked()
    );

    Nan::Set(
      exports,
      Nan::New<String>("extract").ToLocalChecked(),
      Nan::GetFunction(Nan::New<v8::FunctionTemplate>(Extract)).ToLocalChecked()
    );

    Nan::Set(
      exports,
      Nan::New<String>("extractBuffer").ToLocalChecked(),
      Nan::GetFunction(Nan::New<v8::FunctionTemplate>(ExtractBuffer)).ToLocalChecked()
    );

    Nan::Set(
      exports,
      Nan::New<String>("getExif").ToLocalChecked(),
      Nan::GetFunction(Nan::New<v8::FunctionTemplate>(GetExif)).ToLocalChecked()
    );
  }

  NAN_MODULE_WORKER_ENABLED(libraw, init)
}
