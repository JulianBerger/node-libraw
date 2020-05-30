const fs = require('fs');

const raw = require('./build/Release/node_libraw');

const libraw = {
  extract: function(input, output) {
    return new Promise(function(resolve, reject) {
      fs.access(input, fs.R_OK | fs.W_OK, function(err) {
        if (err)
          reject(err);

        else
          raw.extract(input, output, function(err, output) {
            if (err)
              reject(err);
            else
              resolve(output);
          });
      });
    });
  },

  extractThumb: function(input, output) {
    return new Promise(function(resolve, reject) {
      fs.access(input, fs.R_OK | fs.W_OK, function(err) {
        if (err)
          reject(err);

        else
          raw.extractThumb(input, output, function(err, output) {
            if (err)
              reject(err);
            else
              resolve(output);
          });
      });
    });
  },

  extractThumbBuffer: function(input) {
    return new Promise(function(resolve, reject) {
      fs.access(input, fs.R_OK | fs.W_OK, function(err) {
        if (err)
          reject(err);
        else
          raw.extractThumbBuffer(input, function(err, output) {
            if (err)
              reject(err);
            else
              resolve(output);
          });
      });
    });
  },

  extractBuffer: function(input) {
    return new Promise(function(resolve, reject) {
      fs.access(input, fs.R_OK | fs.W_OK, function(err) {
        if (err)
          reject(err);
        else
          raw.extractBuffer(input, function(err, output) {
            if (err)
              reject(err);
            else
              resolve(output);
          });
      });
    });
  },

  getExif: function(input) {
    return new Promise(function(resolve, reject) {
      fs.access(input, fs.R_OK | fs.W_OK, function(err) {
        if (err)
          reject(err);
        else
          raw.getExif(input, function(err, output) {
            if (err)
              reject(err);
            else
              resolve(output);
          });
      });
    });
  }
};

module.exports = libraw;
